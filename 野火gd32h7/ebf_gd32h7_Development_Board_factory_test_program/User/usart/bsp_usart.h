#ifndef __BSP_USART_H
#define	__BSP_USART_H

#include "gd32h7xx.h"
#include <stdio.h>
#include "debug/bsp_gpio_debug.h"

#if (DEBUG_USART_NUM == 0)

    //串口0-USART0
    #define  DEBUG_USARTX           USART0
    #define  DEBUG_USARTX_CLK       RCU_USART0
    #define  DEBUG_APBXCLKCMD       rcu_periph_clock_enable
    #define  DEBUG_BAUDRATE         115200
                
    #define  DEBUG_IRQ              USART0_IRQn
    #define  DEBUG_IRQHANDLER       USART0_IRQHandler
#endif

void USARTX_NVIC_Config(void);
void USARTX_Mode_Config(void);
void USARTX_Init(void);

void USARTX_SendByte(uint32_t usart_periph, uint8_t ch);
void USARTX_SendArray(uint32_t usart_periph, uint8_t *array, uint16_t num);
void USARTX_SendHalfWord(uint32_t usart_periph, uint16_t ch);
void USARTX_SendString(uint32_t usart_periph, uint8_t *str);
void USARTX_SendString_Lenth(uint32_t usart_periph, uint8_t *str,uint32_t num);

#endif /* __BSP_USART_H */
