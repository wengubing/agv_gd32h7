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
#define	MAGNETIC_NAVIGATION_DATA_GLOBALS

#include "drv_MagneticNavigationData.h"
#include "EQPComm_Init.h"

/***************************************************************************************
** 函数名称: MN_DataProcess
** 功能描述: MN数据处理
** 参    数: *MNData--磁导航数据结构体类型指针
** 返 回 值: None
****************************************************************************************/
void MN_DataProcess_MCB(MN_TypeDef *MNData)
{
	MN_Read_StDef		*p_Read_Output	= &MNData->MN_Read;
	EQP_MagObj_StDef	*p_Read_Input	= MNData->InPutData;

	/*数据更新*/
	p_Read_Output->Strength 		= p_Read_Input->Intensity;
	p_Read_Output->Distance 		=  MNData->Diff * p_Read_Input->Distance;
	if(p_Read_Input->Value != 0X00)
	{
		p_Read_Output->Status	= 1;
	}
	else
	{
		p_Read_Output->Status	= 0;
	}
	/*暂时不处理 岔路口功能需要的数据*/
	p_Read_Output->L_or_R_Sta		= 0;
	p_Read_Output->DistanceLeft		= 0;
	p_Read_Output->DistanceRight	= 0;

}
/***************************************************************************************
** 函数名称: MN_DataProcess
** 功能描述: MN数据处理
** 参    数: *MNData--磁导航数据结构体类型指针
** 返 回 值: None
****************************************************************************************/
void MN_DataProcess_SerialExp(MN_TypeDef *MNData)
{
	MN_Read_StDef		*p_Read_Output	= &MNData->MN_Read;
	MagObj_StDef		*p_Read_Input	= MNData->InPutData;

	/*数据更新*/
	p_Read_Output->Strength 		= p_Read_Input->Intensity;
	p_Read_Output->Distance 		= MNData->Diff * p_Read_Input->Distance;
	if(p_Read_Input->Value != 0X00)
	{
		p_Read_Output->Status	= 1;
	}
	else
	{
		p_Read_Output->Status	= 0;
	}
	/*暂时不处理 岔路口功能需要的数据*/
	p_Read_Output->L_or_R_Sta		= 0;
	p_Read_Output->DistanceLeft		= 0;
	p_Read_Output->DistanceRight	= 0;
}


/***************************************************************************************
** 函数名称: MN_DataProcess
** 功能描述: MN数据处理
** 参    数: *MNData--磁导航数据结构体类型指针
** 返 回 值: None
****************************************************************************************/
void MN_DataProcess(MN_TypeDef *MNData)
{
	if(MNData->InPutData == NULL) {return;}
	switch (MNData->MN_Source)
	{
		default:
		case MN_InPutSource_MCB :
		{
			MN_DataProcess_MCB(MNData);
		}break;
		case MN_InPutSource_SerialExp :
		{
			MN_DataProcess_SerialExp(MNData);
		}break;
	}
}

/*****************************************************END OF FILE*********************************************/
