/**
  ******************************************************************************
  * @file       bsp_gpio_potentiometer.c
  * @author     embedfire
  * @version     V1.0
  * @date        2024
  * @brief      电位器函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台：野火 GDH759IMK6 GD32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************************
  */ 
  
#include "potentiometer/bsp_gpio_potentiometer.h"


/**
 * @brief  初始化控制电位器的IO
 * @param  无
 * @retval 无
 */
void POTENTIOMETER_GPIO_Config(void)
{
#if 1  
    /* 开启POTENTIOMETER相关的GPIO外设/端口时钟 */
    rcu_periph_clock_enable(POTENTIOMETER_SIG_GPIO_CLK_PORT);
	
	/*选择要控制的GPIO引脚、设置GPIO模式为浮空模拟输入*/
    gpio_mode_set(POTENTIOMETER_SIG_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, POTENTIOMETER_SIG_GPIO_PIN);

#endif     
}


/*****************************END OF FILE***************************************/

