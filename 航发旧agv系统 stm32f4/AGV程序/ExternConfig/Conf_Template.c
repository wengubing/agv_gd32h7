/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Conf_Template.c
** 创 建 人: Master
** 描    述: 模板项目
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期:  2025年2月10日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define Conf_Template_GLOBALS

//自定义引用			
#include "Conf_Template.h"
#include "Fun_Template.h"
#include "ObjConf.h"
#include "ACC_Project.h"
#include "HMI.h"
#include "CommConf.h"
#include "ObjUpdate.h"
#include "ObjUpdate_Assemly.h"
#include "ACC_Init.h"
#include "Debug.h"

/***************************************************************************************
** 函数名称:		app_SkeletonAGV_ParaIndexInit
** 功能描述:		自动运行的参数设置函数
** 参    数: 	无
** 返 回 值:	 	无
****************************************************************************************/
void app_SkeletonAGV_ParaIndexInit_Template(void)
{
	/*不需要屏幕设置的数据，依次赋值到自动运行框架中*/

	/*需要屏幕设置的数据，依次赋值到自动运行框架中*/

	//自激AGV的本地路径数据生成
	{
		if(g_HostData.AGVPendantFun.AutoexcitationNode == 1)
		{
			uint8_t err = 0;
			g_AutoExcitation.AE_Para.p_DHR = &(EQP_DSP.Data[0]);
			g_AutoExcitation.AE_Para.Path_MaxNum = 1;
			//A->B
			//第一路段
			err = err + AGVAutoexcitationCreateNodePoint(&(g_AutoExcitation.AE_Para.AE_NodeList[0]),1, 1120,0000,270.0f,0,0);
			err = err + AGVAutoexcitationCreateNodePoint(&(g_AutoExcitation.AE_Para.AE_NodeList[0]),2, 1120,3955,180.0f,0,0);
			err = err + AGVAutoexcitationCreateNodePoint(&(g_AutoExcitation.AE_Para.AE_NodeList[0]),3, 3125,3955,180.0f,0,0);

			//A->B
			//第一路段·
			err = err + AGVAutoexcitationCreateNodePath(&(g_AutoExcitation.AE_Para.AE_NodeList[0]),1,2,000.0f);
			err = err + AGVAutoexcitationCreateNodePath(&(g_AutoExcitation.AE_Para.AE_NodeList[0]),2,3,090.0f);


			//初始化标志置位
			if(err == 0){g_AutoExcitation.AE_InitFlag = 0XAA;}
				
		}
	}
}
/***************************************************************************************
** 函数名称:		app_SkeletonAGV_FunInit
** 功能描述:		自动运行的功能设置函数
** 参    数:	无
** 返 回 值:		无
****************************************************************************************/
void app_SkeletonAGV_FunInit_Template(void)
{

	/*底盘类型：舵轮车*/
	g_HostData.AGVChassisMode = AGVChassisMode_DifSteeringWheel;
	/*导航方式：磁导航*/
	g_HostData.AGVNavigateMode = AGVNavigateMode_Magnetic;
	/*选用传感器有：无*/
	memset(g_HostData.InPutSensor,AGVInPutSensor_CrossMag,sizeof(g_HostData.InPutSensor));

	/*自动运行的刹车距离初始化*/
	{
		//提前减速，增加减速距离已减缓负载时停靠过冲
		float SlowDownDisPlan[4] 		= {500, 700, 900,1000 };
		float SlowDownDisPlan_Goods[4] 	= {600,1000,1200,1600 };
		memcpy(g_HostData.AGVWalkSlowDownDisPlan ,SlowDownDisPlan,sizeof(g_HostData.AGVWalkSlowDownDisPlan));
		memcpy(g_HostData.AGVGoodsSlowDownDisPlan, SlowDownDisPlan_Goods,sizeof(g_HostData.AGVGoodsSlowDownDisPlan));
	}
	/*停靠、取物料、放物料、充电停车	通过：虚拟里程计+RFID		进行减速判断*/
	g_HostData.AGVWalkCorrectMode		= AGVCorrection_ODO_Mag;
	g_HostData.AGVGetParkCorrectMode 	= AGVCorrection_ODO_Mag;
	g_HostData.AGVPutParkCorrectMode 	= AGVCorrection_ODO_Mag;
	g_HostData.AGVChargeCorrectMode 	= AGVCorrection_ODO_Mag;
	/*停靠、取物料、放物料、充电停车	通过：RFID				进行停车判断*/
	g_HostData.AGVWalkParkMode		= AGVParking_CrossMag;
	g_HostData.AGVGetParkGoodsMode	= AGVParking_CrossMag;
	g_HostData.AGVPutParkGoodsMode	= AGVParking_CrossMag;
	g_HostData.AGVChargeParkMode	= AGVParking_CrossMag;
	/*自转							通过：十字磁条+虚拟里程计	进行到位判断*/
	g_HostData.AGVPosture = AGVPosture_ODO_Mag;
	/*取放工装的时候，没有精定位*/
	g_HostData.AGVGetGoodsAccPosMode = AGVAccPosMode_NULL;
	g_HostData.AGVPutGoodsAccPosMode = AGVAccPosMode_NULL;

	/*特殊点位盲举				功能：关闭*/
	g_HostData.AGVPendantFun.BlindLift 			= 0x00;
	/*弧线路径					功能：关闭*/
	g_HostData.AGVPendantFun.ArcWalk 			= 0x00;
	/*悬挂调节					功能：关闭*/
	g_HostData.AGVPendantFun.HangControl		= 0x00;
	/*放工装时纠XZ				功能：关闭*/
	g_HostData.AGVPendantFun.PutGoodsQRMag 		= 0x00;
	/*撤销任务立即停止所有动作	功能：关闭*/
	g_HostData.AGVPendantFun.RepealTaskStopMove	= 0x00;
	/*车端暂停任务				功能：关闭*/
	g_HostData.AGVPendantFun.PauseTask 			= 0x00;
	/*支腿控制					功能：关闭*/
	g_HostData.AGVPendantFun.LegTelescopic		= 0x00;
	/*调度切遥控清除本地任务缓存	功能：打开*/
	g_HostData.AGVPendantFun.ModelAlterStopTask	= 0x01;
	/*自发生成点位数据的功能		功能：关闭*/
	g_HostData.AGVPendantFun.AutoexcitationNode	= 0x00;
	
	/*自动运行的参数设置函数调用*/
	app_SkeletonAGV_ParaIndexInit_Template();
}

//称重参数参数初始化
void Para_LoadDetection_Template(void)
{
	LoadDetection_TypeDef* p = &g_LoadDetection;
	//四边形顶点参数，依次两个元素组成一个点坐标，mm
	//注意按顺时针或逆时针顺序设定！
	s32 QuadrangleTemp[8] = { -550,1300,550,1300,550,-1900,-550,-1900 };

	/*通过系统参数配置 示例
	if (g_Para.Lift.PfType == LC_PFTP_Titan)
	{
		g_Para.Load.Dim 			= 150;		//油缸直径		单位：1mm  		需要机械负责轮系的同事给出
		g_Para.Load.WeightNoload 	= 0.0f;		//空载压力		单位：1T		下地、轮系升降正常后根据实际值自行填写，未知情况下可填0
		g_Para.Load.SingleMax 		= 13.5f;	//单杠超载压力	单位：1MP
		g_Para.Load.OverLoad 		= 0.0f;		//超载报警值（额定吨位）单位：1T下地、轮系升降正常后根据实际值自行填写，未知情况下可填0
	}*/

	/*CBB_Init中已注释掉配置调用，此处直接配置CBB对象*/
	//使能计算
	p->CalEn = 1;
	//报警参数
	p->Errpar.SingleMin = 0;
	p->Errpar.SingleMax = 15;
	p->Errpar.OverLoad = 100;
	p->Errpar.cnt_max = 6000;
	
	p->Num = 3;										//根据压力点数设定 目前最大4组
	g_Para.Load.WeightNoload = 5;					//设备空载时的重量，设置方法：设备空载的时候，仿真观察总重量WeightAll，将其设置为该值
	g_Para.Load.SingleMax = 15.0f;					//单个液压控制点 压力超限报警  单位Mpa
	g_Para.Load.OverLoad = 100.0f;					//超载报警值（额定吨位）单位：1T下地、轮系升降正常后根据实际值自行填写，未知情况下可填0
	g_Para.Other.OverLoad = 100.0f;					//超载报警值（额定吨位）单位：1T下地、轮系升降正常后根据实际值自行填写，未知情况下可填0

	//第一组
	p->Sensor[0].AxisX = -589;//压力传感器等效坐标 X mm
	p->Sensor[0].AxisY = 1312;//压力传感器等效坐标 Y mm
	p->Sensor[0].Dim = 150;//油缸直径 mm
	p->Sensor[0].UnitNum = 2;//并联数量
	p->Sensor[0].LeverCoefficient = 1;//力臂系数
	//第二组
	p->Sensor[1].AxisX = 589;
	p->Sensor[1].AxisY = 1312;
	p->Sensor[1].Dim = 150;
	p->Sensor[1].UnitNum = 2;
	p->Sensor[1].LeverCoefficient = 1;
	//第三组
	p->Sensor[2].AxisX = 0;
	p->Sensor[2].AxisY = -1914;
	p->Sensor[2].Dim = 150;
	p->Sensor[2].UnitNum = 2;
	p->Sensor[2].LeverCoefficient = 1;
	//第四组，仅四点液压时有效
	p->Sensor[3].AxisX = 0;
	p->Sensor[3].AxisY = -1914;
	p->Sensor[3].Dim = 150;
	p->Sensor[3].UnitNum = 2;
	p->Sensor[3].LeverCoefficient = 1;

	//判定三角形在压力三角形的内侧
	g_LoadDetection.Errpar.JudgeTriganle.a.X = -550;
	g_LoadDetection.Errpar.JudgeTriganle.a.Y = 1300;
	g_LoadDetection.Errpar.JudgeTriganle.b.X = 550;
	g_LoadDetection.Errpar.JudgeTriganle.b.Y = 1300;
	g_LoadDetection.Errpar.JudgeTriganle.c.X = 0;
	g_LoadDetection.Errpar.JudgeTriganle.c.Y = -1900;

	//判定四边形，仅四点液压时有效
	memcpy(g_LoadDetection.Errpar.JudgeQuadrangle, QuadrangleTemp, sizeof(QuadrangleTemp));
}

//轮系角度参数初始化
bool Para_WheelAngle_Template(void)
{
	u8 i = 0;
	g_Para.WheelAngle.Num = 6;
	g_Para.WheelAngle.WheelAngleType = AngleType_Single;

	g_Para.WheelAngle.Prop = 0.00549324;
	g_Para.WheelAngle.SetA = 0;
	g_Para.WheelAngle.SetB = 0;
	for (i = 0; i < g_Para.WheelAngle.Num; i++)
	{
		g_Para.WheelAngle.OrgSetA[i] = 0;
		g_Para.WheelAngle.OrgSetB[i] = 0;
		g_Para.WheelAngle.Offset[i]  = 0;
	}

	return 1;
}
//轮系报警参数配置
void ObjInit_MotorResoling_Template(void)
{
	u8 i = 0;

	/*常规配置示例*/
	for(i = 0; i < g_Para.WheelAngle.Num ; i++)
	{
		g_MotorResolving.Para.Mxy[i].Inf_Invflg 			= 0;
		g_MotorResolving.Para.Mxy[i].Rot_Invflg 			= 0;
		g_MotorResolving.Para.Mxy[i].RealAngMax_Left 	= g_Para.Walk.CtrlAngMax_Left;
		g_MotorResolving.Para.Mxy[i].RealAngMax_Right 	= g_Para.Walk.CtrlAngMax_Right;
		g_MotorResolving.Para.Mxy[i].CtrlAngMax_Left 	= g_MotorResolving.Para.Mxy[i].RealAngMax_Left  - 1;//控制角度比实际角度小1度
		g_MotorResolving.Para.Mxy[i].CtrlAngMax_Right 	= g_MotorResolving.Para.Mxy[i].RealAngMax_Right + 1;//左正右负 右侧需要加1才算是小1度
	}

	/*超低车配置示例
	for (i = 0; i < g_Para.WheelAngle.Num; i++)
	{
		if (g_Conf_Template.ModuleType[i] == Conf_ModuleType_General)//正常模组
		{
			g_MotorResolving.Para.Mxy[i].Inf_Invflg = 0;
			g_MotorResolving.Para.Mxy[i].Rot_Invflg = 0;
			g_MotorResolving.Para.Mxy[i].RealAngMax_Left = +47;
			g_MotorResolving.Para.Mxy[i].RealAngMax_Right = -97;
			g_MotorResolving.Para.Mxy[i].CtrlAngMax_Left = +46;//控制角度比实际角度小1度
			g_MotorResolving.Para.Mxy[i].CtrlAngMax_Right = -96;
		}
		else//特殊模组
		{
			g_MotorResolving.Para.Mxy[i].Inf_Invflg = 1;
			g_MotorResolving.Para.Mxy[i].Rot_Invflg = 0;
			g_MotorResolving.Para.Mxy[i].RealAngMax_Left = +97;
			g_MotorResolving.Para.Mxy[i].RealAngMax_Right = -47;
			g_MotorResolving.Para.Mxy[i].CtrlAngMax_Left = +96;//控制角度比实际角度小1度
			g_MotorResolving.Para.Mxy[i].CtrlAngMax_Right = -46;
		}
	}*/
}

