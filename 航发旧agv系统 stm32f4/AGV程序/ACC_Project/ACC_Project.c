/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ACC_Project.c
** 创 建 人: 文小兵  
** 描    述: 标准设备程序顶层调用,目前作为模板，每个项目需要适当修改
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.3  
** 创建日期: 2024年4月25日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define ACC_PROJECT_GLOBALS

//自定义引用			
#include "includes.h"
#include "CBB_Init.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "app_DependenceAGV.h"
#include "drv_IO.h"
#include "BaseApi.h"
#include "EmgJudge.h"
#include "ADDA.h"
#include "drv_Servo.h"
#include "LinearCal.h"
#include "CommConf.h"
#include "ObjUpdate.h"
#include "PowerOn.h"
#include "Debug.h"
#include "HMI_ConfInit.h"
#include "CtrlSourceSwitch.h"
#include "Para_EEPROM.h"
#include "ACC_AsbCtrl_OnlyRemote.h"
#include "BaseApi.h"

/*下一级调用的函数引用*/
#include "ACC_WalkMotorResolving.h"
#include "ACC_AssemlyCtrl.h"
#include "ACC_LiftCtrl.h"
#include "ACC_LegCtrl.h"
#include "ACC_AbsorptionCtrl.h"
#include "ACC_AGVCtrl.h"
#include "ACC_LeadShineCtrl.h"
#include "ACC_AutoCentre.h"
#include "ACC_CableRollCtrl.h"

//初始化后只在本文件中使用的对象


//外部对象声明
extern u16 YoLsTemp[YOUT_DHR_NUM_LS];								//输出数据

//内部方法声明

//内部对象声明

//清除各个模式下的动作命令
bool CmdClear(void)
{
	g_ChannelSwitch.CtrlCmd.Vxyz.x = 0;
	g_ChannelSwitch.CtrlCmd.Vxyz.y = 0;
	g_ChannelSwitch.CtrlCmd.Vxyz.z = 0;
	g_ChannelSwitch.CtrlCmd.Button.LiftUp = 0;
	g_ChannelSwitch.CtrlCmd.Button.LiftDown = 0;

	return 1;
}

//系统断电复位
void SystemReset(void)
{
	Fuc_All_FreshIO(YoLsTemp, 0xAA);//把所有IO清零
	__set_FAULTMASK(1);//关闭中断
	NVIC_SystemReset();//系统复位
}

//ACC复位 不断电
void AccReset(void)
{
	g_Sys_1ms_Counter = 0;//清除系统ms计数
	g_PowerOn.PowerOnStep = ACC_PowerOnStep_Init;
	CanMaster.CanopenStep = Step_ParaIni;//将canopen主机步骤清零
}

//过载力矩限制
bool LoadCntTorqueLimit(void)
{
	char i = 0;
	for (i = 0;i < SVO_MAXNUM;i++)
	{
		if (SVO[i].LoadCnt >= g_Para.Walk.LoadCntMax)
			SVO[i].CMDLimitTorque = g_Para.Walk.TorqueLimitMin;
		else if (SVO[i].LoadCnt <= g_Para.Walk.LoadCntMin)
			SVO[i].CMDLimitTorque = g_Para.Walk.TorqueLimitMax;
		else {}//保持
	}
	return 1;
}

