/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: CBB_Init.c
** 创 建 人: 文小兵  
** 描    述: 模块对象实例化、初始化，整理汇总到此文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年4月12日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define CBB_INIT_GLOBALS

//自定义引用			
#include "includes.h"
#include "CBB_Init.h"
#include "ACC_Init.h"
#include "app_DependenceAGV.h"

#include "CommConf.h"
#include "ACC_Project.h"
#include "app_DataSmoothing.h"

//外部对象声明


//内部方法声明

/**************************************************************************
* 函数名:JoystickDataSmooth_Init
* 功  能:模块初始化内容
* 参  数:
* 返回值:bool
**************************************************************************/
bool JoystickDataSmooth_Init(void)
{
	f32 temp_AccAdsorb = 0.5;								//快速吸附加速增量
	f32 temp_DecAdsorb = 1;									//快速吸附减速增量
	f32 temp_AdsorbRange = 3;								//吸附范围

	g_Para.Walk.Smooth.VxAcc = g_Para.Walk.Smooth.VyAcc = g_Para.Walk.Smooth.VzAcc = (float)g_Para.Other.in_SmoothVxyzAcc / 100.0f;
	g_Para.Walk.Smooth.VxDec = g_Para.Walk.Smooth.VyDec = g_Para.Walk.Smooth.VzDec = (float)g_Para.Other.in_SmoothVxyzDec / 100.0f;

	DataSmooth_Init(&g_SmoothVx, g_Para.Walk.Smooth.VxAcc, g_Para.Walk.Smooth.VxDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	DataSmooth_Init(&g_SmoothVy, g_Para.Walk.Smooth.VyAcc, g_Para.Walk.Smooth.VyDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	DataSmooth_Init(&g_SmoothVz, g_Para.Walk.Smooth.VzAcc, g_Para.Walk.Smooth.VzDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	DataSmooth_Init(&g_SmoothAx, g_Para.Walk.Smooth.AxAcc, g_Para.Walk.Smooth.AxDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	DataSmooth_Init(&g_SmoothAy, g_Para.Walk.Smooth.AyAcc, g_Para.Walk.Smooth.AyDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	DataSmooth_Init(&g_SmoothAz, g_Para.Walk.Smooth.AzAcc, g_Para.Walk.Smooth.AzDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	DataSmooth_Init(&g_SmoothVxyz[0], g_Para.Walk.Smooth.VxAcc, g_Para.Walk.Smooth.VxDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	return 0;
}

//联动下的平滑参数设置
bool JoystickDataSmooth_Asb_Init(void)
{
	f32 temp_AccAdsorb = 0.5;								//快速吸附加速增量
	f32 temp_DecAdsorb = 1;									//快速吸附减速增量
	f32 temp_AdsorbRange = 3;								//吸附范围

	DataSmooth_Init(&g_SmoothVx, g_Para.Walk_Asb.Smooth.VxAcc, g_Para.Walk_Asb.Smooth.VxDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	DataSmooth_Init(&g_SmoothVy, g_Para.Walk_Asb.Smooth.VyAcc, g_Para.Walk_Asb.Smooth.VyDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	DataSmooth_Init(&g_SmoothVz, g_Para.Walk_Asb.Smooth.VzAcc, g_Para.Walk_Asb.Smooth.VzDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	DataSmooth_Init(&g_SmoothAx, g_Para.Walk_Asb.Smooth.AxAcc, g_Para.Walk_Asb.Smooth.AxDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	DataSmooth_Init(&g_SmoothAy, g_Para.Walk_Asb.Smooth.AyAcc, g_Para.Walk_Asb.Smooth.AyDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	DataSmooth_Init(&g_SmoothAz, g_Para.Walk_Asb.Smooth.AzAcc, g_Para.Walk_Asb.Smooth.AzDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	DataSmooth_Init(&g_SmoothVxyz[0], g_Para.Walk_Asb.Smooth.VxAcc, g_Para.Walk_Asb.Smooth.VxDec, temp_AdsorbRange, temp_AccAdsorb, temp_DecAdsorb);
	return 0;
}

/**************************************************************************
* 函数名:MagneticTrace_Init
* 功  能:模块初始化内容
* 参  数:MotorResolving_TypeDef *p
* 返回值:bool
**************************************************************************/
bool MagneticTrace_Init(MagneticTrace_TypeDef *p)
{
	//磁导航偏移数据最大值
	if(p->in_MagMax[0] == 0){p->in_MagMax[0] = 75;}
	if(p->in_MagMax[1] == 0){p->in_MagMax[1] = 75;}
	if(p->in_MagMax[2] == 0){p->in_MagMax[2] = 75;}
	if(p->in_MagMax[3] == 0){p->in_MagMax[3] = 75;}
	//复合运动参数
	p->in_AngPosMax		=abs(g_Para.Walk.MaxSliAngPos);
	p->in_AngInfMax		=abs(g_Para.Walk.MaxSliAngInf);
	p->in_RadiusPosMax	=g_Para.Walk.MaxRotDisPos;
	p->in_RadiusPosMin	=g_Para.Walk.MinRotDisPos;
	p->in_RadiusInfMax	=g_Para.Walk.MaxRotDisInf;
	p->in_RadiusInfMin	=g_Para.Walk.MinRotDisInf;
	p->in_DistancePos	=g_Para.Walk.MGC_InstY;
	p->in_DistanceInf	=g_Para.Walk.MGC_InstX;

	//命令范围
	if(p->in_CmdVxMax == 0)	{p->in_CmdVxMax = 100;}
	if(p->in_CmdVzMax == 0)	{p->in_CmdVzMax = 100;}

	//巡线模式
	if(g_Para.ChassisType == ACC_ChassisType_Turtle)
	{
		/*麦轮车，使用*/
		p->in_TraceMode = MagneticTrace_TraceMode_MecanumFrontAndBack;  
	}
	else if(g_Para.ChassisType == ACC_ChassisType_Ant)
	{
		p->in_TraceMode = MagneticTrace_TraceMode_DiffVCar;  
	}
	else
	{
		p->in_TraceMode = MagneticTrace_TraceMode_FrontAndBack;  
	}
	
	return 0;
}

/**************************************************************************
* 函数名:CenterPointCal_Init
* 功  能:轮系巡线模块初始化
* 参  数:CenterPointCal_StDef *p
* 返回值:bool
**************************************************************************/
bool CenterPointCal_Init(CenterPointCal_StDef *p)
{
	
	if(p->MagMax == 0)		{p->MagMax = 75;}		//磁条偏差最大值
	//if(p->MagMin == 0)		{p->MagMin = 0;}
	if(p->TraceAngMax ==0)	{p->TraceAngMax = 20;}	//巡线角度最大值
	//if(p->TraceAngMin == 0)	{p->TraceAngMin = 0;}

	/*巡线轮系 坐标初始化 默认1 3号模组巡线
	p->WheelPos[0].X = g_MotorResolving.Mxy[0].X_Axis;
	p->WheelPos[0].Y = g_MotorResolving.Mxy[0].Y_Axis;
	p->WheelPos[1].X = g_MotorResolving.Mxy[2].X_Axis;
	p->WheelPos[1].Y = g_MotorResolving.Mxy[2].Y_Axis;
    */

    //巡线轮系 坐标初始化 380特殊1 2号模组巡线
	p->WheelPos[0].X = g_MotorResolving.Para.Mxy[0].X_Axis;
	p->WheelPos[0].Y = g_MotorResolving.Para.Mxy[0].Y_Axis;
	p->WheelPos[1].X = g_MotorResolving.Para.Mxy[1].X_Axis;
	p->WheelPos[1].Y = g_MotorResolving.Para.Mxy[1].Y_Axis;
	
	//巡线中差动速度设置，如果没有设置或者比正常行驶的差动速度大；都将其约束至正常行驶差动速度
	if(g_Para.CtrlCoef.TraceSkewSpeed == 0 || g_Para.CtrlCoef.TraceSkewSpeed > g_Para.Walk.SkewSpeed)
	{
		g_Para.CtrlCoef.TraceSkewSpeed = g_Para.Walk.SkewSpeed;
	}
	
	return 0;
}

/**************************************************************************
* 函数名:MagneticNailLine_Init
* 功  能:标准磁钉巡线模块初始化
* 参  数:DisMagNavTypedef *p
* 返回值:bool
**************************************************************************/
bool MagneticNailLine_Init(DisMagNavTypedef *p)
{
	PID_t 	PID_Str = {0.3f,0.01f,0.01f},	//直道PID
			PID_Arc = {1.5f,1.0f,0.5f};		//弯道PID
			
	p->Init.SensorDisFB = 2966;		/*传感器距离mm*/
	p->Init.MagOrientation = Minus;	/*磁钉安装方向*/
	p->Init.P_systemCNT = &g_Sys_1ms_Counter;/*系统毫秒计数*/
	p->Init.FixFlag = &g_VirtualOdoData_MG.FixFlag;/*里程计接口*/
	p->Init.FixX = &g_VirtualOdoData_MG.FixX;
	p->Init.FixY = &g_VirtualOdoData_MG.FixY;
	p->Init.FixZ = &g_VirtualOdoData_MG.FixZ;
	p->Init.MagnetStrengthMin = 30;/*磁导航识别磁钉强度阈值*/
	p->Init.OffsetSlowDownLevel_1 = 30;/*磁导航偏移减速中速阈值*/
	p->Init.OffsetSlowDownLevel_2 = 70;/*磁导航偏移减速低速阈值*/
	p->Init.ArcDistance = 600;/*弯道磁钉间距mm*/
	p->Init.ExitArcDis = 800;/*退出弯道距离阈值*/
	p->Init.LostMagDis = 4000;/*丢失磁钉报警阈值*/
	p->Init.PID_Arc = &PID_Arc;/*弯道PID*/
	p->Init.PID_Str = &PID_Str;/*直道PID*/
	p->Init.SlowDownLevel[0] = 0.5f;/*一档限速60% 0.6*/
	p->Init.SlowDownLevel[1] = 0.7f;/*二档限速20% 0.8*/
	p->Init.SlowDownLevel[2] = 1.0f;/*三档不限速*/
	
	return 0;
	
}

/**************************************************************************
* 函数名:SoundLightNotice_Init
* 功  能:模块初始化内容
* 参  数:SoundLightNotice_TypeDef *p
* 返回值:bool
**************************************************************************/
bool SoundLightNotice_Init(SoundLightNotice_TypeDef *p)
{
	p->in_CtrlMode = SLN_CtrlMode_Manual;					//控制模式:遥控
	p->in_WorkMode = SLN_WorkMode_Still;					//工作模式:上电默认静止
	p->in_ObsState = SLN_ObsState_Normal;					//避障状态:上电默认正常
	p->in_FlgMotion = false;								//运动中标志:上电默认关闭
	p->in_FlgModeSwitchOk = false;							//模式切换到位标志:上电默认关闭*
	p->in_FlgWaitForConf = false;							//等待用户确认标志:上电默认关闭*
	p->in_FlgAutoReady = false;								//巡线/自动对中等功能满足条件标志:上电默认关闭*
	p->in_FlgAutoDoing = false;								//巡线/自动对中等功能使能运行中标志:上电默认关闭*
	p->in_FlgMotionArrive = false;							//升降/巡线到位/对中到位等自动调整功能到位后，使能一直有效标志:上电默认关闭*
	return 0;
}


/**************************************************************************
* 函数名:MotorResolving_Init
* 功  能:模块初始化内容
* 参  数:MotorResolving_TypeDef *p
* 返回值:bool
**************************************************************************/
bool MotorResolving_Init(MotorResolving_TypeDef *p)
{
	char i = 0;

	p->Para.MotorNumber=g_Para.Walk.MotorNum;
	p->Para.ModuleNumber=p->Para.MotorNumber/2;
	
	//轮系位置参数 前后Y轴上的超速模组横向目标角度可能存在bug
	for(i=0;i<p->Para.ModuleNumber;i++)
	{
		p->Para.Mxy[i].X_Axis =g_Para.Walk.Mxy[i].x;
		p->Para.Mxy[i].Y_Axis =g_Para.Walk.Mxy[i].y;
	}
	p->Para.Lc = g_Para.Walk.Lc;
	//p->Para.MotorMaxRpm = g_Para.Walk.SpeedPlan[ACC_WALKGEARMAX - 1] + g_Para.Walk.SkewSpeed;//避免六档差动造成前向减速
	p->Para.MotorMaxRpm = g_Para.Walk.MotorMaxRpm;

	//复合运动参数
	p->Para.CtrlAngMax_Left = g_Para.Walk.CtrlAngMax_Left;
	p->Para.CtrlAngMax_Right = g_Para.Walk.CtrlAngMax_Right;
	p->Para.SkewSpeed = g_Para.Walk.SkewSpeed;
	p->Para.SkewSpeed_ChangeIng = g_Para.Walk.SkewSpeed_ChangeIng;	//模式切换过程的差动速度
	p->Para.MaxSliAngPos = g_Para.Walk.MaxSliAngPos;
	p->Para.MaxSliAngInf = g_Para.Walk.MaxSliAngInf;
	p->Para.MaxRotDisPos = g_Para.Walk.MaxRotDisPos;
	p->Para.MinRotDisPos = g_Para.Walk.MinRotDisPos;
	p->Para.MaxRotDisInf = g_Para.Walk.MaxRotDisInf;
	p->Para.MinRotDisInf = g_Para.Walk.MinRotDisInf;
	p->Para.AngInPlaceMin = g_Para.Walk.AngInPlaceMin/100.0f;
	p->Para.AngInPlaceMin_ChangeIng = g_Para.Walk.AngInPlaceMin_ChangeIng/100.0f;
	p->Para.AngSlowDown = g_Para.Walk.AngSlowDown/100.0f;
	p->Para.AngSlowDown_ChangeIng = g_Para.Walk.AngSlowDown_ChangeIng/100.0f;

	//p->ModuleNumber = MC_Wheel_MAX_NUM / 2;
	//p->MotorNumber = MC_Wheel_MAX_NUM;
	//p->Ksm = p->MotorMaxRpm / (MR_MaxSkewHandle * ACC_WALKGEARMAX);
	p->Para.Ksm = g_Para.Walk.SpeedPlan[ACC_WALKGEARMAX - 1] / (MR_MaxSkewHandle * ACC_WALKGEARMAX);

	//轮系类型
	if(g_Para.ChassisType == ACC_ChassisType_Titan || g_Para.ChassisType == ACC_ChassisType_Rhino)//差速舵轮
		p->Para.WheelType = MR_WheelType_DifSpeed;
	else if(g_Para.ChassisType == ACC_ChassisType_Ant)		//单舵轮
		p->Para.WheelType = MR_WheelType_TurnIndependent;
	else if(g_Para.ChassisType == ACC_ChassisType_DiffVCar)
		p->Para.WheelType = MR_WheelType_EntiretyDifSpeed;		//差速轮

	p->Input.GolineFlag = 0;

	//vir_ang init
	//p->VirAng.CallDelayMs=1;
	p->VirAng.Wmax=20;

    //处理模组坐标分析与解算自由度限制
    MotorResolving_GetRadiusMin(p);

	MotorResolving_SignInit(p);

	//单动下的平滑参数设置
	JoystickDataSmooth_Init();

	return 0;
}

//联动
bool MotorResolving_Init_Asb(MotorResolving_TypeDef* p,f32 OffsetX,f32 OffsetY)
{
	char i = 0;

	//轮系位置参数 前后Y轴上的超速模组横向目标角度可能存在bug
	for(i=0;i<p->Para.ModuleNumber;i++)
	{
		p->Para.Mxy[i].X_Axis =g_Para.Walk.Mxy[i].x + OffsetX;
		p->Para.Mxy[i].Y_Axis =g_Para.Walk.Mxy[i].y + OffsetY;
	}

	//复合运动参数 适当在单动基础上减小自由度
	p->Para.MaxSliAngPos = g_Para.Walk_Asb.MaxSliAngPos;
	p->Para.MaxSliAngInf = g_Para.Walk_Asb.MaxSliAngInf;
	p->Para.MinRotDisPos = g_Para.Walk_Asb.MinRotDisPos;
	p->Para.MinRotDisInf = g_Para.Walk_Asb.MinRotDisInf;

	/*联动下的平滑参数设置
	JoystickDataSmooth_Asb_Init();*/

    //处理模组坐标分析与解算自由度限制
    MotorResolving_GetRadiusMin(p);

	MotorResolving_SignInit(p);

	return 0;
}

//虚拟磁条初始化
bool VirtualMagnetic_Init(_MagneticVirtual_TypeDef *p)
{
	p->MapDistance[MagVir_F]=g_Para.Walk.MGC_InstY*10;
	p->MapDistance[MagVir_B]=g_Para.Walk.MGC_InstY*10;	
	p->MapDistance[MagVir_L]=g_Para.Walk.MGC_InstX*10;
	p->MapDistance[MagVir_R]=g_Para.Walk.MGC_InstX*10;

	//PGV角度纠偏对象参数初始化
	memcpy(g_PGVAngle_MagneticVirtual.MapDistance,g_VirtualMagnetic.MapDistance,sizeof(g_PGVAngle_MagneticVirtual.MapDistance));
	return 0;
}

/**************************************************************************
* 函数名:OdometerData_Init
* 功  能:模块初始化内容
* 参  数:OdometerData_t *p
* 返回值:bool
**************************************************************************/
bool OdometerData_Init(OdometerData_t *p)
{
	g_VirtualOdoData.InitData.WheelType = DiffSteerings;	/*选择差速舵轮模式（其他模式暂时不可用）*/
	g_VirtualOdoData.InitData.GBRatio = 1/50.0f;			/*减速比50*/
	g_VirtualOdoData.InitData.Wheelradius = 200;			/*轮径200mm*/
	g_VirtualOdoData.InitData.InitX = 0;
	g_VirtualOdoData.InitData.InitY = 0;
	g_VirtualOdoData.InitData.InitZ = 0;
	g_VirtualOdoData.InitData.WheelNum = g_Para.Walk.MotorNum;			/*使用轮子的数量*/
	g_VirtualOdoData.InitData.GyroType = OdoGyroNULL;					/*使用A0030S陀螺仪数据接入磁导航*/
	g_VirtualOdoData.InitData.Correction_Dis = g_Para.Obo.Correction_X;	/*行走距离校订参数*/
	g_VirtualOdoData.InitData.Correction_X = g_Para.Obo.Correction_X;	/*里程X轴校订参数*/
	g_VirtualOdoData.InitData.Correction_Y = g_Para.Obo.Correction_Y;	/*里程Y轴校订参数*/
	g_VirtualOdoData.InitData.Correction_Z = g_Para.Obo.Correction_Z;	/*里程Z轴校订参数*/
	g_VirtualOdoData.InitData.Correction_Z2 = g_Para.Obo.Correction_Z;	/*里程Z轴校订参数*/
	g_VirtualOdoData.InitData.Correction_Z3 = g_Para.Obo.Correction_Z;	/*里程Z轴校订参数*/
	g_VirtualOdoData.InitData.SysCount = &g_Sys_1ms_Counter;/*系统1ms定时作为里程计的基准源*/

	g_VirtualOdoData.WorkMode = OdoWalkMode_GoLine;
	OdometerInit(&g_VirtualOdoData);
	g_VirtualOdoData.FixFlag=1;
	g_VirtualOdoData.FixX = 0;
	g_VirtualOdoData.FixY = 0;
	g_VirtualOdoData.FixZ = 0;
	OdometerRefresh();
	return 0;
}

/***************************************************************************************
** 函数名称: WirelessHandleCtrlData_Init
** 功能描述: 遥控器控制数据初始化
** 参    数: WH_TypeDef
** 返 回 值: bool
****************************************************************************************/
bool WirelessHandleCtrlData_Init(WH_TypeDef *p)
{
	p->p_DataInput = EQP_WHA.RD;							//遥控器数据输入指针

	//工作模式初始设置为静止
	p->WorkMode = 0x04;										//根据具体项目遥控器面板接线做修改
	
	//联动模式选择A车
	p->CarSel = Car_Sel_A;
		
	//避障控制默认全部打开
	p->ObsSel = Obs_Sel_Off_None;
	
	//照明默认关闭
	p->Lighting = false;
	
	//急停默认未触发
	p->EmgStop = true;
	
	//上升默认未触发
	p->LiftUp = false;
	
	//下降默认未触发
	p->LiftDown = false;
	
	//喇叭默认未触发
	p->Horn = false;
	
	//自动对中使能默认未触发
	p->AutoEnterShelf = false;
	
	//巡线使能默认未触发
	p->LineStop = false;
	
	//自动对中使能（到位停止）默认未触发
	p->ShelfStop = false;

	return 0;
}


//AGV自动运行依赖CBB库初始化
bool AutoRunObj_Init()
{
	bool res =0;
	//调度系统通信缓存指针初始化
	g_DHR_AGVInformation = (uint16_t*)&EQP_DSP.Data;

	/*调用自动框架相关功能初始化，函数具体实现将会在Conf_xxx.c中重写；或者使用自动框架下默认的磁导航相关功能*/
	if(g_Para.p2Fucs.p2SkeletonAGV_Init != NULL)
	{
		g_Para.p2Fucs.p2SkeletonAGV_Init();
	}

	/*对象初始化*/
	{
		g_AGVDependenInputData.AGVHostCtrl	 	= &g_HostData;
		//g_AGVDependenInputData.MRData			= &g_MotorResolving;
		//g_AGVDependenInputData.VirtualMagnetic	= &g_VirtualMagnetic;
		//g_AGVDependenInputData.VirtualMagPGV	= &g_PGVAngle_MagneticVirtual;
		//g_AGVDependenInputData.VirtualOdo 		= &g_VirtualOdoData;

	}
	/*自动运行的参数设置检验，如果配置参数不完全或者有问题，会返回0x01*/
	res = AGV_DependenceInitCheck();

	if(res == 0)
	{
		/*自动运行功能配置正确*/
		return 1;
	}
	else
	{
		/*自动运行功能配置失败*/
		return 0;
	}
}

/**************************************************************************
* 函数名:MecanumResolving_Init
* 功  能:模块初始化内容
* 参  数:Mecanum_TypeDef *p
* 返回值:bool
**************************************************************************/
bool MecanumResolving_Init(Mecanum_TypeDef *p)//单动
{
	char i = 0;
	ACC_Para_TypeDef *p2Para=&g_Para;

	p->Para.WalkMotorNum = p2Para->Walk.MotorNum;
	p->Para.MotorMaxRpm = p2Para->Walk.MotorMaxRpm;
	
	for (i = 0; i < p->Para.WalkMotorNum; i++)
	{
		p->Para.Mxy[i].x = p2Para->Walk.Mxy[i].x;
		p->Para.Mxy[i].y = p2Para->Walk.Mxy[i].y;
		p->RotUnif.Asb[i] = 0;
		p->Para.Ratio[i] = p2Para->Walk.Ratio[i];
		p->Para.Diameter[i] = p2Para->Walk.Diameter[i];
		p->Para.Mxy[i].WheelType = p2Para->Walk.WheelType[i];
		p->Para.Mxy[i].MotorDirection = p2Para->Walk.MotorDirection[i];
	}
	
	//计算电机速度符号
    Mec_SpeedDirCal(p);

    //计算速度归一化系数
    Mec_SpeedUnifCal(p);

    //计算旋转中心归一化系数
    Mec_RotUnifCal(p);

	//电机最大转速与最大摇杆值的比值
	p->Ksm = (f32)(p->Para.MotorMaxRpm) / V_Max;			

	//外部输入数据
	p->RotUnif.LxyMax.ExtIn = Mec_InvalidExtIn;
	p->SpeedZoom.SpeedMax.ExtIn = Mec_InvalidExtIn;

	//单动下的平滑参数设置
	JoystickDataSmooth_Init();

	return 0;
}

//联动下的麦轮解算初始化
bool MecanumResolving_Init_Asb(Mecanum_TypeDef* p, f32 OffsetX, f32 OffsetY)
{
	char i = 0;
	ACC_Para_TypeDef* p2Para = &g_Para;

	p->Para.WalkMotorNum = p2Para->Walk.MotorNum;
	p->Para.MotorMaxRpm = p2Para->Walk.MotorMaxRpm;

	for (i = 0; i < p->Para.WalkMotorNum; i++)
	{
		//p->Para.Mxy[i].x = p2Para->Walk.Mxy[i].x + OffsetX;
		//p->Para.Mxy[i].y = p2Para->Walk.Mxy[i].y + OffsetY;
		//p->RotUnif.Asb[i] = -(OffsetX * p->SpeedDir[i].SAxis_x + OffsetY * p->SpeedDir[i].SAxis_y);
		p->RotUnif.Asb[i] = (OffsetX * p->SpeedDir[i].SAxis_x + OffsetY * p->SpeedDir[i].SAxis_y);
		p->Para.Ratio[i] = p2Para->Walk.Ratio[i];
		p->Para.Diameter[i] = p2Para->Walk.Diameter[i];
		p->Para.Mxy[i].WheelType = p2Para->Walk.WheelType[i];
		p->Para.Mxy[i].MotorDirection = p2Para->Walk.MotorDirection[i];
	}

	//计算电机速度符号
	Mec_SpeedDirCal(p);

	//计算速度归一化系数
	Mec_SpeedUnifCal(p);

	//计算旋转中心归一化系数
	Mec_RotUnifCal(p);

	//电机最大转速与最大摇杆值的比值
	p->Ksm = (f32)(p->Para.MotorMaxRpm) / V_Max;

	/*联动下的平滑参数设置
	JoystickDataSmooth_Asb_Init();*/

	return 0;
}

/**************************************************************************
* 函数名:ROA_Init
* 功  能:模块初始化内容
* 参  数:
* 返回值:bool
**************************************************************************/
bool ROA_Init(ROA_TypeDef *p)
{
	if(g_Para.ROA.Install.Diagonal_L == 1)
	{
		p->InstallPattern=Pattern_Diagonal_L;					//左对角安装
	}
	else if(g_Para.ROA.Install.Diagonal_R == 1)
	{
		p->InstallPattern=Pattern_Diagonal_R;						//右对角安装
	}
	else if(g_Para.ROA.Install.Center == 1)
	{
		p->InstallPattern=Pattern_Center;						//前后双雷达安装
	}
	else if(g_Para.ROA.Install.Surround == 1)
	{
		p->InstallPattern=Pattern_Surround;						//四周四雷达安装方式
	}
	/*没有初始化，默认是对角安装*/
	else
	{
		p->InstallPattern=Pattern_Diagonal_L;
	}
	
	if(p->ChangeTime		==0)	{p->ChangeTime=300;}			//切换时间300ms 单侧的检测时间
	if(p->RecoverTime		==0)	{p->RecoverTime=1000;}			//恢复时间1000ms
	if(p->XAxisMaxSpeedLv3	==0)	{p->XAxisMaxSpeedLv3=200;}		//外层触发摇杆限制值
	if(p->XAxisMaxSpeedLv2	==0)	{p->XAxisMaxSpeedLv2=50;}
	if(p->YAxisMaxSpeedLv3	==0)	{p->YAxisMaxSpeedLv3=200;}
	if(p->YAxisMaxSpeedLv2	==0)	{p->YAxisMaxSpeedLv2=50;}
	//if(p->RadarBrand		==0)	
	{p->RadarBrand=Brand_Hinson;}	//兴颂雷达
	return 0;
}

/**************************************************************************
* 函数名:LiftCtrl_Init
* 功  能:模块初始化内容
* 参  数:
* 返回值:bool
**************************************************************************/
bool LiftCtrl_Init(LiftCtrl_TypeDef* p)
{
	char i = 0;
	// 设置高度参数
	p->Height.Num 			= g_Para.Lift.Num;
	p->Height.SetLow 		= g_Para.Lift.SetLow;
	p->Height.SetHigh		= g_Para.Lift.SetHigh;
	
	//初始化标定参数
	for(i=0;i<p->Height.Num;i++)
	{
		p->Height.SetHighOrg[i] = g_Para.Lift.SetHighOrg[i];
		p->Height.SetLowOrg[i]	= g_Para.Lift.SetLowOrg[i];
	}

	//平台类型
	p->CtrlPara.PfType = g_Para.Lift.PfType;

	/*设置控制参数，泰坦三点液压举升默认3组控制；其他平台通过全局设置*/ 
	if(p->CtrlPara.PfType == LC_PFTP_Titan)
	{
		/*20251017 这里不能写死为3了，要在项目配置文件里配置，因为有的超低车要改四点液压
		p->CtrlPara.CtrlNnm = 3;*/
	}
	else
	{
		p->CtrlPara.CtrlNnm = g_Para.Lift.Num;
	}
	
	p->CtrlPara.LimitUp 	= g_Para.Lift.LimitUp;
	p->CtrlPara.LimitDown 	= g_Para.Lift.LimitDown;
	
	//用内存复制方法，将临时速度表变量赋值给实际速度表
	memcpy(p->CtrlPara.SpeedPlanUp,g_Para.Lift.SpeedPlanUp,sizeof(g_Para.Lift.SpeedPlanUp));
	memcpy(p->CtrlPara.SpeedPlanDown,g_Para.Lift.SpeedPlanDown,sizeof(g_Para.Lift.SpeedPlanDown));
	
	if(p->CtrlPara.PfType == LC_PFTP_Titan)	{p->CtrlPara.k_HgDiff2SpeedDiff = g_Para.Lift.Titan.k_HgDiff2SpeedDiff;}
	else									{p->CtrlPara.k_HgDiff2SpeedDiff = 0;}	//非titan车型 参数设0
	
	// 设置警告参数
	g_LiftData.WarningPara.DiffMax 	= g_Para.Lift.DiffMax;

	//泵归一化参数 初始化
	LiftCtrl_PumpUnifor(&p->CtrlPara);

	return 0;
}

/**************************************************************************
* 函数名:AbsorptionCtrl_Init
* 功  能:模块初始化内容
* 参  数:
* 返回值:bool
**************************************************************************/
bool AbsorptionCtrl_Init(LiftCtrl_TypeDef* p)
{
	char i = 0;
	// 设置高度参数
	if(p->Height.Num == 0)		{p->Height.Num = 4;}
	
	p->Height.SetLow 	= g_Para.Absorption.SetLow;
	p->Height.SetHigh 	= g_Para.Absorption.SetHigh;
	p->Height.Quadrant[i] = LiftCtrl_Quadrant_LF; i++;
	p->Height.Quadrant[i] = LiftCtrl_Quadrant_RF; i++;
	p->Height.Quadrant[i] = LiftCtrl_Quadrant_LB; i++;
	p->Height.Quadrant[i] = LiftCtrl_Quadrant_RB; i++;

	//初始化标定参数
	for(i=0;i<p->Height.Num;i++)
	{
		if(g_Para.Absorption.SetHighOrg[i] != 0)
		{
			p->Height.SetHighOrg[i] = g_Para.Absorption.SetHighOrg[i];
		}
		else{p->Height.SetHighOrg[i] = 1000;}
		if(g_Para.Absorption.SetLowOrg[i] < g_Para.Absorption.SetHighOrg[i])
		{
			p->Height.SetLowOrg[i] = g_Para.Absorption.SetLowOrg[i];
		}
		else{p->Height.SetLowOrg[i]	= 0;}
	}

	/*软件限高，如果没有参数输入，默认100*/
	if(g_Para.Absorption.LimitUp != 0)
	{
		p->CtrlPara.LimitUp 	= g_Para.Absorption.LimitUp;
	}
	else{p->CtrlPara.LimitUp = 100;}
	/*软件限低，如果比软件限高还要低，默认0*/
	if(g_Para.Absorption.LimitDown <= g_Para.Absorption.LimitUp)
	{
		p->CtrlPara.LimitDown = g_Para.Absorption.LimitDown;
	}
	else{p->CtrlPara.LimitDown = 0;}
	
	
	//用内存复制方法，将临时速度表变量赋值给实际速度表
	memcpy(p->CtrlPara.SpeedPlanUp,g_Para.Absorption.SpeedPlanUp,sizeof(g_Para.Absorption.SpeedPlanUp));
	memcpy(p->CtrlPara.SpeedPlanDown,g_Para.Absorption.SpeedPlanDown,sizeof(g_Para.Absorption.SpeedPlanDown));

	//类型 现仅支持泰坦三点液压
	//p->CtrlPara.PfType = LC_PFTP_Titan;

	if(p->CtrlPara.PfType == LC_PFTP_Titan)
	{
		/*设置控制参数，泰坦三点液压举升默认3组控制；其他平台通过全局设置*/ 
		p->CtrlPara.CtrlNnm = 3;

		//控制参数
		p->CtrlPara.k_HgDiff2SpeedDiff = g_Para.Absorption.Titan.k_HgDiff2SpeedDiff;

		if(p->CtrlPara.Pump[0] == 0)			{p->CtrlPara.Pump[0] = 1;}
		if(p->CtrlPara.Pump[1] == 0)			{p->CtrlPara.Pump[1] = 1;}
		if(p->CtrlPara.Pump[2] == 0)			{p->CtrlPara.Pump[2] = 1;}

		if(p->CtrlPara.PumpOnDelayMs == 0)		{p->CtrlPara.PumpOnDelayMs = 500;}
		if(p->CtrlPara.ValveOffDelayMs == 0)	{p->CtrlPara.ValveOffDelayMs = 500;}
		if(p->CtrlPara.ValveOffTgHg == 0)		{p->CtrlPara.ValveOffTgHg = 15;}
		if(p->CtrlPara.ValveReOnTgHg == 0)		{p->CtrlPara.ValveReOnTgHg = 8;}

		// 设置警告参数
		if(p->WarningPara.DiffMax == 0)			{p->WarningPara.DiffMax = 20;}
		if(p->WarningPara.HeightLow == 0)		{p->WarningPara.HeightLow = 0;}
		if(p->WarningPara.HeightHigh == 0)		{p->WarningPara.HeightHigh = 210;}
		if(p->WarningPara.DiffRearMax == 0)		{p->WarningPara.DiffRearMax = 10;}
		if(p->WarningPara.TimeoutTime == 0)		{p->WarningPara.TimeoutTime = 10000;}
		if(p->WarningPara.TimeoutHeight == 0)	{p->WarningPara.TimeoutHeight = 1;}
	}
	else
	{
		p->CtrlPara.CtrlNnm = g_Para.Absorption.Num;
		//其他类型悬挂类型需要进一步代码编写，暂时不支持其他类型
	}

	//模拟器参数设置
	p->Simulator.k_Mot2Hg=0.001;
	p->Simulator.CallDelayMs=1;
	return 0;
}

/**************************************************************************
* 函数名:LoadDetection_Init
* 功  能:模块初始化内容
* 参  数:
* 返回值:bool
**************************************************************************/
bool LoadDetection_Init(LoadDetection_TypeDef *p)
{

	// 自重
	p->WeightNoload 	= g_Para.Load.WeightNoload;
	// 报警阈值
	g_Para.Load.OverLoad= g_Para.Other.OverLoad;
	p->Errpar.OverLoad	= g_Para.Load.OverLoad;
	p->Errpar.SingleMax	= g_Para.Load.SingleMax;

	return 0;
}

/**************************************************************************
* 函数名:ShelfAutadj_Init
* 功  能:模块初始化内容
* 参  数:
* 返回值:bool
**************************************************************************/
bool ShelfAutadj_Init(ShelfAutadj_TypeDef *p)
{
	char res=0;
	
	//初始化方法指针
	p->p_ShelfAutadj_SideStatusFuc=ShelfAutadj_SideStatusFuc;
	p->p_ShelfAutadj_ModeFuc=ShelfAutadj_ModeFuc;

	//初始化固定参数
    p->DistanceMin=10;
    p->DistanceMax=270;
    p->Num=4;

    //偏移到纠正输出的计算系数
    p->k2vx=0.1;
    p->k2vz=0.1;
    p->k2r=0.1;

    res=1;
	
	return res;
}

//磁导航CBB初始化
bool MagData_Init(void)
{
	if(g_MNData[0].Diff == 0)			{g_MNData[0].Diff = (-1);}
	if(g_MNData[1].Diff == 0)			{g_MNData[1].Diff = (-1);}
	if(g_MNData[2].Diff == 0)			{g_MNData[2].Diff = (+1);}
	if(g_MNData[3].Diff == 0)			{g_MNData[3].Diff = (+1);}
	return 1;
}

//语音CBB初始化
bool VoicePlayer_Init(VoicePlayer_TypeDef* p)
{
	if(p->MsPlay == 0)					{p->MsPlay = 5000;}		//播放时间参数
	if(p->MsPlayOff == 0)				{p->MsPlayOff = 1000;}	//播放停止间隔时间参数

	p->MsTriggerHold = p->MsPlay-100;							//触发保持时间参数,暂时这样以避免触发提醒可能会被打断
    p->Voice_ID = Voice_Null;
    p->Music_Volume = 28;
    return 1;
}

//联动测试初始化
bool Assembly_Init(AsbCtrl_TypeDef* p)
{
	p->P2Obj.p2Self = &(p->Cars[p->Para.Iam - AsbCarStrNo_1]);
	return 1;
}

// 环境温度初始化
void AmbientTemp_Init(void)
{
	// 环境温度阈值
	g_Fan.in_Temperature_Environment_FanOff = g_Para.Other.in_Temperature_Environment_FanOff;
	if(g_Fan.in_Temperature_Environment_FanOff == 0)	{g_Fan.in_Temperature_Environment_FanOff = 60;}
	else												{g_Fan.in_Temperature_Environment_FanOff = g_Fan.in_Temperature_Environment_FanOff;}
}
/**************************************************************************
* 函数名:CBB_ObjInit
* 功  能:模块初始化内容
* 参  数:无
* 返回值:bool
**************************************************************************/
bool CBB_ObjInit(void)
{
	char errCnt=0;

	//舵轮解算初始化 巡线模块对此有依赖
	MotorResolving_Init(&g_MotorResolving);

	//磁导航模块初始化
	MagData_Init();

	//磁条巡线模块初始化
	MagneticTrace_Init(&g_MagneticTrace);

	//轮系跟随巡线计算模块初始化
	CenterPointCal_Init(&g_CPC);
	
	//巡磁钉模块初始化
	MagneticNailLine_Init(&DisMagNavData);
	
	//声光报警模块初始化
	//SoundLightNotice_Init(&g_SLN);//用户自定义初始化
	SoundLightNotice_InitExample(&g_SLN);//用户自定义初始化示例
	
	//Mecanum解算初始化
	MecanumResolving_Init(&g_MecanumResolving);

	//举升控制初始化
	LiftCtrl_Init(&g_LiftData);
	
	//悬挂/减震控制初始化
	AbsorptionCtrl_Init(&g_AbsorptionData);
	
	/*负载监测初始化 此处注释掉，则项目配置文件中可配置生效*/
	LoadDetection_Init(&g_LoadDetection);

	//自动对中初始化
	ShelfAutadj_Init(&g_ShelfAutadj);

	//虚拟磁条对象初始化
	VirtualMagnetic_Init(&g_VirtualMagnetic);

	//虚拟里程计CBB对象 初始化
    OdometerData_Init(&g_VirtualOdoData);

	//遥控器控制数据初始化
	WirelessHandleCtrlData_Init(&g_WHData);

	//数据平滑参数初始化
	JoystickDataSmooth_Init();
	
	//避障CBB对象初始化
	ROA_Init(&g_ROA);

    //语音播报初始化
    VoicePlayer_Init(&g_VoicePlayer);
	
	//联动模块初始化
	Assembly_Init(&g_Asb);

	//AGV自动运行依赖CBB库初始化
	if(AutoRunObj_Init()==0)
	{
		errCnt++;
	}
	
	// 环境温度初始化
	AmbientTemp_Init();

	if(errCnt>0)
	{
		return 0;
	}
	else
	{
		return 1;
	}

}



//***********************************END OF FILE***********************************

