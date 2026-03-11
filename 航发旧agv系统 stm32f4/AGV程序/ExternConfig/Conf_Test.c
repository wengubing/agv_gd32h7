/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Conf_Test.c
** 创 建 人: 文小兵  
** 描    述: 程序测试配置,注意目前 仅纯软仿真时 仅部分调用
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期:  2024年9月7日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define Conf_Test_GLOBALS

//自定义引用			
#include "Conf_Test.h"
#include "ObjConf.h"
#include "ACC_Project.h"
#include "HMI.h"
#include "CommConf.h"
#include "ObjUpdate.h"	
#include "Debug.h"

//外部对象声明

//内部方法声明

//acc核心仿真
bool SoftwareSimulat_Acc(void)
{
	ACC_Para_TypeDef* p2Para = &g_Para;

	//强制忽略所有故障
	AtDbug.Password = 985211;//设置口令
	AtDbug.OffAllErr = 1;

	AtDbug.Bit.AsbOff = 1;

	//u16 SpeedPlanTemp[ACC_WALKGEARMAX]={3000,6000,8000,10000,12000,15000};
	p2Para->Debug.IOInputOff = 1;//切断硬件抽象层更新，方便软件仿真
	p2Para->Debug.ACCInputOff = 1;
	//p2Para->Debug.CbbUpdateOff = 1;
	//p2Para->Debug.PowerOnInputOff = 1;
	//p2Para->Debug.IOOutputOff = 1;
	p2Para->Debug.ForcePowerOn = 1;
	p2Para->Debug.ForceOverErr = 1;
	//p2Para->Debug.ForceModeSwitch = 1;
	//p2Para->Debug.ForceADOrgStop = 1;
	//p2Para->Debug.ForceDACmdStop = 1;
	//g_Para.Debug.ForceSVODisable=1;
	p2Para->Debug.MRSimEn = 1;
	p2Para->Debug.LiftSimEn = 1;

	g_ChannelSwitch.CtrlCmd.Mode = ACCMode_PosFlexible;
	g_ChannelSwitch.CtrlCmd.WalkGear = 3;
	g_ChannelSwitch.CtrlCmd.LiftGear = 3;
	g_ChannelSwitch.CtrlCmd.Vxyz.x = 100;
	g_ChannelSwitch.CtrlCmd.Vxyz.y = 100;
	g_ChannelSwitch.CtrlCmd.Vxyz.z = 100;

	//强制遥控器连接完成
	g_ChannelSwitch.WirelessRemoteComminitOk = 1;

	//强制已指定自动上线朝向
	ExtHmi.p2HMI_CommObj->InitDirection = 0;

	//关闭报警
	ExtHmi.p2HMI_CommObj->ButtonForce.Bit.ForceOverErr_Bit10 = 1;

	//使能轮系跟随巡线
	g_Para.CtrlCoef.BitPara.WheelFollow = 1;

	return 1;
}

//裸板核心仿真
bool BareSimulat_Acc(void)
{
	ACC_Para_TypeDef* p2Para = &g_Para;

	p2Para->Debug.CbbUpdateOff = 1;
	g_Para.Debug.ForcePowerOn = 1;
	return 1;
}

//裸板CANOPEN仿真
bool BareSimulat_CanOpen(void)
{
	g_Para.Debug.IOInputOff = 1;
	//g_Para.Debug.CbbUpdateOff = 1;                        //注意伺服使能时序控制在里面
	g_Para.Debug.ForcePowerOn = 1;
	g_Para.Debug.AccCoreStop = 1;
	g_Para.Debug.ACCOutputOff = 1;

	g_PowerOn.Ctrl_CanOpenStart = 1;                        //强制canopen启动
	//CanMaster.in_CanOpenStart = 1;                        //此对象 尚未启用

	g_Para.CanMst.Baud = CAN_BAUDRATE_500K;
	g_Para.CanMst.CheckQuantity = 1;
	//g_Para.CanMst.SVOType = Can_SVOType_Kinco;
	g_Para.CanMst.SVOType = Can_SVOType_Delta;

	return 1;
}

//根据运行模式强制覆盖部分调试参数
bool ParaInit_CodeReconf(void)
{
	//char i = 0;
	//ACC_Para_TypeDef* p2Para = &g_Para;

	if (g_Para.RunningMode == ACC_RunningMode_SoftwareSimulat)//纯软件仿真模式
	{
		g_Para.Debug.IOInputOff								= 1;//关闭功能组 io输入
		g_Para.Debug.ADOff									= 1;//关闭功能组 AD输入
		g_DbugRtx.k2Ms										= 10;//
		AtDbug.Bit.RmcUpdataOff								= 1;//使用模拟器更新遥控器原始命令
		g_Para.Debug.MRSimEn								= 1;//使能差速角度模拟
		g_Para.Debug.LiftSimEn								= 1;//使能升降高度模拟

		/*超低车三2四液压测试*/
		g_LiftData.CtrlPara.CtrlNnm = 4;
		g_Para.Lift.DiffMax = 65535;
		g_LiftData.WarningPara.TimeoutTime = 65535;

		//g_Para.ChassisType = ACC_ChassisType_Turtle;
		//SoftwareSimulat_Acc();//Acc核心仿真
	}
	else if (g_Para.RunningMode == ACC_RunningMode_BareSimulat)//裸机仿真模式
	{
		//BareSimulat_Acc();//裸板Acc核心仿真
		BareSimulat_CanOpen();//CNAOPEN仿真
	}
	else if (g_Para.RunningMode == ACC_RunningMode_HardwareSimulat)//整机仿真模式
	{

	}
	else {}

	return 1;
}

//***********************************END OF FILE***********************************