/***********************************************************
** 函数名称:	ACC_CtrlExecute_IO_Out
** 功能描述: 	将ACC结果给到硬件IO中
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//将ACC结果给到硬件IO中
void ACC_CtrlExecute_IO_Out(void)
{
	IO_Outputs_TypeDef 	*p2IO 	= &g_IOOutputs;
	ACC_DATA_TypeDef 	*p2Acc	= &AccData;

	//基本电源控制
	{
		p2IO->MainDCDC			.OutValue = p2Acc->IOCtrl.MainDCDC;			//主DCDC
		p2IO->CtrlVoltageBypass	.OutValue = p2Acc->IOCtrl.CtrlVoltageBypass;//控制电压旁路
		p2IO->PowerOnBypass		.OutValue = p2Acc->IOCtrl.PowerOnBypass;	//上电旁路
		p2IO->ChargingMC		.OutValue = p2Acc->IOCtrl.ChargingMC;		//充电极板控制

		p2IO->Precharge			.OutValue = p2Acc->IOCtrl.Precharge;		//预充
		p2IO->MainContactor		.OutValue = p2Acc->IOCtrl.MainContactor;	//主接触器
		p2IO->CtrllPower		.OutValue = p2Acc->IOCtrl.CtrllPower;		//控制电源

		p2IO->MotorPrecharge	.OutValue = p2Acc->IOCtrl.MotorPrecharge;	//驱动器预充
		p2IO->MotorContactor	.OutValue = p2Acc->IOCtrl.MotorContactor;	//驱动器主接触器
		p2IO->ANC				.OutValue = p2Acc->IOCtrl.ANC;				//安川M3板卡电源
		
		p2IO->MoveBrake			.OutValue = p2Acc->IOCtrl.MoveBrake;		//抱闸
	}

	//泰坦液压平台的电磁阀控制输出
	{
		p2IO->Platform_Titan_UpLF		.OutValue = p2Acc->IOCtrl.Platform_Titan_UpLF;
		p2IO->Platform_Titan_DownLF		.OutValue = p2Acc->IOCtrl.Platform_Titan_DownLF;	
		p2IO->Platform_Titan_UpRF		.OutValue = p2Acc->IOCtrl.Platform_Titan_UpRF;		
		p2IO->Platform_Titan_DownRF		.OutValue = p2Acc->IOCtrl.Platform_Titan_DownRF;	
		p2IO->Platform_Titan_UpB		.OutValue = p2Acc->IOCtrl.Platform_Titan_UpB;		
		p2IO->Platform_Titan_DownB		.OutValue = p2Acc->IOCtrl.Platform_Titan_DownB;		

		//仅四点液压有效
		p2IO->Platform_Titan_UpLB.OutValue = p2Acc->IOCtrl.Platform_Titan_UpLB;
		p2IO->Platform_Titan_DownLB.OutValue = p2Acc->IOCtrl.Platform_Titan_DownLB;
		p2IO->Platform_Titan_UpRB.OutValue = p2Acc->IOCtrl.Platform_Titan_UpRB;
		p2IO->Platform_Titan_DownRB.OutValue = p2Acc->IOCtrl.Platform_Titan_DownRB;
	}

	//剪刀叉的电磁阀控制输出
	{
		p2IO->Platform_Scissors_Down	.OutValue = p2Acc->IOCtrl.Platform_Scissors_Down;
	}

	//四联泵的电磁阀控制输出
	{
		p2IO->Platform_UnitePump_UpLF.OutValue 		= p2Acc->IOCtrl.Platform_UnitePump_UpLF;	//上升阀左前
		p2IO->Platform_UnitePump_UpRF.OutValue 		= p2Acc->IOCtrl.Platform_UnitePump_UpRF;	//上升阀右前
		p2IO->Platform_UnitePump_UpLB.OutValue 		= p2Acc->IOCtrl.Platform_UnitePump_UpLB;	//上升阀左后
		p2IO->Platform_UnitePump_UpRB.OutValue 		= p2Acc->IOCtrl.Platform_UnitePump_UpRB;	//上升阀右后
		p2IO->Platform_UnitePump_DownLF.OutValue 	= p2Acc->IOCtrl.Platform_UnitePump_DownLF;	//下降阀左前
		p2IO->Platform_UnitePump_DownRF.OutValue 	= p2Acc->IOCtrl.Platform_UnitePump_DownRF;	//下降阀右前
		p2IO->Platform_UnitePump_DownLB.OutValue 	= p2Acc->IOCtrl.Platform_UnitePump_DownLB;	//下降阀左后
		p2IO->Platform_UnitePump_DownRB.OutValue 	= p2Acc->IOCtrl.Platform_UnitePump_DownRB;	//下降阀右后
	}

	//泰坦液压悬挂的电磁阀控制输出
	{
		p2IO->Absorption_Titan_UpLF.OutValue = p2Acc->IOCtrl.Absorption_Titan_UpLF;
		p2IO->Absorption_Titan_DownLF.OutValue = p2Acc->IOCtrl.Absorption_Titan_DownLF;
		p2IO->Absorption_Titan_UpRF.OutValue = p2Acc->IOCtrl.Absorption_Titan_UpRF;
		p2IO->Absorption_Titan_DownRF.OutValue = p2Acc->IOCtrl.Absorption_Titan_DownRF;
		p2IO->Absorption_Titan_UpB.OutValue = p2Acc->IOCtrl.Absorption_Titan_UpB;
		p2IO->Absorption_Titan_DownB.OutValue = p2Acc->IOCtrl.Absorption_Titan_DownB;
	}
		
	//声光提示
	{
		p2IO->LightPower		.OutValue = p2Acc->IOCtrl.LightPower;		//电源指示灯
		p2IO->LightSystem		.OutValue = p2Acc->IOCtrl.LightSystem;		//系统指示灯
		p2IO->LightAlarm		.OutValue = p2Acc->IOCtrl.LightAlarm;		//报警指示灯
		p2IO->AGVled_StartStop	.OutValue = p2Acc->IOCtrl.AGVled_StartStop;	//启动停止指示灯
		p2IO->AGVled_Reset		.OutValue = p2Acc->IOCtrl.AGVled_Reset;		//复位指示灯
		p2IO->Horn				.OutValue = p2Acc->IOCtrl.Horn;				//喇叭
		p2IO->Lighting			.OutValue = p2Acc->IOCtrl.Lighting;			//照明

		p2IO->LampRed			.OutValue = p2Acc->IOCtrl.LampRed;		//红灯
		p2IO->LampGreen			.OutValue = p2Acc->IOCtrl.LampGreen;	//绿灯
		p2IO->LampBlue			.OutValue = p2Acc->IOCtrl.LampBlue;		//蓝灯
		p2IO->LampRed2			.OutValue = p2Acc->IOCtrl.LampRed;		//红灯
		p2IO->LampGreen2		.OutValue = p2Acc->IOCtrl.LampGreen;	//绿灯
		p2IO->LampBlue2			.OutValue = p2Acc->IOCtrl.LampBlue;		//蓝灯

		//防爆LED控制
		if(p2Acc->IOCtrl.ExplosionProof_LED == 1)
		{
			p2IO->ExplosionProof_LED_1		.OutValue = 1;
			p2IO->ExplosionProof_LED_2		.OutValue = 1;
			p2IO->ExplosionProof_LED_3		.OutValue = 1;
			p2IO->ExplosionProof_LED_4		.OutValue = 1;
		}
		else
		{
			p2IO->ExplosionProof_LED_1		.OutValue = 0;	
			p2IO->ExplosionProof_LED_2		.OutValue = 0;	
			p2IO->ExplosionProof_LED_3		.OutValue = 0;	
			p2IO->ExplosionProof_LED_4		.OutValue = 0;	
		}
		
		p2IO->Buzzer			.OutValue = p2Acc->IOCtrl.Buzzer;		//蜂鸣器
		p2IO->LightCaution		.OutValue = p2Acc->IOCtrl.LightCaution;	//警示灯
	}

	//其它控制
	{
		p2IO->CoolingFan		.OutValue = p2Acc->IOCtrl.CoolingFan;	//风扇
		p2IO->PowerOffDealy		.OutValue = p2Acc->IOCtrl.PowerOffDealy;//掉电延时继电器
		p2IO->PowerOffDealy_Bms	.OutValue = p2Acc->IOCtrl.PowerOffDealy_Bms;//掉电延时继电器
		{
			GroundCharge_CylinderControl_TypeDef *pGround = &g_GroundChargeCylinder;
			p2IO->GroundChargingExtend  .OutValue = pGround->OutValue_Extend;		//电缸伸出
			p2IO->GroundChargingRetract .OutValue = pGround->OutValue_Retend;		//电缸缩回
			if(pGround->OutValue_Retend != 0 || pGround->OutValue_Extend != 0)
			{
				p2IO->GroundChargingPower .OutValue = 1;
			}
			else
			{
				p2IO->GroundChargingPower .OutValue = 0;
			}
		}
	}

	//升降的科润控制
	{
		p2IO->KoRunReversal		.OutValue = p2Acc->IOCtrl.KoRunReversal;	//科润反转
		p2IO->KoRunBrake		.OutValue = p2Acc->IOCtrl.KoRunBrake;		//科润正转
		p2IO->KoRunSysPower		.OutValue = p2Acc->IOCtrl.KoRunSysPower;	//科润系统电源
		p2IO->KoRunContactor	.OutValue = p2Acc->IOCtrl.KoRunContactor;	//科润接触器

		p2IO->KoRun_2_Reversal	.OutValue = p2Acc->IOCtrl.KoRun_2_Reversal;	//科润反转
		p2IO->KoRun_2_Brake		.OutValue = p2Acc->IOCtrl.KoRun_2_Brake;	//科润正转
		p2IO->KoRun_2_SysPower	.OutValue = p2Acc->IOCtrl.KoRun_2_SysPower;	//科润系统电源
		p2IO->KoRun_2_Contactor	.OutValue = p2Acc->IOCtrl.KoRun_2_Contactor;//科润接触器
	}

	//推送机构的控制
	{
		p2IO->PushRollerBrake	.OutValue = p2Acc->IOCtrl.PushRollerBrake;
	}

	//避障雷达切区控制输出
	{
        p2IO->ObsRadar_F_Ctrl1.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[0].ObsRadarCtrl1;
        p2IO->ObsRadar_F_Ctrl2.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[0].ObsRadarCtrl2;
        p2IO->ObsRadar_F_Ctrl3.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[0].ObsRadarCtrl3;
        p2IO->ObsRadar_F_Ctrl4.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[0].ObsRadarCtrl4;

        p2IO->ObsRadar_B_Ctrl1.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[1].ObsRadarCtrl1;
        p2IO->ObsRadar_B_Ctrl2.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[1].ObsRadarCtrl2;
        p2IO->ObsRadar_B_Ctrl3.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[1].ObsRadarCtrl3;
        p2IO->ObsRadar_B_Ctrl4.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[1].ObsRadarCtrl4;
		
		p2IO->ObsRadar_L_Ctrl1.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[2].ObsRadarCtrl1;
        p2IO->ObsRadar_L_Ctrl2.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[2].ObsRadarCtrl2;
        p2IO->ObsRadar_L_Ctrl3.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[2].ObsRadarCtrl3;
        p2IO->ObsRadar_L_Ctrl4.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[2].ObsRadarCtrl4;

        p2IO->ObsRadar_R_Ctrl1.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[3].ObsRadarCtrl1;
        p2IO->ObsRadar_R_Ctrl2.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[3].ObsRadarCtrl2;
        p2IO->ObsRadar_R_Ctrl3.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[3].ObsRadarCtrl3;
        p2IO->ObsRadar_R_Ctrl4.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[3].ObsRadarCtrl4;
		
		p2IO->ObsRadar_F_Inclined_Ctrl1.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[4].ObsRadarCtrl1;
        p2IO->ObsRadar_F_Inclined_Ctrl2.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[4].ObsRadarCtrl2;
        p2IO->ObsRadar_F_Inclined_Ctrl3.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[4].ObsRadarCtrl3;
        p2IO->ObsRadar_F_Inclined_Ctrl4.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[4].ObsRadarCtrl4;
		
		p2IO->ObsRadar_B_Inclined_Ctrl1.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[5].ObsRadarCtrl1;
        p2IO->ObsRadar_B_Inclined_Ctrl2.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[5].ObsRadarCtrl2;
        p2IO->ObsRadar_B_Inclined_Ctrl3.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[5].ObsRadarCtrl3;
        p2IO->ObsRadar_B_Inclined_Ctrl4.OutValue = p2Acc->IOCtrl.ObsRadarCtrl[5].ObsRadarCtrl4;
	}
	//内部卷线盘相关
	{
		p2IO->CableRoll_ForcePowerOn.OutValue 	= p2Acc->IOCtrl.CableRoll_ForcePowerOn;
		p2IO->CableRoll_Reel.OutValue 			= p2Acc->IOCtrl.CableRoll_Reel;

	}
	//联动相关
	{
		p2IO->LandscapeLaser.OutValue = p2Acc->IOCtrl.LandscapeLaser;
	}
}

/***********************************************************
** 函数名称:	ACC_CtrlExecute_SVO
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//ACC的伺服控制同步到canopen伺服对象中
void ACC_CtrlExecute_SVO(void)
{
	ACC_DATA_TypeDef 	*p2Acc	= &AccData;
	u8 i=0;

	/*行走电机的速度 从解算对象输出到acc对象*/
	{
		if(!g_Para.Debug.ForceSVODisable)
		{
			for (i = 0; i < g_MotorResolving.Para.MotorNumber; i++)
			{
				SVO[i].En = p2Acc->WalkMotor[i].En;//

				if(g_WMSM.En == 0)//直接赋值
				SVO[i].CMDSpeed = p2Acc->WalkMotor[i].V;
				else//平滑处理
				{
					//SVO[i].CMDSpeed = DataSmoothing_Main(&(g_WMSM.SM[i]), p2Acc->in_Ms, p2Acc->WalkMotor[i].V);
					SVO[i].CMDSpeed = g_WMSM.SM[i].out_Value;
					//g_WMSM.RunFlg = 1;
				}
			}
		}
		else//强制失能
		{
			for (i = 0; i < g_MotorResolving.Para.MotorNumber; i++)
			{
				SVO[i].En = 0;//
				SVO[i].CMDSpeed = 0;
			}
		}
	}

	/*举升电机的速度 从解算对象输出到acc对象*/
	{
		uint8_t Cyc_Num		= g_LiftData.CtrlPara.CtrlNnm;
		uint8_t Walk_Num	= g_MotorResolving.Para.MotorNumber;
		if(!g_Para.Debug.ForceSVODisable_Lift)
		{
			if(g_Para.AppOutSource.FucBit.DACLifeMoter != 0X00)
			{
				//模拟量对象同步到DA接口
				g_DAFucs.KoRunCtrl.OrgCmd = p2Acc->LiftMotor[0].V;/*升降电机速度送入DAC的数据输入管道*/
			}
			else
			{
				for (i = 0; i < Cyc_Num; i++)
				{
					SVO[i+Walk_Num].En = p2Acc->LiftMotor[i].En;
					SVO[i+Walk_Num].CMDSpeed = p2Acc->LiftMotor[i].V;
				}
			}
		}
		else
		{
			if(g_Para.AppOutSource.FucBit.DACLifeMoter != 0X00)
			{
				g_DAFucs.KoRunCtrl.OrgCmd = 0;//升降电机速度
			}
			else
			{
				for (i = 0; i < Cyc_Num; i++)
				{
					SVO[i+Walk_Num].CMDSpeed = 0;
				}
			}
		}
	}
	/*推送电机的速度 从解算对象输出到acc对象*/
	{
		uint8_t Cyc_Num		= g_Push_Pull.PR_CtrlPara.CtrlNum;
		uint8_t Star_Num	= g_MotorResolving.Para.MotorNumber+g_LiftData.CtrlPara.CtrlNnm;
		if(!g_Para.Debug.ForceSVODisable_Lift)
		{
			for (i = 0; i < Cyc_Num; i++)
			{
				SVO[i+Star_Num].En = p2Acc->PushRollerMotor[i].En;
				SVO[i+Star_Num].CMDSpeed = p2Acc->PushRollerMotor[i].V;
			}
		}
		else
		{
			for (i = 0; i < Cyc_Num; i++)
			{
				SVO[i+Star_Num].CMDSpeed = 0;
			}
		}
	}
}

