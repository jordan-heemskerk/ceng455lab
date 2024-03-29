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
#include <timer.h>

#ifdef __cplusplus
extern "C" {
#endif

_pool_id rx_message_pool;
_pool_id tx_message_pool;
_pool_id dds_message_pool;
_pool_id dds_response_pool;

_queue_id serial_tx_qid = 0;

#define LINE_BUFFER_SIZE 128
#define USER_TASKS 5
char line_buffer[LINE_BUFFER_SIZE];
unsigned int line_buffer_offset = 0;

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "SerialAPI.h"
#include "dds_structures.h"
#include "dds_functions.h"

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

	  tx_msg_ptr = (SERIAL_CHAR_MSG_PTR)_msg_alloc_system(sizeof(SERIAL_CHAR_MSG));

	  if (tx_msg_ptr == NULL) {
		  printf("\n Could not allocate a tx_msg_ptr\n");
		  _task_block();
	  }

	  tx_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, qid);
	  tx_msg_ptr->HEADER.SIZE = sizeof(SERIAL_CHAR_MSG);
	  tx_msg_ptr->data = recv_char;

	  result = _msgq_send(tx_msg_ptr);

	  if (result != TRUE) {
		  printf("\n queue_char: Could not send tx message\n");
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


		 //TODO MAKE ME WORK AGAIN
		  // send char to queue of each user task
		  /*
		  int i;
		  for (i = 0; i < _read_tasks_idx; i++) {
			  _queue_id openr_qid = _read_tasks_qid[i];
			  queue_char(recv_char, openr_qid);
		  }
		  */

	  }


    OSA_TimeDelay(10);                 /* Example code (for task release) */




#ifdef PEX_USE_RTOS
  }
#endif
}

uint32_t ticks_in_user_task = 0;
uint32_t ms_in_dds = 0;
uint32_t start_time = 0;
_timer_id deadline_expiry_timer;
uint32_t task_counter = 0;
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
	//TODO print tid

	uint32_t time_left = (uint32_t)task_init_data;

	_task_id tid = _task_get_id();


	TIME_STRUCT time_ptr;
	time_ptr.SECONDS = time_left/1000;
	time_ptr.MILLISECONDS = time_left - time_ptr.SECONDS*1000;

	MQX_TICK_STRUCT tick_time_ptr;

	_time_to_ticks(&time_ptr, &tick_time_ptr);
	uint32_t ticks_left = tick_time_ptr.TICKS[0] * _time_get_hwticks_per_tick()/15;


	uint32_t my_task = task_counter;
	task_counter++;
	printf("Task %d: Running a task for %d milliseconds\n", my_task, (uint32_t)task_init_data);

	// run stuff
	while (ticks_left > 0) {
		ticks_left--;
		ticks_in_user_task++;
	}

	// user task removes itself after running

	printf("Task %d: Finished\n", my_task);
	_timer_cancel(deadline_expiry_timer);
	dd_delete(tid);


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


	  // Output the processor utilitzation and system overhead

		  TIME_STRUCT current_time;
		  _time_get(&current_time);
		  uint32_t running = (current_time.SECONDS*1000 + current_time.MILLISECONDS) - start_time;

	// time user tasks have been running


			TIME_STRUCT time_ptr;
			MQX_TICK_STRUCT tick_time_ptr;
			memset(&tick_time_ptr, 0, sizeof(tick_time_ptr));
			uint32_t ticks_ran = ticks_in_user_task / (_time_get_hwticks_per_tick()/15);
			tick_time_ptr.TICKS[0] = ticks_ran;
			_ticks_to_time(&tick_time_ptr, &time_ptr);
			uint32_t time_in_ut = time_ptr.SECONDS * 1000 + time_ptr.MILLISECONDS;

			uint32_t utilization = (time_in_ut*100)/(running);
			uint32_t dds_overhead = (ms_in_dds*10000)/(running);

			uint16_t infront = dds_overhead/100;
			uint16_t behind = dds_overhead%100;
			printf("Monitor Info: Utilization=%d%% Overhead=%d.%02d%% \n", utilization, infront, behind);

	// how long the system been running

    OSA_TimeDelay(5000);                 /* Example code (for task release) */




