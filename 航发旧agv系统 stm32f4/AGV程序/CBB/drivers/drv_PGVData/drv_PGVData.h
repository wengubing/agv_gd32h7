/****************************Copyright (c)**********************************************                                       
**                  ╭--------------------------╮
**                  ︱   成都航发机器人有限公司 ︱
**                  ︱   http://www.hangfa.com 	︱                     
**					︱ 	  All Rights Reserved	︱
**                  ╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_PGVData.h
** 创 建 人: Master
** 描    述: PGV数据处理头文件，包含所需数据结构体和函数声明
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0
** 创建日期: 2025年4月21日
**--------------------------------------------------------------------------------------
***************************************************************************************/  
//保证以下所有内容只被同一源文件引用一次*/
#ifndef __DRV_PGV_DATA_H__
#define __DRV_PGV_DATA_H__

/*控制对象实例化只被指定文件编译时实例化一次*/
#ifdef  PGV_DATA_GLOBALS
	#define PGV_DATA_GLOBALS_EXT
#else
	#define PGV_DATA_GLOBALS_EXT  extern
#endif

/*自定义引用*/																
#include <stm32f4xx.h>

//数据输入类型
typedef enum
{
	PGV_InPutSource_MCB			= 0x00,		/*系统直连的ModBus*/
	PGV_InPutSource_SerialExp	= 0x01,		/*导航转接板的ModBus*/
	PGV_InPutSource_Assemly		= 0x02,		/*联动通讯的ModBus*/
}PGV_InPutSource_EnumDef;

//数据输入类型
typedef enum
{
	PGV_Sensor_HF			= 0x00,	/*自制PGV	车体坐标*/
	PGV_Sensor_DH			= 0x01,	/*大华PGV	世界坐标*/
	PGV_Sensor_BJF			= 0x02,	/*倍加福PGV	世界坐标*/
	PGV_Sensor_HF_Merge		= 0x03,	/*两个PGV融合的  PGV数据*/
}PGV_Sensor_EnumDef;

//零偏
typedef struct
{
	int16_t		X					;/*X 零偏  1mm*/
	int16_t		Y					;/*Y 零偏  1mm*/
	float		Z					;/*z 零偏  1° 0-360*/
}PGV_ZeroOffset_StDef;

//缓存
typedef struct
{
	int16_t		X					;/*X   1mm*/
	int16_t		Y					;/*Y   1mm*/
	float		Z					;/*z   1° 0-360*/
}PGV_Temp_StDef;

typedef struct
{
	uint8_t		Status			: 1	;/*读状态 0-没有识别 1-识别到PGV	*/
	uint8_t		Tag					;/*标签结果*/
	int16_t		X					;/*X 方向定位数据，单位1mm*/
	int16_t		Y					;/*Y 方向定位数据，单位1mm*/
	float		Z					;/*z 方向定位数据，单位1° 0-360*/
	float		Z_Deviation			;/*z 和正交轴的偏差值，单位1° -45-45*/
}PGV_Read_StDef;

/*PGV数据*/
typedef struct
{
	/*初始化参数*/
	PGV_InPutSource_EnumDef		PGV_Source		;
	PGV_Sensor_EnumDef			PGV_Sensor		;
	PGV_ZeroOffset_StDef		PGV_ZeroOffset	;
	/*输入*/
	void *						InPutData		;/*数据输入的指针，需要和输入源进行绑定使用*/
	
	PGV_Temp_StDef				PGV_Temp		;
	/*输出*/
	PGV_Read_StDef				PGV_Read		;
	uint16_t					PGV_HeartbeatCnt;
	uint16_t					PGV_ErrorCode	;
}PGV_TypeDef;

/*函数声明*/
PGV_DATA_GLOBALS_EXT  void PGV_DataProcess(PGV_TypeDef *PGVData);

#endif
/*****************************************************END OF FILE*********************************************/
