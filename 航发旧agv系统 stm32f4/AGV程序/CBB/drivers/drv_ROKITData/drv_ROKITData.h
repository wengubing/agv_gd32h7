/****************************Copyright (c)**********************************************
**                  ╭--------------------------╮
**                  ︱   成都航发机器人有限公司 ︱
**                  ︱   http://www.hangfa.com 	︱                     
**					︱ 	  All Rights Reserved	︱
**                  ╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_ROKITData.c
** 创 建 人: Master
** 描    述: 力士乐导航传感器数据处理源文件，包含将通讯得到的原始数据放到对应结构体成员的函数
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.1
** 创建日期: 2025年4月21日
**--------------------------------------------------------------------------------------
***************************************************************************************/  
//保证以下所有内容只被同一源文件引用一次*/
#ifndef __DRV_ROKITDATA_H__
#define __DRV_ROKITDATA_H__

/*控制对象实例化只被指定文件编译时实例化一次*/
#ifdef  ROKIT_DATA_GLOBALS
	#define ROKIT_DATA_GLOBALS_EXT
#else
	#define ROKIT_DATA_GLOBALS_EXT  extern
#endif

/*自定义引用*/																
#include <stm32f4xx.h>
//工控机运行状态结果 定义
typedef enum
{
	ROKIT_RunSta_Invalid	 	= 0x00,	//0：无效
	ROKIT_RunSta_Initial 		= 0x01,	//1：初始化
	ROKIT_RunSta_Normal			= 0x02,	//2：正常
	ROKIT_RunSta_Stop			= 0x03,	//3：停止
	ROKIT_RunSta_Fault			= 0x04,	//4：故障
}ROKIT_RunSta_EnumDef;
//工控机故障代码结果 定义
typedef enum
{									/*	位定义：（0：未触发  1：触发）*/
	ROKIT_ErrCode_Null 				= 0x00,
	ROKIT_ErrCode_WriteFail 		= 0x01,	//0：配置写入失败
	ROKIT_ErrCode_ReadFail 			= 0x02,	//1：配置读取失败
	ROKIT_ErrCode_ModBusLinkFail	= 0x04,	//2：Modbus连接失败
	ROKIT_ErrCode_LoginFail			= 0x08,	//3：Locator登录失败
	ROKIT_ErrCode_LaunchFail		= 0x10,	//4：Locator启动失败
	ROKIT_ErrCode_SocketLinkFail	= 0x20,	//5：Locator Socket连接失败
	ROKIT_ErrCode_LocationFail		= 0x30,	//6：定位失败
}ROKIT_ErrCode_EnumDef;

//工控机故障代码结果 定义
typedef enum
{									/*	位定义：（0：未触发  1：触发）*/
	ROKIT_LocationSta_Invalid 			= 0x00,	//0：无效 
	ROKIT_LocationSta_Fail 				= 0x01,	//1：定位失败
	ROKIT_LocationSta_Underway			= 0x02,	//2：正在定位
	ROKIT_LocationStae_LowReliability	= 0x03,	//3：成功定位（低可靠性）
	ROKIT_LocationStae_HighReliability	= 0x04,	//4：成功定位（高可靠性）
}ROKIT_LocationSta_EnumDef;

//工控机控制命令 定义
typedef enum
{									/*	位定义：（0：未触发  1：触发）*/
	ROKIT_CMD_Default 			= 0x00,	//
	ROKIT_CMD_Reset  			= 0x01,	//0：系统复位（0：无效 1：执行）
	ROKIT_CMD_Launch 			= 0x02,	//1：系统启动
	ROKIT_CMD_Stop 				= 0x04,	//2：系统停止
	ROKIT_CMD_SYS_Relocate		= 0x08,	//3：以定位系统当前实时定位坐标启动重定位
	ROKIT_CMD_ModBus_Relocate	= 0x10,	//4：以Modbus给定重定位参数启动重定位
	ROKIT_CMD_Now_Relocate		= 0x20,	//5：以最近保存的重定位参数启动重定位
	ROKIT_CMD_Save_Relocate		= 0x40,	//6：执行保存参数实时坐标操作
	ROKIT_CMD_Layer_Switch		= 0x80,	//7：启动图层切换
}ROKIT_CMD_EnumDef;
/*在导航转接板 中的数据结构定义*/
typedef struct
{
	ROKIT_RunSta_EnumDef		RunSta;			//运行状态	
	ROKIT_LocationSta_EnumDef	LocationSta;	//定位状态
	ROKIT_ErrCode_EnumDef		ErrCode;		//故障代码	
	int32_t						LocationX;		//单位0.1mm
	int32_t						LocationY;		//单位0.1mm
	uint16_t					LocationZ;		//单位0.01°（0-36000）
	uint8_t						SignificantSta;	//有效状态
}ROKIT_Read_StDef;

typedef struct
{
	ROKIT_CMD_EnumDef	CMD;			//控制命令
	uint8_t				LayerNum;		//图层切换
	int32_t				SetX;			//重定位参数X 单位0.1mm	
	int32_t				SetY;			//
	uint16_t			SetZ;			//单位0.01°（0-36000）
}ROKIT_Write_StDef;


//数据输入类型
typedef enum
{
	ROKIT_InPutSource_MCB		= 0x00,	/*系统直连的ModBus*/
	ROKIT_InPutSource_SerialExp	= 0x01,	/*导航转接板的ModBus*/
}ROKIT_InPutSource_EnumDef;

/*软件定位零偏*/
typedef struct
{
	int32_t						Temp_X;		//单位0.1mm
	int32_t						Temp_Y;		//单位0.1mm
	uint16_t					Temp_Z;		//单位0.01°（0-36000）
}ROKIT_TempData_StDef;


/*软件定位零偏*/
typedef struct
{
	int32_t						Offset_X;		//单位0.1mm
	int32_t						Offset_Y;		//单位0.1mm
	int16_t						Offset_Z;		//单位0.01°（-18000-18000）
}ROKIT_OffSet_StDef;

/*软件定位零偏*/
typedef struct
{
	uint8_t						LowReliabilityEn : 1;		//低可靠仍然输出定位信息
}ROKIT_Fun_StDef;

/*力士乐导航数据*/
typedef struct
{
	/*初始化参数*/
	ROKIT_InPutSource_EnumDef	ROKIT_Source;
	ROKIT_Fun_StDef				ROKIT_Fun;
	ROKIT_OffSet_StDef			ROKIT_OffSet;

	/*中间过程量*/
	ROKIT_TempData_StDef		ROKIT_Temp;

	/*输入*/
	void *						InPutReadData	;/*数据输入的指针，需要和输入源进行绑定使用*/
	void *						OutPutWriteData	;/*数据输入的指针，需要和输入源进行绑定使用*/

	/*输出*/
	ROKIT_Read_StDef			ROKIT_Read;
	ROKIT_Write_StDef			ROKIT_Write;
}
ROKIT_TypeDef;	

/*函数声明*/
ROKIT_DATA_GLOBALS_EXT  void ROKIT_DataProcess(ROKIT_TypeDef *ROKITData);

#endif
/*****************************************************END OF FILE*********************************************/
