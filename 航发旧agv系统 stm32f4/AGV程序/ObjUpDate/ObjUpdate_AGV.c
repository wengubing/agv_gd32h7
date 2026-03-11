/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_AGV.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  OBJUPDATE_AGV_GLOBALS

#include "ObjUpdate_AGV.h"

#include "app_DependenceAGV.h"
#include "app_SkeletonAGV.h"
#include "ACC_Init.h"
#include "HMI.h"

#include "EQPComm_Init.h"

/***************************************************************************************
** 函数名称:	AGV_DependenceDataInput_Slam
** 功能描述:	
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
void AGV_DependenceDataInput_Slam(void)
{
	DepAGV_SlamData_InputTypeDef 	*p_Targer	= g_AGVDependenInputData.SlamData;
	ROKIT_Read_StDef 				*p_Source	= &g_ROKITData.ROKIT_Read;

	p_Targer->X 	= p_Source->LocationX;
	p_Targer->Y 	= p_Source->LocationY;
	p_Targer->Z 	= p_Source->LocationZ;
	/*中间件输出正常并且高可靠才标志运行正常*/
	if(		p_Source->RunSta 		== ROKIT_RunSta_Normal 
		&&  p_Source->LocationSta 	== ROKIT_LocationStae_HighReliability)
	{
		p_Targer->Sta 	= DepAGV_SlamDataSta_OK;
	}
	/*中间件启动中*/
	else if( 	p_Source->RunSta	== ROKIT_RunSta_Initial )
	{
		p_Targer->Sta 	= DepAGV_SlamDataSta_Initial;
	}
	/*定位失败或者低可靠*/
	else if( 	p_Source->LocationSta	== ROKIT_LocationSta_Fail 
			||  p_Source->LocationSta 	== ROKIT_LocationStae_HighReliability)
	{
		p_Targer->Sta 	= DepAGV_SlamDataSta_Fail;
	}
	else
	{
		p_Targer->Sta 	= DepAGV_SlamDataSta_Err;
	}
}

