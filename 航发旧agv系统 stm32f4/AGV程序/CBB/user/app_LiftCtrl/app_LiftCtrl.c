/********************************** Copyright ***********************************
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
*
* FileName   : app_LiftCtrl.c
* Version    : V1.0
* Author     : WenXiaoBing
* Date       : 2024.1.19
* Description: 输入磁条偏移，输出整车控制进行巡线
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "basecalfunc.h"
#include "common_def.h"
#include "app_LiftCtrl.h"

/*模块内部的方法列表*/
void LiftCtrl_Exec(LiftCtrl_TypeDef* p);

//内部对象定义

//下降调平失败时 打开全部电磁阀控制
bool LiftCtrl_AutForceDown(LiftCtrl_AutForceDown_TypeDef* p)
{
	if (p->Do_ForceDown == 0)//未触发强制下降
	{
		if (p->In_Hg == p->In_Last)//高度没有变化
		{
			if (p->Para_Ms_Check != 65535)
			{
				if ((p->In_Ms - p->MsRecord_Check) > p->Para_Ms_Check)//持续没有变化超过设定值
				{
					p->Do_ForceDown = 1;
					p->MsRecord_Continue = p->In_Ms;
				}
				else
					__asm__("nop");
			}
			else
				__asm__("nop");
		}
		else//高度有变化
		{
			p->In_Last = p->In_Hg;
			p->MsRecord_Check = p->In_Ms;
		}
	}
	else//已经触发强制下降
	{
		if ((p->In_Ms - p->MsRecord_Continue) > p->Para_Ms_Continue)//持续下降时间到
		{
			p->Do_ForceDown = 0;
		}
		else
			__asm__("nop");
	}

	return p->Do_ForceDown;
}

//高度标定
bool LiftCtrl_Cal(LiftCtrl_Height_TypeDef* p)
{
	char i = 0;
	f32 sum_temp = 0;

	//开始新一次计算
	for (i = 0; i < LiftCtrl_Quadrant_Nnm; i++)
	{
		p->QuadrantCnt[i] = 0;
		p->QuadrantSum[i] = 0;
	}

	//分组计数、求和
	for (i = 0; i < p->Num; i++)
	{
		p->Cal[i] = RatioStand(p->Org[i], p->SetLowOrg[i],
			p->SetHighOrg[i], p->SetLow, p->SetHigh);

		p->QuadrantCnt[p->Quadrant[i]]++;
		p->QuadrantSum[p->Quadrant[i]] = p->QuadrantSum[p->Quadrant[i]]
			+ p->Cal[i];
	}

	//计算各组平均
	for (i = 0; i < LiftCtrl_Quadrant_Nnm; i++)
	{
		p->QuadrantAvg[i] = p->QuadrantSum[i] / p->QuadrantCnt[i];
	}

	//计算三点平均
	p->Point3Avg[LiftCtrl_Ctrl_LF] = p->QuadrantAvg[LiftCtrl_Quadrant_LF];
	p->Point3Avg[LiftCtrl_Ctrl_RF] = p->QuadrantAvg[LiftCtrl_Quadrant_RF];
	p->Point3Avg[LiftCtrl_Ctrl_B] = (p->QuadrantAvg[LiftCtrl_Quadrant_LB]
		+ p->QuadrantAvg[LiftCtrl_Quadrant_RB]) / 2;

	//计算总平均
	for (i = 0; i < LiftCtrl_Quadrant_Nnm; i++)
	{
		sum_temp = sum_temp + p->QuadrantAvg[i];
	}
	p->Avg = sum_temp / 4;

	return 1;
}