//行走参数初始化
bool Para_Walk_Template(void)
{
	//注意留档速度为行走的最大差动速度留一点余量，否则加起来超过最大限制 将触发缩放
	u16 SpeedPlanTemp[ACC_WALKGEARMAX] = { 3000,10000,16000,20000,24000,28000 };//行走速度档位

    //车型
	g_Para.ChassisType = ACC_ChassisType_Titan;
	g_Para.Walk.MotorNum = 12;

	/*要联动：注意超低车的中心要联动的不在车正中！
	g_Para.Walk.Mxy[0].x = -1377;//单位mm
	g_Para.Walk.Mxy[0].y = 1917;
	g_Para.Walk.Mxy[1].x = 0;
	g_Para.Walk.Mxy[1].y = 1917;

	g_Para.Walk.Mxy[2].x = -1377;//单位mm
	g_Para.Walk.Mxy[2].y = 707;
	g_Para.Walk.Mxy[3].x = 0;
	g_Para.Walk.Mxy[3].y = 707;

	g_Para.Walk.Mxy[4].x = -1377;//单位mm
	g_Para.Walk.Mxy[4].y = -1917;
	g_Para.Walk.Mxy[5].x = 0;
	g_Para.Walk.Mxy[5].y = -1917;*/

	/*单动 在车的正中*/
	g_Para.Walk.Mxy[0].x = -688;//单位mm
	g_Para.Walk.Mxy[0].y = 1917;
	g_Para.Walk.Mxy[1].x = 688;
	g_Para.Walk.Mxy[1].y = 1917;

	g_Para.Walk.Mxy[2].x = -688;//单位mm
	g_Para.Walk.Mxy[2].y = 707;
	g_Para.Walk.Mxy[3].x = 688;
	g_Para.Walk.Mxy[3].y = 707;

	g_Para.Walk.Mxy[4].x = -688;//单位mm
	g_Para.Walk.Mxy[4].y = -1917;
	g_Para.Walk.Mxy[5].x = 688;
	g_Para.Walk.Mxy[5].y = -1917;

	memcpy(g_Para.Walk.SpeedPlan, SpeedPlanTemp, sizeof(SpeedPlanTemp));
	g_Para.Walk.Lc = 369;
	g_Para.Walk.JoystickMax = 107;

	//g_Para.Walk.MotorMaxRpm = g_Para.Walk.SpeedPlan[ACC_WALKGEARMAX - 1];//此参数在舵轮车中无效
	//CBB_Init中已修改，注意不能超过驱动器最大转速 例如台达最大3000rpm
	g_Para.Walk.MotorMaxRpm = 30000;

	g_Para.Walk.MGC_InstX = 800;
	g_Para.Walk.MGC_InstY = 1700;

	g_Para.Walk.CtrlAngMax_Left 	= 60;
	g_Para.Walk.CtrlAngMax_Right 	= -110;					//可单独对每个轮系设置报警参数

	/*复合运动解算映射参数*/
	g_Para.Walk.MaxSliAngPos = 30.0f;
	g_Para.Walk.MaxSliAngInf = 3.0f;						//超低车横向斜行范围很小
	g_Para.Walk.MaxRotDisPos = 60000;
	g_Para.Walk.MinRotDisPos = 6000;
	g_Para.Walk.MaxRotDisInf = 60000;
	g_Para.Walk.MinRotDisInf = 8000;

	//差动控制参数
	g_Para.Walk.AngInPlaceMin								= 30;//单位 0.01°
	g_Para.Walk.AngInPlaceMin_ChangeIng						= 150;//单位 0.01°
	g_Para.Walk.AngSlowDown 								= 500;//单位 0.01° 运行中减速角度设置小一点 有利目标角度的跟随
	g_Para.Walk.AngSlowDown_ChangeIng 						= 1000;//单位 0.01°让模式切换时间更短
	g_Para.Walk.SkewSpeed									= 5000;
	g_Para.Walk.SkewSpeed_ChangeIng							= 9000;//模式切换过程的差动速度

	//平滑参数初始化
	g_Para.Walk.Smooth.VxAcc = 0.15f;
	g_Para.Walk.Smooth.VxDec = 0.30f;
	g_Para.Walk.Smooth.VyAcc = 0.15f;
	g_Para.Walk.Smooth.VyDec = 0.30f;
	g_Para.Walk.Smooth.VzAcc = 0.15f;
	g_Para.Walk.Smooth.VzDec = 0.30f;
	g_Para.Walk.Smooth.AxAcc = 0.02f;
	g_Para.Walk.Smooth.AxDec = 0.02f;
	g_Para.Walk.Smooth.AyAcc = 0.02f;
	g_Para.Walk.Smooth.AyDec = 0.02f;
	g_Para.Walk.Smooth.AzAcc = 0.02f;
	g_Para.Walk.Smooth.AzDec = 0.02f;

	g_Para.Other.in_SmoothVxyzAcc = 15;
	g_Para.Other.in_SmoothVxyzDec = 30;

	g_Para.Walk.ModeSwitchMode = ACC_ModeSwitchMode_All;//一起转

	//力矩限制参数设置
	g_Para.Walk.TorqueLimit_Stop	= 95;
	g_Para.Walk.LoadCntMax 			= 200;
	g_Para.Walk.LoadCntMin 			= 190;
	g_Para.Walk.TorqueLimitMax 		= 400;
	g_Para.Walk.TorqueLimitMin 		= 390;//默认设置参数如上，相当于关闭了行走中的力矩限制
	
	//麦轮的特殊设置，其他车型无需设置
	if(g_Para.ChassisType == ACC_ChassisType_Turtle)
	{
		s8 WheelType[4] = { 1, -1, -1, 1};
		u8 i = 0;
		//初始化轮子位置和类型
		for (i = 0; i < g_Para.Walk.MotorNum; i++)
		{
			g_Para.Walk.Ratio[i] = 1.0f;;
			g_Para.Walk.Diameter[i] = 200.0f;//没有特殊的 临时设置轮径为200mm;
			g_Para.Walk.WheelType[i] = WheelType[i];
			g_Para.Walk.MotorDirection[i] = (g_Para.Walk.Mxy[i].x < 0) ? -1 : 1;
		}
	}
	
	
	
	//位域参数
	g_Para.Walk.Bits.AxyzSmoothStopWithVy = 1;//Axyz平滑停止，在前向速度停下时
	
	//舵轮解算cbb对象初始化
	ObjInit_MotorResoling_Template();
	return 1;
}

//行走参数初始化 联动
bool Para_Walk_Asb_Template(void)
{
	//联动的行走参数设置
	g_Para.Walk_Asb.MaxSliAngPos = 12.0f;
	g_Para.Walk_Asb.MaxSliAngInf = 4.0f;
	g_Para.Walk_Asb.MinRotDisPos = 13000;
	g_Para.Walk_Asb.MinRotDisInf = 12000;

	//平滑参数初始化
	g_Para.Walk_Asb.Smooth.VxAcc = 0.1;
	g_Para.Walk_Asb.Smooth.VxDec = 0.3;
	g_Para.Walk_Asb.Smooth.VyAcc = 0.1;
	g_Para.Walk_Asb.Smooth.VyDec = 0.3;
	g_Para.Walk_Asb.Smooth.VzAcc = 0.1;
	g_Para.Walk_Asb.Smooth.VzDec = 0.3;
	g_Para.Walk_Asb.Smooth.AxAcc = 0.02;
	g_Para.Walk_Asb.Smooth.AxDec = 0.02;
	g_Para.Walk_Asb.Smooth.AyAcc = 0.02;
	g_Para.Walk_Asb.Smooth.AyDec = 0.02;
	g_Para.Walk_Asb.Smooth.AzAcc = 0.02;
	g_Para.Walk_Asb.Smooth.AzDec = 0.02;

	/*特殊联动运动中心配置 示例
	if (g_Para.EqpID == 46501)//主车
	{
		g_Para.Walk_Asb.Bit.FB_Axis_Forward = 0;
		g_Para.Walk_Asb.Bit.FB_Axis_Moon 	= 1;
		g_Para.Walk_Asb.Bit.LR_Axis_Fixed 	= 0;
		g_Para.Walk_Asb.Bit.LR_Axis_Moon 	= 1;
	}
	else if (g_Para.EqpID == 46502)//从车
	{
		g_Para.Walk_Asb.Bit.FB_Axis_Forward = 1;
		g_Para.Walk_Asb.Bit.FB_Axis_Moon 	= 0;
		g_Para.Walk_Asb.Bit.LR_Axis_Fixed 	= 1;
		g_Para.Walk_Asb.Bit.LR_Axis_Moon 	= 0;
	}*/

	return 1;
}

//升降参数初始化
bool Para_Lift_Template(void)
{
	char i = 0;

	s16 SpeedPlanUp[GEARS_NUM_MAX]		= {6500, 8500, 9500,11500,13500,15000};//如果是通过追赶进行同步、需要比电机最大速度小一点，才有追赶空间
	s16 SpeedPlanDown[GEARS_NUM_MAX] 	= {2000, 2500, 3000, 3500, 4000, 5000};

	memcpy(g_Para.Lift.SpeedPlanUp	,SpeedPlanUp	,sizeof(SpeedPlanUp)  );
	memcpy(g_Para.Lift.SpeedPlanDown,SpeedPlanDown	,sizeof(SpeedPlanDown));

	g_Para.Lift.Num 		= 4;
	g_Para.Lift.SetLow 		= 0;
	g_Para.Lift.SetHigh 	= 200;
	g_Para.Lift.LimitDown 	= 10;
	g_Para.Lift.LimitUp 	= 80;
	g_Para.Lift.DiffMax		= 20;

	/*泰坦3点式液压*/
	g_Para.Lift.PfType = LC_PFTP_Titan;
	g_Para.Lift.Titan.k_HgDiff2SpeedDiff = 2000.0f;

	/*高度标定参数*/
//	for(i = 0; i < g_Para.Lift.Num; i++)
	{
		i = 0;
		g_Para.Lift.SetLowOrg[i] = 0;g_Para.Lift.SetHighOrg[i] = 200;i++;
		g_Para.Lift.SetLowOrg[i] = 0;g_Para.Lift.SetHighOrg[i] = 200;i++;
		g_Para.Lift.SetLowOrg[i] = 0;g_Para.Lift.SetHighOrg[i] = 200;i++;
		g_Para.Lift.SetLowOrg[i] = 0;g_Para.Lift.SetHighOrg[i] = 200;
	}

	/*CBB直接参数*/
	//CtrlNnm要在项目配置文件里配置，因为有的超低车要改四点液压
	g_LiftData.CtrlPara.CtrlNnm = 3;
	/*报警参数设定*/
	g_LiftData.WarningPara.DiffMax = 20;
	g_LiftData.WarningPara.HeightLow = 5;
	g_LiftData.WarningPara.HeightHigh = 195;
	g_LiftData.WarningPara.DiffRearMax = 65535;//默认不开启后侧高度差报警
	g_LiftData.WarningPara.TimeoutTime = 10000;
	g_LiftData.WarningPara.TimeoutHeight = 3;
	/*调试临时关闭
	g_LiftData.WarningPara.DiffMax = 65535;
	g_LiftData.WarningPara.HeightLow = 0;
	g_LiftData.WarningPara.HeightHigh = 65535;
	g_LiftData.WarningPara.DiffRearMax = 65535;//默认不开启后侧高度差报警
	g_LiftData.WarningPara.TimeoutTime = 65535;
	g_LiftData.WarningPara.TimeoutHeight = 1;*/

	//模拟器参数设置
	g_LiftData.Simulator.k_Mot2Hg = 0.00003;
	g_LiftData.Simulator.CallDelayMs = 1;

	//报警延时参数
	g_LiftData.WarningPara.EjdgDelay = 2000;

	//特殊参数设置
	g_LiftData.CtrlPara.BitPara.Bits.ValveSyn_Down = 1;	//下降时通过电磁阀控制高度，一般3点液压的超低车置1，普通泰坦置0
	g_LiftData.CtrlPara.BitPara.Bits.ValveSyn_Down_OnlyOneOff = 1;
	g_LiftData.CtrlPara.BitPara.Bits.ValveSyn_Down_AFD = 1;
	g_LiftData.AFD.Para_Ms_Check = 3000;
	g_LiftData.AFD.Para_Ms_Continue = 3000;
	g_LiftData.CtrlPara.BitPara.Bits.MotorSyn_Up_Acc = 1;
	g_LiftData.CtrlPara.BitPara.Bits.MotorReverse = 0;
	g_LiftData.CtrlPara.BitPara.Bits.SingleReachStop = 1;

	//泵速度比
	g_LiftData.CtrlPara.Pump[0] = 1;
	g_LiftData.CtrlPara.Pump[1] = 1;
	g_LiftData.CtrlPara.Pump[2] = 1;
	g_LiftData.CtrlPara.Pump[3] = 1;//四点液压时有效

	//高度数据 象限分组设定
	g_LiftData.Height.Quadrant[0] = LiftCtrl_Quadrant_LF;
	g_LiftData.Height.Quadrant[1] = LiftCtrl_Quadrant_RF;
	g_LiftData.Height.Quadrant[2] = LiftCtrl_Quadrant_LB;
	g_LiftData.Height.Quadrant[3] = LiftCtrl_Quadrant_RB;

	//时序参数
	g_LiftData.CtrlPara.PumpOnDelayMs = 500;
	g_LiftData.CtrlPara.ValveOffDelayMs = 500;
	//电磁阀调整参数
	g_LiftData.CtrlPara.ValveOffTgHg = 5;
	g_LiftData.CtrlPara.ValveReOnTgHg = 3;

	//压力均衡控制参数
	g_LiftData.PrBalance.HgMin = 30;
	g_LiftData.PrBalance.LoadMin = 0;//设置为额定负载的40%左右进行测试，设置很大时即关闭压力均衡功能
	g_LiftData.PrBalance.SlowDownMax = 1000;
	g_LiftData.PrBalance.k_Pr2Slow = 100;

	return 1;
}
//悬挂功能参数初始化
bool Para_Absorption_Template(void)
{
	s16 SpeedPlanUp[GEARS_NUM_MAX]	={ 6500, 8500, 9500,11500,13500,15000};
	s16 SpeedPlanDown[GEARS_NUM_MAX]={ 2000, 2500, 3000, 3500, 4000, 5000};

	memcpy(g_Para.Absorption.SpeedPlanUp	,SpeedPlanUp	,sizeof(SpeedPlanUp)  );
	memcpy(g_Para.Absorption.SpeedPlanDown	,SpeedPlanDown	,sizeof(SpeedPlanDown));
	
	g_Para.Absorption.Num 		= 4;
	g_Para.Absorption.SetLow 	= 0;
	g_Para.Absorption.SetHigh 	= 150;
	g_Para.Absorption.LimitDown = 2;
	g_Para.Absorption.LimitUp 	= 145;

	g_Para.Absorption.PfType = LC_PFTP_Null;

	g_Para.Absorption.Titan.k_HgDiff2SpeedDiff = 2000.0f;

//	{
//		char i = 0;
//		i = 0;
//		g_Para.Absorption.SetLowOrg[i] = 100; g_Para.Absorption.SetHighOrg[i] = 1650; i++;
//		g_Para.Absorption.SetLowOrg[i] = 100; g_Para.Absorption.SetHighOrg[i] = 1650; i++;
//		g_Para.Absorption.SetLowOrg[i] = 100; g_Para.Absorption.SetHighOrg[i] = 1650; i++;
//		g_Para.Absorption.SetLowOrg[i] = 100; g_Para.Absorption.SetHighOrg[i] = 1650;  
//	}
 
	return 1;
}
//推拉参数初始化
void Para_Push_Roller_Template(void)
{
	u16 SpeedPlanPush[GEARS_NUM_MAX]	={6500,8500,9500,11500,13500,15000};
	u16 SpeedPlanPull[GEARS_NUM_MAX]	={6500,8500,9500,11500,13500,15000};

	memcpy(g_Push_Pull.PR_CtrlPara.SpeedPlanPush,SpeedPlanPush,sizeof(SpeedPlanPush));
	memcpy(g_Push_Pull.PR_CtrlPara.SpeedPlanPull,SpeedPlanPull,sizeof(SpeedPlanPull));
	
	g_Push_Pull.PR_CtrlPara.CtrlNum = 0;
	g_Push_Pull.PR_CtrlPara.LimitPush = 0;
	g_Push_Pull.PR_CtrlPara.LimitPull = 0;

	g_Push_Pull.PR_Type = PushPullType_Null;
 
}
//Template安装标定参数初始化
bool Para_Inst_Template(void)
{
	//磁导航安装标定参数
	g_Para.Inst.MagF = 0;
	g_Para.Inst.MagB = 0;
	g_Para.Inst.MagL = 0;
	g_Para.Inst.MagR = 0;

	//向上pgv安装标定参数
	g_Para.Inst.PGVUp_X = 0;
	g_Para.Inst.PGVUp_Y = 0;
	g_Para.Inst.PGVUp_Z = 0;

	//向下pgv安装标定参数
	g_Para.Inst.PGVDown_X = 0;
	g_Para.Inst.PGVDown_Y = 0;
	g_Para.Inst.PGVDown_Z = 0;

	//雷赛伺服安装偏移参数
	g_Para.Inst.LeadShine = 0;

	return 1;
}