//电机控制->安川m3板卡
bool ACC_CtrlExecute_SVO2ANC(void)
{
	u8 i = 0;
	//总安川伺服数量
	u8 ANC_Num = g_LiftData.CtrlPara.CtrlNnm + g_MotorResolving.Para.MotorNumber;

	for (i = 0; i < ANC_Num; i++)
	{
		if (SVO[i].En)
			M_K_SET_BIT(EQP_ANC.p2WData->En, i);
		else
			M_K_CLR_BIT(EQP_ANC.p2WData->En, i);

		EQP_ANC.p2WData->VCmd[i] = SVO[i].CMDSpeed;
	}

	return 1;
}

/***********************************************************
** 函数名称:	ACC_CtrlExecute_EQP
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void ACC_CtrlExecute_EQP(void)
{
	//语音播报控制结果送到 通信对象
	//根据参数关闭故障报警语音
	{
		if (g_Para.SysFucBit.ErrVoice == 0 && g_VoicePlayer.Voice_ID_Cmd > Voice_Err_Sys)
		{
			/*根据全局的配置参数来选择不同的输出数据管道*/
			if(g_Para.SerialExpObjSource.Voice == 1)
			{
				EQP_SerialExp.p2Data.VoiceData->BEEP_SongNumber = Voice_Null;
				EQP_SerialExp.p2Data.VoiceData->BEEP_SongVol 	= 0;
			}
			else
			{
				EQP_Voice.WD[0] = Voice_Null;
				EQP_Voice.WD[1] = 0;
			}
		}
		else
		{
			if(g_Para.SerialExpObjSource.Voice == 1)
			{
				EQP_SerialExp.p2Data.VoiceData->BEEP_SongNumber = g_VoicePlayer.Voice_ID_Cmd;
				EQP_SerialExp.p2Data.VoiceData->BEEP_SongVol 	= g_VoicePlayer.Music_Volume;
			}
			else
			{
				EQP_Voice.WD[0] = g_VoicePlayer.Voice_ID_Cmd;
				EQP_Voice.WD[1] = g_VoicePlayer.Music_Volume;
			}
		}
	}
	
}
//not all the io def in AccCore
//AGV底盘控制执行同步
void ACC_CtrlExecute(void)
{
	//ACC到硬件IO
	ACC_CtrlExecute_IO_Out();

	//ACC到硬件伺服
	ACC_CtrlExecute_SVO();
	//电机控制->安川m3板卡
	if (CanMaster.Para.SVOType == Can_SVOType_Null)
		ACC_CtrlExecute_SVO2ANC();

	//ACC到硬件EQP
	ACC_CtrlExecute_EQP();

	//过载力矩限制
	LoadCntTorqueLimit();
}


/**************************************************************************
* 函数名:ACC_PowerOn_Input
* 功  能:上电过程控制输入
* 参  数:
* 返回值:NC
**************************************************************************/
void ACC_PowerOn_Input()
{
//	ACC_DATA_TypeDef* p2Acc = &AccData;
	ACC_PowerOn_TypeDef* p2PowerOn = &g_PowerOn;

	//急停输入
	//p2PowerOn->in_CarEmgStop = (!g_IOInputs.EmgStop.StableValue);
	
	//电池就绪输入
//	if(g_BMS.Voltage>20 && g_BMS.CommOkCnt>10 && g_BMS.TimeOutMs<1000)
	if(EQPMsts.BMS_CATL.CommId != CCFCN_Disable && g_BMSData.BMS_Read.BMS_Ready == 1)
		p2PowerOn->in_BatteryReady = 1;
	else if(EQPMsts.BMS_CATL.CommId == CCFCN_Disable && g_BMSData.BMS_Read.Voltage > 20)
		p2PowerOn->in_BatteryReady = 1;
	else {}

	//控制电压就绪输入
	if(g_ADFucs.SysCtrlVoltage.Fiter.out_FilterValue > 182)
		p2PowerOn->in_ControlVoltageReady = 1;
	else {}

	//canopen状态输入
	if(CanMaster.CanopenStep == Step_Active)
		p2PowerOn->in_CanopenStep = ACC_CanopenStep_Active;
	else if(CanMaster.CanopenStep==Step_CanOpenFault)
		p2PowerOn->in_CanopenStep =	ACC_CanopenStep_Fault;
	else if(CanMaster.CanopenStep >= Step_DrivePowerUp)
		p2PowerOn->in_CanopenStep = ACC_CanopenStep_Precharge;
	else {}
}

//声光提示输入
void ACC_SoundLightNotice_Input(ACC_DATA_TypeDef *p)
{
	char temp = 0;

	//ms输入
	g_SLN.in_Ms=p->in_Ms;

	//系统状态输入
	if(p->FaultsResponseSTM>=ACC_FaultsType_ControlledStop)
		g_SLN.in_SystemState=SLN_SystemStat_Error;
	else if(p->FaultsResponseSTM>=ACC_FaultsType_Warn)
		g_SLN.in_SystemState=SLN_SystemStat_Warning;
	else
		g_SLN.in_SystemState=SLN_SystemStat_Normal;

	//模式状态输入
	if(g_ChannelSwitch.STM_Channel==ACCCC_Auto)
		g_SLN.in_CtrlMode = SLN_CtrlMode_Auto;	
	else
		g_SLN.in_CtrlMode = SLN_CtrlMode_Manual;	
	
	//工作模式输入
	if(ACC_ModeSMTIsWalk(p))
		g_SLN.in_WorkMode = SLN_WorkMode_Walking;	
	else if(  p->CalCmd.Mode==ACCMode_Lift
			||p->CalCmd.Mode==ACCMode_Push
			||p->CalCmd.Mode==ACCMode_Roller)
		g_SLN.in_WorkMode = SLN_WorkMode_Lifting;
	else
		g_SLN.in_WorkMode = SLN_WorkMode_Still;

	//避障状态输入
	g_SLN.in_ObsState = GetObsState();

	//模组单动标定中检出
	temp = (g_MR_ManualCtrl.Stm == MR_ManualCtrl_Stm_Run && p->OrgCmd.Vxyz.z != 0);
	//运动中状态输入
	g_SLN.in_FlgMotion = CheckMotion(p) || temp;

	//分别在模式切换完成中置位，提示完成自动复位
	//g_SLN.in_FlgModeSwitchOk = false;		

	g_SLN.in_FlgWaitForConf = false;							
	
	//到位状态输入						
	g_SLN.in_FlgMotionArrive = CheckMotionArrive(p);	

	/*自由运动时发现横向或正向满足巡线条件时*/
	{
		if(	   p->ModeSwitchSMT == ACCMode_PosFlexible 
			|| p->ModeSwitchSMT == ACCMode_InfFlexible 
			|| p->ModeSwitchSMT == ACCMode_Rotation)
		{
			//正向或横向满足巡线条件（发现磁条）
			if(	   (g_MNData[0].MN_Read.Status != 0 && g_MNData[1].MN_Read.Status != 0)
				|| (g_MNData[2].MN_Read.Status != 0 && g_MNData[3].MN_Read.Status != 0))
			{
				g_SLN.in_FlgAutoReady = true;
			}
			//正向或横向满足巡线条件（发现色带）
			else if(   (g_ColorBand[0].ColorBand_output.Status != 0 && g_ColorBand[1].ColorBand_output.Status != 0)
					|| (g_ColorBand[2].ColorBand_output.Status != 0 && g_ColorBand[3].ColorBand_output.Status != 0)
			)
			{
				g_SLN.in_FlgAutoReady = true;
			}
			else
			{
				g_SLN.in_FlgAutoReady = false;
			}
		}
		else
		{
			g_SLN.in_FlgAutoReady = false;
		}
	}

    //巡线时发现十字磁条，或二维码，或rfid
    if(ACC_IsTraceMode(AccData.CalCmd.Mode))
    {
        if(
            (   g_MNData[0].MN_Read.Status!= 0 && g_MNData[1].MN_Read.Status!= 0
             && g_MNData[2].MN_Read.Status!= 0 && g_MNData[3].MN_Read.Status!= 0)
            || g_RFIDData.RFID_Read.Status	!= 0
            || g_ToolPGV.PGV_Read.Status 	!= 0
			|| g_GroundPGV.PGV_Read.Status 	!= 0
        )
            g_SLN.in_FlgAutoDoing = true;
        else
            g_SLN.in_FlgAutoDoing = false;
    }
    else
        g_SLN.in_FlgAutoDoing = false;
    

    //没有在运动中，清除自动状态提醒
    if(!g_SLN.in_FlgMotion)
    {
        g_SLN.in_FlgAutoReady = false;
        g_SLN.in_FlgAutoDoing = false;
    }
	//对正条件是否有效提示
	if(g_SLAM_CoordinatePoint.Output.InExecutionFlag
		||g_ToolJustify.Output.InExecutionFlag
		||g_GroundJustify.Output.InExecutionFlag
		||g_UltrasonicJustify.Output.InExecutionFlag
		||g_LinkageJustify.Output.InExecutionFlag)
	{
		g_SLN.in_FlgAutoJustify = true;
	}
	else
	{
		g_SLN.in_FlgAutoJustify = false;
	}	

}

