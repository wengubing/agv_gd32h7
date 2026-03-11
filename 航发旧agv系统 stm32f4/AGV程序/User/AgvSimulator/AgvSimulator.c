/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: AgvSimulator.c
** 创 建 人: 文小兵
** 描    述: Agv系统模拟器
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0
** 创建日期: 2025年8月11日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define AgvSimulator_GLOBALS

//自定义引用		
#include "AgvSimulator.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "Debug.h"
#include "common_def.h"
#include "drv_WatchDog.h"
#include "main.h"
#include "CBB_Init.h"
#include <math.h>

//外部对象声明
char dbugtemp = 0;
/*为系统状态定义的字符数组*/
//上电状态
const char* MC_WorkStep_Str[MC_WorkStep_Num] = 
{
    "MC_WorkStep_Init",
    "MC_WorkStep_PowerUp",
    "MC_WorkStep_Run",
    "MC_WorkStep_Fail",
};
//故障响应
const char* FaultsResponseSTM_Str[ACC_FaultsType_Num] =
{
    "复位",
    "正常",
    "提示",
    "限制运动",
    "受控停车",
    "非受控停车",
};
//模式
const char* Mode_Str[ACCMode_Num] =
{
    "复位",
    "正向直线",
    "正向自由",
    "正向巡线",
    "横向直线",
    "横向自由",
    "横向巡线",
    "自转",
    "静止",
    "升降",
    "悬挂",
    "推送",
    "机械臂",
    "抱夹",
    "支腿",
    "滚筒",
};

//外部方法声明

//内部方法声明

//代码逻辑异常检出
bool LogicErrDetect(void)
{
	bool res = 0;
	char i = 0;
	char WalkMotorV_NotZero = 0;
	//抱闸打开，但行走伺服未使能检出

	//行走伺服有速度，但抱闸未打开检出
	for (i = 0; i < g_MotorResolving.Para.MotorNumber; i++)
	{
		if(AccData.WalkMotor[i].V != 0)
			WalkMotorV_NotZero ++;
		else {}
	}

	/**/
    if(WalkMotorV_NotZero && AccData.IOCtrl.MoveBrake == 0)
		CodeLogicErr.LogicErr.Bit.MoveBrake_OffErr = 1;
	else {}//只触发不恢复

	//res = ((*(u16*)&(CodeLogicErr.LogicErr)) != 0);
	res = CodeLogicErr.LogicErr.All;
	CodeLogicErr.Res = res;

	return res;
}
#ifdef SSMLT
//遥控器模拟数据 直接接管项目遥控器解析
bool ASM_WirelessRemote(AgvSimulator_TypeDef* p)
{
    g_ChannelSwitch.CtrlCmd = p->OrgRmcCmd;
    return 1;
}

//获取遥控喇叭命令
bool ASM_WirelessRemote_Horn(AgvSimulator_TypeDef* p)
{
    return p->OrgRmcCmd.Button.Horn;
}
//BMS启动模拟
bool ASM_BMS(AgvSimulator_TypeDef* p)
{
    if(g_PowerOn.BatteryType == ACC_CanopenStep_Self_LithiumBattery_High)//高压锂电
    {
        //上电1s后开始通信
        //并模拟高压锂电池数据
        if(g_Sys_1ms_Counter>1000)
        {
            if (g_BMSData.InPutData)
            {
                //g_BMSData.BMS_Read.Voltage = 300;//在上一级原始数据？
                ((EQP_BMS_CommConvObj_StDef*)(g_BMSData.InPutData))->Voltage = 300;
                ((EQP_BMS_CommConvObj_StDef*)(g_BMSData.InPutData))->SOC = 800;
            }
            else {}
            
            //如果主DCDC输入 打开 系统电压启动
            if(g_IOOutputs.MainDCDC.OutValue)//从底层io_dhr提取非常麻烦，这里简化从g_IOOutputs判断
            {
				g_ADFucs.SysCtrlVoltage.Fiter.out_FilterValue = 240;//模拟系统控制电压
                return 1;
            }
            else {}
       }
       else {}
    }
    else if(g_PowerOn.BatteryType == ACC_CanopenStep_Self_LithiumBattery_Low)//低压锂电
    {
        
    }
    else if(g_PowerOn.BatteryType == ACC_BatteryType_LeadBatteries)//铅酸电池
    {
        
    }
    else
    {
        
    }

    return 0;
}

