/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ACC_ModeSwitch.c
** 创 建 人: Master
** 描    述:
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0
** 创建日期: 2025年4月22日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define  ACC_ModeSwitch_GLOBALS
#include "ACC_ModeSwitch.h"
#include "ACC_Init.h"

extern bool CLR_CBB(void);
extern void ACC_CloseAllValve(ACC_DATA_TypeDef *p);
extern void ACC_StopMotors(ACC_DATA_TypeDef *p);

/**************************************************************************
* 函数名:ACC_ModeSMTIsWalk
* 功  能:是行走模式判断
* 参  数:
* 返回值:bool
**************************************************************************/
bool ACC_ModeSMTIsWalk(ACC_DATA_TypeDef *p)
{
	if(   p->ModeSwitchSMT==ACCMode_PosFlexible
		||p->ModeSwitchSMT==ACCMode_PosStraight
		||p->ModeSwitchSMT==ACCMode_PosTrace
		||p->ModeSwitchSMT==ACCMode_InfFlexible
		||p->ModeSwitchSMT==ACCMode_InfStraight
		||p->ModeSwitchSMT==ACCMode_InfTrace
		||p->ModeSwitchSMT==ACCMode_Rotation
	)
		return 1;
	else
		return 0;
}
/**************************************************************************
* 函数名:ACC_ModeSwitch_StepReset
* 功  能:进入与退出模式过程状态机复位
* 参  数:NC
* 返回值:NC
**************************************************************************/
//进入与退出模式过程状态机复位
void ACC_ModeSwitch_StepReset(ACC_DATA_TypeDef *p)
{
    //进入与退出模式过程状态机复位
    p->ModeSwitch.EnterWalkStep = AccEnterWalkStep_Init;
    p->ModeSwitch.EnterLiftStep = AccEnterLiftStep_Init;
	p->ModeSwitch.EnterPushRollerStep = AccEnterPushRollerStep_Init;
    p->ModeSwitch.EnterStillStep = AccEnterStillStep_Init;
    p->ModeSwitch.ExitWalkStep = AccExitWalkStep_Init;
    p->ModeSwitch.ExitLiftStep = AccExitLiftStep_Init;
	p->ModeSwitch.ExitPushRollerStep = AccExitPushRollerStep_Init;
    p->ModeSwitch.SteeringStep = ACC_SteeringStep_Init;
    p->ModeSwitch.Exiting = 0;
    p->ModeSwitch.MsRecord = p->in_Ms;

    //清除电机速度命令
    ACC_StopMotors(p);
}

/**************************************************************************
* 函数名:ACC_ModeSwitch2Walk
* 功  能:轮子编号是先要转的那组对角 返回1，否则返回0
* 参  数:
* 返回值:
**************************************************************************/
//轮子编号是先要转的那组对角 返回1，否则返回0
char ACC_ModeSwitch_IsSle(char moed_id)
{
    MotorResolving_TypeDef *p2MR = &g_MotorResolving;

    if(p2MR->Para.Mxy[moed_id].Quadrant == MR_Quadrant_Second
            || p2MR->Para.Mxy[moed_id].Quadrant == MR_Quadrant_Fourth
      )
        return 1;
    else
        return 0;
}

