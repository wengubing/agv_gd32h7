/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: AgvChassisController.c
** 创 建 人: 文小兵
** 描    述: AGV 底盘软件控制系统 源文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0
** 创建日期: 2024年4月19日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define  AgvChassisController_GLOBALS

//包含对象头文件
#include "AgvChassisController.h"
#include "CBB_Init.h"
#include "ACC_Init.h"
#include "basecalfunc.h"
#include "Acc_Project.h"

//内部调用
#include "ACC_LineTrace.h"
#include "ACC_OBS.h"
#include "ACC_FaultsResponse.h"
#include "ACC_ModeSwitch.h"
#include "ACC_ServoEnCtrl.h"

//#define ACC_USE_WEAK_FUNCTION								//使用弱函数，疑问：vscode中有错误提示

//弱函数控制
#ifdef ACC_USE_WEAK_FUNCTION
	#define ACC_WEAK __attribute__((weak))
#else
	#define ACC_WEAK
#endif

//ms计数接口
bool Acc_MsCnt(void)
{
	if(AccData.DelayMs.ModeChange < 65535)
    	AccData.DelayMs.ModeChange++;

    return 1;
}

//数据平滑处理对象时间戳更新
//仅在模式状态机切换进入模式时调用一次，防止增量时间戳过大导致平滑模块输出突变
bool SmoothTimestampUpdata(void)
{
	g_SmoothVx.out_Timestamp = g_Sys_1ms_Counter;
	g_SmoothVy.out_Timestamp = g_Sys_1ms_Counter;
	g_SmoothVz.out_Timestamp = g_Sys_1ms_Counter;
	g_SmoothAx.out_Timestamp = g_Sys_1ms_Counter;
	g_SmoothAy.out_Timestamp = g_Sys_1ms_Counter;
	g_SmoothAz.out_Timestamp = g_Sys_1ms_Counter;

	return 1;
}

//仅更新Vxzy时间戳
bool SmoothTimestampUpdata_Vxyz(void)
{
	g_SmoothVx.out_Timestamp = g_Sys_1ms_Counter;
	g_SmoothVy.out_Timestamp = g_Sys_1ms_Counter;
	g_SmoothVz.out_Timestamp = g_Sys_1ms_Counter;

	return 1;
}

//仅更新Axzy时间戳
bool SmoothTimestampUpdata_Axyz(void)
{
	g_SmoothAx.out_Timestamp = g_Sys_1ms_Counter;
	g_SmoothAy.out_Timestamp = g_Sys_1ms_Counter;
	g_SmoothAz.out_Timestamp = g_Sys_1ms_Counter;

	return 1;
}

//清除cbb输入、状态
bool CLR_CBB(void)
{
    //清除舵轮cbb输入
    g_MotorResolving.Input.Oxyz[0] = 0;
    g_MotorResolving.Input.Oxyz[1] = 0;
    g_MotorResolving.Input.Oxyz[2] = 0;
    g_MotorResolving.Input.Aoxyz[0] = 0;
    g_MotorResolving.Input.Aoxyz[1] = 0;
    g_MotorResolving.Input.Aoxyz[2] = 0;

	g_SmoothVx.out_Value = 0;
	g_SmoothVy.out_Value = 0;
	g_SmoothVz.out_Value = 0;
	g_SmoothAx.out_Value = 0;
	g_SmoothAy.out_Value = 0;
	g_SmoothAz.out_Value = 0;

	g_SmoothVxyz[0].out_Value = 0;
	g_SmoothVxyz[1].out_Value = 0;
	g_SmoothVxyz[2].out_Value = 0;

	//清除升降CBB状态
	g_LiftData.Cmd.in_Up = 0;
	g_LiftData.Cmd.in_Down = 0;
	g_LiftData.Cmd.do_Up = 0;
	g_LiftData.Cmd.do_Down = 0;

    return 1;

}