/***************************************************************************************
** 函数名称:	AGV_DependenceDataInput_RFID
** 功能描述:	
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
void AGV_DependenceDataInput_RFID(void)
{
	DepAGV_RFIDData_InputTypeDef 	*p_Targer	= g_AGVDependenInputData.RfidData;
	RFID_Read_StDef 				*p_Source	= &g_RFIDData.RFID_Read;

	p_Targer->Status		= p_Source->Status;
	p_Targer->RSSIStatus	= p_Source->RSSIStatus;
	p_Targer->Tag			= p_Source->Tag;
	p_Targer->LastTag		= p_Source->LastTag;
}
/***************************************************************************************
** 函数名称:	AGV_DependenceDataInput_Mag
** 功能描述:	
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
void AGV_DependenceDataInput_Mag(void)
{
	/*前磁导航数据输入自动框架*/
	{
		DepAGV_MagData_InputTypeDef 	*p_Targer	= g_AGVDependenInputData.F_MagData;
		MN_Read_StDef 					*p_Source	= &g_MNData[0].MN_Read;

		p_Targer->Strength			= p_Source->Strength;
		p_Targer->Distance			= p_Source->Distance;
		p_Targer->Status			= p_Source->Status;
		p_Targer->L_or_R_Sta		= p_Source->L_or_R_Sta;
		p_Targer->DistanceLeft		= p_Source->DistanceLeft;
		p_Targer->DistanceRight		= p_Source->DistanceRight;
	}
	/*后磁导航数据输入自动框架*/
	{
		DepAGV_MagData_InputTypeDef 	*p_Targer	= g_AGVDependenInputData.B_MagData;
		MN_Read_StDef 					*p_Source	= &g_MNData[1].MN_Read;

		p_Targer->Strength			= p_Source->Strength;
		p_Targer->Distance			= p_Source->Distance;
		p_Targer->Status			= p_Source->Status;
		p_Targer->L_or_R_Sta		= p_Source->L_or_R_Sta;
		p_Targer->DistanceLeft		= p_Source->DistanceLeft;
		p_Targer->DistanceRight		= p_Source->DistanceRight;
	}
	/*左磁导航数据输入自动框架*/
	{
		DepAGV_MagData_InputTypeDef 	*p_Targer	= g_AGVDependenInputData.L_MagData;
		MN_Read_StDef 					*p_Source	= &g_MNData[2].MN_Read;

		p_Targer->Strength			= p_Source->Strength;
		p_Targer->Distance			= p_Source->Distance;
		p_Targer->Status			= p_Source->Status;
		p_Targer->L_or_R_Sta		= p_Source->L_or_R_Sta;
		p_Targer->DistanceLeft		= p_Source->DistanceLeft;
		p_Targer->DistanceRight		= p_Source->DistanceRight;
	}
	/*右磁导航数据输入自动框架*/
	{
		DepAGV_MagData_InputTypeDef 	*p_Targer	= g_AGVDependenInputData.R_MagData;
		MN_Read_StDef 					*p_Source	= &g_MNData[3].MN_Read;

		p_Targer->Strength			= p_Source->Strength;
		p_Targer->Distance			= p_Source->Distance;
		p_Targer->Status			= p_Source->Status;
		p_Targer->L_or_R_Sta		= p_Source->L_or_R_Sta;
		p_Targer->DistanceLeft		= p_Source->DistanceLeft;
		p_Targer->DistanceRight		= p_Source->DistanceRight;
	}
}
/***************************************************************************************
** 函数名称:	AGV_DependenceDataInput_PGV
** 功能描述:	
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
void AGV_DependenceDataInput_PGV(void)
{
	/*导航PGV数据输入，默认仅能对地PGV*/
	{
		DepAGV_PGVData_InputTypeDef 	*p_Targer	= g_AGVDependenInputData.Navigation_QRData;
		PGV_Read_StDef 					*p_Source	= &g_GroundPGV.PGV_Read;

		p_Targer->QR_x			= p_Source->X;
		p_Targer->QR_y			= p_Source->Y;
		p_Targer->QR_z			= p_Source->Z;
		p_Targer->Offset_z		= p_Source->Z_Deviation;
		p_Targer->QR_Tag		= p_Source->Tag;
		p_Targer->PintSta		= p_Source->Status;
	}
	/*定位PGV数据输入，根据功能开关 可选择对地PGV定位 或者对工装PGV定位*/
	{
		DepAGV_PGVData_InputTypeDef 	*p_Targer	= g_AGVDependenInputData.Location_QRData;
		PGV_Read_StDef 					*p_Source	= NULL;

		if(1)	{p_Source = &g_GroundPGV.PGV_Read;}
		else	{p_Source = &g_ToolPGV.PGV_Read;}
		p_Targer->QR_x			= p_Source->X;
		p_Targer->QR_y			= p_Source->Y;
		p_Targer->QR_z			= p_Source->Z;
		p_Targer->Offset_z		= p_Source->Z_Deviation;
		p_Targer->QR_Tag		= p_Source->Tag;
		p_Targer->PintSta		= p_Source->Status;
	}
}
/***************************************************************************************
** 函数名称:	AGV_DependenceDataInput_VirtualOdo
** 功能描述:	
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
void AGV_DependenceDataInput_VirtualOdo(void)
{
	/*虚拟里程计输入， 主要是磁导航、二维码导航的减速功能需要*/
	{
		DepAGV_VirtualOdoData_InputTypeDef 	*p_Targer	= g_AGVDependenInputData.VirtualOdoData;
		OdometerData_t 						*p_Source	= &g_VirtualOdoData;

		p_Targer->RTx			= p_Source->RTx;
		p_Targer->RTy			= p_Source->RTy;
		p_Targer->RTz			= p_Source->RTz;
		p_Targer->Distance		= p_Source->Distance;
	}
}
/***************************************************************************************
** 函数名称:	AGV_DependenceDataInput
** 功能描述:	
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
void AGV_DependenceDataInput(void)
{
	/*SLAM数据*/
	AGV_DependenceDataInput_Slam();
	/*RFID数据*/
	AGV_DependenceDataInput_RFID();
	/*磁导航数据*/
	AGV_DependenceDataInput_Mag();
	/*PGV数据*/
	AGV_DependenceDataInput_PGV();
	/*虚拟里程计*/
	AGV_DependenceDataInput_VirtualOdo();
}
/***************************************************************************************
** 函数名称:	AGV_Skeleton_InPutChassisData_Dependence
** 功能描述:	将自动运行依赖底盘数据管道组织好，输入到自动运行框架中
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
void AGV_Skeleton_InPutChassisData_Dependence(void)
{
	/*实际运动模式输入*/
	if(ACC_IsPosWalkMode(AccData.ModeSwitchSMT))
	{
		g_HostData.ActualAGVWorkMode = AGVMode_GoLine;
	}
	else if(ACC_IsInfWalkMode(AccData.ModeSwitchSMT))
	{
		g_HostData.ActualAGVWorkMode = AGVMode_GoTransverse;
	}
	else if(AccData.ModeSwitchSMT == ACCMode_Rotation)
	{
		g_HostData.ActualAGVWorkMode = AGVMode_GoRotation;
	}
	else if(AccData.ModeSwitchSMT == ACCMode_Lift)
	{
		g_HostData.ActualAGVWorkMode = AGVMode_GoUpAndDown;
	}
	else
	{
		g_HostData.ActualAGVWorkMode = AGVMode_GoStill;
	}
	/*平台高度输入*/
	g_HostData.ActualHigh = g_LiftData.Height.Avg;
	
	/*急停按钮 对自动任务暂停 继续的输入*/
	{
		static uint8_t button_flag = 0x00;
		if(   button_flag == 0x00
			&& g_IOInputs.EmgStop.StableValue==1)
		{
		
			g_HostData.InputGoOnTaskFlag = 0X02;
			button_flag = 0x01;

		}
		else if(button_flag == 0x01 && g_IOInputs.EmgStop.StableValue==0)
		{
			button_flag = 0x00;
		}
	}
	/*故障停车后，自动实现任务暂停*/
	{
		static uint8_t Alarm_flag = 0x00;
		static int Radar_Flag=0;

		switch(Radar_Flag)
		{
			case 0:
			{
				if(g_SLN.in_ObsState == SLN_ObsState_Stop)
				{
					Radar_Flag = 1;
				}
			}
			break;
			default:
			case 1:
			{
				if(g_SLN.in_ObsState != SLN_ObsState_Stop && Radar_Flag == 1)
				{
					Radar_Flag = 0;
					g_HostData.StartAccFlag = 0;
					g_HostData.ActualForwardVelocity = 0;
					
				}
			}
			break;
		}
		
		if((AccData.in_FaultsType == ACC_FaultsType_ControlledStop || AccData.in_FaultsType == ACC_FaultsType_LoseControlStop)&&Alarm_flag == 0x00)
		{
			//g_HostData.InputPauseTaskFlag = 0X03;
			g_HostData.StartAccFlag = 0;
			g_HostData.ActualForwardVelocity = 0;
			Alarm_flag = 0x01;
        }
		else if((AccData.in_FaultsType != ACC_FaultsType_ControlledStop 
			&& AccData.in_FaultsType != ACC_FaultsType_LoseControlStop)
			&&Alarm_flag == 0x01)
		{
			Alarm_flag = 0x00;
		}
	}

	/*如果是麦轮车，运动解算需要填充遥感值，确保虚拟里程计解算正常*/
	if(g_Para.ChassisType == ACC_ChassisType_Turtle)
	{
		g_MotorResolving.Input.Aoxyz[0] = AccData.CalCmd.AOxyz.x;
		g_MotorResolving.Input.Aoxyz[1] = AccData.CalCmd.AOxyz.y;
		g_MotorResolving.Input.Aoxyz[2] = AccData.CalCmd.AOxyz.z;
		
		g_MotorResolving.Input.Oxyz[0] = AccData.CalCmd.VOxyz.x;
		g_MotorResolving.Input.Oxyz[1] = AccData.CalCmd.VOxyz.y;
		g_MotorResolving.Input.Oxyz[2] = AccData.CalCmd.VOxyz.z;
	}
}





