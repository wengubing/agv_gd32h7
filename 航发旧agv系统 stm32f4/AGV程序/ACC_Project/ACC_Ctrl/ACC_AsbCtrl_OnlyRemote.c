/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ACC_AsbCtrl_OnlyRemote.c
** 创 建 人: 文小兵  
** 描    述: 仅遥控控制联动模块组织
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2025年7月22日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define ACC_AsbCtrl_OnlyRemote_GLOBALS

//自定义引用	
#include "ACC_AsbCtrl_OnlyRemote.h"	
#include "ACC_AssemlyCtrl.h"	
//#include "app_Assembly.h"
#include "ACC_Project.h"
#include "ACC_Init.h"
#include "BaseApi.h"
#include "Debug.h"

//外部对象声明

//内部方法声明

//解析单联动选择命令
AsbSelCmd_TypeDef Asb_RO_Get_AsbSelCmd(AsbModel_TypeDef AsbModel)
{
	AsbSelCmd_TypeDef Res = AsbHcmMode_Nnull;
	if (AsbModel == AsbModel_A)
		Res = AsbHcmMode_StrNo1;
	else if (AsbModel == AsbModel_B)
		Res = AsbHcmMode_StrNo2;
	else if (AsbModel == AsbModel_Asb)
		Res = AsbHcmMode_Asb;
	else
		Res = AsbHcmMode_Nnull;
	return Res;
}

//实时输入 底盘信息
bool Asb_RO_Input_Car(AsbCtrl_TypeDef* p)
{
	p->Input.Ms = g_Sys_1ms_Counter;
	p->Input.AsbSel_Panel = g_IOInputs.Assembly.StableValue;//面板选择联动
	//p->Input.Wired = g_IOInputs.RemoteWiredSel.StableValue;//有线控制 438有线 注释避免报警
	return 1;
}

//联动传感器
bool Asb_RO_Input_Sensor(AsbCtrl_TypeDef* p)
{
	AsbSensor_TypeDef* p_Sensor = &(p->Input.Sensor);
	/*438
	p_Sensor->Distance_FL = g_AssemlySensorData.LaserDis_L.Cal;
	p_Sensor->Distance_BR = g_AssemlySensorData.LaserDis_R.Cal;
	p_Sensor->LaserSpot = g_AssemlySensorData.SpotDis.Cal;*/

	/*465只一个前后的测距传感器*/
	//且只前后联动时才从传感器获取，左右联动时需手动输入
	//后面要设计成开关
	if (p->Para.SiteNo != AsbSiteNo_LR)
	{
		p_Sensor->Distance_FL = g_AssemlySensorData.LaserDis_L.Cal;
		p_Sensor->Distance_BR = g_AssemlySensorData.LaserDis_R.Cal;
	}
	else
	{
		/*
		p_Sensor->Distance_FL = p->Input.ManualInput_Dis;
		p_Sensor->Distance_BR = p->Input.ManualInput_Dis;*/
	}

	p_Sensor->LaserSpot = 0;

	return 1;
}

//从车位置 直接用p->Input.Sensor对象 反映数据层次
bool Asb_RO_Input_Place(AsbCtrl_TypeDef* p)
{
	AsbPlace_TypeDef* p_Place = &(p->Input.Place);
	//p_Place->Pos = (g_AssemlySensorData.LaserDis_L.Cal + g_AssemlySensorData.LaserDis_R.Cal) / 2;
	p_Place->Pos = (p->Input.Sensor.Distance_FL + p->Input.Sensor.Distance_BR) / 2;
	p_Place->Inf = g_AssemlySensorData.SpotDis.Cal;
	p_Place->Ang = (g_AssemlySensorData.LaserDis_R.Cal - g_AssemlySensorData.LaserDis_L.Cal);
	return 1;
}