#ifdef PEX_USE_RTOS
  }
#endif
}




//TIMER FUNCTION HERE
	// => send a message to the dds queue
#define DEADLINE_QUEUE 14
void deadline_expiry(_timer_id id, void* data_ptr, MQX_TICK_STRUCT_PTR tick_ptr) {


	printf("DEBUG: DEADLINE REACHED\n");

	 DDS_TASK_MSG_PTR dds_msg_ptr;

	  bool result;


	  dds_msg_ptr = (DDS_TASK_MSG_PTR)_msg_alloc(dds_message_pool);

	  if (dds_msg_ptr == NULL) {
		  printf("\n dd_deadline: Could not allocate a dds_msg_ptr\n");
		  _task_block();
	  }




	  // allocate and set a strcut for create_command_data_t
	  deadline_command_data_t* deadline_data = _mem_alloc_system(sizeof(deadline_command_data_t));
	  deadline_data->tid = *(_task_id*)data_ptr;

	  dds_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, DDS_MSG_QUEUE);
	  dds_msg_ptr->HEADER.SIZE = sizeof(DDS_TASK_MSG);
	  dds_msg_ptr->data = deadline_data;
	  dds_msg_ptr->dds_command = DDS_DEADLINE;

	  result = _msgq_send(dds_msg_ptr);

	  if (result != TRUE) {
		  printf("\n dd_deadline: Could not send dds message\n");
		  _task_block();
	  }
	  return;

}

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



	printf("DDS task started\n");
	DDS_TASK_MSG_PTR dds_msg_ptr;

	//Create message queue for incoming messages from AuxTask_task
	_queue_id dds_task_qid;
	dds_task_qid = _msgq_open(DDS_MSG_QUEUE, 0);

	if (dds_task_qid == 0) {
		printf("Could not open the dds task queue\n");
		_task_block();
	}

	/*
	if (dds_message_pool == MSGPOOL_NULL_POOL_ID) {
		printf("\n Could not create the dds_message_pool\n");
		_task_block();
	}*/

	TD_STRUCT_PTR td_ptr = _task_get_td(_task_get_id_from_name(AUXTASK_TASK_NAME));
	_task_ready(td_ptr);

	TIME_STRUCT s_time;
	_time_get(&s_time);
	start_time = s_time.SECONDS*1000 + s_time.MILLISECONDS;


	dds_message_pool = _msgpool_create(sizeof(DDS_TASK_MSG), DDS_MSG_QUEUE_SIZE, 0, 0);
	dds_response_pool = _msgpool_create(sizeof(DDS_RESP_MSG), DDS_MSG_QUEUE_SIZE, 0, 0);

	if (dds_message_pool == 0) {
		printf("Could not create dds message pool\n");
		_task_block();
	}


	// create AUX task
	_task_create(0, AUXTASK_TASK, 0);
	//Do forever
	while (TRUE){
		//Block while waiting to recieve a message from the message queue
		  dds_msg_ptr = _msgq_receive(dds_task_qid, 0);

  		  TIME_STRUCT dds_start;
  		  _time_get(&dds_start);

		  // start timer
		  if (dds_msg_ptr == NULL) {
			  printf("\n Receiving message failed\n");
			  _task_block();
		  }


			// Act accordingly to the type of message
				// Create
				// Delete
				// Return active (information request)
				// Return overdue (information request)
		  switch(dds_msg_ptr->dds_command) {
		  	  case DDS_CREATE: ;
		  	  {
		  		  create_command_data_t* command_data = (create_command_data_t *)dds_msg_ptr->data;
		  		  //printf("deadline: %d, template_index: %d \n", command_data->deadline, command_data->template_index);
		  		  // insert into active tasks
		  		  task_list_data_t* new_task = _mem_alloc_system(sizeof(task_list_data_t));
		  		  new_task->deadline = command_data->deadline;

		  		  _task_id tid = _task_create(0, command_data->template_index, command_data->runtime);
		  		  new_task->tid = tid;



		  		  TIME_STRUCT current_time;
		  		  _time_get(&current_time);
		  		  new_task->creation_time = (current_time.SECONDS*1000 + current_time.MILLISECONDS) - start_time;
		  		  new_task->task_type = command_data->task_type;
		  		  if (command_data->task_type == TASK_TYPE_PERIODIC) {
		  			  new_task->gid = command_data->gid;
		  		  }
		  		  add_task_list_entry(&active_tasks, new_task);



		  		  DDS_RESP_MSG_PTR resp_msg_ptr;
		  		  resp_msg_ptr = (DDS_RESP_MSG_PTR)_msg_alloc(dds_response_pool);
		  		  resp_msg_ptr->HEADER.TARGET_QID = dds_msg_ptr->HEADER.SOURCE_QID;
		  		  resp_msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0, DDS_MSG_QUEUE);
		  		  resp_msg_ptr->HEADER.SIZE = sizeof(DDS_RESP_MSG);
		  		  resp_msg_ptr->success = tid;

		  		  bool result = _msgq_send(resp_msg_ptr);

		  		  if (result != TRUE) {
		  			  printf("\n Create: Could not send resp message\n");
		  			  _task_block();
		  		  }


		  	  }
		  		  break;
		  	  case DDS_DELETE: ;
		  	  {
		  		  delete_command_data_t* command_data2 = (delete_command_data_t *) dds_msg_ptr->data;
		  		  //printf("tid: %d \n", command_data2->tid);
		  		  delete_task_list_entry(&active_tasks, command_data2->tid);


		  		  DDS_RESP_MSG_PTR resp_msg_ptr;
		  		  resp_msg_ptr = (DDS_RESP_MSG_PTR)_msg_alloc(dds_response_pool);
		  		  resp_msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, DELETE_QUEUE);
		  		  resp_msg_ptr->HEADER.SOURCE_QID = _msgq_get_id(0, DDS_MSG_QUEUE);
		  		  resp_msg_ptr->HEADER.SIZE = sizeof(DDS_RESP_MSG);
		  		  resp_msg_ptr->success = command_data2->tid;

		  		  bool result = _msgq_send(resp_msg_ptr);

		  		  if (result != TRUE) {
		  			  printf("\n DElete: Could not send resp message (%x)\n", _task_get_error());
		  			  _task_block();
		  		  }
		  		  // remove from active tasks
		  		  _task_destroy(command_data2->tid);
		  		  break;
		  	  }
		  	  case DDS_RETURN_ACTIVE_LIST:
		  		  //TODO
		  		  break;
		  	  case DDS_RETURN_OVERDUE_LIST:
		  		  //TODO
		  		  break;
		  	  case DDS_DEADLINE:  ;
		  	  	  {

		  		       deadline_command_data_t* command_data = (deadline_command_data_t *) dds_msg_ptr->data;

		  		  	  	task_list_entry_t* pos = active_tasks;
		  				while (pos != NULL) {

		  					task_list_data_t* data = pos->data;

		  					if (_task_get_td(data->tid) == NULL) {
		  						// task doesn't exist in system anymore
		  						pos = pos->next;
		  						//remove from ll
		  						delete_task_list_entry(&active_tasks, data->tid);
		  						printf("Don't think we should get here\n");
		  						continue;
		  					}

		  					if (command_data->tid == data->tid) {
		  						// this is the task we want
		  						add_task_list_entry(&overdue_tasks, data);
		  						delete_task_list_entry(&active_tasks, command_data->tid);
		  						_task_abort(data->tid);

		  						if (data->task_type == TASK_TYPE_PERIODIC) {
		  							// delete the generator
		  							_task_abort(data->gid);
		  							// remove any tasks that have gid == data->gid
		  							task_list_entry_t* pos2 = active_tasks;
		  							while (pos2 != NULL) {

		  										task_list_data_t* data2 = pos->data;
		  										// remove if this isn't same as above
		  										// all tasks that have same generator id, but not same tid
		  										if (data2->gid == data->gid && data2->tid != data->tid) {
													add_task_list_entry(&overdue_tasks, data2);
													delete_task_list_entry(&active_tasks, data2->tid);
													_task_abort(data2->tid);
		  										}

		  										pos2 = pos2->next;
		  									}
		  						}

		  						break;
		  					}
		  					pos = pos->next;
		  				}

		  		  	  break;
		  	  	  }
		  }

		  _msg_free(dds_msg_ptr);
		  //TODO:
		// Look at all tasks in active pool
		// Select the task with the shortest deadline
		// Set a timer for the deadline
		// Run that task

		_task_id tid_to_start = 0;
		uint32_t  shortest = 0xFFFFFFFF;
		uint16_t buffer_offset = 0;
		task_list_entry_t* pos = active_tasks;
		while (pos != NULL) {

			task_list_data_t* data = pos->data;

			if (_task_get_td(data->tid) == NULL) {
				// task doesn't exist in system anymore

				pos = pos->next;
				//remove from ll
				delete_task_list_entry(&active_tasks, data->tid);
				continue;

			}


			uint32_t this_abs_deadline = data->creation_time + data->deadline;

			if (this_abs_deadline < shortest) {
				tid_to_start = data->tid;
				shortest = this_abs_deadline;
			}


			_mqx_uint old;
			_task_set_priority(data->tid, (PRIORITY_OSA_TO_RTOS(WAITING_USER_TASK_PRIORITY)), &old);


			pos = pos->next;
		}

		_mqx_uint old;

		if (tid_to_start) {
			printf("DEBUG: Choose to run %d\n", tid_to_start);
			_task_set_priority(tid_to_start, (PRIORITY_OSA_TO_RTOS(ACTIVE_USER_TASK_PRIORITY)), &old);
			_task_id* deadline_val = _mem_alloc(sizeof(_task_id));
			*deadline_val = tid_to_start;

			TIME_STRUCT current_time;
			_time_get(&current_time);

			uint32_t c_time = current_time.MILLISECONDS + current_time.SECONDS* 1000;
			int32_t wait_for = shortest - c_time;
			if (wait_for < 0) {
				printf("\nBlew a deadline\n");
			}


			TIME_STRUCT wait_for_s;
			wait_for_s.MILLISECONDS = shortest % 1000;
			wait_for_s.SECONDS = shortest/1000;
			// clear any existing timer
			_timer_cancel(deadline_expiry_timer);
			deadline_expiry_timer = _timer_start_oneshot_at(deadline_expiry, deadline_val, TIMER_ELAPSED_TIME_MODE, &wait_for_s);
			// set a timer for the deadline that sends an "expired message"
		} else {
			printf("Choose to go idle, no tasks to run\n");
		}
		// stop timer and add to total
		  TIME_STRUCT dds_end;
		  _time_get(&dds_end);

		 uint32_t dds_start_ms = dds_start.SECONDS*1000 + dds_start.MILLISECONDS;
		 uint32_t dds_end_ms = dds_end.SECONDS*1000 + dds_end.MILLISECONDS;
		 ms_in_dds += (dds_end_ms - dds_start_ms);
	}






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

	printf("AUX task created\n");


