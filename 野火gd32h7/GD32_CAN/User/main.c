/**
 20260305 基于gd32野火开发板硬件can收发实验
 **/ 

#include "gd32h7xx.h"
#include "usart/bsp_usart.h"
#include "can/bsp_can_fd.h"
#include "led/bsp_gpio_led.h"
#include "key/bsp_gpio_key.h"
#include <stdio.h>
#include "string.h"

can_mailbox_descriptor_struct transmit_message;
can_mailbox_descriptor_struct receive_message;
FlagStatus can2_receive_flag = 0;

/*main*/
int main(void)
{
    uint8_t i = 0U;
    uint32_t timeout = 0xFFFFFFU;

    /* 使能cpu高速缓存*/ 
    SCB_EnableICache();     // 使能 I-Cache
    SCB_EnableDCache();     // 使能 D-Cache
    
    /* 嵌套向量中断控制器组选择 */
    /* 提示 NVIC_PriorityGroupConfig() 在整个工程只需要调用一次来配置优先级分组*/
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0); 
      
    /* 初始化调试串口 */
    USART_Config();
    
    /* 初始化LED */
    LED_GPIO_Config();	
    
    LED1_OFF;   //发送状态
    LED2_ON;    //接收状态
    
	/*初始化can,在中断接收CAN数据包*/
	CAN_Config();
    
    can_struct_para_init(CAN_MDSC_STRUCT, &transmit_message);
    can_struct_para_init(CAN_MDSC_STRUCT, &receive_message);
    
    /* 初始化接收邮箱 */
    Init_RxMes(&receive_message);
    
    /* 初始化按键 */
    KEY_GPIO_Config();
    
    printf("欢迎使用野火教程——CAN双机通讯实验例程FD模式!\r\n");
    
    printf("\r\n 野火 GDH759IMK6-GD32 开发板。\r\n");
	
	printf("\r\n 实验步骤：\r\n");

    printf("\r\n 1.使用导线连接好两个CAN讯设备\r\n");
	printf("\r\n 3.按下开发板的KEY1键，会使用CAN向外发送0-7的数据包，包的扩展ID为0x%x \r\n",CANX_ID);
	printf("\r\n 4.若开发板的CAN接收到扩展ID为0x%x的数据包，会把数据以打印到串口。 \r\n",CANX_ID);
	printf("\r\n 5.本例中的can波特率为1MBps，为gd32的can最高速率。 \r\n");

    while(1)
    {
        /*按一次按键发送一次数据*/
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
            LED1_ON;   //发送状态
            LED2_OFF;    //接收状态
			/*设置要发送的报文*/
			CAN_SetMsg(&transmit_message);
			/*把报文存储到发送邮箱，发送*/
            can_mailbox_config(CANX, CAN_TRANSMIT_NUM, &transmit_message);
            while((RESET == can_flag_get(CANX, CAN_FLAG_MB0)) && (0U != timeout)) 
            {
                timeout--;
            }
            

            timeout = 0xFFFFFFU;
			printf("\r\n已使用CAN发送数据包！\r\n"); 			
			printf("\r\n发送的报文内容为：\r\n");
			printf("\r\n 扩展ID号ExtId：0x%x \r\n",transmit_message.id);
			CAN_DEBUG_ARRAY((uint8_t *)(transmit_message.data),8); 
            if(SET == can2_receive_flag)
            {			
                can2_receive_flag = RESET;
                printf("\r\n通讯成功！\r\n"); 
            }
            else
            {
                printf("\r\n通讯失败！\r\n"); 
            }
            LED1_OFF;   //发送状态
            LED2_ON;    //接收状态
		}
        if(SET == can2_receive_flag) 
        {
            printf("\r\n接收成功！\r\n"); 
            can2_receive_flag = RESET;
            printf("\r\nCAN2 receive data: \r\n");
            CAN_DEBUG_ARRAY((uint8_t *)(receive_message.data),8); 
            Init_RxMes(&receive_message);   //清空接收缓存
        }
       
    }
}

/*********************************************END OF FILE**********************/

