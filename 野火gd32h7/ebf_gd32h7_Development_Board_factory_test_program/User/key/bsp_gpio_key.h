#ifndef __BSP_GPIO_KEY_H
#define	__BSP_GPIO_KEY_H


#include "gd32h7xx.h"

//  引脚定义
#define KEY1_GPIO_CLK_PORT       RCU_GPIOA
#define KEY1_GPIO_PORT           GPIOA			   
#define KEY1_GPIO_PIN            GPIO_PIN_0

#define KEY2_GPIO_CLK_PORT       RCU_GPIOD
#define KEY2_GPIO_PORT           GPIOD		   
#define KEY2_GPIO_PIN            GPIO_PIN_7


/** 触发 板载按键 条件 */
#define KEY_ON	0
#define KEY_OFF	1

extern  void KEY_GPIO_Config(void);
extern  uint8_t KEY_Scan(uint32_t GPIOx,uint16_t GPIO_Pin);


#endif /* __BSP_GPIO_KEY_H */

