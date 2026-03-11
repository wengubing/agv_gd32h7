/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: BaseApi.c
** 创 建 人: 文小兵  
** 描    述: 全局基础api
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年8月26日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define BaseApi_GLOBALS

//自定义引用			
#include "BaseApi.h"
#include "EmgJudge.h"
#include "IOConf.h"

//外部对象声明

//内部方法声明

//检出设备正在行走中
bool CheckWalk(ACC_DATA_TypeDef* p)
{
	char cnt = 0;
	//正向行走模式，且有y方向速度
	if ((p->ModeSwitchSMT == ACCMode_PosFlexible || p->ModeSwitchSMT == ACCMode_PosStraight || p->ModeSwitchSMT == ACCMode_PosTrace) && p->CalCmd.VOxyz.y != 0)
		cnt++;
	
	//麦克纳姆轮，正向自由下
	if (p->ChassisType == ACC_ChassisType_Turtle && p->ModeSwitchSMT == ACCMode_PosFlexible && (p->CalCmd.VOxyz.y != 0 || p->CalCmd.VOxyz.x != 0 || p->CalCmd.VOxyz.z != 0))
		cnt++;

	//横向行走模式，且有x方向速度
	if ((p->ModeSwitchSMT == ACCMode_InfFlexible || p->ModeSwitchSMT == ACCMode_InfStraight || p->ModeSwitchSMT == ACCMode_InfTrace) && p->CalCmd.VOxyz.x != 0)
		cnt++;

	//旋转模式，且有z方向速度
	if ((p->ModeSwitchSMT == ACCMode_Rotation) && p->CalCmd.VOxyz.z != 0)
		cnt++;

	if (cnt)
		return 1;
	else
		return 0;
}

//检出设备正在动作中
bool CheckMotion(ACC_DATA_TypeDef* p)
{
	char cnt = 0;
	//正向行走模式，且有y方向速度
	if ((p->ModeSwitchSMT == ACCMode_PosFlexible || p->ModeSwitchSMT == ACCMode_PosStraight || p->ModeSwitchSMT == ACCMode_PosTrace) && p->CalCmd.VOxyz.y != 0)
		cnt++;
	//麦克纳姆轮，正向自由下
	if (p->ChassisType == ACC_ChassisType_Turtle && p->ModeSwitchSMT == ACCMode_PosFlexible && (p->CalCmd.VOxyz.y != 0 || p->CalCmd.VOxyz.x != 0 || p->CalCmd.VOxyz.z != 0))
		cnt++;
	//横向行走模式，且有x方向速度
	if ((p->ModeSwitchSMT == ACCMode_InfFlexible || p->ModeSwitchSMT == ACCMode_InfStraight || p->ModeSwitchSMT == ACCMode_InfTrace) && p->CalCmd.VOxyz.x != 0)
		cnt++;

	//旋转模式，且有z方向速度
	if ((p->ModeSwitchSMT == ACCMode_Rotation) && p->CalCmd.VOxyz.z != 0)
		cnt++;

	//升降模式，且正在执行升降命令
	if ((p->ModeSwitchSMT == ACCMode_Lift) && (g_LiftData.Cmd.do_Down || g_LiftData.Cmd.do_Up))
		cnt++;

	//悬挂模式，且正在执行升降命令
	if ((p->ModeSwitchSMT == ACCMode_Absorption) && (g_AbsorptionData.Cmd.do_Down || g_AbsorptionData.Cmd.do_Up))
		cnt++;

	//推送 或者滚筒模式，且正在推拉命令
	if ((p->ModeSwitchSMT == ACCMode_Push || p->ModeSwitchSMT == ACCMode_Roller) && (g_Push_Pull.PR_Cmd.do_Pull || g_Push_Pull.PR_Cmd.do_Push))
		cnt++;

	//轮系模式切换中
	if ((p->ModeSwitchSMT == ACCMode_Null)
		&& (p->ModeSwitch.SteeringStep == ACC_SteeringStep_DiagonalLF || p->ModeSwitch.SteeringStep == ACC_SteeringStep_DiagonalRF))
		cnt++;

	if (cnt)
		return 1;
	else
		return 0;
}

//检出设备运动到达限位状态
bool CheckMotionArrive(ACC_DATA_TypeDef* p)
{
	char cnt = 0;

	//升降模式下，且到达上限位或下限位
	if ((p->ModeSwitchSMT == ACCMode_Lift) && (g_LiftData.CtrlOut.LiftST == LiftCtrl_LiftST_LimitHigh || g_LiftData.CtrlOut.LiftST == LiftCtrl_LiftST_LimitLow))
		cnt++;
	
	//悬挂模式下，且到达上限位或下限位
	if ((p->ModeSwitchSMT == ACCMode_Absorption) && (g_AbsorptionData.CtrlOut.LiftST == LiftCtrl_LiftST_LimitHigh || g_AbsorptionData.CtrlOut.LiftST == LiftCtrl_LiftST_LimitLow))
		cnt++;
	
	//推拉模式下，且到达推限位或拉限位
	if ((p->ModeSwitchSMT == ACCMode_Push || p->ModeSwitchSMT == ACCMode_Roller) && (g_Push_Pull.PR_STM == PushPullCtrl_LimitPush ||g_Push_Pull.PR_STM == PushPullCtrl_LimitPull))
		cnt++;

	if (cnt)
		return 1;
	else
		return 0;
}

//获取避障状态
SLN_ObsState_TypeDef GetObsState(void)
{
	char i = 0;
	RADAR_STATE CbbState = State_Nomal;
	SLN_ObsState_TypeDef ObsState = SLN_ObsState_Normal;

	for (i = 0; i < Radar_MaxNum; i++)
	{
		if (g_ROA.RadarCtrl[i] > ObsState)
			CbbState = g_ROA.RadarCtrl[i];
	}

	if (CbbState == State_Error || CbbState == State_Emergency)
		ObsState = SLN_ObsState_Stop;
	else if (CbbState == State_Caution)
		ObsState = SLN_ObsState_MiddleSlowdown;
	else if (CbbState == State_Warning)
		ObsState = SLN_ObsState_OutSlowdown;
	else
		ObsState = SLN_ObsState_Normal;

	return ObsState;
}
#include "drv_RFIDData.h"
//检出是否在点位上
bool ACC_IsOnPoint(void)
{
	bool IsOnCrossMag = 1;
	//检出是否在十字磁条上
	if (g_MNData[0].MN_Read.Status == 0)
		IsOnCrossMag = 0;
	if (g_MNData[1].MN_Read.Status == 0)
		IsOnCrossMag = 0;
	if (g_MNData[2].MN_Read.Status == 0)
		IsOnCrossMag = 0;
	if (g_MNData[3].MN_Read.Status == 0)
		IsOnCrossMag = 0;

	if (IsOnCrossMag || g_RFIDData.RFID_Read.Status == 1)
		return 1;
	else
		return 0;
}

//单动联动切换时 重置计时时间
bool Switch_AsbSelect_ResetCnt(void)
{
	static uint8_t flag_last = 0;
	if (g_IOInputs.Assembly.RealtimeValue && flag_last == 0)
	{
		g_Faults.Button_Asb_TimeOut_Ms = 0;
		flag_last = 1;
	}
	else if (g_IOInputs.Assembly.RealtimeValue == 0 && flag_last == 1) {
		g_Faults.Button_Asb_TimeOut_Ms = 0;
		flag_last = 0;
	}

	return 1;
}

//***********************************END OF FILE***********************************

