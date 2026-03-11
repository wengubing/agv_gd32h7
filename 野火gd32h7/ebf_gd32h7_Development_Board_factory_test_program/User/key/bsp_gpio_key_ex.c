/**
  ******************************************************************************
  * @file    bsp_gpio_key_ex.c
  * @author     embedfire
  * @version     V1.0
  * @date        2024
  * @brief   扫描扩展按键函数接口
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
  
  
#include "key/bsp_gpio_key_ex.h"
#include "delay/bsp_delay.h"

/**
 * @brief  初始化控制KEY的IO
 * @param  无
 * @retval 无
 */
void KEY_EX_GPIO_Config(void)
{
    /* 定义一个GPIO结构体 */
    GPIO_InitTypeDef gpio_initstruct = {0};
//控制KEY3  
#if 0    
    /* 开启KEY相关的GPIO外设/端口时钟 */
    RCC_APB2PeriphClockCmd(KEY3_GPIO_CLK_PORT,ENABLE);

    /*选择要控制的GPIO引脚、设置GPIO模式为浮空输入、设置GPIO速率为50MHz*/
    gpio_initstruct.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin    = KEY3_GPIO_PIN;
    gpio_initstruct.GPIO_Speed  = GPIO_Speed_50MHz;
    GPIO_Init(KEY3_GPIO_PORT,&gpio_initstruct);

#endif
    
    
//控制KEY4  
#if 0    
    /* 开启KEY相关的GPIO外设/端口时钟 */
    RCC_APB2PeriphClockCmd(KEY4_GPIO_CLK_PORT,ENABLE);

    /*选择要控制的GPIO引脚、设置GPIO模式为浮空输入、设置GPIO速率为50MHz*/
    gpio_initstruct.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin    = KEY4_GPIO_PIN;
    gpio_initstruct.GPIO_Speed  = GPIO_Speed_50MHz;
    GPIO_Init(KEY4_GPIO_PORT,&gpio_initstruct);

#endif

//控制KEY5  
#if 0    
    /* 开启KEY相关的GPIO外设/端口时钟 */
    RCC_APB2PeriphClockCmd(KEY5_GPIO_CLK_PORT,ENABLE);

    /*选择要控制的GPIO引脚、设置GPIO模式为浮空输入、设置GPIO速率为50MHz*/
    gpio_initstruct.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin    = KEY5_GPIO_PIN;
    gpio_initstruct.GPIO_Speed  = GPIO_Speed_50MHz;
    GPIO_Init(KEY5_GPIO_PORT,&gpio_initstruct);

#endif
    
    
//控制KEY6  
#if 0    
    /* 开启KEY相关的GPIO外设/端口时钟 */
    RCC_APB2PeriphClockCmd(KEY6_GPIO_CLK_PORT,ENABLE);

    /*选择要控制的GPIO引脚、设置GPIO模式为浮空输入、设置GPIO速率为50MHz*/
    gpio_initstruct.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin    = KEY6_GPIO_PIN;
    gpio_initstruct.GPIO_Speed  = GPIO_Speed_50MHz;
    GPIO_Init(KEY6_GPIO_PORT,&gpio_initstruct);

#endif
    
}

/**
 * @brief  初始化控制中断按键用到的I/O口
 * @param  无
 * @retval 无
 */