//命令预处理
void LiftCtrl_CmdPrep(LiftCtrl_TypeDef* p)
{
	p->Cmd.do_Gears = p->Cmd.in_Gears;

	//是否限位
	if (p->Cmd.in_Up && !p->Cmd.in_Down)
	{
		if (p->Height.Avg > p->CtrlPara.LimitUp && p->Cmd.do_Force == 0)
		{
			p->Cmd.do_Up = 0;
			p->Cmd.do_Down = 0;
			p->CtrlOut.LiftST = LiftCtrl_LiftST_LimitHigh;
		}
		else
		{
			p->Cmd.do_Up = 1;
			p->Cmd.do_Down = 0;
			p->CtrlOut.LiftST = LiftCtrl_LiftST_Up;
		}
	}
	else if (!p->Cmd.in_Up && p->Cmd.in_Down)
	{
		if (p->Height.Avg < p->CtrlPara.LimitDown && p->Cmd.do_Force == 0)
		{
			p->Cmd.do_Up = 0;
			p->Cmd.do_Down = 0;
			p->CtrlOut.LiftST = LiftCtrl_LiftST_LimitLow;
		}
		else
		{
			p->Cmd.do_Up = 0;
			p->Cmd.do_Down = 1;
			p->CtrlOut.LiftST = LiftCtrl_LiftST_Down;
		}
	}
	else
	{
		p->Cmd.do_Up = 0;
		p->Cmd.do_Down = 0;
		p->CtrlOut.LiftST = LiftCtrl_LiftST_Normal;
	}
}

//泵流量比归一化 转换为速度比
bool LiftCtrl_PumpUnifor(LiftCtrl_CtrlPara_TypeDef* p)
{
	char i = 0;
	Unifor_TypeDef unifor_temp;

	unifor_temp.data_num = p->CtrlNnm;

	for (i = 0; i < p->CtrlNnm; i++)
	{
		unifor_temp.org[i] = p->Pump[i];
	}

	unifor_find_max_min(&unifor_temp);
	unifor_cal(&unifor_temp);

	for (i = 0; i < p->CtrlNnm; i++)
	{
		p->PumpUnifor[i] = unifor_temp.unifor[i];
	}

	for (i = 0; i < p->CtrlNnm; i++)
	{
		p->PumpUnifor2Speed[i] = unifor_temp.unifor[unifor_temp.min_id]
			/ p->PumpUnifor[i];
	}

	return 1;
}

//高度数据均值、极值分析计算
bool LiftCtrl_Extreme(LiftCtrl_Height_TypeDef* p)
{
	char i = 0;
	Unifor_TypeDef unifor_temp;

	//单独的各组高度计算极值
	unifor_temp.data_num = p->Num;
	for (i = 0; i < p->Num; i++)
		unifor_temp.org[i] = p->Cal[i];

	unifor_find_max_min(&unifor_temp);
	p->Extreme.Lowest = unifor_temp.min;
	p->Extreme.LowestID = unifor_temp.min_id;
	p->Extreme.Highest = unifor_temp.max;
	p->Extreme.HighestID = unifor_temp.max_id;

	//titan3点高度计算极值
	unifor_temp.data_num = 3;
	unifor_temp.org[0] = p->Point3Avg[LiftCtrl_Ctrl_LF];
	unifor_temp.org[1] = p->Point3Avg[LiftCtrl_Ctrl_RF];
	unifor_temp.org[2] = p->Point3Avg[LiftCtrl_Ctrl_B];
	unifor_find_max_min(&unifor_temp);
	p->Extreme_Titan.Lowest = unifor_temp.min;
	p->Extreme_Titan.LowestID = unifor_temp.min_id;
	p->Extreme_Titan.Highest = unifor_temp.max;
	p->Extreme_Titan.HighestID = unifor_temp.max_id;

	//按象限位置进行分组后的，各组高度计算极值
	//unifor_temp.data_num = p->CtrlPara.CtrlNnm;
	unifor_temp.data_num = 4;
	for (i = 0; i < unifor_temp.data_num; i++)
		unifor_temp.org[i] = p->QuadrantAvg[i];
	unifor_find_max_min(&unifor_temp);
	p->Extreme_Quadrant.Lowest = unifor_temp.min;
	p->Extreme_Quadrant.LowestID = unifor_temp.min_id;
	p->Extreme_Quadrant.Highest = unifor_temp.max;
	p->Extreme_Quadrant.HighestID = unifor_temp.max_id;

	return 1;
}

//阀门关闭
void LiftCtrl_ValveOff(LiftCtrl_TypeDef* p)
{
	char i = 0;
	for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
	{
		p->CtrlOut.ValveUp[i] = 0;
		p->CtrlOut.ValveDown[i] = 0;
	}
}

