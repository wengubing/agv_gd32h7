#ifndef __BSP_GPIO_BEEP_H
#define	__BSP_GPIO_BEEP_H

#include "gd32h7xx.h"

/* 定义蜂鸣器连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的蜂鸣器引脚 */
#define BEEP_GPIO_PORT    			        GPIOB			              /* GPIO端口 */
#define BEEP_GPIO_CLK_PORT    	            RCU_GPIOB			          /* GPIO时钟端口 */
#define BEEP_GPIO_PIN		 				GPIO_PIN_0			        /* 连接到蜂鸣器的GPIO */

/** 触发 蜂鸣器 条件 */
#define BEEP_ON	    1
#define BEEP_OFF	0

/* 带参宏，可以像内联函数一样使用 */
#define BEEP(A)	if (A)	    gpio_bit_set(BEEP_GPIO_PORT, BEEP_GPIO_PIN);\
                else	  gpio_bit_reset(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
 /**
  * @brief  初始化控制蜂鸣器的IO
  * @param  无
  * @retval 无
  */				
extern void BEEP_GPIO_Config(void);
					
#endif /* __BSP_GPIO_BEEP_H */