//modbus 通信计数
bool ASM_CommCnt(AgvSimulator_TypeDef* p)
{
    char i = 0;
    CCFEQP_Master_StDef* p2EQPMst = (CCFEQP_Master_StDef*)&EQPMsts;
	CCFEQP_Slave_StDef* p2EQPSlv = (CCFEQP_Slave_StDef*)&EQPSlvs;

    for (i = 0; i < EQPMsts.Number; i++)
	{
		p2EQPMst[i].TimeOutMs = 0;
        p2EQPMst[i].CommOkCnt ++;
	}
    for (i = 0; i < EQPSlvs.Number; i++)
	{
		p2EQPSlv[i].TimeOutMs = 0;
        p2EQPSlv[i].CommOkCnt ++;
	}
	return 1;
}

//状态机 初始化
bool ASM_StmInit(AgvSimulator_TypeDef* p)
{
    p->CommCntDelay = 1000;

	/*电池启动正常标志 这里临时强制为1，后续可扩展为BMS通信模拟*/
	g_PowerOn.in_BatteryReady = 1;
    /*电池SOC 这里临时强制初始化为固定正常值，后续升级为正常根据电池类型解析*/
    g_BMSData.BMS_Read.SOC = 90;

    //输入io
    g_IOInputs.EmgStop.RealtimeValue = 1;
	g_IOInputs.EmgStop.Filter.ButtonPressCnt = 1000;//直接跳过了按键扫描等待
    g_IOInputs.RemoteReceiverEmgStop.RealtimeValue = 1;

    //模拟遥控器通信急停未按下 将p->p_DataInput[12]第8位置1
    g_WHData.p_DataInput[12] |= 0x01<<8;
    //模拟遥控器信号强度
    g_WHData.p_DataInput[3] = 5;

    //轮系传感器转接板在线状态
    EQP_CommConv.RD[0] = 0xff;
    EQP_CommConv2.RD[0] = 0xff;

    //遥控器
    p->OrgRmcCmd.HandleUnionVal = AsbModel_A;
    p->OrgRmcCmd.Mode = ACCMode_PosStraight;
    p->OrgRmcCmd.Vxyz.x = 100;
    p->OrgRmcCmd.Vxyz.y = 101;
    p->OrgRmcCmd.Vxyz.z = 102;
    p->OrgRmcCmd.WalkGear = 3;
    p->OrgRmcCmd.LiftGear = 1;

    p->OrgRmcCmd.RadarEn.Front = 0;
    p->OrgRmcCmd.RadarEn.Back = 0;
    p->OrgRmcCmd.RadarEn.Left = 0;
    p->OrgRmcCmd.RadarEn.Right = 0;
    p->OrgRmcCmd.BarEn.Front = 0;
    p->OrgRmcCmd.BarEn.Back = 0;
    p->OrgRmcCmd.BarEn.Left = 0;
    p->OrgRmcCmd.BarEn.Right = 0;

    p->OrgRmcCmd.Button.Horn = 0;
    p->OrgRmcCmd.Button.EStop = 1;
    return 1;
}

//用于测试底盘参数初始化
bool AuT_WalkParaInit(void)
{
    char i = 0;
    g_Para.EmgJudge.ModeSwitchTimeOutJudge = 65535;//临时关闭模式切换超时检出
    g_Para.Walk.AngInPlaceMin_ChangeIng = 3;
    g_Para.Walk.MotorNum = 16;
	g_Para.WheelAngle.Num = g_Para.Walk.MotorNum / 2;
    i = 0;
    g_Para.Walk.Mxy[i].x = -1000;
    g_Para.Walk.Mxy[i].y = 2000;
    i = 1;
    g_Para.Walk.Mxy[i].x = 1000;
    g_Para.Walk.Mxy[i].y = 2000;
    i = 2;
    g_Para.Walk.Mxy[i].x = -1000;
    g_Para.Walk.Mxy[i].y = -2000;
    i = 3;
    g_Para.Walk.Mxy[i].x = 1000;
    g_Para.Walk.Mxy[i].y = -2000;
    i = 4;//x轴上
    g_Para.Walk.Mxy[i].x = 1000;
    g_Para.Walk.Mxy[i].y = 0;
    i = 5;//y轴上
    g_Para.Walk.Mxy[i].x = 0;
    g_Para.Walk.Mxy[i].y = 2000;
    i = 6;//原点
    g_Para.Walk.Mxy[i].x = 0;
    g_Para.Walk.Mxy[i].y = 0;
    i = 7;//y轴上 负半轴
    g_Para.Walk.Mxy[i].x = 0;
    g_Para.Walk.Mxy[i].y = -2000;
    MotorResolving_Init(&g_MotorResolving);

    //cbb 直接初始化内容
    for (i = 0; i < g_Para.Walk.MotorNum / 2; i++)
    {
        g_MotorResolving.Para.Mxy[i].Inf_Invflg = 0;
        g_MotorResolving.Para.Mxy[i].RealAngMax_Left = +97;
        g_MotorResolving.Para.Mxy[i].RealAngMax_Right = -138;
        g_MotorResolving.Para.Mxy[i].CtrlAngMax_Left = +96;//控制角度比实际角度小1度
        g_MotorResolving.Para.Mxy[i].CtrlAngMax_Right = -137;
    }
    g_MotorResolving.VirAng.Wmax = 100;

    //8号 轮系方向取反
    i = 7;
    g_MotorResolving.Para.Mxy[i].Inf_Invflg = 1;
    g_MotorResolving.Para.Mxy[i].RealAngMax_Left = +97;
    g_MotorResolving.Para.Mxy[i].RealAngMax_Right = -47;
    g_MotorResolving.Para.Mxy[i].CtrlAngMax_Left = +96;//控制角度比实际角度小1度
    g_MotorResolving.Para.Mxy[i].CtrlAngMax_Right = -46;

    return 1;
}

