/**
  ******************************************************************************
  * @file    bsp_gpio_key.c
  * @author     embedfire
  * @version     V1.0
  * @date        2023
  * @brief   key应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台：野火 GDF303-霸道 GD32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************************
  */ 
  
#include "key/bsp_gpio_key.h" 

/**
  * @brief  配置按键用到的I/O口
  * @param  无
  * @retval 无
  */
void KEY_GPIO_Config(void)
{
	/*开启按键端口的时钟*/
	rcu_periph_clock_enable(KEY1_GPIO_CLK_PORT);
	rcu_periph_clock_enable(KEY2_GPIO_CLK_PORT);

	/*选择按键的引脚、设置按键的引脚为浮空输入、设置GPIO速率为50MHz*///	gpio_init(KEY1_GPIO_PORT|KEY2_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,KEY1_GPIO_PIN|KEY2_GPIO_PIN);
    gpio_mode_set(KEY1_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY1_GPIO_PIN);
    gpio_output_options_set(KEY1_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, KEY1_GPIO_PIN);
    
    gpio_mode_set(KEY2_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY2_GPIO_PIN);
    gpio_output_options_set(KEY2_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100_220MHZ, KEY2_GPIO_PIN);
    
    
    
}

/**
  * @brief  检测是否有按键按下
  * @param  GPIOx：x 可以是 A，B，C，D或者 E
  *	@param	     GPIO_Pin：待读取的端口位
  * @retval KEY_OFF(没按下按键)、KEY_ON（按下按键）
*/
uint8_t Key_Scan(uint32_t GPIOx,uint16_t GPIO_Pin)
{			
	/*检测是否有按键按下 */
	if(gpio_input_bit_get(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		/*等待按键释放 */
		while(gpio_input_bit_get(GPIOx,GPIO_Pin) == KEY_ON);   
		return 	KEY_ON;	 
	}
	else
    {
    	return KEY_OFF;
    }

}
/*********************************************END OF FILE**********************/
