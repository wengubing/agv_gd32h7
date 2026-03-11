/**
  ******************************************************************************
  * @file       app_led.c
  * @author     embedfire
  * @version     V1.0
  * @date        2024
  * @brief   led应用层功能接口
  ******************************************************************************
  * @attention
  *
  * 实验平台  ：野火 STM32F103C8T6-STM32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************************
  */
#include "App/led/app_led.h"
#include "led/bsp_gpio_led.h" 
#include "systick/bsp_systick.h"

uint32_t  heartbeat1_led_task_cycle;
uint32_t  heartbeat1_led_task_timer;
uint8_t   heartbeat1_led_task_flag;


/**
 * @brief  心率指示灯计数复位
 * @param  无
 * @retval 无
 */
void Heartbeat1_LED_Task_Reset(void)
{
    heartbeat1_led_task_timer   = heartbeat1_led_task_cycle;
    heartbeat1_led_task_flag    = 0;

}

/**
 * @brief  心率指示灯任务初始化
 * @param  heartbeat1_led_cycle: 任务轮询周期 单位ms(可修改系统节拍定时器)
 * @retval 无
 */
void Heartbeat1_LED_Task_Init(uint32_t heartbeat1_led_cycle)
{
    /* LED GPIO 初始化 */
    LED_GPIO_Config();
    heartbeat1_led_task_cycle   = heartbeat1_led_cycle;
    Heartbeat1_LED_Task_Reset();
}



/**
 * @brief  心率指示灯任务  
 * @param  无
 * @retval 无
 */
void Heartbeat1_LED_Task(void)
{
    if(heartbeat1_led_task_flag == 1)
    {
        LED3_TOGGLE;
        Heartbeat1_LED_Task_Reset();
    }
}
 
 /*****************************END OF FILE***************************************/