//从遥控器获取原始遥控器命令
bool Asb_RO_Input_OrgCmd_RMC(AsbCtrl_TypeDef* p)
{
	AsbCmd_TypeDef* p_OrgCmd = &g_Asb.Input.OrgCmd;
	OrgCtrlCmd_TypeDef* p_CtrlCmdCmd = &g_ChannelSwitch.CtrlCmd;

	//单联动选择
	p->Input.AsbSel_Cmd = Asb_RO_Get_AsbSelCmd(p_CtrlCmdCmd->HandleUnionVal);

	//模式
	p_OrgCmd->Mode = (AsbMode_TypeDef)p_CtrlCmdCmd->Mode;
	//摇杆命令
	p_OrgCmd->Vxyz.x = p_CtrlCmdCmd->Vxyz.x;
	p_OrgCmd->Vxyz.y = p_CtrlCmdCmd->Vxyz.y;
	p_OrgCmd->Vxyz.z = p_CtrlCmdCmd->Vxyz.z;

	//行走挡位Z[1,6]
	p_OrgCmd->WalkGear = p_CtrlCmdCmd->WalkGear;
	//升降挡位Z[1,6]
	p_OrgCmd->LiftGear = p_CtrlCmdCmd->LiftGear;
	//上升命令
	p_OrgCmd->Bit.LiftUp = p_CtrlCmdCmd->Button.LiftUp;
	//下降命令
	p_OrgCmd->Bit.LiftDown = p_CtrlCmdCmd->Button.LiftDown;
	return 1;
}

//从联动通信获取原始遥控器命令
bool Asb_RO_Input_OrgCmd_Comm(AsbCtrl_TypeDef* p)
{
	AsbCmd_TypeDef* p_OrgCmd = &g_Asb.Input.OrgCmd;
	AsbDataM2S_TypeDef* p_CustomM = &(g_Asb.Comm.MasterData.CustomM);

	//单联动选择
	{
		AsbModel_TypeDef UnionVal = (AsbModel_TypeDef)(p_CustomM->Bit.UnionVal);
		p->Input.AsbSel_Cmd = Asb_RO_Get_AsbSelCmd(UnionVal);
	}

	//模式
	p_OrgCmd->Mode = p_CustomM->Mode;
	//摇杆命令
	p_OrgCmd->Vxyz.x = p_CustomM->Vx / 10.0f;
	p_OrgCmd->Vxyz.y = p_CustomM->Vy / 10.0f;
	p_OrgCmd->Vxyz.z = p_CustomM->Vz / 10.0f;
	//挡位
	p_OrgCmd->WalkGear = p_CustomM->Bit.WalkGear;
	p_OrgCmd->LiftGear = p_CustomM->Bit.LiftGear;
	//按钮命令
	p_OrgCmd->Bit.LiftUp = p_CustomM->Bit.LiftUp;
	p_OrgCmd->Bit.LiftDown = p_CustomM->Bit.LiftDown;
	
	return 1;
}

//实时输入 原始遥控器命令
bool Asb_RO_Input_OrgCmd(AsbCtrl_TypeDef* p)
{	
	/*区分命令来源
	if (p->Para.Role == AsbRole_Master
		|| (p->Para.Role == AsbRole_Slave && p->Input.AsbSel_Panel == 0)//主车 或 从车 单动时 从遥控器获取
	)
		Asb_RO_Input_OrgCmd_RMC(p);
	else if(p->Para.Role == AsbRole_Slave && p->Input.AsbSel_Panel == 1)//从车选联动时 从联动通信获取
		Asb_RO_Input_OrgCmd_Comm(p);
	else {}*/
	
	//全部从p_CtrlCmdCmd获取
	Asb_RO_Input_OrgCmd_RMC(p);
	
	return 1;
}

//状态
bool Asb_RO_Input_Status(AsbCtrl_TypeDef* p)
{
	//实际模式
	p->Input.ModeReal = (AsbMode_TypeDef)AccData.ModeSwitchSMT;
	//升降高度
	p->Input.Hg = g_LiftData.Height.QuadrantAvg[0];
	//运动中
	p->Input.Bit.Moving = CheckWalk(&AccData);
	
	//上降限位触发
	if (g_LiftData.Height.Avg >= g_LiftData.CtrlPara.LimitUp)
	{
		p->Input.Bit.UpLimited = 1;
	}
	else
	{
		p->Input.Bit.UpLimited = 0;
	}
	//下降限位触发
	if (g_LiftData.Height.Avg <= g_LiftData.CtrlPara.LimitDown)
	{
		p->Input.Bit.DownLimited = 1;
	}
	else
	{
		p->Input.Bit.DownLimited = 0;
	}
	return 1;
}

