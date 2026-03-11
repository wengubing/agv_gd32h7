/**
  ******************************************************************************
  * @file       app_potentiometer.c
  * @author     embedfire
  * @version     V1.0
  * @date        2024
  * @brief      电位器ADC采集应用层功能接口
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
#include "App/potentiometer/app_potentiometer.h"
#include "adc/bsp_adc.h"
#include "usart/bsp_usart.h"
#include "systick/bsp_systick.h"

#include "beep/bsp_gpio_beep.h"
#include "led/bsp_gpio_led.h" 
#include "dwt/bsp_dwt_delay.h"  

uint32_t  potentiometer_task_cycle;
uint32_t  potentiometer_task_timer;
uint8_t   potentiometer_task_flag;
uint8_t   test_potentiometer = 0;

/**
 * @brief  电位器计数复位
 * @param  无
 * @retval 无
 */
void Potentiometer_Task_Reset(void)
{
    potentiometer_task_timer   = potentiometer_task_cycle;
    potentiometer_task_flag    = 0;
}

/**
 * @brief  POTENTIOMETER_ADC任务初始化
 * @param  potentiometer_cycle: 任务轮询周期 单位ms(可修改系统节拍定时器)
 * @retval 无
 */
void Potentiometer_Task_Init(uint32_t potentiometer_cycle)
{
    potentiometer_task_cycle   = potentiometer_cycle;
    Potentiometer_Task_Reset();
}
extern uint8_t test_success_num;
extern uint8_t test_error_num;
/**
 * @brief  POTENTIOMETER_ADC任务  
 * @param  无
 * @retval 无
 */
void Potentiometer_Task(void)
{
    if(potentiometer_task_flag == 1)
    {
        float potentiometer_sig_convertedvalue = 0;//转化后的源始值的计算值
        /* ADC的采样值 / ADC精度 = 电压值 / 3.3 (16384--14B) */
        potentiometer_sig_convertedvalue = (float)adc_source_convertedvalue[0]*3.3f/16384;
        if(potentiometer_sig_convertedvalue >= 3)
        {
            if(test_potentiometer == 2)
            {
                test_potentiometer = 3;
            }
            else if(test_potentiometer == 0)
            {
                test_potentiometer = 1;
            }
        }
        else if(potentiometer_sig_convertedvalue ==0)
        {
            if(test_potentiometer == 1)
            {
                test_potentiometer = 3;
            }
            else if(test_potentiometer == 0)
            {
                test_potentiometer = 2;
            }
        }
        if(test_potentiometer == 3)
        {       
            test_success_num += 1;
            test_potentiometer = 4;
//            printf("\r\n 开始自动测试  注意：测试过程中LED2会不断闪烁,否则程序错误(按下NRST按键可复位并重新开始测试) !\r\n");
        }
//        printf("\r\n The current AD value = %f V(0x%04X)\r\n",potentiometer_sig_convertedvalue,adc_source_convertedvalue[0]);
        Potentiometer_Task_Reset();
    
    }
}
 
 /*****************************END OF FILE***************************************/