/**************************************************************************
* 函数名:ACC_ModeSwitch_Steering
* 功  能:舵轮角度切换处理
* 参  数:
* 返回值:
**************************************************************************/
//舵轮角度切换处理
ACC_SteeringStep_TypeDef ACC_ModeSwitch_Steering(ACC_DATA_TypeDef *p2Acc)
{
    MotorResolving_TypeDef *p2MR = &g_MotorResolving;
    char i = 0, j = 0;
    bool DiagonalFinish = 1; //对角切换完成标志

    //补丁：防止差动速度过小
    {
        p2MR->Para.SkewSpeed = g_Para.Walk.SkewSpeed;
    }
    //建立输入管道(实时角度输入)
    for (i = 0; i < p2MR->Para.ModuleNumber; i++)
    {
        p2MR->Input.ActualAng[i] = g_WheelAngle[i].Res;
    }

    if (p2Acc->ModeSwitch.SteeringStep == ACC_SteeringStep_Init)
    {
        if(p2Acc->CalCmd.Mode == ACCMode_PosFlexible
                || p2Acc->CalCmd.Mode == ACCMode_PosStraight
                || p2Acc->CalCmd.Mode == ACCMode_PosTrace
          )
        {
            for(i = 0; i < p2MR->Para.ModuleNumber; i++)
                p2MR->TgtAng[i] = 0;
        }
        else if(p2Acc->CalCmd.Mode == ACCMode_InfFlexible
                || p2Acc->CalCmd.Mode == ACCMode_InfStraight
                || p2Acc->CalCmd.Mode == ACCMode_InfTrace
               )
        {
            for(i = 0; i < p2MR->Para.ModuleNumber; i++)
            {
                if(p2MR->Para.Mxy[i].Inf_Invflg)
                    p2MR->TgtAng[i] = ( - MR_InfeedSkewAng);
                else
                    p2MR->TgtAng[i] = MR_InfeedSkewAng;
            }
        }
        else if(p2Acc->CalCmd.Mode == ACCMode_Rotation)
        {
            p2MR->Input.WalkMode = MR_WALK_ROT;
            MotorResolving_WhellRdAndTgtAngCal_Rot(p2MR);
        }
        p2Acc->ModeSwitch.SteeringStep = ACC_SteeringStep_DiagonalLF;
        p2Acc->ModeSwitch.MsRecord_Steering = p2Acc->in_Ms;
    }
    else if (p2Acc->ModeSwitch.SteeringStep == ACC_SteeringStep_DiagonalLF)
    {
        for(i = 0; i < p2MR->Para.MotorNumber; i++)
        {
            j = i / 2; //模组
            //差动速度计算
            if (ACC_ModeSwitch_IsSle(j) || p2Acc->ModeSwitchMode == ACC_ModeSwitchMode_All)//对角
            {
                p2Acc->WalkMotor[i].V_Temp = MotorResolving_OffsetModCalc(p2MR->TgtAng[i / 2],
                                        p2MR->Input.ActualAng[i / 2], MR_ChangeIng, p2MR,i) * p2MR->Para.SkewSpeed_ChangeIng; //*p2MR->AngAdjEn;//模式切换计算差动速度时不要乘以角度调整使能
                if(p2Acc->WalkMotor[i].V_Temp != 0)
                    DiagonalFinish = 0;
            }
            else
                p2Acc->WalkMotor[i].V_Temp = 0;
        }

        //对角切换完成
        if (DiagonalFinish)
        {
            if(p2Acc->ModeSwitchMode == ACC_ModeSwitchMode_Diagonal)
                p2Acc->ModeSwitch.SteeringStep = ACC_SteeringStep_DiagonalRF;
            else if (p2Acc->ModeSwitchMode == ACC_ModeSwitchMode_All)
                p2Acc->ModeSwitch.SteeringStep = ACC_SteeringStep_Finish;
            else {}
        }
        else {}
    }
    else if (p2Acc->ModeSwitch.SteeringStep == ACC_SteeringStep_DiagonalRF)
    {
        //后面需要提炼
        for(i = 0; i < p2MR->Para.MotorNumber; i++)
        {
            j = i / 2; //模组
            //差动速度计算
            if(!ACC_ModeSwitch_IsSle(j))//对角
            {
                p2Acc->WalkMotor[i].V_Temp = MotorResolving_OffsetModCalc(p2MR->TgtAng[i / 2],
                                        p2MR->Input.ActualAng[i / 2], MR_ChangeIng, p2MR,i) * p2MR->Para.SkewSpeed_ChangeIng; //*p2MR->AngAdjEn;//模式切换计算差动速度时不要乘以角度调整使能
                if(p2Acc->WalkMotor[i].V_Temp != 0)
                    DiagonalFinish = 0;
            }
            else
                p2Acc->WalkMotor[i].V_Temp = 0;
        }

        //对角切换完成
        if(DiagonalFinish)
            p2Acc->ModeSwitch.SteeringStep = ACC_SteeringStep_Finish;
    }
    else if (p2Acc->ModeSwitch.SteeringStep == ACC_SteeringStep_Finish)
    {
        for(i = 0; i < p2MR->Para.MotorNumber; i++)
            p2Acc->WalkMotor[i].V_Temp = 0;
    }
    else {}

    //V_Temp更新到V
    for(i=0; i<p2MR->Para.MotorNumber; i++)
    {
        if(p2Acc->ModeSwitch.SteeringStep == ACC_SteeringStep_DiagonalLF || p2Acc->ModeSwitch.SteeringStep == ACC_SteeringStep_DiagonalRF)
        {//差动进行中
            //未使能差动 直接赋值
            if(g_OSSM.En == 0)
                p2Acc->WalkMotor[i].V = p2Acc->WalkMotor[i].V_Temp;
            else//平滑处理
            {
                p2Acc->WalkMotor[i].V = DataSmoothing_Main(&(g_OSSM.SM[i]),p2Acc->in_Ms,p2Acc->WalkMotor[i].V_Temp);
                g_OSSM.RunFlg = 1;
            }
        }
        else
            p2Acc->WalkMotor[i].V = 0;
    }
    return p2Acc->ModeSwitch.SteeringStep;
}

