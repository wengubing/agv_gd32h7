/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ACC_AGVCtrl.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  ACC_AGVCTRL_GLOBALS

#include "ACC_AGVCtrl.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "app_AGVAutoexcitationNode.h"

#include "HMI.h"
/***************************************************************************************
** 函数名称:	AGV_Skeleton_InPutChassisData_Dependence
** 功能描述:	将自动运行对底盘输出的数据管道组织好，接管遥控程序
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
float xzw_speed_K = 1.00;	/*测试代码*/
void AGV_Skeleton_OutPutChassisData_Dependence(void)
{
	/*目标运动模式和遥感命令输出*/
	{
		if(g_HostData.TargetAGVWorkMode == AGVMode_GoLine)	
		{
			g_ChannelSwitch.CtrlCmd.Mode = ACCMode_PosTrace;
			g_ChannelSwitch.CtrlCmd.Vxyz.x = 0;
			g_ChannelSwitch.CtrlCmd.Vxyz.y	= xzw_speed_K*g_HostData.Hxyz[1]/6.0f;
			g_ChannelSwitch.CtrlCmd.Vxyz.z	= 0;
		}
		/*横向巡线*/
		else if(g_HostData.TargetAGVWorkMode == AGVMode_GoTransverse)
		{
			g_ChannelSwitch.CtrlCmd.Mode = ACCMode_InfTrace;
			g_ChannelSwitch.CtrlCmd.Vxyz.x = xzw_speed_K*g_HostData.Hxyz[0]/6.0f;
			g_ChannelSwitch.CtrlCmd.Vxyz.y	= 0;
			g_ChannelSwitch.CtrlCmd.Vxyz.z	= 0;
		}
		/*自转*/
		else if(g_HostData.TargetAGVWorkMode == AGVMode_GoRotation)
		{
			g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Rotation;
			g_ChannelSwitch.CtrlCmd.Vxyz.x= 0;
			g_ChannelSwitch.CtrlCmd.Vxyz.y= 0;
			g_ChannelSwitch.CtrlCmd.Vxyz.z = xzw_speed_K*g_HostData.Hxyz[2]/6.0f;
		}
		/*平台升降*/
		else if(g_HostData.TargetAGVWorkMode == AGVMode_GoUpAndDown)
		{
			g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Lift;
			g_ChannelSwitch.CtrlCmd.Vxyz.x	= 0;
			g_ChannelSwitch.CtrlCmd.Vxyz.y	= 0;
			g_ChannelSwitch.CtrlCmd.Vxyz.z	= 0;
		}
		/*静止*/
		else if(g_HostData.TargetAGVWorkMode == AGVMode_GoStill)
		{
			g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Still;
			g_ChannelSwitch.CtrlCmd.Vxyz.x	= 0;
			g_ChannelSwitch.CtrlCmd.Vxyz.y	= 0;
			g_ChannelSwitch.CtrlCmd.Vxyz.z	= 0;
		}
		/*支腿控制*/
		else if(g_HostData.TargetAGVWorkMode == AGVMode_LegTelescopic)
		{
			g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Leg;
			g_ChannelSwitch.CtrlCmd.Vxyz.x	= 0;
			g_ChannelSwitch.CtrlCmd.Vxyz.y	= 0;
			g_ChannelSwitch.CtrlCmd.Vxyz.z	= 0;
		}
		/*悬挂控制*/
		else if(g_HostData.TargetAGVWorkMode == AGVMode_HangHigh)
		{
			g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Absorption;
			g_ChannelSwitch.CtrlCmd.Vxyz.x	= 0;
			g_ChannelSwitch.CtrlCmd.Vxyz.y	= 0;
			g_ChannelSwitch.CtrlCmd.Vxyz.z	= 0;
		}
		else
		{
			/*不控制模式，底盘可自行强制*/
		}
		g_ChannelSwitch.CtrlCmd.WalkGear = g_HostData.AGVWalkSpeedVal;
		g_ChannelSwitch.CtrlCmd.LiftGear = g_HostData.AGVUpDownSpeedVal;
	}
	/*升降按钮输出*/
	{
		if(g_HostData.AGVHandleButtom == 1)
		{
			g_ChannelSwitch.CtrlCmd.Button.LiftUp = 1;
		}
		else
		{
			g_ChannelSwitch.CtrlCmd.Button.LiftUp = 0;
		}

		if(g_HostData.AGVHandleButtom == 2)
		{
			g_ChannelSwitch.CtrlCmd.Button.LiftDown = 1;
		}
		else
		{
			g_ChannelSwitch.CtrlCmd.Button.LiftDown = 0;
		}
	}

    //送到控制源切换接口
    g_ChannelSwitch.DspObsArea = (DspObsArea_TypeDef)g_HostData.AGVRadio.ScanningArea;

    /*避障雷达使能输入 先不用，方便从hmi临时控制
    g_ChannelSwitch.CtrlCmd.RadarEn.Front = g_HostData.CurrentForRadioEnalbe;
    g_ChannelSwitch.CtrlCmd.RadarEn.Back = g_HostData.CurrentBackRadioEnalbe;
    g_ChannelSwitch.CtrlCmd.RadarEn.Left = g_HostData.CurrentLeftRadioEnalbe;
    g_ChannelSwitch.CtrlCmd.RadarEn.Right = g_HostData.CurrentRightRadioEnalbe;*/

    //防撞触边恢复 直接从缓存读取
    //要清零，且上升沿才触发 待完善
    if(((DSPCOMMOBJ_StDef*)(EQP_DSP.Data))->AntiCcollisionRecovery == 1)
    {
        g_SysApi.ClearObsBarLatch = 1;
        ((DSPCOMMOBJ_StDef*)(EQP_DSP.Data))->AntiCcollisionRecovery = 0;
    }
    else
    {
        
    }

    //故障恢复 临时处理为非0清除所有报警，按类清除待完善
    if(((DSPCOMMOBJ_StDef*)(EQP_DSP.Data))->ClearAllAlalrm != 0)
    {
        g_SysApi.ClearAllErr = 1;
        ((DSPCOMMOBJ_StDef*)(EQP_DSP.Data))->ClearAllAlalrm = 0;
    }
    else
    {
        
    }
    
    //避障开关响应在 后面的多源控制中处理

    
}

