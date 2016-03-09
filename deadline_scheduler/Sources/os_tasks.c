/* ###################################################################

**     Filename    : os_tasks.c
**     Project     : serial_echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-02-03, 16:03, # CodeGen: 1
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         serial_task - void serial_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file os_tasks.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         
/* MODULE os_tasks */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif 

_pool_id rx_message_pool;
_pool_id tx_message_pool;
_queue_id serial_tx_qid = 0;

#define LINE_BUFFER_SIZE 128
#define USER_TASKS 5
char line_buffer[LINE_BUFFER_SIZE];
unsigned int line_buffer_offset = 0;

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "SerialAPI.h"
#include "dds_structures.h"

void start_serial_task() {
	// create a new serial task
	_task_id new_task;
	new_task = _task_create(0, SERIALTASK_TASK, 0);
}

/*
** ===================================================================
**     Callback    : serial_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void serial_task(os_task_param_t task_init_data)
{
	 //printf("serialTask Created!\n\r");

	SERIAL_CHAR_MSG_PTR tx_msg_ptr;
	if(serial_tx_qid == 0){
		serial_tx_qid = _msgq_open_system(TX_MSG_QUEUE, 0, start_serial_task, NULL); // can change nulls for notification function
	}

	  while(_msgq_get_count(serial_tx_qid)) {
		  tx_msg_ptr = _msgq_receive(serial_tx_qid, 0);
		  if (tx_msg_ptr == NULL) {
			  printf("\n Receiving message failed\n");
			  _task_block();
		  }

		  // Receive new char
		  unsigned char recv_char[2];
		  recv_char[0] = tx_msg_ptr->data;
		  recv_char[1] = '\0';

		  UART_DRV_SendDataBlocking(myUART_IDX, recv_char, 1, 0);

		  // free up the message sent to us, we don't need it anymore
		  _msg_free(tx_msg_ptr);
	  }
	  _task_destroy(MQX_NULL_TASK_ID);


#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

void queue_char(unsigned char recv_char, _queue_id qid) {
	  SERIAL_CHAR_MSG_PTR tx_msg_ptr;
	  bool result;

	  tx_msg_ptr = (SERIAL_CHAR_MSG_PTR)_msg_alloc_system(tx_message_pool);

	  if (tx_msg_ptr == NULL) {
		  printf("\n Could not allocate a tx_msg_ptr\n");
		  _task_block();
	  }

	  tx_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, qid);
	  tx_msg_ptr->HEADER.SIZE = sizeof(SERIAL_CHAR_MSG);
	  tx_msg_ptr->data = recv_char;

	  result = _msgq_send(tx_msg_ptr);

	  if (result != TRUE) {
		  printf("\n Could not send tx message\n");
		  _task_block();
	  }
}

void queue_tx_char(unsigned char recv_char) {
	queue_char(recv_char, TX_MSG_QUEUE);
}

void queue_tx_str(unsigned char* str, uint16_t len) {
	queue_str(str, len, TX_MSG_QUEUE);
}

void queue_str(unsigned char* str, uint16_t len, _queue_id qid) {
	int i;
	for (i = 0; i < len; i++) {
		queue_char(str[i], qid);
	}
}

void line_buffer_add_char (unsigned char toAdd) {
	  // don't overflow and account for null terminator
	  if (line_buffer_offset > LINE_BUFFER_SIZE-2) {
		  printf("WARNING: Overflow");
		  return;
	  }


		// add new char
	  line_buffer[line_buffer_offset] = toAdd;
	  line_buffer_offset++;


}

void line_buffer_cursor_back () {
	// don't underflow!
	if (line_buffer_offset < 1) return;

	line_buffer_offset--;
}

/*
** ===================================================================
**     Callback    : handler_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void handler_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
   _task_block();
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */

	  /* Locals */
	  SERIAL_CHAR_MSG_PTR rx_msg_ptr;
	  SERIAL_CHAR_MSG_PTR tx_msg_ptr;
	  _mqx_uint i;
	  _queue_id serial_rx_qid;
	  _queue_id serial_tx_qid;
	  bool result;
	  _task_id task_id;

	  // Do initialization
	  printf("Handler Task Created\n\r");

	  /* setup a system message queue for outgoing chars for serial tx ISR */

	  SerialAPI_init();
	  result = _msgpool_create_system(sizeof(SERIAL_CHAR_MSG), TX_MSG_QUEUE_SIZE, 0, 0);

	  if (!result) {
		  printf("\n Could not create the tx_message_pool\n");
		  _task_block();
	  }

	  /* open a private message queue for recieving chars from serial rx ISR */
	  serial_rx_qid = _msgq_open(RX_MSG_QUEUE, 0);

	  if (serial_rx_qid == 0) {
		  printf("Could not open the serial rx queue\n");
		  _task_block();
	  }

	  rx_message_pool = _msgpool_create(sizeof(SERIAL_CHAR_MSG), RX_MSG_QUEUE_SIZE, 0, 0);

	  if (rx_message_pool == MSGPOOL_NULL_POOL_ID) {
		  printf("\n Could not create the rx_message_pool\n");
		  _task_block();
	  }

	  // create user tasks
	  int j;
	  for (j = 1; j < USER_TASKS+1; j++) {
		  _task_create(0, USERTASK_TASK, j);
	  }


	  // Main loop
	  while (TRUE) {

		  // get a new char
		  rx_msg_ptr = _msgq_receive(serial_rx_qid, 0);

		  if (rx_msg_ptr == NULL) {
			  printf("\n Receiving message failed\n");
			  _task_block();
		  }


		  // Receive new char
		  unsigned char recv_char;
		  recv_char = rx_msg_ptr->data;



		  // free up the message sent to us, we don't need it anymore
		  _msg_free(rx_msg_ptr);

		  /* handle the recieved character now */

		  if (recv_char == 8) {
			  // backspace or ctrl-h

			  // go back 1 position
			  queue_tx_char(0x1B);
			  queue_tx_char(0x5B);
			  queue_tx_char('D');


			  // print space
			  queue_tx_char(' ');

			  // go back 1 position
			  queue_tx_char(0x1B);
			  queue_tx_char(0x5B);
			  queue_tx_char('D');

			  // our buffer
			  line_buffer_cursor_back();
			  line_buffer_add_char(' ');
			  line_buffer_cursor_back();

		  }


		  if (recv_char == 13) {

			  //Return (enter pressed)

			  // Do stuff for getline
			  int i;
			  for (i = 0; i < _read_tasks_idx; i++) {
				  if (_getline_addr[i] != NULL) {
					  sprintf(_getline_addr[i], "%s", line_buffer);
					  TD_STRUCT_PTR td_ptr = _task_get_td(_read_tasks[i]);
					  _task_ready(td_ptr);
				  }
			  }

			  //debug
			  //printf("%s \n", line_buffer);

			  queue_tx_char(10); // send line feed, ie go to next line

			  // get number of chars to go back
			  char len[4];
			  sprintf(len, "%d", line_buffer_offset);

			  // go back that many on serial
			  // go back "len" spaces
			  queue_tx_char(0x1B);
			  queue_tx_char(0x5B);
			  queue_tx_str(len, strlen(len));
			  queue_tx_char('D');

			  // go back in line buffer
			  memset(line_buffer, 0, LINE_BUFFER_SIZE);
			  line_buffer_offset = 0;
		  }

		  // ctrl-w pressed
		  if (recv_char == 23){

			  unsigned int temp_line_buffer_offset = line_buffer_offset;
			  while(TRUE){
				  if(temp_line_buffer_offset == 0 || line_buffer[temp_line_buffer_offset] == ' '){
					  break;
				  }
				  temp_line_buffer_offset--;
			  }

			  unsigned int chars_to_del = line_buffer_offset - temp_line_buffer_offset;

			  // get number of chars to go back
			  char len[4];
			  memset(len, 0, 4);
			  sprintf(len, "%d", chars_to_del);

			  // go back that many on serial
			  // go back "len" spaces
			  queue_tx_char(0x1B);
			  queue_tx_char(0x5B);
			  queue_tx_str(len, strlen(len));
			  queue_tx_char('D');

			  // print space
			  int i;
			  for(i=0; i < chars_to_del; i++){
				  queue_tx_char(' ');
			  }

			  // go back that many on serial
			  // go back "len" spaces
			  queue_tx_char(0x1B);
			  queue_tx_char(0x5B);
			  queue_tx_str(len, strlen(len));
			  queue_tx_char('D');

			  // go back in line buffer
			  memset(line_buffer+temp_line_buffer_offset, 0, chars_to_del);
			  line_buffer_offset = temp_line_buffer_offset;

		  }

		  // ctrl-l pressed
		  if (recv_char == 12 || recv_char == 21){

			  unsigned int chars_to_del = line_buffer_offset;

			  // get number of chars to go back
			  char len[4];
			  memset(len, 0, 4);
			  sprintf(len, "%d", chars_to_del);

			  // go back that many on serial
			  // go back "len" spaces
			  queue_tx_char(0x1B);
			  queue_tx_char(0x5B);
			  queue_tx_str(len, strlen(len));
			  queue_tx_char('D');

			  // print space
			  int i;
			  for(i=0; i < chars_to_del; i++){
				  queue_tx_char(' ');
			  }

			  // go back that many on serial
			  // go back "len" spaces
			  queue_tx_char(0x1B);
			  queue_tx_char(0x5B);
			  queue_tx_str(len, strlen(len));
			  queue_tx_char('D');


			  // go back in line buffer
			  memset(line_buffer, 0, LINE_BUFFER_SIZE);
			  line_buffer_offset = 0;
		  }

		  // handle printable character
		  if (recv_char <= 127 && recv_char >= 32) { // check for printable character
			  //serial
			  queue_tx_char(recv_char);
			  // our buffer
			  line_buffer_add_char(recv_char);
		  }


		  // send char to queue of each user task
		  int i;
		  for (i = 0; i < _read_tasks_idx; i++) {
			  _queue_id openr_qid = _read_tasks_qid[i];
			  queue_char(recv_char, openr_qid);
		  }

	  }

    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   


    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : UserTask_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void UserTask_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    

	  unsigned int user_task_id = (unsigned int)task_init_data;

	  _queue_id ut_rx_qid = _msgq_open(USER_TASK_QUEUE_START + user_task_id, 0);

	  if (ut_rx_qid == 0) {
		  printf("Could not open the recv queue: %d\n", user_task_id);
		  _task_block();
	  }


	  bool echoer = (user_task_id == 2);
	  _queue_id qid;
	  OpenR(ut_rx_qid);
	  if (echoer) qid = OpenW();
	  char test[128];


	  while (TRUE) {

#if 0 // testing _putline and _getline
		  memset(test, '\0', 128);
		  _getline(&test);
		  printf("User Task %d received: %s\n", user_task_id, test);

		  if (test[0] == '~' && echoer) {
			  char echo[160];
			  sprintf(echo, "User Task %d echos: %s\n", user_task_id, test+1);
			  _putline(qid, echo);
		  }
#endif

#if 1 //testing char stream from OpenR
		  SERIAL_CHAR_MSG_PTR ut_msg_ptr = _msgq_receive(ut_rx_qid, 0);

		  if (ut_msg_ptr == NULL) {
			  printf("\n Receiving message failed\n");
			  _task_block();
		  }


		  // Receive new char
		  unsigned char recv_char;
		  recv_char = ut_msg_ptr->data;
		  printf("User Task %d char stream: %c\n", user_task_id, recv_char);
#endif
	  }

    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : MonitorTask_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void MonitorTask_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

//TIMER FUNCTION HERE
	// => send a message to the dds queue

/*
** ===================================================================
**     Callback    : DdsTask_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void DdsTask_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
  

	//Do forever
		//Block while waiting to recieve a message from the message queue

		// Act accordingly to the type of message
			// Create
			// Delete
			// Return active (information request)
			// Return overdue (information request)


		// Look at all tasks in active pool
		// Select the task with the shortest deadline
		// Set a timer for the deadline
		// Run that task


#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : AuxTask_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void AuxTask_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
  




#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