/***************************************************************************************
** 函数名称: AGV_StatusUpdate
** 功能描述: AGV状态更新
** 参    数: None
** 返 回 值: bool
****************************************************************************************/
bool AGV_StatusUpdate(void)
{
	u8 i;
	
    //设备类型码
    g_DHR_AGVInformation[Para_Index_AGV_DeviceTypeCode] = 985;
	
    //设备序列号
    g_DHR_AGVInformation[Para_Index_AGV_DriverSN] = g_Para.EqpID;	
	
    //硬件版本
    g_DHR_AGVInformation[Para_Index_AGV_HARDVERSION] = 1000;
	
    //软件自动框架的版本
    g_DHR_AGVInformation[Para_Index_AGV_SOFTVERSION] = g_HostData.SoftVerSion;	
	
    //AGV运行状态
    g_DHR_AGVInformation[Para_Index_AGV_RunState] = g_HostData.AGVAutoMode;
		
    //控制模式	
    if(g_HostData.ControlMODEL == Style_Auto && g_MC_WorkStep == MC_WorkStep_Run)
	{
        g_DHR_AGVInformation[Para_Index_AGV_ControlMode] = 0x04;
		//诱骗调度系统切出调度的功能
		{
			static uint32_t ms = 0;
			if(g_HostData.TrickRCSFunc.ControlModeTrick == 1)
			{
				//持续上报是遥控状态；
				g_DHR_AGVInformation[Para_Index_AGV_ControlMode] = 0x01;
				//计时开始 大约5秒后 清除标志位，恢复正常的上报
				if(g_HostData.Sys1msCounter >= ms+5000)
				{
					g_HostData.TrickRCSFunc.ControlModeTrick = 0;
				}
			}
			else
			{
				ms = g_HostData.Sys1msCounter;
			}
		}
	}
    else
    {
		g_DHR_AGVInformation[Para_Index_AGV_ControlMode] = 0x01; 	
	}	
		
	//运行模式
	g_DHR_AGVInformation[Para_Index_AGV_RunMode] = AccData.CalCmd.Mode;
	
	//运动状态
	for (i = 0; i < g_MotorResolving.Para.MotorNumber; i++) 
	{
		// 添加速度死区阈值
		const float SPEED_DEADZONE = 0.01f;  // 例如0.01 RPM
		
		// 判断电机是否有效运行
		if(fabsf(SVO[i].Speed) > SPEED_DEADZONE) 
		{
			g_HostData.RunStatusFeedback = RunStatus_Run;
			break;  // 发现一个运行电机即可终止循环
		}else
			g_HostData.RunStatusFeedback = RunStatus_Stop;
	}
	
	g_DHR_AGVInformation[Para_Index_AGV_MovementState] = g_HostData.RunStatusFeedback;
	
		
    //电池电压
	g_DHR_AGVInformation[Para_Index_AGV_BatteryVoltage] = g_BMSData.BMS_Read.Voltage * 10;
    //电量
    g_DHR_AGVInformation[Para_Index_AGV_BatteryLevel] = g_BMSData.BMS_Read.SOC;		

    //上面的代码引起了stm32硬件错误中断，原因是u64操作没有对齐
    /*下面改用内存复制方法进行赋值*/
    {
        u64 TempFaultCode = 0;
        TempFaultCode = g_Faults.LoseControl.All+((u64)g_Faults.Auto.All<<32);
        memcpy(&((DSPCOMMOBJ_StDef*)(EQP_DSP.Data))->SystemFaultCodeLevel1_1,&TempFaultCode,sizeof(u64));

        TempFaultCode = g_Faults.ControlledStop.All;
        memcpy(&((DSPCOMMOBJ_StDef*)(EQP_DSP.Data))->SystemFaultCodeLevel2_1,&TempFaultCode,sizeof(u64));

        TempFaultCode = g_Faults.Remind.All;
        memcpy(&((DSPCOMMOBJ_StDef*)(EQP_DSP.Data))->SystemFaultCodeLevel3_1,&TempFaultCode,sizeof(u64));
    }
    

	/*避障雷达信息
    i = (CallBackSecuritySensorState(pMC,ForwardRadar) & 0x000F);
	j = ((CallBackSecuritySensorState(pMC,BackRadar)   & 0x000F)<<4);	  
	k = ((CallBackSecuritySensorState(pMC,LeftRadar)   & 0x000F)<<8);	
	l = ((CallBackSecuritySensorState(pMC,RightRadar)  & 0x000F)<<12);				
    Temp_SecuritySensorState = (i+j+k+l);  	
    g_DHR_AGVInformation[Para_Index_AGV_SecuritySensorState1] = Temp_SecuritySensorState; 
	*/
		
	//其他避障信息

	/*速度*/
	{
		int16_t Cmd_Vx = 0;int16_t Cmd_Vy = 0;int16_t Cmd_Vz = 0;
		static uint16_t Last_RTz = 0;
		static uint32_t RTz_Time = 0;
		float	V0xyz_x = AccData.CalCmd.VOxyz.x;
		float	V0xyz_y = AccData.CalCmd.VOxyz.y;

		if(V0xyz_x < 0)	{Cmd_Vx = -866.66f*V0xyz_x/V_H;}
		else			{Cmd_Vx =  866.66f*V0xyz_x/V_H;}

		if(V0xyz_x < 0)	{Cmd_Vy = -866.66f*V0xyz_y/V_H;}
		else			{Cmd_Vy =  866.66f*V0xyz_y/V_H;}

		if(g_HostData.ActualAGVWorkMode == AGVMode_GoRotation)
		{
			if(g_Sys_1ms_Counter >= RTz_Time + 1000)
			{
				Cmd_Vz = (g_VirtualOdoData.RTz - Last_RTz)*1000/g_Sys_1ms_Counter-RTz_Time;
				RTz_Time = g_Sys_1ms_Counter;
				Last_RTz = g_VirtualOdoData.RTz;
			}
		}
		else
		{
			Cmd_Vz = 0;
		}

		/*实时速度Vx */
		g_DHR_AGVInformation[Para_Index_AGV_RunningSpeed_Vx] = (s16)(Cmd_Vx);
		/*实时速度Vy*/
		g_DHR_AGVInformation[Para_Index_AGV_RunningSpeed_Vy] = (s16)(Cmd_Vy);		
		/*实时速度Vz */
		g_DHR_AGVInformation[Para_Index_AGV_RunningSpeed_Vz] = (s16)(Cmd_Vz);
	}
	//坐标
	{
		BYTE4_UNION *pBU4=0; 	
		//实时更新当前坐标
		pBU4 = (BYTE4_UNION *)&g_DHR_AGVInformation[Para_Index_AGV_CurrentXValue];
		pBU4->B4_S32 = g_HostData.NavigationData.NavLidar_x;
		
		pBU4 = (BYTE4_UNION *)&g_DHR_AGVInformation[Para_Index_AGV_CurrentYValue];		
		pBU4->B4_S32 =g_HostData.NavigationData.NavLidar_y;

		g_DHR_AGVInformation[Para_Index_AGV_CurrentZValue] = g_HostData.NavigationData.NavLidar_Toward;
		
		//当前位置标签值（SLAM/GPS导航下不需要给调度系统写入）
		if(g_HostData.AGVNavigateMode == AGVNavigateMode_QR || g_HostData.AGVNavigateMode == AGVNavigateMode_Magnetic)	
		{
			g_DHR_AGVInformation[Para_Index_AGV_CurrentQRCode] = g_HostData.CurrentPointTag;
		}
	}
	//当前路径航向
	g_DHR_AGVInformation[Para_Index_AGV_CurrentPathHeading] = g_HostData.AGVCurrentCourse;
	
	//当前升降高度
	g_DHR_AGVInformation[Para_Index_AGV_LiftingPlatformHigh] = g_HostData.ActualHigh;
		
	//电机转速1，2，3，4 --- 获取电机转速	
    g_DHR_AGVInformation[Para_Index_AGV_MotorSpeedNum1] = SVO[0].Speed;
    g_DHR_AGVInformation[Para_Index_AGV_MotorSpeedNum2] = SVO[1].Speed;
    g_DHR_AGVInformation[Para_Index_AGV_MotorSpeedNum3] = SVO[2].Speed;
    g_DHR_AGVInformation[Para_Index_AGV_MotorSpeedNum4] = SVO[3].Speed;
    
	//电机编码器1，2，3，4 --- 获取电机编码器脉冲数

	//驱动器故障代码1，2，3，4 --- 获取驱动器故障代码

	//模组角度1
	//模组角度2
	
	//前、后、左、右磁导航传感器数据
	g_DHR_AGVInformation[Para_Index_AGV_MagneticData_F] = g_HostData.MagneticData.Distance_F;
	g_DHR_AGVInformation[Para_Index_AGV_MagneticData_B] = g_HostData.MagneticData.Distance_B;
	g_DHR_AGVInformation[Para_Index_AGV_MagneticData_L] = g_HostData.MagneticData.Distance_L;
	g_DHR_AGVInformation[Para_Index_AGV_MagneticData_R] = g_HostData.MagneticData.Distance_R;
	//RFID标签数据
	if(g_HostData.RFIDData.ReadStatus != 0X00 )
	{
		g_DHR_AGVInformation[Para_Index_AGV_RFID_Value]	= g_HostData.RFIDData.TagValue;
	}
	else
	{
		g_DHR_AGVInformation[Para_Index_AGV_RFID_Value]	= g_HostData.CurrentPointTag;
	}
	

	//二维码传感器码值、二维码传感器X轴、Y轴、角度值
	g_DHR_AGVInformation[Para_Index_AGV_QRCodeData] 		= g_HostData.ParkQRData.QR_Tag;	//二维码
	g_DHR_AGVInformation[Para_Index_AGV_QRCodeData_x] 		= g_HostData.ParkQRData.QR_x;	//二维码x数据
	g_DHR_AGVInformation[Para_Index_AGV_QRCodeData_y] 		= g_HostData.ParkQRData.QR_y;	//二维码y数据
	g_DHR_AGVInformation[Para_Index_AGV_QRCodeData_angle] 	= g_HostData.ParkQRData.QR_z;	//二维码角度数据


	//设备控制判断信息
	//执行路径状态 --- 为了统一、便于校对而写
	g_DHR_AGVInformation[Para_Index_AGV_ExecutionPathState] = g_HostData.ExecutionPathState;
	 
	//充电状态
//	g_DHR_AGVInformation[Para_Index_AGV_ChargingState] = g_IOOutputs.ChargingMC.OutValue;
	//充电状态
	if ((g_ActiveCharge.Step == 4 ||g_ActiveCharge.Step == 0) && g_PowerOn.PowerOnStep == ACC_PowerOnStep_StartupComplete)
	{
		g_DHR_AGVInformation[Para_Index_AGV_ChargingState] = 0;
		g_ActiveCharge.Step = 0;
	}
	else
		g_DHR_AGVInformation[Para_Index_AGV_ChargingState] = 1;

   //路径下载状态
	g_DHR_AGVInformation[Para_Index_AGV_WayPointDownloadStatus] = PathTypePara.g_DownloadStatus;
	
	//电池总电流
	g_DHR_AGVInformation[Para_Index_AGV_BatTotalCurrent] = g_BMSData.BMS_Read.Current * 10;	
	
	//电池健康状态

	g_DHR_AGVInformation[Para_Index_AGV_BatHealthStatus] =  g_BMSData.BMS_Read.SOH;

	return 1;
}


