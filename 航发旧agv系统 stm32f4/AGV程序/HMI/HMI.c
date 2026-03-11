/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: HMI.c
** 创 建 人: 文小兵  
** 描    述: HMI与系统的交互
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年7月9日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define HMI_GLOBALS

//自定义引用			
#include "HMI.h"
#include "CBB_Init.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "drv_IO.h"
#include "Debug.h"
#include "Fun_Template.h"


short int ACTTemp1[HMI_AngleSensor_MaxNum] = {0};
//外部对象声明

//内部方法声明

//功能开关 不保存的
bool HMI_ReadFunctionSwitch(void)
{
    
	// 0 全显示  1 只显示 前后左右  2  只显示 转到设置  3 全部不显示
	Screen_NOParaObs* p = ExtHmi.pScr_NOParaObs; 
		p->HMI_NOOROFF = g_Para.HmiConceal.ParaObs_NO_Switch ;
    //避障雷达开关 
    if (ExtHmi.PageInitialized != 1)
    {
        ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.ForwardRadarEnable_Bit0 = g_ChannelSwitch.HmiRadarEn.Front;
        ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.BackRadarEnable_Bit1 = g_ChannelSwitch.HmiRadarEn.Back;
        ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.LeftRadarEnable_Bit2 = g_ChannelSwitch.HmiRadarEn.Left;
        ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.RightRadarEnable_Bit3 = g_ChannelSwitch.HmiRadarEn.Right;
		if(AccData.Para.DisLimit_Min == 65535)
			ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.Forward_MoveLimtEnable_Bit0 = 0;
		else
			ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.Forward_MoveLimtEnable_Bit0 = 1;
		if(AccData.Para.DisLimit_Max == 65535)
			ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.Back_MoveLimtEnable_Bit1 = 0;
		else
			ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.Back_MoveLimtEnable_Bit1 = 1;
		
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        g_ChannelSwitch.HmiRadarEn.Front = ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.ForwardRadarEnable_Bit0;
        g_ChannelSwitch.HmiRadarEn.Back = ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.BackRadarEnable_Bit1;
        g_ChannelSwitch.HmiRadarEn.Left = ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.LeftRadarEnable_Bit2;
        g_ChannelSwitch.HmiRadarEn.Right = ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.RightRadarEnable_Bit3;
                if(ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.Forward_MoveLimtEnable_Bit0 == 0)
			AccData.Para.DisLimit_Min = 65535;
		else
			AccData.Para.DisLimit_Min = AccData.Para.DisLimit_Min_temp;
		if(ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.Back_MoveLimtEnable_Bit1 == 0)
			AccData.Para.DisLimit_Max = 65535;
		else
			AccData.Para.DisLimit_Max = AccData.Para.DisLimit_Max_temp;
    }
    return 1;
}


//功能开关 可保存按钮
bool HMI_FunctionButtonSave(void)
{
    Screen_ParaObs* p = ExtHmi.p2Scr_ParaObs;
   // 0 全显示  1 只显示雷达  2 只显示触边 3 不显示
    p->HMI_NOOROFF = g_Para.HmiConceal.ParaObs_Switch;
    if (ExtHmi.PageInitialized != 1)
    {
        //可保存的功能开关 雷达
        p->Bit.RadarEnF = g_Para.FunctionSwitch.RadarEn.Front;
        p->Bit.RadarEnB = g_Para.FunctionSwitch.RadarEn.Back;
        p->Bit.RadarEnL = g_Para.FunctionSwitch.RadarEn.Left;
        p->Bit.RadarEnR = g_Para.FunctionSwitch.RadarEn.Right;

        //触边
        p->Bit.CrashEnF = g_Para.FunctionSwitch.BarEn.Front;
        p->Bit.CrashEnB = g_Para.FunctionSwitch.BarEn.Back;
        p->Bit.CrashEnL = g_Para.FunctionSwitch.BarEn.Left;
        p->Bit.CrashEnR = g_Para.FunctionSwitch.BarEn.Right;

        ExtHmi.PageInitialized = 1;
    }
    else
    {
        //可保存的功能开关 雷达
        g_Para.FunctionSwitch.RadarEn.Front = p->Bit.RadarEnF;
        g_Para.FunctionSwitch.RadarEn.Back = p->Bit.RadarEnB;
        g_Para.FunctionSwitch.RadarEn.Left = p->Bit.RadarEnL;
        g_Para.FunctionSwitch.RadarEn.Right = p->Bit.RadarEnR;

        //触边
        g_Para.FunctionSwitch.BarEn.Front = p->Bit.CrashEnF;
        g_Para.FunctionSwitch.BarEn.Back = p->Bit.CrashEnB;
        g_Para.FunctionSwitch.BarEn.Left = p->Bit.CrashEnL;
        g_Para.FunctionSwitch.BarEn.Right = p->Bit.CrashEnR;
    }
	return 1;
}

//强制按钮读取 不可保存
bool HMI_ReadForceButton(void)
{
    //系统重启复位
	if(ExtHmi.p2HMI_CommObj->ButtonForce.Bit.SysReboot_Bit1)
	{
		g_SysApi.SysReboot = 1;
		ExtHmi.p2HMI_CommObj->ButtonForce.Bit.SysReboot_Bit1 = 0;
	}
    else {}
	
	//标定低点
	if(ExtHmi.p2HMI_CommObj->ButtonForce.Bit.LowValueSet_Bit2)
	{
		g_SysApi.LiftLowSet = 1;
		ExtHmi.p2HMI_CommObj->ButtonForce.Bit.LowValueSet_Bit2 = 0;
	}
    else {}

	//标定高点
	if(ExtHmi.p2HMI_CommObj->ButtonForce.Bit.HighValueSet_Bit3)
	{
		g_SysApi.LiftHighSet = 1;
		ExtHmi.p2HMI_CommObj->ButtonForce.Bit.HighValueSet_Bit3 = 0;
	}
    else {}

	//强制升降
	if(ExtHmi.p2HMI_CommObj->ButtonForce.Bit.FreeUpDownEnable_Bit4)
		g_LiftData.Cmd.do_Force = 1;
	else
		g_LiftData.Cmd.do_Force = 0;

    //强制忽略角度超限
    if(ExtHmi.p2HMI_CommObj->ButtonForce.Bit.FreeAngleEnable_Bit6)
    {
        g_SysApi.IgrWlAngLimit = 1;
    }
    else
        g_SysApi.IgrWlAngLimit = 0;

    //防撞触边故障恢复
    if(ExtHmi.p2HMI_CommObj->ButtonForce.Bit.CarshReset_Bit7)
    {
        g_SysApi.ClearObsBarLatch = 1;
    }
    else
        g_SysApi.ClearObsBarLatch = 0;

    //强制忽略所有报警
    //此处将屏蔽系统所有报警，发布程序不能轻易打开
    //使用时加入条件 危险硬件故障不响应！
    g_Para.Debug.ForceOverErr = ExtHmi.p2HMI_CommObj->ButtonForce.Bit.ForceOverErr_Bit10;

    return 1;
}


//20起始界面
bool HMI_Start(void)
{
	Screen_HMIPage_Start* p = ExtHmi.p2Scr_Start;
	
	p->Run_Mode				= AccData.CalCmd.Mode;
	p->System_SOC			= g_BMSData.BMS_Read.SOC * 10;

	return 1;
}


