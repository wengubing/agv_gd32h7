/****************************Copyright (c)**********************************************                                       
**                  ╭--------------------------╮
**                  ︱   成都航发机器人有限公司 ︱
**                  ︱   http://www.hangfa.com 	︱                     
**					︱ 	  All Rights Reserved	︱
**                  ╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_BMSData.C
** 创 建 人: Master
** 描    述: BMS数据处理源文件，包含将通讯得到的原始数据放到对应结构体成员的函数
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0  
** 创建日期: 2025年4月21日
**--------------------------------------------------------------------------------------
***************************************************************************************/  
#define	BMS_DATA_GLOBALS

#include "ACC_Project.h"
#include "ACC_Init.h"
#include "drv_BMSData.h"
#include "EQPComm_Init.h"

//电量指示阀值定义:
float Vol[10]					=	{61.52f,63.10f,64.80f,66.60f,67.32f,68.10f,68.82f,69.60f,69.90f,70.20f};	//自定义电池表
float Vol_TN_48V [10] 			=	{44.00f,45.40f,46.32f,47.08f,47.67f,48.40f,49.04f,49.60f,50.16f,50.68f};	//天能48V电池电压表
float Vol_TN_72V[10]			=	{66.00f,68.10f,69.48f,70.62f,71.64f,72.60f,73.56f,74.40f,75.24f,76.02f};	//天能72V电池电压表
float Vol_HBK_48V[11]			=	{43.51f,45.62f,46.43f,47.10f,47.60f,48.10f,48.60f,49.09f,49.48f,49.76f,49.90f}; 	//荷贝克电池电压表 48V
float Vol_HBK_72V[10] 			=	{68.10f,69.30f,70.26f,71.10f,71.88f,72.60f,73.26f,73.86f,74.28f,74.50f};	//荷贝克电池电压表 72V
float Vol_LN_48V [10] 			=	{38.00f,40.00f,42.00f,44.10f,45.10f,46.00f,47.00f,48.00f,48.90f,49.90f};	//力朗电池48V（无BMS）

//只有S2阶段的自动充电 SOC表
float Vol_Charge_HBK_48V[11]	=	{45.62f,46.43f,48.84f,49.40f,51.92f,53.40f,54.90f,55.50f,56.52f,57.80f,59.80f}; 	//荷贝克电池电压表 48V
//有S2 S3阶段的自动充电 SOC表
//float Vol_Charge_HBK_48V[10]	=	{45.62f,46.43f,48.84f,49.40f,51.92f,53.40f,54.90f,56.52f,57.60f,60.00f,63.00f}; 	//荷贝克电池电压表 48V

