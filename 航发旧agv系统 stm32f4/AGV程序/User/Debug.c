/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Debug.c
** 创 建 人: 文小兵  
** 描    述: 调试文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月28日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define Debug_GLOBALS

// ITM端口宏定义
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000 + 4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000 + 4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000 + 4*n)))
#define DEMCR_UserDef           (*((volatile unsigned long *)(0xE000EDFC))) //修改定义 避免和DEMCR系统定义冲突
#define TRCENA          0x01000000

//自定义引用		
#include "Debug.h"	
#include "includes.h"
#include "CBB_Init.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "EmgJudge.h"
#include "PowerOn.h"
#include "ObjConf.h"
#include "common_def.h"

//外部对象声明
extern uint8_t g_AGV_DependenceInitCheckSucc;//

//系统对象查看指针初始化
bool GocInit_SYS(void)
{
	//系统ms计数器
	goc.Sysms = &g_Sys_1ms_Counter;
	goc.workstep = &g_MC_WorkStep;
	goc.sysapi = &g_SysApi;

	return 1;
}

//cbb对象查看指针初始化
bool GocInit_CBB(void)
{
	//goc.cbb.rfid = &g_RFIDData;
	//goc.cbb.mag = &g_MNData[0];
	goc.cbb.mag=(MNStr_StDef*)g_MNData;
	goc.cbb.lift = &g_LiftData;
	goc.cbb.whA = &g_WHData;
	goc.cbb.motor = &g_MotorResolving;
	goc.cbb.mecanum = &g_MecanumResolving;
	goc.cbb.magTrace = &g_MagneticTrace;
	goc.cbb.sln = &g_SLN;
	goc.cbb.roa = &g_ROA;
	goc.cbb.load = &g_LoadDetection;
	goc.cbb.shelf = &g_ShelfAutadj;
	goc.cbb.smoothVx = &g_SmoothVx;
	goc.cbb.smoothVy = &g_SmoothVy;
	goc.cbb.smoothVz = &g_SmoothVz;
	goc.cbb.smoothAx = &g_SmoothAx;
	goc.cbb.smoothAy = &g_SmoothAy;
	goc.cbb.smoothAz = &g_SmoothAz;
	goc.cbb.smoothVxyz = (DataSmoothingStr_TypeDef*)g_SmoothVxyz;
	goc.cbb.magVir = &g_VirtualMagnetic;
	goc.cbb.magVirForPgv = &g_PGVAngle_MagneticVirtual;
	goc.cbb.odometer = &g_VirtualOdoData;
	goc.cbb.hostCtrl = &g_HostData;
    goc.cbb.CPC = &g_CPC;
	return 1;
}

//串口对象查看指针初始化
bool GocInit_USART(void)
{
	goc.uart.USARTx = (USARTStr_StDef*)RS485_USARTx;
	goc.uart.CCFs = &CCFs;
	goc.uart.EQPMsts = &EQPMsts;
	goc.uart.EQPSlvs = &EQPSlvs;
	goc.uart.MBMst = &mb_manger_app;
	goc.uart.EQP_HMI = &EQP_HMI;
	goc.uart.EQP_DSP = &EQP_DSP;
	goc.uart.EQP_COMMTEST = &EQP_COMMTEST;
	goc.uart.EQP_WHA = &EQP_WHA;
	goc.uart.EQP_WHB = &EQP_WHB;
	goc.uart.EQP_BMS_CommConv = &EQP_BMS_CommConv;
	goc.uart.EQP_Voice = &EQP_Voice;
	goc.uart.EQP_AD = &EQP_AD;
	goc.uart.EQP_AD1 = &EQP_AD1;
	goc.uart.EQP_Thermodetector = &EQP_Thermodetector;
	goc.uart.EQP_Inclinometer_Rion = &EQP_Inclinometer_Rion;
	goc.uart.EQP_MagF = &EQP_MagF;
	goc.uart.EQP_MagB = &EQP_MagB;
	goc.uart.EQP_MagL = &EQP_MagL;
	goc.uart.EQP_MagR = &EQP_MagR;
	goc.uart.EQP_RFID = &EQP_RFID;
	goc.uart.EQP_PGVUpward = &EQP_PGVUpward;
	goc.uart.EQP_PGVDownward = &EQP_PGVDownward;
	goc.uart.EQP_PGVUpward_F = &EQP_PGVUpward_F;
	goc.uart.EQP_PGVUpward_B = &EQP_PGVUpward_B;
	goc.uart.EQP_PGVDownward_F = &EQP_PGVDownward_F;
	goc.uart.EQP_PGVDownward_B = &EQP_PGVDownward_B;
	goc.uart.EQP_SerialExp = &EQP_SerialExp;
	goc.uart.EQP_CommConv = &EQP_CommConv;
	goc.uart.EQP_CommConv2 = &EQP_CommConv2;
	goc.uart.EQP_IPC = &EQP_IPC;
	goc.uart.EQP_Roll_Controller = &EQP_Roll_Controller;
	goc.uart.EQP_LaserSpot = &EQP_LaserSpot;
	goc.uart.EQP_Assembly_Master = &EQP_Assembly_Master;
	goc.uart.EQP_Hinson_Radar_Front = &EQP_Hinson_Radar_Front;
	goc.uart.EQP_Hinson_Radar_Rear = &EQP_Hinson_Radar_Rear;
	goc.uart.EQP_Hinson_Radar_Left = &EQP_Hinson_Radar_Left;
	goc.uart.EQP_Hinson_Radar_Right = &EQP_Hinson_Radar_Right;
	goc.uart.EQP_Hinson_Radar_FrontUp = &EQP_Hinson_Radar_FrontUp;
	goc.uart.EQP_Hinson_Radar_RearUp = &EQP_Hinson_Radar_RearUp;
	goc.uart.EQP_ME15_Near = &EQP_ME15_Near;
	goc.uart.EQP_ME15_Far = &EQP_ME15_Far;

	return 1;
}