/**************************************************************************
* 函数名:ACC_ModeSwitch2Walk
* 功  能:行走模式切换处理
* 参  数:
* 返回值:
**************************************************************************/
//模式切换处理
ACC_EnterWalkStep_TypeDef ACC_ModeSwitch2Walk(ACC_DATA_TypeDef *p)
{
    char i = 0;
    //强制模式切换
    if (g_Para.Debug.ForceModeSwitch)
    {
        p->ModeSwitch.EnterWalkStep = AccEnterWalkStep_Finish;
        return p->ModeSwitch.EnterWalkStep;
    }

    if(p->ModeSwitch.EnterWalkStep == AccEnterWalkStep_Init)
        p->ModeSwitch.EnterWalkStep = AccEnterWalkStep_ServoEn;
    else if(p->ModeSwitch.EnterWalkStep == AccEnterWalkStep_ServoEn)
    {
        p->WalkServoEn = 1;
        p->ModeSwitch.MsRecord = p->in_Ms;
        p->ModeSwitch.EnterWalkStep = AccEnterWalkStep_WaitTurnOnBrake;
    }
    else if(p->ModeSwitch.EnterWalkStep == AccEnterWalkStep_WaitTurnOnBrake)
    {
        if(p->in_Ms - p->ModeSwitch.MsRecord > 1000)
            p->ModeSwitch.EnterWalkStep = AccEnterWalkStep_TurnOnBrake;
    }
    else if(p->ModeSwitch.EnterWalkStep == AccEnterWalkStep_TurnOnBrake)
    {
        p->IOCtrl.MoveBrake = 1;
        p->ModeSwitch.EnterWalkStep = AccEnterWalkStep_TurnWheel;
    }
    else if(p->ModeSwitch.EnterWalkStep == AccEnterWalkStep_TurnWheel)
    {
        //舵轮角度切换
        if(p->ChassisType == ACC_ChassisType_Ant || p->ChassisType == ACC_ChassisType_Rhino || p->ChassisType == ACC_ChassisType_Titan)
        {
            ACC_ModeSwitch_Steering(p);//舵轮角度切换

            //将电机速度送往解算模块的差速输出，以便模拟角度积分计算
            for(i = 0; i < g_MotorResolving.Para.MotorNumber; i++)
                g_MotorResolving.Output.WheelDiffSpeed[i] = p->WalkMotor[i].V;
        }
        else
            p->ModeSwitch.SteeringStep = ACC_SteeringStep_Finish; //非舵轮车直接完成


        /*升降模组高度控制
        if(1)
        {
        	//ACC_ModeSwitch_NavigationHeightCtrl(p);
        }
        else
        	*/

        if(p->ModeSwitch.SteeringStep == ACC_SteeringStep_Finish)
            //&&升降模组高度控制完成
        {
            p->ModeSwitch.MsRecord = p->in_Ms;
            p->ModeSwitch.EnterWalkStep = AccEnterWalkStep_WaitEnter;
        }
    }
    else if(p->ModeSwitch.EnterWalkStep == AccEnterWalkStep_WaitEnter)
    {
        if(p->in_Ms - p->ModeSwitch.MsRecord > 1000)
            p->ModeSwitch.EnterWalkStep = AccEnterWalkStep_Finish;
    }
    return p->ModeSwitch.EnterWalkStep;
}

/***************************************************************************************
** 函数名称:		ACC_ModeSwitch2Lift_EnterCheck
** 功能描述:		进入升降模式前的条件判断,如果不允许可以直接报警
** 参    数:	无
** 返 回 值:		0-允许进入升降模式  1-不允许
****************************************************************************************/
__weak uint8_t ACC_ModeSwitch2Lift_EnterCheck(void)
{
	return 0;
}

/**************************************************************************
* 函数名:ACC_ModeSwitch2Lift
* 功  能:升降模式切换处理
* 参  数:
* 返回值:
**************************************************************************/
//升降模式切换处理
ACC_EnterLiftStep_TypeDef ACC_ModeSwitch2Lift(ACC_DATA_TypeDef *p)
{
    //强制模式切换
    if (g_Para.Debug.ForceModeSwitch)
    {
        p->ModeSwitch.EnterLiftStep = AccEnterLiftStep_Finish;
        return p->ModeSwitch.EnterLiftStep;
    }

    if(p->ModeSwitch.EnterLiftStep == AccEnterLiftStep_Init)
	{
		p->ModeSwitch.EnterLiftStep = AccEnterLiftStep_EnterCheck;
	}
	//检测是否能进入升降模式
	else if(p->ModeSwitch.EnterLiftStep == AccEnterLiftStep_EnterCheck)
    {
		uint8_t Res = ACC_ModeSwitch2Lift_EnterCheck();
        if(Res == 0)
		{
			p->ModeSwitch.EnterLiftStep = AccEnterLiftStep_ServoEn;
		}
    }
    else if(p->ModeSwitch.EnterLiftStep == AccEnterLiftStep_ServoEn)
    {
        p->LiftServoEn = 1;
		p->PushRollerServoEn = 0;
        p->ModeSwitch.MsRecord = p->in_Ms;
        p->ModeSwitch.EnterLiftStep = AccEnterLiftStep_WaitEnter;
    }
    else if(p->ModeSwitch.EnterLiftStep == AccEnterLiftStep_WaitEnter)
    {
        if(p->in_Ms - p->ModeSwitch.MsRecord > 1000)
		{
            p->ModeSwitch.EnterLiftStep = AccEnterLiftStep_Finish;
		}
    }

    return p->ModeSwitch.EnterLiftStep;
}

