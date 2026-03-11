/****************************Copyright (c)**********************************************                                       
**                  ╭--------------------------╮
**                  ︱   成都航发机器人有限公司 ︱
**                  ︱   http://www.hangfa.com 	︱                     
**					︱ 	  All Rights Reserved	︱
**                  ╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_PGVData.C
** 创 建 人: Master
** 描    述: PGV数据处理源文件，包含将通讯得到的原始数据放到对应结构体成员的函数
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0  
** 创建日期: 2025年4月21日
**--------------------------------------------------------------------------------------
***************************************************************************************/  
#define	PGV_DATA_GLOBALS

#include "drv_PGVData.h"
#include "EQPComm_Init.h"


/***************************************************************************************
** 函数名称: PGV_DataProcess_MCB_HF
** 功能描述: PGV数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void PGV_DataProcess_MCB_HF(PGV_TypeDef *PGVData)
{
	PGV_Read_StDef			*p_Read_Output	= &PGVData->PGV_Read;
	PGV_Temp_StDef			*p_Temp_Output	= &PGVData->PGV_Temp;
	EQP_HFPGVObj_StDef		*p_Read_Input	= PGVData->InPutData;

	/*数据更新*/
	if(p_Read_Input->PGV_IdentifyStatus == 0X01)
	{
		p_Read_Output->Status	= 1;
	}
	else
	{
		p_Read_Output->Status	= 0;
	}
	//将u16赋值给u8有一定的溢出风险
	p_Read_Output->Tag	 		= p_Read_Input->PGV_ControlCode;
	p_Temp_Output->X 			= p_Read_Input->PGV_Xpositon/10.0f;
	p_Temp_Output->Y 			= p_Read_Input->PGV_Ypositon/10.0f;
	p_Temp_Output->Z 			= p_Read_Input->PGV_Zpositon/100.0f;
	/*抽象层不需要的数据*/
