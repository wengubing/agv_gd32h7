/**
  ******************************************************************************
  * @file    bsp_beep.c
  * @author  embedfire
  * @version V1.0
  * @date    2023
  * @brief   蜂鸣器应用函数接口
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
  
#include "beep/bsp_gpio_beep.h"

 /**
  * @brief  初始化控制蜂鸣器的IO
  * @param  无
  * @retval 无
  */
void BEEP_GPIO_Config(void)
{	
#if 1  	
    /* 开启beep相关的GPIO外设/端口时钟 */
    rcu_periph_clock_enable(BEEP_GPIO_CLK_PORT);
    
    /* 关闭蜂鸣器*/
    BEEP(BEEP_OFF);
    
   /*选择要控制的GPIO引脚、设置GPIO模式为推挽复用、设置GPIO速率*/
    gpio_mode_set(BEEP_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, BEEP_GPIO_PIN);
    gpio_output_options_set(BEEP_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, BEEP_GPIO_PIN);
    
#endif
}
/*********************************************END OF FILE**********************/