//检查目标角度轮系轴线都交于一点
bool AuT_CheckTgtAngCrossPoint(void)
{
	char i = 0;
	char errcnt = 0;
	AuT_CrossP_Typedef* p2CrossP = &g_ASM.AutoTest.CrossP;
    char CrossP_Length = sizeof(g_ASM.AutoTest.CrossP.CrossP) / sizeof(g_ASM.AutoTest.CrossP.CrossP[0]);

    /*构建8条直线方程
    if(dbugtemp == 0)*/
    for (i = 0; i < g_Para.Walk.MotorNum / 2; i++)
    {
        p2CrossP->Lines[i].P.X = g_MotorResolving.Para.Mxy[i].X_Axis;
        p2CrossP->Lines[i].P.Y = g_MotorResolving.Para.Mxy[i].Y_Axis;
        //p2CrossP->Lines[i].Ang = -(g_MotorResolving.TgtAng[i] - 90);//轴线与目标角度垂直，且取反结果才正确 待细究
        p2CrossP->Lines[i].Ang = -(g_MotorResolving.Input.ActualAng[i] - 90);//这是实际角度
	}

	//清除上次结果
    p2CrossP->Num = 0;
    for (i = 0; i < CrossP_Length; i++)
    {
        p2CrossP->CrossP[i].X = 0;
        p2CrossP->CrossP[i].Y = 0;
	}

	//获取交点
    p2CrossP->Num = CalcLinesCrossPoint(p2CrossP->Lines, g_Para.Walk.MotorNum / 2, p2CrossP->CrossP);

	//交点数量至少28个
    if (p2CrossP->Num < 24)//有三条线都为90度平行，所以少4条线 28-4=24
        errcnt++;
    else
        __asm__("nop");

    /*找到所有点的最小外接矩形的对角线长度*/
    p2CrossP->Diagonal = CalculateBoundingBoxDiagonal(p2CrossP->CrossP, p2CrossP->Num);
    if (p2CrossP->Diagonal > 50)
		errcnt++;
    else
        __asm__("nop");

    if (errcnt)
        return 0;
    else
        return 1;
}

//计算目标角度轮系轴线相交情况
bool AuT_CalTgtAngCrossPoint(void)
{
    char i = 0;
    char errcnt = 0;
    AuT_CrossP_Typedef* p2CrossP = &g_ASM.AutoTest.CrossP;
    char CrossP_Length = sizeof(g_ASM.AutoTest.CrossP.CrossP) / sizeof(g_ASM.AutoTest.CrossP.CrossP[0]);

    /*构建8条直线方程
    if(dbugtemp == 0)*/
    for (i = 0; i < g_Para.Walk.MotorNum / 2; i++)
    {
        p2CrossP->Lines[i].P.X = g_MotorResolving.Para.Mxy[i].X_Axis;
        p2CrossP->Lines[i].P.Y = g_MotorResolving.Para.Mxy[i].Y_Axis;
        p2CrossP->Lines[i].Ang = -(g_MotorResolving.TgtAng[i] - 90);//轴线与目标角度垂直，且取反结果才正确 待细究
        //p2CrossP->Lines[i].Ang = -(g_MotorResolving.Input.ActualAng[i] - 90);//这是实际角度
    }

    //清除上次结果
    p2CrossP->Num = 0;
    for (i = 0; i < CrossP_Length; i++)
    {
        p2CrossP->CrossP[i].X = 0;
        p2CrossP->CrossP[i].Y = 0;
    }

    //获取交点
    p2CrossP->Num = CalcLinesCrossPoint(p2CrossP->Lines, g_Para.Walk.MotorNum / 2, p2CrossP->CrossP);

    //交点数量至少28个
    if (p2CrossP->Num < 24)//有三条线都为90度平行，所以少4条线 28-4=24
        errcnt++;
    else
        __asm__("nop");

    /*找到所有点的最小外接矩形的对角线长度*/
    p2CrossP->Diagonal = CalculateBoundingBoxDiagonal(p2CrossP->CrossP, p2CrossP->Num);
    if (p2CrossP->Diagonal > 50)
        errcnt++;
    else
        __asm__("nop");

    if (errcnt)
        return 0;
    else
        return 1;
}