/**************************************************************************
* 函数名:ACC_ModeSwitch2PushRoller
* 功  能:推送模式切换处理
* 参  数:
* 返回值:
**************************************************************************/
//推送模式切换处理
ACC_EnterPushRollerStep_TypeDef ACC_ModeSwitch2PushRoller(ACC_DATA_TypeDef *p)
{
    //强制模式切换
    if (g_Para.Debug.ForceModeSwitch)
    {
        p->ModeSwitch.EnterPushRollerStep = AccEnterPushRollerStep_Finish;
        return p->ModeSwitch.EnterPushRollerStep;
    }

    if(p->ModeSwitch.EnterPushRollerStep == AccEnterPushRollerStep_Init)
        p->ModeSwitch.EnterPushRollerStep = AccEnterPushRollerStep_ServoEn;
    else if(p->ModeSwitch.EnterPushRollerStep == AccEnterPushRollerStep_ServoEn)
    {
        p->PushRollerServoEn = 1;
		p->LiftServoEn = 0;
        p->ModeSwitch.MsRecord = p->in_Ms;
        p->ModeSwitch.EnterPushRollerStep = AccEnterPushRollerStep_WaitEnter;
    }
    else if(p->ModeSwitch.EnterPushRollerStep == AccEnterPushRollerStep_WaitEnter)
    {
        if(p->in_Ms - p->ModeSwitch.MsRecord > 1000)
		{
			p->IOCtrl.PushRollerBrake = 1;
            p->ModeSwitch.EnterPushRollerStep = AccEnterPushRollerStep_Finish;
		}
		
    }

    return p->ModeSwitch.EnterPushRollerStep;
}

/***********************************************************
** 函数名称:	ACC_ModeSwitch2Still
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//切换到静止模式
ACC_EnterStillStep_TypeDef ACC_ModeSwitch2Still(ACC_DATA_TypeDef *p)
{
    //强制模式切换
    if (g_Para.Debug.ForceModeSwitch)
    {
        p->ModeSwitch.EnterStillStep = AccEnterStillStep_Finish;
        return p->ModeSwitch.EnterStillStep;
    }

    if(p->ModeSwitch.EnterStillStep == AccEnterStillStep_Init)
        p->ModeSwitch.EnterStillStep = AccEnterStillStep_NC;
    else if(p->ModeSwitch.EnterStillStep == AccEnterStillStep_NC)
    {
        p->ModeSwitch.EnterStillStep = AccEnterStillStep_WaitEnter;
        p->ModeSwitch.MsRecord = p->in_Ms;
    }
    else if(p->ModeSwitch.EnterStillStep == AccEnterStillStep_WaitEnter)
    {
        if(p->in_Ms - p->ModeSwitch.MsRecord > 1000)
            p->ModeSwitch.EnterStillStep = AccEnterStillStep_Finish;
    }

    return p->ModeSwitch.EnterStillStep;
}

/**************************************************************************
* 函数名:ACC_ModeSwitch_EnterModeCtrl
* 功  能:进入模式过程控制
* 参  数:NC
* 返回值:NC
**************************************************************************/
//进入模式过程控制
void ACC_ModeSwitch_EnterModeCtrl(ACC_DATA_TypeDef *p)
{
    if(    p->CalCmd.Mode == ACCMode_PosStraight
            || p->CalCmd.Mode == ACCMode_PosFlexible
            || p->CalCmd.Mode == ACCMode_PosTrace
            || p->CalCmd.Mode == ACCMode_InfStraight
            || p->CalCmd.Mode == ACCMode_InfFlexible
            || p->CalCmd.Mode == ACCMode_InfTrace
            || p->CalCmd.Mode == ACCMode_Rotation)
    {
        if(ACC_ModeSwitch2Walk(p) == AccEnterWalkStep_Finish)
            p->ModeSwitch.Finished = 1;
    }
    else if(p->CalCmd.Mode == ACCMode_Lift)
    {
        if(ACC_ModeSwitch2Lift(p) == AccEnterLiftStep_Finish)
            p->ModeSwitch.Finished = 1;
    }
    else if(p->CalCmd.Mode == ACCMode_Absorption)
    {
        if(ACC_ModeSwitch2Lift(p) == AccEnterLiftStep_Finish)
            p->ModeSwitch.Finished = 1;
        //p->ModeSwitchSMT=p->CalCmd.Mode;
    }
    else if(p->CalCmd.Mode == ACCMode_Push)
    {
        if(ACC_ModeSwitch2PushRoller(p) == AccEnterLiftStep_Finish)
            p->ModeSwitch.Finished = 1;;
    }
    else if(p->CalCmd.Mode == ACCMode_Arm)
    {
        //p->ModeSwitchSMT=p->CalCmd.Mode;
    }
    else if(p->CalCmd.Mode == ACCMode_Clip)
    {
        //p->ModeSwitchSMT=p->CalCmd.Mode;
    }
    else if(p->CalCmd.Mode == ACCMode_Leg)
    {
        if(ACC_ModeSwitch2Lift(p) == AccEnterLiftStep_Finish)
            p->ModeSwitch.Finished = 1;
    }
    else if(p->CalCmd.Mode == ACCMode_Roller)
    {
        if(ACC_ModeSwitch2PushRoller(p) == AccEnterLiftStep_Finish)
            p->ModeSwitch.Finished = 1;
    }
    else if(p->CalCmd.Mode == ACCMode_Still)
    {
        if(ACC_ModeSwitch2Still(p) == AccEnterStillStep_Finish) //不是解决切静止不延时长鸣提醒问题
            p->ModeSwitch.Finished = 1;
    }

    //模式切换完成后，等待2s
    if(p->ModeSwitch.Finished)
    {
        if(p->DelayMs.ModeChange > 400)						//切换内部已有延时，这里再统一延迟少一点
        {
            p->ModeSwitchSMT = p->CalCmd.Mode;
            //printf("模式切换完成\n");

            //补丁代码
            //偶发在磁条离线报警触发后切到自由模式，没有打开抱闸
            //强行打开抱闸，并计数
            if(p->IOCtrl.MoveBrake == 0 && ACC_ModeSMTIsWalk(p))
            {
                p->IOCtrl.MoveBrake = 1;
                p->MoveBrake_Patch_Cnt ++;
            }
            else {}

            p->ModeSwitch.Finished = 0;						//清除模式切换完成标志

            //清除超时计时
            g_LiftData.Cmd.ExecTimeoutStartMs = g_Sys_1ms_Counter;

            //清除超时计时
            g_AbsorptionData.Cmd.ExecTimeoutStartMs = g_Sys_1ms_Counter;
        }
    }
    else
        p->DelayMs.ModeChange = 0;

    //模式切换完成后，置位模式切换提醒标志
    if(p->ModeSwitchSMT != ACCMode_Null)
    {
        g_SLN.in_FlgModeSwitchOk = 1;
        g_VoicePlayer.BitInput.ModeChangeOk = 1;

        //如果目标模式是静止模式
        if(p->CalCmd.Mode == ACCMode_Still)
        {
            g_VoicePlayer.BitInput.Still = 1;
        }
    }
    else {}
}
/**************************************************************************
* 函数名:ACC_WalkReduced
* 功  能:判断所有行走电机速度是否减小的指定范围
* 参  数:
* 返回值:
**************************************************************************/
//判断所有行走电机速度是否减小的指定范围
bool ACC_WalkReduced()
{
    bool WalkReduced = 1;
    //char i=0;
    ACC_DATA_TypeDef *p2Acc = &AccData;
    //MotorResolving_TypeDef* p2MR = &g_MotorResolving;

    /*通过电机速度判断行走是否停止
    for(i=0; i<p2MR->Para.MotorNumber; i++)
    {
    	if(fabs(p2Acc->WalkMotor[i].VActual)>20)
    		WalkReduced=0;
    }*/

    //通过控制命令判断行走是否停止
    if(p2Acc->CalCmd.VOxyz.x != 0 || p2Acc->CalCmd.VOxyz.y != 0 || p2Acc->CalCmd.VOxyz.z != 0)
        WalkReduced = 0;

    return WalkReduced;
}



