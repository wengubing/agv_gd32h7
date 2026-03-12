/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: can_test.c
** 创 建 人: wxb
** 描    述: 基于gd32野火开发板硬件进行can收发测试
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 20260305
** 标准化重构日期：20260305
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define CAN_TEST_GLOBALS

//头文件
#include "can_test.h"
#include "bsp_can_fd.h"
#include "bsp_usart.h"                                      //调试打印串口
#include "bsp_gpio_key.h"									//按键扫描

//宏定义

//全局对象
 
//方法定义
/*can收发测试初始化*/
bool CanTest_Init(void)
{
	/*初始化can,在中断接收CAN数据包*/
	CAN_Config();
	can_struct_para_init(CAN_MDSC_STRUCT, &transmit_message);
	can_struct_para_init(CAN_MDSC_STRUCT, &receive_message);
	/* 初始化接收邮箱 */
	Init_RxMes(&receive_message);

	return 1;
}

/*启动时发送一帧数据测试*/
bool CanTest_SendOnce(void)
{
    uint8_t i = 0U;
    uint32_t timeout = 0xFFFFFFU;

    /*设置要发送的报文*/
    CAN_SetMsg(&transmit_message);

    /*把报文存储到发送邮箱，发送*/
    can_mailbox_config(CANX, CAN_TRANSMIT_NUM, &transmit_message);
    while ((RESET == can_flag_get(CANX, CAN_FLAG_MB0)) && (0U != timeout))
    {
        timeout--;
    }


    timeout = 0xFFFFFFU;
	printf("\r\n已使用CAN发送数据包！\r\n");
    printf("\r\n发送的报文内容为：\r\n");
    printf("\r\n 扩展ID号ExtId：0x%x \r\n", transmit_message.id);
    
    CAN_DEBUG_ARRAY((uint8_t*)(transmit_message.data), 8);
	
    if (SET == can2_receive_flag)
    {
        can2_receive_flag = RESET;
        printf("\r\n通讯成功！\r\n");
    }
    else
    {
        printf("\r\n通讯失败！\r\n");
    }

    if (SET == can2_receive_flag)
    {
        printf("\r\n接收成功！\r\n");
        can2_receive_flag = RESET;
        printf("\r\nCAN2 receive data: \r\n");
        CAN_DEBUG_ARRAY((uint8_t*)(receive_message.data), 8);
        Init_RxMes(&receive_message);   //清空接收缓存
    }

	return 1;
}

/*按键发送测试*/
bool CanTest_KeySend(void)
{
	/*按一次按键发送一次数据*/
	if(	Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
	{
		printf("main_while：按键一次\r\n");
		//LED1_ON;   //发送状态
		//LED2_OFF;    //接收状态
		CanTest_SendOnce();
	}
	
	return 1;
}

/************************************END OF FILE************************************/
 