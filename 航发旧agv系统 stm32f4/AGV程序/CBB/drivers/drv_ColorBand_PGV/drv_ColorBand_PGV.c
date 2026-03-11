/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	drv_ColorBand_PGV.c
	* Version    :	v1.0
	* Author     :	MT
	* Date       : 2025-06-17
	* Description:
	* 说    明：目前只是适配的448转接板，倍加福PGV数据；还需持续吸收其他项目完善
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define COLOR_BAND_PGV_GLOBALS

#include "drv_ColorBand_PGV.h"
#include "ACC_Project.h"


/***************************************************************************************
** 函数名称: HF_RibbonDataAnalysis_FB
** 功能描述: 直连--自制色带传感器---前后
** 参    数: 色带PGV输入参数
** 返 回 值: None
****************************************************************************************/
void HF_RibbonDataAnalysis_FB (ColorBand_TypeDef *ColorBandData)
{
	ColorBand_output_StDef	*p_Output		= &ColorBandData->ColorBand_output;
	EQP_HFPGVObj_StDef		*p_Read_Input	= ColorBandData->InPutData;
	ColorBand_Data_StDef    *p_Data			= &ColorBandData->ColorBand_Deviation;
	
	/*数据更新*/
	p_Output->Status 		= p_Read_Input->PGV_IdentifyStatus;
	p_Output->Tag	 		= p_Read_Input->PGV_ControlCode;
	p_Output->X 			= p_Read_Input->PGV_Xpositon;
	p_Output->Y 			= p_Read_Input->PGV_Ypositon;
	p_Output->Z 			= p_Read_Input->PGV_Zpositon;


	/*色带偏差数据归一化处理*/
	{
		uint8_t Sta = p_Read_Input->PGV_IdentifyStatus;
		/*色带偏差数据归一化处理*/
		if(Sta == 2)
		{
			p_Data->Status = 1;
			p_Data->Distance = p_Output->Y *( ColorBandData->Color_diff);
		}
		else if(Sta == 1 || Sta == 3)
		{
			p_Data->Status = 1;
			p_Data->Distance = p_Output->X *( ColorBandData->Color_diff);
		}
		else
		{
			p_Data->Status = 0;
			p_Data->Distance = 0;
		}
	}

}


/***************************************************************************************
** 函数名称: HF_RibbonDataAnalysis_LR
** 功能描述: 直连--自制色带传感器---左右
** 参    数: PGV输入参数
** 返 回 值: None
****************************************************************************************/
void HF_RibbonDataAnalysis_LR (ColorBand_TypeDef *ColorBandData)
{
	ColorBand_output_StDef	*p_Output		= &ColorBandData->ColorBand_output;
	EQP_HFPGVObj_StDef		*p_Read_Input	= ColorBandData->InPutData;
	ColorBand_Data_StDef    *p_Data			= &ColorBandData->ColorBand_Deviation;
	
	/*数据更新*/
	p_Output->Status 		= p_Read_Input->PGV_IdentifyStatus;
	p_Output->Tag	 		= p_Read_Input->PGV_ControlCode;
	p_Output->X 			= p_Read_Input->PGV_Xpositon;
	p_Output->Y 			= p_Read_Input->PGV_Ypositon;
	p_Output->Z 			= p_Read_Input->PGV_Zpositon;


	/*色带偏差数据归一化处理*/
	{
		uint8_t Sta = p_Read_Input->PGV_IdentifyStatus;
		/*色带偏差数据归一化处理*/
		if(Sta == 2)
		{
			p_Data->Status = 1;
			p_Data->Distance = p_Output->Y *( ColorBandData->Color_diff);
		}
		else if(Sta == 1 || Sta == 3)
		{
			p_Data->Status = 1;
			p_Data->Distance = p_Output->X *( ColorBandData->Color_diff);
		}
		else
		{
			p_Data->Status = 0;
			p_Data->Distance = 0;
		}
	}
}


/***************************************************************************************
** 名称: 倍加福PGV坐标系
**							 ——————					   |X+
** 							|			|					   |
** 			通信线缆方向	——|			|	 	  ———————|—————— Y+
** 							|			|					   |
**							 ——————					   |
**说明：倍加福PGV识别色带，安装时PGV的X轴必须与色带平行，
**		这样才能保证能够正常识别到色带，此时Y坐标标识色带的偏差量
****************************************************************************************/