//21 主页显示更新
bool HMI_Show_MainPage(void)
{
    Screen_Main_Typedef* p = ExtHmi.p2Scr_Main;
	
	p->HMI_NOOROFF = g_Para.HmiConceal.AutomaticFunctions_Switch;
	//  0 默认全部显示  1 磁导航  2激光导航 
	
    //未初始化 先初始化
    if (ExtHmi.PageInitialized != 1)
    {
		p->InputPointID = g_ChannelSwitch.InputPointID;
		
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 
    {
        
		//输入id直接从界面读取
        g_ChannelSwitch.InputPointID = p->InputPointID;

        //控制方式
        p->Control_Mode = g_ChannelSwitch.STM_Channel;

        //模式统一用底盘定义
        p->WorkMode = (u16)AccData.CalCmd.Mode;

        p->System_SOC = g_BMSData.BMS_Read.SOC * 10;
        p->Insrx_Status = !g_WHData.WirelessSleep;

        //运行模式 0待机 1系统故障 2运行 3暂停 4停止 5急停 6急停恢复 7报警 8报警回复
        if (AccData.FaultsResponseSTM >= ACC_FaultsType_ControlledStop)
            p->Run_Mode = 1;
        else
        {
            if (g_SLN.in_FlgMotion)
                p->Run_Mode = 2;
            else
                p->Run_Mode = 0;
        }

        //电压
        p->System_Voltage = g_BMSData.BMS_Read.Voltage*10;

        //平台高度
        p->Pltform_Height = g_LiftData.Height.Avg * 10;

        //悬挂高度
        p->Hang_Height = g_AbsorptionData.Height.Avg;

        //当前设备朝向
        p->CurrentOrientation = g_VirtualOdoData.RTz / 100;

        //当前设备点id
        p->CurrentPointID = g_HostData.CurrentPointTag;
				
		p->ErrCode 			= EQP_IPC.R_Data->ErrCode;
		p->RunSta 			= EQP_IPC.R_Data->RunSta;
		p->LocationSta 		= EQP_IPC.R_Data->LocationSta;
    }

    return 1;
}

//24 系统信息
bool HMI_SystemInf(void)
{
    //界面对象首地址指针初始化
    SystemInformation_Typedef* p2ScreenObj = (SystemInformation_Typedef*)ExtHmi.p2Data;
    
    p2ScreenObj->HardwareVersion = 985;
    p2ScreenObj->SoftVersion = g_Para.CodeVersion;

    p2ScreenObj->System_MsCnt = g_Sys_1ms_Counter;
    p2ScreenObj->SystemCore_Temp = HardInfo.CoreTemp*10;
    p2ScreenObj->Environment_Temp = 18.2*10;
    
    p2ScreenObj->EqpID = g_Para.EqpID;
	
    p2ScreenObj->CodeErr = g_CodeErr.All;
    p2ScreenObj->LogicErr = CodeLogicErr.LogicErr.All;
	p2ScreenObj->SysBitExpand = HardInfo.SysBitExpand.All;
    return 1;
}


//25 板载模拟量界面 初始化一次后 根据选择开关读或写
bool HMI_AdOnboard(void)
{
    Screen_AdOnboard_Typedef * p = ExtHmi.p2Scr_AdOnboard;
	p->HMI_NOOROFF = g_Para.HmiConceal.Onboard;

	//未初始化 先初始化
    if(ExtHmi.PageInitialized != 1)
    {
        //AD原始值、电压值送到屏幕对象
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Digital_Input_Value[0] = g_ADCNs.CN1.OrgVal;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Actual_Value[0] = g_ADCNs.CN1.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Digital_Input_Value[1] = g_ADCNs.CN2.OrgVal;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Actual_Value[1] = g_ADCNs.CN2.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Digital_Input_Value[2] = g_ADCNs.CN9.OrgVal;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Actual_Value[2] = g_ADCNs.CN9.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Digital_Input_Value[3] = g_ADCNs.CN5.OrgVal;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Actual_Value[3] = g_ADCNs.CN5.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Digital_Input_Value[4] = g_ADCNs.CNREF.OrgVal;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Actual_Value[4] = g_ADCNs.CNREF.mV;

        //DA输出命令送到屏幕对象
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[0] = g_DACNs.CN11.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[1] = g_DACNs.CN7.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[2] = g_DACNs.CN4.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[3] = g_DACNs.CN3.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[4] = g_DACNs.CN10.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[5] = g_DACNs.CN6.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[6] = g_DACNs.CN12.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[7] = g_DACNs.CN8.mV;

        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 根据选择开关读或写
    {
        //先实现显示
        //AD原始值、电压值送到屏幕对象
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Digital_Input_Value[0] = g_ADCNs.CN1.OrgVal;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Actual_Value[0] = g_ADCNs.CN1.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Digital_Input_Value[1] = g_ADCNs.CN2.OrgVal;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Actual_Value[1] = g_ADCNs.CN2.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Digital_Input_Value[2] = g_ADCNs.CN9.OrgVal;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Actual_Value[2] = g_ADCNs.CN9.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Digital_Input_Value[3] = g_ADCNs.CN5.OrgVal;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Actual_Value[3] = g_ADCNs.CN5.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Digital_Input_Value[4] = g_ADCNs.CNREF.OrgVal;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Actual_Value[4] = g_ADCNs.CNREF.mV;

        //DA输出命令送到屏幕对象
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[0] = g_DACNs.CN11.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[1] = g_DACNs.CN7.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[2] = g_DACNs.CN4.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[3] = g_DACNs.CN3.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[4] = g_DACNs.CN10.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[5] = g_DACNs.CN6.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[6] = g_DACNs.CN12.mV;
        ((Screen_AdOnboard_Typedef*)(ExtHmi.p2Data))->Analog_Out_Value[7] = g_DACNs.CN8.mV;
    }
    return 1;
}

//26 遥控器信息显示
bool HMI_Wireless(void)
{
	
	 WirelessStatus_Typedef* p = ExtHmi.p2Scr_Wireless;
    //未初始化 先初始化
    if(ExtHmi.PageInitialized != 1)
    {
        //*(WirelessStatus_Typedef*)ExtHmi.p2Data = ;
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 先完成显示
    {
		p->J7_X = 1 ;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->CommucationMsCnt = g_WHData.CommucationMsCnt;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->CountPerSecend = g_WHData.CountPerSecend;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->J7_X = g_WHData.XAxisOutput;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->J7_Y = g_WHData.YAxisOutput;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->J7_Z = g_WHData.ZAxisOutput;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->J41 = g_WHData.J41;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->J42 = g_WHData.J42;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->J43 = g_WHData.J43;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->J44 = g_WHData.J44;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.WirelessSleep = g_WHData.WirelessSleep;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J10 = g_WHData.J10;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J12 = g_WHData.J12;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J13 = g_WHData.J13;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J15 = g_WHData.J15;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J16 = g_WHData.J16;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J18 = g_WHData.J18;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J19 = g_WHData.J19;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J20 = g_WHData.J20;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J21 = g_WHData.J21;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J23 = g_WHData.J23;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J24 = g_WHData.J24;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J25 = g_WHData.J25;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J26 = g_WHData.J26;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J27 = g_WHData.J27;
        ((WirelessStatus_Typedef*)ExtHmi.p2Data)->Bit.J28 = g_WHData.J28;
    }
    return 1;
}

//27 通信状态显示更新
bool HMI_Show_Communication(void)
{
	Screen_LinkStaus_Typedef* p = ExtHmi.p2Scr_LinkStaus;
	//成功计数
	p->OkCnt.WHA			= EQPMsts.WHA.CommOkCnt;
	p->OkCnt.HMI			= EQPSlvs.HMI.CommOkCnt;
	p->OkCnt.BMS			= EQPMsts.BMS_CommConv.CommOkCnt;
	p->OkCnt.CommConv		= EQPMsts.CommConv.CommOkCnt;
	p->OkCnt.AD				= EQPMsts.AD.CommOkCnt;
	p->OkCnt.MagF			= EQPMsts.MagF.CommOkCnt;
	p->OkCnt.MagB			= EQPMsts.MagB.CommOkCnt;
	p->OkCnt.MagL			= EQPMsts.MagL.CommOkCnt;
	p->OkCnt.MagR			= EQPMsts.MagR.CommOkCnt;
	p->OkCnt.RFID			= EQPMsts.RFID.CommOkCnt;
	p->OkCnt.CommConv		= EQPMsts.CommConv.CommOkCnt;
	
	p->OkCnt.SerialExp		= EQPMsts.SerialExp.CommOkCnt;
	p->OkCnt.DSP			= EQPSlvs.DSP.CommOkCnt;
	p->OkCnt.Voice			= EQPMsts.Voice.CommOkCnt;
	p->OkCnt.PGVDown		= EQPMsts.PGVDownward.CommOkCnt;
	p->OkCnt.PGVUp			= EQPMsts.PGVUpward.CommOkCnt;
	
	
	p->OkCnt.PGVUpward_F	= EQPMsts.PGVUpward_F.CommOkCnt;
	p->OkCnt.PGVUpward_B	= EQPMsts.PGVUpward_B.CommOkCnt;
	p->OkCnt.PGVDownward_F	= EQPMsts.PGVDownward_F.CommOkCnt;
	p->OkCnt.PGVDownward_B	= EQPMsts.PGVDownward_B.CommOkCnt;
	p->OkCnt.AD1			= EQPMsts.AD1.CommOkCnt;
    p->OkCnt.DA             = EQPMsts.DA.CommOkCnt;
	p->OkCnt.CommConv1		= EQPMsts.CommConv.CommOkCnt;
	p->OkCnt.CommConv2		= EQPMsts.CommConv2.CommOkCnt;
	

	//超时计数
    p->TimeOutCnt.WHA		= EQPMsts.WHA.TimeOutMs;
	p->TimeOutCnt.HMI		= EQPSlvs.HMI.TimeOutMs;
	p->TimeOutCnt.BMS		= EQPMsts.BMS_CommConv.TimeOutMs;
	p->TimeOutCnt.CommConv 	= EQPMsts.CommConv.TimeOutMs;
	p->TimeOutCnt.AD		= EQPMsts.AD.TimeOutMs;
	p->TimeOutCnt.MagF		= EQPMsts.MagF.TimeOutMs;
	p->TimeOutCnt.MagB		= EQPMsts.MagB.TimeOutMs;
	p->TimeOutCnt.MagL		= EQPMsts.MagL.TimeOutMs;
	p->TimeOutCnt.MagR		= EQPMsts.MagR.TimeOutMs;
	p->TimeOutCnt.RFID		= EQPMsts.RFID.TimeOutMs;
	p->TimeOutCnt.PGVUp 	= EQPMsts.PGVUpward.TimeOutMs;
	p->TimeOutCnt.SerialExp = EQPMsts.SerialExp.TimeOutMs;
	p->TimeOutCnt.DSP		= EQPSlvs.DSP.TimeOutMs;
	p->TimeOutCnt.Voice 	= EQPMsts.Voice.TimeOutMs;
	p->TimeOutCnt.PGVDown 	= EQPMsts.PGVDownward.TimeOutMs;
	
	p->TimeOutCnt.PGVUpward_F 	= EQPMsts.PGVUpward_F.TimeOutMs;
	p->TimeOutCnt.PGVUpward_B 	= EQPMsts.PGVUpward_B.TimeOutMs;
	p->TimeOutCnt.PGVDownward_F = EQPMsts.PGVDownward_F.TimeOutMs;
	p->TimeOutCnt.PGVDownward_B = EQPMsts.PGVDownward_B.TimeOutMs;
	p->TimeOutCnt.AD1 			= EQPMsts.AD1.TimeOutMs;
    p->TimeOutCnt.DA            = EQPMsts.DA.TimeOutMs;
	p->TimeOutCnt.CommConv1 	= EQPMsts.CommConv.TimeOutMs;
	p->TimeOutCnt.CommConv2 	= EQPMsts.CommConv2.TimeOutMs;

	//舵轮传感器在线状态
    p->AngleNum = *((u32*)(&(EQP_CommConv.RD[0])));

	return 1;
}


//28 io状态界面 先直接显示
bool HMI_IOStatus(void)
{
    //char i = 0;
    //界面对象首地址指针初始化
    //IO_List* p2ScreenObj = (IO_List*)ExtHmi.p2Data;


    //将IO_DHR开始的8个short内拷贝到ExtHmi.p2Data开始的8个short内存中
    memcpy(ExtHmi.p2Data,IO_DHR,8*2);
    
    
    return 1;
}

//29驱动器信息界面 进去就写
bool HMI_ServoStatus(void)
{
    char i = 0;
	char N = ExtHmi.p2HMI_CommObj->SubIndex;			//翻页后，索引自加
    char SvoNum = MIN(HMI_Servo_MaxNum, SVO_MAXNUM);	//取最小值
    //界面对象首地址指针初始化
    CanopenStatus_Typedef* p2ScreenObj = (CanopenStatus_Typedef*)ExtHmi.p2Data;

    p2ScreenObj->CanopenStep = CanMaster.CanopenStep;
    p2ScreenObj->MasterStatus = CanMaster.MasterStatu;
    p2ScreenObj->SlaveNum = CanMaster.SlaveOnLineNum;

    for (i=0;i<SvoNum;i++)
    {
        p2ScreenObj->Servo[i].Actual_Voltage 	= SVO[i+N*SvoNum].Voltage;
        p2ScreenObj->Servo[i].Actual_Current 	= SVO[i+N*SvoNum].Current;
        p2ScreenObj->Servo[i].Actual_Temp 		= SVO[i+N*SvoNum].Temperature;
        p2ScreenObj->Servo[i].Actual_Speed 		= SVO[i+N*SvoNum].Speed;
        p2ScreenObj->Servo[i].SlaveStatus 		= SVO[i+N*SvoNum].SlaveStatus;

        p2ScreenObj->Servo[i].Control_Word 		= SVO[i+N*SvoNum].CMDCtrl;
        p2ScreenObj->Servo[i].Control_Speed		= SVO[i+N*SvoNum].CMDSpeed;

        p2ScreenObj->Servo[i].Status_Word 		= SVO[i+N*SvoNum].StatusWord;
        p2ScreenObj->Servo[i].SlaveErrCounter 	= SVO[i+N*SvoNum].CommErrCnt;
        p2ScreenObj->Servo[i].Event_Word 		= SVO[i+N*SvoNum].EventWord;
        p2ScreenObj->Servo[i].Faltch_Code 		= SVO[i+N*SvoNum].FaultCode;
		
		
		if(i+N*SvoNum >= SVO_MAXNUM)
		{
		p2ScreenObj->Servo[i].Actual_Voltage 	= \
		p2ScreenObj->Servo[i].Actual_Current 	= \
		p2ScreenObj->Servo[i].Actual_Temp 		= \
		p2ScreenObj->Servo[i].Actual_Speed 		= \
		p2ScreenObj->Servo[i].SlaveStatus 		= \

		p2ScreenObj->Servo[i].Control_Word 		= \
		p2ScreenObj->Servo[i].Control_Speed		= \

		p2ScreenObj->Servo[i].Status_Word 		= \
		p2ScreenObj->Servo[i].SlaveErrCounter 	= \
		p2ScreenObj->Servo[i].Event_Word 		= \
		p2ScreenObj->Servo[i].Faltch_Code 		= 0;
		}
    }
    
    return 1;
}


//30 手动控车
//进入画面 先关闭所有值
//之后根据使能开关判读是否将屏幕输入到系统
//退出画面时将屏幕数据 和 系统控制命令清零
//临时雷达避障使能被强制打开了？
bool HMI_ManualCtrl(void)
{
    //未初始化 先初始化
    if(ExtHmi.PageInitialized != 1)
    {
        //*((Screen_ManualControl_Typedef*)(ExtHmi.p2Data)) = {0};
        //根据Screen_ManualControl_Typedef对象大小清除内存
        memset((Screen_ManualControl_Typedef*)(ExtHmi.p2Data),0,sizeof(Screen_ManualControl_Typedef));
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 根据使能开关判读是否将屏幕输入到系统
    {
        //获取挡位
        g_ChannelSwitch.CtrlCmd.WalkGear = ((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->WalkGear;
        g_ChannelSwitch.CtrlCmd.LiftGear = ((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->LiftSpeed;
        
        //手动控车使能
        if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.Enable)
        {
            //获取模式命令
            g_ChannelSwitch.CtrlCmd.Mode = (ACCMode_TypeDef)((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->RunMode;
            //方向按钮 和 前向速度命令 操作摇杆命令
            if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->WalkSpeed && g_ChannelSwitch.CtrlCmd.WalkGear)
            {
                u16 vtemp = ((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->WalkSpeed;

                g_ChannelSwitch.CtrlCmd.Vxyz.x = 0;
                g_ChannelSwitch.CtrlCmd.Vxyz.y = 0;
                g_ChannelSwitch.CtrlCmd.Vxyz.z = 0;

                if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.LF)
                {   
                    g_ChannelSwitch.CtrlCmd.Vxyz.x = -vtemp;
                    g_ChannelSwitch.CtrlCmd.Vxyz.y = vtemp;
                }
                else if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.F)
                {
                    g_ChannelSwitch.CtrlCmd.Vxyz.y = vtemp;
                }
                else if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.RF)
                {
                    g_ChannelSwitch.CtrlCmd.Vxyz.x = vtemp;
                    g_ChannelSwitch.CtrlCmd.Vxyz.y = vtemp;
                }
                else if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.L)
                {
                    g_ChannelSwitch.CtrlCmd.Vxyz.x = -vtemp;
                }
                else if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.R)
                {
                    g_ChannelSwitch.CtrlCmd.Vxyz.x = vtemp;
                }
                else if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.LB)
                {
                    g_ChannelSwitch.CtrlCmd.Vxyz.x = -vtemp;
                    g_ChannelSwitch.CtrlCmd.Vxyz.y = -vtemp;
                }
                else if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.B)
                {
                    g_ChannelSwitch.CtrlCmd.Vxyz.y = -vtemp;
                }
                else if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.RB)
                {
                    g_ChannelSwitch.CtrlCmd.Vxyz.x = vtemp;
                    g_ChannelSwitch.CtrlCmd.Vxyz.y = -vtemp;
                }
                else if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.Anticlockwise)
                {
                    g_ChannelSwitch.CtrlCmd.Vxyz.z = vtemp;
                }
                else if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.Clockwise)
                {
                    g_ChannelSwitch.CtrlCmd.Vxyz.z = -vtemp;
                }
            }
            else
            {
                g_ChannelSwitch.CtrlCmd.Vxyz.x = 0;
                g_ChannelSwitch.CtrlCmd.Vxyz.y = 0;
                g_ChannelSwitch.CtrlCmd.Vxyz.z = 0;
            }

            //获取升降按钮值
            //有升降挡位时才能操作
            if(g_ChannelSwitch.CtrlCmd.LiftGear)
            {
                if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.Up)
                {
                    g_ChannelSwitch.CtrlCmd.Button.LiftUp = 1;
                    g_ChannelSwitch.CtrlCmd.Button.LiftDown = 0;
                }
                else if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.Down)
                {
                    g_ChannelSwitch.CtrlCmd.Button.LiftUp = 0;
                    g_ChannelSwitch.CtrlCmd.Button.LiftDown = 1;
                }
                else
                {
                    g_ChannelSwitch.CtrlCmd.Button.LiftUp = 0;
                    g_ChannelSwitch.CtrlCmd.Button.LiftDown = 0;
                }
            }

            //获取喇叭按钮值
            if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.Horn)
            {
                g_ChannelSwitch.CtrlCmd.Button.Horn = 1;
            }
            else
            {
                g_ChannelSwitch.CtrlCmd.Button.Horn = 0;
            }

            //获取照明按钮值
            if(((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.Lighting)
            {
                g_ChannelSwitch.CtrlCmd.Button.Lighting = 1;
            }
            else
            {
                g_ChannelSwitch.CtrlCmd.Button.Lighting = 0;
            }
        }
        else
        {
            memset((Screen_ManualControl_Typedef*)(ExtHmi.p2Data),0,sizeof(Screen_ManualControl_Typedef));
            //关闭所有值
            g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Null;
            g_ChannelSwitch.CtrlCmd.Vxyz.x = 0;
            g_ChannelSwitch.CtrlCmd.Vxyz.y = 0;
            g_ChannelSwitch.CtrlCmd.Vxyz.z = 0;
            g_ChannelSwitch.CtrlCmd.WalkGear = 1;
            g_ChannelSwitch.CtrlCmd.LiftGear = 1;
            //关闭所有按钮
             //memset(&g_ChannelSwitch.CtrlCmd.Button,0,sizeof(ACC_FunctionButton_TypeDef));
            g_ChannelSwitch.CtrlCmd.Button.LiftUp   = 0;
            g_ChannelSwitch.CtrlCmd.Button.LiftDown = 0;
            g_ChannelSwitch.CtrlCmd.Button.Horn     = 0;
            g_ChannelSwitch.CtrlCmd.Button.Lighting = 0;

        }
       
    }
    return 1;
}

