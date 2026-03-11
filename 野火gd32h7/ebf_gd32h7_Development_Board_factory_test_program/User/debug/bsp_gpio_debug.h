#ifndef __BSP_GPIO_DEBUG_H
#define __BSP_GPIO_DEBUG_H

#include "gd32h7xx.h"
#include <stdio.h>

#define DEBUG_USART_NUM 0

#if (DEBUG_USART_NUM == 0)
          
   // USART GPIO 引脚宏定义
    #define  DEBUG_GPIO_CLK_TX            RCU_GPIOA
    #define  DEBUG_TX_GPIO_PORT           GPIOA   
    #define  DEBUG_TX_GPIO_PIN            GPIO_PIN_9
    
    #define  DEBUG_GPIO_CLK_RX            RCU_GPIOA
    #define  DEBUG_RX_GPIO_PORT           GPIOA
    #define  DEBUG_RX_GPIO_PIN            GPIO_PIN_10
    
    #define  DEBUG_TX_RX_GPIO_AF          GPIO_AF_7
    
#endif

void DEBUG_GPIO_Config(void);

#endif /* __BSP_GPIO_DEBUG_H  */