//关闭所有电磁阀
void ACC_CloseAllValve(ACC_DATA_TypeDef *p)
{
	//泰坦平台
	if		(g_LiftData.CtrlPara.PfType == LC_PFTP_Titan)
	{
		p->IOCtrl.Platform_Titan_UpLF=0;
		p->IOCtrl.Platform_Titan_UpRF=0;
		p->IOCtrl.Platform_Titan_UpB=0;
		p->IOCtrl.Platform_Titan_DownLF=0;
		p->IOCtrl.Platform_Titan_DownRF=0;
		p->IOCtrl.Platform_Titan_DownB=0;
	}
	//剪刀架平台
	else if	(g_LiftData.CtrlPara.PfType == LC_PFTP_Scissors)
	{
		p->IOCtrl.Platform_Scissors_Down=0;
	}
	//联合泵平台
	else if	(g_LiftData.CtrlPara.PfType == LC_PFTP_UnitePump)
	{
		p->IOCtrl.Platform_UnitePump_UpLF=0;
		p->IOCtrl.Platform_UnitePump_UpRF=0;
		p->IOCtrl.Platform_UnitePump_UpLB=0;
		p->IOCtrl.Platform_UnitePump_UpRB=0;
		p->IOCtrl.Platform_UnitePump_DownLF=0;
		p->IOCtrl.Platform_UnitePump_DownRF=0;
		p->IOCtrl.Platform_UnitePump_DownLB=0;
		p->IOCtrl.Platform_UnitePump_DownRB=0;
	}
	//泰坦悬挂
	if		(g_AbsorptionData.CtrlPara.PfType == LC_PFTP_Titan)
	{
		p->IOCtrl.Absorption_Titan_UpLF=0;
		p->IOCtrl.Absorption_Titan_UpRF=0;
		p->IOCtrl.Absorption_Titan_UpB=0;
		p->IOCtrl.Absorption_Titan_DownLF=0;
		p->IOCtrl.Absorption_Titan_DownRF=0;
		p->IOCtrl.Absorption_Titan_DownB=0;
	}
}



//是否为行走模式判断
bool ACC_IsWalkMode(ACCMode_TypeDef in_Mode)
{
	if(   in_Mode==ACCMode_PosFlexible
		||in_Mode==ACCMode_PosStraight
		||in_Mode==ACCMode_PosTrace
		||in_Mode==ACCMode_InfFlexible
		||in_Mode==ACCMode_InfStraight
		||in_Mode==ACCMode_InfTrace
		||in_Mode==ACCMode_Rotation
	)
		return 1;
	else
		return 0;
}

//判断是否为正向行走模式
bool ACC_IsPosWalkMode(ACCMode_TypeDef in_Mode)
{
	if(	  in_Mode==ACCMode_PosFlexible
		||in_Mode==ACCMode_PosStraight
		||in_Mode==ACCMode_PosTrace
	)
		return 1;
	else
		return 0;
}

//判断是否为横向行走模式
bool ACC_IsInfWalkMode(ACCMode_TypeDef in_Mode)
{
	if(	  in_Mode==ACCMode_InfFlexible
		||in_Mode==ACCMode_InfStraight
		||in_Mode==ACCMode_InfTrace
	)
		return 1;
	else
		return 0;
}

//判断是否为巡线模式
bool ACC_IsTraceMode(ACCMode_TypeDef in_Mode)
{
	if(	  in_Mode==ACCMode_PosTrace
		||in_Mode==ACCMode_InfTrace
	)
		return 1;
	else
		return 0;
}


