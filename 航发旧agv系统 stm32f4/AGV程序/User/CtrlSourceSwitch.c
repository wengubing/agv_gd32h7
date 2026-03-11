/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: CtrlSourceSwitch.c
** 创 建 人: 文小兵
** 描    述: 控制命令源切换、解析
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0
** 创建日期: 2024年8月26日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define CtrlSourceSwitch_GLOBALS

//自定义引用			
#include "CtrlSourceSwitch.h"
#include "HMI.h"
#include "app_DependenceAGV.h"
#include "BaseApi.h"
#include "Debug.h"

//外部对象声明

//内部方法声明

//Acquisition control source
bool CSS_Get(void)
{
	uint8_t DspModeSel = 0;
	//调度系统切换的硬件输入逻辑综合处理
	{
		//面板上的旋钮开关
		if(g_IOInputs.DspModeSel.ID != ICN_Disable && g_IOInputs.DspModeSel.StableValue == 1)
		{
			DspModeSel = 1;
		}
		//遥控器的旋钮开关
		else if (g_IOInputs.DspModeSel.ID == ICN_Disable && g_ChannelSwitch.Bits.DspModeSel == 1)
		{
			DspModeSel = 2;
		}
		//屏幕上的按钮
		else if (g_IOInputs.DspModeSel.ID == ICN_Disable && ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.DspModeSel == 1)
		{
			DspModeSel = 3;
		}
		//遥控器调试后台开关
		else if (0)
		{
			DspModeSel = 4;
		}
	}
	//根据逻辑 获取控制源
	{
		if (ExtHmi.ScreenID == ManualControlPage && ((Screen_ManualControl_Typedef*)(ExtHmi.p2Data))->Button.Enable)
		{
			g_ChannelSwitch.in_Channel = ACCCC_HMI;
		}
		else if (DspModeSel != 0)
		{
			g_ChannelSwitch.in_Channel = ACCCC_Auto;
		}
		//如果无线/有线 切换开关没有初始化，默认只能使用有线遥控不能使用无线
		else if (g_IOInputs.RemoteWiredSel.ID == ICN_Disable || g_IOInputs.RemoteWiredSel.StableValue)
		{
			g_ChannelSwitch.in_Channel = ACCCC_WiredRemote;
		}
		else if (1)
		{
			g_ChannelSwitch.in_Channel = ACCCC_WirelessRemote;
		}
		else if (0)
		{
			g_ChannelSwitch.in_Channel = ACCCC_HandShank;
		}
		else
		{
			g_ChannelSwitch.in_Channel = ACCCC_Null;
		}
	}
	//模式切换的时候，将遥控器超时计数清除一次，防止报警一闪而过
	{
		static uint8_t flag_last = 0;
		if (g_ChannelSwitch.in_Channel == ACCCC_WirelessRemote && flag_last == 0)
		{
			EQPMsts.WHA.TimeOutMs = 0;
			flag_last = 1;
		}
		else if (flag_last == 1 && g_ChannelSwitch.in_Channel == ACCCC_WiredRemote)
		{

			flag_last = 0;
		}
	}

	return 1;
}

//控制源切换 退出当前控制源
bool CSS_ExitCheck(void)
{
	if (g_ChannelSwitch.STM_Channel != g_ChannelSwitch.in_Channel && g_ChannelSwitch.STM_Channel != ACCCC_Null)
	{
		//清除控制命令
		g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Still;
		g_ChannelSwitch.CtrlCmd.Vxyz.x = 0;
		g_ChannelSwitch.CtrlCmd.Vxyz.y = 0;
		g_ChannelSwitch.CtrlCmd.Vxyz.z = 0;
		g_ChannelSwitch.CtrlCmd.Button.LiftDown = 0;
		g_ChannelSwitch.CtrlCmd.Button.LiftUp = 0;

		//退出模式为自动时，清除朝向初始化值
		if (g_ChannelSwitch.in_Channel != ACCCC_Auto)
		{
			ExtHmi.p2HMI_CommObj->InitDirection = 361;//清除朝向初始化值

			//不是自动模式时，清除条件不满足标志
			g_ChannelSwitch.DirectionNotInit_Flag = 0;
			g_ChannelSwitch.NotFindPointSign_Flag = 0;
		}

		//退出当前控制源状态机
		g_ChannelSwitch.STM_Channel = ACCCC_Null;
	}
	return 1;
}