/***********************************************************
** 函数名称:	ACC_AutoStartStop
** 功能描述: 	启动/停止 按钮的功能实现，附带LED控制
** 参    数:	None
** 返 回 值:	None
***********************************************************/
void ACC_AutoStartStop(void)
{
	//按下锁定，避免来回触发
	static char AutoStartStop_lock = 0;
	//根据当前自动任务状态，判断按钮按下是暂停还是继续-----------------------------------
	if(g_IOInputs.AutoStartStop.StableValue && AutoStartStop_lock==0)
	{
		if(g_HostData.ActualForwardVelocity !=0 || g_HostData.AGVHandleButtom !=0)//车端存在速度或升降动作
		{
			g_HostData.InputPauseTaskFlag=1;
			g_HostData.InputGoOnTaskFlag=0;
			g_VoicePlayer.BitInput.TaskPause = 1;//任务暂停语音提示
		}
		else if(g_HostData.ActualForwardVelocity ==0 || g_HostData.AGVHandleButtom == 0)
		{
			g_HostData.InputPauseTaskFlag=0;
			g_HostData.InputGoOnTaskFlag=1;
			g_VoicePlayer.BitInput.TaskStart = 1;//任务继续语音提示
		}
		
		AutoStartStop_lock = 1;
	}
	
	if(g_IOInputs.AutoStartStop.StableValue==0)
		AutoStartStop_lock = 0;

	//底盘故障后需要通知自动暂停？

	//暂停/继续led控制(虽然在外部，任通过acc对象控制)
    //模式为自动 且 有任务时运行时常亮、暂停时闪烁(250/250ms)，其它时侯熄灭
	if(g_ChannelSwitch.in_Channel == ACCCC_Auto && g_HostData.ExecutionPathState == ExePathState_HaveTask)
	{
		if(g_HostData.AGVAutoMode == AutoMode_Run && !g_HostData.InputPauseTaskFlag)
			AccData.IOCtrl.AGVled_StartStop = 1;
		else if(g_HostData.AGVAutoMode == AutoMode_Pause || g_HostData.InputPauseTaskFlag)
		{
			/*不依赖ms记录的闪烁控制，一直在刷*/
			if (g_Sys_1ms_Counter % 500 < 250)
			{
				AccData.IOCtrl.AGVled_StartStop = 1;
			}
			else
			{
				AccData.IOCtrl.AGVled_StartStop = 0;
			}
        }
	}
	else
		AccData.IOCtrl.AGVled_StartStop = 0;
}
/***********************************************************
** 函数名称:	ACC_SysResetStop
** 功能描述: 	复位 按钮的功能实现，附带LED控制
** 参    数:	None
** 返 回 值:	None
***********************************************************/
void ACC_SysResetStop(void)
{
	static char SysReset_lock = 0;
	//复位按钮---------------------------------------------
	if(g_IOInputs.SysReset.StableValue && SysReset_lock==0)
	{
        //暂用于执行控制-----------------------------------
		/*if(g_Para.Debug.ForceSVODisable == 0)//状态机
		{
			g_Para.Debug.ForceSVODisable = 1;
			g_Para.Debug.ForceSVODisable_Lift = 1;
			AccData.IOCtrl.AGVled_Reset = 1;//灯亮表示执行被强制关闭
		}
		else
		{
			g_Para.Debug.ForceSVODisable = 0;
			g_Para.Debug.ForceSVODisable_Lift = 0;
			AccData.IOCtrl.AGVled_Reset = 0;
		}*/
        //------------------------------------------------------

        //------------------------------------------------------
        //常亮：有报警，且车没有正在复位
        //闪亮（250ms亮-250ms灭）：车正在复位（复位按钮已按下，且车正在重启的过程中或者刷新报警状态中）
        //灭：车无报警
        
        //按下后，清除一次所有报警锁存
        g_SysApi.ClearAllErr = 1;

        //------------------------------------------------------

		SysReset_lock = 1;
	}
	//长按直接重启系统
	if (g_IOInputs.SysReset.Filter.out_Status == IOFilterButtonStatus_PressLongDown)
		g_SysApi.SysReboot = 1;
	
	//led控制
    //有报警 常亮
    if(g_Faults.Res > ACC_FaultsType_Normal)
        AccData.IOCtrl.AGVled_Reset = 1;
    //正在启动 闪烁
    else if(g_MC_WorkStep != MC_WorkStep_Run)
    {
        if(g_Sys_1ms_Counter%500 < 250)
        {
            AccData.IOCtrl.AGVled_Reset = 1;
        }
        else
        {
            AccData.IOCtrl.AGVled_Reset = 0;
        }
    }
    //无报警 熄灭
    else
        AccData.IOCtrl.AGVled_Reset = 0;

	//长按触发复位前先快闪提醒
	if (g_IOInputs.SysReset.Filter.out_Status == IOFilterButtonStatus_PressShortLongDown)
	{
		if (g_Sys_1ms_Counter % 100 < 50)
			AccData.IOCtrl.AGVled_Reset = 1;
		else
			AccData.IOCtrl.AGVled_Reset = 0;
	}
	
    //------------------------------------------------------

	if(g_IOInputs.SysReset.StableValue == 0)
		SysReset_lock = 0;
}
/***********************************************************
** 函数名称:	AGV_OperationButton
** 功能描述: 	agv操作按钮响应
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//agv操作按钮响应
bool AGV_OperationButton(void)
{
	//启停按钮
	ACC_AutoStartStop();
	//复位按钮
	ACC_SysResetStop();

	return 1;
}
/***********************************************************
** 函数名称:		AGV_SlamLayerSwitch
** 功能描述:		导航激光雷达图层切换
** 参    数:	None
** 返 回 值:		None
***********************************************************/
//导航激光雷达图层切换
uint8_t	LayerSwitch = 0;

