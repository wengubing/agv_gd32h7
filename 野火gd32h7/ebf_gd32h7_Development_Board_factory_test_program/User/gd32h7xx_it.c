/*!
    \file    gd32h7xx_it.c
    \brief   interrupt service routines
    
    \version 2023-03-31, V1.0.0, demo for GD32H7xx
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32h7xx_it.h"
#include "main.h"
#include "usart/bsp_usart.h"
#include "adc/bsp_adc.h"

extern void time_update(void);
/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
    /* if NMI exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
    /* if DebugMon exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
    /* if SVC exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
    /* if PendSV exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    time_update();
    if (heartbeat1_led_task_timer > 0x00)
    {
        heartbeat1_led_task_timer--;
        if(heartbeat1_led_task_timer == 0)
        {
            heartbeat1_led_task_flag = 1;
        }
    }
    if (potentiometer_task_timer > 0x00)
    {
        potentiometer_task_timer--;
        if(potentiometer_task_timer == 0)
        {
            potentiometer_task_flag = 1;
        }
    }
    if (rtc_task_timer > 0x00)
    {
        rtc_task_timer--;
        if(rtc_task_timer == 0)
        {
            rtc_task_flag = 1;
        }
    }
}

void POTENTIOMETER_INT_ADC_IRQHANDLER(void)
{
    /* 判断转换是否完成 */
    if (adc_interrupt_flag_get(POTENTIOMETER_ADCX, ADC_INT_FLAG_EOC) == SET)
    {
        /* 读取ADC_Data寄存器 (可清除中断标志位 无需软件清除) */
        adc_source_convertedvalue[0] = adc_regular_data_read(POTENTIOMETER_ADCX);
    }
}

/*!
    \brief      this function handles RTC interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TAMPER_STAMP_LXTAL_IRQHandler(void)
{
    if(RESET != rtc_flag_get(RTC_FLAG_TP0)) {
        rtc_flag_clear(RTC_FLAG_ALARM0);
        exti_flag_clear(EXTI_18);
        rtc_flag_clear(RTC_FLAG_TS | RTC_FLAG_TSOVR | RTC_FLAG_TP0);
    }
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHANDLER(void)
{

}*/
/*********************************************END OF FILE**********************/