//自身故障状态
bool Asb_RO_Input_SelfFault(AsbCtrl_TypeDef* p)
{
	AsbInput_TypeDef* p_TargetData = &g_Asb.Input;
	//故障类型
	p_TargetData->OrgErr.Faults = (Asb_FaultsType_TypeDef)g_Faults.Output.Self.Faults;
	//限制上升
	p_TargetData->OrgErr.Bit.UpLimited = g_Faults.Output.Self.Bit.UpLimited;
	//限制下降
	p_TargetData->OrgErr.Bit.DownLimited = g_Faults.Output.Self.Bit.DownLimited;
	//限制行走
	p_TargetData->OrgErr.Bit.MoveLimited = g_Faults.Output.Self.Bit.MoveLimited;
	return 1;
}

//行走缩放参数
bool Asb_RO_Input_WalkZoom(AsbCtrl_TypeDef* p)
{
	AsbInput_TypeDef* p_TargetData = &g_Asb.Input;

	if (AccData.ChassisType == ACC_ChassisType_Turtle)
	{
		p_TargetData->WheelRMax = g_MecanumResolving.RotUnif.LxyMax.Self;
		p_TargetData->WheelVMax = g_MecanumResolving.SpeedZoom.SpeedMax.Self;
	}
	else
	{
		p_TargetData->WheelRMax = g_MotorResolving.RdMax.RdMax_Self;
		p_TargetData->WheelVMax = g_MotorResolving.WheelSpeedObj.WheelSpeedMax_Self;
	}
	return 1;
}

//主车广播遥控器命令
bool Asb_RO_Input_BroadcastCmd(AsbCtrl_TypeDef* p)
{
	AsbInput_TypeDef* p_TargetData = &g_Asb.Input;
	AsbDataM2S_TypeDef* p_CustomM = &p_TargetData->CustomM;
	OrgCtrlCmd_TypeDef* p_OrgCtrlCmd = &(g_ChannelSwitch.CtrlCmd);

	//联动选择
	p_CustomM->Bit.UnionVal = p_OrgCtrlCmd->HandleUnionVal;
	//主车对从车共享的遥感值共享
	p_CustomM->Vx = p_OrgCtrlCmd->Vxyz.x * 10; //放大10倍
	p_CustomM->Vy = p_OrgCtrlCmd->Vxyz.y * 10; //放大10倍
	p_CustomM->Vz = p_OrgCtrlCmd->Vxyz.z * 10; //放大10倍
	//模式
	p_CustomM->Mode = (AsbMode_TypeDef)p_OrgCtrlCmd->Mode;
	//挡位
	p_CustomM->Bit.WalkGear = p_OrgCtrlCmd->WalkGear;
	p_CustomM->Bit.LiftGear = p_OrgCtrlCmd->LiftGear;
	//按钮
	p_CustomM->Bit.LiftUp = p_OrgCtrlCmd->Button.LiftUp;
	p_CustomM->Bit.LiftDown = p_OrgCtrlCmd->Button.LiftDown;
	//喇叭
	p_CustomM->Bit.Horn = p_OrgCtrlCmd->Button.Horn;
	return 1;
}

//校验输入
bool Asb_RO_Input_Check(AsbCtrl_TypeDef* p)
{
	AsbInput_TypeDef* p_TargetData = &g_Asb.Input;
	//本地校验参数
	AsbLocalParaCheck_TypeDef* p_CheckData = &p_TargetData->CheckData;
	/*平滑参数
	p_CheckData->VxAcc = g_SmoothVx.in_Acc;
	p_CheckData->VyAcc = g_SmoothVy.in_Acc;
	p_CheckData->VzAcc = g_SmoothVz.in_Acc;*/
	/*校验执行值
	p_CheckData->VxDec = g_SmoothVx.in_Dec;
	p_CheckData->VyDec = g_SmoothVy.in_Dec;
	p_CheckData->VzDec = g_SmoothVz.in_Dec;*/
	/*校验参数值
	p_CheckData->VxDec = g_Para.Walk_Asb.Smooth.VxDec;
	p_CheckData->VyDec = g_Para.Walk_Asb.Smooth.VyDec;
	p_CheckData->VzDec = g_Para.Walk_Asb.Smooth.VzDec;
	
	p_CheckData->AxAcc = g_SmoothAx.in_Acc;
	p_CheckData->AyAcc = g_SmoothAy.in_Acc;
	p_CheckData->AzAcc = g_SmoothAz.in_Acc;
	p_CheckData->AxDec = g_SmoothAx.in_Dec;
	p_CheckData->AyDec = g_SmoothAy.in_Dec;
	p_CheckData->AzDec = g_SmoothAz.in_Dec;*/
	//行走解算参数 实时值校验 而不是g_Para
	p_CheckData->MaxSliAngPos = g_MotorResolving.Para.MaxSliAngPos;
	p_CheckData->MaxSliAngInf = g_MotorResolving.Para.MaxSliAngInf;
	p_CheckData->MaxRotDisPos = g_MotorResolving.Para.MaxRotDisPos;
	p_CheckData->MinRotDisPos = g_MotorResolving.Para.MinRotDisPos;
	//Bit
	p_CheckData->Bit.AsbSel = p_TargetData->AsbSel_Panel;
	return 1;
}