/**************************************************************************
* 函数名:ACC_CommandCalculate_Process
* 功  能:控制命令处理
* 参  数:NC
* 返回值:NC
**************************************************************************/
void ACC_CommandCalculate_Process(ACC_DATA_TypeDef *p)
{
	f32 temp = 0;
	//模式
	p->CalCmd.Mode = p->OrgCmd.Mode;

	//速度命令输入限制
	p->CalCmd.Vxyz.x = AbsFuc(p->OrgCmd.Vxyz.x, p->JoystickMax);
	p->CalCmd.Vxyz.y = AbsFuc(p->OrgCmd.Vxyz.y, p->JoystickMax);
	p->CalCmd.Vxyz.z = AbsFuc(p->OrgCmd.Vxyz.z, p->JoystickMax);

	/*前进方向取反响应*/
	if (p->AsbCtrl.MoveDir_Reverse)
	{
		p->CalCmd.Vxyz.x = -p->CalCmd.Vxyz.x;
		p->CalCmd.Vxyz.y = -p->CalCmd.Vxyz.y;
	}
	else
		__asm__("nop");

	/*根据Vz限制Vy，类似圆形遥杆内x轴和y轴的线性牵制关系*/
	if (p->ChassisType != ACC_ChassisType_Turtle)//不是麦轮车才限制
	{
		//fix：修复Vz限制Vy因AOxyz平滑与Vy互锁问题
		p->CalCmd.Vxyz.y = AbsFuc(p->CalCmd.Vxyz.y, 160 - fabs(p->CalCmd.Vxyz.z));
		p->CalCmd.Vxyz.y = AbsFuc(p->CalCmd.Vxyz.y, 160 - fabs(p->CalCmd.VOxyz.z / ACC_WALKGEARMAX));
	}
	else {}

	//挡位输入
	p->CalCmd.WalkGear = p->OrgCmd.WalkGear;
	p->CalCmd.LiftGear = p->OrgCmd.LiftGear;

	//按钮输入
	p->CalCmd.Button = p->OrgCmd.Button;

	//避障使能输入
	p->CalCmd.BarEn = p->OrgCmd.BarEn;
	p->CalCmd.RadarEn = p->OrgCmd.RadarEn;

	//横向和自转模式时，行走挡位限制到3档
	if (p->CalCmd.Mode == ACCMode_InfStraight || p->CalCmd.Mode == ACCMode_InfFlexible
		|| p->CalCmd.Mode == ACCMode_InfTrace || p->CalCmd.Mode == ACCMode_Rotation)
	{
		p->CalCmd.WalkGear = AbsFuc(p->CalCmd.WalkGear, 3);
	}
	else
	{
		p->CalCmd.WalkGear = AbsFuc(p->CalCmd.WalkGear, ACC_WALKGEARMAX);
	}

	//挡位最大值限制
	p->CalCmd.WalkGear = AbsFuc(p->CalCmd.WalkGear, ACC_WALKGEARMAX);
	p->CalCmd.LiftGear = AbsFuc(p->CalCmd.LiftGear, ACC_WALKGEARMAX);

	//舵轮横向与自转(舵轮车运动方向与Az符号映射，在后面处理了。运动方向改变导致目标角度突变？)
	p->CalCmd.Axyz.x = p->CalCmd.Vxyz.x;
	p->CalCmd.Axyz.y = p->CalCmd.Vxyz.y;
	p->CalCmd.Axyz.z = p->CalCmd.Vxyz.z;

	//Axyz半轴映射
	p->CalCmd.Axyz.x = RatioFuc(p->CalCmd.Axyz.x, 0, 80, 0, p->JoystickMax);
	p->CalCmd.Axyz.y = RatioFuc(p->CalCmd.Axyz.y, 0, 80, 0, p->JoystickMax);
	//p->CalCmd.Axyz.z = RatioFuc(p->CalCmd.Axyz.z, 0, 50, 0, p->JoystickMax);

	//直线模式时清除横向与自转
	if(p->CalCmd.Mode == ACCMode_PosStraight)
	{
		p->CalCmd.Axyz.x = 0;
		p->CalCmd.Axyz.z = 0;
		p->CalCmd.Vxyz.x = 0;
		p->CalCmd.Vxyz.z = 0;
	}

	if(p->CalCmd.Mode==ACCMode_InfStraight)
	{
		p->CalCmd.Axyz.y = 0;
		p->CalCmd.Axyz.z = 0;
		p->CalCmd.Vxyz.y = 0;
		p->CalCmd.Vxyz.z = 0;
	}

	//挡位换算
	//p->CalCmd.Vxyz.x=(p->CalCmd.Vxyz.x)*p->CalCmd.WalkGear;
	temp = ACC_WALKGEARMAX * ((f32)(p->SpeedPlan[p->CalCmd.WalkGear - 1]) / p->SpeedPlan[ACC_WALKGEARMAX - 1]);
	p->CalCmd.Vxyz.x = (p->CalCmd.Vxyz.x) * temp;
	p->CalCmd.Vxyz.y = (p->CalCmd.Vxyz.y) * temp;
	p->CalCmd.Vxyz.z = (p->CalCmd.Vxyz.z) * temp;

	/*麦轮时，限制横移和自转的速度命令*/
	if (p->ChassisType == ACC_ChassisType_Turtle)
	{
		p->CalCmd.Vxyz.x = AbsFuc(p->CalCmd.Vxyz.x, 300);
		p->CalCmd.Vxyz.z = AbsFuc(p->CalCmd.Vxyz.z, 300);
	}

	//	//非升降模式时清除升降按钮
	//	if(p->CalCmd.Mode!=ACCMode_Lift)
	//	{
	//		p->CalCmd.Button.LiftUp=0;
	//		p->CalCmd.Button.LiftDown=0;
	//	}

		//非升降、悬挂减震、支腿、推送、辊筒、模式时清除升降按钮
	if ((p->CalCmd.Mode != ACCMode_Lift) && (p->CalCmd.Mode != ACCMode_Absorption) && (p->CalCmd.Mode != ACCMode_Leg)
		&& (p->CalCmd.Mode != ACCMode_Push) && (p->CalCmd.Mode != ACCMode_Roller))
	{
		p->CalCmd.Button.LiftUp = 0;
		p->CalCmd.Button.LiftDown = 0;
	}

	//过程命令状态初始化
	p->CalCmd.BitPara.CentreCtrl = 0;

	//取反平滑前的Axyz
	if (
		(ACC_IsPosWalkMode(p->OrgCmd.Mode) && p->CalCmd.Vxyz.y < 0)
		|| (ACC_IsInfWalkMode(p->OrgCmd.Mode) && p->CalCmd.Vxyz.x < 0)
		)
	{
		p->CalCmd.Axyz.x = -p->CalCmd.Axyz.x;
		p->CalCmd.Axyz.y = -p->CalCmd.Axyz.y;
		p->CalCmd.Axyz.z = -p->CalCmd.Axyz.z;
	}
	else {}
}