//上升阀门打开
void LiftCtrl_AllValveUp(LiftCtrl_TypeDef* p)
{
	char i = 0;
	for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
	{
		p->CtrlOut.ValveUp[i] = 1;
		p->CtrlOut.ValveDown[i] = 0;
	}
}

//下降阀门打开
void LiftCtrl_AllValveDown(LiftCtrl_TypeDef* p)
{
	char i = 0;
	for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
	{
		p->CtrlOut.ValveUp[i] = 0;
		p->CtrlOut.ValveDown[i] = 1;
	}
}

//所有电机关闭
void LiftCtrl_AllMotorOff(LiftCtrl_TypeDef* p)
{
	char i = 0;
	for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
	{
		p->CtrlOut.MotorSpeed[i] = 0;
	}
}

//所有阀门、电机关闭
void LiftCtrl_AllValveMotorOff(LiftCtrl_TypeDef* p)
{
	char i = 0;
	for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
	//for (i = 0; i < LIFT_HG_MAX; i++)//防止参数设置错误导致数组越界、防止中间跳跃
	{
		p->CtrlOut.ValveUp[i] = 0;
		p->CtrlOut.ValveDown[i] = 0;
		p->CtrlOut.MotorSpeed[i] = 0;
	}
}

//单组到限位即关电机
bool LiftCtrl_SingleReachStop(LiftCtrl_TypeDef* p)
{
	char i = 0;
	f32* p2Avgs;

	/*如果没有开启该功能 直接返回*/
	if (p->CtrlPara.BitPara.Bits.SingleReachStop == 0)
		return 1;

	//如果三点控制，使用三点平均值
	if (p->CtrlPara.CtrlNnm == 3)
		p2Avgs = p->Height.Point3Avg;
	else
		p2Avgs = p->Height.QuadrantAvg;

	//上升且大于限位高度
	for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
	{
		if (p->Cmd.LiftSTM == LiftCtrl_STM_Up && p2Avgs[i] > p->CtrlPara.LimitUp && p->Cmd.do_Force == 0)
			p->CtrlOut.MotorSpeed[i] = 0;
		else if (p->Cmd.LiftSTM == LiftCtrl_STM_Down && p2Avgs[i] < p->CtrlPara.LimitDown && p->Cmd.do_Force == 0)
			p->CtrlOut.MotorSpeed[i] = 0;
		else
			__asm__("nop");
	}
	return 1;
}

/*根据压力控制油泵电机速度*/
bool LiftCtrl_PressureCtrl(LiftCtrl_TypeDef* p)
{
	char i = 0;

	//进入压力调节条件：平均高度>30、负载>40%、联动
	if (p->Height.Avg > p->PrBalance.HgMin
		&& p->PrBalance.Load > p->PrBalance.LoadMin
		&& p->PrBalance.AsbFlag
	)
	{
		//转速消减（前提：减高度 == 减压力） = （本身压力 - 最小压力 ） * K
		for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
		{
			p->PrBalance.SlowDown[i] = (p->PrBalance.Pr[i] - p->PrBalance.PrMin)
				* p->PrBalance.k_Pr2Slow;

			//限制最大消减量
			if (p->PrBalance.SlowDown[i] > p->PrBalance.SlowDownMax)
				p->PrBalance.SlowDown[i] = p->PrBalance.SlowDownMax;

			//实际转速比消减量大时，实际转速 = 计划转速 - 消减转速，不然实际转速 = 0
			if (p->CtrlOut.MotorSpeed[i] > p->PrBalance.SlowDown[i])
				p->CtrlOut.MotorSpeed[i] = p->CtrlOut.MotorSpeed[i] - p->PrBalance.SlowDown[i];
			else
				p->CtrlOut.MotorSpeed[i] = 0;
		}
	}
	else
	{
		//什么都不做
	}

	return 1;
}