//静态代码检查
bool AuT_StaticCodeErrCheck(AutoTest_Typedef* p)
{
    char ErrCnt = 0;

    //如果没有使能 直接退出
    if (p->Config.StaticCheck == 0)
        return 0;
    else {}

    //检查外部看门狗是否开启
    /*#ifndef无法正确判断局部的宏定义*/
    #ifndef EXT_WatchDog_FUNC
    ErrCnt++;
    #endif
		
    /*用if可以在编译时通过报错避免没有宏定义的作用范围
    if (EXT_WatchDog_FUNC == 0)
        ErrCnt++;
    else {}*/

    if(ErrCnt)
        p->Result.Bit.Static = 1;
    else {}

    //运行后关闭检测使能
    p->Config.StaticCheck = 0;

    return ErrCnt;
}

//车体急停故障检出及其故障响应测试
bool AuT_CarEStop(AutoTest_Typedef* p)
{
    //如果没有使能 直接退出
    if (p->Config.CarEstop == 0)
        return 0;
    else {}

    switch(p->CSP_Stm)
    {
        case AuT_CSP_Stm_Init://初始化状态
        {
            p->MsRecord_CSP = p->Ms;
            printf("CSP：开始...\n");
            p->CSP_Stm = AuT_CSP_Stm_WaitPowerUp;
        }
        break;

        case AuT_CSP_Stm_WaitPowerUp://等待上电
        {
            if(g_MC_WorkStep == MC_WorkStep_Run)
            {
                g_ASM.OrgRmcCmd.Mode = ACCMode_Still;
                printf("CSP：等待静止且无报警...\n");
                p->CSP_Stm = AuT_CSP_Stm_WaitStill;
            }
            else {}
        }
        break;

        case AuT_CSP_Stm_WaitStill://等待静止
        {
            if(AccData.ModeSwitchSMT == ACCMode_Still && AccData.FaultsResponseSTM == ACC_FaultsType_Null)//模式静止且没有报警
            {
                printf("CSP：模拟按下车体急停...\n");
                p->CSP_Stm = AuT_CSP_Stm_OprCSP;
            }
            else {}
        }
        break;

        case AuT_CSP_Stm_OprCSP://软件模拟操作急停
        {
            g_IOInputs.EmgStop.RealtimeValue = 0;
			p->MsRecord_CSP = p->Ms;
            printf("CSP：稍等...\n");
            p->CSP_Stm = AuT_CSP_Stm_Wait;
        }
        break;

        case AuT_CSP_Stm_Wait://延时
        {
            if(p->Ms - p->MsRecord_CSP > 3000)
            {
                printf("CSP：检查非受控停车标志检出与响应是否正常...\n");
                p->CSP_Stm = AuT_CSP_Stm_Judge;
            }
            else {}
        }
        break;

        case AuT_CSP_Stm_Judge://判断故障检出和响应是否正常
        {
            if (AccData.FaultsResponseSTM == ACC_FaultsType_LoseControlStop
                && g_Faults.LoseControl.Bit.CarEstop
                //&& 所有电机都0速 关闭使能 抱闸关闭
                )
            {
                printf("CSP：Success!\n");
            }
            else
            {
                p->Result.Bit.CarEstop = 1;
                printf("CSP：Failed!\n");
            }

            p->Config.CarEstop = 0;
            p->CSP_Stm = AuT_CSP_Stm_Init;
        }
        break;

        default:
        break;
    }
	
	return 1;
}