//模式命令切换时的操作
bool ACC_ModeSwitch_Operation(void)
{
	if (g_ChannelSwitch.CtrlCmd.Mode != g_ChannelSwitch.CtrlCmd.ModeLast)
	{
		g_ChannelSwitch.CtrlCmd.ModeLast = g_ChannelSwitch.CtrlCmd.Mode;

		//复位磁条离线检出状态机
		g_Faults.TraceOutline.STM = TraceOutline_Init;
	}

	return 1;
}

//解析遥控器控制命令
bool ACC_GetCmdFormWirelessRemote_Process(void)
{
	if (AtDbug.Bit.RmcUpdataOff == 0)
		g_Para.p2Fucs.p2ACC_GetCmdFormWirelessRemote();
#ifdef SSMLT
	else if (1)
		ASM_WirelessRemote(&g_ASM);//从模拟器获取无线遥控器命令
#endif
	return 1;
}

//获取自动状态下的遥控命令
void ACC_GetCmdFormWirelessRemote_Auto(void)
{
	if (AtDbug.Bit.RmcUpdataOff == 0)
	{
		g_Para.p2Fucs.p2ACC_GetCmdFormWirelessRemote_Auto();
	}
#ifdef SSMLT
	else if (1)
	{
		g_ChannelSwitch.CtrlCmd.Button.Horn = ASM_WirelessRemote_Horn(&g_ASM);//从模拟器获取无线遥控器喇叭命令
	}
#endif
}

//遥控模式下根据运动模式设置避障区域
bool ACC_ObsAreaSet_Remote(void)
{
	//根据模式避障区域 正向：空载正向；横向：空载横向；旋转：旋转负载
	if (ACC_IsPosWalkMode(g_ChannelSwitch.CtrlCmd.Mode))
		g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_LoadPos;
	else if (ACC_IsInfWalkMode(g_ChannelSwitch.CtrlCmd.Mode))
		g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_Load_Inf;
	else
		g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_Rotation;

	return 1;
}

//自动模式下，根据自动接口的避障区域设置 和 当前运动模式设置避障区域
bool ACC_ObsAreaSet_Auto(void)
{
	//根据模式避障区域 正向：空载正向；横向：空载横向；旋转：旋转负载
	//if(g_ChannelSwitch.DspObsArea == DspObsArea_Noload)

	if (0)//直接透传命令
	{
		g_ChannelSwitch.CtrlCmd.Button.ObsArea = (AccObsArea_TypeDef)g_ChannelSwitch.DspObsArea;
		return 1;
	}

	//用switch处理g_ChannelSwitch.DspObsArea
	switch (g_ChannelSwitch.DspObsArea)
	{
	case DspObsArea_Null:
		g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_NoloadPos;
		break;

	case DspObsArea_Noload:
		if (ACC_IsPosWalkMode(g_ChannelSwitch.CtrlCmd.Mode))
			g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_NoloadPos;
		else if (ACC_IsInfWalkMode(g_ChannelSwitch.CtrlCmd.Mode))
			g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_Noload_Inf;
		else
			g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_Rotation;
		break;

	case DspObsArea_Load:
		if (ACC_IsPosWalkMode(g_ChannelSwitch.CtrlCmd.Mode))
			g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_LoadPos;
		else if (ACC_IsInfWalkMode(g_ChannelSwitch.CtrlCmd.Mode))
			g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_Load_Inf;
		else
			g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_Rotation;
		break;

	case DspObsArea_Charge:
		g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_Charge;
		break;

	case DspObsArea_Enter_Noload:
		g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_Enter_Noload_PosInf;
		break;

	case DspObsArea_Narrow:
		g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_Rotation;
		break;

	case DspObsArea_Enter_Load:
		g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_Enter_Load_PosInf;
		break;

	case DspObsArea_HighSpeed://392项目高速模式 用作 空载横向
		g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_Noload_Inf;
		break;

	case DspObsArea_Reserved://392项目预留 用作 负载横向
		g_ChannelSwitch.CtrlCmd.Button.ObsArea = AccObsArea_Load_Inf;
		break;

	default:
		break;
	}

	return 1;
}

//强制设置避障区域
bool ACC_ObsAreaSet_Force(void)
{
	//通过遥控器-----------------------------------------
	//升降同时按，避障旋钮开启时，将行走和升降两挡位相加为强制结果，
	//避障旋钮关闭 和 控制模式切换时，复位强制
	/*if(g_WHData.J10 == 0)//避障开启
	{
		if(g_WHData.J12 && g_WHData.J18)//升降同时按
		{
			g_ChannelSwitch.ForceObsArea = (AccObsArea_TypeDef)(g_WHData.J42 + g_WHData.LiftSpeedVal);//行走和升降两挡位相加为强制结果
		}
	}
	else
		g_ChannelSwitch.ForceObsArea = AccObsArea_Null;*/
		//通过遥控器-----------------------------------------

		//强制设置避障区域
		//检查强制设置避障区域是否合法 不为无效且在枚举范围内
	if (g_ChannelSwitch.ForceObsArea != AccObsArea_Null && g_ChannelSwitch.ForceObsArea < AccObsArea_Num)
		g_ChannelSwitch.CtrlCmd.Button.ObsArea = g_ChannelSwitch.ForceObsArea;

	return 1;
}