//功能模块输出统一到ACC输出
void ACC_UniformControl(ACC_DATA_TypeDef *p)
{
	SoundLightNotice_TypeDef *p2SLN=&g_SLN;
	if(g_MC_WorkStep!=MC_WorkStep_Run)//启动期间由启动管理对象管理控制,且状态保持传递到ACC控制中
	{
		p->IOCtrl.Precharge			=g_PowerOn.Ctrl_Precharge;
		p->IOCtrl.MainContactor		=g_PowerOn.Ctrl_MainContactor;
		p->IOCtrl.CtrllPower		=g_PowerOn.Ctrl_CtrllPower;
		p->IOCtrl.MainDCDC			=g_PowerOn.Ctrl_MainDCDC;
		p->IOCtrl.CtrlVoltageBypass	=g_PowerOn.Ctrl_CtrlVoltageBypass;
		p->IOCtrl.PowerOnBypass		=g_PowerOn.Ctrl_PowerOnBypass;
		//驱动器预充/主接触器
		p->IOCtrl.MotorPrecharge	=g_PowerOn.Ctrl_MotorPrecharge;
		p->IOCtrl.MotorContactor	=g_PowerOn.Ctrl_MotorContactor;
		p->IOCtrl.ANC				=g_PowerOn.Ctrl_ANC;
	}
	else //启动正常后，由ACC对象管理控制
	{
		
	}

	//兼容声光提示接PLC
	//声光提示
	if (g_IOOutputs.LightPower.ID != OCN_Disable)
	{
		p->IOCtrl.LightPower = p2SLN->out_LightPower;
	}
	else if (g_Para.SerialExpObjSource.PLC == 1)
	{
		EQP_SerialExp.p2Data.PLCData->PLC_powerLight = p2SLN->out_LightPower;
	}
	else if (g_Para.EQPCommSource.EN.PLC == 1)
	{
		EQP_PLC.WD[1] = p2SLN->out_LightPower;//临时处理，待上车实测确认
	}
	else {}

	if (g_IOOutputs.LightSystem.ID != OCN_Disable)
	{
		p->IOCtrl.LightSystem = p2SLN->out_LightSystem;
	}
	else if (g_Para.SerialExpObjSource.PLC == 1)
	{
		EQP_SerialExp.p2Data.PLCData->PLC_sysLight = p2SLN->out_LightSystem;
	}
	else if (g_Para.EQPCommSource.EN.PLC == 1)
	{
		EQP_PLC.WD[0] = p2SLN->out_LightSystem;
	}
	else {}

	//报警指示灯兼顾运行提示
	if (g_Para.SysFucBit.Buzzer2LightAlarm)
	{
		//没有报警时，把蜂鸣器送到报警输出
		//if(0)//p2SLN->out_LightAlarm)
		if (AccData.CalCmd.Mode == ACCMode_Still)
			//if(AccData.CalCmd.Mode==ACCMode_Still && 0)//g_SLN.in_FlgModeSwitchOk==0)//解决切静止时蜂鸣器不响问题,in_FlgModeSwitchOk判断不知为何失效
		{
			if (g_IOOutputs.LightAlarm.ID != OCN_Disable)
			{
			p->IOCtrl.LightAlarm = p2SLN->out_LightAlarm;
			}
			else if (g_Para.SerialExpObjSource.PLC == 1)
			{
				EQP_SerialExp.p2Data.PLCData->PLC_alarmLight = p2SLN->out_LightAlarm;
			}
			else if (g_Para.EQPCommSource.EN.PLC == 1)
			{
				EQP_PLC.WD[2] = p2SLN->out_LightAlarm;
			}
			else {}
		}
		else
		{
			if (g_IOOutputs.LightAlarm.ID != OCN_Disable)
			{
			p->IOCtrl.LightAlarm = p2SLN->out_Buzzer;
	}
			else if (g_Para.SerialExpObjSource.PLC == 1)
			{
				EQP_SerialExp.p2Data.PLCData->PLC_alarmLight = p2SLN->out_Buzzer;
			}
			else if (g_Para.EQPCommSource.EN.PLC == 1)
			{
				EQP_PLC.WD[2] = p2SLN->out_Buzzer;
			}
			else {}
		}
	}
	else
	{
		if (g_IOOutputs.LightAlarm.ID != OCN_Disable)
		{
		p->IOCtrl.LightAlarm = p2SLN->out_LightAlarm;
                }
		else if (g_Para.SerialExpObjSource.PLC == 1)
		{
			EQP_SerialExp.p2Data.PLCData->PLC_alarmLight = p2SLN->out_LightAlarm;
		}
		else if (g_Para.EQPCommSource.EN.PLC == 1)
		{
			EQP_PLC.WD[2] = p2SLN->out_LightAlarm;
		}
		else {}
	}

	p->IOCtrl.LampBlue		= p2SLN->out_LampBlue;
	p->IOCtrl.LampGreen		= p2SLN->out_LampGreen;
	p->IOCtrl.LampRed		= p2SLN->out_LampRed;
	p->IOCtrl.Buzzer		= p2SLN->out_Buzzer;
	p->IOCtrl.LightCaution	= p2SLN->out_LightCaution;

	//避障雷达切区控制输出

	//风扇
	p->IOCtrl.CoolingFan 	= g_Fan.FanCtrl;

	//遥控器即时功能执行
	p->IOCtrl.Horn 			= AccData.CalCmd.Button.Horn;		//喇叭
	p->IOCtrl.Lighting 		= AccData.CalCmd.Button.Lighting;	//照明
	
	//充电命令及时打开极板
	p->IOCtrl.ChargingMC 	= p->CalCmd.Button.ChargingMC;
}

//ACC实时输入更新任务，不同项目在此方法内修改输入的不同来源
bool ACC_FanCtrl(void)
{
	g_Fan.in_Ms=AccData.in_Ms;

	//优先驱动最高温度、再107核心温度
	if(g_Fan.Mode == FanCtrl_Mode_Temperature)
		g_Fan.in_Temperature=SVOs.Temp.Max;
	
	//环境温度原始值输入
	g_Fan.in_Temperature_Environment = EQP_Thermodetector.RD[0];
	g_Fan.out_Temperature_Environment = ((s16)g_Fan.in_Temperature_Environment) / 100; //传感器手册比例系数为100

	FanCtrl(&g_Fan);

	return 1;
}

//ACC 1秒任务
bool ACC_Tsk1s(void)
{
	SVOs_Analysis();//伺服分析
	ACC_FanCtrl();//风扇控制
	
	return 1;
}

