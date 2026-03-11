/****************************Copyright (c)**********************************************
**                  ╭--------------------------╮
**                  ︱   成都航发机器人有限公司 ︱
**                  ︱   http://www.hangfa.com 	︱                     
**					︱ 	  All Rights Reserved	︱
**                  ╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_MagneticNavigationData.c
** 创 建 人: Master  
** 描    述: 磁导航传感器数据处理源文件，包含将通讯得到的原始数据放到对应结构体成员的函数
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0  
** 创建日期: 2025年4月21日
**--------------------------------------------------------------------------------------
***************************************************************************************/    
//保证以下所有内容只被同一源文件引用一次*/
#ifndef __DRV_MAGNETIC_NAVIGATION_DATA_H__
#define __DRV_MAGNETIC_NAVIGATION_DATA_H__

/*控制对象实例化只被指定文件编译时实例化一次*/
#ifdef  MAGNETIC_NAVIGATION_DATA_GLOBALS
	#define MAGNETIC_NAVIGATION_DATA_GLOBALS_EXT
#else
	#define MAGNETIC_NAVIGATION_DATA_GLOBALS_EXT  extern
#endif

/*自定义引用*/																
#include <stm32f4xx.h>

//数据输入类型
typedef enum
{
	MN_InPutSource_MCB			= 0x00,	/*系统直连的ModBus*/
	MN_InPutSource_SerialExp	= 0x01,	/*导航转接板的ModBus*/
}MN_InPutSource_EnumDef;

/*磁导航数据*/
typedef struct
{
	/*常用标准数据*/
	int16_t					Strength			;/*中间磁条 磁场强度*/
	int16_t					Distance			;/*左侧磁条和传感器中心偏差距离 单位：mm*/
	uint8_t					Status			: 1	;/*读状态 0-没有识别 1-识别到磁条	*/
	/*岔路口功能需要的数据，其他工况无需考虑*/
	uint8_t					L_or_R_Sta		: 3	;/*磁条识别结果按位编码 识别到磁条后必然不为0
													0-bit 左侧有磁条 1-bit 中间侧有磁条 2bit 右侧有磁条*/
	int16_t					DistanceLeft		;/*左侧磁条和传感器中心偏差距离 	单位：mm*/
	int16_t					DistanceRight		;/*右侧侧磁条和传感器中心偏差距离 单位：mm*/

}MN_Read_StDef;

typedef struct
{
	/*初始化参数*/
	int8_t					Diff		;/*安装方向 只能填写 1或者-1*/
	MN_InPutSource_EnumDef	MN_Source	;
	void *					InPutData	;/*数据输入的指针，需要和输入源进行绑定使用*/

	MN_Read_StDef			MN_Read		;/**/

}MN_TypeDef;

/*函数声明*/
MAGNETIC_NAVIGATION_DATA_GLOBALS_EXT  void MN_DataProcess(MN_TypeDef *MNData);

#endif
/*****************************************************END OF FILE*********************************************/
