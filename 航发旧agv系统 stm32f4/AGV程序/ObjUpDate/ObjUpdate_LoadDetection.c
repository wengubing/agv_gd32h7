/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_LoadDetection.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  OBJUPDATA_LoadDetection_GLOBALS

#include "ObjUpdate_LoadDetection.h"
#include "ACC_Project.h"

/***********************************************************
** 函数名称:		ObjUpdate_LoadDetection
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_LoadDetection(void)
{
	LoadDetection_TypeDef* p = &g_LoadDetection;
	/*使用板载ADC接入负载检测模块 不支持四点*/
	if(g_Para.AppInSource.FucBit.MCBLoadPressure == 1)
	{
		p->Sensor[0].Org = ((g_ADFucs.FL_Pressure.p2ADCN->mV * 0.75)-375)/100.0f;
		p->Sensor[1].Org = ((g_ADFucs.FR_Pressure.p2ADCN->mV * 0.75)-375)/100.0f;
		p->Sensor[2].Org = ((g_ADFucs.B_Pressure.p2ADCN->mV  * 0.75)-375)/100.0f;
	}
	/*使用舟正模块接入负载检测模块*/
	else
	{
		/*
		ADDAObj_StDef* DAQM	= EQP_SerialExp.p2Data.ADDAData;
		p->Sensor[0].Org = (DAQM->DAQM4202_Ch4/ 1747.6f) - 3.75f;
		p->Sensor[1].Org = (DAQM->DAQM4202_Ch5/ 1747.6f) - 3.75f;
		p->Sensor[2].Org = (DAQM->DAQM4202_Ch6/ 1747.6f) - 3.75f;*/
		
		/*舟正模块*/
		p->Sensor[0].Org = (EQP_AD1.RD[0] / 1747.6f) - 3.75f;
		p->Sensor[1].Org = (EQP_AD1.RD[1] / 1747.6f) - 3.75f;
		p->Sensor[2].Org = (EQP_AD1.RD[2] / 1747.6f) - 3.75f;
		p->Sensor[3].Org = (EQP_AD1.RD[3] / 1747.6f) - 3.75f;//仅四点液压时有效
	}
	

}

/************************************END OF FILE************************************/
