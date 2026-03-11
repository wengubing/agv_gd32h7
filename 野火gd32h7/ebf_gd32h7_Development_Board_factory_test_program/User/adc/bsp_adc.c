/**
  ******************************************************************************
  * @file       bsp_adc.c
  * @author     embedfire
  * @version     V1.0
  * @date        2024
  * @brief      ADC配置函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台：野火 GDH759IMK6-GD32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************************
  */
  
#include "potentiometer/bsp_gpio_potentiometer.h"
#include "adc/bsp_adc.h"


__IO uint16_t adc_source_convertedvalue[20] = {0};//转化后的源始值

/**
  * @brief  配置POTENTIOMETER_ADC中断配置
  * @param  无
  * @retval 无
  */
void ADCX_NVIC_Config(void)
{
    /* 配置中断源：ADC 、配置抢占优先级、 配置响应优先级、使能中断通道*/
#if 1
    nvic_irq_enable(POTENTIOMETER_INT_ADC_IRQ, 1U, 0U);  
#endif    
}

/**
 * @brief  配置ADCX模式
 * @param  无
 * @retval 无
 */
void ADCX_Mode_Config(void)
{
   
#if 1  
    /* 开启ADC相关的GPIO外设/端口时钟 */
    rcu_periph_clock_enable(POTENTIOMETER_ADC_CLK_PORT);
    
    /* 初始化adc */
    adc_deinit(POTENTIOMETER_ADCX);
    
    /* 配置ADC时钟 */
    adc_clock_config(POTENTIOMETER_ADCX, ADC_CLK_SYNC_HCLK_DIV6);
    
    /* ADC 模式配置 */
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);                                                    //只有一个ADC,属于独立模式
    adc_special_function_config(POTENTIOMETER_ADCX, ADC_SCAN_MODE,DISABLE);                             //禁止扫描模式，单通道不需要
    adc_special_function_config(POTENTIOMETER_ADCX, ADC_CONTINUOUS_MODE, ENABLE);                       //使能连续扫描模式
    adc_external_trigger_config(POTENTIOMETER_ADCX,ADC_REGULAR_CHANNEL, EXTERNAL_TRIGGER_DISABLE);      //不需要外部触发转换,使用软件开启
    adc_data_alignment_config(POTENTIOMETER_ADCX, ADC_DATAALIGN_RIGHT);                                 //转换结构右对齐
    adc_channel_length_config(POTENTIOMETER_ADCX, ADC_REGULAR_CHANNEL, 1U);                             // 转换通道个数
    adc_resolution_config(POTENTIOMETER_ADCX,ADC_RESOLUTION_14B);                                       // ADC分辨率 14B
 
#endif

    /* ADC模拟看门狗阈值配置 */
    adc_watchdog0_threshold_config(POTENTIOMETER_ADCX, ADC_WATCHDOG_LT, ADC_WATCHDOG_HT);
    /* ADC模拟看门狗单通道配置 */
    adc_watchdog0_single_channel_enable(POTENTIOMETER_ADCX, POTENTIOMETER_ADC_CHANNEL);
    

    /* ADC的转化配置 */
    adc_regular_channel_config(POTENTIOMETER_ADCX, 0, POTENTIOMETER_ADC_CHANNEL, 240);      //配置 ADC 通道转换顺序和采样时间     
    adc_interrupt_enable(POTENTIOMETER_ADCX,ADC_INT_EOC);       //ADC 转换结束产生中断，在中断服务程序中读取转换值
//    adc_interrupt_enable(POTENTIOMETER_ADCX, ADC_INT_WDE0);                
    adc_enable(POTENTIOMETER_ADCX);                         //开启ADC转换
    
    
    /* ADC校准 */
    adc_calibration_mode_config(POTENTIOMETER_ADCX, ADC_CALIBRATION_OFFSET_MISMATCH); //ADC校准模式配置
    adc_calibration_number(POTENTIOMETER_ADCX, ADC_CALIBRATION_NUM1);                 //ADC校准数配置
    adc_calibration_enable(POTENTIOMETER_ADCX);                                       //开始校准
    
    adc_software_trigger_enable(POTENTIOMETER_ADCX, ADC_REGULAR_CHANNEL); //由于没有采用外部触发,所以配置软件触发ADC转换
}

/** 
 * @brief  ADCX初始化
 * @param  无
 * @retval 无
 */
void ADCX_Init(void)
{
    /* 对应的GPIO的配置 */
//    POTENTIOMETER_GPIO_Config();
    
    /* 配置ADCX模式 */
    ADCX_Mode_Config();
    
    /* 配置对应的中断 */
    ADCX_NVIC_Config();
    
}



/*****************************END OF FILE***************************************/