//避障雷达使能多源控制
bool RadarEnSwitch(void)
{
	//目标被其它源修改
	if (*(char*)&g_ChannelSwitch.CtrlCmd.RadarEn != *(char*)&g_ChannelSwitch.RadarEnLast)
	{
		g_ChannelSwitch.RadarEnLast = g_ChannelSwitch.CtrlCmd.RadarEn;

		//同步到各个源一次,且在它自己没有变时，不触发变化同步
		//g_ChannelSwitch.WhRadarEn = g_ChannelSwitch.RadarEnLast;//遥控器一直在强刷，无法同步
		//g_ChannelSwitch.WhRadarEnLast = g_ChannelSwitch.RadarEnLast;

		g_ChannelSwitch.HmiRadarEn = g_ChannelSwitch.RadarEnLast;//hmi做成触发式 或直接写缓存，可以同步
		g_ChannelSwitch.HmiRadarEnLast = g_ChannelSwitch.RadarEnLast;

		//同步到hmi缓存对象
		ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.ForwardRadarEnable_Bit0 = g_ChannelSwitch.HmiRadarEn.Front;
		ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.BackRadarEnable_Bit1 = g_ChannelSwitch.HmiRadarEn.Back;
		ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.LeftRadarEnable_Bit2 = g_ChannelSwitch.HmiRadarEn.Left;
		ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.RightRadarEnable_Bit3 = g_ChannelSwitch.HmiRadarEn.Right;

		//同步到调度系统对象
		((DSPCOMMOBJ_StDef*)(EQP_DSP.Data))->SecuritySensorControl
			&= (*(u8*)&g_ChannelSwitch.RadarEnLast + 0xF0);
	}

	//遥控数据发生变化时
	if (*(char*)&g_ChannelSwitch.WhRadarEn != *(char*)&g_ChannelSwitch.WhRadarEnLast)
	{
		g_ChannelSwitch.WhRadarEnLast = g_ChannelSwitch.WhRadarEn;

		//同步到目标一次,不同步last值，以便触发下次变化
		g_ChannelSwitch.CtrlCmd.RadarEn = g_ChannelSwitch.WhRadarEn;
	}

	//HMI数据发生变化时
	if (*(char*)&g_ChannelSwitch.HmiRadarEn != *(char*)&g_ChannelSwitch.HmiRadarEnLast)
	{
		g_ChannelSwitch.HmiRadarEnLast = g_ChannelSwitch.HmiRadarEn;

		//同步到目标一次,不同步last值，以便触发下次变化
		g_ChannelSwitch.CtrlCmd.RadarEn = g_ChannelSwitch.HmiRadarEn;

	}

	//自动时，调度系统安全开关发生变化时，假设调度系统下发命令为触发式通信
	//调度系统没用该功能，且定义为0开启 1关闭
	/*if (g_ChannelSwitch.STM_Channel == ACCCC_Auto)
	{
		ACC_ObsEn_BitDef ObsEn_DSP_Temp;
		*((u8*)&ObsEn_DSP_Temp) = (~(((DSPCOMMOBJ_StDef*)(EQP_DSP.Data))->SecuritySensorControl & 0x0F)) & 0x0F;

		if(*((u8*)&ObsEn_DSP_Temp) != *((u8*)&g_ChannelSwitch.CtrlCmd.RadarEn))
		{
			g_ChannelSwitch.CtrlCmd.RadarEn = ObsEn_DSP_Temp;
		}
	}*/

	//临时补丁，249和327项目无雷达，强制关闭雷达避障 和防撞触边功能
	if (
		g_Para.EqpID == 32701 || g_Para.EqpID == 32702 || g_Para.EqpID == 32703
	||	g_Para.EqpID == 24901 || g_Para.EqpID == 24902 || g_Para.EqpID == 24903 || g_Para.EqpID == 24904
	)
	{
		//关闭雷达避障功能
		g_ChannelSwitch.CtrlCmd.RadarEn.Front = 0;
		g_ChannelSwitch.CtrlCmd.RadarEn.Back = 0;
		g_ChannelSwitch.CtrlCmd.RadarEn.Left = 0;
		g_ChannelSwitch.CtrlCmd.RadarEn.Right = 0;

		//关闭防撞触边功能
		g_ChannelSwitch.CtrlCmd.BarEn.Front = 0;
		g_ChannelSwitch.CtrlCmd.BarEn.Back = 0;
		g_ChannelSwitch.CtrlCmd.BarEn.Left = 0;
		g_ChannelSwitch.CtrlCmd.BarEn.Right = 0;
	}
	else {}

	return 1;
}