/**************************************************************************
* 函数名:ACC_ModeWalkExit
* 功  能:退出行走模式
* 参  数:
* 返回值:
**************************************************************************/
//退出行走模式
ACC_ExitWalkStep_TypeDef ACC_ModeWalkExit(ACC_DATA_TypeDef *p)
{
    //退出行走模式时，持续清除控制命令
    p->CalCmd.Vxyz.x = 0;
    p->CalCmd.Vxyz.y = 0;
    p->CalCmd.Vxyz.z = 0;

    if (p->ModeSwitch.ExitWalkStep == AccExitWalkStep_Init)
        p->ModeSwitch.ExitWalkStep = AccExitWalkStep_ClearCmd;
    else if (p->ModeSwitch.ExitWalkStep == AccExitWalkStep_ClearCmd)
    {
        p->ModeSwitch.MsRecord = p->in_Ms;
        p->ModeSwitch.ExitWalkStep = AccExitWalkStep_WaitMotorStop;
    }
    else if (p->ModeSwitch.ExitWalkStep == AccExitWalkStep_WaitMotorStop)
    {
        if (p->in_Ms - p->ModeSwitch.MsRecord > 5000 || ACC_WalkReduced())//等待超时或车停下来
        {
            p->ModeSwitch.MsRecord = p->in_Ms;
            p->ModeSwitch.ExitWalkStep = AccExitWalkStep_WaitOffBreke;
        }
    }
    else if (p->ModeSwitch.ExitWalkStep == AccExitWalkStep_WaitOffBreke)
    {
        if (p->in_Ms - p->ModeSwitch.MsRecord > 1000)
        {
            p->ModeSwitch.MsRecord = p->in_Ms;
            p->ModeSwitch.ExitWalkStep = AccExitWalkStep_TurnOffBrake;
        }
    }
    else if (p->ModeSwitch.ExitWalkStep == AccExitWalkStep_TurnOffBrake)
    {
        p->IOCtrl.MoveBrake = 0;
        p->ModeSwitch.ExitWalkStep = AccExitWalkStep_ServoOff;
    }
    else if (p->ModeSwitch.ExitWalkStep == AccExitWalkStep_ServoOff)
    {
        p->WalkServoEn = 0;
        p->ModeSwitch.ExitWalkStep = AccExitWalkStep_Wait;
    }
    else if (p->ModeSwitch.ExitWalkStep == AccExitWalkStep_Wait)
    {
        p->ModeSwitch.ExitWalkStep = AccExitWalkStep_Finish;
    }
    return p->ModeSwitch.ExitWalkStep;
}

