/**
  ******************************************************************************
  * @file       app_rtc.c
  * @author     embedfire
  * @version     V1.0
  * @date        2024
  * @brief      rtc应用层功能接口
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
#include "App/rtc/app_rtc.h"
#include "usart/bsp_usart.h"
#include "systick/bsp_systick.h"
#include "rtc/bsp_rtc.h"

uint32_t  rtc_task_cycle;
uint32_t  rtc_task_timer;
uint8_t   rtc_task_flag;
uint8_t   test_rtc = 0;

/**
 * @brief  rtc 计数复位
 * @param  无
 * @retval 无
 */
void Rtc_Task_Reset(void)
{
    rtc_task_timer   = rtc_task_cycle;
    rtc_task_flag    = 0;
}

/** 
 * @brief  rtc 任务初始化
 * @param  rtc_cycle: 任务轮询周期 单位ms(可修改系统节拍定时器)
 * @retval 无
 */
void Rtc_Task_Init(uint32_t rtc_cycle)
{
    /* 配置RTC时钟 */
    RTC_Configuration();
    
    /* 配置RTC相关参数 */
    RTC_CheckAndConfig();
    
    rtc_task_cycle   = rtc_cycle;
    Rtc_Task_Reset();
}
extern uint8_t test_success_num;
extern uint8_t test_error_num;
extern uint8_t rcu_osci_state;
/**
 * @brief  rtc 任务  
 * @param  无
 * @retval 无
 */
void Rtc_Task(void)
{
    if(rtc_task_flag == 1)
    {
        RTC_SHOW_TIME(); 
        if(rcu_osci_state == 1)
        {         
            test_success_num += 1;
            rcu_osci_state = 2;
        }
        if(rcu_osci_state == 0)
        { 
            test_error_num +=1;
            rcu_osci_state = 2;
        }
        Rtc_Task_Reset();
    }
}
 
 /*****************************END OF FILE***************************************/