//系统参数未使能雷达避障时，强制关闭雷达避障
bool RadarEnForceClose(void)
{
	if (g_Para.FunctionSwitch.RadarEn.Front == 0)
		g_ChannelSwitch.CtrlCmd.RadarEn.Front = 0;
	if (g_Para.FunctionSwitch.RadarEn.Back == 0)
		g_ChannelSwitch.CtrlCmd.RadarEn.Back = 0;
	if (g_Para.FunctionSwitch.RadarEn.Left == 0)
		g_ChannelSwitch.CtrlCmd.RadarEn.Left = 0;
	if (g_Para.FunctionSwitch.RadarEn.Right == 0)
		g_ChannelSwitch.CtrlCmd.RadarEn.Right = 0;

	return 1;
}

//控制源切换 Null
bool CSS_Process_Null(void)
{
	if (g_ChannelSwitch.in_Channel == ACCCC_WirelessRemote)
	{
		g_ChannelSwitch.STM_Channel = ACCCC_WirelessRemote;
	}
	else if (g_ChannelSwitch.in_Channel == ACCCC_WiredRemote)
	{
		g_ChannelSwitch.STM_Channel = ACCCC_WiredRemote;
	}
	else if (g_ChannelSwitch.in_Channel == ACCCC_HandShank)
	{
		g_ChannelSwitch.STM_Channel = ACCCC_HandShank;
	}
	else if (g_ChannelSwitch.in_Channel == ACCCC_HMI)
	{
		g_ChannelSwitch.STM_Channel = ACCCC_HMI;
	}
	else if (g_ChannelSwitch.in_Channel == ACCCC_Auto)
	{
		/*依据导航方式的不同,切入导航模式的时候 有不同的运行初始条件检测*/
		switch(g_HostData.AGVNavigateMode)
		{
			case AGVNavigateMode_QR:
			{
				//检出上线时是否在地标二维码点位上
				if (g_HostData.ParkQRData.PintSta == 0X00)	{g_ChannelSwitch.NotFindPointSign_Flag = 1;}
				else										{g_ChannelSwitch.NotFindPointSign_Flag = 0;}

				g_ChannelSwitch.STM_Channel = ACCCC_Auto;
			}break;
			case AGVNavigateMode_Magnetic:
			{
				//检出上线朝向是否已初始化
				if (ExtHmi.p2HMI_CommObj->InitDirection == 361)
					g_ChannelSwitch.DirectionNotInit_Flag = 1;
				else
					g_ChannelSwitch.DirectionNotInit_Flag = 0;

				//检出上线时是否在点位上
				if (ACC_IsOnPoint() == 0)
					g_ChannelSwitch.NotFindPointSign_Flag = 1;
				else
					g_ChannelSwitch.NotFindPointSign_Flag = 0;

				if (g_ChannelSwitch.DirectionNotInit_Flag == 0 && g_ChannelSwitch.NotFindPointSign_Flag == 0)
				{
					//初始化上线朝向
					g_VirtualOdoData.FixZ = ExtHmi.p2HMI_CommObj->InitDirection * 100;
					g_VirtualOdoData.FixFlag = 1;

					//初始化上线点位
					if (g_ChannelSwitch.InputPointID != 0)
					{
						g_HostData.CurrentPointTag = g_ChannelSwitch.InputPointID;
						g_ChannelSwitch.InputPointID = 0;//清除输入点位
					}

					g_ChannelSwitch.STM_Channel = ACCCC_Auto;
				}
			}
			break;
			default:
			case AGVNavigateMode_NULL:		/*初始化*/
			case AGVNavigateMode_GPS:		/*GPS导航，暂时和SLAM不做区分*/
			case AGVNavigateMode_SLAM:		
			{
				g_ChannelSwitch.STM_Channel = ACCCC_Auto;
			}
			break;
		}
	}

	//如果目标模式不是自动，清除自动条件不满足标志
	if (g_ChannelSwitch.in_Channel != ACCCC_Auto)
	{
		g_ChannelSwitch.DirectionNotInit_Flag = 0;
		g_ChannelSwitch.NotFindPointSign_Flag = 0;
	}
	else {}

	//避障使能用系统参数初始化一次
	if (g_ChannelSwitch.in_Channel != ACCCC_HMI)//手动控车时不去更新使能
	{
		g_ChannelSwitch.CtrlCmd.BarEn = g_Para.FunctionSwitch.BarEn;
		g_ChannelSwitch.CtrlCmd.RadarEn = g_Para.FunctionSwitch.RadarEn;
	}
	else {}

	//控制方式切换后，清除充电极板命令
	g_ChannelSwitch.CtrlCmd.Button.ChargingMC = 0;

	//控制方式切换后，清除强制避障区域
	g_ChannelSwitch.ForceObsArea = AccObsArea_Null;

	return 1;
}

