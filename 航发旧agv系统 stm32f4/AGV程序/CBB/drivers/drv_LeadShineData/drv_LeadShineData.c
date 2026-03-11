/****************************Copyright (c)**********************************************                                       
**                  ╭--------------------------╮
**                  ︱   成都航发机器人有限公司 ︱
**                  ︱   http://www.hangfa.com 	︱                     
**					︱ 	  All Rights Reserved	︱
**                  ╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_LeadShineData.c
** 创 建 人: 何维  
** 描    述: 雷赛升降模块数据处理源文件，包含将通讯得到的原始数据放到对应结构体成员的函数
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年1月22日       
**--------------------------------------------------------------------------------------
***************************************************************************************/  
#include "..\..\CBB\user\drv_LeadShineData\drv_LeadShineData.h"

/***************************************************************************************
** 函数名称: LS_RawDataProcess
** 功能描述: LS原始数据处理
** 参    数: *LSData--雷赛升降模块数据结构体类型指针
** 返 回 值: None
****************************************************************************************/
void LS_RawDataProcess(LS_TypeDef *LSData)
{
	LSData->LS_RawAlarmInformation = LSData->p_DataInput[InputOffset_AlarmInformation];
	LSData->LS_RawIOInputState = LSData->p_DataInput[InputOffset_IOInputState];
	LSData->LS_RawPosition = (s32)((LSData->p_DataInput[InputOffset_PositionH] << 16) + LSData->p_DataInput[InputOffset_PositionL]);
}

/***************************************************************************************
** 函数名称: LS_DataProcess
** 功能描述: LS数据处理
** 参    数: *LSData--雷赛升降模块数据结构体类型指针
** 返 回 值: None
****************************************************************************************/
void LS_DataProcess(LS_TypeDef *LSData)
{
	LS_RawDataProcess(LSData);
	
	LSData->LS_OutputTx[OutOffset_CmdEnable] = LSData->LS_InputCmdEnable;
	LSData->LS_OutputTx[OutOffset_CmdRunStop] = LSData->LS_InputCmdRunStop;
	LSData->LS_OutputTx[OutOffset_CmdOperateionMode] = LSData->LS_InputCmdOperateionMode;
	LSData->LS_OutputTx[OutOffset_CmdSystemReset] = LSData->LS_InputCmdSystemReset;
	
	LSData->LS_OutputTx[OutOffset_CmdPositionL] = (u16)LSData->LS_InputCmdPosition;
	LSData->LS_OutputTx[OutOffset_CmdPositionH] = (u16)(LSData->LS_InputCmdPosition >> 16);
	LSData->LS_OutputTx[OutOffset_CmdSpeedValue] = LSData->LS_InputCmdSpeedValue;
	LSData->LS_OutputTx[OutOffset_CmdReserve] = 0x00;
	
	LSData->LS_OutputAlarmInformation = LSData->LS_RawAlarmInformation;
	LSData->LS_OutputIOInputState = LSData->LS_RawIOInputState;
	LSData->LS_OutputPosition = LSData->LS_RawPosition;
	
}

/*****************************************************END OF FILE*********************************************/