/***************************************************************************************
** 函数名称: BJF_RibbonDataAnalysis_FB
** 功能描述: 直连--倍加福色带传感器---前后
** 参    数: PGV输入参数
** 返 回 值: None
****************************************************************************************/
void BJF_RibbonDataAnalysis_FB (ColorBand_TypeDef *ColorBandData)
{
	ColorBand_output_StDef	*p_Output		= &ColorBandData->ColorBand_output;
	EQP_BJF_PGVObj_StDef	*p_Read_Input	= ColorBandData->InPutData;
	ColorBand_Data_StDef    *p_Data			= &ColorBandData->ColorBand_Deviation;

	/*数据更新*/
	p_Output->Status 		= p_Read_Input->State;
	p_Output->Tag	 		= p_Read_Input->ControlCode;
	p_Output->X 			= p_Read_Input->Xpositon;
	p_Output->Y 			= p_Read_Input->Ypositon;
	p_Output->Z 			= p_Read_Input->Anglevalue;


	/*色带偏差数据归一化处理*/
	{
		uint8_t Sta = p_Read_Input->State;
		/*色带偏差数据归一化处理*/
		if(Sta == 1)
		{
			p_Data->Status = 1;
			p_Data->Distance = p_Output->Y *( ColorBandData->Color_diff);
		}
		else if(Sta == 8 || Sta == 9)
		{
			p_Data->Status = 1;
			p_Data->Distance = -p_Output->X *( ColorBandData->Color_diff);
		}
		else
		{
			p_Data->Status = 0;
			p_Data->Distance = 0;
		}
	}

}

/***************************************************************************************
** 函数名称: BJF_RibbonDataAnalysis_LR
** 功能描述: 直连--倍加福色带传感器---左右
** 参    数: PGV输入参数
** 返 回 值: None
****************************************************************************************/
void BJF_RibbonDataAnalysis_LR (ColorBand_TypeDef *ColorBandData)
{
	ColorBand_output_StDef	*p_Output		= &ColorBandData->ColorBand_output;
	EQP_BJF_PGVObj_StDef	*p_Read_Input	= ColorBandData->InPutData;
	ColorBand_Data_StDef    *p_Data			= &ColorBandData->ColorBand_Deviation;

	/*数据更新*/
	p_Output->Status 		= p_Read_Input->State;
	p_Output->Tag	 		= p_Read_Input->ControlCode;
	p_Output->X 			= p_Read_Input->Xpositon;
	p_Output->Y 			= p_Read_Input->Ypositon;
	p_Output->Z 			= p_Read_Input->Anglevalue;


	/*色带偏差数据归一化处理*/
	{
		uint8_t Sta = p_Read_Input->State;
		/*色带偏差数据归一化处理*/
		if(Sta == 1)
		{
			p_Data->Status = 1;
			p_Data->Distance = p_Output->Y *( ColorBandData->Color_diff);
		}
		else if(Sta == 8 || Sta == 9)
		{
			p_Data->Status = 1;
			p_Data->Distance = -p_Output->X *( ColorBandData->Color_diff);
		}
		else
		{
			p_Data->Status = 0;
			p_Data->Distance = 0;
		}
	}

}



/***************************************************************************************
** 函数名称: DataSourceSelection
** 功能描述: 色带数据处理
** 参    数: PGV输入参数
** 返 回 值: None
****************************************************************************************/
void DataSourceSelection(ColorBand_TypeDef *ColorBandData)
{
	switch(ColorBandData->Color_Sensor)
	{
		// 自制PGV色带传感器---前后安装
		// 转接板PGV色带传感器---前后安装
		case HF_F_B_Sensor:
		case NCS_F_B_Sensor:
			{
				HF_RibbonDataAnalysis_FB(ColorBandData);	
			}break;
		// 自制PGV色带传感器---左右安装
		// 转接板PGV色带传感器---左右安装
		case HF_L_R_Sensor:
		case NCS_L_R_Sensor:
			{
				HF_RibbonDataAnalysis_LR(ColorBandData);	
			}break;

		// 倍加福PGV色带传感器---前后安装
		case BJF_F_B_Sensor:
			{
				BJF_RibbonDataAnalysis_FB(ColorBandData);	
			}break;
		// 倍加福PGV色带传感器---左右安装
		case BJF_L_R_Sensor:
			{
				BJF_RibbonDataAnalysis_LR(ColorBandData);
			}break;
	}
	
}