/***********************************************************
** 函数名称:	ACC_ROA_RealTimeInput
** 功能描述: 雷达避障状态io输入 实时值
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//雷达避障状态io输入 实时值
void ACC_ROA_RealTimeInput(void)
{
	ACC_DATA_TypeDef *p2Acc = &AccData;
	

	/*io输入的雷达， 这种情况下 最多只允许4个雷达*/
	if(g_Para.AppInSource.FucBit.ModBusROA	== 0)
	{
		//由于雷达被设置未常闭输出：即有障碍时输出0，无障碍时输出1；与CBB雷达避障逻辑相反
		//所以这里取反
		p2Acc->InputIO.ObsRadarInput[0].ObsRadarInner 	= !g_IOInputs.ObsRadar_F_Inner.RealtimeValue;
		p2Acc->InputIO.ObsRadarInput[0].ObsRadarMiddle	= !g_IOInputs.ObsRadar_F_Middle.RealtimeValue;
		p2Acc->InputIO.ObsRadarInput[0].ObsRadarOuter	= !g_IOInputs.ObsRadar_F_Outer.RealtimeValue;

		p2Acc->InputIO.ObsRadarInput[1].ObsRadarInner 	= !g_IOInputs.ObsRadar_B_Inner.RealtimeValue;
		p2Acc->InputIO.ObsRadarInput[1].ObsRadarMiddle 	= !g_IOInputs.ObsRadar_B_Middle.RealtimeValue;
		p2Acc->InputIO.ObsRadarInput[1].ObsRadarOuter	= !g_IOInputs.ObsRadar_B_Outer.RealtimeValue;

		p2Acc->InputIO.ObsRadarInput[2].ObsRadarInner	= !g_IOInputs.ObsRadar_L_Inner.RealtimeValue;
		p2Acc->InputIO.ObsRadarInput[2].ObsRadarMiddle	= !g_IOInputs.ObsRadar_L_Middle.RealtimeValue;
		p2Acc->InputIO.ObsRadarInput[2].ObsRadarOuter	= !g_IOInputs.ObsRadar_L_Outer.RealtimeValue;

		p2Acc->InputIO.ObsRadarInput[3].ObsRadarInner	= !g_IOInputs.ObsRadar_R_Inner.RealtimeValue;
		p2Acc->InputIO.ObsRadarInput[3].ObsRadarMiddle	= !g_IOInputs.ObsRadar_R_Middle.RealtimeValue;
		p2Acc->InputIO.ObsRadarInput[3].ObsRadarOuter	= !g_IOInputs.ObsRadar_R_Outer.RealtimeValue;
		
		p2Acc->InputIO.ObsRadarInput[4].ObsRadarInner	= !g_IOInputs.ObsRadar_F_Inclined_Inner.RealtimeValue;
		p2Acc->InputIO.ObsRadarInput[4].ObsRadarMiddle	= !g_IOInputs.ObsRadar_F_Inclined_Middle.RealtimeValue;
		p2Acc->InputIO.ObsRadarInput[4].ObsRadarOuter	= !g_IOInputs.ObsRadar_F_Inclined_Outer.RealtimeValue;
		
		p2Acc->InputIO.ObsRadarInput[5].ObsRadarInner	= !g_IOInputs.ObsRadar_B_Inclined_Inner.RealtimeValue;
		p2Acc->InputIO.ObsRadarInput[5].ObsRadarMiddle	= !g_IOInputs.ObsRadar_B_Inclined_Middle.RealtimeValue;
		p2Acc->InputIO.ObsRadarInput[5].ObsRadarOuter	= !g_IOInputs.ObsRadar_B_Inclined_Outer.RealtimeValue;

		//如果雷达是HE 3091 ERR信号是常开的
		if(g_Para.ROA.Brand.Hinson_HE_3091 == 1 || g_Para.ROA.Brand.Sick_XXX_XXX == 1)
		{
			p2Acc->InputIO.ObsRadarInput[0].ObsRadarErr = !g_IOInputs.ObsRadar_F_Err.StableValue;
			p2Acc->InputIO.ObsRadarInput[1].ObsRadarErr = !g_IOInputs.ObsRadar_B_Err.StableValue;
			p2Acc->InputIO.ObsRadarInput[2].ObsRadarErr = !g_IOInputs.ObsRadar_L_Err.StableValue;
			p2Acc->InputIO.ObsRadarInput[3].ObsRadarErr = !g_IOInputs.ObsRadar_R_Err.StableValue;
			p2Acc->InputIO.ObsRadarInput[4].ObsRadarErr = !g_IOInputs.ObsRadar_F_Inclined_Err.StableValue;
			p2Acc->InputIO.ObsRadarInput[5].ObsRadarErr = !g_IOInputs.ObsRadar_B_Inclined_Err.StableValue;
		}
		//如果是LE 50821 没有Err信号输入，直接强制为0
		else if(g_Para.ROA.Brand.Hinson_LE_50821 == 1)
		{
			p2Acc->InputIO.ObsRadarInput[0].ObsRadarErr = 0;
			p2Acc->InputIO.ObsRadarInput[1].ObsRadarErr = 0;
			p2Acc->InputIO.ObsRadarInput[2].ObsRadarErr = 0;
			p2Acc->InputIO.ObsRadarInput[3].ObsRadarErr = 0;
			p2Acc->InputIO.ObsRadarInput[4].ObsRadarErr = 0;
			p2Acc->InputIO.ObsRadarInput[5].ObsRadarErr = 0;
		}
		//其他方式的雷达err是常开信号与CBB雷达避障逻辑一致，所以err信号不取反！
		else
		{
			p2Acc->InputIO.ObsRadarInput[0].ObsRadarErr = g_IOInputs.ObsRadar_F_Err.StableValue;
			p2Acc->InputIO.ObsRadarInput[1].ObsRadarErr = g_IOInputs.ObsRadar_B_Err.StableValue;
			p2Acc->InputIO.ObsRadarInput[2].ObsRadarErr = g_IOInputs.ObsRadar_L_Err.StableValue;
			p2Acc->InputIO.ObsRadarInput[3].ObsRadarErr = g_IOInputs.ObsRadar_R_Err.StableValue;
			p2Acc->InputIO.ObsRadarInput[4].ObsRadarErr = g_IOInputs.ObsRadar_F_Inclined_Err.StableValue;
			p2Acc->InputIO.ObsRadarInput[5].ObsRadarErr = g_IOInputs.ObsRadar_B_Inclined_Err.StableValue;
		}
	}
	/*Modbus通讯输入的雷达， 这种情况下 最多只允许6个雷达*/
	else
	{
		//输出从导航转接板 来的
		if(1)
		{
			EQP_HinsonRadarObj_StDef* p = EQP_Hinson_Radar_Front.p2Data;
			p2Acc->InputIO.ObsRadarInput[0].ObsRadarErr 	= p->InputIO_Err;
			p2Acc->InputIO.ObsRadarInput[0].ObsRadarInner 	= !(p->InputIO_1);
			p2Acc->InputIO.ObsRadarInput[0].ObsRadarMiddle	= !(p->InputIO_2);
			p2Acc->InputIO.ObsRadarInput[0].ObsRadarOuter	= !(p->InputIO_3);
			p->SetRegion = g_ROA.ObstacleAreaSelect;
			
			
			p = EQP_Hinson_Radar_Rear.p2Data;
			p2Acc->InputIO.ObsRadarInput[1].ObsRadarErr 	= p->InputIO_Err;
			p2Acc->InputIO.ObsRadarInput[1].ObsRadarInner 	= !(p->InputIO_1);
			p2Acc->InputIO.ObsRadarInput[1].ObsRadarMiddle	= !(p->InputIO_2);
			p2Acc->InputIO.ObsRadarInput[1].ObsRadarOuter	= !(p->InputIO_3);
			p->SetRegion = g_ROA.ObstacleAreaSelect;
			
			p = EQP_Hinson_Radar_Left.p2Data;
			p2Acc->InputIO.ObsRadarInput[2].ObsRadarErr 	= p->InputIO_Err;
			p2Acc->InputIO.ObsRadarInput[2].ObsRadarInner 	= !(p->InputIO_1);
			p2Acc->InputIO.ObsRadarInput[2].ObsRadarMiddle	= !(p->InputIO_2);
			p2Acc->InputIO.ObsRadarInput[2].ObsRadarOuter	= !(p->InputIO_3);
			p->SetRegion = g_ROA.ObstacleAreaSelect;
			
			p = EQP_Hinson_Radar_Right.p2Data;
			p2Acc->InputIO.ObsRadarInput[3].ObsRadarErr 	= p->InputIO_Err;
			p2Acc->InputIO.ObsRadarInput[3].ObsRadarInner 	= !(p->InputIO_1);
			p2Acc->InputIO.ObsRadarInput[3].ObsRadarMiddle	= !(p->InputIO_2);
			p2Acc->InputIO.ObsRadarInput[3].ObsRadarOuter	= !(p->InputIO_3);
			p->SetRegion = g_ROA.ObstacleAreaSelect;
			
			p = EQP_Hinson_Radar_FrontUp.p2Data;
			p2Acc->InputIO.ObsRadarInput[4].ObsRadarErr 	= p->InputIO_Err;
			p2Acc->InputIO.ObsRadarInput[4].ObsRadarInner 	= !(p->InputIO_1);
			p2Acc->InputIO.ObsRadarInput[4].ObsRadarMiddle	= !(p->InputIO_2);
			p2Acc->InputIO.ObsRadarInput[4].ObsRadarOuter	= !(p->InputIO_3);
			p->SetRegion = g_ROA.ObstacleAreaSelect;
			
			p = EQP_Hinson_Radar_RearUp.p2Data;
			p2Acc->InputIO.ObsRadarInput[5].ObsRadarErr 	= p->InputIO_Err;
			p2Acc->InputIO.ObsRadarInput[5].ObsRadarInner 	= !(p->InputIO_1);
			p2Acc->InputIO.ObsRadarInput[5].ObsRadarMiddle	= !(p->InputIO_2);
			p2Acc->InputIO.ObsRadarInput[5].ObsRadarOuter	= !(p->InputIO_3);
			p->SetRegion = g_ROA.ObstacleAreaSelect;
		}
	}
}

//清除所有故障
bool SysClearAllErr(void)
{
	//清除所有故障锁存
	g_Faults.LoseControl.All = 0;
	g_Faults.TimeOut.All = 0;
	g_Faults.EmgStop.All = 0;
	g_Faults.Lift.All = 0;
	g_Faults.Func.All = 0;
	g_Faults.Auto.All = 0;
	g_Faults.Other.All = 0;

	//清除所有故障的触发源：CBB里的故障标志、故障检出的状态机、故障检出的计数等
	//清除舵轮解算超限CBB标记
	g_MotorResolving.Output.CtrlAngLimit_Flg = 0;
	//复位磁条离线检出状态机与计数
	g_Faults.MagOutLineTime[0] = 0;
	g_Faults.MagOutLineTime[1] = 0;
	g_Faults.MagOutLineTime[2] = 0;
	g_Faults.MagOutLineTime[3] = 0;
	g_Faults.TraceOutline.STM = TraceOutline_Move;//以便能够继续行走，不是直接马上停止
	//清除模式切换超时计数
	AccData.ModeSwitch.MsRecord_Steering = 0;

	//清除升降超时报警
	g_LiftData.WarningOut.ExecTimeout = 0;//清除超时报警标记
	g_LiftData.Cmd.ExecTimeoutStartMs = g_Sys_1ms_Counter;//清除超时计时

	//清除悬挂超时报警
	g_AbsorptionData.WarningOut.ExecTimeout = 0;//清除超时报警标记
	g_AbsorptionData.Cmd.ExecTimeoutStartMs = g_Sys_1ms_Counter;//清除超时计时

	//清除拓展异常对象
	CodeLogicErr.LogicErr.All = 0;

	g_SysApi.ClearAllErr = 0;

	return 1;
}

