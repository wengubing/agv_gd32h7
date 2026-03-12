/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: can_test.h
** 创 建 人: wxb
** 描    述: 基于gd32野火开发板硬件进行can收发测试
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 20260305
** 标准化重构日期：20260305
**--------------------------------------------------------------------------------------
***************************************************************************************/
 
/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __can_test_h__
#define __can_test_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  CAN_TEST_GLOBALS
	#define CAN_TEST_EXT
#else
	#define CAN_TEST_EXT  extern
#endif

//头文件
#include "gd32h7xx.h"
#include <stdbool.h>
//#include <stdio.h>
//#include "bsp_can_fd.h"

//宏定义
 
//对象定义
 
/*外部声明*/
/*can配置对象*/
CAN_TEST_EXT can_mailbox_descriptor_struct transmit_message;
CAN_TEST_EXT can_mailbox_descriptor_struct receive_message;
CAN_TEST_EXT FlagStatus can2_receive_flag;
CAN_TEST_EXT bool CanTest_Init(void);
CAN_TEST_EXT bool CanTest_SendOnce(void);
CAN_TEST_EXT bool CanTest_KeySend(void);
	
#endif

/************************************END OF FILE************************************/