void EXTI_KEY_EX_GPIO_Config(void)
{
    /* 定义一个GPIO结构体 */
    GPIO_InitTypeDef gpio_initstruct = {0};
    /* 定义一个中断控制器结构体 */
    NVIC_InitTypeDef nvic_initstruct = {0};
    /* 定义一个中断结构体 */
    EXTI_InitTypeDef exti_initstruct = {0};
//控制KEY3 中断   
#if 0   
    
    /* 开启KEY相关的GPIO外设/端口时钟 */
    RCC_APB2PeriphClockCmd(KEY3_GPIO_CLK_PORT,ENABLE);

    /*开启KEY相关的GPIO外设/端口复用时钟*/
    RCC_APB2PeriphClockCmd(KEY3_EXTI_CLK_PORT,ENABLE);
    
    /* 配置中断源 */
    nvic_initstruct.NVIC_IRQChannel                     = KEY3_EXTI_IRQ;
    /* 配置抢占优先级 */
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority   = 1;
    /* 配置子优先级 */
    nvic_initstruct.NVIC_IRQChannelSubPriority          = 0;
    /* 使能配置中断通道 */
    nvic_initstruct.NVIC_IRQChannelCmd                  = ENABLE;
    NVIC_Init(&nvic_initstruct);
    
    /*选择要控制的GPIO引脚、设置GPIO模式为浮空输入、设置GPIO速率为50MHz*/
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin = KEY3_GPIO_PIN;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY3_GPIO_PORT,&gpio_initstruct);

	/* 选择中断信号源*/
    GPIO_EXTILineConfig(KEY3_EXTI_PORTSOUTCE,KEY3_EXTI_PINSOUTCE);
    /* 选择中断LINE */
    exti_initstruct.EXTI_Line       = KEY3_EXTI_LINE;
    /* 选择中断模式*/
    exti_initstruct.EXTI_Mode       = EXTI_Mode_Interrupt;
    /* 选择触发方式*/
    exti_initstruct.EXTI_Trigger    = EXTI_Trigger_Falling;
    /* 使能中断*/
    exti_initstruct.EXTI_LineCmd    = ENABLE;
    
    EXTI_Init(&exti_initstruct);
#endif
/*****************************************************************************************************/        
//控制KEY4 中断   
#if 0   
    
    /* 开启KEY相关的GPIO外设/端口时钟 */
    RCC_APB2PeriphClockCmd(KEY4_GPIO_CLK_PORT,ENABLE);

    /*开启KEY相关的GPIO外设/端口复用时钟*/
    RCC_APB2PeriphClockCmd(KEY4_EXTI_CLK_PORT,ENABLE);
    
    /* 配置中断源 */
    nvic_initstruct.NVIC_IRQChannel                     = KEY4_EXTI_IRQ;
    /* 配置抢占优先级 */
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority   = 1;
    /* 配置子优先级 */
    nvic_initstruct.NVIC_IRQChannelSubPriority          = 0;
    /* 使能配置中断通道 */
    nvic_initstruct.NVIC_IRQChannelCmd                  = ENABLE;
    NVIC_Init(&nvic_initstruct);
    
    /*选择要控制的GPIO引脚、设置GPIO模式为浮空输入、设置GPIO速率为50MHz*/
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin = KEY4_GPIO_PIN;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY4_GPIO_PORT,&gpio_initstruct);

	/* 选择中断信号源*/
    GPIO_EXTILineConfig(KEY4_EXTI_PORTSOUTCE,KEY4_EXTI_PINSOUTCE);
    /* 选择中断LINE */
    exti_initstruct.EXTI_Line       = KEY4_EXTI_LINE;
    /* 选择中断模式*/
    exti_initstruct.EXTI_Mode       = EXTI_Mode_Interrupt;
    /* 选择触发方式*/
    exti_initstruct.EXTI_Trigger    = EXTI_Trigger_Falling;
    /* 使能中断*/
    exti_initstruct.EXTI_LineCmd    = ENABLE;
    
    EXTI_Init(&exti_initstruct);
