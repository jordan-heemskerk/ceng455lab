/* ###################################################################
**     Filename    : Events.c
**     Project     : serial_echo
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-02-03, 15:53, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */
#include <stdio.h>
/*
** ===================================================================
**     Callback    : myUART_RxCallback
**     Description : This callback occurs when data are received.
**     Parameters  :
**       instance - The UART instance number.
**       uartState - A pointer to the UART driver state structure
**       memory.
**     Returns : Nothing
** ===================================================================
*/
void myUART_RxCallback(uint32_t instance, void * uartState)
{
  /* Write your code here ... */

	//debug
	//printf("%s\n\n", myRxBuff);

	// grab character and dump into serial_rxq

	SERIAL_CHAR_MSG_PTR msg_ptr;
	_queue_id client_id;
	bool result;

	msg_ptr = (SERIAL_CHAR_MSG_PTR)_msg_alloc(rx_message_pool);

	if (msg_ptr == NULL) {
		printf("\n Could not allocate a new message \n");
		_task_block();
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, RX_MSG_QUEUE);
	msg_ptr->HEADER.SIZE = sizeof(SERIAL_CHAR_MSG);
	msg_ptr->data = myRxBuff[0];

	result = _msgq_send(msg_ptr);

	if (result != TRUE) {
		printf("\nCould not send a message\n");
		_task_block();
	}

	//send data back to UART
	//UART_DRV_SendData(myUART_IDX, myRxBuff, sizeof(myRxBuff));
}


/* END Events */

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