//清除电机速度命令
void ACC_StopMotors(ACC_DATA_TypeDef *p)
{
	int i;
	for(i=0; i<ACC_WALKMOTOR_MAX; i++)
		p->WalkMotor[i].V=0;

	for(i=0; i<ACC_LIFTMOTOR_MAX; i++)
		p->LiftMotor[i].V=0;

	//将电机速度送往解算模块的差速输出，以便模拟角度积分计算
	for(i=0; i<g_MotorResolving.Para.MotorNumber; i++)
		g_MotorResolving.Output.WheelDiffSpeed[i]=p->WalkMotor[i].V;
}

//清除所有控制执行的命令
bool ACC_ClearCmd(ACC_DATA_TypeDef *p)
{
//	char i=0;
	p->CalCmd.Vxyz.x=0;
	p->CalCmd.Vxyz.y=0;
	p->CalCmd.Vxyz.z=0;
	p->CalCmd.WalkGear=1;
	p->CalCmd.LiftGear=1;
	p->CalCmd.Mode=ACCMode_Still;
	p->CalCmd.Button.LiftUp=0;
	p->CalCmd.Button.LiftDown=0;
	//按领导要求 喇叭和照明也清除
	p->CalCmd.Button.Horn =0;
	p->CalCmd.Button.Lighting =0;

	//清除电机使能和速度命令
	p->WalkServoEn=0;
	p->LiftServoEn=0;

	p->IOCtrl.KoRunBrake = 0;
	p->IOCtrl.KoRunSysPower = 0;
	p->IOCtrl.KoRunContactor = 0;

	//清除电机速度命令
	ACC_StopMotors(p);

	return 1;
}