/***************************************************************************************
** 函数名称: BMS_DataProcess_LeadBat
** 功能描述: BMS数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
DataFilter_TypeDef	LeadBat_DataFilter;
DataFilter_TypeDef	LeadBat_DataFilter_SecondOrder;
void BMS_DataProcess_LeadBat(BMS_TypeDef *BMSData)
{
	BMS_Read_StDef				*p_Read_Output	= &BMSData->BMS_Read;
	//static DataFilter_TypeDef	LeadBat_DataFilter;
	uint8_t i 	= 0;
	static float	Vol = 0;
	uint8_t res = 0;
	{
		float *pVolChargeTh = Vol_TN_48V;	/*默认使用天能48V电池*/
		if(g_Para.BatteryType == ACC_LeadBatteries_TN48V)
		{
			pVolChargeTh = Vol_TN_48V;
		}
		else if(g_Para.BatteryType == ACC_LeadBatteries_HBK48V)
		{
			if(g_DHR_AGVInformation[Para_Index_AGV_ChargingControl] != 0x00)
			{
				pVolChargeTh = Vol_Charge_HBK_48V;
			}
			else
			{
				pVolChargeTh = Vol_HBK_48V;
			}
			
		}
		else if (g_Para.BatteryType == ACC_LeadBatteries_TN72V)
		{
			pVolChargeTh = Vol_TN_72V;
		}
		/*其他类型铅酸电池未验证*/
		else{}

		/*隔200ms的窗口滑动 取窗口中的最大值*/
		{
			static uint16_t LeadBat_Ms = 0;
			static uint16_t LeadBatSecondOrder_Ms = 0;
			if(LeadBat_Ms == 0)
			{
				LeadBat_DataFilter.in_FifoLength = 10;
				LeadBat_DataFilter.in_FilterMode = DataFilter_Mode_Maximum;
			}
			
			if(g_Sys_1ms_Counter > LeadBat_Ms + 1000)
			{
				LeadBat_Ms = g_Sys_1ms_Counter;
				LeadBat_DataFilter.in_RealTimeValue = g_ADFucs.SysCtrlVoltage.Fiter.out_FilterValue;
				LeadBat_DataFilter.out_FilterValue = DataFilter_Main(&LeadBat_DataFilter);
			}
			if(LeadBatSecondOrder_Ms == 0)
			{
				LeadBat_DataFilter_SecondOrder.in_FifoLength = 10;
				LeadBat_DataFilter_SecondOrder.in_FilterMode = DataFilter_Mode_Maximum;
			}
			if(g_Sys_1ms_Counter > LeadBatSecondOrder_Ms + 10000)
			{
				LeadBatSecondOrder_Ms = g_Sys_1ms_Counter;
				LeadBat_DataFilter_SecondOrder.in_RealTimeValue = LeadBat_DataFilter.out_FilterValue;
				LeadBat_DataFilter_SecondOrder.out_FilterValue = DataFilter_Main(&LeadBat_DataFilter_SecondOrder);
			}
		}
		//启动前10秒 强制SOC 100%
		if(g_Sys_1ms_Counter < 10000)
		{
			Vol = pVolChargeTh[9];//0.1V
		}
		//如果在静止状态 才更新电量
		else if(AccData.ModeSwitchSMT == AsbMode_Still)
		{
			Vol = (LeadBat_DataFilter.out_FilterValue) / 10.0f;//0.1V
		}
		else
		{
			//无需更新电量
		}
		
		if(Vol < pVolChargeTh[0])
		{//最小电压
			res = 0;
		}
		else if(Vol >= pVolChargeTh[10])
		{//最大电压
			res = 100;
		}
		else
		{
			for(i=0;i<10;i++)
			{
				if(Vol >= pVolChargeTh[i] && Vol < pVolChargeTh[i+1])
				{
					res = (i)*10+((Vol-pVolChargeTh[i])/(pVolChargeTh[i+1]-pVolChargeTh[i]))*10.0f;
					break;
				}
			}
		}
	}
	p_Read_Output->Voltage = Vol;
	//p_Read_Output->Current = 0;
	p_Read_Output->SOC = res;
	p_Read_Output->SOH = 100;
	p_Read_Output->Alarm_Code = 0;
	if(   g_Para.BatteryType == ACC_LeadBatteries_TN48V
		||g_Para.BatteryType == ACC_LeadBatteries_HBK48V
		||g_Para.BatteryType == ACC_LeadBatteries_TN72V)
	{
		p_Read_Output->MinCellVoltage = Vol / 16.0f;
		p_Read_Output->MaxCellVoltage = Vol / 13.0f;
	}
	else
	{
		p_Read_Output->MinCellVoltage = Vol / 22.0f;
		p_Read_Output->MaxCellVoltage = Vol / 21.0f;
	}

	p_Read_Output->CommOkCnt++;
	p_Read_Output->TimeOutMs = 0;

}
/***************************************************************************************
** 函数名称: BMS_DataProcess_MCB
** 功能描述: BMS数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void BMS_DataProcess_MCB(BMS_TypeDef *BMSData)
{
	BMS_Read_StDef				*p_Read_Output	= &BMSData->BMS_Read;
	EQP_BMS_MCBObj_StDef		*p_Read_Input	= BMSData->InPutData;
	
	p_Read_Output->Voltage = p_Read_Input->Voltage/1.0f;
	p_Read_Output->Current = p_Read_Input->Current/1.0f;
	p_Read_Output->SOC = p_Read_Input->SOC/1.0f;
	p_Read_Output->SOH = p_Read_Input->SOH/10.0f;
	p_Read_Output->Alarm_Code = p_Read_Input->ALARM;
	p_Read_Output->MinCellVoltage = p_Read_Input->MinCellVoltage/10.0f;
	p_Read_Output->MaxCellVoltage = p_Read_Input->MaxCellVoltage/10.0f;

	p_Read_Output->CommOkCnt = EQPMsts.BMS_MCB.CommOkCnt;
	p_Read_Output->TimeOutMs = EQPMsts.BMS_MCB.TimeOutMs;

	//原始数据有，抽象数据池不需要的内容
	//p_Read_Output->HOT_Current = p_Read_Input->HOT_Current/10.0f;
	//g_BMS.BMS_Ready = p->BMS_Ready;

}

/***************************************************************************************
** 函数名称: BMS_DataProcess_CommConv
** 功能描述: BMS数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void BMS_DataProcess_CommConv(BMS_TypeDef *BMSData)
{
	BMS_Read_StDef				*p_Read_Output	= &BMSData->BMS_Read;
	EQP_BMS_CommConvObj_StDef	*p_Read_Input	= BMSData->InPutData;

	p_Read_Output->Voltage = p_Read_Input->Voltage/10.0f;
	p_Read_Output->Current = p_Read_Input->Current/10.0f;
	p_Read_Output->SOC = p_Read_Input->SOC/10.0f;
	p_Read_Output->SOH = p_Read_Input->SOH/10.0f;
	p_Read_Output->Alarm_Code = p_Read_Input->Alarm_Code;
	p_Read_Output->MinCellVoltage = p_Read_Input->MinCellVoltage;
	p_Read_Output->MaxCellVoltage = p_Read_Input->MaxCellVoltage;

	p_Read_Output->CommOkCnt = EQPMsts.BMS_CommConv.CommOkCnt;
	p_Read_Output->TimeOutMs = EQPMsts.BMS_CommConv.TimeOutMs;
	
}
/***************************************************************************************
** 函数名称: BMS_DataProcess_SerialExp
** 功能描述: BMS数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void BMS_DataProcess_SerialExp(BMS_TypeDef *BMSData)
{
	BMS_Read_StDef				*p_Read_Output	= &BMSData->BMS_Read;
	BMSDataDef					*p_Read_Input	= BMSData->InPutData;

	p_Read_Output->Voltage = p_Read_Input->Voltage;
	p_Read_Output->Current = p_Read_Input->Current;
	p_Read_Output->SOC = p_Read_Input->Soc;
	p_Read_Output->SOH = p_Read_Input->Soh;
	p_Read_Output->Alarm_Code = p_Read_Input->Alarm;
	//p_Read_Output->MinCellVoltage = p_Read_Input->MinCellVoltage/10.0f;
	//p_Read_Output->MaxCellVoltage = p_Read_Input->MaxCellVoltage/10.0f;

	if(EQP_SerialExp.p2Data.LinkSta->BMS == 1)
	{
		p_Read_Output->CommOkCnt = EQPMsts.SerialExp.CommOkCnt;
		p_Read_Output->TimeOutMs = EQPMsts.SerialExp.TimeOutMs;
	}
}
/***************************************************************************************
** 函数名称: BMS_DataProcess_KXD_MCB
** 功能描述: BMS数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void BMS_DataProcess_KXD_MCB(BMS_TypeDef *BMSData)
{
	BMS_Read_StDef				*p_Read_Output	= &BMSData->BMS_Read;
	EQP_KXD_BMSObj_StDef		*p_Read_Input	= BMSData->InPutData;
	
	p_Read_Output->Voltage = p_Read_Input->Voltage/100.0f;
	p_Read_Output->Current = p_Read_Input->Current/100.0f;
	p_Read_Output->SOC = p_Read_Input->SOC;
	p_Read_Output->SOH = (10000-(p_Read_Input->Cycle_Index))/10000.0f;
	p_Read_Output->Alarm_Code = p_Read_Input->ProtectionState;
	p_Read_Output->MinCellVoltage = p_Read_Output->Voltage/p_Read_Input->BatteryString;
	p_Read_Output->MaxCellVoltage = p_Read_Output->Voltage/p_Read_Input->BatteryString;

	p_Read_Output->CommOkCnt = EQPMsts.BMS_MCB.CommOkCnt;
	p_Read_Output->TimeOutMs = EQPMsts.BMS_MCB.TimeOutMs;

}
/***************************************************************************************
** 函数名称: BMS_DataProcess_CATL
** 功能描述: 宁德时代BMS数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void BMS_DataProcess_CATL(BMS_TypeDef *BMSData)
{
	BMS_Read_StDef				*p_Read_Output	= &BMSData->BMS_Read;
	EQP_BMS_CATLObj_StDef   *p_Read_Input = BMSData->InPutData;

	p_Read_Output->Voltage = p_Read_Input->Voltage/10.0f;
	p_Read_Output->Current = -p_Read_Input->Current/10.0f;
	p_Read_Output->SOC = p_Read_Input->SOC/10.0f;
	p_Read_Output->SOH = p_Read_Input->SOH/10.0f;
	p_Read_Output->Alarm_Code = p_Read_Input->Alarm_Code;
	p_Read_Output->Alarm_In_CATL = p_Read_Input->Alarm_In_CATL;
	p_Read_Output->MinCellVoltage = p_Read_Input->MinCellVoltage;
	p_Read_Output->MaxCellVoltage = p_Read_Input->MaxCellVoltage;

	p_Read_Output->CommOkCnt = EQPMsts.BMS_CommConv.CommOkCnt;
	p_Read_Output->TimeOutMs = EQPMsts.BMS_CommConv.TimeOutMs;
	p_Read_Output->Alarm_In_Bit1 = p_Read_Input->Alarm_In_Bit1;
	p_Read_Output->Alarm_In_Bit2 = p_Read_Input->Alarm_In_Bit2;
	p_Read_Output->Alarm_In_Bit3 = p_Read_Input->Alarm_In_Bit3;
	p_Read_Output->Alarm_In_Bit4 = p_Read_Input->Alarm_In_Bit4;
	p_Read_Output->BMS_Ready = p_Read_Input->BMS_Ready;
}
/***************************************************************************************
** 函数名称: BMS_DataProcess_CablePower
** 功能描述: 外接电源 BMS数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void BMS_DataProcess_CablePower(BMS_TypeDef *BMSData)
{
	BMS_Read_StDef			*p_Read_Output	= &BMSData->BMS_Read;

	//伪造了BMS数据，使其他依赖功能正常
	p_Read_Output->Voltage = 48.0f;
	p_Read_Output->Current = -1.0f;
	p_Read_Output->SOC = 100.0f;
	p_Read_Output->SOH = 0;
	p_Read_Output->Alarm_Code = 0;
	p_Read_Output->Alarm_In_CATL = 0;
	p_Read_Output->MinCellVoltage = 13.0f;
	p_Read_Output->MaxCellVoltage = 12.0f;

	p_Read_Output->CommOkCnt ++;
	p_Read_Output->TimeOutMs = 0;
	p_Read_Output->Alarm_In_Bit1 = 0;
	p_Read_Output->Alarm_In_Bit2 = 0;
	p_Read_Output->Alarm_In_Bit3 = 0;
	p_Read_Output->Alarm_In_Bit4 = 0;
	p_Read_Output->BMS_Ready = 0;
}
/***************************************************************************************
** 函数名称: BMS_DataProcess
** 功能描述: BMS数据处理
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void BMS_DataProcess(BMS_TypeDef *BMSData)
{
	/*输入保护，如果没有输入指针 直接返回*/
	if(BMSData->InPutData == NULL)	{return;}

	switch (BMSData->BMS_Source)
	{
		default:
		case BMS_InPutSource_LeadBat :
		{
			BMS_DataProcess_LeadBat(BMSData);
		}break;
		case BMS_InPutSource_MCB :
		{
			BMS_DataProcess_MCB(BMSData);
		}break;
		case BMS_InPutSource_CommConv :
		{
			BMS_DataProcess_CommConv(BMSData);
		}break;
		case BMS_InPutSource_SerialExp :
		{
			BMS_DataProcess_SerialExp(BMSData);
		}break;
		case BMS_InPutSource_KXD_MCB :
		{
			BMS_DataProcess_KXD_MCB(BMSData);
		}break;
		case BMS_InPutSource_CATL:
		{
			BMS_DataProcess_CATL(BMSData);
		}break;
		case BMS_InPutSource_CablePower:
		{
			BMS_DataProcess_CablePower(BMSData);
		}break;
	}
}

/*****************************************************END OF FILE*********************************************/