//底盘模式切换测试 切横向
bool AuT_ModeSwitch2Inf(AutoTest_Typedef* p)
{
    const char* p2Str = "切横向测试：";

    //如果没有使能 直接退出
    if (p->Config.ModeSwitch2Inf == 0)
        return 0;
    else {}

    switch (p->MS2Inf_Stm)
    {
    case AuT_ModeSwitch2Inf_Stm_Init://初始化状态
    {
        p->MsRecord = p->Ms;
        printf("\n------------------模式切换测试------------------\n");
        printf("%s开始...\n",p2Str);
		g_ASM.OrgRmcCmd.Mode = ACCMode_Still;
        g_ASM.OrgRmcCmd.Vxyz.x = 0;
        g_ASM.OrgRmcCmd.Vxyz.y = 0;
        g_ASM.OrgRmcCmd.Vxyz.z = 0;
        p->MS2Inf_Stm = AuT_ModeSwitch2Inf_Stm_WaitStill;
    }
    break;

    case AuT_ModeSwitch2Inf_Stm_WaitStill://等待静止
    {
        if (g_MC_WorkStep == MC_WorkStep_Run && AccData.ModeSwitchSMT == ACCMode_Still && AccData.FaultsResponseSTM == ACC_FaultsType_Null)//模式静止且没有报警
        {
            p->MsRecord = p->Ms;
            printf("%s模拟切换到横向...\n", p2Str);
            /*设置四个模组，每个象限各1各，3象限的横向朝向取反设置，一起转，
            g_MotorResolving.VirAng.Wmax = 50;
            g_MotorResolving.Para.AngInPlaceMin_ChangeIng = 3;
            g_MotorResolving.Para.ModuleNumber = 4;
            g_MotorResolving.Para.MotorNumber = 8;
            g_MotorResolving.Para.Mxy[2].Inf_Invflg = 1;
            g_MotorResolving.Para.Mxy[2].RealAngMax_Left = +97;
            g_MotorResolving.Para.Mxy[2].RealAngMax_Right = -47;
            g_MotorResolving.Para.Mxy[2].CtrlAngMax_Left = +96;//控制角度比实际角度小1度
            g_MotorResolving.Para.Mxy[2].CtrlAngMax_Right = -46;*/

            //用于测试底盘参数初始化
            AuT_WalkParaInit();

			g_ASM.OrgRmcCmd.Mode = ACCMode_InfFlexible;
            p->MS2Inf_Stm = AuT_ModeSwitch2Inf_Stm_Judge;
        }
        else {}
	}
	break;

    case AuT_ModeSwitch2Inf_Stm_Judge://判断是否切换正确
    {
        char errcnt = 0;
		char i = 0;
		short AngTgtTemp[8] = { -90,-90,-90,-90,-90,-90,-90,90 };//目标角度

        if (AccData.ModeSwitchSMT != ACCMode_InfFlexible)
            errcnt++;
        else {}

        //检查各个轮系是否转到既定的目标角度
        for (i = 0; i < 8; i++)
        {
            if (fabs(g_WheelAngle[i].Res - AngTgtTemp[i]) > g_MotorResolving.Para.AngInPlaceMin_ChangeIng)
                errcnt++;
            else {}
		}     
        
        if (errcnt == 0)
        {
            printf("%sSuccess!\n", p2Str);
        }
        else
        {
			//判断超时
            if (p->Ms - p->MsRecord < 30000)
				break;
            else
            {
                p->Result.Bit.ModeSwitch2Inf = 1;
                printf("%sFailed!\n", p2Str);
            }
        }
        p->Config.ModeSwitch2Inf = 0;
        p->MS2Inf_Stm = AuT_ModeSwitch2Inf_Stm_Init;
	}
	break;

    default:
        break;
    }

    return 1;
}

