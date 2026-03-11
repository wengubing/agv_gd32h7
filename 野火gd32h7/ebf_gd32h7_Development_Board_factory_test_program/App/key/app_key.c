/**
  ******************************************************************************
  * @file       app_key.c
  * @author     embedfire
  * @version     V1.0
  * @date        2024
  * @brief      key 应用层功能接口
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
#include "App/key/app_key.h"
#include "key/bsp_gpio_key.h" 
#include "led/bsp_gpio_led.h"    

/**
 * @brief  Key任务初始化
 * @param  无
 * @retval 无
 */
void Key_Task_Init(void)
{
    /* KEY GPIO 初始化 */
    KEY_GPIO_Config();
    syscfg_analog_switch_enable(SYSCFG_PA0_ANALOG_SWITCH);
}
extern uint8_t test_success_num;
extern uint8_t test_error_num;
uint8_t key_test_num = 0;
/**
 * @brief  Key任务  
 * @param  无
 * @retval 无
 */
void Key_Task(void)
{
    if(KEY_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
    {
        key_test_num++;
        LED1_ON;
    }
    if(KEY_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
    {
        key_test_num++;
        LED2_ON;
    }
    if(key_test_num == 2)
    {
        test_success_num += 2;
        key_test_num = 3;
    }

}
 
 /*****************************END OF FILE***************************************/
