/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : uart1.h
**     Project     : deadline_scheduler
**     Processor   : MK64FN1M0VLL12
**     Component   : fsl_uart
**     Version     : Component 1.3.0, Driver 01.00, CPU db: 3.00.000
**     Repository  : KSDK 1.3.0
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-03-09, 13:53, # CodeGen: 0
**     Contents    :
**         UART_DRV_Init                - uart_status_t UART_DRV_Init(uint32_t instance,uart_state_t *...
**         UART_DRV_Deinit              - uart_status_t UART_DRV_Deinit(uint32_t instance);
**         UART_DRV_InstallRxCallback   - uart_rx_callback_t UART_DRV_InstallRxCallback(uint32_t...
**         UART_DRV_InstallTxCallback   - uart_tx_callback_t UART_DRV_InstallTxCallback(uint32_t...
**         UART_DRV_SendDataBlocking    - uart_status_t UART_DRV_SendDataBlocking(uint32_t instance,const uint8_t *...
**         UART_DRV_SendData            - uart_status_t UART_DRV_SendData(uint32_t instance,const uint8_t *...
**         UART_DRV_GetTransmitStatus   - uart_status_t UART_DRV_GetTransmitStatus(uint32_t instance,uint32_t *...
**         UART_DRV_AbortSendingData    - uart_status_t UART_DRV_AbortSendingData(uint32_t instance);
**         UART_DRV_ReceiveDataBlocking - uart_status_t UART_DRV_ReceiveDataBlocking(uint32_t instance,uint8_t *...
**         UART_DRV_ReceiveData         - uart_status_t UART_DRV_ReceiveData(uint32_t instance,uint8_t *...
**         UART_DRV_GetReceiveStatus    - uart_status_t UART_DRV_GetReceiveStatus(uint32_t instance,uint32_t *...
**         UART_DRV_AbortReceivingData  - uart_status_t UART_DRV_AbortReceivingData(uint32_t instance);
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file uart1.h
** @version 01.00
*/         
/*!
**  @addtogroup uart1_module uart1 module documentation
**  @{
*/         
#ifndef __uart1_H
#define __uart1_H
/* MODULE uart1. */
/* Include inherited beans */
#include "clockMan1.h"
#include "Cpu.h"

/*! @brief Device instance number */
#define uart1_IDX UART0_IDX
/*! @brief Device instance number for backward compatibility */
#define FSL_UART1 uart1_IDX
  
  
/*! @brief UART configuration declaration */
extern const uart_user_config_t uart1_InitConfig0;
    
#endif
/* ifndef __uart1_H */
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