//Template雷达参数初始化
void Para_ROA_Template(void)
{
	
	g_Para.ROA.Brand.Hinson_HE_3091	 	= 0;
	g_Para.ROA.Brand.Hinson_LE_50821 	= 0;
	g_Para.ROA.Brand.Hinson_LE_XX 	 	= 1;	/*使用标准雷达*/
	g_Para.ROA.Brand.Hinson_SE_XX 	 	= 0;
	g_Para.ROA.Brand.Sick_XXX_XXX 	 	= 0;

	g_Para.ROA.Install.Diagonal_L 			= 1;	/*使用左对角雷达*/
	g_Para.ROA.Install.Diagonal_R 			= 0;	/*使用右对角雷达*/
	g_Para.ROA.Install.Center 				= 0;	/*前后雷达*/
	g_Para.ROA.Install.Surround 			= 0;	/*四周雷达*/
	g_Para.ROA.Install.FourCorners 			= 0;	/*四角雷达*/
	g_Para.ROA.Install.ForwardBackInclined	= 0;	/*前后斜面雷达*/
	
	
	g_Para.ROA.InPutSource.MCB_IO 				= 1;	/*使用IO直连			*/
	g_Para.ROA.InPutSource.MCB_Modbus			= 0;	/*数字通讯——ModBus直连	*/
	g_Para.ROA.InPutSource.MCB_Can				= 0;	/*数字通讯——Can直连输入	*/
	g_Para.ROA.InPutSource.SerialExp_Community	= 0;	/*数字通讯——导航转接板	*/
	g_Para.ROA.InPutSource.PLC_Community 		= 0;	/*数字通讯——PLC安全模块	*/
}   

//Template虚拟里程计参数初始化
void Para_OBO_Template(void)
{
	g_Para.Obo.Correction_X = 3.60281205f;
	g_Para.Obo.Correction_Y = 3.60281205f;
	g_Para.Obo.Correction_Z = 2.42391586f;
}

//Template 巡线控制参数初始化
void Para_CtrlCoef_Template(void)
{
	g_Para.TarceType.FunType = Para_TarceType_Null_Fun;//巡线方式 默认是没有巡线，常见的有：磁导航、slam和色带
	
	g_Para.CtrlCoef.k_TraceX 	= 0.10f;	//x方向纠偏系数
	g_Para.CtrlCoef.k_TraceZ 	= 0.05f;
	g_Para.CtrlCoef.VMin 		= 0;
	g_Para.CtrlCoef.VMax 		= 600;
	g_Para.CtrlCoef.k_TraceXMin = 80;
	g_Para.CtrlCoef.k_TraceXMax = 600;
	g_Para.CtrlCoef.k_TraceZMin = 40;
	g_Para.CtrlCoef.k_TraceZMax = 200;

	g_Para.CtrlCoef.BitPara.WheelFollow = 0;					//默认不是轮系跟随的巡线方式
	g_Para.CtrlCoef.BitPara.MagneticNail = 0;					//默认不是磁钉的巡线方式
	g_Para.CtrlCoef.TraceSkewSpeed = g_Para.Walk.SkewSpeed; 	//默认是行走参数中的差动速度
}
//Template 控制参数初始化
void Para_SysFucBit_Template(void)
{
	g_Para.SysFucBit.Buzzer2LightAlarm = 0;						//关闭报警指示灯兼顾运行提示
	g_Para.SysFucBit.ErrVoice = 0;								//关闭错误语音提示
	g_Para.SysFucBit.LogicErr2Emg = 0;							//关闭逻辑错误转急停
	g_Para.SysFucBit.LogicErr2Remind = 0;						//关闭逻辑错误提醒
	g_Para.SysFucBit.SelCarB = 0;								//
	g_Para.SysFucBit.AssemblyInfeed = 0;						//
	g_Para.SysFucBit.Asb_OR = 1;								//仅遥控联动
	g_Para.SysFucBit.Asb_CommDirect = 1;						//直接使用对象作为数据池
	g_Para.SysFucBit.Asb_Comm2Cmd = 0;							//控制命令来着联动通信 （具体代码待从438吸收）
	g_Para.SysFucBit.LimitWalk_Overload = 1;					//
	g_Para.SysFucBit.LimitWalk_UnbalancedLoad = 0;				//
	g_Para.SysFucBit.LimitLiftUp_Overload = 1;					//
	g_Para.SysFucBit.LimitLiftUp_UnbalancedLoad = 0;			//
}

/*输入ADC端口号配置 Template*/
bool ObjInit_ADC_IDInit_Template(void)
{
	g_ADFucs.SysCtrlVoltage.Id 		= ADCN5;

	g_ADFucs.Lift_Cable.Id		 	= ADCN_Disable;
	g_ADFucs.Lift_Cable1.Id		 	= ADCN_Disable;
	g_ADFucs.Lead_Screw_Wire.Id 	= ADCN_Disable; 
	g_ADFucs.Leg_Pressure.Id 		= ADCN_Disable;

	g_ADFucs.FL_Pressure.Id			= ADCN_Disable;
	g_ADFucs.FR_Pressure.Id 		= ADCN_Disable;
	g_ADFucs.B_Pressure.Id 			= ADCN_Disable;
	return 1;
}
/*输出DAC端口号配置 Template*/
bool ObjInit_DAC_IDInit_Template(void)
{
	g_DAFucs.KoRunCtrl.Id = DACN_Disable;
	//科润阳光举升时，需要将RatioActB改成升降速度的最大值
	g_DAFucs.KoRunCtrl.RatioActB = g_Para.Lift.SpeedPlanUp[GEARS_NUM_MAX-1];
	
	/*科润阳光2号驱动器，部分特殊项目需要用到*/
	g_DAFucs.KoRunCtrl_2.Id = DACN_Disable;
	g_DAFucs.KoRunCtrl_2.RatioActB = g_Para.Lift.SpeedPlanUp[GEARS_NUM_MAX-1];

	return 1;
}
//输入io端口号配置 Template
bool ObjInit_IOInputs_IDInit_Template(void)
{
	IO_Inputs_TypeDef* p2IOs = &g_IOInputs;

	p2IOs->PowerOn.ID 					= ICN57;		//启动开关
	p2IOs->EmgStop.ID 					= ICN56;		//急停
	p2IOs->Little_Estop.ID 				= ICN_Disable;	//小急停
	p2IOs->RemoteReceiverEmgStop.ID 	= ICN72;		//遥控器接收急停
	p2IOs->RemoteWiredSel.ID 			= ICN73;		//遥控器有线选择
	p2IOs->ExternalBrakeSel.ID 			= ICN64;		//外部抱闸选择
	p2IOs->DspModeSel.ID 				= ICN_Disable;	//选择调度控制
	p2IOs->ObsBarF.ID 					= ICN58;		//前触边
	p2IOs->ObsBarB.ID 					= ICN74;		//后触边
	p2IOs->ObsBarL.ID 					= ICN61;		//左触边
	p2IOs->ObsBarR.ID 					= ICN77;		//右触边
	
	p2IOs->AutoStartStop.ID 			= ICN_Disable;	//自动启动/暂停按钮
	p2IOs->AutoStop.ID 					= ICN_Disable;	//自动停止按钮
	p2IOs->SysReset.ID 					= ICN_Disable;	//系统复位
	p2IOs->Car_Key.ID 					= ICN_Disable;	//授权钥匙
	p2IOs->GroundChargeRetractLimit.ID	= ICN_Disable;	//地充回收到位限位信号

	p2IOs->Assembly.ID					= ICN_Disable;	//联动开关

	p2IOs->CableRoll_Emg.ID				= ICN_Disable;	//卷线盘 放线到超过限位后 触发一类故障的检测
	p2IOs->CableRoll_OutLimit.ID		= ICN_Disable;	//卷线盘 放线 限位

	p2IOs->LiftUpperLimit.ID 			= ICN_Disable;	//升降上限位
	p2IOs->LiftLowerLimit.ID 			= ICN_Disable;	//升降下限位

	//避障雷达io初始化
	p2IOs->ObsRadar_F_Err.ID 	= (IO_iCNxBit_EnumDef)IOBit_XIN8;	//前雷达故障信号
	p2IOs->ObsRadar_F_Inner.ID	= (IO_iCNxBit_EnumDef)IOBit_XIN9;	//前雷达内层触发信号
	p2IOs->ObsRadar_F_Middle.ID = (IO_iCNxBit_EnumDef)IOBit_XIN10;	//前雷达中层触发信号
	p2IOs->ObsRadar_F_Outer.ID	= (IO_iCNxBit_EnumDef)IOBit_XIN11;	//前雷达外层触发信号

	p2IOs->ObsRadar_B_Err.ID	= (IO_iCNxBit_EnumDef)IOBit_XIN15;	//后雷达故障信号
	p2IOs->ObsRadar_B_Inner.ID	= (IO_iCNxBit_EnumDef)IOBit_XIN14;	//后雷达内层触发信号
	p2IOs->ObsRadar_B_Middle.ID = (IO_iCNxBit_EnumDef)IOBit_XIN13;	//后雷达中层触发信号
	p2IOs->ObsRadar_B_Outer.ID	= (IO_iCNxBit_EnumDef)IOBit_XIN12;	//后雷达外层触发信号

	p2IOs->ObsRadar_L_Err.ID	= (IO_iCNxBit_EnumDef)IOBit_XIN47;	//左雷达故障信号
	p2IOs->ObsRadar_L_Inner.ID	= (IO_iCNxBit_EnumDef)IOBit_XIN46;	//左雷达内层触发信号
	p2IOs->ObsRadar_L_Middle.ID = (IO_iCNxBit_EnumDef)IOBit_XIN45;	//左雷达中层触发信号
	p2IOs->ObsRadar_L_Outer.ID	= (IO_iCNxBit_EnumDef)IOBit_XIN44;	//左雷达外层触发信号

	p2IOs->ObsRadar_R_Err.ID	= (IO_iCNxBit_EnumDef)IOBit_XIN51;	//右雷达故障信号
	p2IOs->ObsRadar_R_Inner.ID	= (IO_iCNxBit_EnumDef)IOBit_XIN50;	//右雷达内层触发信号
	p2IOs->ObsRadar_R_Middle.ID = (IO_iCNxBit_EnumDef)IOBit_XIN49;	//右雷达中层触发信号
	p2IOs->ObsRadar_R_Outer.ID	= (IO_iCNxBit_EnumDef)IOBit_XIN48;	//右雷达外层触发信号
	
	p2IOs->ObsRadar_F_Inclined_Err.ID		= (IO_iCNxBit_EnumDef)ICN_Disable;	//前斜面雷达故障信号
	p2IOs->ObsRadar_F_Inclined_Inner.ID		= (IO_iCNxBit_EnumDef)ICN_Disable;	//前斜面雷达内层触发信号
	p2IOs->ObsRadar_F_Inclined_Middle.ID	= (IO_iCNxBit_EnumDef)ICN_Disable;	//前斜面雷达中层触发信号
	p2IOs->ObsRadar_F_Inclined_Outer.ID		= (IO_iCNxBit_EnumDef)ICN_Disable;	//前斜面雷达外层触发信号
	
	p2IOs->ObsRadar_B_Inclined_Err.ID		= (IO_iCNxBit_EnumDef)ICN_Disable;	//后斜面雷达故障信号
	p2IOs->ObsRadar_B_Inclined_Inner.ID		= (IO_iCNxBit_EnumDef)ICN_Disable;	//后斜面雷达内层触发信号
	p2IOs->ObsRadar_B_Inclined_Middle.ID	= (IO_iCNxBit_EnumDef)ICN_Disable;	//后斜面雷达中层触发信号
	p2IOs->ObsRadar_B_Inclined_Outer.ID		= (IO_iCNxBit_EnumDef)ICN_Disable;	//后斜面雷达外层触发信号

	return 1;
}

