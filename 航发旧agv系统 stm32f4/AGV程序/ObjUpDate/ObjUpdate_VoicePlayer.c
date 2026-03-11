/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_VoicePlayer.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  OBJUPDATE_VoicePlayer_GLOBALS

#include "ObjUpdate_VoicePlayer.h"
#include "ACC_Project.h"
#include "ACC_Init.h"

/***********************************************************
** 函数名称:		ObjUpdate_VoicePlayer
** 功能描述: 	语音播报输入更新
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_VoicePlayer(void)
{
	static char DspConnected = 0;//开机调度系统连接上后置1一次

	//模式输入
    g_VoicePlayer.Cur_mode = AccData.ModeSwitchSMT;
    g_VoicePlayer.Target_mode = AccData.OrgCmd.Mode;

	//急停状态位输入
	g_VoicePlayer.BitInput.EStop_Car = g_Faults.LoseControl.Bit.CarEstop;
	g_VoicePlayer.BitInput.EStop_Manual = g_Faults.EmgStop.Bit.Remote;
	g_VoicePlayer.BitInput.EStop_DSP = g_Faults.EmgStop.Bit.Dsp;
	//g_VoicePlayer.BitInput.EStop_PDA = ;

	//避障状态位输入
	g_VoicePlayer.BitInput.Stop_Obs = g_Faults.Remind.Class.RemindOnly.Bit.LaserObs;
	g_VoicePlayer.BitInput.Stop_Crash = (g_Faults.Func.Bit.CrashBarF 
								|| g_Faults.Func.Bit.CrashBarB 
								|| g_Faults.Func.Bit.CrashBarL 
								|| g_Faults.Func.Bit.CrashBarR);
	

	//其他报警
	//g_VoicePlayer.BitInput.Err_Sys = (g_Faults.TypeCnt.LoseControlStop!=0 || g_Faults.TypeCnt.ControlledStop!=0);
	g_VoicePlayer.BitInput.Err_Sys = (g_Faults.LoseControl.Bit.CanMstErr || g_Faults.LoseControl.Bit.CanSlvOffline|| g_Faults.LoseControl.Bit.WalkSvoErr);
	//g_VoicePlayer.BitInput.Err_Unit = 
	//g_VoicePlayer.BitInput.Err_Auto = g_Faults.Auto.All;
	g_VoicePlayer.BitInput.Stop_Limit = g_Faults.Remind.Class.MoveLimit.All;
	//g_VoicePlayer.BitInput.LowPower = (g_Faults.Func.Bit.LowBatteryStop || g_Faults.Remind.Class.RemindOnly.Bit.LowBatteryRemind);
    
    //到位等触发提醒 在具体状态机中触发
	//g_VoicePlayer.BitInput.InitOk = 0;
	//g_VoicePlayer.BitInput.ModeChangeOk = 0;
	g_VoicePlayer.BitInput.Arrive = 0;
	//g_VoicePlayer.BitInput.Still = 0;
	//g_VoicePlayer.BitInput.TaskPause = 0;
	//g_VoicePlayer.BitInput.TaskStart = 0;
	if (DspConnected == 0)//开机调度系统连接上后置1一次
	{
		if (EQPSlvs.DSP.CommOkCnt > 10)
		{
			DspConnected = 1;
			//g_VoicePlayer.BitInput.DspConnected = 1;
		}
		else {}
	}else {}


	//动作提醒
	if(g_SLN.in_FlgMotion)
	{
		g_VoicePlayer.BitInput.ModeChange = (AccData.ModeSwitch.EnterWalkStep == AccEnterWalkStep_TurnWheel);

		g_VoicePlayer.BitInput.Moving = 1;
		g_VoicePlayer.BitInput.Walking = ACC_IsWalkMode(AccData.OrgCmd.Mode);
		g_VoicePlayer.BitInput.Lifting = (AccData.CalCmd.Mode == ACCMode_Lift);
		if(g_VoicePlayer.BitInput.Walking)
		{
			g_VoicePlayer.BitInput.WalkPos = ACC_IsPosWalkMode(AccData.CalCmd.Mode);
			g_VoicePlayer.BitInput.WalkInf = ACC_IsInfWalkMode(AccData.CalCmd.Mode);
			g_VoicePlayer.BitInput.WalkRot = (AccData.CalCmd.Mode == ACCMode_Rotation);
		}
		else
		{
			g_VoicePlayer.BitInput.Up = AccData.CalCmd.Button.LiftUp;
			g_VoicePlayer.BitInput.Down = AccData.CalCmd.Button.LiftDown;
		
		}
	}
	else
	{
		g_VoicePlayer.BitInput.ModeChange = 0;
		g_VoicePlayer.BitInput.Moving = 0;
		g_VoicePlayer.BitInput.Walking = 0;
		g_VoicePlayer.BitInput.WalkPos = 0;
		g_VoicePlayer.BitInput.WalkInf = 0;
		g_VoicePlayer.BitInput.WalkRot = 0;
		g_VoicePlayer.BitInput.Lifting = 0;
		g_VoicePlayer.BitInput.Up = 0;
		g_VoicePlayer.BitInput.Down = 0;
	}
}

/************************************END OF FILE************************************/
