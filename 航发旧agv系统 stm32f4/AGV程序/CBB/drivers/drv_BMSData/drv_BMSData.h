/****************************Copyright (c)**********************************************                                       
**                  ╭--------------------------╮
**                  ︱   成都航发机器人有限公司 ︱
**                  ︱   http://www.hangfa.com 	︱                     
**					︱ 	  All Rights Reserved	︱
**                  ╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_BMSData.h
** 创 建 人: Master
** 描    述: BMS数据处理头文件，包含所需数据结构体和函数声明
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0
** 创建日期: 2025年4月21日
**--------------------------------------------------------------------------------------
***************************************************************************************/  
//保证以下所有内容只被同一源文件引用一次*/
#ifndef __DRV_BMS_DATA_H__
#define __DRV_BMS_DATA_H__

/*控制对象实例化只被指定文件编译时实例化一次*/
#ifdef  BMS_DATA_GLOBALS
	#define BMS_DATA_GLOBALS_EXT
#else
	#define BMS_DATA_GLOBALS_EXT  extern
#endif

/*自定义引用*/																
#include <stm32f4xx.h>

//数据输入类型
typedef enum
{
	BMS_InPutSource_LeadBat		= 0x00,		/*铅酸电池		*/
	BMS_InPutSource_MCB			= 0x01,		/*系统直连的ModBus		*/
	BMS_InPutSource_CommConv	= 0x02,		/*卡式转接板直连的ModBus	*/
	BMS_InPutSource_SerialExp	= 0x03,		/*导航转接板的ModBus		*/
	BMS_InPutSource_KXD_MCB		= 0x04,		/*凯信达的私有协议		*/
	BMS_InPutSource_CATL		= 0x05,		/*凯信达的私有协议		*/
	BMS_InPutSource_CablePower	= 0x06,		/*外接电源				*/
}BMS_InPutSource_EnumDef;


typedef struct
{
	float 		Voltage			;	//电压 	单位：1v
	float 		Current			;	//电流 	单位：1A,正为充电，负为放电
	float 		SOC				;	//SOC 	单位：1%
	float 		SOH				;	//SOH	
	uint32_t 	Alarm_Code		;	//报警	BMS的原始报警代码
	float 		MinCellVoltage	;	//最低单体电压 
  	float 		MaxCellVoltage	;	//最高单体电压
	uint16_t    BMS_Ready       ;   //电池就绪 宁德时代1为就绪

	uint16_t	CommOkCnt		;	//通信成功次数
	uint16_t 	TimeOutMs		;	//超时计数
	uint16_t Alarm_In_Bit1;										//报警bit1
	uint16_t Alarm_In_Bit2;										//报警bit2
	uint16_t Alarm_In_Bit3;										//报警bit3
	uint16_t Alarm_In_Bit4;										//报警bit4 
	uint16_t Alarm_In_Bit1_Lock;										//报警bit1
	uint16_t Alarm_In_Bit2_Lock;										//报警bit2
	uint16_t Alarm_In_Bit3_Lock;										//报警bit3
	uint16_t Alarm_In_Bit4_Lock;										//报警bit4
	uint16_t Alarm_In_CATL;										//报警数字，只取最低位的一个报警
	uint16_t Alarm_In_CATL_Lock;
	
}BMS_Read_StDef;

/*BMS数据*/
typedef struct
{
	/*初始化参数*/
	BMS_InPutSource_EnumDef		BMS_Source		;

	/*输入*/
	void *						InPutData		;/*数据输入的指针，需要和输入源进行绑定使用*/
	/*输出*/
	BMS_Read_StDef				BMS_Read		;
	uint16_t					BMS_HeartbeatCnt;
	uint16_t					BMS_ErrorCode	;
}BMS_TypeDef;

/*函数声明*/
BMS_DATA_GLOBALS_EXT  void BMS_DataProcess(BMS_TypeDef *BMSData);

#endif
/*****************************************************END OF FILE*********************************************/