//电机控制
bool LC_MotorCtrl(LiftCtrl_TypeDef* p)
{
	char i = 0;
	f32 speed_temp = 0;
	f32 k_HgDiff2SpeedDiff_temp = 0;
	f32* p2Avgs;
	LiftCtrl_Extreme_TypeDef* p2Ext;

	//如果三点控制，使用三点平均值
	if (p->CtrlPara.CtrlNnm == 3)
	{
		p2Avgs = p->Height.Point3Avg;
		p2Ext = &p->Height.Extreme_Titan;
	}
	else
	{
		p2Avgs = p->Height.QuadrantAvg;
		p2Ext = &p->Height.Extreme_Quadrant;
	}

	//强制控制时 同步控制系数为0，即不进行同步控制
	if (p->Cmd.do_Force)
		k_HgDiff2SpeedDiff_temp = 0;
	else
		k_HgDiff2SpeedDiff_temp = p->CtrlPara.k_HgDiff2SpeedDiff;

	if (1)//p->CtrlPara.PfType == LC_PFTP_Titan)//titan
	{
		for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
		{
			if (p->Cmd.LiftSTM == LiftCtrl_STM_Up)
			{
				if (p->CtrlPara.BitPara.Bits.MotorSyn_Up_Acc == 0)//通过电机加速调平
				{
					speed_temp = p->CtrlPara.SpeedPlanUp[p->Cmd.do_Gears - 1] * p->CtrlPara.PumpUnifor2Speed[i]
						- (p2Avgs[i] - p2Ext->Lowest) * k_HgDiff2SpeedDiff_temp;
				}
				else
				{
					speed_temp = p->CtrlPara.SpeedPlanUp[p->Cmd.do_Gears - 1] * p->CtrlPara.PumpUnifor2Speed[i]
						+ (p2Ext->Highest - p2Avgs[i]) * k_HgDiff2SpeedDiff_temp;
				}
			}
			else if (p->Cmd.LiftSTM == LiftCtrl_STM_Down)
			{
				speed_temp = p->CtrlPara.SpeedPlanDown[p->Cmd.do_Gears - 1] * p->CtrlPara.PumpUnifor2Speed[i]
					- fabs(p2Avgs[i] - p2Ext->Highest) * k_HgDiff2SpeedDiff_temp;
			}

			if (speed_temp < 0)
				p->CtrlOut.MotorSpeed[i] = 0;
			else if(speed_temp>=30000)
				p->CtrlOut.MotorSpeed[i] = 30000;
			else
				p->CtrlOut.MotorSpeed[i] = speed_temp;

			/*压力均衡控制*/
			LiftCtrl_PressureCtrl(p);

			/*单组到达即停*/
			LiftCtrl_SingleReachStop(p);
		}
	}
	
	return 1;
}

//阀门同步控制
bool LC_ValveSynCtrl(LiftCtrl_TypeDef* p)
{
	char i = 0;
	f32* p2Avgs;
	LiftCtrl_Extreme_TypeDef* p2Ext;

	//如果三点控制，使用三点平均值
	if (p->CtrlPara.CtrlNnm == 3)
	{
		p2Avgs = p->Height.Point3Avg;
		p2Ext = &p->Height.Extreme_Titan;
	}
	else
	{
		p2Avgs = p->Height.QuadrantAvg;
		p2Ext = &p->Height.Extreme_Quadrant;
	}

	for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
	{
		if (p->Cmd.do_Up)//上升
		{
			if (p2Avgs[i] - p2Ext->Lowest > p->CtrlPara.ValveOffTgHg)//超过关闭阈值
				p->CtrlOut.ValveUp[i] = 0;
			else if (p2Avgs[i] - p2Ext->Lowest < p->CtrlPara.ValveReOnTgHg)//低于重新打开阈值
				p->CtrlOut.ValveUp[i] = 1;

			/*打开了强制升降时，不进行同步控制*/
			if (p->Cmd.do_Force)
				p->CtrlOut.ValveUp[i] = 1;
		}
		else if (p->Cmd.do_Down)//下降
		{
			if (p->CtrlPara.BitPara.Bits.ValveSyn_Down_OnlyOneOff == 0)//可能关一个以上电磁阀
			{
				if (p2Avgs[i] - p2Ext->Highest < -p->CtrlPara.ValveOffTgHg)//超过关闭阈值
					p->CtrlOut.ValveDown[i] = 0;
				else if (p2Avgs[i] - p2Ext->Highest > -p->CtrlPara.ValveReOnTgHg)//低于重新打开阈值
					p->CtrlOut.ValveDown[i] = 1;
			}
			else
			{
				//默认先打开
				p->CtrlOut.ValveDown[i] = 1;
				
				if (i == p2Ext->LowestID)
				{
					if (p2Ext->Highest - p2Ext->Lowest > p->CtrlPara.ValveOffTgHg)
						p->CtrlOut.ValveDown[i] = 0;//最低点关闭 这里没有回差，但应为液压大惯性滞后控制，所以不会明显来回跳
				}
			}
			
			/*打开了强制升降时，不进行同步控制*/
			if (p->Cmd.do_Force)
				p->CtrlOut.ValveDown[i] = 1;
		}
	}

	return 1;
}