uint8_t AGV_SlamLayerSwitch(void)
{
	static uint8_t 	LayerSwitchStep = 0;
	static uint64_t LayerSwitchMs	= 0;
	uint8_t	LocationTag_index = 0;
	uint8_t i = 0;
	uint8_t res = 0xFF;
	
	SlamLayerLocation_StDef *p 				= &g_SlamLayer;
	ROKIT_Write_StDef 		*p_ROKIT_Write	= &g_ROKITData.ROKIT_Write;
	
	//0、查看当前定位状态;
	//1、根据地标结果，设置图层编号，延时一段时间
	//2、启动图层切换（控制命令1的bit7），延时一段时间
	//3、查看当前定位状态是否恢复正常，超时报警
	//4、退出状态机，清除控制命令
	//5、报警
	switch(LayerSwitchStep)
	{
		/*查看当前定位状态*/
		case 0:
		{
			if(
				1	//默认通过，暂时不约束进入图层切换的条件
			)
			{
				LayerSwitchStep = 1;
				LayerSwitchMs = g_Sys_1ms_Counter;
				for(i=0;i<LayerMaxNum;i++)
				{
					if(p->Input_Tag == p->Location.Tag[i])
					{
						LocationTag_index = i;
						i = LayerMaxNum;
					}
				}
			}
			/*进入错误*/
			else
			{
				LayerSwitchStep = 5;	
			}
			res = 1;
		}break;
		/*根据地标结果，设置图层编号，延时一段时间*/
		case 1:
		{
			
			//中间件的大端模式
			p_ROKIT_Write->SetX = ((p->Location.X[LocationTag_index]&0x0000ffff)<<16)+((p->Location.X[LocationTag_index]&0xffff0000)>>16);
			p_ROKIT_Write->SetY = ((p->Location.Y[LocationTag_index]&0x0000ffff)<<16)+((p->Location.Y[LocationTag_index]&0xffff0000)>>16);
			p_ROKIT_Write->SetZ = p->Location.Z[LocationTag_index];
			p_ROKIT_Write->LayerNum = p->Location.LayerNum[LocationTag_index];
			if(g_Sys_1ms_Counter > LayerSwitchMs+2000)
			{
				LayerSwitchStep = 2;
				LayerSwitchMs = g_Sys_1ms_Counter;
			}
			res = 2;
		}break;
		/*启动图层切换（控制命令1的bit7），延时一段时间*/
		case 2:
		{
			p_ROKIT_Write->CMD = ROKIT_CMD_Layer_Switch;
			if(g_Sys_1ms_Counter > LayerSwitchMs+1000)
			{
				LayerSwitchStep = 3;
				LayerSwitchMs = g_Sys_1ms_Counter;
			}
			res = 3;
		}break;
		/*查看当前定位状态是否恢复正常，超时报警*/
		case 3:
		{
			if(g_ROKITData.ROKIT_Read.LocationSta == ROKIT_LocationStae_HighReliability)
			{
				LayerSwitchStep = 4;
			}
			else if(g_Sys_1ms_Counter > LayerSwitchMs+5000)
			{
				//报警
				LayerSwitchStep = 5;
			}
			res = 4;
		}break;
		/*退出状态机，清除控制命令*/
		case 4:
		{
			p_ROKIT_Write->SetX = 0;
			p_ROKIT_Write->SetY = 0;
			p_ROKIT_Write->SetZ = 0;
			p_ROKIT_Write->LayerNum = 0;
			p_ROKIT_Write->CMD = ROKIT_CMD_Default;
			LayerSwitchStep = 0;
			res = 0;
		}break;
		/*报警*/
		case 5:
		{
			p_ROKIT_Write->SetX = 0;
			p_ROKIT_Write->SetY = 0;
			p_ROKIT_Write->SetZ = 0;
			p_ROKIT_Write->LayerNum = 0;
			p_ROKIT_Write->CMD = ROKIT_CMD_Default;
			LayerSwitchStep = 0;
		}break;
	}
	
	return res;
			
}