#endif
/*****************************************************************************************************/        
//控制KEY5 中断   
#if 0   
    
    /* 开启KEY相关的GPIO外设/端口时钟 */
    RCC_APB2PeriphClockCmd(KEY5_GPIO_CLK_PORT,ENABLE);

    /*开启KEY相关的GPIO外设/端口复用时钟*/
    RCC_APB2PeriphClockCmd(KEY5_EXTI_CLK_PORT,ENABLE);
    
    /* 配置中断源 */
    nvic_initstruct.NVIC_IRQChannel                     = KEY5_EXTI_IRQ;
    /* 配置抢占优先级 */
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority   = 1;
    /* 配置子优先级 */
    nvic_initstruct.NVIC_IRQChannelSubPriority          = 0;
    /* 使能配置中断通道 */
    nvic_initstruct.NVIC_IRQChannelCmd                  = ENABLE;
    NVIC_Init(&nvic_initstruct);
    
    /*选择要控制的GPIO引脚、设置GPIO模式为浮空输入、设置GPIO速率为50MHz*/
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin = KEY5_GPIO_PIN;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY5_GPIO_PORT,&gpio_initstruct);

	/* 选择中断信号源*/
    GPIO_EXTILineConfig(KEY5_EXTI_PORTSOUTCE,KEY5_EXTI_PINSOUTCE);
    /* 选择中断LINE */
    exti_initstruct.EXTI_Line       = KEY5_EXTI_LINE;
    /* 选择中断模式*/
    exti_initstruct.EXTI_Mode       = EXTI_Mode_Interrupt;
    /* 选择触发方式*/
    exti_initstruct.EXTI_Trigger    = EXTI_Trigger_Falling;
    /* 使能中断*/
    exti_initstruct.EXTI_LineCmd    = ENABLE;
    
    EXTI_Init(&exti_initstruct);
#endif

/*****************************************************************************************************/        
//控制KEY6 中断   
#if 0   
    
    /* 开启KEY相关的GPIO外设/端口时钟 */
    RCC_APB2PeriphClockCmd(KEY6_GPIO_CLK_PORT,ENABLE);

    /*开启KEY相关的GPIO外设/端口复用时钟*/
    RCC_APB2PeriphClockCmd(KEY6_EXTI_CLK_PORT,ENABLE);
    
    /* 配置中断源 */
    nvic_initstruct.NVIC_IRQChannel                     = KEY6_EXTI_IRQ;
    /* 配置抢占优先级 */
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority   = 1;
    /* 配置子优先级 */
    nvic_initstruct.NVIC_IRQChannelSubPriority          = 0;
    /* 使能配置中断通道 */
    nvic_initstruct.NVIC_IRQChannelCmd                  = ENABLE;
    NVIC_Init(&nvic_initstruct);
    
    /*选择要控制的GPIO引脚、设置GPIO模式为浮空输入、设置GPIO速率为50MHz*/
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initstruct.GPIO_Pin = KEY6_GPIO_PIN;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY6_GPIO_PORT,&gpio_initstruct);

	/* 选择中断信号源*/
    GPIO_EXTILineConfig(KEY6_EXTI_PORTSOUTCE,KEY6_EXTI_PINSOUTCE);
    /* 选择中断LINE */
    exti_initstruct.EXTI_Line       = KEY6_EXTI_LINE;
    /* 选择中断模式*/
    exti_initstruct.EXTI_Mode       = EXTI_Mode_Interrupt;
    /* 选择触发方式*/
    exti_initstruct.EXTI_Trigger    = EXTI_Trigger_Falling;
    /* 使能中断*/
    exti_initstruct.EXTI_LineCmd    = ENABLE;
    
    EXTI_Init(&exti_initstruct);
#endif
   
}


/**
  * @brief  检测按键是否按下
  * @param  GPIOx：x 可以是 A，B，C，D或者 E  
  * @param  GPIO_Pin：待读取的端口位  
  * @note   无
  * @retval KEY_OFF(没按下按键)、KEY_ON（按下按键）
  */
uint32_t KEY_EX_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    uint32_t key_ex_scan_temp_i = 0;
    uint32_t filtering_factor = 50;
    if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_EX_ON)
    {
        while(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_EX_ON)
        {
            key_ex_scan_temp_i++;
            Rough_Delay_Ms(1);
            if(key_ex_scan_temp_i == filtering_factor)
            {
                break;
            }
        }
        if(key_ex_scan_temp_i != filtering_factor)
        {
            return KEY_EX_ON; 
        }  
    }
    return KEY_EX_OFF;
}


/*****************************END OF FILE***************************************/