//电机控制执行
void LiftCtrl_Exec(LiftCtrl_TypeDef* p)
{
	char i = 0;
	//上升过程
	if(p->Cmd.LiftSTM == LiftCtrl_STM_Up && p->Cmd.do_Up)
	{
		//上升过程 所有的平台类型  都需要电磁阀动
		LC_ValveSynCtrl(p);	//阀门控制
		//上升过程 所有的平台类型  都需要电机转
		if (p->Cmd.in_Ms - p->Cmd.MotorDelayStartMs > p->CtrlPara.PumpOnDelayMs)
		{	//延迟时间结束
			p->Cmd.ValveDelayStartMs = p->Cmd.in_Ms;

			LC_MotorCtrl(p);//电机控制
		}
	}
	//下降过程
	else if ((p->Cmd.LiftSTM == LiftCtrl_STM_Down && p->Cmd.do_Down))
	{
		if (p->Cmd.in_Ms - p->Cmd.MotorDelayStartMs > p->CtrlPara.PumpOnDelayMs)
		{//延迟时间结束
			p->Cmd.ValveDelayStartMs = p->Cmd.in_Ms;

			//根据是否启用电磁阀控制同步
			if (p->CtrlPara.BitPara.Bits.ValveSyn_Down == 1 && p->Cmd.do_Force == 0)
			{
				LC_ValveSynCtrl(p);	//阀门控制
			}
            else {}//其他控制方式不进行电磁阀同步控制

			//根据平台类型确定电机要不要转
			if (p->CtrlPara.PfType == LC_PFTP_Screw || p->CtrlPara.PfType == LC_PFTP_Scissors_Two//液压排油同步 不需要电机转
				  ||(p->CtrlPara.PfType == LC_PFTP_Titan && p->CtrlPara.BitPara.Bits.ValveSyn_Down == 0)//泰坦车型 使用电磁阀控制升降时不需要转
			)
			{
				LC_MotorCtrl(p);//电机控制
			}
			else
			{
				LiftCtrl_AllMotorOff(p);//电机关闭
			}

            if(p->CtrlPara.BitPara.Bits.ValveSyn_Down == 1)//下降时，同步控制下降阀门
            {
				//如果启用了自动强制下降
				if (p->CtrlPara.BitPara.Bits.ValveSyn_Down_AFD == 1)
				{
					//输入
					p->AFD.In_Ms = p->Cmd.in_Ms;
					p->AFD.In_Hg = p->Height.Avg;
					
					//子模块调用
					LiftCtrl_AutForceDown(&p->AFD);

					//输出
					if (p->AFD.Do_ForceDown)
					{
						for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
							p->CtrlOut.ValveDown[i] = 1;
					}
					else
						__asm__("nop");
				}
            }
            else
				__asm__("nop");
		}
        else
			__asm__("nop");
	}
	else//停止
	{
		LiftCtrl_AllMotorOff(p);//电机关闭

		//复位自动强制下降状态
		p->AFD.Do_ForceDown = 0;//非下降时复位该状态，突然切模式时没有复位，但是也无妨因为强制下架时间很短
		p->AFD.MsRecord_Check = p->AFD.In_Ms;

		if (p->Cmd.in_Ms - p->Cmd.ValveDelayStartMs > p->CtrlPara.ValveOffDelayMs)
		{//关阀延迟时间结束
			LiftCtrl_ValveOff(p);//阀门关闭
			p->Cmd.LiftSTM = LiftCtrl_STM_Init;
		}
        else
			__asm__("nop");

	}
}

//电机速度反向
bool LiftCtrl_MotorReverse(LiftCtrl_TypeDef* p)
{
	char i = 0;
	for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
	{
		if (p->CtrlOut.MotorSpeed[i] != 0)
			p->CtrlOut.MotorSpeed[i] = -p->CtrlOut.MotorSpeed[i];
		else
			__asm__("nop");
	}
	return 1;
}

