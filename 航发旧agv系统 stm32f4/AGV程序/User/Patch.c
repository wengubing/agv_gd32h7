/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Patch.c
** 创 建 人: 文小兵
** 描    述: 外部方法补丁
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0
** 创建日期: 2024年9月20日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define Patch_GLOBALS

//自定义引用		
#include "Patch.h"
#include "ACC_Init.h"
#include "ACC_Project.h"

//外部对象声明

//内部方法声明

//行走限制
bool WalkLimit_Run(WalkLimit_TypeDef* p)
{
    bool res = 0;
    //未限制前的处理
    p->Output_Acc = p->AccMax;
    p->State_Limit.Acc = 0;

    //在升降模式退出时更新重量的状态机,
    /*或者在外部升降模式退出时 更新重量输入 和 调用更新加速度对象*/
    switch (p->LiftModeSTM)
    {
        //初始化
    case LiftMode_Init:
    {
        p->LiftModeSTM = LiftMode_WaitEnter;
        //p->Weight = 0;                                  //重量清零
    }
    break;

    //等待升降模式进入
    case LiftMode_WaitEnter:
    {
        if (p->Input_Mode == ACCMode_Lift)
        {
            p->LiftModeSTM = LiftMode_WaitQuit;
        }
        else {}
    }
    break;

    //等待升降模式退出
    case LiftMode_WaitQuit:
    {
        if (p->Input_Mode != ACCMode_Lift)
        {
            p->Weight = p->Input_Weight;                //更新重量
            p->LiftModeSTM = LiftMode_Init;             //回到初始化
        }
        else {}
    }
    break;

    default:
        break;
    }

    //根据挡位、重量限制
    if (p->ParaBit.Rely_WalkGearAndWeight)
    {
        if (p->Input_WalkGear >= p->Para_WalkGear && p->Weight >= p->Para_Weight)
        {
            p->Output_Acc = p->AccMin;
            p->State_Limit.Acc = 1;
        }
        else {}
    }
    else {}

    return res;
}

//档位限制
bool GearLimit_Single_Run(GearLimit_Single_TypeDef* p)
{
    bool res = 0;
    //未限制前的处理
    p->Output_Gear = p->Input_Gear;
    p->Output_State.Limited = 0;

    //实时更新输入
    p->Rely = p->Input_Rely;

    //处理触发参数限制状态机
    if (p->Rely > p->Para_RelyMax)
    {
        p->Output_State.Limited_Rely = 1;
    }
    else if (p->Rely < p->Para_RelyMin)
    {
        p->Output_State.Limited_Rely = 0;
    }
    else {}

    //根据回差限制
    if (p->Output_State.Limited_Rely == 1)
    {
        if (p->Input_Gear > p->Para_LimitGear)
        {
            p->Output_Gear = p->Para_LimitGear;
            p->Output_State.Limited = 1;
        }
        else
        {
            p->Output_Gear = p->Input_Gear;
            p->Output_State.Limited = 0;
        }
    }
    else
    {
        p->Output_Gear = p->Input_Gear;
        p->Output_State.Limited = 0;
    }

    return res;
}

//舵轮车手动操作控制单个模组对象
bool MR_ManualCtrl_Run(MR_ManualCtrl_TypeDef* p2MRMC)
{
    char i = 0;
    ACC_DATA_TypeDef* p2Acc = &AccData;
    MotorResolving_TypeDef* p2MR = &g_MotorResolving;
    char ID_MotorL = (p2MRMC->Select - 1) * 2;              //左电机ID
    char ID_MotorR = (p2MRMC->Select - 1) * 2 + 1;          //右电机ID

    p2MRMC->Speed = p2MRMC->SpeedMax
        * ((p2Acc->OrgCmd.Vxyz.z) / (ACC_WALKGEARMAX * p2Acc->JoystickMax));//无平滑，使用org，因为静止模式的时候cmd里的vxyz被清零

    for (i = 0; i < p2MR->Para.MotorNumber; i++)
    {
        if (i == ID_MotorL)
            p2Acc->WalkMotor[i].V = p2MRMC->Speed;
        else if (i == ID_MotorR)
            p2Acc->WalkMotor[i].V = p2MRMC->Speed;
        else
            p2Acc->WalkMotor[i].V = 0;
    }

    return 1;
}

//载入单轴运行参数
bool Patch_AngAndR_AngOrR(void)
{
    MotorResolving_TypeDef* p2MR = &g_MotorResolving;
    Patch_AngAndR_CmdSel_TypeDef* p2PAA = &g_Patch_AngAndR_CmdSel;

    p2MR->Para.MaxSliAngPos = p2PAA->MaxSliAngPos;
    p2MR->Para.MinRotDisPos = p2PAA->MinRotDisPos;

    return 1;
}