//联动纠正响应
bool ACC_AsbAdj(ACC_DATA_TypeDef* p)
{
	AsbCtrl_TypeDef* p2Asb = &g_Asb;

	if (p2Asb->Input.AsbSel_Cmd == AsbHcmMode_Asb)
	{
		//限制V
		p->CalCmd.Vxyz.x = AbsFuc(p->CalCmd.Vxyz.x, p2Asb->Para.Acc.VxyzMax.x);
		p->CalCmd.Vxyz.y = AbsFuc(p->CalCmd.Vxyz.y, p2Asb->Para.Acc.VxyzMax.y);
		p->CalCmd.Vxyz.z = AbsFuc(p->CalCmd.Vxyz.z, p2Asb->Para.Acc.VxyzMax.z);

		//限制A
		p->CalCmd.Axyz.x = AbsFuc(p->CalCmd.Axyz.x, p2Asb->Para.Acc.AxyzMax.x);
		p->CalCmd.Axyz.y = AbsFuc(p->CalCmd.Axyz.y, p2Asb->Para.Acc.AxyzMax.y);
		p->CalCmd.Axyz.z = AbsFuc(p->CalCmd.Axyz.z, p2Asb->Para.Acc.AxyzMax.z);

		//摇杆命令都为0时，无纠正响应
		if (p->CalCmd.Vxyz.x == 0 && p->CalCmd.Vxyz.y == 0 && p->CalCmd.Vxyz.z == 0)
		{
			return 1;
		}
		else if (1)//正向模式
		{
			p->CalCmd.Vxyz.y += p2Asb->Output.Adj.Vy;//通过运动方向速度 纠正联动距离
			p->CalCmd.Vxyz.x += p2Asb->Output.Adj.Vx;//通过垂直方向分量 纠正横向距离
			p->CalCmd.Vxyz.z += p2Asb->Output.Adj.Vz;//通过旋转方向分量 纠正姿态角度
		}
		else
		{

		}
	}
	else
	{}

	return 1;
}

//档位换算后的速度限制
bool ACC_CmdLimit(ACC_DATA_TypeDef *p)
{
	p->CalCmd.Vxyz.x = AbsFuc(p->CalCmd.Vxyz.x, p->JoystickMax * ACC_WALKGEARMAX);
	p->CalCmd.Vxyz.y = AbsFuc(p->CalCmd.Vxyz.y, p->JoystickMax * ACC_WALKGEARMAX);
	p->CalCmd.Vxyz.z = AbsFuc(p->CalCmd.Vxyz.z, p->JoystickMax * ACC_WALKGEARMAX);
	p->CalCmd.Axyz.x = AbsFuc(p->CalCmd.Axyz.x, p->JoystickMax);
	p->CalCmd.Axyz.y = AbsFuc(p->CalCmd.Axyz.y, p->JoystickMax);
	p->CalCmd.Axyz.z = AbsFuc(p->CalCmd.Axyz.z, p->JoystickMax);

	return 1;
}

//平滑模块补丁
bool ACC_SmoothPatch(ACC_DATA_TypeDef *p)
{
	//处理Vxyz
	if (p->SM_RunFlg == 0)
	{
		//SmoothTimestampUpdata();
		SmoothTimestampUpdata_Vxyz();
	}
	else
	{
		//清除平滑模块运行标志
		p->SM_RunFlg = 0;
	}

	//处理Axyz
	if (p->SM_RunFlg_Axyz == 0)
	{
		//SmoothTimestampUpdata();
		SmoothTimestampUpdata_Axyz();
	}
	else
	{
		//清除平滑模块运行标志
		p->SM_RunFlg_Axyz = 0;
	}

	return 1;
}