//点动补偿控制
bool LiftCtrl_Manual_Compensation(LiftCtrl_TypeDef* p)
{
	char i = 0;

	if (p->CtrlPara.BitPara.Bits.Tst_Temp_LF_UP)
		p->CtrlOut.MotorSpeed[0] += p->CtrlPara.Tst_Temp_Rpm;

	if (p->CtrlPara.BitPara.Bits.Tst_Temp_RF_UP)
		p->CtrlOut.MotorSpeed[1] += p->CtrlPara.Tst_Temp_Rpm;

	if (p->CtrlPara.BitPara.Bits.Tst_Temp_B_UP)
		p->CtrlOut.MotorSpeed[2] += p->CtrlPara.Tst_Temp_Rpm;

	if (p->CtrlPara.BitPara.Bits.Tst_Temp_LB_UP)
		p->CtrlOut.MotorSpeed[2] += p->CtrlPara.Tst_Temp_Rpm;

	if (p->CtrlPara.BitPara.Bits.Tst_Temp_RB_UP)
		p->CtrlOut.MotorSpeed[3] += p->CtrlPara.Tst_Temp_Rpm;
		
	//限幅
	for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
	{
		if (p->CtrlOut.MotorSpeed[i] > 30000)
			p->CtrlOut.MotorSpeed[i] = 30000;
	}
	return 1;
}

//手动操作下降电磁阀
bool LiftCtrl_Manual_ValveDown(LiftCtrl_TypeDef* p)
{
	if (p->Cmd.Bits.ManualDown_LF)
		p->CtrlOut.ValveDown[0] = 1;

	if (p->Cmd.Bits.ManualDown_RF)
		p->CtrlOut.ValveDown[1] = 1;

	if (p->Cmd.Bits.ManualDown_B)
		p->CtrlOut.ValveDown[2] = 1;

	if (p->Cmd.Bits.ManualDown_LB)
		p->CtrlOut.ValveDown[2] = 1;

	if (p->Cmd.Bits.ManualDown_RB)
		p->CtrlOut.ValveDown[3] = 1;

	return 1;
}

//同步控制 仅包含titan和超低车、其他类型平台全部移到了外面
void LiftCtrl_Ctrl(LiftCtrl_TypeDef* p)
{
	//	char i=0;
	switch (p->Cmd.LiftSTM)
	{
	case LiftCtrl_STM_Init://初始化
	{
		LiftCtrl_AllValveMotorOff(p);//阀门、电机关闭
		if (p->Cmd.do_Up)//上升
		{
			LiftCtrl_AllValveUp(p);//上升阀门控制
			p->Cmd.MotorDelayStartMs = p->Cmd.in_Ms;
			//p->cmd.valve_delay_start_ms=p->cmd.in_ms;
			p->Cmd.LiftSTM = LiftCtrl_STM_Up;
		}
		else if (p->Cmd.do_Down)//下降
		{
			LiftCtrl_AllValveDown(p);//下降阀门控制
			p->Cmd.MotorDelayStartMs = p->Cmd.in_Ms;
			//p->cmd.valve_delay_start_ms=p->cmd.in_ms;
			
			//复位自动强制下降状态
			p->AFD.Do_ForceDown = 0;//非下降时复位该状态，突然切模式时没有复位，但是也无妨因为强制下架时间很短
			p->AFD.MsRecord_Check = p->AFD.In_Ms;
			
			p->Cmd.LiftSTM = LiftCtrl_STM_Down;
		}
        else {}
	}
	break;

	case LiftCtrl_STM_Up://上升
	case LiftCtrl_STM_Down://下降
	{
		LiftCtrl_Exec(p);//电机控制执行
	}
	break;

	default:
		break;

	}
	
	//点动补偿控制
	LiftCtrl_Manual_Compensation(p);

	//电机速度反向
	if(p->CtrlPara.BitPara.Bits.MotorReverse)
		LiftCtrl_MotorReverse(p);
	else
		__asm__("nop");

	//手动操作下降电磁阀
	LiftCtrl_Manual_ValveDown(p);

}