//31 一般参数  超低车项目参数放这里
bool HMI_ParaSetPage_One(void)
{
	Screen_GeneralParameter_TypeDef* p = ExtHmi.p2Scr_GeneralParameter;
		
	p->HMI_NOOROFF = g_Para.HmiConceal.GeneralParameter_Switch;
	
	//未初始化 先初始化
    if(ExtHmi.PageInitialized != 1)
    {
	    p->diffALarmJudge = g_Para.Lift.DiffMax;
        p->diffALarmJudge = g_LiftData.WarningPara.DiffMax;
		p->in_SmoothVxyzAcc  = g_Para.Other.in_SmoothVxyzAcc;
        p->in_SmoothVxyzDec = g_Para.Other.in_SmoothVxyzDec;
        p->in_Temperature_Environment_FanOff = g_Para.Other.in_Temperature_Environment_FanOff;
        p->overweightAlarmJudge = g_LoadDetection.Errpar.OverLoad;
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)
        {
            p->diffALarmJudge = g_Para.Lift.DiffMax;
            p->diffALarmJudge = g_LiftData.WarningPara.DiffMax;
            p->in_SmoothVxyzAcc = g_Para.Other.in_SmoothVxyzAcc;
            p->in_SmoothVxyzDec = g_Para.Other.in_SmoothVxyzDec;
            p->in_Temperature_Environment_FanOff = g_Para.Other.in_Temperature_Environment_FanOff;
            p->overweightAlarmJudge = g_LoadDetection.Errpar.OverLoad;
        }
        else if (ExtHmi.p2HMI_CommObj->FucBit.Write)
        {
            g_Para.Lift.DiffMax = p->diffALarmJudge;
            g_LiftData.WarningPara.DiffMax = p->diffALarmJudge;
            g_Para.Other.in_SmoothVxyzAcc = p->in_SmoothVxyzAcc;
            g_Para.Other.in_SmoothVxyzDec = p->in_SmoothVxyzDec;
            ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
        }
	    
    }
    return 1;
}

//32 轮系参数
bool HMI_WheelParaPage_One(void)
{
	Screen_WheelParameter_TypeDef* p = ExtHmi.p2Scr_WheelPParameter;
	p->HMI_NOOROFF = g_Para.HmiConceal.WheelParameter_Switch;

	p->PositiveObliqueAngle = g_BMSData.BMS_Read.SOC; //测试写入
	
	return 1;
};


//33 读取升降标定界面
bool LiftCalibration2hmi(void)
{
    char i = 0;
    Screen_HeightInstall_Typedef* p = ExtHmi.p2Scr_HeightInstall;

    p->HMI_NOOROFF = g_Para.HmiConceal.HeightCalibration_Switch;

    p->Height_Set_LowValue = g_Para.Lift.SetLow;
    p->Height_Set_HighValue = g_Para.Lift.SetHigh;
    p->Height_Limit_LowValue = g_Para.Lift.LimitDown;
    p->Height_Limit_HighValue = g_Para.Lift.LimitUp;

    for (i = 0; i < LIFT_HG_MAX; i++)
    {
        p->Height_LowValue_Calibration[i] = g_Para.Lift.SetLowOrg[i];
        p->Height_HighValue_Calibration[i] = g_Para.Lift.SetHighOrg[i];
    }

	memcpy(p->SpeedPlanUp,g_Para.Lift.SpeedPlanUp,sizeof(g_Para.Lift.SpeedPlanUp));
	memcpy(p->SpeedPlanDown,g_Para.Lift.SpeedPlanDown,sizeof(g_Para.Lift.SpeedPlanDown));

	return 1;
}

//读取标定数据
bool hmi2LiftCalibration(void)
{
    char i = 0;
    Screen_HeightInstall_Typedef* p = ExtHmi.p2Scr_HeightInstall;

    g_Para.Lift.SetLow = p->Height_Set_LowValue;
    g_Para.Lift.SetHigh = p->Height_Set_HighValue;
    g_Para.Lift.LimitDown = p->Height_Limit_LowValue;
    g_Para.Lift.LimitUp = p->Height_Limit_HighValue;

    for (i = 0; i < LIFT_HG_MAX; i++)
    {
        g_Para.Lift.SetLowOrg[i] = p->Height_LowValue_Calibration[i];
        g_Para.Lift.SetHighOrg[i] = p->Height_HighValue_Calibration[i];
    }

	memcpy(g_Para.Lift.SpeedPlanUp,p->SpeedPlanUp,sizeof(g_Para.Lift.SpeedPlanUp));
	memcpy(g_Para.Lift.SpeedPlanDown,p->SpeedPlanDown,sizeof(g_Para.Lift.SpeedPlanDown));

    return 1;
}

//读取标定数据
bool HMI_ReadLiftCalibration(void)
{
    //未初始化 先初始化
    if (ExtHmi.PageInitialized != 1)
    {
        LiftCalibration2hmi();
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)
		{
            LiftCalibration2hmi();
		}
		else if (ExtHmi.p2HMI_CommObj->FucBit.Write)
		{
			hmi2LiftCalibration();
			ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
		}
    }
    return 1;
}


//34 角度标定
bool HMI_AngleSet(void)
{
    char i = 0;
	Screen_AngleSet_Typedef* p = ExtHmi.p2Scr_AngleSet;
    MR_ManualCtrl_TypeDef* p2MRM = &g_MR_ManualCtrl;//指向模组单动标定对象
	p->HMI_NOOROFF = g_Para.HmiConceal.AngleCalibration_Switch;
	
    //未初始化 先初始化
    if (ExtHmi.PageInitialized != 1)
    {
		memset(ExtHmi.p2Scr_AngleSet, 0, sizeof(Screen_AngleSet_Typedef));
        for (i = 0; i < g_Para.WheelAngle.Num; i++)
        {
            ExtHmi.p2Scr_AngleSet->ActualAngle[i] = g_WheelAngle[i].OrgRes*100;//原始值
            ExtHmi.p2Scr_AngleSet->AngleSet[i] = g_Para.WheelAngle.Offset[i] * 100;//标定参数
            ExtHmi.p2Scr_AngleSet->ActualAngle1[i] = g_WheelAngle[i].Res*100;//标定后的值
        }
        ExtHmi.p2Scr_AngleSet->CalibrateButton = 0;
        p2MRM->Select = 0;
        p2MRM->SpeedMax = 0;
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        //仅显示 不受读写控件影响
        for (i = 0; i < g_Para.WheelAngle.Num; i++)
        {
            ExtHmi.p2Scr_AngleSet->ActualAngle[i] = g_WheelAngle[i].OrgRes*100;//原始值
            ExtHmi.p2Scr_AngleSet->ActualAngle1[i] = g_WheelAngle[i].Res*100;//标定后的值
        }
        
        //根据读写控件决定读还是写
		if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)
        {
            for (i = 0; i < g_Para.WheelAngle.Num; i++)
            {
                ExtHmi.p2Scr_AngleSet->AngleSet[i] = g_Para.WheelAngle.Offset[i] * 100;//标定参数
            }
        }
        else if (ExtHmi.p2HMI_CommObj->FucBit.Write)
        {
            for (i = 0; i < g_Para.WheelAngle.Num; i++)
            {
                g_Para.WheelAngle.Offset[i] = ExtHmi.p2Scr_AngleSet->AngleSet[i]/100.0f;
            }
            ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
        }
        else
            __asm("NOP");

        //下面的都是屏幕上的按钮操作参数响应
        //角度标定按钮响应
        if(ExtHmi.p2Scr_AngleSet->CalibrateButton)
		{
			g_SysApi.WheelAngleZeroSet = 1;
            ExtHmi.p2Scr_AngleSet->CalibrateButton = 0;
		}

        /*模组单动标定按钮、组号、速度等设定输入*/
        p2MRM->En = ExtHmi.p2Scr_AngleSet->SetButton;//使能
        p2MRM->Select = ExtHmi.p2Scr_AngleSet->ModuleNumber;//组号
        p2MRM->SpeedMax = ExtHmi.p2Scr_AngleSet->SpeedSetting;//最大速度设置
		p->CurrentSpeed = p2MRM->Speed;
        
    }
    return 1;
}

//35激光测距标定
bool HMI_DistanceSetPage(void)
{
	Screen_LDRCalibration_TypeDef* p = ExtHmi.pScr_LDRCalibration;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.LaserCalibration_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	
	return 1;
};

//38雷赛伺服标定
bool HMI_LeadShineSetPage(void)
{
	Screen_ReinforcedCalibration_TypeDef* p = ExtHmi.p2Scr_ReinforcedCalibration;
		
	p->HMI_NOOROFF = !g_Para.HmiConceal.LeadshineServoCalibration_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;
};

//39传感器标定读取安装标定参数界面 初始化一次后 一直读取
bool HMI_ReadInst(void)
{
    Screen_PGVCalibration_TypeDef* p = ExtHmi.p2Scr_PGVCalibration_TypeDef;
		
	p->HMI_NOOROFF = g_Para.HmiConceal.PGVCalibration_Switch;
	//未初始化 先初始化
    if(ExtHmi.PageInitialized != 1)
    {
		*(Para_Inst_TypeDef*)ExtHmi.p2Data = g_Para.Inst;
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
		g_Para.Inst = *(Para_Inst_TypeDef*)ExtHmi.p2Data;
    }
    return 1;
}

//40 读取虚拟里程计数据
bool HMI_ReadVirtualOdo(void)
{
    Screen_OdometerCalibration_TypeDef* p = ExtHmi.p2Scr_Odometer;
		
	p->HMI_NOOROFF = g_Para.HmiConceal.Odometry_Switch;
	//非多源控制更新
    //未初始化 先初始化
    if(ExtHmi.PageInitialized != 1)
    {
		*(Para_Obo_StDef*)ExtHmi.p2Data = g_Para.Obo;
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
		g_Para.Obo = *(Para_Obo_StDef*)ExtHmi.p2Data;
    }
    return 1;
}


//41控制系数参数界面 初始化一次后 一直读取
bool HMI_ReadCtrlCoef(void)
{
	Screen_ControlCoeff_TypeDef* p = ExtHmi.p2Scr_ControlCoeff;

    p->HMI_NOOROFF = g_Para.HmiConceal.ControlCoeff_Switch;
    //未初始化 先初始化
    if(ExtHmi.PageInitialized != 1)
    {
		//*(CtrlCoef_TypeDef*)ExtHmi.p2Data = g_Para.CtrlCoef;
		p->k_TraceX=g_Para.CtrlCoef.k_TraceX;
		p->k_TraceXMax=g_Para.CtrlCoef.k_TraceXMax;
		p->k_TraceXMin=g_Para.CtrlCoef.k_TraceXMin;
		p->k_TraceZ=g_Para.CtrlCoef.k_TraceZ;
		p->k_TraceZMax=g_Para.CtrlCoef.k_TraceZMax;
		p->k_TraceZMin=g_Para.CtrlCoef.k_TraceZMin;
//		 p->Electromagnet = g_Para.Electromagnet;
		
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
       g_Para.CtrlCoef = *(CtrlCoef_TypeDef*)ExtHmi.p2Scr_ControlCoeff;
		
//		if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)
//		{
//           p->Electromagnet = g_Para.Electromagnet;
//		}
//		else if (ExtHmi.p2HMI_CommObj->FucBit.Write)
//		{
//			g_Para.Electromagnet; = p->Electromagnet;
//			ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
//		}
    }

    return 1;
}

