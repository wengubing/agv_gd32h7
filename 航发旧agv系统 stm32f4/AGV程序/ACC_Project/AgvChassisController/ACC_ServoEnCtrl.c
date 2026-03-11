/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ACC_ServoEnCtrl.c
** 创 建 人: Master 
** 描    述: 
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2025年4月22日
**--------------------------------------------------------------------------------------
***************************************************************************************/  

//声明对象在此文件编译时实例化
#define  ACC_ServoEnCtrl_GLOBALS
#include "ACC_ServoEnCtrl.h"
#include "..\..\CBB\user\app_MotorResolving\app_MotorResolving.h"
#include "ACC_Init.h"

/**************************************************************************
* 函数名:ACC_ServoEnCtrl
* 功  能:具体某个伺服使能使能操作，行走升降集中一起处理
* 参  数:
* 返回值:
**************************************************************************/
void ACC_ServoEnCtrl(void)
{
	char i = 0;
	ACC_DATA_TypeDef* p2Acc = &AccData;
	MotorResolving_TypeDef* p2MR = &g_MotorResolving;
	//Mecanum_TypeDef* p2MR_Mec = &g_MecanumResolving;

	if(p2Acc->WalkServoEnLast!= p2Acc->WalkServoEn)
	{
		for (i = 0; i < p2MR->Para.MotorNumber; i++)
		{
			if (p2Acc->WalkServoEn)
				p2Acc->WalkMotor[i].En = 1;
			else
				p2Acc->WalkMotor[i].En = 0;
		}
		p2Acc->WalkServoEnLast = p2Acc->WalkServoEn;
	}

	if (p2Acc->LiftServoEnLast != p2Acc->LiftServoEn)
	{
		for (i = 0; i < g_LiftData.CtrlPara.CtrlNnm; i++)
		{
			if (p2Acc->LiftServoEn)
			{
				p2Acc->LiftMotor[i].En = 1;
			}
			else
			{
				p2Acc->LiftMotor[i].En = 0;
			}
		}
		p2Acc->LiftServoEnLast = p2Acc->LiftServoEn;
	}

	if (p2Acc->PushRollerServoEnLast != p2Acc->PushRollerServoEn)
	{
		for (i = 0; i < g_Push_Pull.PR_CtrlPara.CtrlNum; i++)
		{
			if (p2Acc->PushRollerServoEn)
			{
				p2Acc->PushRollerMotor[i].En = 1;
			}
			else
			{
				p2Acc->PushRollerMotor[i].En = 0;
			}
		}
		p2Acc->PushRollerServoEnLast = p2Acc->PushRollerServoEn;
	}

}

//***********************************END OF FILE***********************************