/***********************************************************
** 函数名称:	AGV_PowerOffDealy
** 功能描述: SLAM导航下的下电延时，确保力士乐中间件保存了当前位置
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//SLAM导航下的下电延时，确保力士乐中间件保存了当前位置
#define 	PowerOffSucMaxNum			5
void AGV_PowerOffDealy(void)
{ 
	static uint8_t 	PowerOffDealyStep = 0;
	static uint64_t	PowerOffDealyMs	= 0;
	static int16_t  SucTick = 0;
	ROKIT_Write_StDef 		*p_ROKIT_Write	= &g_ROKITData.ROKIT_Write;

	//0、旁路继电器  上电后一直吸合
	//1、如果关闭开关，向力士乐中间件 发送指令
	//2、延时，等待中间件响应;延时中，每隔1秒判定一次中间件是否正常，防止中间件死机
	//3、判断中间件是否写入成功，失败需要报警；成功就下电
	//4、故障状态
	if(g_IOOutputs.PowerOffDealy.ID	!= OCN_Disable)
	{
		switch(PowerOffDealyStep)
		{
			case 0:
			{
				if(g_MC_WorkStep == MC_WorkStep_Run)
				{
					AccData.IOCtrl.PowerOffDealy = 1;
					AccData.IOCtrl.PowerOffDealy_Bms = 1;
					//300v 电池要在正确启动后，确保启动电池供电回路也导通的
					if(g_Para.BatteryType == ACC_LithiumBattery_300V_High)
					{
						AccData.IOCtrl.CtrlVoltageBypass = 1;
					}
					//其他的不用管，直接吸合继电器
					else
					{
						//无需动作
					}
					PowerOffDealyStep = 1;
					SucTick=0;
				}				
			}
			break;
			case 1:
			{
				if(g_IOInputs.PowerOn.RealtimeValue == 0)
				{
					p_ROKIT_Write->CMD = ROKIT_CMD_Save_Relocate;
					PowerOffDealyMs = g_Sys_1ms_Counter;
					PowerOffDealyStep = 2;
				}
			}
			break;
			case 2:
			{	
				static uint16_t Timer_Temp = 0;
				if(g_Sys_1ms_Counter > PowerOffDealyMs +1000)
				{
					PowerOffDealyMs = g_Sys_1ms_Counter;
					/*直联工控机*/
					if(g_Para.EQPCommSource.EN.IPC == 1)
					{
						Timer_Temp = EQP_IPC.R_Data->SyS_ms;
						if(Timer_Temp != EQP_IPC.R_Data->SyS_ms){SucTick++;}
						else									{SucTick--;}
					}
					else
					{
						SucTick++;
					}
					//失败的时候进入第4步
					if(SucTick < -PowerOffSucMaxNum)
					{
						PowerOffDealyStep = 4;
					}
					//成功的时候进入第4步
					else if(SucTick > PowerOffSucMaxNum)
					{
						PowerOffDealyStep=3;
					}			
				}
			}
			break;
			case 3:
			{
				//写入成功，下电
				if(1)
				{
					AccData.IOCtrl.PowerOffDealy = 0;
					//300v电池需要断开旁路
					if(g_Para.BatteryType == ACC_LithiumBattery_300V_High)
					{
						AccData.IOCtrl.CtrlVoltageBypass = 0;
					}
				}
			}
			break;
			//故障
			case 4:
			{
				//失败后，再次延迟30秒等待中间件自动保存，再掉电
				if(g_Sys_1ms_Counter > PowerOffDealyMs+30000)
				{
					PowerOffDealyStep = 3;
				}
			}
			break;
			default:
			break;
		}
	}
}