//输出io端口号配置 Template
bool ObjInit_IOOutputs_IDInit_Template(void)
{
	IO_Outputs_TypeDef* p2IO = &g_IOOutputs;

	p2IO->MainDCDC.ID 				= OCN_Disable;	//主DCDC
	p2IO->CtrlVoltageBypass.ID		= OCN50;		//控制电压旁路
	p2IO->PowerOnBypass.ID			= OCN_Disable;	//启动开关旁路

	p2IO->ChargingMC.ID				= OCN_Disable;	//充电极板控制
	p2IO->GroundChargingExtend.ID	= OCN_Disable;	//地充电缸 伸出
	p2IO->GroundChargingRetract.ID	= OCN_Disable;	//地充电缸 缩回
	p2IO->GroundChargingPower.ID	= OCN_Disable;	//地充电缸 供电
	
	
	p2IO->Precharge.ID				= OCN32;		//预充电
	p2IO->MainContactor.ID			= OCN44;		//主接触器
	p2IO->MotorPrecharge.ID			= OCN_Disable;	//驱动器预充电
	p2IO->MotorContactor.ID			= OCN_Disable;	//驱动器主接触器
	p2IO->CtrllPower.ID				= OCN_Disable;	//控制电源
	p2IO->LightPower.ID				= OCN34;		//电源指示灯
	p2IO->LightSystem.ID			= OCN49;		//系统指示灯
	p2IO->LightAlarm.ID				= OCN35;		//报警指示灯
	p2IO->LampRed.ID				= OCN31_2;		//红色轮廓灯
	p2IO->LampGreen.ID				= OCN31_3;		//绿色轮廓灯
	p2IO->LampBlue.ID				= OCN31_4;		//蓝色轮廓灯
	p2IO->LampRed2.ID				= OCN46_2;		//红色轮廓灯
	p2IO->LampGreen2.ID				= OCN46_3;		//绿色轮廓灯
	p2IO->LampBlue2.ID				= OCN46_4;		//蓝色轮廓灯
	p2IO->ExplosionProof_LED_1.ID	= OCN_Disable;	//防爆LED
	p2IO->ExplosionProof_LED_2.ID	= OCN_Disable;	//
	p2IO->ExplosionProof_LED_3.ID	= OCN_Disable;	//
	p2IO->ExplosionProof_LED_4.ID	= OCN_Disable;	//
	
	
	p2IO->Buzzer.ID					= OCN43;		//蜂鸣器
	p2IO->LightCaution.ID			= OCN_Disable;	//警示灯
	p2IO->AGVled_StartStop.ID		= OCN52;		//AGV启动停止指示灯
	p2IO->AGVled_Reset.ID			= OCN37;		//AGV复位指示灯
	p2IO->Horn.ID					= OCN27;		//喇叭
	p2IO->Lighting.ID				= OCN_Disable;	//照明灯
	
	p2IO->CableRoll_ForcePowerOn.ID	= OCN_Disable;	//卷线盘 出现一类报警后 强制供电的继电器
	p2IO->CableRoll_Reel.ID			= OCN_Disable;	//卷线盘收线


	/*平台升降：泰坦三点液压类型 电磁阀控制*/
	p2IO->Platform_Titan_UpLF.ID 		= OCN_Disable;	//左前平台上升电磁阀
	p2IO->Platform_Titan_DownLF.ID 		= OCN_Disable;	//左前平台下降电磁阀
	p2IO->Platform_Titan_UpRF.ID 		= OCN_Disable;	//右前平台上升电磁阀
	p2IO->Platform_Titan_DownRF.ID 		= OCN_Disable;	//右前平台下降电磁阀
	p2IO->Platform_Titan_UpB.ID 		= OCN_Disable;	//后平台上升电磁阀
	p2IO->Platform_Titan_DownB.ID 		= OCN_Disable;	//后平台下降电磁阀

	//仅四点时有效
	p2IO->Platform_Titan_UpLB.ID 	= OCN_Disable;
	p2IO->Platform_Titan_DownLB.ID 	= OCN_Disable;
	p2IO->Platform_Titan_UpRB.ID 	= OCN_Disable;
	p2IO->Platform_Titan_DownRB.ID 	= OCN_Disable;

	/*平台升降：剪刀架单杠液压类型 电磁阀控制*/
	p2IO->Platform_Scissors_Down.ID		= OCN_Disable;	//后平台下降电磁阀
	/*平台升降：联合泵类型 电磁阀控制*/
	p2IO->Platform_UnitePump_UpLF.ID	= OCN_Disable;	//左前上升阀
	p2IO->Platform_UnitePump_UpRF.ID	= OCN_Disable;	//右前上升阀	
	p2IO->Platform_UnitePump_UpLB.ID	= OCN_Disable;	//左后上升阀
	p2IO->Platform_UnitePump_UpRB.ID	= OCN_Disable;	//右后上升阀
	p2IO->Platform_UnitePump_DownLF.ID	= OCN_Disable;	//左前下降阀
	p2IO->Platform_UnitePump_DownRF.ID	= OCN_Disable;	//右前下降阀	
	p2IO->Platform_UnitePump_DownLB.ID	= OCN_Disable;	//左后下降阀
	p2IO->Platform_UnitePump_DownRB.ID	= OCN_Disable;	//右后下降阀

	/*平台升降：双泵四点液压类型 电磁阀控制*/
	p2IO->Platform_DoublePumpQuad_DownLF.ID = OCN_Disable;	//左前下降阀
	p2IO->Platform_DoublePumpQuad_DownRF.ID = OCN_Disable;	//右前下降阀	
	p2IO->Platform_DoublePumpQuad_DownLB.ID = OCN_Disable;	//左后下降阀
	p2IO->Platform_DoublePumpQuad_DownRB.ID = OCN_Disable;	//右后下降阀

	p2IO->Platform_DoublePumpQuad_ThrottleValveF.ID = OCN_Disable;	/*前侧节流阀，开启后高度固定 无法升降*/
	p2IO->Platform_DoublePumpQuad_ThrottleValveB.ID = OCN_Disable;	/*后侧节流阀，开启后高度固定 无法升降*/

	/*悬挂升降：泰坦三点液压类型 电磁阀控制*/
	p2IO->Absorption_Titan_UpLF.ID 		= OCN_Disable;	//左前悬挂上升电磁阀
	p2IO->Absorption_Titan_DownLF.ID	= OCN_Disable;	//左前悬挂下降电磁阀
	p2IO->Absorption_Titan_UpRF.ID 		= OCN_Disable;	//右前悬挂上升电磁阀
	p2IO->Absorption_Titan_DownRF.ID	= OCN_Disable;	//右前悬挂下降电磁阀
	p2IO->Absorption_Titan_UpB.ID 		= OCN_Disable;	//后悬挂上升电磁阀
	p2IO->Absorption_Titan_DownB.ID		= OCN_Disable;	//后悬挂下降电磁阀

	/*抱闸*/
	p2IO->MoveBrake.ID			= OCN47;			//移动抱闸
	p2IO->PushRollerBrake.ID	= OCN_Disable;		//推拉机构抱闸
	p2IO->LiftBrake.ID			= OCN_Disable;		//平台机构抱闸

	p2IO->CoolingFan.ID			= OCN26;			//散热风扇

	p2IO->PowerOffDealy.ID		= OCN_Disable;		//掉电延时继电器
	p2IO->PowerOffDealy_Bms.ID	= OCN_Disable;		//掉电延时继电器 差速小车专用

	//科润驱动器控制IO接口配置
	p2IO->KoRunReversal.ID 		= OCN_Disable;	//科润驱动器反转
	p2IO->KoRunBrake.ID			= OCN_Disable;	//科润驱动器刹车
	p2IO->KoRunSysPower.ID		= OCN_Disable;	//科润驱动器系统电源
	p2IO->KoRunContactor.ID 	= OCN_Disable;	//科润驱动器接触器
	p2IO->KoRun_2_Reversal.ID	= OCN_Disable;	//2号科润驱动器反转
	p2IO->KoRun_2_Brake.ID		= OCN_Disable;	//2号科润驱动器刹车
	p2IO->KoRun_2_SysPower.ID	= OCN_Disable;	//2号科润驱动器系统电源
	p2IO->KoRun_2_Contactor.ID 	= OCN_Disable;	//2号科润驱动器接触器

	//避障雷达io初始化
	p2IO->ObsRadar_F_Ctrl1.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT27;	//前雷切区控制1
	p2IO->ObsRadar_F_Ctrl2.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT26;	//前雷切区控制2
	p2IO->ObsRadar_F_Ctrl3.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT25;	//前雷切区控制3
	p2IO->ObsRadar_F_Ctrl4.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT24;	//前雷切区控制4

	p2IO->ObsRadar_B_Ctrl1.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT33;	//后雷切区控制1
	p2IO->ObsRadar_B_Ctrl2.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT32;	//后雷切区控制2
	p2IO->ObsRadar_B_Ctrl3.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT31;	//后雷切区控制3
	p2IO->ObsRadar_B_Ctrl4.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT30;	//后雷切区控制4
	
	p2IO->ObsRadar_L_Ctrl1.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT4;	//左雷切区控制1
	p2IO->ObsRadar_L_Ctrl2.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT5;	//左雷切区控制2
	p2IO->ObsRadar_L_Ctrl3.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT7;	//左雷切区控制3
	p2IO->ObsRadar_L_Ctrl4.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT8;	//左雷切区控制4
                                                                             
	p2IO->ObsRadar_R_Ctrl1.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT14;	//右雷切区控制1
	p2IO->ObsRadar_R_Ctrl2.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT15;	//右雷切区控制2
	p2IO->ObsRadar_R_Ctrl3.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT17;	//右雷切区控制3
	p2IO->ObsRadar_R_Ctrl4.ID = (IO_oCNxBit_EnumDef)IOBit_YOUT18;	//右雷切区控制4

	p2IO->ObsRadar_F_Inclined_Ctrl1.ID = (IO_oCNxBit_EnumDef)OCN_Disable;	//前斜面雷切区控制1
	p2IO->ObsRadar_F_Inclined_Ctrl2.ID = (IO_oCNxBit_EnumDef)OCN_Disable;	//前斜面雷切区控制2
	p2IO->ObsRadar_F_Inclined_Ctrl3.ID = (IO_oCNxBit_EnumDef)OCN_Disable;	//前斜面雷切区控制3
	p2IO->ObsRadar_F_Inclined_Ctrl4.ID = (IO_oCNxBit_EnumDef)OCN_Disable;	//前斜面雷切区控制4

	p2IO->ObsRadar_B_Inclined_Ctrl1.ID = (IO_oCNxBit_EnumDef)OCN_Disable;	//后斜面雷切区控制1
	p2IO->ObsRadar_B_Inclined_Ctrl2.ID = (IO_oCNxBit_EnumDef)OCN_Disable;	//后斜面雷切区控制2
	p2IO->ObsRadar_B_Inclined_Ctrl3.ID = (IO_oCNxBit_EnumDef)OCN_Disable;	//后斜面雷切区控制3
	p2IO->ObsRadar_B_Inclined_Ctrl4.ID = (IO_oCNxBit_EnumDef)OCN_Disable;	//后斜面雷切区控制4

	//安川m3板卡电源
	p2IO->ANC.ID = OCN_Disable;

	return 1;
}

