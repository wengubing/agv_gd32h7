/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_LeadShine.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2026-01-12
	* Description:
	* 说    明	：
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define ObjUpdate_LeadShine_GLOBALS
#include <stm32f4xx.h>
#include "ACC_Project.h"
#include "ACC_Init.h"
#include "EQPComm_Init.h"
#include "ObjUpdate_LeadShine.h"


/***********************************************************
** 函数名称:	LeadShine_InPut
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void LeadShine_InPut(void)
{
	ACC_LeadShineCtrl_Input_StDef *p_Input =  &g_LeadShineCtrl.LS_Input;
	//是否自动状态 输入到雷赛控制中
	if(g_ChannelSwitch.Bits.DspModeSel == 1)
	{
		p_Input->AGV_Remote = 0;
	}
	else
	{
		p_Input->AGV_Remote = 1;
	}
	//slam巡线下强制收回
	if(g_Para.TarceType.FunType == Para_TarceType_Slam)
	{
		p_Input->AGV_Extend = 1;
	}
	//slam+磁导航 巡线下 AGV_Extend 受到自动框架控制
	else if(g_Para.TarceType.FunType == Para_TarceType_Slam_Magnetic)
	{
		//p_Input->AGV_Extend = 1;
	}
	else
	{
		p_Input->AGV_Extend = 0;
	}
	//模式输入（2026年1月13日 master 未测试）
	{
		ACCMode_TypeDef	AccData_Mode = AccData.ModeSwitchSMT;

		if(AccData_Mode == ACCMode_PosTrace)
		{
			p_Input->AGV_Mode = LeadShine_PosTrace;
		}
		else if(AccData_Mode == ACCMode_InfTrace)
		{
			p_Input->AGV_Mode = LeadShine_InfTrace;
		}
		else if(AccData_Mode == ACCMode_Rotation)
		{
			p_Input->AGV_Mode = LeadShine_Rotation;
		}
		else if(AccData_Mode == ACCMode_Lift)
		{
			p_Input->AGV_Mode = LeadShine_Lift;
		}
		else
		{
			p_Input->AGV_Mode = LeadShine_Still;
		}
	}
	//高度输入
	p_Input->AGV_Height = g_LiftData.Height.Avg;
	//标定高度输入
	g_LeadShineCtrl.LS_Para.Inst_High = g_Para.Inst.LeadShine;
}

/***********************************************************
** 函数名称:	LeadShine_OutPut
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void LeadShine_OutPut(void)
{

	//暂时只有直接输入到导航转接板的数据通道
	{
		ACC_LeadShineCtrl_Output_StDef 	*p_Output 		= &(g_LeadShineCtrl.LS_Output);
		LS_Write_StDef					*LS_WriteData	= EQP_SerialExp.p2Data.LS_WriteData;
		LS_Read_StDef					*LS_ReadData	= EQP_SerialExp.p2Data.LS_ReadData;

		/*雷赛伺服输入控制指令*/
		LS_WriteData->ControlHeight = p_Output->ControlHeight;

		LS_WriteData->ControlMode 	= p_Output->ControlMode;

		/*对外输出的雷赛伺服状态*/
		p_Output->LeadShine_Sta		= (ACC_LeadShine_Sta_TypeDef)LS_ReadData->Operating;
		/*对外输出的雷赛伺服故障*/
		p_Output->LeadShine_Alarm	= (ACC_LeadShine_Alarm_TypeDef)LS_ReadData->LS_Alarm;

	}
	
}
/***********************************************************
** 函数名称:		ObjUpdate_LeadShine
** 功能描述: 	色带数据更新
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_LeadShine(void)
{
	static uint8_t STM = 0;
	//
	{
		switch(STM)
		{
			case 0:		/*初始化*/
			{
				if(0)
				{
					
				}
				else
				{
					STM = 1;
				}
				
			}break;
			case 1:		/*正常运行*/
			{
				LeadShine_InPut();
				LeadShine_OutPut();
			}break;
		}
	}

}

/************************************END OF FILE************************************/