/***********************************************************
** 函数名称:		AGV_SlamIPCReboot
** 功能描述:		中间件自启异常的情况下，指令重启
** 参    数:	None
** 返 回 值:		None
***********************************************************/
//中间件自启异常的情况下，指令重启
void AGV_SlamIPCReboot(void)
{
	//0、系统时间达到1分钟检查Slam定位状态，如果是定位失败跳转 1状态；其他异常状态跳转 4状态
	//1、发送系统复位指令，等待3s
	//2、发送系统启动指令，等待10s
	//3、重试次数小于3次；回到0状态；否则报故障
	//4、故障状态
	static uint8_t 	SlamIPCRebootStep = 0;
	static uint8_t 	Err_Tice = 0;
	static uint64_t	SlamIPCRebootMs	= 0;
	ROKIT_Write_StDef 		*p_ROKIT_Write	= &g_ROKITData.ROKIT_Write;
	ROKIT_Read_StDef 		*p_ROKIT_Read	= &g_ROKITData.ROKIT_Read;

	//if(EQPMsts.IPC.CommId != CCFCN_Disable)
	if(1)
	{
		switch(SlamIPCRebootStep)
		{
			/*0、系统时间达到1分钟检查Slam定位状态，如果是定位失败跳转 1状态；其他异常状态跳转 4状态*/
			case 0:
			{
				if(SlamIPCRebootMs == 0)
				{
					SlamIPCRebootMs = g_Sys_1ms_Counter;
				}
				if(g_Sys_1ms_Counter > SlamIPCRebootMs + 60000)
				{
					if(p_ROKIT_Read->LocationSta == ROKIT_LocationStae_HighReliability)
					{
						//正常状态，无需操作
					}
					else if(p_ROKIT_Read->RunSta == ROKIT_RunSta_Stop || p_ROKIT_Read->ErrCode == ROKIT_ErrCode_LocationFail)
					{
						SlamIPCRebootStep = 1;
						SlamIPCRebootMs = g_Sys_1ms_Counter;
					}
					//别的工况处理不了，直接故障状态
					else
					{	
						SlamIPCRebootStep = 4;
					}
					
				}
			}
			break;
			/*1、发送系统复位指令，等待3s*/
			case 1:
			{
				p_ROKIT_Write->CMD = ROKIT_CMD_Reset;
				if(g_Sys_1ms_Counter > SlamIPCRebootMs + 3000)
				{
					SlamIPCRebootStep = 2;
				}
			}
			break;
			/*2、发送系统启动指令，等待10s*/
			case 2:
			{
				p_ROKIT_Write->CMD = ROKIT_CMD_Launch;
				if(g_Sys_1ms_Counter > SlamIPCRebootMs + 10000)
				{
					SlamIPCRebootStep = 3;
					p_ROKIT_Write->CMD = ROKIT_CMD_Default;
				}
			}
			break;
			/*3、重试次数小于3次；回到1状态；否则报故障*/
			case 3:
			{
				if(p_ROKIT_Read->LocationSta != ROKIT_LocationStae_HighReliability)
				{
					Err_Tice++;
				}
				else
				{
					Err_Tice = 0;
				}
				if(Err_Tice < 3)
				{
					SlamIPCRebootStep = 1;
				}
				else
				{
					SlamIPCRebootStep = 4;
				}
			}
			break;
			/*4、故障状态*/
			case 4:
			{
				
			}
			break;
		}
	}
}