//系统API响应
bool SysApiResponse(void)
{
    /*调试时，外部抱闸选择暂时用于伺服使能 / 失能*/
    if(g_IOInputs.ExternalBrakeSel.StableValue)
    {
        g_Para.Debug.ForceSVODisable = 1;
        g_Para.Debug.ForceSVODisable_Lift = 1;
    }
    else
    {
        g_Para.Debug.ForceSVODisable = 0;
        g_Para.Debug.ForceSVODisable_Lift = 0;
    }

	//急停
	if(g_SysApi.EStop)
	{
		//...
		g_SysApi.EStop = 0;
	}
	else {}

	//关闭所有输出
	if(g_SysApi.CloseAllOutput)
	{
		//...
		g_SysApi.CloseAllOutput = 0;
	}
	else {}

	//系统复位
	if(g_SysApi.SysReset)
	{
		AccReset();
		g_SysApi.SysReset = 0;
	}
	else {}

	//系统重启
	if(g_SysApi.SysReboot)
	{
		SystemReset();
		g_SysApi.SysReboot = 0;
	}
	else {}

	//标定轮系角度零点
	if(g_SysApi.WheelAngleZeroSet)
	{
		WheelAngleZeroSet();
		g_SysApi.WheelAngleZeroSet = 0;
	}
	else {}


	//升降标定 高点
	if(g_SysApi.LiftHighSet)
	{
		LiftHighSet();
		g_SysApi.LiftHighSet = 0;
	}
	else {}

	//升降标定 低点
	if(g_SysApi.LiftLowSet)
	{
		LiftLowSet();
		g_SysApi.LiftLowSet = 0;
	}
	else {}

	//清除所有故障 危险硬件故障不响应！
	//注意：这里是清除一次 非一直强制关闭
	if (g_SysApi.ClearAllErr)
	{
		SysClearAllErr();
	}
	else {}

	//保存eep参数涉及到eep操作，在原来的apptsk位置

	/*获取EEPROM数据*/
	if(EEPCtrl.EEPObj2Sys)
	{
		ParaInit_EEPObj2Sys();
		ObjInit_HMI();
		EEPCtrl.EEPObj2Sys=0;
	}
	else {}

	//从项目代码中获取数据到系统数据,并写一次hmi对象
	if(EEPCtrl.Prj2Sys)
	{
		//根据设备ID初始化编译预制参数
		if(g_Para.p2Fucs.p2PrjParaInit)
		    g_Para.p2Fucs.p2PrjParaInit();
		
		//初始化HMI对象
		ObjInit_HMI();

		EEPCtrl.Prj2Sys=0;
	}
	else {}

	//系统参数对象 写入 EEPROM对象
	if(EEPCtrl.Sys2EEObj)
	{
		ParaInit_Sys2EEPObj();
		EEPCtrl.Sys2EEObj=0;
	}
	else {}

	//EEPROM对象 写入 系统参数对象
	if(EEPCtrl.EEPObj2Sys)
	{
		ParaInit_EEPObj2Sys();
		EEPCtrl.EEPObj2Sys=0;
	}
	else {}

	//系统对象 写入 EEPROM
	if (EEPCtrl.Sys2EEP)
	{
		ParaInit_Sys2EEPObj();//系统参数到EEPROM对象
		EEPCtrl.EEPObj2EEP = 1;
		EEPCtrl.Sys2EEP = 0;
	}
	else {}

	return 1;
}



//自动运行对象中检出相应的系统API命令
bool GetSysApi_Auto()
{
	//上位机防撞触边恢复
	if(g_DHR_AGVInformation[Para_Index_AGV_AntiCcollisionRecovery] == 0x01)
	{
		g_SysApi.ClearObsBarLatch = 1;
		g_DHR_AGVInformation[Para_Index_AGV_AntiCcollisionRecovery] = 0x00;	 
	}

	//上位机清除故障
	if(M_TST_BIT(g_DHR_AGVInformation[Para_Index_AGV_ClearAllAlalrm],0))//1/2待实现		
	{		
		g_SysApi.ClearObsBarLatch = 1;
		M_K_CLR_BIT(g_DHR_AGVInformation[Para_Index_AGV_ClearAllAlalrm],0);
	}


	return 1;
}

//急停恢复重启系统
bool EmgStopReset(void)
{
    //if (g_IOInputs.EmgStop.StableValue == 0)
    {
        static bool s_FlgEmgStop = 0;

        //急停触发
        if (g_Faults.LoseControl.Bit.CarEstop)
            s_FlgEmgStop = 1;

        //急停触发后 又恢复，重启系统
        if (s_FlgEmgStop && g_Faults.LoseControl.Bit.CarEstop == 0)
            SystemReset();
    }
	return 1;
}

//复位被强制的功能按钮
bool SysApi_Reset(void)
{
    /*复位系统接口
    *(int*)&g_SysApi = 0;
    g_SysApi.ClearAllErr = 0;*/

    //复位hmi接口
    //*(u16*)(g_HMI.p2ButtonForce) = 0;
	memset(&(ExtHmi.p2HMI_CommObj->ButtonForce), 0, sizeof(ButtonForce_List));

    return 1;
}

//ACC_Project Ms任务接口
bool ACC_MsTsk(void)
{
	//1s任务调用
	if (g_Sys_1ms_Counter % 1000 == 0)
		ACC_Tsk1s();

	//定时复位被强制的功能按钮
	if (g_Sys_1ms_Counter % 100000 == 0)
	{
		if(g_Para.RunningMode != ACC_RunningMode_SoftwareSimulat)//纯软件仿真不定时清除强制按钮
			SysApi_Reset();
		else {}
	}
	else {}

	//ACC对象计数
	Acc_MsCnt();

	return 1;
}

//模拟器调用
bool SimulatorRun(void)
{
	//虚拟角度积分
	if (g_Para.Debug.MRSimEn)
		MotorResolving_VirAngRun(&g_MotorResolving, g_Sys_1ms_Counter);

	//升降模拟器
	if (g_Para.Debug.LiftSimEn)
		LiftSimulator(&g_LiftData);

	return 1;
}

//行走加速度限制
bool WalkLimit(void)
{
	/*联动时强制较小的加速度*/
	if (g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb)
	{
		g_SmoothVx.in_Acc = g_WalkLimit.AccMin;
		g_SmoothVy.in_Acc = g_WalkLimit.AccMin;
		g_SmoothVz.in_Acc = g_WalkLimit.AccMin;
		return 1;
	}
	else
		__asm__("nop");

    //输入管道
    //g_WalkLimit.Input_Acc = g_SmoothVx.in_Acc;
    g_WalkLimit.Input_Vx = AccData.CalCmd.VOxyz.x;
    g_WalkLimit.Input_Vy = AccData.CalCmd.VOxyz.y;
    g_WalkLimit.Input_Vz = AccData.CalCmd.VOxyz.z;
    g_WalkLimit.Input_SOC = g_BMSData.BMS_Read.SOC;
    g_WalkLimit.Input_Height = g_LiftData.Height.Avg;
    g_WalkLimit.Input_WalkGear = AccData.CalCmd.WalkGear;
    g_WalkLimit.Input_Weight = g_LoadDetection.WeightShow;
    g_WalkLimit.Input_Mode = AccData.CalCmd.Mode;

    //模块方法调用
    WalkLimit_Run(&g_WalkLimit);

    //输出管道
	if (*(u16*)(&(g_WalkLimit.ParaBit)) != 0)				//仅当功能开启时才更新限制输出
	{
		g_SmoothVx.in_Acc = g_WalkLimit.Output_Acc;
		g_SmoothVy.in_Acc = g_WalkLimit.Output_Acc;
		g_SmoothVz.in_Acc = g_WalkLimit.Output_Acc;
	}
	else {}

	/*减速增量维持不变*/
    //g_SmoothVx.in_Dec = g_WalkLimit.Output_Acc * 2;
    //g_SmoothVy.in_Dec = g_WalkLimit.Output_Acc * 2;
    //g_SmoothVz.in_Dec = g_WalkLimit.Output_Acc * 2;

    return 1;
}

//根据负载限制行走档位
bool WalkGearLimit(void)
{
	/*联动时强制2档*/
	if (g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb)
	{
		g_ChannelSwitch.CtrlCmd.WalkGear = AbsFuc(g_ChannelSwitch.CtrlCmd.WalkGear, 2);//联动限制2档
		return 1;
	}
	else
		__asm__("nop");

	//输入管道
	g_WalkGearLimit.Input_Gear = g_ChannelSwitch.CtrlCmd.WalkGear;
	g_WalkGearLimit.Input_Rely = g_LoadDetection.WeightShow;

	//模块方法调用
	GearLimit_Single_Run(&g_WalkGearLimit);

	//输出管道
	g_ChannelSwitch.CtrlCmd.WalkGear = g_WalkGearLimit.Output_Gear;

	return 1;
}

//根据高度限制行走档位
bool WalkGearLimit_Hg(void)
{
	/*联动时强制2档*/
	if (g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb)
	{
		//g_ChannelSwitch.CtrlCmd.WalkGear = AbsFuc(g_ChannelSwitch.CtrlCmd.WalkGear, 2);//前面根据负载已经限制了 这里注释避免重复
		return 1;
	}
	else
		__asm__("nop");
	
	//输入管道
	g_WalkGearLimit_Hg.Input_Gear = g_ChannelSwitch.CtrlCmd.WalkGear;
	g_WalkGearLimit_Hg.Input_Rely = g_LiftData.Height.Avg;
	
	//模块方法调用
	GearLimit_Single_Run(&g_WalkGearLimit_Hg);
	
	//输出管道
	g_ChannelSwitch.CtrlCmd.WalkGear = g_WalkGearLimit_Hg.Output_Gear;
	return 1;
}