/**************************************************************************
* 函数名:ACC_ShelfAutadjReady
* 功  能:自动对中就绪判断
* 参  数:
* 返回值:bool
**************************************************************************/
bool ACC_ShelfAutadjReady(ACC_DATA_TypeDef *p)
{
	//行走模式下、功能使能按钮按下、测距传感器状态满足
	if(
        AccData.CalCmd.Mode == ACCMode_PosFlexible
        && AccData.CalCmd.Button.ShelfAutadj == 1
        //&& ACC_ShelfAutadj_IsReady(p)
    )
		return 1;
	else
		return 0;
}
/**************************************************************************
* 函数名:ACC_ShelfAutadj_Process
* 功  能:自动对中处理
* 参  数:
* 返回值:bool
**************************************************************************/
void ACC_ShelfAutadj_Process(ACC_DATA_TypeDef *p)
{
	ShelfAutadj_TypeDef *p2ShelfAutadj=&g_ShelfAutadj;

	//调用自动对中控制计算方法
	g_ShelfAutadj.in_Velocity = AccData.CalCmd.Vxyz.y;
	ShelfAutadj_ModeFuc(p2ShelfAutadj);

	//获取纠正控制量
	if (p->ChassisType == ACC_ChassisType_Titan)//舵轮
	{
        p2ShelfAutadj->out_AdjStart = 1;
		if (p2ShelfAutadj->out_Mode == ShelfAutadj_Mode_Carturn)
		{
			p->CalCmd.Axyz.z = p2ShelfAutadj->out_CarAng;
		}
		else if (p2ShelfAutadj->out_Mode == ShelfAutadj_Mode_Parallel)
		{
			p->CalCmd.Axyz.x = p2ShelfAutadj->out_Vx;
			p->CalCmd.Axyz.z = 0;
		}
		else if (p2ShelfAutadj->out_Mode == ShelfAutadj_Mode_Complex)
		{
			p->CalCmd.Axyz.x = p2ShelfAutadj->out_Vx;
			p->CalCmd.Axyz.z = p2ShelfAutadj->out_Vz;
		}
        else
        {
            p2ShelfAutadj->out_AdjStart = 0;
        }
//			/*兼容STD补丁*/
//		if(p->CalCmd.VOxyz.y < 0)
//		{
//			p->CalCmd.Axyz.x *= -1;
//			p->CalCmd.Axyz.z *= -1; 
//		}
	}
	else//麦轮需要解算支持前桥转向模式
	{
	}

    //限速
    if(p2ShelfAutadj->out_AdjStart)
        p->CalCmd.Vxyz.y = AbsFuc(p->CalCmd.Vxyz.y,150);    //此处对超低车加强对中时Y速度限制
    
    //优化 当不满足对中条件时，停车且蜂鸣器常鸣
    if(p2ShelfAutadj->out_Mode == ShelfAutadj_Sidestatus_Null)
    {
         p->CalCmd.Vxyz.x = 0;
         p->CalCmd.Vxyz.y = 0;
         p->CalCmd.Vxyz.z = 0;
    }
   
}