#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */


/*	_task_id t1 = dd_tcreate(USERTASK_TASK, 3000, 1000, TASK_TYPE_APERIODIC);

    OSA_TimeDelay(500);                 /* Example code (for task release)

    _task_id t2 = dd_tcreate(USERTASK_TASK, 3000, 2000, TASK_TYPE_APERIODIC);

    OSA_TimeDelay(500);


    //printf("%d \n", dd_delete(t2));

    OSA_TimeDelay(3500);*/

	  dd_init();



	char buffer[128];
	memset(&buffer, 0, 128);

	char resp[1000];
	memset(&resp, 0, 512);

	OpenR(0);
    if (!_getline(&buffer)) {
    	printf("_getline failed\n");
    	_task_block();
    }

    Close();
    bool valid = false;

    if (buffer[0] == '\0') {
    	continue;
    }

    if (buffer[0] == 'A' || buffer[0] == 'a') {
    	// start an aperiodic task

    	char * toks  = strtok(&buffer[0], " "); // skip the A

    	uint32_t deadline = 0;
    	uint32_t runtime = 0;


    	toks = strtok(NULL, " "); //store runtime
    	if (toks == NULL) {
    		valid = false;
    	}
    	runtime = atoi(toks);
    	toks = strtok(NULL, " "); // store deadline
    	if (toks == NULL) {
    		valid = false;
    	}
    	deadline = atoi(toks);


    	sprintf(resp, "Creating an aperiodic task (R = %d; D = %d)", runtime, deadline);
    	_task_id t1 = dd_tcreate(USERTASK_TASK, deadline, runtime, TASK_TYPE_APERIODIC, 0);
    	valid = true;
    }
    if (buffer[0] == 'P' || buffer[0] == 'p') {
    		char * toks  = strtok(&buffer[0], " "); // skip the P

    		generator_params_t * params = _mem_alloc_system(sizeof(generator_params_t));


        	toks = strtok(NULL, " "); //store runtime
        	if (toks == NULL) {
        		valid = false;
        	}
        	params->runtime = atoi(toks);
        	toks = strtok(NULL, " "); // store deadline
        	if (toks == NULL) {
        		valid = false;
        	}
        	params->deadline = atoi(toks);

        	toks = strtok(NULL, " "); // store deadline
        	if (toks == NULL) {
        		valid = false;
        	}
        	params->period = atoi(toks);
        	_task_id tid = _task_create(0, PERIODICGENERATOR_TASK, params);
        	sprintf(resp, "Creating a periodic task (ID = %d) (R = %d; D = %d, P = %d)", tid, params->runtime, params->deadline, params->period);
        	valid = true;
    }

    if (buffer[0] == 'L' || buffer[0] == 'l') {
    	if (buffer[1] == 'A' || buffer[1] == 'a') {
    		task_list_entry_t* active_tasks;
    		dd_return_active_list(&active_tasks);
    		task_list_summary_str(active_tasks, &resp[0]);
    		valid = true;
    	}
    	if (buffer[1] == 'O' || buffer[1] == 'o') {
    		task_list_entry_t* overdue_tasks;
    		dd_return_overdue_list(&overdue_tasks);
    		task_list_summary_str(overdue_tasks, &resp[0]);
    		valid = true;
    	}
    }

    if (buffer[0] == 'D' || buffer[0] == 'd') {
    		char * toks  = strtok(&buffer[0], " "); // skip the P

    		_task_id to_destroy;


        	toks = strtok(NULL, " "); //store runtime
        	if (toks == NULL) {
        		valid = false;
        	}
        	to_destroy = atoi(toks);
	  		_task_destroy(to_destroy);
        	sprintf(resp, "Deleting periodic task %d", to_destroy);
        	valid = true;
    }

    if (!valid) {
    	sprintf(resp, "Invalid command");
    }

    _queue_id serial = OpenW();
	_putline(serial, resp);
	Close();


#ifdef PEX_USE_RTOS
  }
#endif
}

/*
** ===================================================================
**     Callback    : PeriodicGenerator_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void PeriodicGenerator_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
  
#ifdef PEX_USE_RTOS
 _task_id gid = _task_get_id();
 generator_params_t* params = (generator_params_t*)task_init_data;
  while (1) {
#endif
    /* Write your code here ... */

    dd_tcreate(USERTASK_TASK, params->deadline, params->runtime, TASK_TYPE_PERIODIC, gid);
    OSA_TimeDelay(params->period);                 /* Example code (for task release) */
   

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