//外设通信配置对象初始化 Template
void EQP_SerialExpObjInit_Template(void)
{
	//导航转接板使能后，才允生效
	if(g_Para.EQPCommSource.EN.SerialExp == 1)
	{
		g_Para.SerialExpObjSource.BMS					= 0;
		g_Para.SerialExpObjSource.Voice					= 0;
		g_Para.SerialExpObjSource.AD					= 0;
		g_Para.SerialExpObjSource.AD1					= 0;
		g_Para.SerialExpObjSource.Thermodetector		= 0;
		g_Para.SerialExpObjSource.Inclinometer_Rion		= 0;
		g_Para.SerialExpObjSource.MagF					= 0;
		g_Para.SerialExpObjSource.MagB					= 0;
		g_Para.SerialExpObjSource.MagL					= 0;
		g_Para.SerialExpObjSource.MagR					= 0;
		g_Para.SerialExpObjSource.RFID					= 0;
		g_Para.SerialExpObjSource.PGVUpward				= 0;
		g_Para.SerialExpObjSource.PGVDownward			= 0;
		g_Para.SerialExpObjSource.PGVUpward_F			= 0;
		g_Para.SerialExpObjSource.PGVUpward_B			= 0;
		g_Para.SerialExpObjSource.PGVDownward_F			= 0;
		g_Para.SerialExpObjSource.PGVDownward_B			= 0;

		g_Para.SerialExpObjSource.CommConv				= 0;
		g_Para.SerialExpObjSource.IPC					= 0;
		g_Para.SerialExpObjSource.LaserSpot				= 0;

		g_Para.SerialExpObjSource.Hinson_Radar_Front	= 0;
		g_Para.SerialExpObjSource.Hinson_Radar_Rear		= 0;
		g_Para.SerialExpObjSource.Hinson_Radar_Left		= 0;
		g_Para.SerialExpObjSource.Hinson_Radar_Righ		= 0;
		g_Para.SerialExpObjSource.Hinson_Radar_FrontUp	= 0;
		g_Para.SerialExpObjSource.Hinson_Radar_RearUp	= 0;

		g_Para.SerialExpObjSource.ME15_Near				= 0;
		g_Para.SerialExpObjSource.ME15_Far				= 0;
	}
	//第二导航转接板使能后，才允生效
	if(g_Para.EQPCommSource.EN.SerialExp_2 == 1)
	{
		g_Para.SerialExp_2_ObjSource.BMS					= 0;
		g_Para.SerialExp_2_ObjSource.Voice					= 0;
		g_Para.SerialExp_2_ObjSource.AD						= 0;
		g_Para.SerialExp_2_ObjSource.AD1					= 0;
		g_Para.SerialExp_2_ObjSource.Thermodetector			= 0;
		g_Para.SerialExp_2_ObjSource.Inclinometer_Rion		= 0;
		g_Para.SerialExp_2_ObjSource.MagF					= 0;
		g_Para.SerialExp_2_ObjSource.MagB					= 0;
		g_Para.SerialExp_2_ObjSource.MagL					= 0;
		g_Para.SerialExp_2_ObjSource.MagR					= 0;
		g_Para.SerialExp_2_ObjSource.RFID					= 0;
		g_Para.SerialExp_2_ObjSource.PGVUpward				= 0;
		g_Para.SerialExp_2_ObjSource.PGVDownward			= 0;
		g_Para.SerialExp_2_ObjSource.PGVUpward_F			= 0;
		g_Para.SerialExp_2_ObjSource.PGVUpward_B			= 0;
		g_Para.SerialExp_2_ObjSource.PGVDownward_F			= 0;
		g_Para.SerialExp_2_ObjSource.PGVDownward_B			= 0;

		g_Para.SerialExp_2_ObjSource.CommConv				= 0;
		g_Para.SerialExp_2_ObjSource.IPC					= 0;
		g_Para.SerialExp_2_ObjSource.LaserSpot				= 0;

		g_Para.SerialExp_2_ObjSource.Hinson_Radar_Front	= 0;
		g_Para.SerialExp_2_ObjSource.Hinson_Radar_Rear		= 0;
		g_Para.SerialExp_2_ObjSource.Hinson_Radar_Left		= 0;
		g_Para.SerialExp_2_ObjSource.Hinson_Radar_Righ		= 0;
		g_Para.SerialExp_2_ObjSource.Hinson_Radar_FrontUp	= 0;
		g_Para.SerialExp_2_ObjSource.Hinson_Radar_RearUp	= 0;

		g_Para.SerialExp_2_ObjSource.ME15_Near				= 0;
		g_Para.SerialExp_2_ObjSource.ME15_Far				= 0;
	}
}

//外设通信配置对象初始化 Template
void Para_EQPInit_Template(void)
{
	//开发板作为从站的设备
	/*				modbus设备						挂载端口		站号	*/
	Para_EQPInit(Para_EQP_HMI,					CCFCN13,		0X0A);
	Para_EQPInit(Para_EQP_DSP,					CCFCN_Disable,	0X0A);
	Para_EQPInit(Para_EQP_API_Slave,			CCFCN_Disable,	0X0A);

	/*已改为用前面初始化的参数变量来初始化
	Para_EQPInit(Para_EQP_Assembly_Slave,		CCFCN_Disable,	0X0A);
	Para_EQPInit(Para_EQP_Assembly_Master,		CCFCN_Disable,	0X0A);*/
	Para_EQPInit(Para_EQP_Assembly_Slave,		g_Para.Asb.CN_Slave, 0X0A);
	Para_EQPInit(Para_EQP_Assembly_Master,		g_Para.Asb.CN_Master, 0X0A);

	/*固定联动角色配置示例
	{
		if (g_Para.EqpID == EqpID_46501)
		{
			Para_EQPInit(Para_EQP_Assembly_Slave, CCFCN_Disable, 0X0A);
			Para_EQPInit(Para_EQP_Assembly_Master, CCFCN21, 0X0A);
		}
		else if (g_Para.EqpID == EqpID_46502)
		{
			Para_EQPInit(Para_EQP_Assembly_Slave, CCFCN21, 0X0A);
			Para_EQPInit(Para_EQP_Assembly_Master, CCFCN_Disable, 0X0A);
		}
		else {}
	}*/
	//开发板作为主站的设备
	/*				modbus设备						挂载端口		站号	*/
	Para_EQPInit(Para_EQP_WHA,					CCFCN22,		0x7E);
	Para_EQPInit(Para_EQP_BMS_CommConv,			CCFCN_Disable,	0X04);
	Para_EQPInit(Para_EQP_BMS_CATL,				CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_BMS_MCB,				CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_Voice,				CCFCN_Disable,	0X06);
	Para_EQPInit(Para_EQP_AD,					CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_AD1,					CCFCN_Disable,	0X02);
	Para_EQPInit(Para_EQP_DA,					CCFCN_Disable,	0X03);
	Para_EQPInit(Para_EQP_Thermodetector,		CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_Inclinometer_Rion,	CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_MagF,					CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_MagB,					CCFCN_Disable,	0X02);
	Para_EQPInit(Para_EQP_MagL,					CCFCN_Disable,	0X03);
	Para_EQPInit(Para_EQP_MagR,					CCFCN_Disable,	0X04);
	Para_EQPInit(Para_EQP_RFID,					CCFCN_Disable,	0X05);
	Para_EQPInit(Para_EQP_RFID_Up,				CCFCN_Disable,	0X06);
	Para_EQPInit(Para_EQP_PGVUpward,			CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_PGVDownward,			CCFCN_Disable,	0X02);
	Para_EQPInit(Para_EQP_PGVUpward_F,			CCFCN_Disable,	0X03);
	Para_EQPInit(Para_EQP_PGVUpward_B,			CCFCN_Disable,	0X04);
	Para_EQPInit(Para_EQP_PGVDownward_F,		CCFCN_Disable,	0X05);
	Para_EQPInit(Para_EQP_PGVDownward_B,		CCFCN_Disable,	0X06);
	Para_EQPInit(Para_EQP_SerialExp,			CCFCN_Disable,	0X0A);
	Para_EQPInit(Para_EQP_SerialExp_2,			CCFCN_Disable,	0X0A);
	Para_EQPInit(Para_EQP_CommConv,				CCFCN_Disable,	0X03);
	Para_EQPInit(Para_EQP_CommConv2,			CCFCN_Disable,	0X05);
	Para_EQPInit(Para_EQP_IPC,					CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_Roll_Controller,		CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_WHB,					CCFCN_Disable,	0x7F);
	Para_EQPInit(Para_EQP_LaserSpot,			CCFCN_Disable,	0X03);

	Para_EQPInit(Para_EQP_Hinson_Radar_Front,	CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_Hinson_Radar_Rear,	CCFCN_Disable,	0X02);
	Para_EQPInit(Para_EQP_Hinson_Radar_Left,	CCFCN_Disable,	0X03);
	Para_EQPInit(Para_EQP_Hinson_Radar_Righ,	CCFCN_Disable,	0X04);
	Para_EQPInit(Para_EQP_Hinson_Radar_FrontUp,	CCFCN_Disable,	0X05);
	Para_EQPInit(Para_EQP_Hinson_Radar_RearUp,	CCFCN_Disable,	0X06);
	Para_EQPInit(Para_EQP_ME15_Near,			CCFCN_Disable,	0X03);
	Para_EQPInit(Para_EQP_ME15_Far,				CCFCN_Disable,	0X05);
	Para_EQPInit(Para_PLC_Estop,				CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_API_Master,			CCFCN_Disable,	0X0A);

	Para_EQPInit(Para_EQP_BMS_KXD,				CCFCN_Disable,	0X01);

	Para_EQPInit(Para_EQP_BJF_PGV_Upward,		CCFCN_Disable, 	0X03);
	Para_EQPInit(Para_EQP_BJF_PGV_Downward,		CCFCN_Disable,	0X06);
	Para_EQPInit(Para_EQP_BJF_PGV_Upward_F,		CCFCN_Disable,	0X01);/*配置1 对应PGV站号0 */
	Para_EQPInit(Para_EQP_BJF_PGV_Upward_B,		CCFCN_Disable,	0X02);
	Para_EQPInit(Para_EQP_BJF_PGV_Downward_F,	CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_BJF_PGV_Downward_B,	CCFCN_Disable,	0X02);
	Para_EQPInit(Para_EQP_ANC,					CCFCN_Disable,	0X0A);
	Para_EQPInit(Para_EQP_DBSET,				CCFCN_Disable,	0X01);
	Para_EQPInit(Para_EQP_PLC,					CCFCN_Disable,		0X14);
}
//外设通信配置对象初始化 Template
bool EQPInit_Template(void)
{
	/*标准外设初始化的内容通过函数 Para_EQPInit_Template 完成，此处进行非标设备初始化*/
	/*例如：*/
//	//HMI
//	EQPSlvInit(&EQPSlvs.HMI, Baud_115200, 7, CCFRTU, HMI_Buf, sizeof(HMI_Buf), CCFCN13);

//	//遥控器A
//	EQPMstInit(&EQPMsts.WHA, Baud_115200, 3, CCFRTU, 100, 50, 1, 126, MD_FR_MHR_RDHR, OP_REG,
//		EQP_WHA.RD, 194, sizeof(EQP_WHA.RD) / 2, EQP_WHA.WD, 230, sizeof(EQP_WHA.WD) / 2,
//		NORMAL_QUE, CCFCN22);
	return 1;
}

/*数据管道建立的特殊功能开关*/
void Para_AppInSource_Template(void)
{
	/*输入管道*/
	{
		/*高度数据来源*/
		g_Para.AppInSource.FucBit.CommConvLiftHeight 		= 0;	/*升降高度的数据从卡式转接模块中得到*/
		g_Para.AppInSource.FucBit.CommConvAbsorptionHeight 	= 0;	/*悬挂高度的数据从卡式转接模块中得到*/ 
		g_Para.AppInSource.FucBit.AD_MCBLiftHeight 			= 1;	/*升降高度的数据从AD模块中得到*/ 
		g_Para.AppInSource.FucBit.CommConvDouble			= 0;	/*通过2个卡式转接板分别获取角度和磁致数据*/
		/*避障雷达数据来源*/
		g_Para.AppInSource.FucBit.ModBusROA 				= 0;	/*通过通过IO信号控制雷达切区和避障*/
	}

	/*输出管道*/
	{
		
	}
}

//行走加速限制 对象初始化
bool ObjInit_WalkLimit_Template(WalkLimit_TypeDef* p)
{
	*(u16 *)(&p->ParaBit) = 0;
	p->ParaBit.Rely_WalkGearAndWeight = 0;					//默认关闭加速度限制功能

	p->AccMax = g_Para.Walk.Smooth.VxAcc;
	p->AccMin = p->AccMax / 2;								//限制到一半
	p->Para_WalkGear = 3;
	p->Para_Weight = 20;

	p->Weight = p->Para_Weight + 1;                         //初始为较大值 使得启动时默认重量为限速的状态
	return 1;
}

//行走档位限制 对象初始化
bool ObjInit_WalkGearLimit_Template(void)
{
	g_WalkGearLimit.Para_LimitGear = 6;
	g_WalkGearLimit.Para_RelyMax = 65535;
	g_WalkGearLimit.Para_RelyMin = 65535;//默认将限制档位设置到最大档、吨位参数设置的很大，即使得限制不会发生
	return 1;
}

//行走档位限制 根据高度 对象初始化
bool ObjInit_WalkGearLimit_Hg_Template(void)
{
	g_WalkGearLimit_Hg.Para_LimitGear = 6;
	g_WalkGearLimit_Hg.Para_RelyMax = 65535;
	g_WalkGearLimit_Hg.Para_RelyMin = 0;//默认将限制档位设置到最大档、吨位参数设置的很大，即使得限制不会发生
	return 1;
}