//底盘模式切换测试 切自转
bool AuT_ModeSwitch2Rot(AutoTest_Typedef* p)
{
    const char* p2Str = "切自转测试：";

    //如果没有使能 直接退出
    if (p->Config.ModeSwitch2Rot == 0)
        return 0;
    else {}

    switch (p->MS2Rot_Stm)
    {
    case AuT_ModeSwitch2Rot_Stm_Init://初始化状态
    {
        p->MsRecord = p->Ms;
        printf("\n------------------模式切换测试------------------\n");
        printf("%s开始...\n", p2Str);
        g_ASM.OrgRmcCmd.Mode = ACCMode_Still;
        g_ASM.OrgRmcCmd.Vxyz.x = 0;
        g_ASM.OrgRmcCmd.Vxyz.y = 0;
        g_ASM.OrgRmcCmd.Vxyz.z = 0;
        p->MS2Rot_Stm = AuT_ModeSwitch2Rot_Stm_WaitStill;
    }
    break;

    case AuT_ModeSwitch2Rot_Stm_WaitStill://等待静止
    {
        if (g_MC_WorkStep == MC_WorkStep_Run && AccData.ModeSwitchSMT == ACCMode_Still && AccData.FaultsResponseSTM == ACC_FaultsType_Null)//模式静止且没有报警
        {
            p->MsRecord = p->Ms;
            printf("%s模拟切换到...\n", p2Str);
            //用于测试底盘参数初始化
            AuT_WalkParaInit();
            
            g_ASM.OrgRmcCmd.Mode = ACCMode_Rotation;
            p->MS2Rot_Stm = AuT_ModeSwitch2Rot_Stm_Judge;
        }
        else
            __asm__("nop");
    }
    break;

    case AuT_ModeSwitch2Rot_Stm_Judge://判断是否切换正确
    {
		char errcnt = 0;
		char i = 0;

        if (AccData.ModeSwitchSMT != ACCMode_Rotation)
            errcnt++;
        else
            __asm__("nop");

        if (AuT_CheckTgtAngCrossPoint() == 0)//目标角度轮系轴线都交于一点
            errcnt++;
        else
            __asm__("nop");
        
        //检查各个轮系是否转到既定的目标角度
        for (i = 0; i < 8; i++)
        {
            if (fabs(g_WheelAngle[i].Res - g_MotorResolving.TgtAng[i]) > g_MotorResolving.Para.AngInPlaceMin_ChangeIng)
                errcnt++;
            else
                __asm__("nop");
        }

        if (errcnt == 0)
        {
            printf("%sSuccess!\n", p2Str);
        }
        else
        {
            //判断超时
            if (1)//p->Ms - p->MsRecord < 30000)
                break;
            else
            {
                p->Result.Bit.ModeSwitch2Rot = 1;
                printf("%sFailed!\n", p2Str);
            }
        }
        p->Config.ModeSwitch2Rot = 0;
        p->MS2Rot_Stm = AuT_ModeSwitch2Rot_Stm_Init;
    }
    break;

    default:
        break;
    }

    return 1;
}

//底盘解算 模组交于一点测试
bool AuT_CheckTgtAngCross(AutoTest_Typedef* p)
{
    const char* p2Str = "交于一点测试：";

    //如果没有使能 直接退出
    if (p->Config.CheckTgtAngCross == 0)
        return 0;

    switch (p->CTAC_Stm)
    {
    case AuT_CTAC_Stm_Init://初始化状态
    {
        p->MsRecord = p->Ms;
        printf("\n------------------%s------------------\n", p2Str);
        printf("%s开始...\n", p2Str);
        g_ASM.OrgRmcCmd.Mode = ACCMode_Still;
        g_ASM.OrgRmcCmd.Vxyz.x = 0;
        g_ASM.OrgRmcCmd.Vxyz.y = 0;
        g_ASM.OrgRmcCmd.Vxyz.z = 0;
        p->CTAC_Stm = AuT_CTAC_Stm_WaitStill;
    }
    break;

    case AuT_CTAC_Stm_WaitStill://等待静止
    {
        if (g_MC_WorkStep == MC_WorkStep_Run && AccData.ModeSwitchSMT == ACCMode_Still && AccData.FaultsResponseSTM == ACC_FaultsType_Null)//模式静止且没有报警
        {
            p->MsRecord = p->Ms;
            printf("%s模拟切换到...\n", p2Str);
            //用于测试底盘参数初始化
            AuT_WalkParaInit();

            g_ASM.OrgRmcCmd.Mode = ACCMode_PosFlexible;
            g_ASM.OrgRmcCmd.Vxyz.x = 10;
            g_ASM.OrgRmcCmd.Vxyz.y = 100;
            g_ASM.OrgRmcCmd.Vxyz.z = 20;

            p->CTAC_Stm = AuT_CTAC_Stm_Judge;
        }
        else
            __asm__("nop");
    }
    break;

    case AuT_CTAC_Stm_Judge://判断是否切换正确
    {
        char errcnt = 0;
        char i = 0;

        if (AccData.ModeSwitchSMT != ACCMode_Rotation)
            errcnt++;
        else
            __asm__("nop");

        /*目标角度轮系轴线都交于一点
        if (AuT_CheckTgtAngCrossPoint() == 0)
            errcnt++;
        else
            __asm__("nop");*/

        //只是计算交点来查看
        AuT_CalTgtAngCrossPoint();

        //检查各个轮系是否转到既定的目标角度
        for (i = 0; i < 8; i++)
        {
            if (fabs(g_WheelAngle[i].Res - g_MotorResolving.TgtAng[i]) > g_MotorResolving.Para.AngInPlaceMin_ChangeIng)
                errcnt++;
            else
                __asm__("nop");
        }

        if (errcnt == 0)
        {
            printf("%sSuccess!\n", p2Str);
        }
        else
        {
            //判断超时
            if (1)//p->Ms - p->MsRecord < 30000)
                break;
            else
            {
                p->Result.Bit.ModeSwitch2Rot = 1;
                printf("%sFailed!\n", p2Str);
            }
        }
        p->Config.ModeSwitch2Rot = 0;
        p->CTAC_Stm = AuT_CTAC_Stm_Init;
    }
    break;

    default:
        break;
    }

    return 1;
}

