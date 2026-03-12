#ifndef __BSP_GPIO_KEY_H
#define	__BSP_GPIO_KEY_H


#include "gd32h7xx.h"

//  引脚定义
#define    KEY1_GPIO_CLK_PORT    	 RCU_GPIOA
#define    KEY1_GPIO_PORT    	     GPIOA			   
#define    KEY1_GPIO_PIN		    GPIO_PIN_0

#define    KEY2_GPIO_CLK_PORT     	RCU_GPIOD
#define    KEY2_GPIO_PORT    	    GPIOD		   
#define    KEY2_GPIO_PIN		    GPIO_PIN_7


 /** 按键按下标置宏
	*  按键按下为高电平，设置 KEY_ON=1， KEY_OFF=0
	*  若按键按下为低电平，把宏设置成KEY_ON=0 ，KEY_OFF=1 即可
	*/
#define KEY_ON	0
#define KEY_OFF	1

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
extern void KEY_GPIO_Config(void);

/**
  * @brief  检测是否有按键按下
  * @param  GPIOx：x 可以是 A，B，C，D或者 E
  *	@param	     GPIO_Pin：待读取的端口位
  * @retval KEY_OFF(没按下按键)、KEY_ON（按下按键）
*/
extern uint8_t Key_Scan(uint32_t GPIOx,uint16_t GPIO_Pin);


#endif /* __BSP_GPIO_KEY_H */