//300V自动充电结束重新上电流程
void MC_Running_ChargeMode(ACC_PowerOn_TypeDef *p)
{
	volatile ACC_Faults_TypeDef p2Faults;
	ACC_DATA_TypeDef *p2Acc = &AccData;
	
	if(!AccData.IOCtrl.ChargingMC)
	{
		//AGV 继续路径标志
		g_DHR_AGVInformation[Para_Index_AGV_ContinuePathState]=1;
		g_ActiveCharge.Step = 0;
		//初始化各个控制对象
		p->Ctrl_CanOpenStart = 0;								//canopen-----关闭
		p->Ctrl_Precharge = 0;									//预充控制-----关闭
		p->Ctrl_MainContactor = 0;								//主接触器------关闭
		p->Ctrl_CtrllPower = 0;									//伺服控制电源---关闭
		p->Ctrl_CtrlVoltageBypass = 1;							//系统旁路控制DC-DC---打开
		p->Ctrl_PowerOnBypass = 0;								//系统开关 旁路控制---关闭
		
		p->Ctrl_MotorPrecharge = 0;								//驱动器预充控制----关闭
		p->Ctrl_MotorContactor = 0;								//驱动器主接触器----关闭
		
		
		//初始化系统启动步骤
		g_MC_WorkStep=MC_WorkStep_Init;
		
		//初始化上电步骤状态机
		p->PowerOnStep = ACC_PowerOnStep_Init;
		
		//初始化CANopen状态机
		p->in_CanopenStep = ACC_CanopenStep_Init;
		
		//将canopen主机步骤清零
		CanMaster.CanopenStep = Step_ParaIni;

		
		g_PowerOn.MsRecord = g_Sys_1ms_Counter;
		
		p2Acc->CalCmd.Mode = ACCMode_Still;

		//屏蔽驱动器相关报警			
		p2Faults.LoseControl.Bit.CanMstErr = 0;
		p2Faults.LoseControl.Bit.CanSlvOffline = 0;
		p2Faults.LoseControl.Bit.WalkSvoErr = 0;
		p2Faults.LoseControl.Bit.LiftSvoErr = 0;
		

	}

}



