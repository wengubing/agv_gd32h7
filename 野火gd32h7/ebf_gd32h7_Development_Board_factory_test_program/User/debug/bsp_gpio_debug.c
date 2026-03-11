/**
  ******************************************************************************
  * @file       bsp_gpio_debug.c
  * @author     embedfire
  * @version     V1.0
  * @date        2024
  * @brief      debug函数接口
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
  
#include "debug/bsp_gpio_debug.h"


/**
 * @brief  初始化控制DEBUG的IO
 * @param  无
 * @retval 无
 */
void DEBUG_GPIO_Config(void)
{
#if 1  
    /* 开启USART相关的GPIO外设/端口时钟 */
    rcu_periph_clock_enable(DEBUG_GPIO_CLK_TX);
	
	/*选择要控制的GPIO引脚、设置GPIO模式为推挽复用、设置GPIO速率*/
    gpio_af_set(DEBUG_TX_GPIO_PORT, DEBUG_TX_RX_GPIO_AF, DEBUG_TX_GPIO_PIN);
    gpio_mode_set(DEBUG_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, DEBUG_TX_GPIO_PIN);
    gpio_output_options_set(DEBUG_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, DEBUG_TX_GPIO_PIN);
#endif 
    
#if 1  
    /* 开启USART相关的GPIO外设/端口时钟 */
    rcu_periph_clock_enable(DEBUG_GPIO_CLK_RX);
    
    /*选择要控制的GPIO引脚、设置GPIO模式为浮空输入、设置GPIO速率*/
    gpio_af_set(DEBUG_RX_GPIO_PORT, DEBUG_TX_RX_GPIO_AF, DEBUG_RX_GPIO_PIN);
    gpio_mode_set(DEBUG_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, DEBUG_RX_GPIO_PIN);
    gpio_output_options_set(DEBUG_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, DEBUG_RX_GPIO_PIN);
#endif  
}


/*****************************END OF FILE***************************************/