//根据负载限制升降档位
bool LiftGearLimit(void)
{
	/*联动时强制限档*/
	if (g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb)
	{
		g_ChannelSwitch.CtrlCmd.LiftGear = AbsFuc(g_ChannelSwitch.CtrlCmd.LiftGear, 3);
		return 1;
	}
	else
		__asm__("nop");

	//输入管道
	g_LiftGearLimit.Input_Gear = g_ChannelSwitch.CtrlCmd.LiftGear;
	g_LiftGearLimit.Input_Rely = g_LoadDetection.WeightShow;
	
	//模块方法调用
	GearLimit_Single_Run(&g_LiftGearLimit);
	
	//输出管道
	g_ChannelSwitch.CtrlCmd.LiftGear = g_LiftGearLimit.Output_Gear;
	return 1;
}

//底盘数据处理监控任务
bool Tsk_Acc_Monitor(void)
{
	//底盘数据处理监控任务
	if (!g_Para.Debug.CbbUpdateOff)
	{
		if (!MC_Monitor())
		{
			//g_MC_WorkStep=MC_WorkStep_Fail;
			g_CodeErr.Bit.BackRes = 1;
		}
		else {}
	}
	else {}
	return 1;
}

//系统初始化
bool Tsk_Acc_Stm_Init(void)
{
	if (g_Para.Debug.ForcePowerOn)
		g_MC_WorkStep = MC_WorkStep_Run;//初始化完成后直接进入运行状态
	else
	{
		//系统时间到达后，进入上电启动
		//if(g_Sys_1ms_Counter>6000)
		//printf("Tsk_Acc_Stm：启动中...\n");
		g_MC_WorkStep = MC_WorkStep_PowerUp;
	}

	return 1;
}

//系统上电
bool Tsk_Acc_Stm_PowerUp(void)
{
	if (!g_Para.Debug.PowerOnInputOff)
	{
		ACC_PowerOn_Input();
	}
	else {}

	if (!g_Para.Debug.ForcePowerOn)
	{
		//上电启动控制
		//如果使用的是铅酸电池，延时3秒后再启动
		if (g_Para.BatteryType == ACC_LeadBatteries_TN48V 
			|| g_Para.BatteryType == ACC_LeadBatteries_HBK48V
			|| g_Para.BatteryType == ACC_LeadBatteries_TN72V)
		{
			if (g_Sys_1ms_Counter > 3000)
			{
				ACC_PowerOn_Process(g_Sys_1ms_Counter, &g_PowerOn);
			}
		}
		else
		{
			ACC_PowerOn_Process(g_Sys_1ms_Counter, &g_PowerOn);
		}
		

		//上电启动控制输出
		CanMaster.in_CanOpenStart = g_PowerOn.Ctrl_CanOpenStart;

		if (g_PowerOn.PowerOnStep == ACC_PowerOnStep_StartupComplete)
		{
			//printf("Tsk_Acc_Stm：启动成功...\n");
			g_MC_WorkStep = MC_WorkStep_Run;
		}
		else if (g_PowerOn.PowerOnStep == ACC_PowerOnStep_StartupFail)
		{
			//printf("Tsk_Acc_Stm：启动失败...\n");
			g_MC_WorkStep = MC_WorkStep_Fail;
		}
		else {}
	}
	else
		g_MC_WorkStep = MC_WorkStep_Run;

	return 1;
}

//联动功能模块调用 与 Acc核心控制输入更新
bool Asb_And_AccInput(void)
{
	if (AtDbug.Bit.AsbOff == 0)
	{
		if (g_Para.SysFucBit.Asb_OR == 0)
			ACC_AssemlyCtrl();
		else
			ACC_AssemlyCtrl_OnlyRemote();

		//单动联动切换时 重置计时时间
		Switch_AsbSelect_ResetCnt();
	}
	else
	{
		//直接单车控制
		if (!g_Para.Debug.ACCInputOff)
		{
			AccData.OrgCmd = g_ChannelSwitch.CtrlCmd;
			ACC_ROA_RealTimeInput();
		}
		else {}
	}

	//避障雷达输入
	if (!g_Para.Debug.ACCInputOff)
		ACC_ROA_RealTimeInput();
	else {}

	return 1;
}

//麦轮车Vxyz在平滑处理前进行三轴求和限制处理
bool Vxzy_ZoomLimit(void)
{
	//仅麦轮车才处理
	if (g_Para.ChassisType != ACC_ChassisType_Turtle)
		return 0;
	else {}

	//缩放输入
	g_ChannelSwitch.VxyzZoom.org[0] = g_ChannelSwitch.CtrlCmd.Vxyz.x;
	g_ChannelSwitch.VxyzZoom.org[1] = g_ChannelSwitch.CtrlCmd.Vxyz.y;
	g_ChannelSwitch.VxyzZoom.org[2] = g_ChannelSwitch.CtrlCmd.Vxyz.z;

	//缩放限制
	Unifor_Zoom_Sum(&g_ChannelSwitch.VxyzZoom);

	//输出管道
	g_ChannelSwitch.CtrlCmd.Vxyz.x = g_ChannelSwitch.VxyzZoom.ZoomOut[0];
	g_ChannelSwitch.CtrlCmd.Vxyz.y = g_ChannelSwitch.VxyzZoom.ZoomOut[1];
	g_ChannelSwitch.CtrlCmd.Vxyz.z = g_ChannelSwitch.VxyzZoom.ZoomOut[2];

	return 1;
}

/*超低车 横向目标角度取反时 横向模式只响应斜行不响应转弯*/
bool Inf_Inv_NoRot(void)
{
	char i = 0;
	char flg = 0;

	//非横向模式 退出
	if (ACC_IsInfWalkMode(g_ChannelSwitch.CtrlCmd.Mode) == 0)
		return 1;
	else {}

	for (i = 0; i < g_MotorResolving.Para.ModuleNumber; i++)
	{
		if (g_MotorResolving.Para.Mxy[i].Inf_Invflg)
			flg++;
		else {}
	}

	if (flg)
	{
		g_ChannelSwitch.CtrlCmd.Vxyz.z = 0;
		AccData.CalCmd.VOxyz.z = 0;
		AccData.CalCmd.AOxyz.z = 0;
		g_SmoothVz.out_Value = 0;
		g_SmoothAz.out_Value = 0;
	}
	else {}

	return 1;
}

//系统Run
bool Tsk_Acc_Stm_Run(void)
{
	//控制源切换 并解析
	CSS_Process();

	/*超低车 横向目标角度取反时 横向模式只响应斜行不响应转弯*/
	Inf_Inv_NoRot();

	//斜行和转弯命令二选一控制
	Patch_AngAndR_CmdSel();

	//根据负载限制行走档位
	WalkGearLimit();
	
	//根据高度限制行走档位
	WalkGearLimit_Hg();

	//根据负载限制升降档位
	LiftGearLimit();

	//麦轮车Vxyz在平滑处理前进行三轴求和限制处理
	//注意在这里处理是为了兼容类似438项目将Vxyz通过联动通信传给从车的情况
	Vxzy_ZoomLimit();

	//自动任务时 系统api响应
	if (g_ChannelSwitch.STM_Channel == ACCCC_Auto)
		GetSysApi_Auto();
	else {}
		
	//自动对中功能
	AutoCentre_Ctrl();

	//联动功能模块调用 与 Acc核心控制输入更新
	Asb_And_AccInput();
	
	//超低车 卷线盘相关功能
	ACC_CableRoll_Ctrl();

	//雷赛伺服的相关功能
	ACC_LeadShineCtrl(&g_LeadShineCtrl);

	//底盘控制模块调用
	if (g_Para.Debug.AccCoreStop == 0)
		AgvChassisControl(g_Sys_1ms_Counter, &AccData);
	else {}

	return 1;
}

//外部提示
bool Tsk_Acc_Notice(void)
{
	if (g_MC_WorkStep == MC_WorkStep_Fail)					//启动失败时直接报警
	{
		g_SLN.in_Ms = g_Sys_1ms_Counter;
		g_SLN.in_SystemState = SLN_SystemStat_Error;
	}
	else													//启动成功后根据运行情况控制
		ACC_SoundLightNotice_Input(&AccData);

	//声光提示模块调用
	SoundLightNotice_Main(&g_SLN);
	//语音播报模块调用
	app_VoicePlayer_Ctrl(&g_VoicePlayer, g_Sys_1ms_Counter);

	return 1;
}

//按领导要求遥控器上，升降按钮同时长按清除报警
bool LiftButton2ErrClear(void)
{
	if (g_ChannelSwitch.CtrlCmd.Button.LiftUp && g_ChannelSwitch.CtrlCmd.Button.LiftDown)
	{
		if (g_Sys_1ms_Counter - g_ChannelSwitch.MsRecord_LiftTwoButtonPress > g_ChannelSwitch.Para_LiftTwoButtonPress
		&& g_ChannelSwitch.Para_LiftTwoButtonPress <65535//设65535关闭同时长按功能
		&& g_ChannelSwitch.CtrlCmd.Mode == ACCMode_Still//仅静止模式下有效
		)
		{
			g_SysApi.ClearAllErr = 1;
			g_ChannelSwitch.MsRecord_LiftTwoButtonPress = g_Sys_1ms_Counter;//避免重复触发
		}
	}
	else
	{
		g_ChannelSwitch.MsRecord_LiftTwoButtonPress = g_Sys_1ms_Counter;
	}

	return 1;
}