//can对象查看指针初始化
bool GocInit_CAN(void)
{
	goc.can.STM32_CAN1 = &STM32_CAN[user_CAN_ID];
	goc.can.CANOD = &CanObjectDict_Data;
	goc.can.can_master = &CanMaster;
	return 1;
}

//IO对象查看指针初始化
bool GocInit_IO(void)
{
	goc.io.inpus = &g_IOInputs;
	goc.io.outputs = &g_IOOutputs;
	goc.io.io2dhrbit = &g_IOCN2DHRBit;
	return 1;
}

//adda对象查看指针初始化
bool GocInit_ADDA(void)
{
	goc.adda.ADCNs = &g_ADCNs;
	goc.adda.ADFucs = &g_ADFucs;
	goc.adda.DACNs = &g_DACNs;
	goc.adda.DAFucs = &g_DAFucs;
	return 1;
}

//参数对象查看指针初始化
bool GocInit_Para(void)
{
	goc.para.para = &g_Para;
	goc.para.eep = &EEPPara;
	goc.para.eepctrl = &EEPCtrl;
	return 1;
}

//调试对象查看指针初始化
bool GocInit_Dbug(void)
{
	goc.Dbug.dbugrtx = &g_DbugRtx;
	goc.Dbug.faults = &g_Faults;
	goc.Dbug.codeerr = &g_CodeErr;
	goc.Dbug.AtDbug = &AtDbug;
	goc.Dbug.CodeLogicErr = &CodeLogicErr;
	return 1;
}

//其它对象查看指针初始化
bool GocInit_Acc(void)
{
    goc.Acc.p2MsRecord = &MsRecord;
	goc.Acc.Acc = &AccData;
	goc.Acc.poweron = &g_PowerOn;
	goc.Acc.channel = &g_ChannelSwitch;
//	goc.Acc.bms = &g_BMSData;
//	goc.Acc.pgvUpward = &g_PGVUpward;
//	goc.Acc.pgvDownward = &g_PGVDownward;
	goc.Acc.angle = (AngleStr_StDef*)g_WheelAngle;
	goc.Acc.hwinfo = &HardInfo;
	goc.Acc.svo = (SVOStr_StDef*)SVO;
	goc.Acc.SVOs = &SVOs;
	goc.Acc.fan = &g_Fan;
	goc.Acc.Voice = &g_VoicePlayer;
	return 1;
}