//休眠1分钟后切静止
bool CSS_WirelessSleep2Still(void)
{
	if (g_WHData.WirelessSleep == 1)
	{
		if (g_ChannelSwitch.WirelessSleepCnt < g_ChannelSwitch.WirelessSleepCntMax)//1min
			g_ChannelSwitch.WirelessSleepCnt++;
		else
		{
			//时间到操作一次WirelessRemoteComminitOk
			if (g_ChannelSwitch.WirelessSleepCnt == g_ChannelSwitch.WirelessSleepCntMax)
			{
				g_ChannelSwitch.WirelessRemoteComminitOk = 0;
				g_ChannelSwitch.WirelessSleepCnt = g_ChannelSwitch.WirelessSleepCntMax + 1;
			}
		}
	}
	else
	{
		g_ChannelSwitch.WirelessSleepCnt = 0;
	}

	return 1;
}

//控制源切换
bool CSS_Process(void)
{
	//获取控制源
	CSS_Get();

	CSS_ExitCheck();//检测退出当前控制源

	switch (g_ChannelSwitch.STM_Channel)
	{
		//初始化，切换时的操作可添加到一下初始化中
		case ACCCC_Null:
			CSS_Process_Null();
		break;

		//从遥控器无线更新数据
		case ACCCC_WirelessRemote:				            
		{
			if (g_ChannelSwitch.WirelessRemoteComminitOk)    //遥控器通信正常时，获取遥控器命令
				ACC_GetCmdFormWirelessRemote_Process();
			else
			{
				g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Still; //遥控器通信异常时，停止
				if (g_WHData.CountPerSecend > 2 && g_WHData.WirelessSleep == 0)//有通信且没有休眠
					g_ChannelSwitch.WirelessRemoteComminitOk = 1;
			}

			//休眠1分钟后切静止 特殊功能正常情况不要打开
			if (g_ChannelSwitch.WirelessSleepCntMax)
				CSS_WirelessSleep2Still();

			//遥控模式下根据运动模式设置避障区域
			ACC_ObsAreaSet_Remote();
		}
		break;

		case ACCCC_WiredRemote:				                //从遥控器有线更新数据
		{
			ACC_GetCmdFormWirelessRemote_Process();
			//遥控模式下根据运动模式设置避障区域
			ACC_ObsAreaSet_Remote();
		}
		break;

		case ACCCC_HandShank:					            //从特殊手柄更新数据
		{
		}
		break;

		case ACCCC_HMI:						                //从触摸屏更新数据
		{
		}
		break;

		case ACCCC_Auto:						            //从调度系统更新数据
		{
			AGV_Skeleton_Dependence();

			//强制解析遥控器控制命令 控制运行
			if (g_ChannelSwitch.in_WROverAuto)
				ACC_GetCmdFormWirelessRemote_Process();
			else {}

			//自动模式下，根据自动接口的避障区域设置 和 当前运动模式设置避障区域
			ACC_ObsAreaSet_Auto();

			//调度控制时也响应遥控器的一部分命令  例如喇叭、急停、照明等
			ACC_GetCmdFormWirelessRemote_Auto();

			//自动模式下强制遥控器授权信号，避免自动模式受影响
			g_ChannelSwitch.Bits.RemoteKeyAuth = 1;
		}
		break;

		default:
			break;
	}

	//强制设置避障区域
	ACC_ObsAreaSet_Force();

	//检查区域合理性
	//1充电工位前必须是充电区域 且 设备平台是降低的，且上pgv不应该识别到工装
	//2负载时不能是空载区域
	//3运行方向与区域应该匹配 

	//避障使能多源控制
	RadarEnSwitch();

	//系统参数未使能雷达避障时，强制关闭雷达避障
	RadarEnForceClose();

	//模式命令切换时的操作
	ACC_ModeSwitch_Operation();

	return 1;
}

//***********************************END OF FILE***********************************