//功能调用
bool Tsk_Acc_Function(void)
{
	/*加速度限制更新,根据周工要求取消加速度限制，避免不同档位时设备启动停止性能差异给操作者带来困扰
	WalkLimit();*/

	//负载监测调用
	LoadDetection_Main(&g_LoadDetection);

	//受控停车汇总 改到此处解决启动失败时hmi显示报警条目,同时也解决了屏蔽报警但hmi上依然有显示的问题
	EMG_Judge_Controlled();
	//触摸屏处理
	HMI_Main();

	//按领导要求遥控器上升降按钮同时长按清除报警
	LiftButton2ErrClear();

	//系统API响应
	SysApiResponse();

	//急停恢复重启系统
	EmgStopReset();

	//AGV项目的相关功能
	ACC_AGV_Ctrl();

	return 1;
}

//电机速度平滑处理
bool Tsk_Acc_MotorSM(void)
{
	char i = 0;

	/*行走电机速度持续平滑处理*/
	for (i = 0; i < g_MotorResolving.Para.MotorNumber; i++)
		DataSmoothing_Main(&(g_WMSM.SM[i]), g_Sys_1ms_Counter, AccData.WalkMotor[i].V);

	/*电机速度平滑如没有运行，则在此处装载时间戳，避免输出突变
	但 输出突变正好实现强制关闭输出后，平滑输出直接=输入*/

	//差动速度平滑复位
	if (g_OSSM.En == 1 && g_OSSM.RunFlg == 0)
	{
		for (i = 0; i < g_MotorResolving.Para.MotorNumber; i++)
		{
			g_OSSM.SM[i].out_Timestamp = g_Sys_1ms_Counter;
			g_OSSM.SM[i].out_Value = 0;
		}
	}
	else
	{
		g_OSSM.RunFlg = 0;//由于这里清零，所以必须放在平滑调用之后
	}
	return 1;
}

//自动切换信道号
bool Tsk_Acc_AutoSwitchChannel(void)
{
	if (g_IOInputs.Assembly.StableValue)//联动
	{
		EQP_WHA.WD[0] = g_Para.Asb.Address_Asb;
		EQP_WHA.WD[1] = g_Para.Asb.Channel_Asb;
		/*
		switch (g_Asb.Para.WirelessController)
		{
		case 11:
			{
				EQP_WHA.WD[0] = g_Para.Asb.Address_Asb_1A;
				EQP_WHA.WD[1] = g_Para.Asb.Channel_Asb_1A;
			}
			break;
		case 12:
			{
				EQP_WHA.WD[0] = g_Para.Asb.Address_Asb_1B;
				EQP_WHA.WD[1] = g_Para.Asb.Channel_Asb_1B;
			}
			break;
		case 21:
			{
				EQP_WHA.WD[0] = g_Para.Asb.Address_Asb_2A;
				EQP_WHA.WD[1] = g_Para.Asb.Channel_Asb_2A;
			}
			break;
		case 22:
			{
				EQP_WHA.WD[0] = g_Para.Asb.Address_Asb_2B;
				EQP_WHA.WD[1] = g_Para.Asb.Channel_Asb_2B;
			}
			break;
		case 31:
			{
				EQP_WHA.WD[0] = g_Para.Asb.Address_Asb_3A;
				EQP_WHA.WD[1] = g_Para.Asb.Channel_Asb_3A;
			}
			break;
		case 32:
			{
				EQP_WHA.WD[0] = g_Para.Asb.Address_Asb_3B;
				EQP_WHA.WD[1] = g_Para.Asb.Channel_Asb_3B;
			}
			break;
		case 41:
			{
				EQP_WHA.WD[0] = g_Para.Asb.Address_Asb_4A;
				EQP_WHA.WD[1] = g_Para.Asb.Channel_Asb_4A;
			}
			break;
		case 42:
			{
				EQP_WHA.WD[0] = g_Para.Asb.Address_Asb_4B;
				EQP_WHA.WD[1] = g_Para.Asb.Channel_Asb_4B;
			}
			break;
		default:
			break;
		}*/
	}
	else//单动
	{
		EQP_WHA.WD[0] = g_Para.Asb.Address_Single;
		EQP_WHA.WD[1] = g_Para.Asb.Channel_Single;
	}

	return 1;
}

//控制结果送往接口
bool Tsk_Acc_Execute(void)
{
	//自动切换信道号
	Tsk_Acc_AutoSwitchChannel();
	
	//控制汇总送往接口
	ACC_UniformControl(&AccData);
	//ACC结果执行
	if (!g_Para.Debug.ACCOutputOff)
	{
		ACC_CtrlExecute();//执行ACC控制
	}
	else {}
	return 1;
}

//系统启动运行状态机
bool Tsk_Acc_Stm(void)
{
	if (g_MC_WorkStep == MC_WorkStep_Init)
	{
		Tsk_Acc_Stm_Init();
	}
	else if (g_MC_WorkStep == MC_WorkStep_PowerUp)
	{
		Tsk_Acc_Stm_PowerUp();
	}
	else if (g_MC_WorkStep == MC_WorkStep_Run)
	{
		Tsk_Acc_Stm_Run();
	}
	else if (g_MC_WorkStep == MC_WorkStep_Fail)
	{
		
	}
	else {}
	return 1;
}

//舵轮车手动操作控制单个模组任务入口 后面整理到单独源文件
bool Tsk_Acc_MR_ManualCtrl(MR_ManualCtrl_TypeDef* p)
{
	char i = 0;
	ACC_DATA_TypeDef *p2Acc	= &AccData;
	
	switch (p->Stm)
	{
		case MR_ManualCtrl_Stm_Init://初始化
		{
			/*模式为静止、功能开启、使能打开条件下进入*/
			if (AccData.CalCmd.Mode == ACCMode_Still && p->Para_En && p->En)
			{
				p->Stm = MR_ManualCtrl_Stm_Enter;
			}
			else
				__asm("NOP");
		}
		break;

		case MR_ManualCtrl_Stm_Enter://进入
		{
			//此位置调用已经接管了系统控制命令
			/*打开抱闸*/
			p2Acc->IOCtrl.MoveBrake = 1;	
			/*打开行走电机使能、清零速度*/
			for (i = 0; i < g_MotorResolving.Para.MotorNumber; i++)
			{
				p2Acc->WalkMotor[i].En = 1;
				p2Acc->WalkMotor[i].V = 0;
			}
			/*记录进入操作时间戳*/
			p->MsRecord = g_Sys_1ms_Counter;
			p->Stm = MR_ManualCtrl_Stm_EnterWait;
		}
		break;

		case MR_ManualCtrl_Stm_EnterWait://等待进入操作完成
		{
			/*等待1000ms*/
			if (g_Sys_1ms_Counter - p->MsRecord > 1000)
				p->Stm = MR_ManualCtrl_Stm_Run;
			else
				__asm("NOP");
		}
		break;

		case MR_ManualCtrl_Stm_Run://运行
		{
			MR_ManualCtrl_Run(p);
			/*如果使能关闭，进入退出操作*/
			if (!p->En)
				p->Stm = MR_ManualCtrl_Stm_Exit;
		}
		break;

		case MR_ManualCtrl_Stm_Exit:
		{
			/*关闭行走电机使能、清零速度，最好是重新启动系统*/
			for (i = 0; i < g_MotorResolving.Para.MotorNumber; i++)
			{
				p2Acc->WalkMotor[i].En = 0;
				p2Acc->WalkMotor[i].V = 0;
			}
			/*记录退出操作时间戳*/
			p->MsRecord = g_Sys_1ms_Counter;
			p->Stm = MR_ManualCtrl_Stm_ExitWait;
		}
		break;

		case MR_ManualCtrl_Stm_ExitWait:
		{
			/*等待1000ms*/
			if (g_Sys_1ms_Counter - p->MsRecord > 1000)
			{
				p->Stm = MR_ManualCtrl_Stm_Init;
				//自动重启系统
				SystemReset();
			}
			else
				__asm("NOP");
		}
		break;

		default:
			break;
	}

	return 1;
}

//控制系统任务入口
void Tsk_Acc(void)
{
#ifdef SSMLT
	//agv系统模拟器
	ASM_Run(&g_ASM);
#endif
	//项目自定义 TskAcc_Start
	if (g_Para.p2Fucs.p2PrjExp_TskAccStart != 0)
		g_Para.p2Fucs.p2PrjExp_TskAccStart();
	else {}

	//底盘数据处理监控任务
	Tsk_Acc_Monitor();

	//系统运行状态机
	Tsk_Acc_Stm();

	//外部提示
	Tsk_Acc_Notice();

	//功能调用
	Tsk_Acc_Function();

	//电机速度平滑处理
	Tsk_Acc_MotorSM();

	//按周工要求 hmi上实现强制接管系统单独操作模组转动进行标定
	Tsk_Acc_MR_ManualCtrl(&g_MR_ManualCtrl);

	//控制结果送往接口
	Tsk_Acc_Execute();

	//逻辑错误检测
	LogicErrDetect();

	//模拟器调用 放在最后
	SimulatorRun();
}

//***********************************END OF FILE***********************************