//面板开关联动状态下，根据各自的距离传感器，限制前向运动
bool AsbDistance_MoveLimit(ACC_DATA_TypeDef* p)
{
	ACC_Joystick_TypeDef* p2Vxyz = &p->CalCmd.Vxyz;
	f32 LimitVy = 0;
	f32 DistanceInput = g_AssemlySensorData.LaserDis_L.Cal;//用在实时更新的测距数据

	/*面板单动 或 遥控器选联动 直接退出
	if (p->Input.AsbSel_Panel == 0 || p->Input.AsbSel_Cmd == AsbHcmMode_Asb)
		return 1;*/

		/*按领导要求：遥控器选联动 直接退出，即只要不是联动就启用距离限制*/
	if (g_Asb.Input.AsbSel_Cmd == AsbHcmMode_Asb)
		return 1;

	/*以前直接停车
	if (DistanceInput < AccData.Para.DisLimit_Min && AccData.Para.DisLimit_Min != 65535)//距离近
	{
		if (AccData.AsbCtrl.MoveDir_Reverse)//反向后限制往后走
		{
			if (p2Vxyz->y < 0)
				p2Vxyz->y = 0;
		}
		else//限制往前走
		{
			if (p2Vxyz->y > 0)
				p2Vxyz->y = 0;
		}
	}*/

	/*升级为根据距离先进行减速，最后再停车*/
	if (DistanceInput < AccData.Para.DisLimit_Min_SlowDown && AccData.Para.DisLimit_Min != 65535)//距离近
	{
		//计算速度限制
		if (DistanceInput < AccData.Para.DisLimit_Min)
			LimitVy = 0;
		else
		{
			/*无极限速
			LimitVy = (DistanceInput - AccData.Para.DisLimit_Min) / (AccData.Para.DisLimit_Min_SlowDown - AccData.Para.DisLimit_Min)
				* (AccData.JoystickMax);*/
				/*固定限速*/
			LimitVy = AccData.Para.DisLimit_Min_SlowDown_V;
		}

		//限制往前走
		if (p2Vxyz->y > 0)
			p2Vxyz->y = AbsFuc(p2Vxyz->y, LimitVy);
		else
			__NOP();
	}

	if (DistanceInput > AccData.Para.DisLimit_Max && AccData.Para.DisLimit_Max != 65535)//距离远
	{
		//限制往后走
		if (p2Vxyz->y < 0)
			p2Vxyz->y = 0;
	}
	else
		__NOP();

	return 1;
}

/**************************************************************************
* 函数名:AgvChassisControl
* 功  能:AGV底盘控制
* 参  数:ms
* 返回值:NC
**************************************************************************/
void AgvChassisControl(u32 in_Ms,ACC_DATA_TypeDef *p)
{
	static ACC_ApplicationType_EnumDef *p_Type = &(g_Para.ApplicationType);
	p->in_Ms = in_Ms;

	//项目自定义 AccStart函数
	if (g_Para.p2Fucs.p2PrjExp_AccStart != NULL)
		g_Para.p2Fucs.p2PrjExp_AccStart();
	else {}

	//单车控制命令处理
	ACC_CommandCalculate_Process(p);

	//联动下控制命令处理
	{
		if(	  *p_Type == ACC_Application_AssemlyRemote
			||*p_Type == ACC_Application_AssemlyAGV
		)
		{
			//联动下的单动时，根据各自的距离传感器，限制前向运动
			AsbDistance_MoveLimit(p);
			//联动纠正响应
			ACC_AsbAdj(p);
		}
	}

	//遥控巡线（包含磁导航、色带巡线）
	ACC_LineTrace_Process(p);

	/*自动对中纠正处理*/
	if(ACC_ShelfAutadjReady(p))
	{
		ACC_ShelfAutadj_Process(p);
    }
	else 
    {
        g_ShelfAutadj.out_AdjStart = 0;
    }

	//档位换算后的速度限制
	ACC_CmdLimit(p);

	//避障限速
	ACC_OBS_Process(p);

	//钥匙授权
	if ((g_IOInputs.Car_Key.ID != ICN_Disable && !g_IOInputs.Car_Key.StableValue)//车体钥匙
	 || g_ChannelSwitch.Bits.RemoteKeyAuth == 0//遥控器钥匙
	)
		ACC_ClearCmd(p);//清除所有控制执行的命令
	else {}

	//故障响应控制
	ACC_FaultsResponse_Process(p);

	//模式切换处理
	if (p->FaultsResponseSTM != ACC_FaultsType_LoseControlStop)//非受控停车状态下,正常模式切换和控制被中断
		ACC_ModeSwitch_Process(p);
	else {}

	//平滑模块补丁
	ACC_SmoothPatch(p);

	//具体某个伺服使能使能操作，行走升降集中一起处理
	ACC_ServoEnCtrl();

	//项目自定义 AccEnd函数
	if (g_Para.p2Fucs.p2PrjExp_AccEnd != NULL)
		g_Para.p2Fucs.p2PrjExp_AccEnd();
	else {}
}


//***********************************END OF FILE***********************************
