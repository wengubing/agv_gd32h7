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
#define	ROKIT_DATA_GLOBALS

#include "drv_ROKITData.h"
#include "EQPComm_Init.h"


/***********************************************************
** 函数名称:	ROKIT_DataProcess_SerialExp
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void ROKIT_DataProcess_SerialExp(ROKIT_TypeDef *ROKITData)
{
	/*读回来的数据*/
	{
		ROKIT_Read_StDef 		*p_Read_Output	= &ROKITData->ROKIT_Read;
		ROKIT_TempData_StDef	*p_Read_Temp	= &ROKITData->ROKIT_Temp;
		IPC_Read_StDef 			*p_Read_Input 	= ROKITData->InPutReadData;
		int32_t					Temp_z 			= p_Read_Input->LocationZ;

		p_Read_Output->RunSta 		= (ROKIT_RunSta_EnumDef)		p_Read_Input->RunSta;
		p_Read_Output->LocationSta 	= (ROKIT_LocationSta_EnumDef)	p_Read_Input->LocationSta;
		p_Read_Output->ErrCode 		= (ROKIT_ErrCode_EnumDef)		p_Read_Input->ErrCode;

//		p_Read_Output->LocationX 	= 								p_Read_Input->LocationX;
//		p_Read_Output->LocationY 	= 								p_Read_Input->LocationY;
//		p_Read_Output->LocationZ 	= 								p_Read_Input->LocationZ;
		
		p_Read_Temp->Temp_X 	=	p_Read_Input->LocationX;
		p_Read_Temp->Temp_Y 	=	p_Read_Input->LocationY;
		/*中间件 原始z的范围是-18000-18000；抽象数据池的输出是0-36000，此处需要加上零偏*/
		if(Temp_z < 0)
		{
			p_Read_Temp->Temp_Z	=  36000+Temp_z;
		}
		else
		{
			p_Read_Temp->Temp_Z	=  Temp_z;
		}
		
		/*抽象层面不需要的原始数据*/
	}
	/*写到从机的数据*/
	{
		IPC_Write_StDef			*p_Write_Output	= ROKITData->OutPutWriteData;
		ROKIT_Write_StDef 		*p_Write_Input	= &ROKITData->ROKIT_Write;

		p_Write_Output->CMD				= p_Write_Input->CMD;
		p_Write_Output->LayerNum		= p_Write_Input->LayerNum;
		p_Write_Output->SetX			= p_Write_Input->SetX;
		p_Write_Output->SetY			= p_Write_Input->SetY;
		p_Write_Output->SetZ			= p_Write_Input->SetZ;
		/*抽象层面没有的数据*/
	}
}

/***********************************************************
** 函数名称:	ROKIT_DataProcess_MCB
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void ROKIT_DataProcess_MCB(ROKIT_TypeDef *ROKITData)
{
	/*读回来的数据*/
	{
		ROKIT_Read_StDef 		*p_Read_Output	= &ROKITData->ROKIT_Read;
		ROKIT_TempData_StDef	*p_Read_Temp	= &ROKITData->ROKIT_Temp;
		EQP_IPC_Read_Obj_StDef 	*p_Read_Input 	= ROKITData->InPutReadData;
		int32_t					Temp_z 			= p_Read_Temp->Temp_Z;

		p_Read_Output->RunSta 		= (ROKIT_RunSta_EnumDef)		p_Read_Input->RunSta;
		p_Read_Output->LocationSta 	= (ROKIT_LocationSta_EnumDef)	p_Read_Input->LocationSta;
		p_Read_Output->ErrCode 		= (ROKIT_ErrCode_EnumDef)		p_Read_Input->ErrCode;

//		p_Read_Output->LocationX 	= (p_Read_Input->LocationX_H<<16)+p_Read_Input->LocationX_L;
//		p_Read_Output->LocationY 	= (p_Read_Input->LocationY_H<<16)+p_Read_Input->LocationY_L;
//		p_Read_Output->LocationZ 	= 								p_Read_Input->LocationZ;

		p_Read_Temp->Temp_X 		= (p_Read_Input->LocationX_H<<16)+p_Read_Input->LocationX_L;
		p_Read_Temp->Temp_Y 		= (p_Read_Input->LocationY_H<<16)+p_Read_Input->LocationY_L;
		
		if(Temp_z < 0)
		{
			p_Read_Temp->Temp_Z 		=  36000+Temp_z;
		}
		else
		{
			p_Read_Temp->Temp_Z 		=  Temp_z;
		}
		
		/*抽象层面不需要的原始数据*/
		//p_Read_Output->LocationY 	= 								p_Read_Input->CMD1_Tick;
		//p_Read_Output->LocationZ 	= 								p_Read_Input->SyS_ms;
	}
	/*写到从机的数据*/
	{
		EQP_IPC_Write_Obj_StDef	*p_Write_Output	= ROKITData->OutPutWriteData;
		ROKIT_Write_StDef 		*p_Write_Input	= &ROKITData->ROKIT_Write;

		p_Write_Output->CMD1			= p_Write_Input->CMD;
		p_Write_Output->SetX_H			= (p_Write_Input->SetX&0XFFFF0000)>>16;
		p_Write_Output->SetX_L			= (p_Write_Input->SetX&0X0000FFFF);
		p_Write_Output->SetX_H			= (p_Write_Input->SetY&0XFFFF0000)>>16;
		p_Write_Output->SetX_L			= (p_Write_Input->SetY&0X0000FFFF);
		p_Write_Output->SetZ			= p_Write_Input->SetZ;
		p_Write_Output->LayerNum		= p_Write_Input->LayerNum;

		/*抽象层面没有的数据*/
		p_Write_Output->CalBackSyS_ms	= g_Sys_1ms_Counter; /*直接将系统心跳填入*/

		//p_Write_Output->CMD2	= p_Write_Input->CMD;
		//p_Write_Output->SetNC	= p_Write_Input->CMD;

	}
}