//42 PID标定
bool HMI_PidSetPage(void)
{
	Screen_PID_TypeDef* p = ExtHmi.p2Scr_PID;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.PID_Switch;
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;
	
};
//43 位置雷达标定
bool HMI_RadarLocationPage(void)
{
	Screen_PositionRadar_TypeDef* p = ExtHmi.p2Scr_PositionRadar;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.PositionRadarCalibration_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;
	
};


//44 位置雷达速度标定
bool HMI_SpeedThresholdPage(void)
{
	Screen_PositionRadarSpeed_TypeDef* p = ExtHmi.p2Scr_PositionRadarSpeed;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.PositionRadarSpeedCalibration_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;
	
};


//45 电量电压关联参数
bool HMI_VoltageParaPage(void)
{
	Screen_BVRelated_TypeDef* p = ExtHmi.p2Scr_BVRelated;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.VoltageSOC_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;
};


//46 更多参数  读取故障检出参数
bool HMI_ReadEmgJudgePara(void)
{
    Screen_MoreParameters_TypeDef * p=ExtHmi.p2Scr_MoreParameters;
	p->HMI_NOOROFF = g_Para.HmiConceal.MoreParameters_Switch;
	
    //非多源控制更新
    //未初始化 先初始化
    if(ExtHmi.PageInitialized != 1)
    {
        p->TimeOutJudge = g_Para.EmgJudge.TimeOutJudge;
		p->ModeSwitchTimeOutJudge = g_Para.EmgJudge.ModeSwitchTimeOutJudge;
		p->WheelAngleAlarmRight = g_Para.EmgJudge.WheelAngleAlarmRight;
		p->WheelAngleAlarmLeft = g_Para.EmgJudge.WheelAngleAlarmLeft;
		p->MagOutLineTime = g_Para.EmgJudge.MagOutLineTime;
		p->MagOutLineDistance = g_Para.EmgJudge.MagOutLineDistance;
		p->TimeOutDsp = g_Para.EmgJudge.TimeOutDsp;
		p->ColourOutLineTime = g_Para.EmgJudge.ColourOutLineTime;
		p->TimeOutSlam = g_Para.EmgJudge.TimeOutSlam;
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        g_Para.EmgJudge.TimeOutJudge = p->TimeOutJudge;
		g_Para.EmgJudge.ModeSwitchTimeOutJudge = p->ModeSwitchTimeOutJudge;
		g_Para.EmgJudge.WheelAngleAlarmRight = p->WheelAngleAlarmRight;
		g_Para.EmgJudge.WheelAngleAlarmLeft = p->WheelAngleAlarmLeft;
		g_Para.EmgJudge.MagOutLineTime = p->MagOutLineTime;
		g_Para.EmgJudge.MagOutLineDistance = p->MagOutLineDistance;
		g_Para.EmgJudge.TimeOutDsp = p->TimeOutDsp;
		g_Para.EmgJudge.ColourOutLineTime = p->ColourOutLineTime;
		g_Para.EmgJudge.TimeOutSlam = p->TimeOutSlam;
    }
    return 1;
}

//47 角度  舵轮角度显示更新
bool HMI_Show_WheelAngle(void)
{
    char i = 0;
    Screen_Angle_Typedef* p = ExtHmi.p2Scr_Angle;
	
    p->HMI_NOOROFF = g_Para.HmiConceal.AngleShow_Switch;
    if (WheelAngleNum_MAX > HMI_AngleSensor_MaxNum)
        return 0;

    for (i = 0; i < WheelAngleNum_MAX; i++)
    { 
		p->ActualAngle[i] = g_WheelAngle[i].Res * 100;
        p->TgtAngle[i] = g_MotorResolving.TgtAng[i] * 100;
    }
	
    return 1;
}

//48 高度界面  压力界面调用同一个函数
bool HMI_High_Page(void)
{
	Screen_High_Typedef* p = ExtHmi.p2Scr_High;
	unsigned  char i = 0;
	p->HMI_NOOROFF = g_Para.HmiConceal.HighShow_Switch;  //0显示压力 1压力，流量比，负载，中心坐标，2压力换成正三角，3压力换成四点液压 
	p->HMI_NOOROFF1 = g_Para.HmiConceal.HighButton_Switch;  //0显示单动按钮，温度，补偿转速，1关闭显示，2按钮换成正三角
	for (i = 0; i < LIFT_HG_MAX; i++)
		p->High_Data[i] = g_LiftData.Height.Cal[i];

	//液压相关临时参数增加在此页面  XWR 重心坐标显示错误 需要处理
	p->CoreX  = g_LoadDetection.CoreX;
	p->CoreY  = g_LoadDetection.CoreY;
	p->Weight = g_LoadDetection.WeightShow    * 100;
	p->FL_cal = g_LoadDetection.Sensor[0].Cal *  10;
	p->RL_cal = g_LoadDetection.Sensor[1].Cal *  10;
	p->B_cal  = g_LoadDetection.Sensor[2].Cal *  10;

	p->tempeature = g_Fan.out_Temperature_Environment;

    if(ExtHmi.PageInitialized != 1)
    {
		
		//参数复位
		p->Bit_Para.All        = 0;
		p->HMI_Temp_Rpm_Input  = 0;
		//标志位复位
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        //响应点动按钮
        g_LiftData.CtrlPara.BitPara.Bits.Tst_Temp_LF_UP = p->Bit_Para.Bits.Tst_Temp_LF_UP_HMI ? 1 : 0;
        g_LiftData.CtrlPara.BitPara.Bits.Tst_Temp_RF_UP = p->Bit_Para.Bits.Tst_Temp_RF_UP_HMI ? 1 : 0;
        g_LiftData.CtrlPara.BitPara.Bits.Tst_Temp_B_UP = p->Bit_Para.Bits.Tst_Temp_B_UP_HMI ? 1 : 0;

        //输入转速补偿
        g_LiftData.CtrlPara.Tst_Temp_Rpm = p->HMI_Temp_Rpm_Input;

        //手动操作电磁阀下降 hmi界面对象定义好后改在下面
        g_LiftData.Cmd.Bits.ManualDown_LF = p->Bit_Para.Bits.Tst_Temp_LF_Down_HMI ? 1 : 0;
        g_LiftData.Cmd.Bits.ManualDown_RF = p->Bit_Para.Bits.Tst_Temp_RF_Down_HMI ? 1 : 0;
        g_LiftData.Cmd.Bits.ManualDown_B = p->Bit_Para.Bits.Tst_Temp_B_Down_HMI ? 1 : 0;
    }
	
    return 1;
}

//49 悬挂高度
bool HMI_HangHeightPage(void)
{
	Screen_HangingHeight_TypeDef* p = ExtHmi.p2Scr_HangingHeight;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.Suspension_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;
};
//50 支腿高度
bool HMI_LegHeightPage(void)
{
	Screen_LegHeight_TypeDef* p = ExtHmi.p2Scr_LegHeight;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.Outrigger_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;	
};
//51 激光测距
bool HMI_LaserRangePage(void)
{
	Screen_LaserRanging_TypeDef* p = ExtHmi.p2Scr_LaserRanging;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.LaserShow_Switch; //屏幕显示是否开启
	
	p->LaserFront[0] = 1;
	
	p->LaserBack[0]	 = 1;
	
	p->LaserLeft[0]	 = 1;
	
	p->LaserRight[0] = 1;
	
	p->LaserFront[1] = 1;
	
	p->LaserBack[1]	 = 1;
	
	p->LaserLeft[1]	 = 11;
	
	p->LaserRight[1] = 1;
	return 1;
};
//52超声波测距
bool HMI_UltrasonicDistancePage(void)
{
	Screen_USRanging_TypeDef* p = ExtHmi.p2Scr_USRanging;
	unsigned  char i = 0;
	p->HMI_NOOROFF = !g_Para.HmiConceal.Ultrasonic_Switch; //屏幕显示是否开启
	for (i = 0; i < NUMMAX; i++)
	{
		p->ultrasonic_Data[2*i] = g_ShelfAutadj.in_DistanceLeft[i];
	}
	for (i = 0; i < NUMMAX; i++)
	{
		p->ultrasonic_Data[2*i + 1] = g_ShelfAutadj.in_DistanceRight[i];
	}
		return 1;	
};


//53压力传感器无函数： 直接调用了高度界面

//54模拟量采集模块1
bool HMI_AnalogModulePage(void)
{
	Screen_AnalogCollection_TypeDef* p = ExtHmi.p2Scr_AnalogCollection;
	unsigned  char i = 0;

    p->HMI_NOOROFF = g_Para.HmiConceal.AD_Switch;
    if (g_Para.EQPCommSource.EN.SerialExp == 1)
    {
        if (g_Para.SerialExpObjSource.AD == 1)
        {
            for (i = 0; i < 8; i++)
                p->ADDAData[i] = (&(EQP_SerialExp.p2Data.ADDAData->DAQM4202_Ch0))[i];
        }
        if (g_Para.SerialExpObjSource.AD1 == 1)
        {
            for (i = 0; i < 8; i++)
                p->ADDAData1[i] = (&(EQP_SerialExp.p2Data.ADDA1Data->DAQM4202_Ch0))[i];
        }
    }
    if (g_Para.EQPCommSource.EN.AD == 1)
    {
        for (i = 0; i < 8; i++)
            p->ADDAData[i] = EQP_AD.RD[i];
    }
    if (g_Para.EQPCommSource.EN.AD1 == 1)
    {
        for (i = 0; i < 8; i++)
            p->ADDAData1[i] = EQP_AD1.RD[i];
    }
	
	return 1;		
};
//55模拟量输出模块
bool HMI_AnalogOutPutModulePage(void)
{
	Screen_AnalogOut_TypeDef* p = ExtHmi.p2Scr_AnalogOut;
    char i = 0;
	
	p->HMI_NOOROFF = g_Para.HmiConceal.DA_Switch; //屏幕显示是否开启

    for (i = 0; i < 8; i++)
        p->OutputData[i] = EQP_DA.WD[i];
	
	return 1;	
};

//56GPS数据
bool HMI_GpsDataPage(void)
{
	Screen_GPS_TypeDef* p = ExtHmi.p2Scr_GPS;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.GPS_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;
};
//57激光导航
bool HMI_LaserNavigationPage(void)
{
	Screen_LaserNavigation_TypeDef* p = ExtHmi.p2Scr_LaserNavigation;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.LaserNavigation_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;	
};

//58 调度系统信息 只显示部分内容,界面待完善
bool HMI_Dsp(void)
{
    //char i = 0;
    //界面对象首地址指针初始化
    u16 Num = 122*2;
	DispatchSystemCmd_Typedef * p = ExtHmi.p2Scr_DispatchSystemCmd;
	
	 p->HMI_NOOROFF = g_Para.HmiConceal.DispatchingSystem_Switch;
	
    if(Num < sizeof(ExtHmi.p2HMI_CommObj->Screen_Head))
	{}
        //memcpy(ExtHmi.p2Data,g_DHR_AGVInformation, Num);
    else
    {
        g_Faults.CodeRemind.Bit.Screen_HeadLimit = 1;
    }
    return 1;
}

//206自动运行参数界面 初始化一次后 一直读取
bool HMI_ReadAutoPara(void)
{
    AtuoRunStatus_Typedef * p = ExtHmi.p2Scr_AtuoRunStatus;
	 p->HMI_NOOROFF = g_Para.HmiConceal.AutoOperation_Switch;
	//未初始化 先初始化
    if (ExtHmi.PageInitialized != 1)
    {
		*(Para_Auto_StDef*)ExtHmi.p2Data = g_Para.Auto;
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
		g_Para.Auto = *(Para_Auto_StDef*)ExtHmi.p2Data;
    }
    return 1;
}

//60避障显示页面 四周障碍状态跟新显示、避障区域显示、避障区域设置
bool HMI_ReadObs(void)
{
    SafeSenorStatus_Typedef * p = ExtHmi.p2Scr_SafeSenorStatus_Typedef;
	p->HMI_NOOROFF = g_Para.HmiConceal.Obs_Switch;
	//四周障碍状态跟新显示
    ((SafeSenorStatus_Typedef*)(ExtHmi.p2Data))->RadarStatus.RadarStatus[0] = *(u16*)(&g_ROA.RadarObsSta);
    
    //避障区域显示
    ((SafeSenorStatus_Typedef*)(ExtHmi.p2Data))->RadarArea = (u16)g_ChannelSwitch.CtrlCmd.Button.ObsArea;

    //避障区域强制设置读取
    //未初始化 先初始化
    if(ExtHmi.PageInitialized != 1)
    {
		((SafeSenorStatus_Typedef*)(ExtHmi.p2Data))->RadarArea_Force = g_ChannelSwitch.ForceObsArea;
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        //设置区域无效时 不读取
        if(((SafeSenorStatus_Typedef*)(ExtHmi.p2Data))->RadarArea_Force != AccObsArea_Null)
        {
			g_ChannelSwitch.ForceObsArea = (AccObsArea_TypeDef)(((SafeSenorStatus_Typedef*)(ExtHmi.p2Data))->RadarArea_Force);
        }
    }

    return 1;
}