//自动测试
/*模拟异常发生*/
/*模拟异常发生后，判断对应的异常报警是否检出，实现自动化测试*/
//局部测试->回归测试
//正常功能->异常能检出、单点、全域测试、复合全域测试
bool AuT_Run(AutoTest_Typedef* p)
{
    p->Ms = g_Sys_1ms_Counter;

    /*如果没有使能 直接退出*/
    if (p->Config.En == 0)
        return 1;
    else {}

    //静态代码检测
    AuT_StaticCodeErrCheck(p);

    //车体急停故障检出及其故障响应测试
    AuT_CarEStop(p);

	//模式切换测试 切横向
	AuT_ModeSwitch2Inf(p);

    //切自转
    AuT_ModeSwitch2Rot(p);

	//模组交于一点测试
	AuT_CheckTgtAngCross(p);

	/*自转模式解算测试
    AuT_MR_Rot(p);*/

    //角度传感器通信超时

    //解算超限
    //

    //实际角度过大

    //解算测试 目标角度、
    //通过目标角度、电机目标速度，模拟实际运动是否协调、模组角度是否受控
    //坐标轴和原点 转弯运动解算
    //多小的目标角度差认为平行？

    return 1;
}

//启动模拟器 状态机
bool ASM_PowerOn_Stm(AgvSimulator_TypeDef* p)
{
    switch (p->Stm)
    {
    case ASM_Stm_Null://初始化状态
        p->Stm = ASM_Stm_Init;
        break;

    case ASM_Stm_Init:
    {
        if (ASM_StmInit(p))
            p->Stm = ASM_Stm_BMS;
        else {}
    }
    break;

    case ASM_Stm_BMS:
    {
        if (ASM_BMS(p))//BMS启动模拟
            p->Stm = ASM_Stm_PowerUp;
        else {}
    }
    break;

    case ASM_Stm_PowerUp:
    {
        if (g_IOOutputs.MotorPrecharge.OutValue || g_IOOutputs.Precharge.OutValue)//如果预充打开
        {
            CanMaster.CanopenStep = Step_Active;//canopen 启动模拟
            p->Stm = ASM_Stm_Ready;
        }
        else {}
    }
    break;

    default:
        break;
    }

    return 1;
}

//上电状态
bool PowerOn_Log(Debug_Log_TypeDef* p)
{
    if (p->LastST.PowerOnStep != g_PowerOn.PowerOnStep)
    {
        p->LastST.PowerOnStep = g_PowerOn.PowerOnStep;
        printf("PowerOnStep --> %d\n", p->LastST.PowerOnStep);
    }
    else {}

    return 1;
}

//启动状态
bool WorkStep_Log(Debug_Log_TypeDef* p)
{
    if (p->LastST.MC_WorkStep != g_MC_WorkStep)
    {
        p->LastST.MC_WorkStep = g_MC_WorkStep;
        printf("PowerOnStep --> %s\n", MC_WorkStep_Str[p->LastST.MC_WorkStep]);
    }
    else {}

    return 1;
}

//异常响应
bool FaultsResponse_Log(Debug_Log_TypeDef* p)
{
    if (p->LastST.FaultsResponseSTM != AccData.FaultsResponseSTM)
    {
        p->LastST.FaultsResponseSTM = AccData.FaultsResponseSTM;
        printf("FaultsResponseSTM --> %s\n", FaultsResponseSTM_Str[p->LastST.FaultsResponseSTM]);
    }
    else {}
    return 1;
}