//实时输入
bool Asb_RO_Input(void)
{
	AsbCtrl_TypeDef* p = &g_Asb;

	//底盘
	Asb_RO_Input_Car(p);

	//更新联动位置
	if (AtDbug.Bit.AsbSensorInOff == 0)
	{
		Asb_RO_Input_Sensor(p);//联动传感器
		Asb_RO_Input_Place(p);//从车位置
	}
	else {}

	//原始控制命令
	Asb_RO_Input_OrgCmd(p);

	//状态
	Asb_RO_Input_Status(p);
	
	//避障状态
	ACC_Assemly_InputObs();

	//自身故障状态
	Asb_RO_Input_SelfFault(p);

	//行走缩放参数
	Asb_RO_Input_WalkZoom(p);

	//主车广播遥控器命令
	Asb_RO_Input_BroadcastCmd(p);

	/*主车广播 之 避障切区选择*/
	g_Asb.Input.CustomM.ObsAreaSel = (u16)g_ChannelSwitch.CtrlCmd.Button.ObsArea;

	//主车广播 之 激光避障使能信息
	g_Asb.Input.CustomM.Bit.RadarObsEnF = g_ChannelSwitch.CtrlCmd.RadarEn.Front;
	g_Asb.Input.CustomM.Bit.RadarObsEnB = g_ChannelSwitch.CtrlCmd.RadarEn.Back;
	g_Asb.Input.CustomM.Bit.RadarObsEnL = g_ChannelSwitch.CtrlCmd.RadarEn.Left;
	g_Asb.Input.CustomM.Bit.RadarObsEnR = g_ChannelSwitch.CtrlCmd.RadarEn.Right;

	//广播之实时联动距离 用主车自己输入的联动距离广播
	g_Asb.Input.CustomM.Dis = g_Asb.Input.Place.Pos;

	//校验输入
	Asb_RO_Input_Check(p);

	return 1;
}

//输出 控制命令
bool Asb_RO_Output_Cmd(void)
{
	OrgCtrlCmd_TypeDef* p_OrgCmd = &AccData.OrgCmd;
	AsbCmd_TypeDef* p_Cmd = &g_Asb.Output.Cmd;

	//模式
	p_OrgCmd->Mode = (ACCMode_TypeDef)p_Cmd->Mode;
	//摇杆命令
	p_OrgCmd->Vxyz.x = p_Cmd->Vxyz.x;
	p_OrgCmd->Vxyz.y = p_Cmd->Vxyz.y;
	p_OrgCmd->Vxyz.z = p_Cmd->Vxyz.z;

	//行走挡位Z[1,6]
	p_OrgCmd->WalkGear = p_Cmd->WalkGear;
	//升降挡位Z[1,6]
	p_OrgCmd->LiftGear = p_Cmd->LiftGear;
	//上升命令
	p_OrgCmd->Button.LiftUp = p_Cmd->Bit.LiftUp;
	//下降命令
	p_OrgCmd->Button.LiftDown = p_Cmd->Bit.LiftDown;

	return 1;
}

//输出
bool Asb_RO_Output(void)
{
	//控制命令
	Asb_RO_Output_Cmd();

	//避障状态 输入到避障模块
	ACC_Assemly_ObsUpdate();

	//激光避障使能 从主车的通信广播中获取
	AccData.OrgCmd.RadarEn.Front = g_Asb.Comm.MasterData.CustomM.Bit.RadarObsEnF;
	AccData.OrgCmd.RadarEn.Back = g_Asb.Comm.MasterData.CustomM.Bit.RadarObsEnB;
	AccData.OrgCmd.RadarEn.Left = g_Asb.Comm.MasterData.CustomM.Bit.RadarObsEnL;
	AccData.OrgCmd.RadarEn.Right = g_Asb.Comm.MasterData.CustomM.Bit.RadarObsEnR;

	/*各车根据不同联动位置 自定义关闭部分方向避障 直接关使能行不通，改为了避障模块内部关障碍状态*/
	ACC_Assemly_ObsEnCustom();

	/*避障切区 从主车的通信广播中获取*/
	AccData.OrgCmd.Button.ObsArea = (AccObsArea_TypeDef)g_Asb.Comm.MasterData.CustomM.ObsAreaSel;

	/*如从车需要在特殊位置切换到特殊避障区域，在此处添加逻辑*/
	ACC_Assemly_ObsAreaCustom();

	return 1;
}