//载入复合运行参数
bool Patch_AngAndR_AngAndR(void)
{
    MotorResolving_TypeDef* p2MR = &g_MotorResolving;
    //Patch_AngAndR_CmdSel_TypeDef* p2PAA = &g_Patch_AngAndR_CmdSel;

    /*p2MR->Para.MaxSliAngPos = p2PAA->MaxSliAngPos_Free;
    p2MR->Para.MinRotDisPos = p2PAA->MinRotDisPos_Free;*/
	
	/*用原系统参数中的正常复合运动映射参数初始化cbb*/
	p2MR->Para.MaxSliAngPos = g_Para.Walk.MaxSliAngPos;
    p2MR->Para.MinRotDisPos = g_Para.Walk.MinRotDisPos;

    return 1;
}

//斜行和转弯命令二选一控制
bool Patch_AngAndR_CmdSel(void)
{
    Patch_AngAndR_CmdSel_TypeDef* p2PAA = &g_Patch_AngAndR_CmdSel;
    //ACC_DATA_TypeDef *p2Acc = &AccData;
    OrgCtrlCmd_TypeDef* p2CCC = &g_ChannelSwitch.CtrlCmd;
    bool AngOrR_En_Temp = 0;//临时变量

    //面板选择联动时 退出
    if (p2CCC->HandleUnionVal == AsbModel_Asb)
        return 1;

    //通过参数+辅助按钮是否按下确定是否进入单轴放大
    if (p2CCC->Button.LiftUp == 1 && p2PAA->Prees_AngOrR)
        AngOrR_En_Temp = 1;
    if (p2CCC->Button.LiftUp == 0 && p2PAA->Prees_AngOrR == 0)
        AngOrR_En_Temp = 1;

    if (
        (p2PAA->Enable                                      //打开了二选一
            && (p2CCC->Mode == ACCMode_PosFlexible || p2CCC->Mode == ACCMode_InfFlexible)//自由遥控模式
			//&& p2CCC->Button.LiftUp == 0                  //不按单轴放大
			//&& p2CCC->Button.LiftUp == 1					//按下单轴放大
            && AngOrR_En_Temp == 1                          //自动对中按下 还是复合运动
            && p2CCC->Button.ShelfAutadj == 0               //自动对中按下 还是复合运动
            //&& p2CCC->HandleUnionVal != AsbModel_Asb        //没有选择联动(联动状态下不切换单轴放大)
            //在上面已经判断是否联动 这里处理会造成问题
         )
        || p2PAA->ForceEnable == 1                          //强制使能
    )
        p2PAA->AngAndR_Free = 0;
    else
        p2PAA->AngAndR_Free = 1;

    //如果使能功能切换了
    if (
        p2PAA->AngAndR_Free != p2PAA->AngAndR_Free_Last
        && p2CCC->Vxyz.x == 0
        && p2CCC->Vxyz.y == 0
        && p2CCC->Vxyz.z == 0
        )
    {
        p2PAA->STM = AngAndRSel_Init;                       //状态机复位
        p2PAA->AngAndR_Free_Last = p2PAA->AngAndR_Free;     //更新last
        
        //清除Axyz平滑输出值，避免单轴放大解算过程中出现解算超限
        g_SmoothAx.out_Value = 0;
        g_SmoothAy.out_Value = 0;
        g_SmoothAz.out_Value = 0;
    }
    else {}

    //确定斜行轴下标
    if (p2CCC->Mode == ACCMode_PosFlexible)
    {
        p2PAA->AngAxis = 0;
        p2PAA->AngCmd = p2CCC->Vxyz.x;
    }
    else if (p2CCC->Mode == ACCMode_InfFlexible)
    {
        p2PAA->AngAxis = 1;
        p2PAA->AngCmd = p2CCC->Vxyz.y;
    }
    else
    {
        p2PAA->AngCmd = 0;
    }

    switch (p2PAA->STM)
    {
    case AngAndRSel_Init:
    {
        if (p2PAA->AngAndR_Free == 0)
        {
            if (p2PAA->AngCmd != 0)
                p2PAA->STM = AngAndRSel_Ang;
            else if (p2CCC->Vxyz.z != 0)
                p2PAA->STM = AngAndRSel_R;
            else {}

            //载入单轴运行参数
            Patch_AngAndR_AngOrR();
        }
        else
        {
            p2PAA->STM = AngAndRSel_AngR;
            //载入复合运行参数
            Patch_AngAndR_AngAndR();
        }
    }
    break;

    case AngAndRSel_Ang:
    {
        if (p2PAA->AngCmd == 0)
        {
            p2PAA->STM = AngAndRSel_Init;
        }
        else
        {
            p2CCC->Vxyz.z = 0;
        }
    }
    break;

    case AngAndRSel_R:
    {
        if (p2CCC->Vxyz.z == 0)
        {
            p2PAA->STM = AngAndRSel_Init;
        }
        else
        {
            if (p2CCC->Mode == ACCMode_PosFlexible)
                p2CCC->Vxyz.x = 0;
            else if (p2CCC->Mode == ACCMode_InfFlexible)
                p2CCC->Vxyz.y = 0;
            else {}
        }
    }

    case AngAndRSel_AngR:
    {
        //复合运动模式 什么也不做
    }

    default:
        break;
    }

    return 1;
}

//外部对象声明

//内部方法声明



//***********************************END OF FILE***********************************

