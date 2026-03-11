/**
  ******************************************************************************
  * @file       bsp_dwt_delay.c
  * @author     embedfire
  * @version     V1.0
  * @date        2024
  * @brief      使用内核定时器函数接口
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
  
#include "dwt/bsp_dwt_delay.h"   


/**
  * @brief  初始化时间戳
  * @param  无
  * @retval 无
  * @note   使用延时函数前，必须调用本函数
  */
void DWT_InitTick(void)
{
    /* 使能DWT外设 */
    DEMCR |= (uint32_t)DEMCR_TRCENA;
    
    /* DWT CYCCNT寄存器计数清0 */
    DWT_CYCCNT = (uint32_t)0U;//使能CYCCNT寄存器之前，先清0
    
    /* 使能Cortex-M DWT CYCCNT寄存器 */
    DWT_CTRL  |=(uint32_t)DWT_CTRL_CYCCNTENA;
}

/**
  * @brief  读取当前时间戳
  * @param  无
  * @retval 当前时间戳，即DWT_CYCCNT寄存器的值
  */
uint32_t DWT_GetTick(void)
{ 
    return ((uint32_t)DWT_CYCCNT);
}


/**
  * @brief  采用CPU的内部计数实现精确延时，32位计数器
  * @param  time : 延迟长度，单位1 us
  * @retval 无
  * @note   使用本函数前必须先调用DWT_InitTick函数使能计数器，
            或使能宏CPU_TS_INIT_IN_DELAY_FUNCTION
            最大延时值为8秒，即8*1000*1000
  */
void DWT_Delay_Us(uint32_t time)
{
    uint32_t dwt_ticks = 0;
    uint32_t dwt_tick1,dwt_tick2,tcnt=0;
    
/* 在函数内部初始化时间戳寄存器， */  
#if (DWT_INIT_IN_DELAY_FUNCTION)     
    /* 初始化时间戳并清零 */
    DWT_InitTick();
#endif
    dwt_ticks =time*(GET_CPU_CLKFREQ()/1000000); 
    tcnt = 0;
    dwt_tick1 = (uint32_t)DWT_GetTick();         /* 刚进入时的计数器值 */
    while(1)
    {
        dwt_tick2 = (uint32_t)DWT_GetTick();
        if(dwt_tick2 != dwt_tick1)
        {
            /* 32位计数器是递增计数器 */    
            if(dwt_tick2 > dwt_tick1)
            {
                tcnt += dwt_tick2 - dwt_tick1;  
            }
            else/* 重新装载 */
            {
                tcnt += dwt_tick2 + (UINT32_MAX-dwt_tick1);  
            }
            dwt_tick1 = dwt_tick2;
            
            /*时间超过/等于要延迟的时间,则退出 */
            if(tcnt >= dwt_ticks)
            {
                break;
            }
        }            
    
    
    }
}

/**
  * @brief  采用CPU的内部计数实现精确延时，32位计数器
  * @param  time : 延迟长度，单位1 ms
  * @retval 无
  */
void DWT_Delay_Ms(uint32_t time)
{
    DWT_Delay_Us(time*1000);
}


/**
  * @brief  采用CPU的内部计数实现精确延时，32位计数器
  * @param  time : 延迟长度，单位1 S
  * @retval 无
  */
void DWT_Delay_S(uint32_t time)
{
    DWT_Delay_Us(time*1000000);
}





/*********************************************END OF FILE**********************/