//复位所有故障标志
bool LiftCtrl_WarningReset(LiftCtrl_TypeDef* p)
{
	p->WarningOut.TooHigh = 0;
	p->WarningOut.TooLow = 0;
	p->WarningOut.DiffLimit = 0;
	p->WarningOut.DiffLimitRear = 0;
	p->WarningOut.ExecTimeout = 0;
	
	return 1;
}

//故障检出
bool LiftCtrl_Ejdg(LiftCtrl_TypeDef* p)
{
	f32 Lowest_temp = 0;
	f32 Highest_temp = 0;
	u32 Ms = p->Cmd.in_Ms;
	
	/*打开了强制升降时，清除报警并退出*/
	if (p->Cmd.do_Force)
	{
		LiftCtrl_WarningReset(p);
		return 1;
	}
	else
		__nop();

	//高度过高
	if (p->Height.Avg > p->WarningPara.HeightHigh)
	{
		if(Ms - p->MsRecord.TooHigh > p->WarningPara.EjdgDelay)
			p->WarningOut.TooHigh = 1;
		else
			__asm__("nop");
	}
	else
	{
		p->WarningOut.TooHigh = 0;
		p->MsRecord.TooHigh = Ms;
	}

	//高度过低
	if (p->Height.Avg < p->WarningPara.HeightLow && p->WarningPara.HeightLow > 0)//参数设0时关闭报警
	{
		if(Ms - p->MsRecord.TooLow > p->WarningPara.EjdgDelay)
			p->WarningOut.TooLow = 1;
		else
			__asm__("nop");
	}
	else
	{
		p->WarningOut.TooLow = 0;
		p->MsRecord.TooLow = Ms;
	}

	//高度差过大
	if (p->CtrlPara.PfType == LC_PFTP_Titan)
	{
		if (p->CtrlPara.CtrlNnm == 3)
		{
			Lowest_temp = p->Height.Extreme_Titan.Lowest;
			Highest_temp = p->Height.Extreme_Titan.Highest;
		}
		else if (p->CtrlPara.CtrlNnm == 4)
		{
			Lowest_temp = p->Height.Extreme_Quadrant.Lowest;
			Highest_temp = p->Height.Extreme_Quadrant.Highest;
		}
	}
	else
	{
		Lowest_temp = p->Height.Extreme.Lowest;
		Highest_temp = p->Height.Extreme.Highest;
	}

	if (fabs(Highest_temp - Lowest_temp) > p->WarningPara.DiffMax && p->WarningPara.DiffMax != 65535)
	{
		if (Ms - p->MsRecord.DiffLimit > p->WarningPara.EjdgDelay)
			p->WarningOut.DiffLimit = 1;
		else
			__asm__("nop");
	}
	else
	{
		p->WarningOut.DiffLimit = 0;
		p->MsRecord.DiffLimit = Ms;
	}

	//后轮高度差过大
	if (fabs(p->Height.QuadrantAvg[LiftCtrl_Quadrant_LB] - p->Height.QuadrantAvg[LiftCtrl_Quadrant_RB]) > p->WarningPara.DiffRearMax
		&& p->WarningPara.DiffRearMax != 65535)
	{
		if (Ms - p->MsRecord.DiffLimitRear > p->WarningPara.EjdgDelay)
			p->WarningOut.DiffLimitRear = 1;
		else
			__asm__("nop");
	}
	else
	{
		p->WarningOut.DiffLimitRear = 0;
		p->MsRecord.DiffLimitRear = Ms;
	}

	//执行超时
	if (p->Cmd.do_Up || p->Cmd.do_Down)
	{
		if (p->Cmd.in_Ms - p->Cmd.ExecTimeoutStartMs > p->WarningPara.TimeoutTime && p->WarningPara.TimeoutTime != 65535)
		{
			if (fabs(p->Height.Avg - p->Height.AvgBefor) < p->WarningPara.TimeoutHeight)
				p->WarningOut.ExecTimeout = 1;

			p->Cmd.ExecTimeoutStartMs = p->Cmd.in_Ms;
			p->Height.AvgBefor = p->Height.Avg;
		}
	}
	else
	{
		p->Cmd.ExecTimeoutStartMs = p->Cmd.in_Ms;
		p->Height.AvgBefor = p->Height.Avg;
	}
	
	return 1;
}