//升降档位限制 对象初始化
bool ObjInit_LiftGearLimit_Template(void)
{
	g_LiftGearLimit.Para_LimitGear = 6;
	g_LiftGearLimit.Para_RelyMax = 65535;
	g_LiftGearLimit.Para_RelyMin = 65535;//默认将限制档位设置到最大档、吨位参数设置的很大，即使得限制不会发生
	return 1;
}

/*自动对中功能初始化*/
void Para_AutoCentre_Template(void)
{
	/*自动对中功能类型*/
	g_Para.AutoCentre.FunType = Para_AutoCentre_ToolPGV;
	/*自动对中的传感器输入*/
	g_Para.AutoCentre.IntPutType = Para_AutoCentre_HF_SoleTool;
}

//行走电机速度平滑参数初始化
bool Para_WalkMotorSmooth_Template()
{
	WalkMotorSmooth_TypeDef* p = &g_WMSM;
	char i = 0;
	f32 temp_AccAdsorb = 50;								//快速吸附加速增量
	f32 temp_DecAdsorb = 60;								//快速吸附减速增量
	f32 temp_AdsorbRange = 80;								//吸附范围

	//打开/关闭
	p->En = 0;
	//增量参数
	p->Acc = 20;
	p->Dec = 40;

	//所有轴的平滑对象初始化
	for (i = 0;i < ACC_WALKMOTOR_MAX;i++)
	{
		DataSmooth_Init(&p->SM[i], p->Acc, p->Dec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	}

	return 1;
}

//差动速度平滑参数初始化
bool Para_OffsetMotorSmooth_Template()
{
	WalkMotorSmooth_TypeDef* p = &g_OSSM;
	char i = 0;
	f32 temp_AccAdsorb = 5;									//快速吸附加速增量
	f32 temp_DecAdsorb = 10;								//快速吸附减速增量
	f32 temp_AdsorbRange = 30;								//吸附范围

	//打开/关闭
	p->En = 0;
	//增量参数
	p->Acc = 10;
	p->Dec = 20;

	//所有轴的平滑对象初始化
	for (i = 0;i < ACC_WALKMOTOR_MAX;i++)
	{
		DataSmooth_Init(&p->SM[i], p->Acc, p->Dec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	}

	return 1;
}

//功能使能参数初始化 通用
bool Para_FunctionSwitch_Template(void)
{
	g_Para.FunctionSwitch.BarEn.Front = 1;
	g_Para.FunctionSwitch.BarEn.Back = 1;
	g_Para.FunctionSwitch.BarEn.Left = 1;
	g_Para.FunctionSwitch.BarEn.Right = 1;
	g_Para.FunctionSwitch.RadarEn.Front = 1;
	g_Para.FunctionSwitch.RadarEn.Back = 1;
	g_Para.FunctionSwitch.RadarEn.Left = 1;
	g_Para.FunctionSwitch.RadarEn.Right = 1;
	return 1;
}

bool Para_EmgJudge_Template(void)
{
	//故障判断参数
	g_Para.EmgJudge.TimeOutJudge = 2000;
	g_Para.EmgJudge.ModeSwitchTimeOutJudge = 60000;
	//轮系角度报警参数
	g_Para.EmgJudge.WheelAngleAlarmRight = abs(g_Para.Walk.CtrlAngMax_Right) + 1;
	g_Para.EmgJudge.WheelAngleAlarmLeft = abs(g_Para.Walk.CtrlAngMax_Left) + 1;
	//磁导航离线报警参数
	g_Para.EmgJudge.MagOutLineTime = 10000;
	//磁条离线里程计报警参数
	g_Para.EmgJudge.MagOutLineDistance = 200;

	//调度系统超时
	g_Para.EmgJudge.TimeOutDsp = 10000;
	//色带脱线判定时间
	g_Para.EmgJudge.ColourOutLineTime = 10000;
	//Slam通讯超时
	g_Para.EmgJudge.TimeOutSlam = 10000;

	//预留参数 初始化为0
	memset(g_Para.EmgJudge.NC, 0, sizeof(g_Para.EmgJudge.NC));

	//偏载报警检出参数
	g_Faults.Para.UnbalancedLoad_WeightMin = 15;
	g_Faults.Para.UnbalancedLoad_HeightMin = 20;
	
	g_Faults.Para.ButtonTimeOutJudge = 8000;				//按钮切换超时时间 ms
	return 1;
}

//联动传感器标定参数初始化
bool Para_AsbSensorCal_Template(void)
{
	//激光测距传感器标定参数
	g_AssemlySensorData.LaserDis_L.Para.Org1 = 1064;
	g_AssemlySensorData.LaserDis_L.Para.Org2 = 3679;
	g_AssemlySensorData.LaserDis_L.Para.Act1 = 500;
	g_AssemlySensorData.LaserDis_L.Para.Act2 = 1500;
	g_AssemlySensorData.LaserDis_L.Para.Offset = 51;

	g_AssemlySensorData.LaserDis_R.Para.Org1 = 1121;
	g_AssemlySensorData.LaserDis_R.Para.Org2 = 3728;
	g_AssemlySensorData.LaserDis_R.Para.Act1 = 500;
	g_AssemlySensorData.LaserDis_R.Para.Act2 = 1500;
	g_AssemlySensorData.LaserDis_R.Para.Offset = 51;

	//光斑传感器
	g_AssemlySensorData.SpotDis.Para.Org1 = 92;//传感器安装盒子螺丝向下 偏插头一侧
	g_AssemlySensorData.SpotDis.Para.Org2 = 126;
	g_AssemlySensorData.SpotDis.Para.Act1 = 0;
	g_AssemlySensorData.SpotDis.Para.Act2 = 92.5;

	return 1;
}

//多倍通设备ID初始化
bool ObjInit_Doublecom_Template(DBCOMSET_Typedef_Template* p)
{
//	switch (g_Para.EqpID)
//	{
//	case EqpID_54601:
//		{
//			p->SendCmdObj.Id[0] = '0';
//			p->SendCmdObj.Id[1] = '1';
//		}
//		break;
//	case EqpID_54602:
//		{
//			p->SendCmdObj.Id[0] = '0';
//			p->SendCmdObj.Id[1] = '2';
//		}
//		break;
//	case EqpID_54603:
//		{
//			p->SendCmdObj.Id[0] = '0';
//			p->SendCmdObj.Id[1] = '1';
//		}
//		break;
//	case EqpID_54604:
//		{
//			p->SendCmdObj.Id[0] = '0';
//			p->SendCmdObj.Id[1] = '2';
//		}
//		break;
//	case EqpID_54605:
//		{
//			p->SendCmdObj.Id[0] = '0';
//			p->SendCmdObj.Id[1] = '1';
//		}
//		break;
//	case EqpID_54606:
//		{
//			p->SendCmdObj.Id[0] = '0';
//			p->SendCmdObj.Id[1] = '2';
//		}
//		break;
//	default:
//		break;
//	}
	
	//初始化多倍通配置超时时间为15S
	p->FeedBackTimeOut = 15000;
	return 1;
}

//联动cbb初始化
bool ObjInit_Assembly_Template(AsbCtrl_TypeDef* p)
{
	p->Comm.SlaveDataLength = sizeof(AsbCommSlave_TypeDef) / 2;//除以2是因为是u16类型
	p->Comm.MasterDataLength = sizeof(AsbCommMaster_TypeDef) / 2;

	/*Iam参数需在项目配置文件中初始化
	//如果角度固定在项目配置中初始化
	if (g_Para.EqpID == EqpID_43801)
	{
		p->Para.Iam = AsbCarStrNo_1;
		p->Para.Role = AsbRole_Master;
		p->Para.SiteNo = AsbSiteNo_FB;
		p->Para.Seat = AsbSiteSeat_2;//主车在后面
	}
	else if (g_Para.EqpID == EqpID_43802)
	{
		p->Para.Iam = AsbCarStrNo_2;
		p->Para.Role = AsbRole_Slave;
		p->Para.SiteNo = AsbSiteNo_FB;
		p->Para.Seat = AsbSiteSeat_1;
	}
	else {}*/

	/*如果是自适应则在运行中正确配置
	if (g_Para.EqpID == EqpID_46501)
	{
	p->Para.Iam = AsbCarStrNo_1;
	p->Para.Role = AsbRole_Master;
	p->Para.Seat = AsbSiteSeat_2;//主车在后面
	}
	else
	{
		p->Para.Iam = AsbCarStrNo_2;
		p->Para.Role = AsbRole_Slave;
		p->Para.Seat = AsbSiteSeat_1;
	}*/
	p->Para.SiteNo = AsbSiteNo_LR;
	
	//阵型表
	p->Para.Seats[0] = AsbSiteSeat_2;
	p->Para.Seats[1] = AsbSiteSeat_1;

	//强制参数
	p->Para.Bit.ForceAsb_OverJdg = 0;
	p->Para.Bit.ForceAsb_OverOtherCar = 0;
	p->Para.Bit.ForceModeEnter = 0;
	p->Para.Bit.ForceOverErr = 0;

	//半长参数
	p->Para.HalfCar.Front = 1000;
	p->Para.HalfCar.Back = 1000;
	p->Para.HalfCar.Left = 600;
	p->Para.HalfCar.Right = 600;

	//联动超低车本车特殊的半长参数 轮系到车体边缘的
	p->Para.HalfCar_Edge.Front = 651;
	p->Para.HalfCar_Edge.Back = 1379;
	p->Para.HalfCar_Edge.Left = 606;
	p->Para.HalfCar_Edge.Right = 606;
	
	//联动超低车从车特殊的半长参数 轮系到车体边缘的
	p->Para.HalfCar_Edge_Slave.Front = 651;
	p->Para.HalfCar_Edge_Slave.Back = 1379;
	p->Para.HalfCar_Edge_Slave.Left = 606;
	p->Para.HalfCar_Edge_Slave.Right = 606;
	
	//联动超低车从车特殊的半长参数数组，根据所有联动车参数设定 轮系到车体边缘的
	p->Para.HalfCar_Edge_Slave_List[0].Front = 651;
	p->Para.HalfCar_Edge_Slave_List[0].Back  = 1325;
	p->Para.HalfCar_Edge_Slave_List[0].Left  = 606;
	p->Para.HalfCar_Edge_Slave_List[0].Right = 606;

	p->Para.HalfCar_Edge_Slave_List[1].Front = 651;
	p->Para.HalfCar_Edge_Slave_List[1].Back  = 1325;
	p->Para.HalfCar_Edge_Slave_List[1].Left  = 606;
	p->Para.HalfCar_Edge_Slave_List[1].Right = 606;

	p->Para.HalfCar_Edge_Slave_List[2].Front = 651;
	p->Para.HalfCar_Edge_Slave_List[2].Back  = 1325;
	p->Para.HalfCar_Edge_Slave_List[2].Left  = 606;
	p->Para.HalfCar_Edge_Slave_List[2].Right = 606;

	p->Para.HalfCar_Edge_Slave_List[3].Front = 651;
	p->Para.HalfCar_Edge_Slave_List[3].Back  = 1325;
	p->Para.HalfCar_Edge_Slave_List[3].Left  = 606;
	p->Para.HalfCar_Edge_Slave_List[3].Right = 606;

	p->Para.HalfCar_Edge_Slave_List[4].Front = 651;
	p->Para.HalfCar_Edge_Slave_List[4].Back  = 1325;
	p->Para.HalfCar_Edge_Slave_List[4].Left  = 606;
	p->Para.HalfCar_Edge_Slave_List[4].Right = 606;

	p->Para.HalfCar_Edge_Slave_List[5].Front = 651;
	p->Para.HalfCar_Edge_Slave_List[5].Back  = 1325;
	p->Para.HalfCar_Edge_Slave_List[5].Left  = 606;
	p->Para.HalfCar_Edge_Slave_List[5].Right = 606;

	p->Para.HalfCar_Edge_Slave_List[6].Front = 651;
	p->Para.HalfCar_Edge_Slave_List[6].Back  = 1325;
	p->Para.HalfCar_Edge_Slave_List[6].Left  = 606;
	p->Para.HalfCar_Edge_Slave_List[6].Right = 606;

	p->Para.HalfCar_Edge_Slave_List[7].Front = 651;
	p->Para.HalfCar_Edge_Slave_List[7].Back  = 1325;
	p->Para.HalfCar_Edge_Slave_List[7].Left  = 606;
	p->Para.HalfCar_Edge_Slave_List[7].Right = 606;
	
	//作为前后头对头联动中心的轮系
	p->Para.Asb_Point_Num = 2;//选择A车2号轮系作为前后头对头串联动的自转中心

	//报警参数 65535即关闭报警
	p->Para.HgDiffMax = 30;
	p->Para.SensorAlarm.DisEnterMax = 1300;
	p->Para.SensorAlarm.DisEnterMin = 300;
	p->Para.SensorAlarm.DiffLR_EnterMax = 30;
	p->Para.SensorAlarm.LaserSpot_EnterMax = 20;
	p->Para.SensorAlarm.Ang_EnterMax = 65535;
	p->Para.SensorAlarm.DiffLR_Max = 50;
	p->Para.SensorAlarm.DisChangeMax = 100;
	p->Para.SensorAlarm.DisChangeMax_Inf = 65535;
	p->Para.SensorAlarm.AngChangeMax = 65535;
	p->Para.SensorAlarm.LaserSpotMax = 50;
	p->Para.SynTimeout = 3000;
	p->Para.PullApart = p->Para.SensorAlarm.DisEnterMax + 300;

	/*临时关闭
	p->Para.SensorAlarm.DisEnterMax = 65535;
	p->Para.SensorAlarm.DisEnterMin = 65535;
	p->Para.SensorAlarm.DiffLR_EnterMax = 65535;
	p->Para.SensorAlarm.LaserSpot_EnterMax = 65535;
	p->Para.SensorAlarm.Ang_EnterMax = 65535;
	p->Para.SensorAlarm.DiffLR_Max = 65535;
	p->Para.SensorAlarm.DisChangeMax = 65535;
	p->Para.SensorAlarm.DisChangeMax_Inf = 65535;
	p->Para.SensorAlarm.AngChangeMax = 65535;
	p->Para.SensorAlarm.LaserSpotMax = 65535;
	p->Para.HgDiffMax = 65535;
	p->Para.SynTimeout = 65535;
	p->Para.PullApart = 65535;
	*/

	//纠正参数
	p->Para.K_HgAdj = 10;
	p->Para.K_DisAdj = 1;
	p->Para.K_InfAdj = 1;
	p->Para.K_AngAdj = 1;
	p->Para.HgDiff_Tg = 20;
	p->Para.HgDiff_Re = 10;

	//联动时底盘参数
	p->Para.Acc.VxyzMax.x = 500;
	p->Para.Acc.VxyzMax.y = 500;
	p->Para.Acc.VxyzMax.z = 500;
	p->Para.Acc.AxyzMax.x = 80;
	p->Para.Acc.AxyzMax.y = 80;
	p->Para.Acc.AxyzMax.z = 80;

	/*临时设置联动位置*/
	//AtDbug.Bit.AsbSensorInOff = 1;
	p->Input.Sensor.Distance_FL = 500;
	p->Input.Sensor.Distance_BR = 500;
	p->Input.Place.Pos = (p->Input.Sensor.Distance_FL + p->Input.Sensor.Distance_BR) / 2;
	p->Input.Place.Inf = 0;
	p->Input.Place.Ang = 0;

	//其它参数
	p->Para.ErrRecover 	= 2000;
	p->Para.AsbSTM_Delay= 2000;					//单联动切换延迟时间 ms

	//联动应用场景,上电默认是选择A车；防止遥控器没启动，屏幕控制不正常
	if(	  g_Para.ApplicationType == ACC_Application_AssemlyRemote
		|| g_Para.ApplicationType == ACC_Application_AssemlyAGV)
	{
		g_ChannelSwitch.CtrlCmd.HandleUnionVal = AsbModel_A;
		AtDbug.Bit.AsbOff = 0;
	}
	//单车应用场景，上电就强行调试单动
	else
	{
		AtDbug.Bit.AsbOff = 1;								//临时强行调试单动
	}
	return 1;
}

//系统参数对象 写入 EEPROM对象
bool ParaInit_Sys2EEPObj_Template(void)
{
	Para_EEPROM_PrjExp_StDef_Template* p2_PrjExp = (Para_EEPROM_PrjExp_StDef_Template*)&EEPPara.User.PrjExp;

	/*示例 注意和ParaInit_EEPObj2Sys里是反的*/
	p2_PrjExp->xxx1 = g_Para.xxx1;

	return 1;
}

//项目外部自定义参数 读取
bool ParaInit_EEPObj2Sys_Template(void)
{
	Para_EEPROM_PrjExp_StDef_Template* p2_PrjExp = (Para_EEPROM_PrjExp_StDef_Template*)&EEPPara.User.PrjExp;

	/*示例*/
	g_Para.xxx1 = p2_PrjExp->xxx1;

	return 1;
}

//项目自定义拓展对象 越界检出
bool CheckAlign_Template(void)
{
	char errCnt = 0;

	//检查自定义异常位域定义是否越界
	if (sizeof(ACC_Faults_LoseControl_PrjExp_BitDef_Template) != sizeof(ACC_Faults_LoseControl_PrjExp_UnionDef_Template))
		errCnt++;
	else {}
	if (sizeof(ACC_Faults_ControlledStop_PrjExp_BitDef_Template) != sizeof(ACC_Faults_ControlledStop_PrjExp_UnionDef_Template))
		errCnt++;
	else {}
	if (sizeof(ACC_Faults_Warn_PrjExp_BitDef_Template) != sizeof(ACC_Faults_Warn_PrjExp_UnionDef_Template))
		errCnt++;
	else {}

	//项目外部自定义参数 越界检出
	if (sizeof(Para_EEPROM_PrjExp_StDef_Template) > PrjExpEepNum*2)
		errCnt++;
	else {}

	if (errCnt)
	{
		g_CodeErr.Bit.Unaligned = 1;//长度限制错误
		return 0;
	}
	else
		return 1;
}

//GocInit 注意和系统的goc对象一样，只用于调试方便查看
bool GocInit_Template(void)
{
	goc_Template.p2PrjExp_LoseControl = (ACC_Faults_LoseControl_PrjExp_UnionDef_Template*)&g_Faults.ProjectExtend.LoseControl;
	goc_Template.p2PrjExp_ControlledStop = (ACC_Faults_ControlledStop_PrjExp_UnionDef_Template*)&g_Faults.ProjectExtend.ControlledStop;
	goc_Template.p2PrjExp_Warn = (ACC_Faults_Warn_PrjExp_UnionDef_Template*)&g_Faults.ProjectExtend.Warn;
	goc_Template.p2PrjExp_EEPara = (Para_EEPROM_PrjExp_StDef_Template*)&EEPPara.User.PrjExp;
	return 1;
}

//项目自定义函数注册
bool Para_p2FuncInit_Template()
{
    // 通信接口配置函数 注册
    g_Para.p2Fucs.p2EQPInit = EQPInit_Template;
    // 无线遥控数据解析函数 注册
    g_Para.p2Fucs.p2ACC_GetCmdFormWirelessRemote = ACC_GetCmdFormWirelessRemote_Template;
    // AGV配置函数 注册
    g_Para.p2Fucs.p2SkeletonAGV_Init = app_SkeletonAGV_FunInit_Template;
    // 遥控器喇叭响应函数 注册
    g_Para.p2Fucs.p2ACC_GetCmdFormWirelessRemote_Auto = ACC_GetCmdFormWirelessRemote_Auto_Template;
    // 项目自定义拓展异常检出函数 注册
    g_Para.p2Fucs.p2EmgJudge_ProjectExtend = EmgJudge_ProjectExtend_Template;
    // 系统参数对象 写入 EEPROM对象 项目自定义函数 注册
    g_Para.p2Fucs.p2ParaInit_Sys2EEPObj = ParaInit_Sys2EEPObj_Template;
    // 项目自定义 hmi界面 注册
    g_Para.p2Fucs.p2HMI_Screen_PrjExp = HMI_Screen_PrjExp_Template;

	//项目自定义 AccStart函数
	g_Para.p2Fucs.p2PrjExp_AccStart = PrjExp_AccStart_Template;
	//项目自定义 AccStop函数
	g_Para.p2Fucs.p2PrjExp_AccEnd = PrjExp_AccEnd_Template;
	//项目自定义 TskAccStart函数
	g_Para.p2Fucs.p2PrjExp_TskAccStart = PrjExp_TskAccStart_Template;

	//项目自定义 联动坐标变换处理函数
	g_Para.p2Fucs.p2PrjExp_MR_Init_Asb_Axis = PrjExp_MR_Init_Asb_Axis_Template;

	return 1;
}

//CANOPEN设置
bool Para_CanOpen_Template(void)
{
	/*canopen节点数量和行走电机数量不一定相同，具体项目需要具体正确配置
	g_Para.CanMst.CheckQuantity = g_Para.Walk.MotorNum;*/
	g_Para.CanMst.CheckQuantity = 11;
	g_Para.CanMst.Baud = CAN_BAUDRATE_500K;

	//通过宏定义来选择驱动器类型，而不是直接改变量赋值
#if (SVO_Type == 0)      //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器	
	g_Para.CanMst.SVOType = Can_SVOType_Kinco;
	/* 部分项目是4代步科和5代步科的混用，5代步科需要单独设置驱动器类型，4代步科无需设置*/
	//SVO[0].SvoType = SvoType_Kinco_5;//1号驱动器设置成5代步科

	/* 部分步科驱动器需要反转，可以将下面代码启用；   台达驱动器的反转需要到台达上位机中配置*/
	//SVO[8].SvoDirection = SvoSvoDriveDirection_Reverse;//9号驱动器反转，常见于液压电机需要反转的情况

#elif (SVO_Type == 1)
	g_Para.CanMst.SVOType = Can_SVOType_Delta;
#elif (SVO_Type == 2)
	g_Para.CanMst.SVOType = Can_SVOType_Windcon;
#elif (SVO_Type == 3)
	g_Para.CanMst.SVOType = Can_SVOType_Wheatstone;
#elif (SVO_Type == 4)
	g_Para.CanMst.SVOType = Can_SVOType_Keya;
#elif (SVO_Type == 5)
	g_Para.CanMst.SVOType = Can_SVOType_Copley;
#endif

	/*如果是安川伺服，通过m3板卡控制，而非canopen，需要将类型设置为无效，以跳过canopen相关处理
	CanMaster.Para.SVOType == Can_SVOType_Null
	*/

    return 1;
}

//电池相关设置
bool Para_Battery_Template(void)
{
	/*电池类型：低压自制锂电池*/
	g_Para.BatteryType = ACC_LithiumBattery_300V_High;

	/*充电桩类型：被动式充电桩*/
	g_Para.ChargePileType = ACC_ChargePileType_Passive;
	return 1;
}

//舵轮车手动操作控制单个模组 初始化
bool Para_MR_ManualControl_Template(void)
{
	g_MR_ManualCtrl.Para_En = 0;							//默认关闭功能
	g_MR_ManualCtrl.En = 0;									//默认关闭使能
	g_MR_ManualCtrl.Select = 1;								//默认选择第一个模组
	g_MR_ManualCtrl.SpeedMax = 300;							//

	return 1;
}

//斜行和转弯命令二选一控制 初始化
bool Para_Patch_AngAndR_CmdSel_Template(void)
{
	g_Patch_AngAndR_CmdSel.Enable = 0;						//默认关闭斜行和转弯命令二选一控制
	g_Patch_AngAndR_CmdSel.ForceEnable = 0;
	g_Patch_AngAndR_CmdSel.MaxSliAngPos = 30;				//最大斜行角度 单轴
	g_Patch_AngAndR_CmdSel.MinRotDisPos = 8000;				//最小转弯距离 单轴
	//g_Patch_AngAndR_CmdSel.MaxSliAngPos_Free = 25;			//最大斜行角度 复合
	//g_Patch_AngAndR_CmdSel.MinRotDisPos_Free = 10000;		//最小转弯距离 复合

	return 1;
}

//散热风扇控制初始化
bool ObjInit_Fan_Template(FanCtrl_StDef *p)
{
	if(p==NULL)
		return 0;

	//工作模式配置
	if(g_Para.CanMst.SVOType == Can_SVOType_Delta)//台达
		p->Mode = FanCtrl_Mode_Temperature;
	else if(g_Para.CanMst.SVOType == Can_SVOType_Kinco)//步科
		p->Mode = FanCtrl_Mode_Time;

	p->FanType = FanCtrl_InterfaceType_IO;
	p->in_TemperatureFanOn = 40;
	p->in_TemperatureFanOff = 35;
	p->in_TimeFanOn = 10;
	p->in_TimeFanOff = 30;
	p->in_Temperature_Environment_FanOff = 60;//参数设65535即关闭该功能

	return 1;
}

//其它
bool Para_Other_Template(void)
{
	//目标角度与实际角度 角度差过大检出参数
	g_Faults.DiffAng.Para_DiffAng = 15;
	g_Faults.DiffAng.Para_Time = 60001;
    //AccData
	//非受控停车 延时抱闸时间参数 ms 领导要求200ms
	AccData.Para.LoseControlStop_BrakeTime = 200;

	g_ChannelSwitch.VxyzZoom.LimitMax = 107;//麦轮整体缩放限制到单轴摇杆最大值
	g_ChannelSwitch.VxyzZoom.data_num = 3;

	//限制两车距离
	AccData.Para.DisLimit_Min = 300;
	AccData.Para.DisLimit_Min_SlowDown = 800;
	AccData.Para.DisLimit_Min_SlowDown_V = 70;
	AccData.Para.DisLimit_Max = 65535;
	AccData.Para.DisLimit_Max_temp = AccData.Para.DisLimit_Max;
	AccData.Para.DisLimit_Min_temp = AccData.Para.DisLimit_Min;

	//休眠后强制切静止模式，特殊功能一般情况不用，设0关闭
	g_ChannelSwitch.WirelessSleepCntMax = 0;

	//升降双按钮按下清楚报警 判定时间 ms
	g_ChannelSwitch.Para_LiftTwoButtonPress = 5000;

	return 1;
}

//项目配置中定义的对象初始化
bool Conf_Init_Template(void)
{
	/*超低车轮系类型初始化配置示例
	后面可优化为根据 超低车固定的象限来计算*/
	Conf_ModuleType_TypeDef_Template ModuleType[MR_ModuleNumMxa] = {
		Conf_ModuleType_General,Conf_ModuleType_General,
		Conf_ModuleType_General,Conf_ModuleType_General,
		Conf_ModuleType_Special,Conf_ModuleType_General,
	};

	memcpy(g_Conf_Template.ModuleType,ModuleType,sizeof(ModuleType));

	return 1;
}

//联动特殊参数初始化
bool ObjInit_Asb_Template(void)
{
	g_Para.Asb.Channel_Single = 123;
	g_Para.Asb.Channel_Asb = 456;
	g_Para.Asb.Address_Single = 789;
	g_Para.Asb.Address_Asb = 123;
	
	/*switch (g_Para.EqpID)
	{
	case EqpID_54601:
		{
			g_Para.Asb.Channel_Single = 29;
			g_Para.Asb.Address_Single = 10870;
		}
		break;
	case EqpID_54602:
		{
			g_Para.Asb.Channel_Single = 33;
			g_Para.Asb.Address_Single = 10871;
		}
		break;
	case EqpID_54603:
		{
			g_Para.Asb.Channel_Single = 37;
			g_Para.Asb.Address_Single = 10872;
		}
		break;
	case EqpID_54604:
		{
			g_Para.Asb.Channel_Single = 41;
			g_Para.Asb.Address_Single = 10873;
		}
		break;
	case EqpID_54605:
		{
			g_Para.Asb.Channel_Single = 45;
			g_Para.Asb.Address_Single = 10874;
		}
		break;
	case EqpID_54606:
		{
			g_Para.Asb.Channel_Single = 49;
			g_Para.Asb.Address_Single = 10875;
		}
		break;
	default:
		break;
	}

	g_Para.Asb.Channel_Asb_1A = 29;
	g_Para.Asb.Address_Asb_1A = 10870;
							   
	g_Para.Asb.Channel_Asb_1B = 33;
	g_Para.Asb.Address_Asb_1B = 10871;
							   
	g_Para.Asb.Channel_Asb_2A = 37;
	g_Para.Asb.Address_Asb_2A = 10872;
							   
	g_Para.Asb.Channel_Asb_2B = 41;
	g_Para.Asb.Address_Asb_2B = 10873;
							   
	g_Para.Asb.Channel_Asb_3A = 45;
	g_Para.Asb.Address_Asb_3A = 10874;
							   
	g_Para.Asb.Channel_Asb_3B = 49;
	g_Para.Asb.Address_Asb_3B = 10875;
							   
	g_Para.Asb.Channel_Asb_4A = 53;
	g_Para.Asb.Address_Asb_4A = 10876;
							   
	g_Para.Asb.Channel_Asb_4B = 57;
	g_Para.Asb.Address_Asb_4B = 10877;*/

	/*根据情况设置是否在切换联动时 切换遥控器接收器信道
	//Channel_Write开启之后，就可以在hmi等外部修改g_Para.Asb下面的信道和地址参数，实现遥控器的选择
	if (g_Para.EqpID == EqpID_46501)
		g_Para.Asb.Bit.Channel_Write = 0;
	else
		g_Para.Asb.Bit.Channel_Write = 1;*/

	/*没有实体联动切换旋钮的车从hmi模拟按钮时设1
	g_Para.Asb.Bit.AsbSelect_FromHMI = 1;*/

	//联动通信CN口
	if(	  g_Para.ApplicationType == ACC_Application_AssemlyRemote
		||g_Para.ApplicationType == ACC_Application_AssemlyAGV)
	{
		g_Para.Asb.CN_Master = CCFCN13;
		g_Para.Asb.CN_Slave  = CCFCN13;
	}
	else
	{
		g_Para.Asb.CN_Master = CCFCN_Disable;
		g_Para.Asb.CN_Slave  = CCFCN_Disable;
	}
	
	return 1;
}

//超声波自动对中测距标定初始化
bool ObjInit_ShelfAutadj_Liner_Template(ShelfAutadj_Para_TypeDef* p)
{
	char i = 0;
	p->Num = 0;//0:维持原本的简单乘系数计算 8：8组超声波进行线性标定

	i = 0;
	p->Data[i].SetLowOrg = 10;
	p->Data[i].SetHighOrg = 100;
	p->Data[i].SetLow = 10;
	p->Data[i].SetHigh = 100;
	p->OffSet[i] = 0;

	i = 1;
	p->Data[i].SetLowOrg = 10;
	p->Data[i].SetHighOrg = 100;
	p->Data[i].SetLow = 10;
	p->Data[i].SetHigh = 100;
	p->OffSet[i] = 0;

	i = 2;
	p->Data[i].SetLowOrg = 10;
	p->Data[i].SetHighOrg = 100;
	p->Data[i].SetLow = 10;
	p->Data[i].SetHigh = 100;
	p->OffSet[i] = 0;

	i = 3;
	p->Data[i].SetLowOrg = 10;
	p->Data[i].SetHighOrg = 100;
	p->Data[i].SetLow = 10;
	p->Data[i].SetHigh = 100;
	p->OffSet[i] = 0;

	i = 4;
	p->Data[i].SetLowOrg = 10;
	p->Data[i].SetHighOrg = 100;
	p->Data[i].SetLow = 10;
	p->Data[i].SetHigh = 100;
	p->OffSet[i] = 0;

	i = 5;
	p->Data[i].SetLowOrg = 10;
	p->Data[i].SetHighOrg = 100;
	p->Data[i].SetLow = 10;
	p->Data[i].SetHigh = 100;
	p->OffSet[i] = 0;

	i = 6;
	p->Data[i].SetLowOrg = 10;
	p->Data[i].SetHighOrg = 100;
	p->Data[i].SetLow = 10;
	p->Data[i].SetHigh = 100;
	p->OffSet[i] = 0;

	i = 7;
	p->Data[i].SetLowOrg = 10;
	p->Data[i].SetHighOrg = 100;
	p->Data[i].SetLow = 10;
	p->Data[i].SetHigh = 100;
	p->OffSet[i] = 0;

	return 1;
}

//屏幕界面隐藏参数初始化
bool Para_HMIPageHideInit_Template(void)
{
	g_Para.HmiConceal.AutomaticFunctions_Switch = 1;			//主界面自动相关隐藏开关  0：全部显示  1：磁导航  2：激光导航 3：全隐藏
	g_Para.HmiConceal.Onboard = 1;								//板载模拟量输出隐藏开关	0：显示  1：隐藏
	g_Para.HmiConceal.GeneralParameter_Switch = 1;				//一般参数隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.WheelParameter_Switch = 1;				//舵轮参数隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.HeightCalibration_Switch = 0;				//高度标定隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.AngleCalibration_Switch = 0;				//角度标定隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.LaserCalibration_Switch = 1;				//激光标定隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.ParaObs_Switch = 0;						//避障可保存参数界面隐藏开关  0：全显示  1：仅雷达	2：仅触边	3：全隐藏
	g_Para.HmiConceal.ParaObs_NO_Switch = 0;					//避障不可保存参数界面隐藏开关  0：全显示  1：仅前后左右雷达	2：仅转到设置按钮	3：全隐藏
	g_Para.HmiConceal.LeadshineServoCalibration_Switch = 1;		//雷赛标定隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.PGVCalibration_Switch = 1;				//PGV标定隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.Odometry_Switch = 1;						//里程计隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.ControlCoeff_Switch = 1;					//控制系数隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.PID_Switch = 1;							//PID参数隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.PositionRadarCalibration_Switch = 1;		//位置雷达标定隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.PositionRadarSpeedCalibration_Switch = 1;	//位置雷达速度标定隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.PositioningRadar_Switch = 1;				//位置雷达隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.VoltageSOC_Switch = 1;					//电池电压关联参数隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.MoreParameters_Switch = 0;				//更多参数隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.AngleShow_Switch = 0;						//角度显示隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.HighShow_Switch = 2;						//高度压力显示隐藏开关 0：仅倒三角压力 1：倒三角 2：正三角	3：四点液压	4：仅高度 5：全隐藏
	g_Para.HmiConceal.HighButton_Switch = 1;					//高度界面单动开关	0：倒三角	1：正三角	2：四点液压	3：全隐藏
	g_Para.HmiConceal.Suspension_Switch = 1;					//悬挂参数隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.Outrigger_Switch = 1;						//支腿参数隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.LaserShow_Switch = 1;						//激光显示隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.Ultrasonic_Switch = 1;					//超声波显示隐藏开关  0：显示  1：隐藏
	g_Para.HmiConceal.AD_Switch = 1;							//AD显示隐藏开关  0：全显示  1：仅模块1	2：全隐藏
	g_Para.HmiConceal.DA_Switch = 1;							//DA显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.GPS_Switch = 1;							//GPS显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.LaserNavigation_Switch = 1;				//激光导航显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.DispatchingSystem_Switch = 1;				//调度系统显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.AutoOperation_Switch = 1;					//自动运行显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.Obs_Switch = 1;							//避障显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.MagneticNavigation_Switch = 1;			//磁导航显示隐藏开关  0：全显示	1：磁导航+1个RFID	2：磁导航	3：仅1个RFID	4：仅两个RFID	5：全隐藏
	g_Para.HmiConceal.LeadshineServo_Switch = 1;				//雷赛显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.PGV_Switch = 1;							//PGV显示隐藏开关(需确认）
	g_Para.HmiConceal.PalletCamera_Switch = 1;					//托盘相机显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.AsbShow_Switch = 1;						//联动显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.Facula_Switch = 1;						//光斑显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.TiltSensor_Switch = 1;					//倾角传感器显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.ManualPulseGenerator_Switch = 1;			//手动脉冲发生器显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.BMSShow_Switch = 1;						//电池显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.IOOutputForcing_Switch = 1;				//IO输出强制显示隐藏开关  0：显示	1：隐藏
	g_Para.HmiConceal.ANC_Switch = 1;							//安川驱动器状态显示隐藏开关	0：显示	1：隐藏
	g_Para.HmiConceal.DBCOM_Switch = 0;							//多倍通设定显示隐藏开关	0：显示	1：隐藏
	return 1;
}

//Template设备参数初始化（通过代码设定）
bool Para_Template(void)
{
	/*应用类型：单车遥控*/
	g_Para.ApplicationType = ACC_Application_GeneralRemote;
	//初始化g_Conf_Template
	Conf_Init_Template();
	//GocInit 注意和系统的goc对象一样，只用于调试方便查看
	GocInit_Template();
	//联动特殊参数初始化
	ObjInit_Asb_Template();
	//-------------------------上面的调用不能放到下面，因为下面有些初始化会用到上面的参数-------------------------

	//功能使能
	Para_FunctionSwitch_Template();
    //CANOPEN设置
	Para_CanOpen_Template();
    //电池相关设置
	Para_Battery_Template();
	//轮系角度参数初始化
	Para_WheelAngle_Template();
	//行走参数初始化
	Para_Walk_Template();
	Para_Walk_Asb_Template();
	//升降参数
	Para_Lift_Template();
	//悬挂参数
	Para_Absorption_Template();
	//推拉参数
	Para_Push_Roller_Template();
	//称重参数
	Para_LoadDetection_Template();
	//避障雷达配置
	Para_ROA_Template();
	//自动对中参数初始化
	Para_AutoCentre_Template();

	//数据管道配置开关
	Para_AppInSource_Template();
	//Modbus设备定义初始化
	Para_EQPInit_Template();
	//导航转接板初始化
	EQP_SerialExpObjInit_Template();

	//输入IO控制
	ObjInit_IOInputs_IDInit_Template();
	//输出IO控制	
	ObjInit_IOOutputs_IDInit_Template();
	//输入ADC控制
	ObjInit_ADC_IDInit_Template();
	//输出DAC控制
	ObjInit_DAC_IDInit_Template();
    //项目自定义函数注册
	Para_p2FuncInit_Template();
    //传感器安装参数标定
	Para_Inst_Template();
	//报警参数
	Para_EmgJudge_Template();
	//虚拟里程计参数
	Para_OBO_Template();
	//控制系数
	Para_CtrlCoef_Template();
	//系统功能位参数
	Para_SysFucBit_Template();
	//系统对象初始化
	ObjInit_WalkLimit_Template(&g_WalkLimit);						//加速度控制对象初始化
	ObjInit_WalkGearLimit_Template();								//行走档位限制对象初始化
	ObjInit_WalkGearLimit_Hg_Template();							//根据高度限制行走挡位
	ObjInit_LiftGearLimit_Template();								//升降档位限制对象初始化

	//行走电机速度平滑参数初始化
	Para_WalkMotorSmooth_Template();
	//差动速度平滑参数初始化
	Para_OffsetMotorSmooth_Template();
	//传感器标定参数初始化
	Para_AsbSensorCal_Template();
	//舵轮车手动操作控制单个模组 初始化
	Para_MR_ManualControl_Template();
	//斜行和转弯命令二选一控制 初始化
	Para_Patch_AngAndR_CmdSel_Template();
	//多倍通设备ID初始化
	ObjInit_Doublecom_Template(&g_DBComSet_Template);
	//联动模块对象初始化
	ObjInit_Assembly_Template(&g_Asb);
	//项目外部自定义参数 读取
	ParaInit_EEPObj2Sys_Template();
	//项目自定义拓展对象 越界检出
	CheckAlign_Template();
	//Fan对象初始化
	ObjInit_Fan_Template(&g_Fan);
    //其它
	Para_Other_Template();
	//超声波自动对中测距标定初始化
	ObjInit_ShelfAutadj_Liner_Template(&ShelfAutadj_Liner);
	//屏幕界面隐藏参数初始化
	Para_HMIPageHideInit_Template();

	/*项目调试时临时设置的参数，过后记得检查注释掉*/
	//g_Para.Debug.ForceSVODisable = 1;						//新软件开始不使能 安全起见
	//g_Para.Debug.ForceSVODisable_Lift = 1;
	//AtDbug.Bit.AsbOff = 1;								//临时强行调试单动
	//g_IOInputs.Assembly.RealtimeValue = 1;					//临时强制开机单动/联动开关状态
	//g_LiftData.WarningPara.DiffMax = 65535;					//临时关闭高低差报警
	return 1;
}

//***********************************END OF FILE***********************************

