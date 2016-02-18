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
  
  char buf[13];
  sprintf(buf, "\n\rType here: ");
  UART_DRV_SendDataBlocking(myUART_IDX, buf, sizeof(buf), 1000);

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
	  SERIAL_RX_MSG_PTR msg_ptr;
	  _mqx_uint i;
	  _queue_id serial_rx_qid;
	  bool result;
	  _task_id task_id;

	  printf("Handler Task Created\n\r");

	  /* open a message queue for recieving chars from serial rx ISR */
	  serial_rx_qid = _msgq_open(RX_MSG_QUEUE, 0);

	  if (serial_rx_qid == 0) {
		  printf("Could not open the serial rx queue\n");
		  _task_block();
	  }

	  rx_message_pool = _msgpool_create(sizeof(SERIAL_RX_MSG), RX_MSG_QUEUE_SIZE, 0, 0);

	  if (rx_message_pool == MSGPOOL_NULL_POOL_ID) {
		  printf("\n Could not create the rx_message_pool\n");
		  _task_block();
	  }

	  while (TRUE) {
		  msg_ptr = _msgq_receive(serial_rx_qid, 0);

		  if (msg_ptr == NULL) {
			  printf("\n Receiving message failed\n");
			  _task_block();
		  }


		  // put new character into the line buffer
		  line_buffer[line_buffer_offset] = msg_ptr->data;
		  line_buffer_offset++;
		  if (line_buffer_offset > LINE_BUFFER_SIZE-2) { // account for  null terminator
			  line_buffer_offset = 0;
		  }
		  _msg_free(msg_ptr);
		  printf("%s \n", line_buffer);
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