//模式切换
bool Mode_Log(Debug_Log_TypeDef* p)
{
    if (p->LastST.Mode != AccData.CalCmd.Mode)
    {
        p->LastST.Mode = AccData.CalCmd.Mode;
        printf("Mode --> %s\n", Mode_Str[p->LastST.Mode]);
    }
    else {}
    return 1;
}

//故障检出
bool ErrJudge_Log(Debug_Log_TypeDef* p)
{
    ACC_Faults_TypeDef* p2Faults = &g_Faults;
    LastErrBit_TypeDef* p2Last = &(p->LastErrBit);

    if (p2Last->EStop_Rmc != p2Faults->EmgStop.Bit.Remote)
    {
        p2Last->EStop_Rmc = p2Faults->EmgStop.Bit.Remote;
        if (p2Last->EStop_Rmc)
            printf("异常检出 --> 遥控器急停 触发\n");
        else
            printf("异常检出 --> 遥控器急停 恢复\n");
    }
    else {}

    //车体急停
    if (p2Last->EStop_Car != p2Faults->LoseControl.Bit.CarEstop)
    {
        p2Last->EStop_Car = p2Faults->LoseControl.Bit.CarEstop;
        if (p2Last->EStop_Car)
            printf("异常检出 --> 车体急停 触发\n");
        else
            printf("异常检出 --> 车体急停 恢复\n");
    }
    else {}

    //轮系角度解算异常
    if (p2Last->WheelAngCalErr != p2Faults->Other.Bit.WheelAngCalErr)
    {
        p2Last->WheelAngCalErr = p2Faults->Other.Bit.WheelAngCalErr;
        if (p2Last->WheelAngCalErr)
            printf("异常检出 --> 轮系角度解算异常 触发\n");
        else
            printf("异常检出 --> 轮系角度解算异常 恢复\n");
    }
	else {}

	//轮系实际角度过大
    if (p2Last->wheelAngleLimited != p2Faults->Func.Bit.wheelAngleLimited)
    {
        p2Last->wheelAngleLimited = p2Faults->Func.Bit.wheelAngleLimited;
        if (p2Last->wheelAngleLimited)
            printf("异常检出 --> 轮系实际角度过大 触发\n");
        else
            printf("异常检出 --> 轮系实际角度过大 恢复\n");
    }
	else {}

	//模式切换超时
    if (p2Last->ModeSwitchTimeOut != p2Faults->Func.Bit.ModeSwitchTimeOut)
    {
        p2Last->ModeSwitchTimeOut = p2Faults->Func.Bit.ModeSwitchTimeOut;
        if (p2Last->ModeSwitchTimeOut)
            printf("异常检出 --> 模式切换超时 触发\n");
        else
            printf("异常检出 --> 模式切换超时 恢复\n");
	}
	else {}

    return 1;
}

//系统日志 触发式记录、打印
bool Debug_Log(Debug_Log_TypeDef* p)
{
    //上电状态
    PowerOn_Log(p);
    //启动状态
    WorkStep_Log(p);
    /*控制源切换
    CtrlSrc_Log(p);*/
    /*自动与遥控切换
    AutoRmc_Log(p);*/
    /*单动联动切换
    Asb_Log(p);*/
    //异常响应
    FaultsResponse_Log(p);
    //模式切换
    Mode_Log(p);

    //异常检出 检出和恢复时分别触发打印log
    ErrJudge_Log(p);

	return 1;
}
//agv模拟器 调用入口
bool ASM_Run(AgvSimulator_TypeDef* p)
{
    /*如果非纯软件仿真模式，立即返回*/
	if (g_Para.RunningMode != ACC_RunningMode_SoftwareSimulat)
		return 0;
    else {}

    //系统日志 触发式记录、打印
    Debug_Log(&(p->DBLog));

    //自动化测试
    AuT_Run(&p->AutoTest);

    //ms更新
	p->Ms = g_Sys_1ms_Counter;

    //启动模拟器 状态机
    ASM_PowerOn_Stm(p);

    //定时模拟modbus通信
    if(p->Ms - p->MsRecord_CommCnt > p->CommCntDelay)
    {
        ASM_CommCnt(p);
        p->MsRecord_CommCnt = p->Ms;
    }
    else {}

	return 1;   
}
#else
//非simulator模式不做任何操作 节省编译空间
bool ASM_Run(AgvSimulator_TypeDef* p)
{
    return 0;
}
#endif

//***********************************END OF FILE***********************************