/***********************************************************
** 函数名称:	ROKIT_DataProcess
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void ROKIT_DataProcess(ROKIT_TypeDef *ROKITData)
{
	switch (ROKITData->ROKIT_Source)
	{
		default:
		case ROKIT_InPutSource_MCB :
		{
			ROKIT_DataProcess_MCB(ROKITData);
		}break;
		case ROKIT_InPutSource_SerialExp :
		{
			ROKIT_DataProcess_SerialExp(ROKITData);
		}break;
	}
	/*统一增加 定位有效的标识*/
	{
		ROKIT_Read_StDef *p_Read_Output	= &ROKITData->ROKIT_Read;
		if(	  p_Read_Output->ErrCode 		== ROKIT_ErrCode_Null
			&&p_Read_Output->LocationSta 	== ROKIT_LocationStae_HighReliability
			&&p_Read_Output->RunSta 		== ROKIT_RunSta_Normal)
		{
			ROKITData->ROKIT_Read.SignificantSta = 1;
		}
		else
		{
			ROKITData->ROKIT_Read.SignificantSta = 0;
		}
	}
	/*统一增加零偏设置*/
	{
		uint8_t Sta = 0;
		ROKIT_Read_StDef 		*p_Read_Output	= &ROKITData->ROKIT_Read;
		if(ROKITData->ROKIT_Read.LocationSta == ROKIT_LocationStae_HighReliability)
		{
			Sta = 0X01;
		}
		else if(ROKITData->ROKIT_Fun.LowReliabilityEn == 1
			&&ROKITData->ROKIT_Read.LocationSta == ROKIT_LocationStae_LowReliability)
		{
			Sta = 0X02;
		}
		if(Sta != 0X00)
		{
			ROKIT_TempData_StDef	*p_Read_Temp	= &ROKITData->ROKIT_Temp;
			ROKIT_OffSet_StDef		*p_Offset		= &ROKITData->ROKIT_OffSet;
			int32_t					Temp_z 			= p_Read_Temp->Temp_Z + p_Offset->Offset_Z;
			p_Read_Output->LocationX	=	p_Read_Temp->Temp_X + p_Offset->Offset_X;
			p_Read_Output->LocationY	=	p_Read_Temp->Temp_Y + p_Offset->Offset_Y;
			if(Temp_z < 0)
			{
				p_Read_Output->LocationZ	=	Temp_z+36000;
			}
			else if(Temp_z >= 36000)
			{
				p_Read_Output->LocationZ	=	Temp_z-36000;
			}
			else
			{
				p_Read_Output->LocationZ	=	Temp_z;
			}
		}
		else
		{
			p_Read_Output->LocationX	=	0;
			p_Read_Output->LocationY	=	0;
			p_Read_Output->LocationZ	=	0;
		}
	}
}
/*****************************************************END OF FILE*********************************************/