//模块单独测试时的初始化内容，实际外部使用模块时可参考编写
void LiftCtrl_InitExample(LiftCtrl_TypeDef* p)
{
	char i = 0;
	p->Height.Num = 4;
	p->Height.SetLow = 0;
	p->Height.SetHigh = 200;
	p->Height.Org[i] = 100; p->Height.SetLowOrg[i] = 0; p->Height.SetHighOrg[i] = 2000; p->Height.Quadrant[i] = LiftCtrl_Quadrant_LF; i++;
	p->Height.Org[i] = 200; p->Height.SetLowOrg[i] = 0; p->Height.SetHighOrg[i] = 2000; p->Height.Quadrant[i] = LiftCtrl_Quadrant_RF; i++;
	p->Height.Org[i] = 300; p->Height.SetLowOrg[i] = 0; p->Height.SetHighOrg[i] = 2000; p->Height.Quadrant[i] = LiftCtrl_Quadrant_LB; i++;
	p->Height.Org[i] = 400; p->Height.SetLowOrg[i] = 0; p->Height.SetHighOrg[i] = 2000; p->Height.Quadrant[i] = LiftCtrl_Quadrant_RB; i++;

	p->CtrlPara.LimitUp = 200;
	p->CtrlPara.LimitDown = 20;
	p->CtrlPara.Pump[0] = 2;
	p->CtrlPara.Pump[1] = 3;
	p->CtrlPara.Pump[2] = 4;
	p->CtrlPara.k_HgDiff2SpeedDiff = 1;
	p->CtrlPara.PumpOnDelayMs = 11000;
	p->CtrlPara.ValveOffDelayMs = 11000;

	p->WarningPara.DiffMax = 20;
	p->WarningPara.HeightLow = 30;
	p->WarningPara.HeightHigh = 150;
	p->WarningPara.DiffRearMax = 10;
	p->WarningPara.TimeoutTime = 10000;
	p->WarningPara.TimeoutHeight = 1;

}

//模块单独测试时的主函数内容，实际外部使用模块时可参考编写
void LiftCtrl_Main(LiftCtrl_TypeDef* p)
{
	//lift_hg_test_in(p);
	LiftCtrl_Cal(&p->Height);								//高度计算
	LiftCtrl_Extreme(&p->Height);							//极值分析
	LiftCtrl_CmdPrep(p);									//命令预处理
	LiftCtrl_Ctrl(p);										//控制
	LiftCtrl_Ejdg(p);										//故障检出
}

//函数功能：根据模块输出的电机转速和阀门控制信号，在时间上模拟电机运行，计算高度变化
bool LiftSimulator(LiftCtrl_TypeDef* p)
{
	char i = 0;

	if (p->CtrlPara.PfType == LC_PFTP_Titan)
	{
		/*只仿真了单独的三个 泵*/
		for (i = 0; i < p->CtrlPara.CtrlNnm; i++)
		{
			p->Simulator.Hg_Diff[i] = 0;
			if (p->Cmd.do_Up)
			{
				p->Simulator.Hg_Diff[i] = (fabsf(p->CtrlOut.MotorSpeed[i]) * p->Simulator.k_Mot2Hg) * p->CtrlOut.ValveUp[i];
				p->Simulator.Hg[i] = p->Simulator.Hg[i] + p->Simulator.Hg_Diff[i];
			}
			else if (p->Cmd.do_Down)
			{
				if (p->CtrlPara.BitPara.Bits.ValveSyn_Down == 0)
					p->Simulator.Hg_Diff[i]  = -(fabsf(p->CtrlOut.MotorSpeed[i]) * p->Simulator.k_Mot2Hg) * p->CtrlOut.ValveDown[i];
				else
					p->Simulator.Hg_Diff[i] = -(10000 * p->Simulator.k_Mot2Hg) * p->CtrlOut.ValveDown[i];//通过电磁阀同步下降时，下降速度固定
			
				p->Simulator.Hg[i] += p->Simulator.Hg_Diff[i];
			}
		}

		//第四个和第三个相同
		if (p->CtrlPara.CtrlNnm == 3)
			p->Simulator.Hg[3] = p->Simulator.Hg[2];
	}

	return 1;

}

/************************************END OF FILE************************************/