//自动对象查看指针初始化
bool GocInit_Auto(void)
{
	goc.Auto.PathTypePara = &PathTypePara;
	goc.Auto.p2PathStack = pPathStation;
	goc.Auto.p2PathStation = (struct PathTypeDefStr*)PathStack;

	goc.Auto.g_HostData = &g_HostData;
	goc.Auto.p2AGVInf = g_DHR_AGVInformation;
	goc.Auto.p2CommObj = (DSPCOMMOBJ_StDef*)goc.Auto.p2AGVInf;

	goc.Auto.g_AGVDependenCBB = &g_AGVDependenInputData;
//	goc.Auto.g_QRDataHF = &g_QRDataHF;
//	goc.Auto.g_QRDataDH = &g_QRDataDH;
//	goc.Auto.g_GyroA0030SData = &g_GyroA0030SData;

	goc.Auto.KeyState.p2ControlMODEL = &g_HostData.ControlMODEL;
	goc.Auto.KeyState.p2ExecutionPathState = &g_HostData.ExecutionPathState;
	goc.Auto.KeyState.p2AGVAutoMode = &g_HostData.AGVAutoMode;
	goc.Auto.KeyState.p2TargetAGVWorkMode = &g_HostData.TargetAGVWorkMode;
	goc.Auto.KeyState.p2Hxyz = (struct HxyzStDef *)g_HostData.Hxyz;
	goc.Auto.KeyState.p2CheckSucc = &g_AGV_DependenceInitCheckSucc;
	return 1;
}

//HMI对象查看指针初始化
bool GocInit_HMI(void)
{
	goc.HMI.p2HMI_Buf = (struct HMI_Buf_StDef *)HMI_Buf;
	goc.HMI.p2ExtHmi = &ExtHmi;
	return 1;
}

//其它对象查看指针初始化
bool GocInit_Others(void)
{
#ifdef SSMLT
    goc.Others.p2ASM = &g_ASM;
#endif
	goc.Others.p2ASD = &g_AssemlySensorData;
	return 1;
}

//全局对象指针汇总初始化
bool GocInit(void)
{
	//系统对象查看指针初始化
	GocInit_SYS();
	
	//cbb对象查看指针初始化
	GocInit_CBB();

	//串口对象查看指针初始化
	GocInit_USART();

	//can对象查看指针初始化
	GocInit_CAN();

	//IO对象查看指针初始化
	GocInit_IO();
	
	//adda对象查看指针初始化
	GocInit_ADDA();
	
	//参数对象查看指针初始化
	GocInit_Para();

	//调试对象查看指针初始化
	GocInit_Dbug();

	//其它对象查看指针初始化
	GocInit_Acc();

	//自动对象查看指针初始化
	GocInit_Auto();

	//HMI对象查看指针初始化
	GocInit_HMI();

    //联动对象初始化
    goc.Asb = &g_Asb;

    //其它对象查看指针初始化
    GocInit_Others();
	
	return 1;
}

//自动对中测试
bool Dbg_ShelfAutadj(void)
{
    ExtHmi.p2HMI_CommObj->ButtonForce.Bit.ForceOverErr_Bit10 = 1;//强制忽略错误
    AccData.FaultsResponseSTM = ACC_FaultsType_Null;

    AccData.OrgCmd.Mode = ACCMode_PosFlexible;
    AccData.OrgCmd.Vxyz.y = 30;
    AccData.OrgCmd.Button.ShelfAutadj = 1;

    g_ShelfAutadj.in_DistanceLeft[0] = 300;
    g_ShelfAutadj.in_DistanceRight[0] = 250;

    return 1;
}

//轮系跟随巡线 测试
bool Dbg_CPC(void)
{
    ExtHmi.p2HMI_CommObj->ButtonForce.Bit.ForceOverErr_Bit10 = 1;//强制忽略错误
    AccData.FaultsResponseSTM = ACC_FaultsType_Null;

    AccData.OrgCmd.Mode = ACCMode_PosTrace;
    AccData.OrgCmd.Vxyz.y = 30;

    return 1;
}