/***************************************************************************************
** 函数名称:GroundCharge_CylinderControl
** 功能描述:地充电缸控制函数
** 参    数:Input1 - 输入1信号触发伸出,Input2 - 输入2信号触发收回
** 返 回 值:None  
****************************************************************************************/
void GroundCharge_CylinderControl(void)
{
    GroundCharge_CylinderControl_TypeDef *pCtrl = &g_GroundChargeCylinder;
	// 检测输入信号
    if (pCtrl->Input_Extend == 1 && pCtrl->PendingTask == TASK_IDLE && pCtrl->CurrentTask != TASK_EXTEND)
    {
        pCtrl->PendingTask = TASK_EXTEND;  // 输入1触发
		pCtrl->Input_Extend=0;
    }
    if (pCtrl->Input_Retend == 1 && pCtrl->PendingTask == TASK_IDLE && pCtrl->CurrentTask != TASK_RETRACT)
    {
        pCtrl->PendingTask = TASK_RETRACT; // 输入2触发
		pCtrl->Input_Retend=0;
    }

    // 2. 状态机处理：当前任务完成后，启动等待
    switch (pCtrl->CurrentTask)
    {
        case TASK_IDLE:
            // 空闲时，检查是否有等待任务
            if (pCtrl->PendingTask != TASK_IDLE)
            {
               pCtrl-> CurrentTask = pCtrl->PendingTask;  // 切换到等待任务
               pCtrl->PendingTask = TASK_IDLE;    // 清空等待任务
               pCtrl->TaskStartTime = g_Sys_1ms_Counter;
                
                // 根据任务类型执行初始化动作
                if (pCtrl->CurrentTask == TASK_EXTEND)
                {
					pCtrl->OutValue_Extend = 1;  // 电缸伸出
					pCtrl->OutValue_Retend = 0;
                }
                else if (pCtrl->CurrentTask == TASK_RETRACT)
                {
                    pCtrl->OutValue_Retend = 1; // 电缸收回
                    pCtrl->OutValue_Extend = 0;
                }
            }
            break;

        case TASK_EXTEND:
            // 执行3秒后结束伸出0
            if (g_Sys_1ms_Counter-pCtrl->TaskStartTime >= pCtrl->Extend_Delay)  // 3000ms = 3秒
            {
                pCtrl->OutValue_Extend = 0;		// 伸出停止
                pCtrl->CurrentTask = TASK_IDLE;	// 回到空闲状态
            }
            break;

        case TASK_RETRACT:
            // 触发限位或持续5秒停止收回
            if (g_IOInputs.GroundChargeRetractLimit.StableValue == 1 || g_Sys_1ms_Counter-pCtrl->TaskStartTime >= pCtrl->Retend_Delay)
            {
                pCtrl->OutValue_Retend = 0;		// 停止收回
                pCtrl->CurrentTask = TASK_IDLE;	// 回到空闲状态
            }
            break;

        default:
            pCtrl->CurrentTask = TASK_IDLE;  // 异常状态复位
			pCtrl->PendingTask = TASK_IDLE;  // 清空等待任务
			pCtrl->OutValue_Extend = 0;  	// 电缸动作停止
			pCtrl->OutValue_Retend = 0;
            break;
    }
}
/***********************************************************
** 函数名称:	AGV_Charge
** 功能描述: AGV自动充电
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//AGV自动充电
#define 	ActiveChargErrMaxNum			30
void AGV_Charge(void)
{
	//充电流程
	//0、充电初始化：在自动模式下，静止停靠了，没有故障，跳转状态2
	//1、充电等待：调度系统下发指令，进入充电
	//2、充电预处理：包括屏蔽故障
	//3、充电中：吸合充电中间继电器，间隔3秒检测一次充电电流、电池电量；充满跳转状态4 异常跳转状态5
	//4、充电结束：释放充电中间继电器，并进行重启流程，进入状态0
	//5、充电异常：3类报警充电异常，并进行重启流程，进入状态0
	static uint8_t 	ActiveChargStep		= 0;
	static uint32_t ActiveChargDealyMs	= 0;
	static uint8_t 	ActiveChargErr		= 0;
	static uint8_t 	AutoRebootFlag		= 0;
	static int16_t 	Err_tice 			= 0;
	ACC_DATA_TypeDef *p2Acc = &AccData;
	
	//有重启标志
	if(AutoRebootFlag != 0X00)
	{
		//直至系统启动结束，才复位标记
		if(g_MC_WorkStep == MC_WorkStep_Run)
		{
			AutoRebootFlag = 0;
		}
		return ;
	}
	

	//状态机
	switch(ActiveChargStep)
	{
		/*0、充电初始化：在自动模式下，静止停靠了，没有故障，跳转状态1*/
		case 0:
		{
			do{
				//如果有故障，直接跳脱
				if(ActiveChargErr != 0x00)							{break;}
				//如果不是自动状态，直接跳脱
				if(g_HostData.ControlMODEL != Style_Auto)			{break;}
				//如果不是自动状态，直接跳脱
				if(g_HostData.ActualAGVWorkMode != AGVMode_GoStill)	{break;}
				//执行到这里，即可让状态机跳转
				ActiveChargStep = 1;
			}while(0);
		}break;
		/*1、充电等待：调度系统下发指令，进入充电状态2*/
		case 1:
		{
			if(g_DHR_AGVInformation[Para_Index_AGV_ChargingControl] != 0x00)
			{
				g_GroundChargeCylinder.Input_Extend = 1;
				ActiveChargStep = 2;
				ActiveChargDealyMs = g_Sys_1ms_Counter;
			}
			else
			{
				//强制继电器断开
				g_ChannelSwitch.CtrlCmd.Button.ChargingMC = 0;
				//强制回到状态0 判断是不是满足条件
				ActiveChargStep = 0;
			}
		}break;
		/*2、充电预处理：包括屏蔽故障*/
		case 2:
		{
			//屏蔽主动充电下的故障，由于300V锂电池 充电时
			if(g_Para.BatteryType == ACC_LithiumBattery_300V_High)
			{
				//进入充电
				p2Acc->IOCtrl.Precharge = 0;									//预充控制
				p2Acc->IOCtrl.MainContactor = 0;								//主接触器
				p2Acc->IOCtrl.CtrllPower = 0;									//伺服控制电源
				p2Acc->IOCtrl.CtrlVoltageBypass = 1;							//系统旁路控制DC-DC
				p2Acc->IOCtrl.PowerOnBypass = 0;								//系统开关 旁路控制	

				p2Acc->IOCtrl.MotorPrecharge = 0;								//驱动器预充控制
				p2Acc->IOCtrl.MotorContactor = 0;								//驱动器主接触器	

				//关闭CAN总线
				setState(&CanObjectDict_Data, Stopped);
				CanMaster.MasterStatu = Stopped;	
							
				g_ActiveCharge.Step = 3;
				
			}
			//地充 需要伸出极板
			if(g_Para.ChargePileType == ACC_ChargePileType_Active)
			{
				p2Acc->IOCtrl.GroundChargingExtend 	= 1;
				p2Acc->IOCtrl.GroundChargingRetract = 0;
				//通过延时完成电缸 伸出的到位确认 默认3秒 实际需要测试
				if(g_Sys_1ms_Counter >= ActiveChargDealyMs+3000)
				{
					p2Acc->IOCtrl.GroundChargingExtend 	= 0;
					p2Acc->IOCtrl.GroundChargingRetract = 0;
					ActiveChargDealyMs = g_Sys_1ms_Counter;
					ActiveChargStep = 3;
				}
			}
			else
			{
				//适当延时进入状态3
				if(g_Sys_1ms_Counter >= ActiveChargDealyMs+3000)
				{
					ActiveChargDealyMs = g_Sys_1ms_Counter;
					ActiveChargStep = 3;
				}
			}
			
		}break;
		/*3、充电中：吸合充电中间继电器，间隔3秒检测一次充电电流、电池电量；充满跳转状态4 异常跳转状态5*/
		case 3:
		{
			g_ChannelSwitch.CtrlCmd.Button.ChargingMC = 1;
			if(g_DHR_AGVInformation[Para_Index_AGV_ChargingControl] == 0x00)
			{
				ActiveChargStep = 4;
				ActiveChargDealyMs = g_Sys_1ms_Counter;
			}
			if(g_Sys_1ms_Counter >= ActiveChargDealyMs+3000)
			{
				ActiveChargDealyMs = g_Sys_1ms_Counter;
				//充电异常无需此处判断，仅需跳转对应状态即可，故障检出中在执行故障报警
				if(g_BMSData.BMS_Read.Current < 0)	{Err_tice++;}		else{Err_tice = 0;}
				//累计XX次都是没充电，认为异常，需要自动重启
				if(Err_tice > ActiveChargErrMaxNum)
				{
					Err_tice		= 0;
					ActiveChargStep = 5;
				}

				//调度下发停止充电或非自动模式，结束自动充电
				if(g_DHR_AGVInformation[Para_Index_AGV_ChargingControl] == 0x00 || g_HostData.ControlMODEL != Style_Auto)
				{
					ActiveChargStep = 4;
					ActiveChargDealyMs = g_Sys_1ms_Counter;
				}
			}
		}break;
		/*4、充电结束：释放充电中间继电器，并进行重启流程，进入状态0*/
		case 4:
		{
			g_ChannelSwitch.CtrlCmd.Button.ChargingMC = 0;
			//重启倒计时 3秒
			if(g_Sys_1ms_Counter >= ActiveChargDealyMs+3000)
			{
				g_GroundChargeCylinder.Input_Retend = 1;
				ActiveChargDealyMs = g_Sys_1ms_Counter;
				ActiveChargStep = 0;
				//清除故障，只有300V锂电池需要
				if(g_Para.BatteryType == ACC_LithiumBattery_300V_High)
				{				
					MC_Running_ChargeMode(&g_PowerOn);
				}
				//主动式充电桩和高压锂电的情况下，才需要重启
				//if(g_Para.ChargePileType == ACC_ChargePileType_Active)
				//{
				//	AutoRebootFlag	= 1;
				//	g_SysApi.SysReset = 1;//软重启
				//}
			}
		}break;
		/*5、充电异常：3类报警充电异常，并进行重启流程，进入状态0*/
		case 5:
		{
			g_GroundChargeCylinder.Input_Retend = 1;
			//清除故障，只有300V锂电池需要
			if(g_Para.BatteryType == ACC_LithiumBattery_300V_High)
			{				
				g_ChannelSwitch.CtrlCmd.Button.ChargingMC = 0;
				MC_Running_ChargeMode(&g_PowerOn);
				ActiveChargStep = 0;
			}
			else
			{
				AutoRebootFlag	= 1;
				AutoRebootFlag	= 1;
				ActiveChargStep = 0;
			}
			
		}break;
	}
	//地充控制
	GroundCharge_CylinderControl();
	
}

