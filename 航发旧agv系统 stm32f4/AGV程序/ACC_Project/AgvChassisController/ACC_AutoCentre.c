/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ACC_AutoCentre.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       :  2025-06-13
	* Description:
	******************************************************************************
**/

#define  ACC_AutoCentre_GLOBALS
#include "ACC_AutoCentre.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "CommConf.h"
#include "IOConf.h"
/*取得自动对中功能的配置*/
static Para_AutoCentre_Def *p_Para = &g_Para.AutoCentre;

uint8_t AutoCentre_Sta = 0;  //0-初始态 1-进行 2-暂停 3-结束（终止）

uint8_t AutoCentre_STM = 0;

/***********************************************************
** 函数名称:	AutoCentre_Ctrl_Init
** 功能描述: 自动对中功能相关参数初始化
** 参    数: None
** 返 回 值:	None
***********************************************************/
void AutoCentre_Ctrl_Init(void)
{
	FixedJustify_Para_TypeDef	*p_ParaData	= &(g_MagneticJustify.Para);
	//对工装
	{
		p_ParaData	= &(g_ToolJustify.Para);
		//到位精度（死区值）
		if(p_ParaData->x_offset_max == 0) {p_ParaData->x_offset_max = 1;}   //单位1mm
		if(p_ParaData->y_offset_max == 0) {p_ParaData->y_offset_max = 1;}	//单位1mm
		if(p_ParaData->z_offset_max == 0) {p_ParaData->z_offset_max = 10;}	//单位0.1°
		//最大纠偏速度
		if(p_ParaData->max_x_Speed == 0) {p_ParaData->max_x_Speed = 10.0f;}
		if(p_ParaData->max_y_Speed == 0) {p_ParaData->max_y_Speed = 10.0f;}
		if(p_ParaData->max_z_Speed == 0) {p_ParaData->max_z_Speed = 10.0f;}
		//最小纠偏速度
		if(p_ParaData->min_x_Speed == 0) {p_ParaData->min_x_Speed = 3.0f;}
		if(p_ParaData->min_y_Speed == 0) {p_ParaData->min_y_Speed = 3.0f;}
		if(p_ParaData->min_z_Speed == 0) {p_ParaData->min_z_Speed = 3.0f;}
		
		if(p_ParaData->decel_zone == 0) 	{p_ParaData->decel_zone = 10.0f;}		// 开始减速区域 (mm)
		if(p_ParaData->stop_threshold == 0) {p_ParaData->stop_threshold = 1.0f;}	// 停止阈值 (mm)
		if(p_ParaData->kp_near == 0) 		{p_ParaData->kp_near = 0.4f;}			// 近距离比例系数
		if(p_ParaData->kp_far == 0) 		{p_ParaData->kp_far = 2.0f;}			// 远距离比例系数
		if(p_ParaData->kp_transition == 0)  {p_ParaData->kp_transition = 50.0f;}	// 过渡区起始偏差 (mm)

		g_ToolJustify.Temp.FixedJustifyStep = 0;
	}
	//对地
	{
		p_ParaData	= &(g_GroundJustify.Para);
		//到位精度（死区值）
		if(p_ParaData->x_offset_max == 0) 	{p_ParaData->x_offset_max = 1;}   //单位1mm
		if(p_ParaData->y_offset_max == 0) 	{p_ParaData->y_offset_max = 1;}	//单位1mm
		if(p_ParaData->z_offset_max == 0) 	{p_ParaData->z_offset_max = 100;}	//单位0.1°
		//最大纠偏速度
		if(p_ParaData->max_x_Speed == 0) 	{p_ParaData->max_x_Speed = -10.0f;}
		if(p_ParaData->max_y_Speed == 0) 	{p_ParaData->max_y_Speed = 10.0f;}
		if(p_ParaData->max_z_Speed == 0) 	{p_ParaData->max_z_Speed = 10.0f;}
		//最小纠偏速度
		if(p_ParaData->min_x_Speed == 0) 	{p_ParaData->min_x_Speed = -5.0f;}
		if(p_ParaData->min_y_Speed == 0) 	{p_ParaData->min_y_Speed = 5.0f;}
		if(p_ParaData->min_z_Speed == 0) 	{p_ParaData->min_z_Speed = 5.0f;}

		if(p_ParaData->decel_zone == 0) 	{p_ParaData->decel_zone = 10.0f;}		// 开始减速区域 (mm)
		if(p_ParaData->stop_threshold == 0) {p_ParaData->stop_threshold = 1.0f;}	// 停止阈值 (mm)
		if(p_ParaData->kp_near == 0) 		{p_ParaData->kp_near = 0.4f;}			// 近距离比例系数
		if(p_ParaData->kp_far == 0) 		{p_ParaData->kp_far = 2.0f;}			// 远距离比例系数
		if(p_ParaData->kp_transition == 0)  {p_ParaData->kp_transition = 50.0f;}	// 过渡区起始偏差 (mm)

		g_GroundJustify.Temp.FixedJustifyStep = 0;
	}
	//超声波
	{
		p_ParaData	= &(g_UltrasonicJustify.Para);
		//到位精度（死区值）
		if(p_ParaData->x_offset_max == 0) 	{p_ParaData->x_offset_max = 1;}   //单位1mm
		if(p_ParaData->y_offset_max == 0) 	{p_ParaData->y_offset_max = 50;}	//单位1mm
		if(p_ParaData->z_offset_max == 0) 	{p_ParaData->z_offset_max = 100;}	//单位0.1°
		//最大纠偏速度
		if(p_ParaData->max_x_Speed == 0) 	{p_ParaData->max_x_Speed = -10.0f;}
		if(p_ParaData->max_y_Speed == 0) 	{p_ParaData->max_y_Speed = 10.0f;}
		if(p_ParaData->max_z_Speed == 0) 	{p_ParaData->max_z_Speed = 10.0f;}
		//最小纠偏速度
		if(p_ParaData->min_x_Speed == 0) 	{p_ParaData->min_x_Speed = -5.0f;}
		if(p_ParaData->min_y_Speed == 0) 	{p_ParaData->min_y_Speed = 5.0f;}
		if(p_ParaData->min_z_Speed == 0) 	{p_ParaData->min_z_Speed = 5.0f;}
		
		if(p_ParaData->decel_zone == 0) 	{p_ParaData->decel_zone = 10.0f;}		// 开始减速区域 (mm)
		if(p_ParaData->stop_threshold == 0) {p_ParaData->stop_threshold = 1.0f;}	// 停止阈值 (mm)
		if(p_ParaData->kp_near == 0) 		{p_ParaData->kp_near = 0.4f;}			// 近距离比例系数
		if(p_ParaData->kp_far == 0) 		{p_ParaData->kp_far = 2.0f;}			// 远距离比例系数
		if(p_ParaData->kp_transition == 0)  {p_ParaData->kp_transition = 50.0f;}	// 过渡区起始偏差 (mm)

		g_UltrasonicJustify.Temp.FixedJustifyStep = 0;
	}

	//联动找正
	{
		p_ParaData	= &(g_LinkageJustify.Para);
		//到位精度（死区值）
		if(p_ParaData->x_offset_max == 0) 	{p_ParaData->x_offset_max = 1;}   //单位1mm
		if(p_ParaData->y_offset_max == 0) 	{p_ParaData->y_offset_max = 50;}	//单位1mm
		if(p_ParaData->z_offset_max == 0) 	{p_ParaData->z_offset_max = 100;}	//单位0.1°
		//最大纠偏速度
		if(p_ParaData->max_x_Speed == 0) 	{p_ParaData->max_x_Speed = -10.0f;}
		if(p_ParaData->max_y_Speed == 0) 	{p_ParaData->max_y_Speed = 10.0f;}
		if(p_ParaData->max_z_Speed == 0) 	{p_ParaData->max_z_Speed = 10.0f;}
		//最小纠偏速度
		if(p_ParaData->min_x_Speed == 0) 	{p_ParaData->min_x_Speed = -5.0f;}
		if(p_ParaData->min_y_Speed == 0) 	{p_ParaData->min_y_Speed = 5.0f;}
		if(p_ParaData->min_z_Speed == 0) 	{p_ParaData->min_z_Speed = 5.0f;}
		
		if(p_ParaData->decel_zone == 0) 	{p_ParaData->decel_zone = 10.0f;}		// 开始减速区域 (mm)
		if(p_ParaData->stop_threshold == 0) {p_ParaData->stop_threshold = 1.0f;}	// 停止阈值 (mm)
		if(p_ParaData->kp_near == 0) 		{p_ParaData->kp_near = 0.4f;}			// 近距离比例系数
		if(p_ParaData->kp_far == 0) 		{p_ParaData->kp_far = 2.0f;}			// 远距离比例系数
		if(p_ParaData->kp_transition == 0)  {p_ParaData->kp_transition = 50.0f;}	// 过渡区起始偏差 (mm)

		g_LinkageJustify.Temp.FixedJustifyStep = 0;
	}
	
	//SLAM自动上线
	{
		p_ParaData	= &(g_SLAM_CoordinatePoint.Para);
		//到位精度（死区值）
		if(p_ParaData->x_offset_max == 0)	{p_ParaData->x_offset_max = 10;}  //单位1mm
		if(p_ParaData->y_offset_max == 0)	{p_ParaData->y_offset_max = 10;}	//单位1mm
		if(p_ParaData->z_offset_max == 0)	{p_ParaData->z_offset_max = 10;}	//单位0.1°
		//最大纠偏速度
		if(p_ParaData->max_x_Speed == 0)	{p_ParaData->max_x_Speed = 10.0f;}
		if(p_ParaData->max_y_Speed == 0)	{p_ParaData->max_y_Speed = 10.0f;}
		if(p_ParaData->max_z_Speed == 0)	{p_ParaData->max_z_Speed = 10.0f;}
		//最小纠偏速度
		if(p_ParaData->min_x_Speed == 0)	{p_ParaData->min_x_Speed = 5.0f;}
		if(p_ParaData->min_y_Speed == 0)	{p_ParaData->min_y_Speed = 5.0f;}
		if(p_ParaData->min_z_Speed == 0)	{p_ParaData->min_z_Speed = 5.0f;}
		
		if(p_ParaData->decel_zone == 0) 	{p_ParaData->decel_zone = 10.0f;}		// 开始减速区域 (mm)
		if(p_ParaData->stop_threshold == 0) {p_ParaData->stop_threshold = 1.0f;}	// 停止阈值 (mm)
		if(p_ParaData->kp_near == 0) 		{p_ParaData->kp_near = 0.4f;}			// 近距离比例系数
		if(p_ParaData->kp_far == 0) 		{p_ParaData->kp_far = 2.0f;}			// 远距离比例系数
		if(p_ParaData->kp_transition == 0)  {p_ParaData->kp_transition = 50.0f;}	// 过渡区起始偏差 (mm)

		g_SLAM_CoordinatePoint.Temp.FixedJustifyStep = 0;
	}

	//磁条对正
	{
		p_ParaData	= &(g_MagneticJustify.Para);
		//到位精度（死区值）
		if(p_ParaData->x_offset_max == 0) 	{p_ParaData->x_offset_max = 2;}  //单位1mm
		if(p_ParaData->y_offset_max == 0) 	{p_ParaData->y_offset_max = 2;}	//单位1mm
		if(p_ParaData->z_offset_max == 0) 	{p_ParaData->z_offset_max = 2;}	//单位0.1°
		//最大纠偏速度
		if(p_ParaData->max_x_Speed == 0) 	{p_ParaData->max_x_Speed =  5.0f;}
		if(p_ParaData->max_y_Speed == 0) 	{p_ParaData->max_y_Speed = -5.0f;}
		if(p_ParaData->max_z_Speed == 0) 	{p_ParaData->max_z_Speed = -3.0f;}
		//最小纠偏速度
		if(p_ParaData->min_x_Speed == 0) 	{p_ParaData->min_x_Speed =  1.0f;}
		if(p_ParaData->min_y_Speed == 0) 	{p_ParaData->min_y_Speed = -1.0f;}
		if(p_ParaData->min_z_Speed == 0) 	{p_ParaData->min_z_Speed = -1.0f;}

		if(p_ParaData->decel_zone == 0) 	{p_ParaData->decel_zone = 10.0f;}		// 开始减速区域 (mm)
		if(p_ParaData->stop_threshold == 0) {p_ParaData->stop_threshold = 1.0f;}	// 停止阈值 (mm)
		if(p_ParaData->kp_near == 0) 		{p_ParaData->kp_near = 0.4f;}			// 近距离比例系数
		if(p_ParaData->kp_far == 0) 		{p_ParaData->kp_far = 2.0f;}			// 远距离比例系数
		if(p_ParaData->kp_transition == 0)  {p_ParaData->kp_transition = 50.0f;}	// 过渡区起始偏差 (mm)

		g_MagneticJustify.Temp.FixedJustifyStep = 0;
	}

	// 如果未指定顺序，使用默认顺序 Z->X->Y
	if(p_ParaData->step_count == 0) 
	{
		p_ParaData->adjust_order[0] = ADJUST_STEP_Z;	//调整顺序列举
		p_ParaData->adjust_order[1] = ADJUST_STEP_X;
		p_ParaData->adjust_order[2] = ADJUST_STEP_Y;
		p_ParaData->step_count = 3;						//调整总步骤
	}
}
/***********************************************************
** 函数名称:	AutoCentre_Output_Clear
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void AutoCentre_Output_Clear(void)
{
	OrgCtrlCmd_TypeDef	*p_CtrlCmdCmd = &g_ChannelSwitch.CtrlCmd;
	//模式
	p_CtrlCmdCmd->Mode = ACCMode_Still;
	//摇杆命令
	p_CtrlCmdCmd->Vxyz.x = 0;
	p_CtrlCmdCmd->Vxyz.y = 0;
	p_CtrlCmdCmd->Vxyz.z = 0;

	//上升命令
	p_CtrlCmdCmd->Button.LiftUp = 0;
	//下降命令
	p_CtrlCmdCmd->Button.LiftDown = 0;
}

/***********************************************************
** 函数名称:	AutoCentre_Input_Ctrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void AutoCentre_Input_Ctrl(void)
{
	FixedJustify_Input_TypeDef	*p_InData	= &(g_ToolJustify.Input);
	//区分于不同的定位方式，组织不同的传感器偏差给到 输入数据管道
	{
		switch(p_Para->FunType)
		{
			default:{break;}
			case Para_AutoCentre_GroudPGV:
			{
				PGV_Read_StDef 				*PGV_Data 	= &(g_ToolPGV.PGV_Read);
				p_InData	= &(g_GroundJustify.Input);
				//偏差数据装入
				{
					p_InData->x 	= PGV_Data->X;
					p_InData->y 	= PGV_Data->Y;
					p_InData->z 	= PGV_Data->Z_Deviation*100;//float装入s16 需要扩大10倍
					p_InData->Sta 	= PGV_Data->Status;
				}
			}break;
			case Para_AutoCentre_ToolPGV:
			{
				PGV_Read_StDef 				*PGV_Data 	= &(g_ToolPGV.PGV_Read);
				p_InData	= &(g_ToolJustify.Input);
				//偏差数据装入
				{
					p_InData->x 	= PGV_Data->X;
					p_InData->y 	= PGV_Data->Y;
					p_InData->z 	= PGV_Data->Z_Deviation*100;//float装入s16 需要扩大10倍
					p_InData->Sta 	= PGV_Data->Status;
				}
			}break;
			case Para_AutoCentre_Ultrasonic:
			{
				p_InData	= &(g_UltrasonicJustify.Input);
				//偏差数据装入
				{
					p_InData->x 	= 0;
					p_InData->y 	= (EQP_AD.RD[0] / 229.94f) + 15 - g_UltrasonicJustify.Para.y_offset_max; //
					p_InData->z 	= 0;
					p_InData->Sta 	= 1;
				}
			}break;
			case Para_AutoCentre_Linkage:
			{
				p_InData	= &(g_LinkageJustify.Input);
				//偏差数据装入
				{
	//				p_InData->x 	= g_AssemlySensorData.SpotDis.Cal;	//X方向数据来源为光斑传感器
	//				p_InData->y 	= (g_AssemlySensorData.LaserDis_L.Cal + g_AssemlySensorData.LaserDis_R.Cal)/2; 	//Y方向数据来源为左右激光测距传感器
	//				p_InData->z 	= (g_AssemlySensorData.LaserDis_L.Cal - g_AssemlySensorData.LaserDis_R.Cal);	//Z方向数据来源为左右激光测距传感器的差值
	//				if(g_AssemlySensorData.SpotDis.Org != 65535
	//					&&g_AssemlySensorData.LaserDis_L.Cal != 65535 
	//					&&g_AssemlySensorData.LaserDis_R.Cal != 65535)
	//				{
	//					p_InData->Sta 	= 1;			//状态来源为所有传感器是否有效检测
	//				}else
	//				{
	//					p_InData->Sta 	= 0;
	//				}
				}
			}break;
			case Para_AutoCentre_SLAMOnline:
			{
				p_InData	= &(g_SLAM_CoordinatePoint.Input);
				//偏差数据装入
				{
					p_InData->x 	= g_ROKITData.ROKIT_Read.LocationX - SLAM_CoordinatePoint.LocationX;
					p_InData->y 	= g_ROKITData.ROKIT_Read.LocationY - SLAM_CoordinatePoint.LocationY;
					p_InData->z 	= g_ROKITData.ROKIT_Read.LocationZ - SLAM_CoordinatePoint.LocationZ;
					p_InData->rotation_angle = g_ROKITData.ROKIT_Read.LocationZ;											//实时角度
					g_SLAM_CoordinatePoint.Temp.x_context.target_rotation_angle = XAdjust_Parameter.target_rotation_angle; 	//目标角度
					p_InData->Sta 	= 1;
				}
			}break;
			case Para_AutoCentre_Magnetic:
			{
				p_InData	= &(g_MagneticJustify.Input);
				//偏差数据装入
				{
					p_InData->x 	= (g_MNData[0].MN_Read.Distance) - (g_MNData[1].MN_Read.Distance);			//X差值
					p_InData->y 	= (g_MNData[2].MN_Read.Distance) + (g_MNData[3].MN_Read.Distance);			//Y差值
					p_InData->z 	= (g_MNData[0].MN_Read.Distance) + (g_MNData[1].MN_Read.Distance);			//z差值
					p_InData->rotation_angle = g_ROKITData.ROKIT_Read.LocationZ;											//实时角度
					g_SLAM_CoordinatePoint.Temp.x_context.target_rotation_angle = XAdjust_Parameter.target_rotation_angle; 	//目标角度
					
					if(	  g_MNData[0].MN_Read.Distance != 0 && g_MNData[1].MN_Read.Distance != 0
						&&g_MNData[2].MN_Read.Distance != 0 && g_MNData[3].MN_Read.Distance != 0)
						p_InData->Sta 	= 1;
					else
						p_InData->Sta 	= 0;
				}
			}break;
		}
	}

	//实际模式
	{
		ACCMode_TypeDef 			Mode		= AccData.ModeSwitchSMT;
		if(Mode == ACCMode_PosFlexible)
		{
			p_InData->ActualMode = FixedJustify_Mode_PosFlexible;
		}
		else if(Mode == ACCMode_InfFlexible)
		{
			p_InData->ActualMode = FixedJustify_Mode_InfFlexible;
		}
		else if(Mode == ACCMode_Rotation)
		{
			p_InData->ActualMode = FixedJustify_Mode_Rotation;
		}
		else
		{
			p_InData->ActualMode = FixedJustify_Still;
		}
	}
}

/***********************************************************
** 函数名称:	AutoCentre_Output_Ctrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void AutoCentre_Output_Ctrl(void)
{
	if(AutoCentre_STM == 2)
	{
		FixedJustify_Output_TypeDef	*p_OutData	= &(g_ToolJustify.Output);
		OrgCtrlCmd_TypeDef	*p_CtrlCmdCmd 		= &g_ChannelSwitch.CtrlCmd;
		//区分不同的定位方式
		{
			if(p_Para->FunType == Para_AutoCentre_GroudPGV)
			{
				p_OutData	= &(g_GroundJustify.Output);
			}
			else if(p_Para->FunType == Para_AutoCentre_ToolPGV)
			{
				p_OutData	= &(g_ToolJustify.Output);
			}
			else if(p_Para->FunType == Para_AutoCentre_Ultrasonic)
			{
				p_OutData	= &(g_UltrasonicJustify.Output);
			}
			else if(p_Para->FunType == Para_AutoCentre_Linkage)
			{
				p_OutData	= &(g_LinkageJustify.Output);
			}
			else if(p_Para->FunType == Para_AutoCentre_SLAMOnline)
			{
				p_OutData	= &(g_SLAM_CoordinatePoint.Output);
			}
			else if(p_Para->FunType == Para_AutoCentre_Magnetic)
			{
				p_OutData	= &(g_MagneticJustify.Output);
			}
		}
		//目标模式
		{
			FixedJustify_Mode_EnumDef	Mode	= p_OutData->TargetMode;
			if(Mode == FixedJustify_Mode_PosFlexible)
			{
				p_CtrlCmdCmd->Mode = ACCMode_PosFlexible;
			}
			else if(Mode == FixedJustify_Mode_InfFlexible)
			{
				p_CtrlCmdCmd->Mode = ACCMode_InfFlexible;
			}
			else if(Mode == FixedJustify_Mode_Rotation)
			{
				p_CtrlCmdCmd->Mode = ACCMode_Rotation;
			}
			else
			{
				p_CtrlCmdCmd->Mode = ACCMode_Still;
			}
		}
		//遥感命令
		{
			ACC_Joystick_TypeDef		*p_AccVxyz = &(p_CtrlCmdCmd->Vxyz);
			p_AccVxyz->x = p_OutData->Vx;
			p_AccVxyz->y = p_OutData->Vy;
			p_AccVxyz->z = p_OutData->Vz;
		}
		//挡位
		{
			p_CtrlCmdCmd->WalkGear = 1;
			p_CtrlCmdCmd->LiftGear = 1;
		}
	}
}

/***********************************************************
** 函数名称	AutoCentre_STM_Ctrl
** 功能描述:	否需要执行自动对中的判断;
** 参    数:	None
** 返 回 值:	0-不需要对中		其他-不需要进行对中
***********************************************************/
__weak uint8_t AutoCentre_CtrlFlagCheck(void)
{
	static uint32_t ToolingJustify_DelayTime = 0;
	uint8_t res = 0;
	uint8_t temp = 0;
	
	temp = g_ChannelSwitch.CtrlCmd.Button.ToolingAutoadj;
	if (temp == 1)
	{
		if (ToolingJustify_DelayTime == 0)
		{
			ToolingJustify_DelayTime = g_Sys_1ms_Counter;
		}
		if (g_Sys_1ms_Counter > ToolingJustify_DelayTime + 2000)
		{
			res = 1;
		}
	}
	else
	{
		ToolingJustify_DelayTime = 0;
	}
	return res;
}
/***********************************************************
** 函数名称:	AutoCentre_STM_Ctrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
void AutoCentre_STM_Ctrl(void)
{
	FixedJustify_TypeDef	*p_Justify;
	//区分不同的定位方式
	{
		if(p_Para->FunType == Para_AutoCentre_GroudPGV)
		{
			p_Justify	= &(g_GroundJustify);
		}
		else if(p_Para->FunType == Para_AutoCentre_ToolPGV)
		{
			p_Justify	= &(g_ToolJustify);
		}
		else if(p_Para->FunType == Para_AutoCentre_Ultrasonic)
		{
			p_Justify	= &(g_UltrasonicJustify);
		}
		else if(p_Para->FunType == Para_AutoCentre_Linkage)
		{
			p_Justify	= &(g_LinkageJustify);
		}
		else if(p_Para->FunType == Para_AutoCentre_SLAMOnline)
		{
			p_Justify	= &(g_SLAM_CoordinatePoint);
		}
		else if(p_Para->FunType == Para_AutoCentre_Magnetic)
		{
			p_Justify	= &(g_MagneticJustify);
		}
	}

	//1、记录当前的真实遥控器模式 如果出现模式切换 当前自动对中停止
	{
		static uint8_t WorkMode_Old = 0XAA;
		if(WorkMode_Old != g_WHData.WorkMode)
		{
			WorkMode_Old = g_WHData.WorkMode;
			//停止自动对中
			if(AutoCentre_Sta == 1)
			{
				AutoCentre_Sta = 3;
			}
		}
	}
	//2、检验传感器是否丢失目标，如果丢失目标 报警 自动对中停止
	{
		if(0)
		{
			AutoCentre_Sta = 3;
		}
		
	}
	//3、记录当的对正方式，如果出现变化 复位状态机；
	{
		static Para_AutoCentreFun_EnumDef		FunType_Old;
		if(FunType_Old != p_Para->FunType)
		{
			AutoCentre_Sta = 0;
			AutoCentre_STM = 0;
			p_Justify->Output.BeginFlag = 0;
			FunType_Old = p_Para->FunType;
		}
		
	}
	switch(AutoCentre_STM)
	{
		default:
			break;
		//初始化
		case 0:
		{
			AutoCentre_Ctrl_Init();
			AutoCentre_STM = 1;

		}break;
		//检测是触发对中功能
		case 1:
		{
			//整车差速记录对正前的角度
			g_SLAM_CoordinatePoint.Temp.x_context.initial_angle = g_ROKITData.ROKIT_Read.LocationZ;
			//
			if(AutoCentre_CtrlFlagCheck() != 0X00
			 &&p_Justify->Input.Sta == 1)
			{
				AutoCentre_STM = 2;
				AutoCentre_Sta = 1;
				p_Justify->Output.BeginFlag = 1;
			}
		}break;
		//执行对中功能
		case 2:
		{
			//自动对中执行
			if(AutoCentre_Sta == 1)
			{
				//对正进行中标志位
				p_Justify->Output.InExecutionFlag = 1;
				//方法调用
				app_FixedJustify_main(p_Justify);
				//完成后状态机自动跳转
				if(p_Justify->Output.FinishFlag == 1)
				{
					p_Justify->Output.FinishFlag = 0;
					AutoCentre_STM = 3;
				}
			}
			//暂停对中
			else if(AutoCentre_Sta == 2||p_Justify->Input.Sta == 0)
			{
				p_Justify->Output.Vx = 0;
				p_Justify->Output.Vy = 0;
				p_Justify->Output.Vz = 0;
			}
			//停止对中
			else if(AutoCentre_Sta == 3)
			{
				AutoCentre_STM = 3;
			}
			else
			{
				//其他设计外的情况	
			}
		}break;
		//退出对中功能
		case 3:
		{
			//清除控制指令
			AutoCentre_Output_Clear();
			AutoCentre_STM = 1;
			AutoCentre_Sta = 0;
			p_Justify->Output.BeginFlag = 0;
			p_Justify->Output.InExecutionFlag = 0;
			p_Justify->Temp.FixedJustifyStep = 0;
		}break;
	}
}

//自动对中功能
void AutoCentre_Ctrl(void)
{
	/*判断功能是否初始化*/
	if(p_Para->FunType == Para_AutoCentre_Null_Fun)
	{
		return;
	}
	if(p_Para->IntPutType == Para_AutoCentre_Null_Input)
	{
		return;
	}
	
	//1、自动对中输入管道创建
	{
		AutoCentre_Input_Ctrl();
	}
	//2、自动对中状态机执行
	{
		AutoCentre_STM_Ctrl();
	}
	//3、自动对中输出管道接管真实遥感
	{
		AutoCentre_Output_Ctrl();
	}

}



/************************************END OF FILE************************************/
