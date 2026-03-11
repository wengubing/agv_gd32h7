/****************************Copyright (c)**********************************************
**                  ╭--------------------------╮
**                  ︱   成都航发机器人有限公司 ︱
**                  ︱   http://www.hangfa.com 	︱                     
**					︱ 	  All Rights Reserved	︱
**                  ╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_RFIDData.C
** 创 建 人: master
** 描    述: RFID数据处理源文件，包含将通讯得到的原始数据放到对应结构体成员的函数
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0  
** 创建日期: 2025年4月21日
**--------------------------------------------------------------------------------------
***************************************************************************************/   
//保证以下所有内容只被同一源文件引用一次*/
#ifndef __DRV_RFID_DATA_H__
#define __DRV_RFID_DATA_H__

/*控制对象实例化只被指定文件编译时实例化一次*/
#ifdef  RFID_DATA_GLOBALS
	#define RFID_DATA_GLOBALS_EXT
#else
	#define RFID_DATA_GLOBALS_EXT  extern
#endif

/*自定义引用*/																
#include <stm32f4xx.h>

//数据输入类型
typedef enum
{
	RFID_InPutSource_MCB		= 0x00,	/*系统直连的ModBus*/
	RFID_InPutSource_SerialExp	= 0x01,	/*导航转接板的ModBus*/
}RFID_InPutSource_EnumDef;

typedef struct
{
	uint8_t		Status			: 1	;/*读状态 0-没有识别 1-识别到RFID	*/
	uint8_t		RSSIStatus		: 7	;/*读卡信号强度指示	0-7*/
	uint16_t	Tag					;/*标签值			*/
	uint16_t	LastTag				;/*历史标签值		*/
}RFID_Read_StDef;

/*RFID数据*/
typedef struct
{
	/*初始化参数*/
	RFID_InPutSource_EnumDef	RFID_Source	;
	/*输入*/
	void *						InPutData	;/*数据输入的指针，需要和输入源进行绑定使用*/
	/*输出*/
	RFID_Read_StDef				RFID_Read	;
}RFID_TypeDef;

/*函数声明*/
RFID_DATA_GLOBALS_EXT  void RFID_DataProcess(RFID_TypeDef *RfidData);

#endif
/*****************************************************END OF FILE*********************************************/