/***********************************************************
** 函数名称:	ACC_AGV_Ctrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
bool ACC_AGV_Ctrl(void)
{
	//按钮控制
	AGV_OperationButton();
	
	//图层切换
	{
		uint8_t SlamLayerSwitchRes = 0;
		if(LayerSwitch == 1)
		{
			SlamLayerSwitchRes = AGV_SlamLayerSwitch();
			//正常完成图层切换
			if(SlamLayerSwitchRes == 0X00)
			{
				LayerSwitch = 0;
			}
			//切换故障
			else if(SlamLayerSwitchRes == 0xFF)
			{
				LayerSwitch = 0;
			}
		}
	}

	//自激AGV数据生成
	{
		if(g_HostData.AGVPendantFun.AutoexcitationNode == 1)
		{
			AGVAutoexcitationNodeMain(&g_AutoExcitation);

			//任务生成后填充一次结果（由于状态机会自行跳转，不会重复执行）
			if(g_AutoExcitation.AE_STM  == AutoExcitation_STM_Accomplish)
			{
				//给自动框架的当前点位进行初始化赋值，磁导航和二维码导航的首次任务执行是必须的
				g_HostData.CurrentPath_X = g_AutoExcitation.AE_Temp.p_PathStartPoint->X;
				g_HostData.CurrentPath_Y = g_AutoExcitation.AE_Temp.p_PathStartPoint->Y;
				g_HostData.CurrentPath_Tag = g_AutoExcitation.AE_Temp.p_PathStartPoint->Tag;
			}
		}
	}
	
	
	//SLAM导航的延迟下电
	AGV_PowerOffDealy();

	//SLAM导航中间件故障自恢复
	AGV_SlamIPCReboot();

	//AGV自动充电
	AGV_Charge();
	return 0;
}

/************************************END OF FILE************************************/
