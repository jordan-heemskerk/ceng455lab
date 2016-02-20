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

#define LINE_BUFFER_SIZE 128

char line_buffer[LINE_BUFFER_SIZE];
unsigned int line_buffer_offset = 0;

/* User includes (#include below this line is not maintained by Processor Expert) */

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
  /* Write your local variable definition here */
  printf("serialTask Created!\n\r");
  
  //char buf[13];
  //sprintf(buf, "\n\rType here: ");
  //UART_DRV_SendDataBlocking(myUART_IDX, buf, sizeof(buf), 1000);

#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

void queue_tx_char(unsigned char recv_char) {
	  SERIAL_CHAR_MSG_PTR tx_msg_ptr;
	  bool result;

	  tx_msg_ptr = (SERIAL_CHAR_MSG_PTR)_msg_alloc_system(tx_message_pool);

	  if (tx_msg_ptr == NULL) {
		  printf("\n Could not allocate a tx_msg_ptr\n");
		  _task_block();
	  }

	  tx_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, TX_MSG_QUEUE);
	  tx_msg_ptr->HEADER.SIZE = sizeof(SERIAL_CHAR_MSG);
	  tx_msg_ptr->data = recv_char;

	  result = _msgq_send(tx_msg_ptr);

	  if (result != TRUE) {
		  printf("\n Could not send tx message\n");
		  _task_block();
	  }
}

void queue_tx_str(unsigned char* str, uint16_t len) {
	int i;
	for (i = 0; i < len; i++) {
		queue_tx_char(str[i]);
	}
}

void line_buffer_add_char (unsigned char toAdd) {\
	  // don't overflow and account for null terminator
	  if (line_buffer_offset > LINE_BUFFER_SIZE-2) return;

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

	  printf("Handler Task Created\n\r");

	  /* setup a system message queue for outgoing chars for serial tx ISR */
	  serial_tx_qid = _msgq_open_system(TX_MSG_QUEUE, 0, NULL, NULL); // can change nulls for notification function

	  if (serial_tx_qid == 0) {
		  printf("Could not open the serial tx system queue\n");
		  _task_block();
	  }

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

	  while (TRUE) {
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

			  // serial
			  queue_tx_char(0x1B);
			  queue_tx_char(0x5B);
			  queue_tx_char('D');
			  queue_tx_char(' ');
			  queue_tx_char(0x1B);
			  queue_tx_char(0x5B);
			  queue_tx_char('D');

			  // our buffer
			  line_buffer_cursor_back();
			  line_buffer_add_char(' ');
			  line_buffer_cursor_back();

		  }


		  if (recv_char == 13) {

			  //debug: printf("ENTER PRESSED (CR)\n");

			  //HANDLE _getLine stuff here

			  printf("%s \n", line_buffer);

			  queue_tx_char(10); // send line feed, ie go to next line

			  // get number of chars to go back
			  char len[4];
			  sprintf(len, "%d", line_buffer_offset);

			  // go back that many on serial
			  queue_tx_char(0x1B);
			  queue_tx_char(0x5B);
			  queue_tx_str(len, strlen(len));
			  queue_tx_char('D');

			  // go back in line buffer
			  memset(line_buffer, 0, LINE_BUFFER_SIZE);
			  line_buffer_offset = 0;




		  }

		  if (recv_char == 10) {
			  printf("ENTER PRESSED (LF)\n");
		  }



		  if (recv_char <= 127 && recv_char >= 32) { // check for printable character
			  //serial
			  queue_tx_char(recv_char);
			  // our buffer
			  line_buffer_add_char(recv_char);
		  }


		  // echo back necessary characters
		  /* open a message queue for recieving chars from serial rx ISR */

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

	  }

    
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
