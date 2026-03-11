/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_Lift.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  OBJUPDATE_Lift_GLOBALS

#include "ObjUpdate_Lift.h"
#include "ACC_Init.h"
#include "ACC_Project.h"

/***********************************************************
** 函数名称:		ObjUpdate_Lift
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_Lift(void)
{
	
	switch(g_Para.Lift.PfType)
	{
		case LC_PFTP_UnitePump:
		case LC_PFTP_Titan:
		{
			/*选择了导航转接板的数据源头*/
			if(g_Para.AppInSource.FucBit.CommConvLiftHeight != 0)
			{
				/*卡式转接板获取数据*/
				if(g_Para.AppInSource.FucBit.CommConvDouble == 1)
				{
					g_LiftData.Height.Org[0] = EQP_CommConv2.RD[2] + (EQP_CommConv2.RD[3]<<16);
					g_LiftData.Height.Org[1] = EQP_CommConv2.RD[4] + (EQP_CommConv2.RD[5]<<16);
					g_LiftData.Height.Org[2] = EQP_CommConv2.RD[6] + (EQP_CommConv2.RD[7]<<16);
					g_LiftData.Height.Org[3] = EQP_CommConv2.RD[8] + (EQP_CommConv2.RD[9]<<16);
				}
				else
				{
					g_LiftData.Height.Org[0] = EQP_CommConv.RD[26] + (EQP_CommConv.RD[27]<<16);
					g_LiftData.Height.Org[1] = EQP_CommConv.RD[28] + (EQP_CommConv.RD[29]<<16);
					g_LiftData.Height.Org[2] = EQP_CommConv.RD[30] + (EQP_CommConv.RD[31]<<16);
					g_LiftData.Height.Org[3] = EQP_CommConv.RD[32] + (EQP_CommConv.RD[33]<<16);
				}
			}
			else if(g_Para.AppInSource.FucBit.AD_MCBLiftHeight != 0)
			{
				/*直接的舟正模块获取数据*/
				g_LiftData.Height.Org[0] = EQP_AD1.RD[3];
				g_LiftData.Height.Org[1] = EQP_AD1.RD[4];
				g_LiftData.Height.Org[2] = EQP_AD1.RD[5];
				g_LiftData.Height.Org[3] = EQP_AD1.RD[6];
			}
			else
			{
				/*串口转接板的舟正模块获取数据*/
				g_LiftData.Height.Org[0] = EQP_SerialExp.p2Data.ADDAData->DAQM4202_Ch0;
				g_LiftData.Height.Org[1] = EQP_SerialExp.p2Data.ADDAData->DAQM4202_Ch1;
				g_LiftData.Height.Org[2] = EQP_SerialExp.p2Data.ADDAData->DAQM4202_Ch2;
				g_LiftData.Height.Org[3] = EQP_SerialExp.p2Data.ADDAData->DAQM4202_Ch3;
			}
		}break;
		case LC_PFTP_Scissors:
		case LC_PFTP_Screw:
		{
			if(g_ADFucs.Lift_Cable.Id	!= ADCN_Disable)
			{
				g_LiftData.Height.Org[0] = g_ADFucs.Lift_Cable.p2ADCN->mV;
				g_LiftData.Height.Org[1] = g_ADFucs.Lift_Cable.p2ADCN->mV;
				if(g_ADFucs.Lift_Cable1.Id	!= ADCN_Disable)
				{
					g_LiftData.Height.Org[2] = g_ADFucs.Lift_Cable1.p2ADCN->mV;
					g_LiftData.Height.Org[3] = g_ADFucs.Lift_Cable1.p2ADCN->mV;
				}
				else
				{
					g_LiftData.Height.Org[2] = g_ADFucs.Lift_Cable.p2ADCN->mV;
					g_LiftData.Height.Org[3] = g_ADFucs.Lift_Cable.p2ADCN->mV;
				}
			}
			else if(g_ADFucs.Lift_Cable1.Id	!= ADCN_Disable)
			{
				g_LiftData.Height.Org[2] = g_ADFucs.Lift_Cable1.p2ADCN->mV;
				g_LiftData.Height.Org[3] = g_ADFucs.Lift_Cable1.p2ADCN->mV;
				if(g_ADFucs.Lift_Cable.Id	!= ADCN_Disable)
				{
					g_LiftData.Height.Org[0] = g_ADFucs.Lift_Cable.p2ADCN->mV;
					g_LiftData.Height.Org[1] = g_ADFucs.Lift_Cable.p2ADCN->mV;
				}
				else
				{
					g_LiftData.Height.Org[0] = g_ADFucs.Lift_Cable1.p2ADCN->mV;
					g_LiftData.Height.Org[1] = g_ADFucs.Lift_Cable1.p2ADCN->mV;
				}
			}
		}break;
		case LC_PFTP_Scissors_Two:
		{
			g_LiftData.Height.Org[0] = EQP_AD.RD[2];
			g_LiftData.Height.Org[1] = EQP_AD.RD[5];

			g_Push_Pull.PR_CtrlIn.Distance[0] = EQP_AD.RD[3];
			g_Push_Pull.PR_CtrlIn.Distance[1] = EQP_AD.RD[4];
		}break;
		

		/*暂未适配*/
		case LC_PFTP_SynCylinder:
		case LC_PFTP_Null:
		default:
		{
			
		}break;
		
	}

	/*模拟器*/
	{
		char i=0;
		//模拟器结果反馈到实际高度
		if(g_Para.Debug.LiftSimEn)
			for(i=0;i<g_LiftData.Height.Num;i++)
				g_LiftData.Height.Org[i] = g_LiftData.Simulator.Hg[i];
	}

	//升降计算标定
	LiftCtrl_Cal(&g_LiftData.Height);
	LiftCtrl_Extreme(&g_LiftData.Height);//极值分析
	LiftCtrl_Ejdg(&g_LiftData);	
	//此处更新升降cbb的系统时间，防止模式切换中升降超时报警误触
	g_LiftData.Cmd.in_Ms = AccData.in_Ms;

}

/************************************END OF FILE************************************/