//	p_Read_Output->Tag	 		= p_Read_Input->PGV_ReadComStatus;
//	p_Read_Output->X 			= p_Read_Input->PGV_ErrorCode;
//	p_Read_Output->Y 			= p_Read_Input->PGV_Syscnt;

}
/***************************************************************************************
** 函数名称: PGV_DataProcess_MCB_DH
** 功能描述: PGV数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void PGV_DataProcess_MCB_DH(PGV_TypeDef *PGVData)
{
	//暂未实现
}

/***************************************************************************************
** 函数名称: PGV_DataProcess_MCB_BJF
** 功能描述: PGV数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void PGV_DataProcess_MCB_BJF(PGV_TypeDef *PGVData)
{
	PGV_Read_StDef			*p_Read_Output	= &PGVData->PGV_Read;
	PGV_Temp_StDef			*p_Temp_Output	= &PGVData->PGV_Temp;
	EQP_BJF_PGVObj_StDef	*p_Read_Input	= PGVData->InPutData;

	/*数据更新*/
	if(p_Read_Input->State == 8 || p_Read_Input->State == 9)
	{
		p_Read_Output->Status 		= 1;
	}
	else
	{
		p_Read_Output->Status 		= 0;
	}
	
	p_Read_Output->Tag	 		= p_Read_Input->ControlCode;
	p_Temp_Output->X 			= p_Read_Input->Offset.X;
	p_Temp_Output->Y 			= p_Read_Input->Offset.Y;
	
	p_Temp_Output->Z 			= p_Read_Input->Anglevalue;
	if(p_Temp_Output->Z < 0.0f)
	{
		p_Temp_Output->Z = p_Temp_Output->Z+360.0f;
	}
	else if(p_Temp_Output->Z > 360.0f)
	{
		p_Temp_Output->Z = p_Temp_Output->Z - 360.0f;
	}
}
/***************************************************************************************
** 函数名称: PGV_DataProcess_MCB_BJF
** 功能描述: PGV数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
#include "drv_MergeToPGV.h"
void PGV_DataProcess_MCB_HF_Merge(PGV_TypeDef *PGVData)
{
	PGV_Read_StDef				*p_Read_Output	= &PGVData->PGV_Read;
	PGV_Temp_StDef				*p_Temp_Output	= &PGVData->PGV_Temp;
	_MergeToPGV_OutPut_TypeDef	*p_Read_Input	= PGVData->InPutData;

	/*数据更新*/
	p_Read_Output->Status 		= p_Read_Input->PGV_Sta;
	p_Read_Output->Tag	 		= p_Read_Input->PGV_Tag;
	p_Temp_Output->X 			= p_Read_Input->PGV_X;
	p_Temp_Output->Y 			= p_Read_Input->PGV_Y;
	p_Temp_Output->Z 			= p_Read_Input->PGV_Z_Projector;
}
/***************************************************************************************
** 函数名称: PGV_DataProcess_SerialExp
** 功能描述: PGV数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void PGV_DataProcess_SerialExp(PGV_TypeDef *PGVData)
{
	PGV_Read_StDef	*p_Read_Output	= &PGVData->PGV_Read;
	PGV_Temp_StDef			*p_Temp_Output	= &PGVData->PGV_Temp;
	PgvObjDef		*p_Read_Input	= PGVData->InPutData;

	/*数据更新*/
	p_Read_Output->Status 		= p_Read_Input->ReadSta;
	p_Read_Output->Tag	 		= p_Read_Input->Tag;
	p_Temp_Output->X 			= p_Read_Input->X_Value;
	p_Temp_Output->Y 			= p_Read_Input->Y_Value;
	p_Temp_Output->Z 			= p_Read_Input->Z_Value/100.0f;
}
/***************************************************************************************
** 函数名称: PGV_DataProcess_SerialExp_HF_Merge
** 功能描述: PGV数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void PGV_DataProcess_SerialExp_HF_Merge(PGV_TypeDef *PGVData)
{
	PGV_Read_StDef				*p_Read_Output	= &PGVData->PGV_Read;
	PGV_Temp_StDef				*p_Temp_Output	= &PGVData->PGV_Temp;
	_MergeToPGV_OutPut_TypeDef	*p_Read_Input	= PGVData->InPutData;

	/*数据更新*/
	p_Read_Output->Status 		= p_Read_Input->PGV_Sta;
	p_Read_Output->Tag	 		= p_Read_Input->PGV_Tag;
	p_Temp_Output->X 			= p_Read_Input->PGV_X;
	p_Temp_Output->Y 			= p_Read_Input->PGV_Y;
	p_Temp_Output->Z 			= p_Read_Input->PGV_Z_Projector;
}
/***************************************************************************************
** 函数名称: PGV_DataProcess
** 功能描述: PGV数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void PGV_DataProcess(PGV_TypeDef *PGVData)
{
	/*输入保护，如果没有输入指针 直接返回*/
	if(PGVData->InPutData == NULL)	{return;}

	switch (PGVData->PGV_Source)
	{
		default:
		case PGV_InPutSource_MCB :
		{
			/*区分传感器类型*/
			switch (PGVData->PGV_Sensor)
			{
				default:
				case PGV_Sensor_HF :
				{
					PGV_DataProcess_MCB_HF(PGVData);
				}break;
				case PGV_Sensor_DH :
				{
					PGV_DataProcess_MCB_DH(PGVData);
				}break;
				case PGV_Sensor_BJF :
				{
					PGV_DataProcess_MCB_BJF(PGVData);
				}break;
				case PGV_Sensor_HF_Merge :
				{
					PGV_DataProcess_MCB_HF_Merge(PGVData);
				}break;
			}
		}break;
		case PGV_InPutSource_SerialExp :
		case PGV_InPutSource_Assemly :
		{
			/*区分传感器类型*/
			switch (PGVData->PGV_Sensor)
			{
				/*不区分传感器类型，导航转接板会进行抽象处理*/
				default:
				case PGV_Sensor_HF :
				case PGV_Sensor_DH :
				case PGV_Sensor_BJF :
				{
					PGV_DataProcess_SerialExp(PGVData);
				}break;
				case PGV_Sensor_HF_Merge :
				{
					PGV_DataProcess_SerialExp_HF_Merge(PGVData);
				}break;
			}

		}break;
	}
	/*统一追加零偏*/
	{
		PGV_Read_StDef			*p_Read_Output	= &PGVData->PGV_Read;
		PGV_Temp_StDef			*p_Temp			= &PGVData->PGV_Temp;
		PGV_ZeroOffset_StDef	*p_ZeroOffset	= &PGVData->PGV_ZeroOffset;
		if(p_Read_Output->Status != 0X00)
		{
			p_Read_Output->X	=	p_Temp->X + p_ZeroOffset->X;
			p_Read_Output->Y	=	p_Temp->Y + p_ZeroOffset->Y;
			p_Read_Output->Z	=	p_Temp->Z + p_ZeroOffset->Z;
		}
		else
		{
			p_Read_Output->X	=	0;
			p_Read_Output->Y	=	0;
			p_Read_Output->Z	=	0;
		}
		
	}
	
	/*解算和正交轴的偏差*/
	if(PGVData->PGV_Sensor < PGV_Sensor_HF_Merge)
	{
		float temp = PGVData->PGV_Read.Z;
		
		if(temp<45.0f)
		{
			PGVData->PGV_Read.Z_Deviation = temp;
		}
		else if(temp<=135.0f && temp>45.0f)
		{
			PGVData->PGV_Read.Z_Deviation = temp - 90.0f;
		}
		else if(temp<=225.0f && temp>135.0f)
		{
			PGVData->PGV_Read.Z_Deviation = temp - 180.0f;
		}
		else if(temp<=315.0f && temp>225.0f)
		{
			PGVData->PGV_Read.Z_Deviation = temp - 270.0f;
		}
		else if(temp>315.0f)
		{
			PGVData->PGV_Read.Z_Deviation = temp - 360.0f;
		}
	}
	else
	{
		PGVData->PGV_Read.Z_Deviation = PGVData->PGV_Read.Z;
	}
}

/*****************************************************END OF FILE*********************************************/