//输出
bool Asb_RO_OutputCtrl(AsbCtrl_TypeDef* p)
{
	AsbCar_TypeDef* p2Car = p->P2Obj.p2Self;

	if (g_Asb.Input.AsbSel_Panel == 1 && p->Input.AsbSel_Cmd == AsbHcmMode_Asb)//如面板选择联动 且 为命令为联动时 才调用输出
	//if(1)//统一从联动模块处理，不利于单车或面板选单动时调试
	{
		Asb_RO_Output();
	}
	else//面板选择单车时 联动输出不产生作用
	{
		AccData.OrgCmd = g_ChannelSwitch.CtrlCmd;
	}

	//所选非自己车，强制静止,参考联动内部处理
	if (p2Car->AsbSwitch.STM == AsbAsbMode_OtherCar)
	{
		AccData.OrgCmd.Mode = ACCMode_Still;
		AccData.OrgCmd.Vxyz.x = 0;
		AccData.OrgCmd.Vxyz.y = 0;
		AccData.OrgCmd.Vxyz.z = 0;
	}
	else {}

	//喇叭等命令 联动模块内部不做处理
	AccData.OrgCmd.Button.Horn = g_ChannelSwitch.CtrlCmd.Button.Horn;
	AccData.OrgCmd.Button.Lighting = g_ChannelSwitch.CtrlCmd.Button.Lighting;

	//选择非自己时，强制关闭喇叭等
	if( p->Input.AsbSel_Cmd != (AsbSelCmd_TypeDef)p->Para.Iam
		&&p->Input.AsbSel_Cmd != AsbHcmMode_Asb)
	{
		AccData.OrgCmd.Button.Horn = 0;
		AccData.OrgCmd.Button.Lighting = 0;
	}
	else {}

	/*针对遥控器命令从联动通信来的从车 将单联动选择返回给g_ChannelSwitch、以及传递给AccData，以期报警等正常使用*/
	if (g_Para.SysFucBit.Asb_Comm2Cmd)
	{
	g_ChannelSwitch.CtrlCmd.HandleUnionVal = (AsbModel_TypeDef)(p->Comm.MasterData.CustomM.Bit.UnionVal);
	AccData.OrgCmd.HandleUnionVal = g_ChannelSwitch.CtrlCmd.HandleUnionVal;
	}

	return 1;
}

bool Asb_RO_LandscapeLaser(AsbCtrl_TypeDef* p)
{
	//B车 且 面板选择联动时 打开一字激光
	if(p->Para.Role == AsbRole_Slave && p->Input.AsbSel_Panel == 1)
		AccData.IOCtrl.LandscapeLaser = 1;
	else
		AccData.IOCtrl.LandscapeLaser = 0;

	return 1;
}

//面板联动开关状态机
bool Asb_AsbSel_Switch(void)
{
	if (AccData.STMs.Car_AsbSel != g_IOInputs.Assembly.StableValue)
	{
		if (g_IOInputs.Assembly.StableValue == 0)
			AccData.AsbCtrl.MoveDir_Reverse = 0;//切单动时 清除反向标志
		else
			__NOP();
		AccData.STMs.Car_AsbSel = g_IOInputs.Assembly.StableValue;
	}
	else
		__NOP();

	return 1;
}

//仅遥控控制联动模块组织
bool ACC_AssemlyCtrl_OnlyRemote(void)
{
	//面板联动开关状态机
	Asb_AsbSel_Switch();
		
	//实时输入
	if (AtDbug.Bit.AsbInputOff == 0)//调试时可强制输入不调用进行测试
		Asb_RO_Input();
	else {}

	//调用
	Asb_Main(&g_Asb);

	//输出
	if (!g_Para.Debug.ACCInputOff)
		Asb_RO_OutputCtrl(&g_Asb);
	else {}

	//一字激光发射控制
	Asb_RO_LandscapeLaser(&g_Asb);

	return 1;
}


//***********************************END OF FILE***********************************

