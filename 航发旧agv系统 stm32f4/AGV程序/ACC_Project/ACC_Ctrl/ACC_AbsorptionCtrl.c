/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ACC_AbsorptionCtrl.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  ACCABSORPTIONCTRL_GLOBALS

#include "ACC_AbsorptionCtrl.h"
#include "ACC_Init.h"
#include "ACC_Project.h"

/***********************************************************
** 函数名称:	AbsorptionCtrl_Ctrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
bool AbsorptionCtrl_Ctrl(void)
{
	ACC_DATA_TypeDef* p2Acc			= &AccData;
	LiftCtrl_TypeDef* p_Absorption	= &g_AbsorptionData;
	AirCompressor_Typedef* p_AirCompressor = &g_AirCompressor;
	static uint32_t ABS_DelayTime = 0;
	float DisvalueMax = 0.01;		//空气悬挂实时压力与目标压力差值最大值

	//建立命令管道
	p_Absorption->Cmd.in_Ms		= p2Acc->in_Ms;
	p_Absorption->Cmd.in_Up		= p2Acc->CalCmd.Button.LiftUp;
	p_Absorption->Cmd.in_Down	= p2Acc->CalCmd.Button.LiftDown;
	p_Absorption->Cmd.in_Gears	= p2Acc->CalCmd.LiftGear;

	//调用CBB解算接口
	//标定计算已与控制分离，在外部循环调用
	LiftCtrl_CmdPrep(&g_AbsorptionData);
	LiftCtrl_Ctrl(&g_AbsorptionData);
	
	/*标准3点液压升降——四点检测、三点控制（伺服电机） 电机只需要正转泵油即可*/
	p2Acc->LiftMotor[0].V = p_Absorption->CtrlOut.MotorSpeed[0];
	p2Acc->LiftMotor[1].V = p_Absorption->CtrlOut.MotorSpeed[1];
	p2Acc->LiftMotor[2].V = p_Absorption->CtrlOut.MotorSpeed[2];
	//电磁阀控制输出
	p2Acc->IOCtrl.Absorption_Titan_UpLF 		= p_Absorption->CtrlOut.ValveUp[LiftCtrl_Ctrl_LF];
	p2Acc->IOCtrl.Absorption_Titan_UpRF 		= p_Absorption->CtrlOut.ValveUp[LiftCtrl_Ctrl_RF];
	p2Acc->IOCtrl.Absorption_Titan_UpB  		= p_Absorption->CtrlOut.ValveUp[LiftCtrl_Ctrl_B];
	p2Acc->IOCtrl.Absorption_Titan_DownLF 	= p_Absorption->CtrlOut.ValveDown[LiftCtrl_Ctrl_LF];
	p2Acc->IOCtrl.Absorption_Titan_DownRF 	= p_Absorption->CtrlOut.ValveDown[LiftCtrl_Ctrl_RF];
	p2Acc->IOCtrl.Absorption_Titan_DownB 	= p_Absorption->CtrlOut.ValveDown[LiftCtrl_Ctrl_B];
	
	

	//空气悬挂控制输出
	if (g_Para.Absorption.PfType == LC_PFTP_AirSuspension)
	{
		//计算目标压力
		if (g_ChannelSwitch.AirAbsorptionLoad == NoLoad)
		{
			p_AirCompressor->P_SetPoint = (p_AirCompressor->Para.DeadWeight * p_AirCompressor->Para.K_Down * 400) /
				(p_AirCompressor->Para.CylinderNum * p_AirCompressor->Para.K_Arm * PI * pow(p_AirCompressor->Para.CylinderDiameter,2)) / 10;
		}
		else if (g_ChannelSwitch.AirAbsorptionLoad == LowLoad)
		{
			p_AirCompressor->P_SetPoint = ((p_AirCompressor->Para.DeadWeight + p_AirCompressor->Para.LowWeight) * p_AirCompressor->Para.K_Down * 400) /
				(p_AirCompressor->Para.CylinderNum * p_AirCompressor->Para.K_Arm * PI * pow(p_AirCompressor->Para.CylinderDiameter,2)) / 10;
		}
		else if (g_ChannelSwitch.AirAbsorptionLoad == HighLoad)
		{
			p_AirCompressor->P_SetPoint = ((p_AirCompressor->Para.DeadWeight +p_AirCompressor->Para.HighWeight) * p_AirCompressor->Para.K_Down * 400) /
				(p_AirCompressor->Para.CylinderNum * p_AirCompressor->Para.K_Arm * PI * pow(p_AirCompressor->Para.CylinderDiameter,2)) / 10;
		}
//		if (g_ChannelSwitch.AirAbsorptionLoad == NoLoad)
//		{
//			p_AirCompressor->P_SetPoint = 0.14;
//		}
//		else if (g_ChannelSwitch.AirAbsorptionLoad == LowLoad)
//		{
//			p_AirCompressor->P_SetPoint = 0.33;
//		}
//		else if (g_ChannelSwitch.AirAbsorptionLoad == HighLoad)
//		{
//			p_AirCompressor->P_SetPoint = 0.52;
//		}
		
		p_AirCompressor->Disvalue = p_AirCompressor->P_RealTime - p_AirCompressor->P_SetPoint;

		//控制输出
		if (p_AirCompressor->Disvalue < (-1) * DisvalueMax)//当前压力小于目标压力时，开始进气
		{
			p2Acc->IOCtrl.AirCompressor_In	= 1;
			p2Acc->IOCtrl.AirCompressor_Out = 0;
		}
		else if (p_AirCompressor->Disvalue > DisvalueMax)//当前压力大于目标压力时，开始排气
		{
			p2Acc->IOCtrl.AirCompressor_In	= 0;
			p2Acc->IOCtrl.AirCompressor_Out = 1;
		}
		else
		{
			if (ABS_DelayTime == 0)
			{
				ABS_DelayTime = g_Sys_1ms_Counter;
			}
			if (g_Sys_1ms_Counter > ABS_DelayTime + 2000)
			{
				p2Acc->IOCtrl.AirCompressor_In = 0;
				p2Acc->IOCtrl.AirCompressor_Out = 0;
				ABS_DelayTime = 0;
			}
			
		}
	}
	return 0;
}

/************************************END OF FILE************************************/