//61磁导航与RFID界面显示更新
bool HMI_Show_MagRFID(void)
{
    char i = 0;
    Screen_Navigation_Typedef* p = ExtHmi.p2Scr_Navigation;
    p->HMI_NOOROFF = g_Para.HmiConceal.MagneticNavigation_Switch; //屏幕显示是否开启
    if (MN_NUM > 4)
        return 0;
    //磁导航
    for (i = 0; i < MN_NUM; i++)
    {
        p->MagneticSensor_Data[i].MagneticSensor_Status = g_MNData[i].MN_Read.Status;
        p->MagneticSensor_Data[i].MagneticSensor_ReadNum = g_MNData[i].MN_Read.L_or_R_Sta;
        p->MagneticSensor_Data[i].MagneticSensor_Strength = g_MNData[i].MN_Read.Strength;
        p->MagneticSensor_Data[i].MagneticSensor_Location = g_MNData[i].MN_Read.Distance;
    }

    //RFID
    p->RFID_WriteStatus = EQP_RFID.p2Data->WriteState;
    p->RFID_ReadStatus = EQP_RFID.p2Data->ReadState;
    p->RFID_SiganlStatus = EQP_RFID.p2Data->Strong;
    p->RFID_Data1 = EQP_RFID.p2Data->Data1;
    p->RFID_Data2 = EQP_RFID.p2Data->Data2;

    return 1;
}

//62 雷赛伺服状态
bool HMI_LeadShineServoStatusPage(void)
{
	Screen_ReinforcedServo_TypeDef* p = ExtHmi.p2Scr_ReinforcedServo;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.LeadshineServo_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;	
}

//63 pgv界面显示更新
bool HMI_Show_PGV(void)
{
    char i = 0;
    PGV_TypeDef* p = &g_GroundPGV;
    Screen_PGV_Typedef* p2hmi = ExtHmi.p2Scr_PGV;
	p2hmi->HMI_NOOROFF = g_Para.HmiConceal.PGV_Switch;
    for (i = 0; i < HMI_PGV_Num; i++)
    {
        if (i == 0)
            p = &g_ToolPGV;
        else
            p = &g_GroundPGV;

        p2hmi->PGVSenorStatus[i].Run_Status = p->PGV_Source;
        p2hmi->PGVSenorStatus[i].Fault_Code = p->PGV_ErrorCode;
        p2hmi->PGVSenorStatus[i].System_Ms = p->PGV_HeartbeatCnt;
		
        p2hmi->PGVSenorStatus[i].CodeRead_Status = p->PGV_Read.Status;
        p2hmi->PGVSenorStatus[i].Code_Value = p->PGV_Read.Tag;
        p2hmi->PGVSenorStatus[i].X_Offset = p->PGV_Read.X;
        p2hmi->PGVSenorStatus[i].Y_Offset = p->PGV_Read.Y;
        p2hmi->PGVSenorStatus[i].Z_Offset = p->PGV_Read.Z;
    }

    return 1;
}

//64栈板相机
bool HMI_PalletCameraPage(void)
{
	Screen_Camera_TypeDef* p = ExtHmi.p2Scr_Camera;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.PalletCamera_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;
};


//65联动状态
bool HMI_UnionStatus(void)
{
	Screen_UnionStatus_Typedef* p = ExtHmi.p2Scr_UnionStatus;	
	p->HMI_NOOROFF = g_Para.HmiConceal.AsbShow_Switch;
	
	return 1;
}

//66光斑状态
bool HMI_LightSpotPage(void)
{
	Screen_LightSpot_TypeDef* p = ExtHmi.p2Scr_LightSpot;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.Facula_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;
};
	
//67位置定位雷达
bool HMI_PostionRadarPage(void)
{
	Screen_Position_TypeDef* p = ExtHmi.p2Scr_Position;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.PositioningRadar_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;	
};
	
//68倾角传感器
bool HMI_InclinometerPage(void)
{
	Screen_TiltSensor_TypeDef* p = ExtHmi.p2Scr_TiltSensor;
	
	p->HMI_NOOROFF = g_Para.HmiConceal.TiltSensor_Switch;//屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;
};
	
//69手摇脉冲器
bool HMI_HandPulserPage(void)
{
	Screen_Pulser_TypeDef* p = ExtHmi.p2Scr_Pulser;
	
	p->HMI_NOOROFF = g_Para.HmiConceal.ManualPulseGenerator_Switch; //屏幕显示是否开启
	p->test = g_BMSData.BMS_Read.SOC; //测试写入
	return 1;
};

//70BMS界面显示更新
bool HMI_Show_BMS(void)
{
    Screen_BMS_Typedef* p = ExtHmi.p2Scr_BMS;

    p->HMI_NOOROFF = g_Para.HmiConceal.BMSShow_Switch; //屏幕显示是否开启
    if (g_Para.EQPCommSource.EN.BMS_CommConv == 1)
    {
        *p = *((Screen_BMS_Typedef*)EQP_BMS_CommConv.p2Data);
        p->HOT_Current = EQP_BMS_CommConv.p2Data->HOT_Current / 10;

    }
    else if (g_Para.EQPCommSource.EN.BMS_CATL == 1)
    {
        *p = *((Screen_BMS_Typedef*)EQP_BMS_CATL.p2Data);
        p->TCC = EQP_BMS_CATL.RD[36] << 16 | EQP_BMS_CATL.RD[37];
        p->TDC = EQP_BMS_CATL.RD[38] << 16 | EQP_BMS_CATL.RD[39];
        p->ACC_T_DCHG = EQP_BMS_CATL.RD[42] << 16 | EQP_BMS_CATL.RD[43];
        p->ACC_T_CHG = EQP_BMS_CATL.RD[44] << 16 | EQP_BMS_CATL.RD[45];
        p->T_OP_TOT = EQP_BMS_CATL.RD[46] << 16 | EQP_BMS_CATL.RD[47];
        p->HOT_Current = EQP_BMS_CATL.p2Data->HOT_Current / 10;
    }
    else {}
			
    p->TimeOutMs = g_BMSData.BMS_Read.TimeOutMs;
    p->CommOkCnt = g_BMSData.BMS_Read.CommOkCnt;
    p->Voltage = g_BMSData.BMS_Read.Voltage * 10;
    p->Current = g_BMSData.BMS_Read.Current * 10;
    p->SOC = g_BMSData.BMS_Read.SOC * 10;
    p->SOH = g_BMSData.BMS_Read.SOH * 10;
    p->Alarm_Code = g_BMSData.BMS_Read.Alarm_Code;
    p->MinCellVoltage = g_BMSData.BMS_Read.MinCellVoltage;
    p->MaxCellVoltage = g_BMSData.BMS_Read.MaxCellVoltage;
    p->Alarm_In_Bit1 = g_BMSData.BMS_Read.Alarm_In_Bit1;
    p->Alarm_In_Bit2 = g_BMSData.BMS_Read.Alarm_In_Bit2;
    p->Alarm_In_Bit3 = g_BMSData.BMS_Read.Alarm_In_Bit3;
    p->Alarm_In_Bit4 = g_BMSData.BMS_Read.Alarm_In_Bit4;
    p->Alarm_In_Bit1_Lock = g_BMSData.BMS_Read.Alarm_In_Bit1_Lock;
    p->Alarm_In_Bit2_Lock = g_BMSData.BMS_Read.Alarm_In_Bit2_Lock;
    p->Alarm_In_Bit3_Lock = g_BMSData.BMS_Read.Alarm_In_Bit3_Lock;
    p->Alarm_In_Bit4_Lock = g_BMSData.BMS_Read.Alarm_In_Bit4_Lock;
    p->Alarm_In_CATL = g_BMSData.BMS_Read.Alarm_In_CATL;
    p->Alarm_In_CATL_Lock = g_BMSData.BMS_Read.Alarm_In_CATL_Lock;

    return 1;
}


//71IO强制
bool HMI_ForcedOut(void)
{
	Screen_ForcedOut_TypeDef*p=ExtHmi.p2Scr_ForcedOut;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.IOOutputForcing_Switch;

	return 1;
}
//72eeprom控制界面
bool HMI_ReadEEPCtrl(void)
{
    //非多源控制更新
    //未初始化 先初始化
    if(ExtHmi.PageInitialized != 1)
    {
        *(EEP_Ctrl_TypeDef*)ExtHmi.p2Data = EEPCtrl;
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取,有按下时更新，马上再复位，用于触发输入
    {
        if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)
        {
            //ExtHmi.p2Scr_EEPROM->EEPCtrl = *(u16*)&EEPCtrl;
            ExtHmi.p2Scr_EEPROM->EEPCtrl = EEPCtrl;
            ExtHmi.p2Scr_EEPROM->WriteCnt = EEPPara.WriteCnt;
            ExtHmi.p2Scr_EEPROM->WriteDate = EEPPara.WriteDate;
            ExtHmi.p2Scr_EEPROM->ProductSerial = EEPPara.ProductSerial;
            ExtHmi.p2Scr_EEPROM->CodeVersion = EEPPara.CodeVersion;
            ExtHmi.p2Scr_EEPROM->CRCSum = EEPPara.CRCSum;
            ExtHmi.p2Scr_EEPROM->EqpID = EEPPara.User.EqpID;
            ExtHmi.p2Scr_EEPROM->NumOfParaUser = sizeof(EEP_ParaUser_StDef)/2;
        }
        else {}

        if (ExtHmi.p2HMI_CommObj->FucBit.Write)
        {
            EEPPara.User.EqpID = ExtHmi.p2Scr_EEPROM->EqpID;
            ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
        }
        else {}

        if(*(u16*)&ExtHmi.p2Scr_EEPROM->EEPCtrl)
        {
            //EEPCtrl = *(EEP_Ctrl_TypeDef*)&(ExtHmi.p2Scr_EEPROM->EEPCtrl);
			//*(u16*)&EEPCtrl = ExtHmi.p2Scr_EEPROM->EEPCtrl;
            EEPCtrl = ExtHmi.p2Scr_EEPROM->EEPCtrl;
            *(u16*)&ExtHmi.p2Scr_EEPROM->EEPCtrl = 0;
        }
    }
    return 1;
}

bool HMI_ANCStatusPage(void)
{
    char i = 0;
    char N = ExtHmi.p2HMI_CommObj->SubIndex;			//翻页后，索引自加
    char SvoNum = MIN(HMI_Servo_MaxNum, 11);	//取最小值
    //界面对象首地址指针初始化
    ANCStatusPage_TypeDef* p2ScreenObj = (ANCStatusPage_TypeDef*)ExtHmi.p2Scr_ANCStatusPage;

    for (i = 0;i < SvoNum;i++)
    {
        p2ScreenObj->ANC[i].ANC_CPUError = EQP_ANC.p2RData->CPUError;
        p2ScreenObj->ANC[i].ANC_DrvError = EQP_ANC.p2RData->DrvError;
        p2ScreenObj->ANC[i].ANC_Active_Speed = EQP_ANC.p2RData->ActiveSpeed[i + N * SvoNum];
        p2ScreenObj->ANC[i].ANC_Torque = EQP_ANC.p2RData->Torque[i + N * SvoNum];
        p2ScreenObj->ANC[i].ANC_ErrorCode = EQP_ANC.p2RData->ErrorCode[i + N * SvoNum];
        p2ScreenObj->ANC[i].ANC_EN = (((EQP_ANC.p2WData->En) >> (i + N * SvoNum)) & 0x1 );
        p2ScreenObj->ANC[i].ANC_Start = (((EQP_ANC.p2WData->Start) >> (i + N * SvoNum)) & 0x1);
        p2ScreenObj->ANC[i].ANC_VCmd = EQP_ANC.p2WData->VCmd[i + N * SvoNum];


        if (i + N * SvoNum >= 11)
        {
            p2ScreenObj->ANC[i].ANC_CPUError = \
            p2ScreenObj->ANC[i].ANC_DrvError = \
            p2ScreenObj->ANC[i].ANC_Active_Speed = \
            p2ScreenObj->ANC[i].ANC_Torque = \
            p2ScreenObj->ANC[i].ANC_ErrorCode = \
            p2ScreenObj->ANC[i].ANC_EN = \
            p2ScreenObj->ANC[i].ANC_Start = \
            p2ScreenObj->ANC[i].ANC_VCmd = 0;
        }
    }

    p2ScreenObj->HMI_NOOROFF = !g_Para.HmiConceal.ANC_Switch;
    return 1;
}

//定义读写使能状态，0只读，1只写
volatile uint8_t enable_status = 0;

 //75多倍通设定
 bool HMI_DBCOMSet(void)
{
	//界面对象首地址指针初始化
	DBCOMSet_TypeDef* p = (DBCOMSet_TypeDef*)ExtHmi.p2Scr_DBCOMSet;
	
	p->HMI_NOOROFF = !g_Para.HmiConceal.DBCOM_Switch;
	g_DBComSet_Template.In_Data = p->DBCOMSet;
	p->Status = g_DBComSet_Template.STM;
	if(p->Enable)//使能按钮按下
	{
		if(enable_status == 0)//读写使能状态为0，则置1
		{
			enable_status = 1;
		}
		else
		{}
	}
	if(enable_status == 0)//读写使能状态为0，屏幕数值赋值给多倍通结构体
	{
		g_DBComSet_Template.En = p->Enable;
	}
	else//多倍通结构体数值赋值给屏幕
	{
		p->Enable = g_DBComSet_Template.En;
	}
	
	return 1;
}

