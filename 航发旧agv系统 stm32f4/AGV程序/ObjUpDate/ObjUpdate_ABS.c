/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_ABS.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  OBJUPDATE_ABS_GLOBALS

#include "ObjUpdate_ABS.h"
#include "ACC_Project.h"

/***********************************************************
** 函数名称:		ObjUpdate_ABS
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_ABS(void)
{
	uint32_t LF_Org = 0,RF_Org = 0,LB_Org = 0,RB_Org = 0;
	LF_Org = (EQP_CommConv.RD[26] + (EQP_CommConv.RD[27]<<16) + EQP_CommConv.RD[30] + (EQP_CommConv.RD[31]<<16))/2;
	RF_Org = (EQP_CommConv.RD[28] + (EQP_CommConv.RD[29]<<16) + EQP_CommConv.RD[32] + (EQP_CommConv.RD[33]<<16))/2;
	LB_Org = (EQP_CommConv.RD[34] + (EQP_CommConv.RD[35]<<16));
	RB_Org = (EQP_CommConv.RD[36] + (EQP_CommConv.RD[37]<<16));
	g_AbsorptionData.Height.Org[0] = LF_Org;
	g_AbsorptionData.Height.Org[1] = RF_Org;
	g_AbsorptionData.Height.Org[2] = LB_Org;
	g_AbsorptionData.Height.Org[3] = RB_Org;


	//升降计算标定
	LiftCtrl_Cal(&g_AbsorptionData.Height);
	LiftCtrl_Extreme(&g_AbsorptionData.Height);//极值分析					
	LiftCtrl_Ejdg(&g_AbsorptionData);

}

/************************************END OF FILE************************************/
