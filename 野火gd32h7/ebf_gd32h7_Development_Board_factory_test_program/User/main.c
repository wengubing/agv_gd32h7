/**
  ******************************************************************************
  * @file        main.c
  * @author     embedfire
  * @version     V1.0
  * @date        2023
  * @brief       出厂测试程序
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
#include "main.h"
#include "gd32h7xx.h"
#include <stdio.h>
#include "systick/bsp_systick.h"
#include "dwt/bsp_dwt_delay.h"   
#include "usart/bsp_usart.h"
#include "led/bsp_gpio_led.h"  
#include "beep/bsp_gpio_beep.h"
#include "adc/bsp_adc.h"
#include "lcd/bsp_lcd.h"

#include "netconf.h"
#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "usart/bsp_usart.h"   
#include "lwip/apps/lwiperf.h"

#include "enet/bsp_enet.h" 

#include "sdio/bsp_sdio.h"
#include "sdio/bsp_sdio_sdcard.h"
#include "sdio/bsp_sdio_test.h"

uint8_t test_success_num = 0;
uint8_t test_error_num = 0;

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
    /* 使能cpu高速缓存*/ 
    SCB_EnableICache();     // 使能 I-Cache
    SCB_EnableDCache();     // 使能 D-Cache
    
    /* 嵌套向量中断控制器组选择 */
    /* 提示 NVIC_PriorityGroupConfig() 在整个工程只需要调用一次来配置优先级分组*/
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    
    /* systick初始化配置 */
    Systick_Int();

    /* 初始化调试串口 */
    USARTX_Init();
    
    printf("\r\n 欢迎使用野火 GDH759IMK6-GD32开发板 !\r\n");  
    
    /* BEEP GPIO 初始化 */
	BEEP_GPIO_Config();	
    
    /* ADC 初始化 */
    ADCX_Init();
    
    /* 心跳灯 任务初始化 */
    Heartbeat1_LED_Task_Init(100);
    
    /* SDRAM 任务初始化 */
    Sdram_Task_Init();
    sdram_task_flag = 1;
    
    /* FLASH 任务初始化 */
    Flash_Task_Init();
    flash_task_flag = 1;

    /* RTC 任务初始化 */
    Rtc_Task_Init(1000);
  
    /* KEY任务初始化 */
    Key_Task_Init();

    /* 电位器任务初始化 */
    Potentiometer_Task_Init(1000);
  
    /* Enet 任务初始化 */	
    Enet_Task_Init();
    
    /* SD卡 初始化测试 */
    if(SD_integrated_testing()==0)
    {
        test_success_num++;
    }
    printf("\r\n 注意：测试过程中LED3会不断闪烁,否则程序错误(按下NRST按键可复位并重新开始测试) !\r\n");
    while(1)
	{	
        /* 心跳灯 */ 
        Heartbeat1_LED_Task(); 
        
        /* 电位器 任务 */ 
        Potentiometer_Task();
        
        /* SDRAM 测试任务 */
        Sdram_Task();
        
        /* FLASH 测试任务 */ 
        Flash_Task();
        
        /* 按键测试任务 */ 
        Key_Task();
        
        /* RTC测试任务 */
        Rtc_Task();
        
        if(test_error_num)
        {
            BEEP(BEEP_ON);
            LED1_LED2_LED3_EX_ALL_ON;
            printf("\r\n 测试未通过 !\r\n");
        }
        else
        {
            if(test_success_num+test_error_num == 5)
            {
                    BEEP(BEEP_ON);
                    DWT_Delay_Ms(500);
                    BEEP(BEEP_OFF);

                /*初始化液晶屏*/
                LCD_Init();
                LCD_LayerInit();
                tli_enable();
                Colour_Set(0,0xff,0x0,0x0);
                DWT_Delay_Ms(2000);
                Colour_Set(0,0x0,0xff,0x0);
                DWT_Delay_Ms(2000);
                Colour_Set(0,0x0,0x0,0xff);
                DWT_Delay_Ms(2000);
                printf("\r\n 前面测试通过 !\r\n");
                test_success_num++;
            }
            if(test_success_num+test_error_num == 9)
            {
                for(uint8_t temp = 0;temp<3;temp++)
                {
                    LED1_LED2_LED3_EX_ALL_ON;
                    DWT_Delay_Ms(500);
                    LED1_OFF;
                    LED2_OFF;
                    LED3_OFF;
                    DWT_Delay_Ms(500);
                }
                    BEEP(BEEP_ON);
                    DWT_Delay_Ms(500);
                    BEEP(BEEP_OFF);
                printf("\r\n 测试全部通过 !\r\n");
                test_success_num = 0;
            }
        }
        
	}
}


/*********************************************END OF FILE**********************/