//206自动运行信息 暂只显示
bool HMI_AutoRunStatus(void)
{
    //char i = 0;
    //界面对象首地址指针初始化
    AtuoRunStatus_Typedef* p2ScreenObj = (AtuoRunStatus_Typedef*)ExtHmi.p2Data;

    p2ScreenObj->HMI_NOOROFF = g_Para.HmiConceal.AutoOperation_Switch;
    //基本状态
    p2ScreenObj->AGV_AtuoRunState = g_HostData.AGVAutoMode;
    p2ScreenObj->AGV_AtuoRunStep = g_HostData.HostControlBuffer;
    p2ScreenObj->AGV_AtuoRunProcee = g_HostData.AGV_FuncSta;
    p2ScreenObj->AGV_NowTag = g_HostData.CurrentPointTag;
    p2ScreenObj->AGV_NowToward = g_HostData.ActualToward;
    p2ScreenObj->AGV_TargetToward = g_HostData.TargetToward;
    p2ScreenObj->AGV_TargetCourse = g_HostData.AGVCurrentCourse;
    p2ScreenObj->AGV_TargetTag = g_HostData.CurrentPath_Tag;
    p2ScreenObj->AGV_TargetSpeed = g_HostData.TargetForwardVelocity;
    //p2ScreenObj->AGV_SafeSwitchStatus = 
    p2ScreenObj->AGV_RadarArea = g_HostData.AGVRadio.ScanningArea;

    //GoodsQRData
    p2ScreenObj->GoodsQRData_Flag = g_HostData.GoodsQRData.flag;
    p2ScreenObj->GoodsQRData_x = g_HostData.GoodsQRData.QR_x;
    p2ScreenObj->GoodsQRData_y = g_HostData.GoodsQRData.QR_y;
    p2ScreenObj->GoodsQRData_z = g_HostData.GoodsQRData.QR_z * 100;

    //MagParkQRData
    p2ScreenObj->MagParkQRData_Flag = g_HostData.MagParkQRData.flag;
    p2ScreenObj->MagParkQRData_x = g_HostData.MagParkQRData.QR_x;
    p2ScreenObj->MagParkQRData_y = g_HostData.MagParkQRData.QR_y;
    p2ScreenObj->MagParkQRData_z = g_HostData.MagParkQRData.Offset_z * 100;

    //AGVMagParkZeroOffset
    p2ScreenObj->AGVMagParkZeroOffset_Flag = g_HostData.AGVMagParkZeroOffset.Flag;
    p2ScreenObj->AGVMagParkZeroOffset_F = g_HostData.AGVMagParkZeroOffset.Mag_F;
    p2ScreenObj->AGVMagParkZeroOffset_L = g_HostData.AGVMagParkZeroOffset.Mag_L;
    p2ScreenObj->AGVMagParkZeroOffset_R = g_HostData.AGVMagParkZeroOffset.Mag_R;
    p2ScreenObj->AGVMagParkZeroOffset_B = g_HostData.AGVMagParkZeroOffset.Mag_B;
    
    //实时值
    p2ScreenObj->RealTimeValue_x = g_HostData.FixtureQRData.QR_x;
    p2ScreenObj->RealTimeValue_y = g_HostData.FixtureQRData.QR_y;
    p2ScreenObj->RealTimeValue_z = g_HostData.FixtureQRData.QR_z * 100;
    return 1;
}

//207系统功能位参数页
bool HMI_SysFucBit(void)
{
    //未初始化 先初始化
    if (ExtHmi.PageInitialized != 1)
    {
        *(Para_SysFuc_BitDef*)ExtHmi.p2Data = g_Para.SysFucBit;
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        g_Para.SysFucBit = *(Para_SysFuc_BitDef*)ExtHmi.p2Data;
    }
    return 1;
}

//210MMU
bool HMI_MMU(void)
{
    char* p2Byte = NULL;

    //未初始化 先初始化
    if (ExtHmi.PageInitialized != 1)
    {
        ExtHmi.p2Scr_MMU->Address = 0;
        p2Byte = (char*)ExtHmi.p2Scr_MMU->Address;
        ExtHmi.p2Scr_MMU->R_Byte0 = *p2Byte;
        ExtHmi.p2Scr_MMU->R_Byte1 = *(p2Byte+1);
        ExtHmi.p2Scr_MMU->R_Byte2 = *(p2Byte+2);
        ExtHmi.p2Scr_MMU->R_Byte3 = *(p2Byte+3);

        ExtHmi.p2Scr_MMU->W_Byte0 = ExtHmi.p2Scr_MMU->R_Byte0;
        ExtHmi.p2Scr_MMU->W_Byte1 = ExtHmi.p2Scr_MMU->R_Byte1;
        ExtHmi.p2Scr_MMU->W_Byte2 = ExtHmi.p2Scr_MMU->R_Byte2;
        ExtHmi.p2Scr_MMU->W_Byte3 = ExtHmi.p2Scr_MMU->R_Byte3;
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        p2Byte = (char*)ExtHmi.p2Scr_MMU->Address;
        if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)
        {
			ExtHmi.p2Scr_MMU->R_Byte0 = *p2Byte;
			ExtHmi.p2Scr_MMU->R_Byte1 = *(p2Byte+1);
			ExtHmi.p2Scr_MMU->R_Byte2 = *(p2Byte+2);
			ExtHmi.p2Scr_MMU->R_Byte3 = *(p2Byte+3);
        }
        
        if (ExtHmi.p2HMI_CommObj->FucBit.Write)
		{
			*p2Byte = ExtHmi.p2Scr_MMU->W_Byte0;
			*(p2Byte+1) = ExtHmi.p2Scr_MMU->W_Byte1;
			*(p2Byte+2) = ExtHmi.p2Scr_MMU->W_Byte2;
			*(p2Byte+3) = ExtHmi.p2Scr_MMU->W_Byte3;
            ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
		}
    }
    return 1;
}

//208通信缓存路径
bool HMI_Path_Comm(void)
{
    u16 Num = sizeof(PathPoint_StDef) * 4;

    //未初始化 先初始化
    if (ExtHmi.PageInitialized != 1)
    {
        if (Num < sizeof(ExtHmi.p2HMI_CommObj->Screen_Head))
            memcpy(ExtHmi.p2Data, ((DSPCOMMOBJ_StDef*)&EQP_DSP.Data)->PathPoint, Num);
        else
        {
            g_Faults.CodeRemind.Bit.Screen_HeadLimit = 1;
        }
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)
        {
            if (Num < sizeof(ExtHmi.p2HMI_CommObj->Screen_Head))
                memcpy(ExtHmi.p2Data, ((DSPCOMMOBJ_StDef*)&EQP_DSP.Data)->PathPoint, Num);
            else
            {
                g_Faults.CodeRemind.Bit.Screen_HeadLimit = 1;
            }
        }

        if (ExtHmi.p2HMI_CommObj->FucBit.Write)
        {
            if (Num < sizeof(ExtHmi.p2HMI_CommObj->Screen_Head))
                memcpy(((DSPCOMMOBJ_StDef*)&EQP_DSP.Data)->PathPoint, ExtHmi.p2Data, Num);
            else
            {
                g_Faults.CodeRemind.Bit.Screen_HeadLimit = 1;
            }
            ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
        }
    }
    return 1;
}

//209链表路径
bool HMI_Path_Stack(void)
{
    char i = 0;
    char j = 0;
    char PathNum = 5;//单页路径数
    u16 Num = sizeof(Screen_Path_Typedef) * PathNum;

    //未初始化 先初始化
    if (ExtHmi.PageInitialized != 1)
    {
        if (Num < sizeof(ExtHmi.p2HMI_CommObj->Screen_Head))
        {
            //memcpy(ExtHmi.p2Data, PathStack, Num);
            
        }
        else
        {
            g_Faults.CodeRemind.Bit.Screen_HeadLimit = 1;
        }
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)
        {
            if (Num < sizeof(ExtHmi.p2HMI_CommObj->Screen_Head))
            {
                //memcpy(ExtHmi.p2Data, PathStack, Num);
                for (i = 0; i < PathNum; i++)
                {
                    j = ExtHmi.p2HMI_CommObj->SubIndex * PathNum + i;

                    //防止越界
                    if(j >= PathQueueNum)
					{
						j = PathQueueNum -1;
					}
                    else {}
                    
                    ExtHmi.p2Scr_Path[i].PathSequence = PathStack[j].PathSequence;
                    ExtHmi.p2Scr_Path[i].Label = PathStack[j].Label;
                    ExtHmi.p2Scr_Path[i].LiftHigh = PathStack[j].LiftHigh;
                    ExtHmi.p2Scr_Path[i].WaitTime = PathStack[j].WaitTime;
                    ExtHmi.p2Scr_Path[i].DoorFrameDistance = PathStack[j].DoorFrameDistance;
                    ExtHmi.p2Scr_Path[i].CurveParameters_Radius = PathStack[j].CurveParameters_Radius;
                    ExtHmi.p2Scr_Path[i].WishboneSpacing = PathStack[j].WishboneSpacing;
                    *(s32*)&ExtHmi.p2Scr_Path[i].Map_x_Index = PathStack[j].Map_x;
                    *(s32*)&ExtHmi.p2Scr_Path[i].Map_y_Index = PathStack[j].Map_y;
                    ExtHmi.p2Scr_Path[i].Towards = PathStack[j].Towards;
                    ExtHmi.p2Scr_Path[i].Course = PathStack[j].Course;
                    ExtHmi.p2Scr_Path[i].CurveParameters_Angle = PathStack[j].CurveParameters_Angle;
                    ExtHmi.p2Scr_Path[i].SpeedPercentage = PathStack[j].SpeedPercentage;
                    ExtHmi.p2Scr_Path[i].LidarScanAreaSelect = PathStack[j].LidarScanAreaSelect;
                    ExtHmi.p2Scr_Path[i].PathProperty = PathStack[j].PathProperty;
                    ExtHmi.p2Scr_Path[i].TurnProperty = PathStack[j].TurnProperty;
                    ExtHmi.p2Scr_Path[i].NavigationMode = PathStack[j].NavigationMode;

                    ExtHmi.p2Scr_Path[i].CmdBit1.ForRadioEnalbe = PathStack[j].ForRadioEnalbe;
                    ExtHmi.p2Scr_Path[i].CmdBit1.BackRadioEnalbe = PathStack[j].BackRadioEnalbe;
                    ExtHmi.p2Scr_Path[i].CmdBit1.LeftRadioEnalbe = PathStack[j].LeftRadioEnalbe;
                    ExtHmi.p2Scr_Path[i].CmdBit1.RightRadioEnalbe = PathStack[j].RightRadioEnalbe;
                    ExtHmi.p2Scr_Path[i].CmdBit1.BackCollisionEnable = PathStack[j].BackCollisionEnable;
                    ExtHmi.p2Scr_Path[i].CmdBit1.LeftCollisionEnable = PathStack[j].LeftCollisionEnable;
                    ExtHmi.p2Scr_Path[i].CmdBit1.RightCollisionEnable = PathStack[j].RightCollisionEnable;
                    ExtHmi.p2Scr_Path[i].CmdBit1.BevelRadioEnalbe = PathStack[j].BevelRadioEnalbe;
                    ExtHmi.p2Scr_Path[i].CmdBit1.ColumnRadioEnalbe = PathStack[j].ColumnRadioEnalbe;
                    ExtHmi.p2Scr_Path[i].CmdBit1.ConfirmSignal = PathStack[j].ConfirmSignal;
                    ExtHmi.p2Scr_Path[i].CmdBit1.ManualScheduling = PathStack[j].ManualScheduling;
                    ExtHmi.p2Scr_Path[i].CmdBit1.BlindWalk = PathStack[j].BlindWalk;
                    ExtHmi.p2Scr_Path[i].CmdBit1.StackCameraData = PathStack[j].StackCameraData;
                    ExtHmi.p2Scr_Path[i].CmdBit1.DirectionSign = PathStack[j].DirectionSign;
                    ExtHmi.p2Scr_Path[i].CmdBit1.NavigationModeSwitch = PathStack[j].NavigationModeSwitch;

                    *(u16*)&(ExtHmi.p2Scr_Path[i].CmdBit2) = 0;//清除未使用位
                    ExtHmi.p2Scr_Path[i].CmdBit2.VirtualPointSta = PathStack[j].VirtualPointSta;
                    ExtHmi.p2Scr_Path[i].CmdBit2.IntersectionSta = PathStack[j].IntersectionSta;

                    memset(ExtHmi.p2Scr_Path[i].NC,0,sizeof(ExtHmi.p2Scr_Path[i].NC));
                }
            }
            else
            {
                g_Faults.CodeRemind.Bit.Screen_HeadLimit = 1;
            }
        }

        if (ExtHmi.p2HMI_CommObj->FucBit.Write)
        {
            if (Num < sizeof(ExtHmi.p2HMI_CommObj->Screen_Head))
            {    
				//memcpy(((DSPCOMMOBJ_StDef*)&EQP_DSP.Data)->PathPoint, ExtHmi.p2Data, Num);
			}
            else
            {
                g_Faults.CodeRemind.Bit.Screen_HeadLimit = 1;
            }
            ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
        }
    }
    return 1;
}

