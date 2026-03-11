/****************************Copyright (c)**********************************************                                       
**                  ╭--------------------------╮
**                  ︱   成都航发机器人有限公司 ︱
**                  ︱   http://www.hangfa.com 	︱                     
**					︱ 	  All Rights Reserved	︱
**                  ╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_LeadShineData.h
** 创 建 人: 何维  
** 描    述: 雷赛升降模块数据处理头文件，包含所需数据结构体和函数声明
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年1月22日				
**--------------------------------------------------------------------------------------
***************************************************************************************/  
//保证以下所有内容只被同一源文件引用一次*/
#ifndef __DRV_LEAD_SHINE_DATA_H__
#define __DRV_LEAD_SHINE_DATA_H__

/*控制对象实例化只被指定文件编译时实例化一次*/
#ifdef  LEAD_SHINE_DATA_GLOBALS
	#define LEAD_SHINE_DATA_GLOBALS_EXT
#else
	#define LEAD_SHINE_DATA_GLOBALS_EXT  extern
#endif

/*自定义引用*/																
#include <stm32f4xx.h>

/*雷赛升降模块原始输入数据地址偏移定义*/
typedef enum
{
	InputOffset_AlarmInformation = 0,					/*报警状态地址偏移量*/
	InputOffset_IOInputState,							/*IO输入状态地址偏移量*/
	InputOffset_PositionL,								/*位置反馈低16位地址偏移量*/
	InputOffset_PositionH,								/*位置反馈高16位地址偏移量*/
	
}LS_DATA_INPUTOFFSET;

/*雷赛升降模块输出数据地址偏移定义*/
typedef enum
{
	OutOffset_CmdEnable = 0,							/*使能指令地址偏移量*/
	OutOffset_CmdRunStop,								/*运行控制指令地址偏移量*/
	OutOffset_CmdOperateionMode,						/*运行模式指令地址偏移量*/
	OutOffset_CmdSystemReset,							/*软件复位指令地址偏移量*/
	OutOffset_CmdPositionL,								/*电机目标位置指令低16位地址偏移量*/
	OutOffset_CmdPositionH,								/*电机目标位置指令高16位地址偏移量*/
	OutOffset_CmdSpeedValue,							/*电机运行速度指令地址偏移量*/
	OutOffset_CmdReserve,								/*保留地址偏移量*/
	
}LS_DATA_OUTPUTOFFSET;

/*雷赛升降模块数据*/
typedef struct
{
	/*初始化参数*/
	u16	*p_DataInput;						/*数据输入指针*/
	
	/*实时输入*/
	u16 LS_InputCmdEnable,					/*使能指令：使能--0x83  失能--0x00*/
		LS_InputCmdRunStop,					/*运行控制指令：运行--0x10  急停：0x40*/
		LS_InputCmdOperateionMode,			/*运行模式指令：绝对位置模式--0x01
															禁止发速度模式：0x02
															回零模式：0x03*/
		LS_InputCmdSystemReset;				/*软件复位指令：软件复位：0x6666  误动作：0x0000*/
	
	s16 LS_InputCmdSpeedValue;				/*电机运行速度指令，单位rpm*/
	
	s32 LS_InputCmdPosition;				/*电机目标位置指令*/
	
	/*过程参数*/
	u16 LS_RawAlarmInformation,				/*报警状态原始值，只关心有无报警，非0为有报警*/
		LS_RawIOInputState;					/*IO输入状态原始值，Bit2：使能状态，Bit4：限位开关状态*/
	s32 LS_RawPosition;						/*位置反馈原始值*/
	
	/*输出参数*/
	u16 LS_OutputCmdEnable,					/*使能指令：使能--0x83  失能--0x00*/
		LS_OutputCmdRunStop,				/*运行控制指令：运行--0x10  急停：0x40*/
		LS_OututCmdOperateionMode,			/*运行模式指令：绝对位置模式--0x01
															禁止发速度模式：0x02
															回零模式：0x03*/
		LS_OutputCmdSystemReset;			/*软件复位指令：软件复位：0x6666  误动作：0x0000*/
	
	s16 LS_OutputCmdPositionL,				/*电机目标位置指令低16位*/
		LS_OutputCmdPositionH,				/*电机目标位置指令高16位*/
		LS_OutputCmdSpeedValue,				/*电机运行速度指令，单位rpm*/
		LS_OutputCmdReserve;				/*保留*/
	
	u16 LS_OutputAlarmInformation,			/*报警状态，只关心有无报警，非0为有报警*/
		LS_OutputIOInputState;				/*IO输入状态，Bit2：使能状态，Bit4：限位开关状态*/
		
	s32 LS_OutputPosition;					/*位置反馈*/

	u16 LS_OutputTx[8];
}LS_TypeDef;	

/*函数声明*/
LEAD_SHINE_DATA_GLOBALS_EXT  void LS_DataProcess(LS_TypeDef *LSData);

#endif
/*****************************************************END OF FILE*********************************************/
