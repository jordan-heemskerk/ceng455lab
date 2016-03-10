/* ###################################################################
**     Filename    : os_tasks.h
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
** @file os_tasks.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         

#ifndef __os_tasks_H
#define __os_tasks_H
/* MODULE os_tasks */

#include "fsl_device_registers.h"
#include "clockMan1.h"
#include "pin_init.h"
#include "osa1.h"
#include "mqx_ksdk.h"
#include "uart1.h"
#include "fsl_mpu1.h"
#include "fsl_hwtimer1.h"
#include "MainTask.h"
#include "SerialTask.h"
#include "myUART.h"
#include "Handler.h"
#include "AuxTask.h"
#include "DdsTask.h"
#include "UserTask.h"
#include "MonitorTask.h"

#ifdef __cplusplus
extern "C" {
#endif 

#include <message.h>

#define RX_MSG_QUEUE_SIZE 3*128
#define TX_MSG_QUEUE_SIZE 1024
#define DDS_MSG_QUEUE_SIZE 1024

#define DDS_MSG_QUEUE 7
#define RX_MSG_QUEUE 8
#define TX_MSG_QUEUE 9

#define USER_TASK_QUEUE_START 10

typedef enum dds_command{
	DDS_CREATE,
	DDS_DELETE,
	DDS_RETURN_ACTIVE_LIST,
	DDS_RETURN_OVERDUE_LIST
}dds_command_e;

typedef struct serial_char_message
{
	MESSAGE_HEADER_STRUCT HEADER;
	unsigned char data;
} SERIAL_CHAR_MSG, * SERIAL_CHAR_MSG_PTR;

typedef struct dds_task_message
{
	MESSAGE_HEADER_STRUCT HEADER;
	dds_command_e dds_command;
	void *data;
} DDS_TASK_MSG, * DDS_TASK_MSG_PTR;

extern _pool_id rx_message_pool;
extern _pool_id tx_message_pool;
extern _pool_id dds_message_pool;

/*
** ===================================================================
**     Callback    : serial_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void serial_task(os_task_param_t task_init_data);


/*
** ===================================================================
**     Callback    : handler_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void handler_task(os_task_param_t task_init_data);

/*
** ===================================================================
**     Callback    : UserTask_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void UserTask_task(os_task_param_t task_init_data);

/*
** ===================================================================
**     Callback    : MonitorTask_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void MonitorTask_task(os_task_param_t task_init_data);

/*
** ===================================================================
**     Callback    : DdsTask_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void DdsTask_task(os_task_param_t task_init_data);

/*
** ===================================================================
**     Callback    : AuxTask_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void AuxTask_task(os_task_param_t task_init_data);

/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __os_tasks_H*/
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