//213系统命令页 原始命令
bool HMI_SysCmd_Org(void)
{
    ExtHmi.p2Scr_SysCmd->Channel = g_ChannelSwitch.in_Channel;
    ExtHmi.p2Scr_SysCmd->Mode = AccData.OrgCmd.Mode;
    ExtHmi.p2Scr_SysCmd->Vx = AccData.OrgCmd.Vxyz.x;
    ExtHmi.p2Scr_SysCmd->Vy = AccData.OrgCmd.Vxyz.y;
    ExtHmi.p2Scr_SysCmd->Vz = AccData.OrgCmd.Vxyz.z;
    ExtHmi.p2Scr_SysCmd->WalkGear = AccData.OrgCmd.WalkGear;
    ExtHmi.p2Scr_SysCmd->LiftGear = AccData.OrgCmd.LiftGear;
    ExtHmi.p2Scr_SysCmd->ObsArea = AccData.OrgCmd.Button.ObsArea;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.RadarF = AccData.OrgCmd.RadarEn.Front;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.RadarB = AccData.OrgCmd.RadarEn.Back;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.RadarL = AccData.OrgCmd.RadarEn.Left;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.RadarR = AccData.OrgCmd.RadarEn.Right;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.BarF = AccData.OrgCmd.BarEn.Front;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.BarB = AccData.OrgCmd.BarEn.Back;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.BarL = AccData.OrgCmd.BarEn.Left;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.BarR = AccData.OrgCmd.BarEn.Right;
    ExtHmi.p2Scr_SysCmd->ButtonBit.LiftUp = AccData.OrgCmd.Button.LiftUp;
    ExtHmi.p2Scr_SysCmd->ButtonBit.LiftDown = AccData.OrgCmd.Button.LiftDown;
    ExtHmi.p2Scr_SysCmd->ButtonBit.Horn = AccData.OrgCmd.Button.Horn;
    ExtHmi.p2Scr_SysCmd->ButtonBit.Lighting = AccData.OrgCmd.Button.Lighting;
    ExtHmi.p2Scr_SysCmd->ButtonBit.EStop = AccData.OrgCmd.Button.EStop;
    ExtHmi.p2Scr_SysCmd->ButtonBit.ChargingMC = AccData.OrgCmd.Button.ChargingMC;
    return 1;
}

//213系统命令页 执行命令
bool HMI_SysCmd_Cal(void)
{
    ExtHmi.p2Scr_SysCmd->Channel = g_ChannelSwitch.STM_Channel;
    ExtHmi.p2Scr_SysCmd->Mode = AccData.CalCmd.Mode;
    ExtHmi.p2Scr_SysCmd->Vx = AccData.CalCmd.Vxyz.x;
    ExtHmi.p2Scr_SysCmd->Vy = AccData.CalCmd.Vxyz.y;
    ExtHmi.p2Scr_SysCmd->Vz = AccData.CalCmd.Vxyz.z;
    ExtHmi.p2Scr_SysCmd->WalkGear = AccData.CalCmd.WalkGear;
    ExtHmi.p2Scr_SysCmd->LiftGear = AccData.CalCmd.LiftGear;
    ExtHmi.p2Scr_SysCmd->ObsArea = AccData.CalCmd.Button.ObsArea;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.RadarF = AccData.CalCmd.RadarEn.Front;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.RadarB = AccData.CalCmd.RadarEn.Back;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.RadarL = AccData.CalCmd.RadarEn.Left;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.RadarR = AccData.CalCmd.RadarEn.Right;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.BarF = AccData.CalCmd.BarEn.Front;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.BarB = AccData.CalCmd.BarEn.Back;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.BarL = AccData.CalCmd.BarEn.Left;
    ExtHmi.p2Scr_SysCmd->ObsEnBit.BarR = AccData.CalCmd.BarEn.Right;
    ExtHmi.p2Scr_SysCmd->ButtonBit.LiftUp = AccData.CalCmd.Button.LiftUp;
    ExtHmi.p2Scr_SysCmd->ButtonBit.LiftDown = AccData.CalCmd.Button.LiftDown;
    ExtHmi.p2Scr_SysCmd->ButtonBit.Horn = AccData.CalCmd.Button.Horn;
    ExtHmi.p2Scr_SysCmd->ButtonBit.Lighting = AccData.CalCmd.Button.Lighting;
    ExtHmi.p2Scr_SysCmd->ButtonBit.EStop = AccData.CalCmd.Button.EStop;
    ExtHmi.p2Scr_SysCmd->ButtonBit.ChargingMC = AccData.CalCmd.Button.ChargingMC;
    return 1;
}

//213系统命令页 index0原始命令，index1执行命令
bool HMI_SysCmd(void)
{
    //未初始化 先初始化
    if (ExtHmi.PageInitialized != 1)
    {
        memset(ExtHmi.p2Scr_SysCmd,0,sizeof(Screen_SysCmd_Typedef));//全部清除一次
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)
        {
            if(ExtHmi.p2HMI_CommObj->SubIndex == 0)
                HMI_SysCmd_Org();
            else if(ExtHmi.p2HMI_CommObj->SubIndex == 1)
                HMI_SysCmd_Cal();
			else {}
        }
        else if (ExtHmi.p2HMI_CommObj->FucBit.Write)
        {
            
            ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
        }
    }
    return 1;
}

//211自动信息
bool HMI_AtuoInf(void)
{
    //未初始化 先初始化
    if (ExtHmi.PageInitialized != 1)
    {
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)
        {
            ExtHmi.p2Scr_AtuoInf->Distance = g_VirtualOdoData.Distance;
            ExtHmi.p2Scr_AtuoInf->RTx = g_VirtualOdoData.RTx;
            ExtHmi.p2Scr_AtuoInf->RTy = g_VirtualOdoData.RTy;
            ExtHmi.p2Scr_AtuoInf->RTz = g_VirtualOdoData.RTz;
            ExtHmi.p2Scr_AtuoInf->RTxy = g_VirtualOdoData.RTxy;
            ExtHmi.p2Scr_AtuoInf->CurrentPathDelta = g_HostData.CurrentPathDelta;
            ExtHmi.p2Scr_AtuoInf->PointToStart_Dis = g_HostData.PointToStart_Dis;
            ExtHmi.p2Scr_AtuoInf->TargetPostureAngle = g_HostData.TargetPostureAngle;
            ExtHmi.p2Scr_AtuoInf->ActualPostureAngle = g_HostData.ActualPostureAngle;
            ExtHmi.p2Scr_AtuoInf->TargetAGVWorkMode = g_HostData.TargetAGVWorkMode;//目标AGV工作模式
            ExtHmi.p2Scr_AtuoInf->Hx = g_HostData.Hxyz[0];//摇杆x
            ExtHmi.p2Scr_AtuoInf->Hy = g_HostData.Hxyz[1];
            ExtHmi.p2Scr_AtuoInf->Hz = g_HostData.Hxyz[2];
            ExtHmi.p2Scr_AtuoInf->WalkGear = g_HostData.AGVWalkSpeedVal;//挡位
            ExtHmi.p2Scr_AtuoInf->LiftGear = g_HostData.AGVUpDownSpeedVal;

            //命令位
            if (g_HostData.AGVHandleButtom == 1)
            {
                ExtHmi.p2Scr_AtuoInf->CmdBit.LiftUp = 1;
                ExtHmi.p2Scr_AtuoInf->CmdBit.LiftDown = 0;
            }
			else if (g_HostData.AGVHandleButtom == 2)
			{
				ExtHmi.p2Scr_AtuoInf->CmdBit.LiftUp = 0;
				ExtHmi.p2Scr_AtuoInf->CmdBit.LiftDown = 1;
			}
			else
			{
				ExtHmi.p2Scr_AtuoInf->CmdBit.LiftUp = 0;
				ExtHmi.p2Scr_AtuoInf->CmdBit.LiftDown = 0;
			}

            ExtHmi.p2Scr_AtuoInf->CmdBit.ChargingMC = (g_DHR_AGVInformation[Para_Index_AGV_ChargingControl] != 0);
        }

        if (ExtHmi.p2HMI_CommObj->FucBit.Write)
        {
            ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
        }
    }
    return 1;
}

//功能按钮 系统中没有集中对象！
bool HMI_Button(void)
{
    //未初始化 先初始化
    if (ExtHmi.PageInitialized != 1)
    {
        memset(ExtHmi.p2Scr_Button,0,sizeof(Screen_Button_Typedef));//清除未使用位
        ExtHmi.p2Scr_Button->DebugBit.WROverAuto = g_ChannelSwitch.in_WROverAuto;
        ExtHmi.PageInitialized = 1;
    }
    else//已初始化 直接从界面读取
    {
        //直接控制
        g_ChannelSwitch.in_WROverAuto = ExtHmi.p2Scr_Button->DebugBit.WROverAuto;

        if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)
        {
            
        }
        else if (ExtHmi.p2HMI_CommObj->FucBit.Write)
        {
            ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
        }
        else {}
    }
    return 1;
}

//厂家调试
bool HMI_Manufacturer(void)
{
	ExtHmi.p2Data[0] = 123;
    return 1;
}

//临时输入输出
bool HMI_Tmp_INOUT(void)
{
    ExtHmi.p2Data[0] = 1;
    ExtHmi.p2Data[1] = 2;
    ExtHmi.p2Data[2] = 3;
    return 1;
}

//221联动报警
bool HMI_UnionEMG(void)
{
		Screen_UnionEMG_Typedef* p = ExtHmi.p2Scr_UnionEMG;

		p->Faults = g_Asb.Output.CarErr.Faults;      //报警状态
	
		p->AsbEnterErr = g_Asb.Output.AsbErr.AsbEnterErr; //进入联动条件不满足
		p->CantWired   = g_Asb.Output.AsbErr.CantWired ;//联动不能有线
		p->CarPanelSelErr = g_Asb.Output.AsbErr.CarPanelSelErr;//车端单联动切换异常
		p->CarPanelSelErr_Othre = g_Asb.Output.AsbErr.CarPanelSelErr_Othre;//对方车未选择联动
								
		p->CheckErr = g_Asb.Output.AsbErr.CheckErr ;//校验异常   
		p->D_LandR_OverlimitMax = g_Asb.Output.AsbErr.D_LandR_OverlimitMax;//联动时 左右差过大
		p->D_LandR_Overlimit_Enter = g_Asb.Output.AsbErr.D_LandR_Overlimit_Enter;//联动测距过大
		p->HgDiffOverlimit = g_Asb.Output.AsbErr.HgDiffOverlimit;//高度差过大
						  		
		p->LandscapeLaserOverlimit = g_Asb.Output.AsbErr.LandscapeLaserOverlimit;//光斑偏移过大
		p->LandscapeOverlimit_Enter = g_Asb.Output.AsbErr.LandscapeOverlimit_Enter;//光斑偏移过大
		p->LaserRangingChange = g_Asb.Output.AsbErr.LaserRangingChange;//测距变化过大
		p->LaserRangingToofar_Enter = g_Asb.Output.AsbErr.LaserRangingToofar_Enter;//联动测距过大
						  		
		p->LaserRangingToonear_Enter = g_Asb.Output.AsbErr.LaserRangingToonear_Enter;//联动测距过小
		p->MoveSynErr = g_Asb.Output.AsbErr.MoveSynErr;  //动作同步异常
		p->PointErr = g_Asb.Output.AsbErr.PointErr;//存在空指针
		p->WheelRMxaErr = g_Asb.Output.AsbErr.WheelRMxaErr;//单车最大轮系运动半径过大
		p->WheelVMxaErr = g_Asb.Output.AsbErr.WheelVMxaErr;//最大轮系速度过大
		
		p->DownLimited = g_Asb.Output.CarErr.Bit.DownLimited;  //限制下降
		p->MoveLimited = g_Asb.Output.CarErr.Bit.MoveLimited;  //限制行走
		p->UpLimited   = g_Asb.Output.CarErr.Bit.UpLimited;    //限制上升
		
		return 1;
}

