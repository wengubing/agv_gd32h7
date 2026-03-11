/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_BMS.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  OBJUPDATE_BMS_GLOBALS

#include "ObjUpdate_BMS.h"
#include "ACC_Project.h"
/***********************************************************
** 函数名称:		ObjUpdate_BMS_init
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_BMS_init(void)
{
	if(	  g_Para.BatteryType == ACC_LeadBatteries_TN48V
		||g_Para.BatteryType == ACC_LeadBatteries_HBK48V
		||g_Para.BatteryType == ACC_LeadBatteries_TN72V)
	{
		static uint8_t temp_Input = 0;
		g_BMSData.BMS_Source = BMS_InPutSource_LeadBat;
		g_BMSData.InPutData = &temp_Input;
	}
	else if(g_Para.BatteryType == ACC_Cable_Power)
	{
		static uint8_t temp_Input = 0;
		g_BMSData.BMS_Source = BMS_InPutSource_CablePower;
		g_BMSData.InPutData = &temp_Input;
	}
	else if(g_Para.SerialExpObjSource.BMS == 1)
	{
		g_BMSData.BMS_Source = BMS_InPutSource_SerialExp;
		g_BMSData.InPutData  = EQP_SerialExp.p2Data.BMSData;
	}
	else if(g_Para.EQPCommSource.EN.BMS_CommConv == 1)
	{
		g_BMSData.BMS_Source = BMS_InPutSource_CommConv;
		g_BMSData.InPutData  = EQP_BMS_CommConv.p2Data;	
		
	}
	else if(g_Para.EQPCommSource.EN.BMS_MCB == 1)
	{
		g_BMSData.BMS_Source = BMS_InPutSource_MCB;
		g_BMSData.InPutData  = EQP_BMS_MCB.p2Data;
	}
	//凯信达电池直连的情况
	else if(g_Para.EQPCommSource.EN.BMS_KXD == 1)
	{
		g_BMSData.BMS_Source = BMS_InPutSource_KXD_MCB;
		g_BMSData.InPutData  = EQP_KXD_BMS.RD;
	}
	//宁德时代BMS
	else if(g_Para.EQPCommSource.EN.BMS_CATL == 1)
	{
		g_BMSData.BMS_Source = BMS_InPutSource_CATL;
		g_BMSData.InPutData  = EQP_BMS_CATL.p2Data;
	}
}

/***********************************************************
** 函数名称:		ObjUpdate_BMS
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_BMS(void)
{
	
	static uint8_t STM = 0;
	//
	{
		switch(STM)
		{
			case 0:		/*初始化*/
			{
				ObjUpdate_BMS_init();
				STM = 1;

			}break;
			case 1:		/*正常运行*/
			{
				BMS_DataProcess(&g_BMSData);
			}break;
		}
	}
}

/************************************END OF FILE************************************/
