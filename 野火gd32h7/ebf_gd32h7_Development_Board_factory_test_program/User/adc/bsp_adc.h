#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "gd32h7xx.h"
#include <stdio.h>

// POTENTIOMETER_ADC_DATA

#define POTENTIOMETER_ADCX   			                ADC0                                 /* 外设序号 */
#define POTENTIOMETER_ADC_CLK_PORT 	                    RCU_ADC0				            /* 外设时钟 */
#define POTENTIOMETER_ADC_CHANNEL			            ADC_CHANNEL_0	       		        /* 采样通道线 */

#define POTENTIOMETER_INT_ADC_IRQ			            ADC0_1_IRQn                         /* 外部中断向量号 */
#define POTENTIOMETER_INT_ADC_IRQHANDLER			    ADC0_1_IRQHandler                   /* 中断处理函数*/


#define    ADC_WATCHDOG_HT               0x0A00
#define    ADC_WATCHDOG_LT               0x0400

// ADC 通道宏定义
#define    ADC_CHANNEL                   ADC_CHANNEL_0

extern __IO uint16_t adc_source_convertedvalue[20];//转化后的源始值

void ADCX_NVIC_Config(void);
void ADCX_Mode_Config(void);
void ADCX_Init(void);


#endif /* __BSP_ADC_H  */
