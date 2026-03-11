#ifndef __BSP_GPIO_KEY_EX_H
#define __BSP_GPIO_KEY_EX_H

#include "stm32f10x.h"


/* 定义LED连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的LED引脚 */
// KEY3按键
#define KEY3_GPIO_PORT    			    GPIOB			                /* GPIO端口 */
#define KEY3_GPIO_CLK_PORT 	            RCC_APB2Periph_GPIOB			/* GPIO端口时钟 */
#define KEY3_GPIO_PIN			        GPIO_Pin_5	       				/* 连接到GPIO */

#define KEY3_EXTI_CLK_PORT 	            RCC_APB2Periph_AFIO			    /* 复用端口时钟 */
#define KEY3_EXTI_PORTSOUTCE            GPIO_PortSourceGPIOB		    /* 中断端口源 */
#define KEY3_EXTI_PINSOUTCE			    GPIO_PinSource5      		    /* 中断PIN源 */
#define KEY3_EXTI_LINE			        EXTI_Line5                      /* 中断线 */
#define KEY3_EXTI_IRQ			        EXTI9_5_IRQn                    /* 外部中断向量号 */
#define KEY3_EXTI_IRQHANDLER			EXTI9_5_IRQHandler              /* 中断处理函数 */

// KEY4按键
#define KEY4_GPIO_PORT    			    GPIOB			                /* GPIO端口 */
#define KEY4_GPIO_CLK_PORT 	            RCC_APB2Periph_GPIOB			/* GPIO端口时钟 */
#define KEY4_GPIO_PIN			        GPIO_Pin_6	       				/* 连接到GPIO */

#define KEY4_EXTI_CLK_PORT 	            RCC_APB2Periph_AFIO			    /* 复用端口时钟 */
#define KEY4_EXTI_PORTSOUTCE            GPIO_PortSourceGPIOB		    /* 中断端口源 */
#define KEY4_EXTI_PINSOUTCE			    GPIO_PinSource6      		    /* 中断PIN源 */
#define KEY4_EXTI_LINE			        EXTI_Line6                      /* 中断线 */
#define KEY4_EXTI_IRQ			        EXTI9_5_IRQn                    /* 外部中断向量号 */
#define KEY4_EXTI_IRQHANDLER			EXTI9_5_IRQHandler              /* 中断处理函数 */

// KEY5按键
#define KEY5_GPIO_PORT    			    GPIOB			                /* GPIO端口 */
#define KEY5_GPIO_CLK_PORT 	            RCC_APB2Periph_GPIOB			/* GPIO端口时钟 */
#define KEY5_GPIO_PIN			        GPIO_Pin_7	       				/* 连接到GPIO */

#define KEY5_EXTI_CLK_PORT 	            RCC_APB2Periph_AFIO			    /* 复用端口时钟 */
#define KEY5_EXTI_PORTSOUTCE            GPIO_PortSourceGPIOB		    /* 中断端口源 */
#define KEY5_EXTI_PINSOUTCE			    GPIO_PinSource7      		    /* 中断PIN源 */
#define KEY5_EXTI_LINE			        EXTI_Line7                      /* 中断线 */
#define KEY5_EXTI_IRQ			        EXTI9_5_IRQn                    /* 外部中断向量号 */
#define KEY5_EXTI_IRQHANDLER			EXTI9_5_IRQHandler              /* 中断处理函数 */


// KEY6按键
#define KEY6_GPIO_PORT    			    GPIOB			                /* GPIO端口 */
#define KEY6_GPIO_CLK_PORT 	            RCC_APB2Periph_GPIOB			/* GPIO端口时钟 */
#define KEY6_GPIO_PIN			        GPIO_Pin_8	       				/* 连接到GPIO */

#define KEY6_EXTI_CLK_PORT 	            RCC_APB2Periph_AFIO			    /* 复用端口时钟 */
#define KEY6_EXTI_PORTSOUTCE            GPIO_PortSourceGPIOB		    /* 中断端口源 */
#define KEY6_EXTI_PINSOUTCE			    GPIO_PinSource8      		    /* 中断PIN源 */
#define KEY6_EXTI_LINE			        EXTI_Line8                      /* 中断线 */
#define KEY6_EXTI_IRQ			        EXTI9_5_IRQn                    /* 外部中断向量号 */
#define KEY6_EXTI_IRQHANDLER			EXTI9_5_IRQHandler              /* 中断处理函数 */


/** 触发 板载按键 条件 
  * 1 - OFF
  * 0 - ON
  */
/** 按键按下标置宏
 *  按键按下为高电平，设置 KKEY_EX_ON=1， KEY_EX_OFF=0
 *  若按键按下为低电平，把宏设置成KEY_EX_ON=0 ，KEY_EX_OFF=1 即可
 */ 
#define KEY_EX_ON  0
#define KEY_EX_OFF 1

void KEY_EX_GPIO_Config(void);
void EXTI_KEY_EX_GPIO_Config(void);
uint32_t KEY_EX_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif /* __BSP_GPIO_KEY_EX_H  */