/***********************************************************
** 函数名称:	ACC_ModeLiftExit
** 功能描述: 退出举升模式
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//退出举升模式
ACC_ExitLiftStep_TypeDef ACC_ModeLiftExit(ACC_DATA_TypeDef *p2Acc)
{
    if (p2Acc->ModeSwitch.ExitLiftStep == AccExitLiftStep_Init)
        p2Acc->ModeSwitch.ExitLiftStep = AccExitLiftStep_ServoStop;
    else if (p2Acc->ModeSwitch.ExitLiftStep == AccExitLiftStep_ServoStop)
    {
        p2Acc->LiftServoEn = 0;
        p2Acc->ModeSwitch.MsRecord = p2Acc->in_Ms;
        p2Acc->ModeSwitch.ExitLiftStep = AccExitLiftStep_WaitOffValve;
    }
    else if (p2Acc->ModeSwitch.ExitLiftStep == AccExitLiftStep_WaitOffValve)
    {
        /*主动关闭电磁阀
        if (p2Acc->in_Ms - p2Acc->ModeSwitch.MsRecord > 1000)
        {
        	//电磁阀控制输出
        	p2Acc->IOCtrl.ValveUpLF = 0;
        	p2Acc->IOCtrl.ValveUpRF = 0;
        	p2Acc->IOCtrl.ValveUpB = 0;
        	p2Acc->IOCtrl.ValveDownLF = 0;
        	p2Acc->IOCtrl.ValveDownRF = 0;
        	p2Acc->IOCtrl.ValveDownB = 0;
        	p2Acc->ModeSwitch.ExitLiftStep = AccExitLiftStep_Finish;
        }*/

        /*等待判断解算结果 电磁阀关闭*/
        if (g_LiftData.Cmd.LiftSTM == LiftCtrl_STM_Init)
        {
            p2Acc->ModeSwitch.ExitLiftStep = AccExitLiftStep_Finish;
        }
    }

    return p2Acc->ModeSwitch.ExitLiftStep;
}
/***********************************************************
** 函数名称:	ACC_ModePushRollerExit
** 功能描述: 退出推送模式
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//退出推送模式
ACC_ExitPushRollerStep_TypeDef ACC_ModePushRollerExit(ACC_DATA_TypeDef *p2Acc)
{
    if (p2Acc->ModeSwitch.ExitPushRollerStep == AccExitPushRollerStep_Init)
	{
        p2Acc->ModeSwitch.ExitPushRollerStep = AccExitPushRollerStep_ServoStop;
	}
    else if (p2Acc->ModeSwitch.ExitPushRollerStep == AccExitPushRollerStep_ServoStop)
    {
		//伺服失能
        p2Acc->PushRollerServoEn = 0;
        p2Acc->ModeSwitch.MsRecord = p2Acc->in_Ms;
        p2Acc->ModeSwitch.ExitPushRollerStep = AccExitPushRollerStep_TurnOffBrake;
    }
	else if (p2Acc->ModeSwitch.ExitPushRollerStep == AccExitPushRollerStep_TurnOffBrake)
    {
		//等待1秒关闭抱闸
		if(p2Acc->in_Ms - p2Acc->ModeSwitch.MsRecord > 1000)
		{
			p2Acc->IOCtrl.PushRollerBrake = 0;
			p2Acc->ModeSwitch.MsRecord = p2Acc->in_Ms;
			p2Acc->ModeSwitch.ExitPushRollerStep = AccExitPushRollerStep_WaitOffValve;
		}
    }
    else if (p2Acc->ModeSwitch.ExitPushRollerStep == AccExitPushRollerStep_WaitOffValve)
    {
        {
            p2Acc->ModeSwitch.ExitLiftStep = AccExitLiftStep_Finish;
			
        }
    }

    return p2Acc->ModeSwitch.ExitPushRollerStep;
}


/**************************************************************************
* 函数名:ACC_ModeSwitch_ModeSTMExit_JudgeAndCtrl
* 功  能:模式状态机退出调用
* 参  数:NC
* 返回值:NC
**************************************************************************/
//模式状态机退出调用
void ACC_ModeSwitch_ModeSTMExit_JudgeAndCtrl(ACC_DATA_TypeDef *p)
{
    //退出过程一旦开始不可终止
    if(p->ModeSwitchSMT != p->CalCmd.Mode)
    {
        p->ModeSwitch.Exiting = 1;
        p->CalCmd.ModeLast = ACCMode_Null;//一旦退出，必须经过模式改变过程
    }
    else {}

    if(p->ModeSwitch.Exiting)
    {
        if (ACC_ModeSMTIsWalk(p))
        {
            if(ACC_ModeWalkExit(p) == AccExitWalkStep_Finish)
                p->ModeSwitchSMT = ACCMode_Null;
        }
        else if(p->ModeSwitchSMT == ACCMode_Lift)
        {
            if (ACC_ModeLiftExit(p) == AccExitLiftStep_Finish)
                p->ModeSwitchSMT = ACCMode_Null;
        }
        else if(p->ModeSwitchSMT == ACCMode_Absorption)
        {
            if (ACC_ModeLiftExit(p) == AccExitLiftStep_Finish)
                p->ModeSwitchSMT = ACCMode_Null;
        }

        else if(p->ModeSwitchSMT == ACCMode_Leg)
        {
            if (ACC_ModeLiftExit(p) == AccExitLiftStep_Finish)
                p->ModeSwitchSMT = ACCMode_Null;
        }
        else if(p->ModeSwitchSMT == ACCMode_Push)
        {
            if (ACC_ModePushRollerExit(p) == AccExitPushRollerStep_Finish)
                p->ModeSwitchSMT = ACCMode_Null;
        }
        else if(p->ModeSwitchSMT == ACCMode_Roller)
        {
            if (ACC_ModePushRollerExit(p) == AccExitPushRollerStep_Finish)
                p->ModeSwitchSMT = ACCMode_Null;
        }
        else if(p->ModeSwitchSMT == ACCMode_Still)
        {
            //充电极板未打开时，退出静止模式
            if(p->IOCtrl.ChargingMC == 0)
                p->ModeSwitchSMT = ACCMode_Null;		//静止模式直接退出
        }

    }
    else {}
    /*else//复位各模式退出状态机
    {
    	p->ModeSwitch.ExitWalkStep = AccExitWalkStep_Init;
    	p->ModeSwitch.ExitLiftStep=AccExitLiftStep_Init;
    }*/
}