/***************************************************************************************
** 函数名称:	AGV_AutoExcitation_Update
** 功能描述:	将自激AGV的依赖数据更新
** 参    数:	无
** 返 回 值:	无
****************************************************************************************/
void AGV_AutoExcitation_Update(void)
{
	g_AutoExcitation.AE_Ms = g_Sys_1ms_Counter;
	g_AutoExcitation.AE_In.Rtx = g_ROKITData.ROKIT_Read.LocationX/10.0f;
	g_AutoExcitation.AE_In.Rtx = g_ROKITData.ROKIT_Read.LocationY/10.0f;
	if(g_RFIDData.RFID_Read.Status != 0)
	{
		g_AutoExcitation.AE_In.RtTag = g_RFIDData.RFID_Read.Tag;
	}
	else if(g_NavigationPGV.PGV_Read.Status != 0)
	{
		g_AutoExcitation.AE_In.RtTag = g_NavigationPGV.PGV_Read.Tag;
	}else{}

}

/***********************************************************
** 函数名称:		ObjUpdate_AGV
** 功能描述: 	
** 参    数:	None
** 返 回 值:		None
***********************************************************/
void ObjUpdate_AGV(void)
{
	//
	AGV_AutoExcitation_Update();
	
	//AGV状态更新
	AGV_StatusUpdate();
	
	/*所有的传感器输入到自动框架的数据输入管道*/
	AGV_DependenceDataInput();

	/*自动运行需要在运动控制的函数中循环调用的依赖,其中有虚拟里程计功能*/
	AGV_MotionControl_Dependence();

	/*任务类型通知更新*/
	if(g_DHR_AGVInformation[Para_Index_AGV_TaskTypeNotify] == 0x01)
	{
		g_HostData.AGVTaskNotifyType = AGVTaskTypeNotify_Park;
	}
	else if(g_DHR_AGVInformation[Para_Index_AGV_TaskTypeNotify] == 0x02)
	{
		g_HostData.AGVTaskNotifyType = AGVTaskTypeNotify_Charge;
	}
	else if(g_DHR_AGVInformation[Para_Index_AGV_TaskTypeNotify] == 0x03)
	{
		g_HostData.AGVTaskNotifyType = AGVTaskTypeNotify_Goods;
	}
	else
	{
		g_HostData.AGVTaskNotifyType = AGVTaskTypeNotify_NULL;
	}

	//控制模式输入
	if(g_ChannelSwitch.in_Channel == ACCCC_Auto)
	{
		g_HostData.ControlMODEL = Style_Auto;
	}
	else if(g_ChannelSwitch.in_Channel == ACCCC_WirelessRemote || g_ChannelSwitch.in_Channel == ACCCC_WiredRemote)
	{
		g_HostData.ControlMODEL = Style_Remote;
	}
	else
	{
		g_HostData.ControlMODEL = Style_Control_MODEL_NULL;
	}

	/*自动运行框架代码的输入管道*/
	AGV_Skeleton_InData_Dependence();

}

/************************************END OF FILE************************************/
