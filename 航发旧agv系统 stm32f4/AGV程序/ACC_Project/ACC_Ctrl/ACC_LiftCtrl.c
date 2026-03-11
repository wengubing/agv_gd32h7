/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ACC_LiftCtrl.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  ACCLIFTCTRL_GLOBALS

#include "ACC_LiftCtrl.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "ACC_Project.h"
#include "app_DoublePumpQuad_LiftCtrl.h"
/***********************************************************
** 函数名称:	LiftHighSet
** 功能描述: 	平台标定 高点
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//平台标定 高点
bool LiftHighSet(void)
{
	char i = 0;
	for(i=0;i<g_Para.Lift.Num;i++)
	{
		//写到hmi对象，因为系统参数对象一直在从hmi对象中读取
		//g_HMI.Height_Calibration[0].Height_HighValue_Calibration[i] = g_LiftData.Height.Org[i];

		//更新系统参数对象
		g_Para.Lift.SetHighOrg[i] = g_LiftData.Height.Org[i];
	}
	return 1;
}


/***********************************************************
** 函数名称:	LiftLowSet
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//平台标定 低点
bool LiftLowSet(void)
{
	char i = 0;
	for(i=0;i<g_Para.Lift.Num;i++)
	{
		//写到hmi对象，因为系统参数对象一直在从hmi对象中读取
		//g_HMI.Height_Calibration[0].Height_LowValue_Calibration[i] = g_LiftData.Height.Org[i];

		//更新系统参数对象
		g_Para.Lift.SetLowOrg[i] = g_LiftData.Height.Org[i];
	}
	return 1;
}


/***********************************************************
** 函数名称:	Lift_Ctrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
bool Lift_Ctrl(void)
{
	char i = 0;
	ACC_DATA_TypeDef* p2Acc		= &AccData;
	LiftCtrl_TypeDef* p2Lift	= &g_LiftData;

	//建立命令管道
	//p2Lift->Cmd.in_Ms		= p2Acc->in_Ms;		//在升降高度数据更新处进行系统时间更新
	p2Lift->Cmd.in_Up		= p2Acc->CalCmd.Button.LiftUp;
	p2Lift->Cmd.in_Down		= p2Acc->CalCmd.Button.LiftDown;
	p2Lift->Cmd.in_Gears	= p2Acc->CalCmd.LiftGear;

	//输入压力均衡相关数据
	for (i = 0; i < 4; i++)//暂定4点液压
	{
		p2Lift->PrBalance.Pr[i] = g_LoadDetection.Sensor[i].Cal;
	}
	p2Lift->PrBalance.Load = g_LoadDetection.WeightShow * 10;
	p2Lift->PrBalance.PrMax = g_LoadDetection.Extreme.max;
	p2Lift->PrBalance.PrMin = g_LoadDetection.Extreme.min;
	p2Lift->PrBalance.AsbFlag = g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb ? 1 : 0;

	/*调用CBB解算接口 这里移动到了具体平台类型的代码里
	//标定计算已与控制分离，在外部循环调用
	LiftCtrl_CmdPrep(&g_LiftData);
	LiftCtrl_Ctrl(&g_LiftData);*/
	
	if (p2Lift->Cmd.do_Gears)//当档位不为0时，才进行控制
	{
		//下面是联动车 通过调整速度档位表实现联动高度同步
		if (g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb)
		{
			p2Lift->CtrlPara.SpeedPlanUp[p2Lift->Cmd.do_Gears - 1] = g_Para.Lift.SpeedPlanUp[p2Lift->Cmd.do_Gears - 1] + g_Asb.Output.Adj.UpDown;
			p2Lift->CtrlPara.SpeedPlanDown[p2Lift->Cmd.do_Gears - 1] = g_Para.Lift.SpeedPlanDown[p2Lift->Cmd.do_Gears - 1] + g_Asb.Output.Adj.UpDown;
		}
		else
		{
			p2Lift->CtrlPara.SpeedPlanUp[p2Lift->Cmd.do_Gears - 1] = g_Para.Lift.SpeedPlanUp[p2Lift->Cmd.do_Gears - 1];
			p2Lift->CtrlPara.SpeedPlanDown[p2Lift->Cmd.do_Gears - 1] = g_Para.Lift.SpeedPlanDown[p2Lift->Cmd.do_Gears - 1];
		}
	}

	/*1、标准3点液压升降——四点检测、三点控制（伺服电机） 电机只需要正转泵油即可*/
	if(p2Lift->CtrlPara.PfType == LC_PFTP_Titan)
	{
		uint8_t i = 0;

		/*调用CBB解算接口*/
		//标定计算已与控制分离，在外部循环调用
		LiftCtrl_CmdPrep(&g_LiftData);
		LiftCtrl_Ctrl(&g_LiftData);//LiftCtrl_Ctrl将重构，只处理titan和超低车

		//电机控制
		for (i = 0; i < g_LiftData.CtrlPara.CtrlNnm; i++)
		{
			p2Acc->LiftMotor[i].V = p2Lift->CtrlOut.MotorSpeed[i];
		}
		//电磁阀控制输出
		p2Acc->IOCtrl.Platform_Titan_UpLF 	= p2Lift->CtrlOut.ValveUp[LiftCtrl_Ctrl_LF];
		p2Acc->IOCtrl.Platform_Titan_UpRF 	= p2Lift->CtrlOut.ValveUp[LiftCtrl_Ctrl_RF];
		p2Acc->IOCtrl.Platform_Titan_UpB  	= p2Lift->CtrlOut.ValveUp[LiftCtrl_Ctrl_B];
		p2Acc->IOCtrl.Platform_Titan_UpLB	= p2Lift->CtrlOut.ValveUp[LiftCtrl_Ctrl_LB];//仅四点有效
		p2Acc->IOCtrl.Platform_Titan_UpRB	= p2Lift->CtrlOut.ValveUp[LiftCtrl_Ctrl_RB];//仅四点有效
		p2Acc->IOCtrl.Platform_Titan_DownLF = p2Lift->CtrlOut.ValveDown[LiftCtrl_Ctrl_LF];
		p2Acc->IOCtrl.Platform_Titan_DownRF = p2Lift->CtrlOut.ValveDown[LiftCtrl_Ctrl_RF];
		p2Acc->IOCtrl.Platform_Titan_DownB 	= p2Lift->CtrlOut.ValveDown[LiftCtrl_Ctrl_B];
		p2Acc->IOCtrl.Platform_Titan_DownLB = p2Lift->CtrlOut.ValveDown[LiftCtrl_Ctrl_LB];//仅四点有效
		p2Acc->IOCtrl.Platform_Titan_DownRB = p2Lift->CtrlOut.ValveDown[LiftCtrl_Ctrl_RB];//仅四点有效
	}
	/*2、剪刀架 升降平台 单一电机+电磁阀控制升降 无调平功能*/
	else if(p2Lift->CtrlPara.PfType == LC_PFTP_Scissors)
	{
		/*调用CBB解算接口*/
		//标定计算已与控制分离，在外部循环调用
		LiftCtrl_CmdPrep(&g_LiftData);
		//LiftCtrl_Ctrl(&g_LiftData);
		//LiftCtrl_Ctrl只服务titan和超低车，该类型平台的MotorSpeed计算临时移了出来：
		if (p2Lift->Cmd.LiftSTM == LiftCtrl_STM_Up)
			p2Lift->CtrlOut.MotorSpeed[0] = p2Lift->CtrlPara.SpeedPlanUp[p2Lift->Cmd.do_Gears - 1];
		else if (p2Lift->Cmd.LiftSTM == LiftCtrl_STM_Down)
			p2Lift->CtrlOut.MotorSpeed[0] = p2Lift->CtrlPara.SpeedPlanDown[p2Lift->Cmd.do_Gears - 1];

		//电机速度反向
		if (p2Lift->CtrlPara.BitPara.Bits.MotorReverse)
			LiftCtrl_MotorReverse(p2Lift);

		//电机控制
		if(p2Lift->Cmd.do_Up)
		{
			p2Acc->LiftMotor[0].V = p2Lift->CtrlOut.MotorSpeed[0];
			//电磁阀全关闭
			p2Acc->IOCtrl.Platform_Scissors_Down = 0;
		}
		else if(p2Lift->Cmd.do_Down)
		{
			//电机速度为0
			p2Acc->LiftMotor[0].V = 0;
			//电磁阀控制  通过自重下降
			p2Acc->IOCtrl.Platform_Scissors_Down = p2Lift->CtrlOut.ValveDown[0];
		}
		else
		{
			p2Acc->LiftMotor[0].V = 0;
			p2Acc->IOCtrl.Platform_Scissors_Down = 0;
		}
		
	}
	/*3、丝杆 升降平台 没有油路控制  电机需要根据 上升下降命令 正转或者反转*/
	else if(p2Lift->CtrlPara.PfType == LC_PFTP_Screw)
	{
		/*调用CBB解算接口*/
		//标定计算已与控制分离，在外部循环调用
		LiftCtrl_CmdPrep(&g_LiftData);
		//LiftCtrl_Ctrl(&g_LiftData);
		//LiftCtrl_Ctrl只服务titan和超低车，该类型平台的MotorSpeed计算临时移了出来：
		if (p2Lift->Cmd.LiftSTM == LiftCtrl_STM_Up)
			p2Lift->CtrlOut.MotorSpeed[0] = p2Lift->CtrlPara.SpeedPlanUp[p2Lift->Cmd.do_Gears - 1];
		else if (p2Lift->Cmd.LiftSTM == LiftCtrl_STM_Down)
			p2Lift->CtrlOut.MotorSpeed[0] = p2Lift->CtrlPara.SpeedPlanDown[p2Lift->Cmd.do_Gears - 1];

		//电机速度反向
		if (p2Lift->CtrlPara.BitPara.Bits.MotorReverse)
			LiftCtrl_MotorReverse(p2Lift);

		//电机控制 区分
		if(p2Lift->Cmd.do_Up)
		{
			p2Acc->LiftMotor[0].V = p2Lift->CtrlOut.MotorSpeed[0];
		}
		else if(p2Lift->Cmd.do_Down)
		{
			p2Acc->LiftMotor[0].V = -1*p2Lift->CtrlOut.MotorSpeed[0];
		}
		else
		{
			p2Acc->LiftMotor[0].V = 0;
		}
	}
	/*4、联合泵 电磁阀控制同步  单一电机+多个电磁阀控制 通过电磁阀开关调平（暂未适配）*/
	else if(p2Lift->CtrlPara.PfType == LC_PFTP_UnitePump)
	{
		//电机控制
		p2Acc->LiftMotor[0].V = p2Lift->CtrlOut.MotorSpeed[0];

		//电磁阀控制输出
		p2Acc->IOCtrl.Platform_UnitePump_UpLF 	= p2Lift->CtrlOut.ValveUp[LiftCtrl_Ctrl_LF];
		p2Acc->IOCtrl.Platform_UnitePump_UpRF 	= p2Lift->CtrlOut.ValveUp[LiftCtrl_Ctrl_RF];
		p2Acc->IOCtrl.Platform_UnitePump_UpLB 	= p2Lift->CtrlOut.ValveUp[LiftCtrl_Ctrl_LB];
		p2Acc->IOCtrl.Platform_UnitePump_UpRB 	= p2Lift->CtrlOut.ValveUp[LiftCtrl_Ctrl_RB];
		p2Acc->IOCtrl.Platform_UnitePump_DownLF = p2Lift->CtrlOut.ValveDown[LiftCtrl_Ctrl_LF];
		p2Acc->IOCtrl.Platform_UnitePump_DownRF = p2Lift->CtrlOut.ValveDown[LiftCtrl_Ctrl_RF];
		p2Acc->IOCtrl.Platform_UnitePump_DownLB = p2Lift->CtrlOut.ValveDown[LiftCtrl_Ctrl_LB];
		p2Acc->IOCtrl.Platform_UnitePump_DownRB = p2Lift->CtrlOut.ValveDown[LiftCtrl_Ctrl_RB];

	}
	/*5、同步油缸 液压排油同步（暂未适配）*/
	else if(p2Lift->CtrlPara.PfType == LC_PFTP_SynCylinder)
	{
		
	}
	/*6、双泵四点液压（暂未适配）*/
	else if(p2Lift->CtrlPara.PfType == LC_PFTP_DoublePumpQuad)
	{

	}
	else if (p2Lift->CtrlPara.PfType == LC_PFTP_Scissors_Two)//这是从LiftCtrl_Ctrl里移出来放到下面的，未经过测试
	{
		char i = 0;
		f32 speed_temp = 0;
		f32 k_HgDiff2SpeedDiff_temp = 0;
		
		//强制控制时 同步控制系数为0，即不进行同步控制
		if (p2Lift->Cmd.do_Force)
			k_HgDiff2SpeedDiff_temp = 0;
		else
			k_HgDiff2SpeedDiff_temp = p2Lift->CtrlPara.k_HgDiff2SpeedDiff;
		
		for (i = 0; i < p2Lift->CtrlPara.CtrlNnm; i++)
		{
			if (p2Lift->Cmd.LiftSTM == LiftCtrl_STM_Up)
			{
				speed_temp = p2Lift->CtrlPara.SpeedPlanUp[p2Lift->Cmd.do_Gears - 1] * p2Lift->CtrlPara.PumpUnifor2Speed[i]
					- (p2Lift->Height.QuadrantAvg[i] - p2Lift->Height.Extreme.Lowest) * k_HgDiff2SpeedDiff_temp;
			}
			else if (p2Lift->Cmd.LiftSTM == LiftCtrl_STM_Down)
			{
				speed_temp = p2Lift->CtrlPara.SpeedPlanDown[p2Lift->Cmd.do_Gears - 1] * p2Lift->CtrlPara.PumpUnifor2Speed[i]
					- fabs(p2Lift->Height.QuadrantAvg[i] - p2Lift->Height.Extreme.Highest) * k_HgDiff2SpeedDiff_temp;
			}

			if (speed_temp < 0)
				p2Lift->CtrlOut.MotorSpeed[i] = 0;
			else
				p2Lift->CtrlOut.MotorSpeed[i] = speed_temp;
		}
	}

	//如果升降为科润阳光驱动器
	if(g_DAFucs.KoRunCtrl.Id != DACN_Disable)
	{
		static u32 MsTemp=0;
		if(p2Acc->LiftMotor[0].V < 0)
		{
			p2Acc->IOCtrl.KoRunReversal = 1;
			/*科润阳光反转通过IO控制，速度需要是正值*/
			p2Acc->LiftMotor[0].V = -p2Acc->LiftMotor[0].V;
		}
		else
		{
			p2Acc->IOCtrl.KoRunReversal = 0;
		}

		if(p2Acc->LiftMotor[0].V != 0)
		{
			p2Acc->IOCtrl.KoRunBrake = 1;
			p2Acc->IOCtrl.KoRunSysPower = 1;
			p2Acc->IOCtrl.KoRunContactor = 1;
			MsTemp = p2Acc->in_Ms;
		}	
		else
		{
			if(p2Acc->in_Ms - MsTemp > 5000)//延时关闭科润刹车信号
			{
				p2Acc->IOCtrl.KoRunBrake = 0;
			}

			p2Acc->IOCtrl.KoRunSysPower = 0;
			p2Acc->IOCtrl.KoRunContactor = 0;
		}
	}

	return 0;
}

/************************************END OF FILE************************************/