/**************************************************************************
* 函数名:ACC_CommandSmoothing
* 功  能:控制命令平滑处理
* 参  数:NC
* 返回值:NC
**************************************************************************/
//控制命令平滑处理
void ACC_CommandSmoothing(ACC_DATA_TypeDef *p)
{
    //Vxyz独立平滑
    if(1)
    {
        p->CalCmd.VOxyz.x = DataSmoothing_Main(&g_SmoothVx, p->in_Ms, p->CalCmd.Vxyz.x);
        p->CalCmd.VOxyz.y = DataSmoothing_Main(&g_SmoothVy, p->in_Ms, p->CalCmd.Vxyz.y);
        p->CalCmd.VOxyz.z = DataSmoothing_Main(&g_SmoothVz, p->in_Ms, p->CalCmd.Vxyz.z);
    }
    else//关联平滑
    {
        g_SmoothVxyz[0].in_RealTimeValue = p->CalCmd.Vxyz.x;
        g_SmoothVxyz[1].in_RealTimeValue = p->CalCmd.Vxyz.y;
        g_SmoothVxyz[2].in_RealTimeValue = p->CalCmd.Vxyz.z;

        DataSmoothing_Unite(g_SmoothVxyz, 3, p->in_Ms, &g_SmoothVxyz[0]);

        p->CalCmd.VOxyz.x = g_SmoothVxyz[0].out_Value;
        p->CalCmd.VOxyz.y = g_SmoothVxyz[1].out_Value;
        p->CalCmd.VOxyz.z = g_SmoothVxyz[2].out_Value;
    }
	
	//平滑调用标志置位 Vxyz
	p->SM_RunFlg = 1;

    //Axyz独立平滑 在前向速度为零后停止 正好实现再次推摇杆时继续从之前的位置开始平滑
    if (
        (ACC_IsPosWalkMode(p->OrgCmd.Mode) && p->CalCmd.VOxyz.y != 0)
        || (ACC_IsInfWalkMode(p->OrgCmd.Mode) && p->CalCmd.VOxyz.x != 0)
        || p->OrgCmd.Mode == ACCMode_Rotation
		|| g_Para.Walk.Bits.AxyzSmoothStopWithVy == 0
    )
    {
        p->CalCmd.AOxyz.x = DataSmoothing_Main(&g_SmoothAx, p->in_Ms, p->CalCmd.Axyz.x);
        p->CalCmd.AOxyz.y = DataSmoothing_Main(&g_SmoothAy, p->in_Ms, p->CalCmd.Axyz.y);
        p->CalCmd.AOxyz.z = DataSmoothing_Main(&g_SmoothAz, p->in_Ms, p->CalCmd.Axyz.z);
		
		p->SM_RunFlg_Axyz = 1;
    }
}