//开发调试操作
bool DevDbugOperation()
{
	u8 i=0;
	if(g_Para.Debug.SvoDbug_AutoRun)//伺服通用协议栈开发测试
	{
		if(CanMaster.CanopenStep==Step_Active)//canopen启动完成后
		{
			//伺服使能
			for(i=0;i<g_Para.Walk.MotorNum;i++)
			{	
				SVO[i].En=1;
				SVO[i].CMDSpeed = 1000;//伺服速度控制
			}
			g_Para.Debug.SvoDbug_AutoRun=0;
		}
	}

	//切换到自动模式
	if(AtDbug.Change2Auto)
	{
		char i=0;

		//设置上线朝向
		g_VirtualOdoData.FixZ=18000;
		g_VirtualOdoData.FixFlag=1;

		//设置上线点
		g_HostData.CurrentPointTag=1;

		//初始化磁导航数据
		for(i=0;i<MN_NUM;i++)
		{
			g_MNData[i].MN_Read.L_or_R_Sta = 0;
			g_MNData[i].MN_Read.Status = 1;
			g_MNData[i].MN_Read.Distance = 0;
			g_MNData[i].MN_Read.Strength = 500;
		}

		//切换到自动模式
		g_IOInputs.DspModeSel.StableValue=1;

		//暂停任务
		g_HostData.InputPauseTaskFlag=1;

		AtDbug.Load=0;
		AtDbug.Down=0;
		AtDbug.Change2Auto=0;
	}

	//载入测试路径
	if(AtDbug.Load)
	{
		i=1;
		goc.Auto.p2CommObj->PathPoint[i].WayPointSequence = i;
		goc.Auto.p2CommObj->PathPoint[i].WayPointLabelIndex = 1;
		goc.Auto.p2CommObj->PathPoint[i].WayPointXValue = 0;//s32低位
		goc.Auto.p2CommObj->PathPoint[i].WayPointYValue = 0;
		goc.Auto.p2CommObj->PathPoint[i].WayPointZValue = 18000;
		goc.Auto.p2CommObj->PathPoint[i].WayPointPlatformHigh = 100;//mm
		goc.Auto.p2CommObj->PathPoint[i].WayPointWaitTime = 0;
		goc.Auto.p2CommObj->PathPoint[i].WayPointCourse = 0;//0.01°
		goc.Auto.p2CommObj->PathPoint[i].WayPointSpeedPerAndLidarScanAreaSwitch = 20;//低8位速度，高8位激光扫描区域切换
		goc.Auto.p2CommObj->PathPoint[i].WayPointSecuritySensorControl = 0;
		goc.Auto.p2CommObj->PathPoint[i].WayPointConfirmSignalAndManualSched = 0;
		goc.Auto.p2CommObj->PathPoint[i].WayPointProperty = 0;

		i=2;
		goc.Auto.p2CommObj->PathPoint[i].WayPointSequence = i;
		goc.Auto.p2CommObj->PathPoint[i].WayPointLabelIndex = 4;
		goc.Auto.p2CommObj->PathPoint[i].WayPointXValue = 1000;//s32低位
		goc.Auto.p2CommObj->PathPoint[i].WayPointYValue = 0;
		goc.Auto.p2CommObj->PathPoint[i].WayPointZValue = 18000;
		goc.Auto.p2CommObj->PathPoint[i].WayPointPlatformHigh = 100;//mm
		goc.Auto.p2CommObj->PathPoint[i].WayPointWaitTime = 0;
		goc.Auto.p2CommObj->PathPoint[i].WayPointCourse = 0;//0.01°
		goc.Auto.p2CommObj->PathPoint[i].WayPointSpeedPerAndLidarScanAreaSwitch = 20;//低8位速度，高8位激光扫描区域切换
		
		goc.Auto.p2CommObj->WayPointUpdateQuantify = 2;//路径更新数量
		AtDbug.Load=0;
	}

	//路径下载
	if(AtDbug.Down)
	{
		//路径下载
		goc.Auto.p2CommObj->WayPointDownloadStatus = 1;
		AtDbug.Down=0;
	}

	//自动控制命令switch
	if(AtDbug.Ctrl != Auto_Null)
	{
		switch (AtDbug.Ctrl)
		{
			case Auto_Null:
			break;

			case Auto_Init:
			break;

			case Auto_Change2Auto:
			break;

			case Auto_Load:
			break;

			case Auto_Down:
			break;

			case Auto_Start:
				g_HostData.InputGoOnTaskFlag=1;
				//g_IOInputs.AutoStartStop.StableValue=1;//理论上调试接口应该尽可能串行调用api，不产生重复的api，但这里如果使用按键api接口逻辑较为复杂
			break;

			case Auto_Pause:
				g_HostData.InputPauseTaskFlag=1;
			break;

			case Auto_Stop:
			break;

			case Auto_Cancel:
			break;

			case Auto_ExitAuto:
			break;

			default:
			break;
		}

		//清除控制命令
		AtDbug.Ctrl=Auto_Null;
	}

    if(AtDbug.TestRunOnce)
    {
        //自动对中测试
        //Dbg_ShelfAutadj();
        //轮系跟随巡线 测试
        Dbg_CPC();
        
        AtDbug.TestRunOnce=0;
    }

	return 1;
}

//外部对象声明

//内部方法声明



//***********************************END OF FILE***********************************