//222联动参数
bool HMI_UnionParameter(void)
{
    Screen_UnionParameter_Typedef* p = ExtHmi.p2Scr_UnionParameter;
    AsbPara_TypeDef* p_Asb = &g_Asb.Para;
    if (ExtHmi.PageInitialized != 1)
    {
        //此界面默认读，读的时候就一直在更新显示缓存
        //应避免更新显示缓存的代码，在init和read中重复
        ExtHmi.PageInitialized = 1;
        ExtHmi.p2HMI_CommObj->FucBit.ReadEn = 1;
        ExtHmi.p2HMI_CommObj->FucBit.Write = 0;

        //下面的输入特殊处理要初始化一次 因为他们在read中被界面更新
        p->AsbSelect = g_IOInputs.Assembly.RealtimeValue;
        p->AsbDis = g_Asb.Input.Sensor.Distance_FL;
        p->SiteNo = g_Asb.Para.SiteNo;
        //p->AsbDis = g_Asb.Input.ManualInput_Dis;
    }
    else
    {
		if (ExtHmi.p2HMI_CommObj->FucBit.ReadEn)
		{
			p->Iam = p_Asb->Iam;		
			p->HalfCar = 0;
			p->Seat = p_Asb->Seat;
			p->K_HgAdj = p_Asb->K_HgAdj;
			p->K_DisAdj = p_Asb->K_DisAdj;
			p->HgDiffMax = p_Asb->HgDiffMax;

			{
				AsbSensorAlarmPara_TypeDef* p_Hmi = &(p->SensorAlarm);
				AsbSensorAlarmPara_TypeDef* p_Asb_SensorAlarm = &(p_Asb->SensorAlarm);

				p_Hmi->AngChangeMax = p_Asb_SensorAlarm->AngChangeMax;
				p_Hmi->Ang_EnterMax = p_Asb_SensorAlarm->Ang_EnterMax;
				p_Hmi->DiffLR_EnterMax = p_Asb_SensorAlarm->DiffLR_EnterMax;
				p_Hmi->DiffLR_Max = p_Asb_SensorAlarm->DiffLR_Max;
				p_Hmi->DisChangeMax = p_Asb_SensorAlarm->DisChangeMax;
				p_Hmi->DisChangeMax_Inf = p_Asb_SensorAlarm->DisChangeMax_Inf;
				p_Hmi->DisEnterMax = p_Asb_SensorAlarm->DisEnterMax;
				p_Hmi->DisEnterMin = p_Asb_SensorAlarm->DisEnterMin;
				p_Hmi->LaserSpotMax = p_Asb_SensorAlarm->AngChangeMax;
				p_Hmi->LaserSpot_EnterMax = p_Asb_SensorAlarm->LaserSpot_EnterMax;
			}

			p->SynTimeout = p_Asb->SynTimeout;

			p->FuncSwitch.TrackOrg = p_Asb->FuncSwitch.TrackOrg;
			p->FuncSwitch.RadarObsEn_Single = p_Asb->FuncSwitch.RadarObsEn_Single;
			p->FuncSwitch.ObsAreaSel_Single = p_Asb->FuncSwitch.ObsAreaSel_Single;

			/*显示
			p->AsbSelect = g_IOInputs.Assembly.RealtimeValue;
			p->AsbDis = g_Asb.Input.ManualInput_Dis;*/

			/*输入*/
			g_IOInputs.Assembly.RealtimeValue = p->AsbSelect;
			//g_Asb.Input.ManualInput_Dis = p->AsbDis;
		}
		else if (ExtHmi.p2HMI_CommObj->FucBit.Write)
		{
			//g_IOInputs.Assembly.RealtimeValue = p->AsbSelect;
			//g_Asb.Input.ManualInput_Dis = p->AsbDis;
			ExtHmi.p2HMI_CommObj->FucBit.Write = 0;
		}

		/*无论该界面使能读还是写，下面都为输入*/
		if (g_Para.Asb.Bit.AsbSelect_FromHMI)
		{
			g_IOInputs.Assembly.RealtimeValue = p->AsbSelect;//没有实体旋钮的车从hmi选择联动
			//原单动联动切换时 重置计时时间 已整理到系统内部实现
		}
		else//从实体旋钮选择联动时 界面上显示当前选择
		{
			p->AsbSelect = g_IOInputs.Assembly.StableValue;
		}

		/*没有选择联动时可输入，选择联动时仅显示*/
		if(g_IOInputs.Assembly.StableValue != 1)
		{
			p->Iam = p_Asb->Iam;
			p->Seat = p_Asb->Seat;
			//联动角色选择
			p_Asb->Role = p->Role;
			//联动阵型编号选择，只有主车才可以选择阵型，从车阵型数据从广播中来
			if(p_Asb->Role == AsbRole_Master)
				p_Asb->SiteNo = p->SiteNo;
			else
				p->SiteNo = p_Asb->SiteNo;
			
			//联动遥控器编号选择，从车是否可以选择？
			p_Asb->WirelessController = p->WHA_SN;
			//联动从车车型选择，从车是否可以选择？
			p_Asb->SlaveCarType = p->Car_SN;
			if (g_Asb.Para.SiteNo == AsbSiteNo_LR)//仅横向联动时输入
			{
				g_Asb.Input.Sensor.Distance_FL = p->AsbDis;
				g_Asb.Input.Sensor.Distance_BR = p->AsbDis;
			}
			else
			{
				p->AsbDis = g_Asb.Input.Sensor.Distance_FL;
			}
		}
		else
		{
			p->Iam = p_Asb->Iam;
			p->Seat = p_Asb->Seat;
			//联动角色显示
			p->Role   = p_Asb->Role;
			//联动阵型编号显示
			p->SiteNo = p_Asb->SiteNo;
			//联动测距距离显示
			p->AsbDis = g_Asb.Input.Sensor.Distance_FL;
			//联动遥控器编号显示
			p->WHA_SN = p_Asb->WirelessController;
			//联动从车车型显示
			p->Car_SN = p_Asb->SlaveCarType;
		}

    }

    return 1;
}

//进入时初始化一次的界面数据读取
bool HMI_Read_InitOnce(void)
{
    switch (ExtHmi.ScreenID)
    {
    case HMIPage_Start://20开始界面
		HMI_Start();
		break;

	case MainPage://21主界面	
		HMI_Show_MainPage();
		break;
				
    case SystemInfPage://24系统信息
        HMI_SystemInf();
        break;
	
	case AnalogDataPage://25板载模拟量界面
		HMI_AdOnboard();
		break;

	case WirelessStatusPage_One://26遥控器信息显示
		HMI_Wireless();
	break;
	
	case LinkStatusPage://27传感器通信状态	
		HMI_Show_Communication();
		break;
    
    case IOStatusPage://28 IO状态
        HMI_IOStatus();
        break; 
   
    case ServoStatusPage_One: //29 驱动器信息
        HMI_ServoStatus();
		break;
	
	case ManualControlPage://30手动控车
		HMI_ManualCtrl();
		break;
	
	 case ParaSetPage_One://31一般参数
		HMI_ParaSetPage_One();
		break;
		 
	 case WheelParaPage_One://32轮系参数设置界面
		HMI_WheelParaPage_One();
		break;
	 
	case HeightParaSetPage_One://33高度标定
        HMI_ReadLiftCalibration();
		break;
		    
    case HMIPage_AngleSetPage://34角度标定
        HMI_AngleSet();
        break;
	
	case DistanceSetPage://35角度标定
        HMI_DistanceSetPage();
        break;
	
	case FunctionButtonPage://36功能按钮 
		HMI_ReadFunctionSwitch();
		break;
	
	case forceButtonPage://37强制功能按钮
		break;
	
	case LeadShineSetPage://38雷赛伺服标定界面
		HMI_LeadShineSetPage();
		break;
	
	case InstPage://39PGV标定界面
		HMI_ReadInst();
		break;
	
	case OdomterParaPage://40里程计参数设置界面
		HMI_ReadVirtualOdo();
		break;
	
	case CtrlCoefParaPage://41控制系数参数
		HMI_ReadCtrlCoef();
		break;
	
	case PidSetPage://42PID标定界面
		HMI_PidSetPage();
		break;
	
	case RadarLocationPage://43位置雷达标定界面
		HMI_RadarLocationPage();
		break;
	
	case SpeedThresholdPage://44速度档位(行走速度，举升速度)
		HMI_SpeedThresholdPage();
		break;
	
	case VoltageParaPage://45电压与电量关联参数
		HMI_VoltageParaPage();
		break;
	
	case MoreParaPage://46更多参数-故障检出参数
		HMI_ReadEmgJudgePara();
		break;
	
	case RealTimeAnglePage://47舵轮角度
		HMI_Show_WheelAngle();
		break;
        
	case LiftHeightPage://48高度界面
        HMI_High_Page();
		break;
	
	case HangHeightPage://49悬挂高度显示
		HMI_HangHeightPage();
		break;
	
	case LegHeightPage://50支腿高度显示
		HMI_LegHeightPage();
		break;
		
	case LaserRangePage://51激光测距长度
		HMI_LaserRangePage();
		break;
	
	case UltrasonicDistancePage://52超声波测距信息
		HMI_UltrasonicDistancePage();
		break;
		
	case PressuredDataPage://53压力传感器信息
		HMI_High_Page();
		break;	
	
	case AnalogModulePage://54模拟量采集模块1
		HMI_AnalogModulePage();
       break;
	
	case AnalogOutPutModulePage://55模拟量输出模块
		HMI_AnalogOutPutModulePage();
		break;
	
	case GpsDataPage://56 GPS数据显示
		HMI_GpsDataPage();
		break;
	
	case LaserNavigationPage://57 激光导航数据显示
		HMI_LaserNavigationPage();
		break;
	

    case DispatchingSystemPage://58调度系统通信信息
        HMI_Dsp();
        break;

    case AutoRunStatusPage://59自动运行信息
        HMI_AutoRunStatus();
        break;
	
	case SafeSensorStatusPage://60避障显示页面
        HMI_ReadObs();
        break;	
	
	case RFIDDataPage://61磁导航RFID界面
		HMI_Show_MagRFID();
		break;
	
	case LeadShineServoStatusPage://62雷赛伺服状态
		HMI_LeadShineServoStatusPage();
		break;

	case PGVDataPage://63PGV界面
		HMI_Show_PGV();
		break;
	
	case PalletCameraPage://64栈板相机
		HMI_PalletCameraPage();
		break;
	
		
	case UnionStatusPage://65联动状态
		HMI_UnionStatus();
		break;
		
	case LightSpotPage://66光斑状态
		HMI_LightSpotPage();
		break;
	
	case PostionRadarPage://67位置定位雷达（R2000)
		HMI_PostionRadarPage();
		break;	
	
	case InclinometerPage://68倾角传感器
		HMI_InclinometerPage();
		break;	
	
	case HandPulserPage://69手摇脉冲器
		HMI_HandPulserPage();
		break;
			
	case BMSStatus://70BMS界面
		HMI_Show_BMS();
		break;

	case IoOutputForcePage://71IO输出强制页面
		HMI_ForcedOut();
		break;
	
    case EEPROMPage_One: //72EEPROM控制界面
        HMI_ReadEEPCtrl();
        break;

    case ANCStatusPage://74安川驱动器状态显示
        HMI_ANCStatusPage();
        break;
	
	case DBCOMSetPage://75多倍通设定
        HMI_DBCOMSet();
        break;
	
    case FunctionButtonSavePage://202功能按钮保存
		HMI_FunctionButtonSave();
		break;

    case HMIPage_Para_Auto://206自动运行参数
        HMI_ReadAutoPara();
        break;

    case HMIPage_Para_SysFucBit://207系统功能位参数
        HMI_SysFucBit();
        break;

    case HMIPage_Path_Comm://208通信缓存路径信息
        HMI_Path_Comm();
        break;
    
    case HMIPage_Path_Stack://209下载链表路径信息
        HMI_Path_Stack();
        break;

    case HMIPage_MMU://210内存管理
		HMI_MMU();
		break;
    
    case HMIPage_AtuoInf://211自动模式信息
        HMI_AtuoInf();
        break;
    
    case HMIPage_Button://212功能按键
        HMI_Button();
        break;

    case HMIPage_SysCmd://213系统命令
        HMI_SysCmd();
        break;
    
    case HMIPage_Manufacturer://220厂家调试
        HMI_Manufacturer();
        break;

    case HMIPage_Tmp_INOUT: //221临时输入输出
        HMI_Tmp_INOUT();
        break;
	
	case HMIPage_UnionEMG://222联动报警
		HMI_UnionEMG();
	break;

	case HMIPage_UnionParameter://223联动参数
		HMI_UnionParameter();
	break;
	
    default:
        break;
    }

    return 1;
}

//报警信息界面显示更新 此处需重构，类似受控停车ControlledStop，将非受控停车和提醒对象的系统定义和hmi定义分离
bool HMI_Show_Alarm(void)
{
	//u64 tmp = (g_Faults.Auto.All<<32);
	
    //非受控停车32 + 自动故障32
    ExtHmi.p2HMI_CommObj->EMG_Evet1.All = g_Faults.LoseControl.All+((u64)g_Faults.Auto.All<<32);
	
    //受控停车
    ExtHmi.p2HMI_CommObj->EMG_Evet2.All = g_Faults.ControlledStop.All;

    //提醒
    ExtHmi.p2HMI_CommObj->EMG_Remind.All = g_Faults.Remind.All;

    return 1;
}

//触摸屏主方法
bool HMI_Main(void)
{
	if (ExtHmi.LastPageNum != ExtHmi.p2HMI_CommObj->PageNum)//判断页面是否更新
	{
		ExtHmi.LastPageNum = ExtHmi.p2HMI_CommObj->PageNum;
        ExtHmi.PageInitialized = 0;
        ExtHmi.p2HMI_CommObj->FucBit.ReadEn = 1;            //读功能开
        ExtHmi.p2HMI_CommObj->FucBit.Write = 0;             //写功能关
        ExtHmi.p2HMI_CommObj->SubIndex = 0;                 //子索引清零
	}

    //获取当前界面id
    ExtHmi.ScreenID = ExtHmi.p2HMI_CommObj->PageNum;

	//hmi任务：显示更新、参数更新、功能回调
	if(g_Para.Debug.HmiShowOff == 0)
	{
		//实例中转显示 对象更新
        //报警信息界面显示更新
        HMI_Show_Alarm();

        //功能强制按钮 固定通信
	    HMI_ReadForceButton();
		
        //标准界面处理接口 进入时初始化一次的界面数据读取
        HMI_Read_InitOnce();
    }

	return 1;
}

//***********************************END OF FILE***********************************