/**************************************************************************
* 函数名:ACC_ModeSwitch_Process
* 功  能:模式切换处理
* 参  数:NC
* 返回值:NC
**************************************************************************/
//模式切换处理
void ACC_ModeSwitch_Process(ACC_DATA_TypeDef *p)
{
    switch (p->ModeSwitchSMT)
    {
    case ACCMode_Null:						//初始化：过程中 模式发生变化，行走模式间切换不会关闭抱闸和使能
    {
        if(p->CalCmd.Mode != p->CalCmd.ModeLast) //模式发生变化
        {
            //解决模式切换中切入非行走模式再切回行走模式 跳过了模式切换问题
            p->ModeSwitch.Finished = 0;

            //进入与退出模式过程状态机复位 且 清除电机速度命令
            ACC_ModeSwitch_StepReset(p);

            //清除cbb输入、状态
            CLR_CBB();

            //如目标模式为升降模式，则直接关闭行走电机抱闸和使能
            if(p->CalCmd.Mode == ACCMode_Lift || p->CalCmd.Mode == ACCMode_Absorption)
            {
                p->WalkServoEn = 0;
                p->IOCtrl.MoveBrake = 0;
            }
            else if(ACC_IsWalkMode(p->CalCmd.Mode))//如果为行走模式，则关闭升降电机和电磁阀
            {
				/*举升平台静默处理*/
                p->LiftServoEn = 0;
                p->IOCtrl.KoRunBrake = 0;
                p->IOCtrl.KoRunSysPower = 0;
                p->IOCtrl.KoRunContactor = 0;

				/*推送机构静默处理*/
				p->PushRollerServoEn = 0;
				p->IOCtrl.PushRollerBrake = 0;
                ACC_CloseAllValve(p);
            }
            else if(p->CalCmd.Mode == ACCMode_Still)//如果为静止模式，关闭所有电机使能、抱闸、电磁阀
            {
                p->WalkServoEn = 0;
                p->LiftServoEn = 0;
                p->IOCtrl.MoveBrake = 0;
				

                p->IOCtrl.KoRunBrake = 0;
                p->IOCtrl.KoRunSysPower = 0;
                p->IOCtrl.KoRunContactor = 0;
                ACC_CloseAllValve(p);
				
				/*推送机构静默处理*/
				p->PushRollerServoEn = 0;
				p->IOCtrl.PushRollerBrake = 0;
            }

            p->CalCmd.ModeLast = p->CalCmd.Mode;
        }

        //模式切换进入控制
        ACC_ModeSwitch_EnterModeCtrl(p);
    }
    break;

    case ACCMode_PosStraight:
    case ACCMode_PosFlexible:
    case ACCMode_PosTrace:
    case ACCMode_InfStraight:
    case ACCMode_InfFlexible:
    case ACCMode_InfTrace:
    case ACCMode_Rotation:					//行走
    {
        ACC_ModeSwitch_ModeSTMExit_JudgeAndCtrl(p);

        //平滑处理，放在此处，以便行走退出状态机对控制命令的清除能够生效
        ACC_CommandSmoothing(p);

        if(p->ModeSwitch.ExitWalkStep < AccExitWalkStep_TurnOffBrake) //关闭抱闸前正常解算
        {
                if(p->ExtFunc.p2WalkFunc)
                    p->ExtFunc.p2WalkFunc();
        }
    }
    break;

    case ACCMode_Lift:						//升降
    {
        ACC_ModeSwitch_ModeSTMExit_JudgeAndCtrl(p);

        if(p->ExtFunc.p2LiftFunc)
            p->ExtFunc.p2LiftFunc();
    }
    break;

    case ACCMode_Leg:						//支腿控制
    {
        ACC_ModeSwitch_ModeSTMExit_JudgeAndCtrl(p);

        if(p->ExtFunc.p2LegFunc)
            p->ExtFunc.p2LegFunc();
    }
    break;
    case ACCMode_Absorption:				//悬挂控制
    {
        ACC_ModeSwitch_ModeSTMExit_JudgeAndCtrl(p);

        if(p->ExtFunc.p2AbsorptionFunc)
            p->ExtFunc.p2AbsorptionFunc();
    }
    break;

    case ACCMode_Push:						//推拉
    {
        ACC_ModeSwitch_ModeSTMExit_JudgeAndCtrl(p);

        if(p->ExtFunc.p2PushPull_Func)
            p->ExtFunc.p2PushPull_Func();
    }
    break;

    case ACCMode_Roller:					//滚筒
    {
        ACC_ModeSwitch_ModeSTMExit_JudgeAndCtrl(p);

        if(p->ExtFunc.p2Roller_Func)
            p->ExtFunc.p2Roller_Func();
    }
    break;

    case ACCMode_Still:						//静止
    {
        ACC_ModeSwitch_ModeSTMExit_JudgeAndCtrl(p);
    }
    break;

    default:
        break;
    }

}


//***********************************END OF FILE***********************************
