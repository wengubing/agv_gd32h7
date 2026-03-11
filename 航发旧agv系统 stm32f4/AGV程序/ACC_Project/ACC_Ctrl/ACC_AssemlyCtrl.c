/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ACC_AssemlyCtrl.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
#define  ACC_ASSEMLY_CTRL_GLOBALS

#include <stdbool.h>
#include "ACC_AssemlyCtrl.h"
#include "ACC_Project.h"
#include "ACC_Init.h"
#include "Debug.h"	
#include "app_Assembly.h"

#include "ObjUpdate_Assemly.h"

extern bool CheckWalk(ACC_DATA_TypeDef* p);

//外部函数声明
extern bool MotorResolving_Init(MotorResolving_TypeDef *p);
extern bool MotorResolving_Init_Asb(MotorResolving_TypeDef *p,f32 OffsetX,f32 OffsetY);
extern bool MecanumResolving_Init(Mecanum_TypeDef *p);
extern bool MecanumResolving_Init_Asb(Mecanum_TypeDef* p, f32 OffsetX, f32 OffsetY);


ACC_Assemly_InputData_StDef 	ACC_Assemly_InputData;
ACC_Assemly_OutData_StDef		ACC_Assemly_OutData;
/*输出接口*/
//typedef struct
//{
//	float 						MotorResolvingOffset_X	;	
//	float 						MotorResolvingOffset_Y	;

//}ACC_Assemly_OutputData_StDef;


//ACC_Assemly_OutputData_StDef	ACC_Assemly_OutputData;
//
/***********************************************************
** 函数名称:		ACC_Assemly_Condition_Sta
** 功能描述: 	判断是否在联动状态
** 参    数:	None
** 返 回 值:		None
***********************************************************/

uint8_t test = 1;

//判断是否在联动状态
void ACC_Assemly_Condition_Sta(void)
{
	uint8_t 			AGVAssemblySta 	= 0X00;
	uint8_t 			RCSAssemblySta 	= 0X00;
	uint8_t 			AGVRemoteWired 	= 0X00;
	uint8_t 			LinkComSta	 	= 0X00;
	AsbModel_TypeDef 	HandleUnionVal	;
	Control_MODEL		AGVControlMODEL	;
	{
		//获取单动/联动面板旋钮状态
		{
			if((g_IOInputs.Assembly.ID != ICN_Disable && g_IOInputs.Assembly.StableValue == 1))
			{
				AGVAssemblySta = 1;
			}
		}
		//获取遥控器旋钮状态
		{
			HandleUnionVal	= g_ChannelSwitch.CtrlCmd.HandleUnionVal;
		}
		//获取调度/遥控面板旋钮状态
		{
			AGVControlMODEL = g_HostData.ControlMODEL;
		}
		//获取调度系统下发的 联动控制结果
		{
			RCSAssemblySta = g_DHR_AGVInformation[118];
		}
		//获取有线/无线面板旋钮状态
		{
			if((g_IOInputs.RemoteWiredSel.ID != ICN_Disable && g_IOInputs.RemoteWiredSel.StableValue == 1))
			{
				AGVRemoteWired = 1;
			}
		}
		//获取主车广播的进入双车通讯的标志
		{
			LinkComSta = g_Asb.Comm.MasterData.CustomM.Bit.LinkCom_ChannelSwitch;
		}
		//获取主从角色
		{
			
		}
	}
	//逻辑整合处理 得到遥感来源的标志
	{
		//主车而言
		if(ACC_Assemly_InputData.Assemly_Role == 0)
		{
			if(	AGVControlMODEL == Style_Auto)
			{
				ACC_Assemly_OutData.Assemly_OutStaRocker = ACC_AssemlyOutRocker_RCS;
			}
			else
			{
				ACC_Assemly_OutData.Assemly_OutStaRocker = ACC_AssemlyOutRocker_Remote;
			}
		}
		//从车而言
		else
		{
			if(	AGVControlMODEL == Style_Auto)
			{
				//调度系统告知进入联动模式
				if(	  RCSAssemblySta == 1
					||test	!= 0X00)
				{
					ACC_Assemly_OutData.Assemly_OutStaRocker = ACC_AssemlyOutRocker_LinkCom;
				}
				//否则单动运行
				else
				{
					ACC_Assemly_OutData.Assemly_OutStaRocker = ACC_AssemlyOutRocker_RCS;
				}
			}
			else
			{
				//有线联动的情况下
				if(AGVRemoteWired == 1 && AGVAssemblySta == 1)
				{
					ACC_Assemly_OutData.Assemly_OutStaRocker = ACC_AssemlyOutRocker_LinkCom;
				}
				//主车广播了 需要强制双车通讯控制
				else if(LinkComSta==1)
				{
					ACC_Assemly_OutData.Assemly_OutStaRocker = ACC_AssemlyOutRocker_LinkCom;
				}
				//强制了遥感从双车通讯中得到
				else if(ACC_Assemly_InputData.Assemly_ConditionFun.LinkCom_Remote == 1)
				{
					ACC_Assemly_OutData.Assemly_OutStaRocker = ACC_AssemlyOutRocker_LinkCom;
				}
				//否则都是遥控器控制
				else
				{
					ACC_Assemly_OutData.Assemly_OutStaRocker = ACC_AssemlyOutRocker_Remote;
				}
			}
		}
	}
	//逻辑整合，得到纠偏遥感数据来源
	{
//		//主车而言
//		if(ACC_Assemly_InputData.Assemly_Role == 0)
//		{
//			if(	AGVControlMODEL == Style_Auto)
//			{
//				ACC_Assemly_OutData.Assemly_OutStaRocker = ACC_AssemlyOutRocker_RCS;
//			}
//			else
//			{
//				ACC_Assemly_OutData.Assemly_OutStaRocker = ACC_AssemlyOutRocker_Remote;
//			}
//		}
//		//从车而言
//		else
//		{
//			
//		}
	}

	//1、判断联动面板开关
	if( g_HostData.ControlMODEL != Style_Auto
		&& (g_IOInputs.Assembly.ID != ICN_Disable && g_IOInputs.Assembly.StableValue == 1))
	{
		ACC_Assemly_InputData.Assemly_Sta = 1;
		
	}
	//2、调度系统告诉进入联动
	else if (g_HostData.ControlMODEL == Style_Auto)
	{
		//使用双车通讯的遥感
		//if(ACC_Assemly_InputData.Assemly_Role == 1)
		{
			ACC_Assemly_InputData.Assemly_ConditionFun.LinkCom_Remote = 1;
		}
		if((g_DHR_AGVInformation[118] & 0X01) == 0X01
			||test != 0X00
			||g_IOInputs.Assembly.StableValue == 1)
		{
			ACC_Assemly_InputData.Assemly_Sta = 1;
			g_ChannelSwitch.CtrlCmd.HandleUnionVal = AsbModel_Asb;
		}
		else
		{
			ACC_Assemly_InputData.Assemly_Sta = 0;
			g_ChannelSwitch.CtrlCmd.HandleUnionVal = AsbModel_A;
		}
		
	}
	//3、仿真测试
	else if(0)
	{
		ACC_Assemly_InputData.Assemly_Sta = 1;
		g_ChannelSwitch.CtrlCmd.HandleUnionVal = AsbModel_Asb;
	}
	else
	{
		ACC_Assemly_InputData.Assemly_Sta = 0;
		ACC_Assemly_InputData.Assemly_Ctrl_Fun = ACC_AssemlyCtrlFun_Init;
	}
	//遥感控制指令转义
	{
		HandleUnionVal	= g_ChannelSwitch.CtrlCmd.HandleUnionVal;
		if(HandleUnionVal == AsbModel_A)
		{
			ACC_Assemly_InputData.Assemly_Ctrl_Fun = ACC_AssemlyCtrlFun_Main;
		}
		else if(HandleUnionVal == AsbModel_B)
		{
			ACC_Assemly_InputData.Assemly_Ctrl_Fun = ACC_AssemlyCtrlFun_SlaveONE;
		}
		else if(HandleUnionVal == AsbModel_Asb)
		{
			ACC_Assemly_InputData.Assemly_Ctrl_Fun = ACC_AssemlyCtrlFun_Linkage;
		}
		else
		{
			ACC_Assemly_InputData.Assemly_Ctrl_Fun = ACC_AssemlyCtrlFun_Init;
		}
	}
}
//
/***********************************************************
** 函数名称:	ACC_Assemly_Condition_Role
** 功能描述: 	联动主从角色判断
** 参    数:	None
** 返 回 值:	0-未确认  其他-主从角色确认完成
***********************************************************/
//联动主从角色判断
uint8_t ACC_Assemly_Condition_Role(void)
{
	uint8_t Res = 0X00;
	/*非联动模式无需判断主从*/
	if(ACC_Assemly_InputData.Assemly_Sta == 0){return 0;}
	//1、自适应结果的主从结果
	if(0)
	{
		ACC_Assemly_InputData.Assemly_Role = 0;
		
	}
	//2、屏幕输入的主从结果
	else if(0)
	{
		ACC_Assemly_InputData.Assemly_Role = 0;
	}
	//3、代码写死
	else if(1)
	{
		/*if (g_Para.EqpID == EqpID_49101
		  ||g_Para.EqpID == EqpID_49103)//1号车
		{
			ACC_Assemly_InputData.Assemly_Role = 0;
		}
		else if (g_Para.EqpID == EqpID_49102
			   ||g_Para.EqpID == EqpID_49104)
		{
			ACC_Assemly_InputData.Assemly_Role = 1;
		}*/
	}
	else
	{
		ACC_Assemly_InputData.Assemly_Role = 1;
	}
	return Res;
}
void ACC_Assemly_Condition_SN(void)
{
	/*非联动模式从自身设备编号取得车辆编号*/
	if(ACC_Assemly_InputData.Assemly_Sta == 0)
	{
		ACC_Assemly_InputData.Assemly_SN = g_Para.EqpID%4;
	}
	else
	{
		//1、自适应结果的车辆编号结果
		if(0)
		{
			ACC_Assemly_InputData.Assemly_SN = 1;
		}
		else if(0)//2、屏幕输入的车辆编号结果
		{
			ACC_Assemly_InputData.Assemly_SN = 1;
		}
		//3、代码写死
		else if(1)
		{
			/*if (g_Para.EqpID == EqpID_49101
			  ||g_Para.EqpID == EqpID_49103)//1号车
			{
				ACC_Assemly_InputData.Assemly_SN = 1;
				g_Asb.Para.Iam = AsbCarStrNo_1;
				g_Asb.Para.Role = AsbRole_Master;
				//在前的车 需要设置成 1
				g_Asb.Para.Seat = AsbSiteSeat_2;
			}
			else if (g_Para.EqpID == EqpID_49102
				   ||g_Para.EqpID == EqpID_49104)
			{
				ACC_Assemly_InputData.Assemly_SN = 2;
				g_Asb.Para.Iam = AsbCarStrNo_2;
				g_Asb.Para.Role = AsbRole_Slave;
				g_Asb.Para.Seat = AsbSiteSeat_1;
			}*/
		}
		else
		{
			ACC_Assemly_InputData.Assemly_SN = 1;
		}
	}

}

void ACC_Assemly_Condition_Scheme(void)
{
	/*非联动模式无需判断阵型*/
	if(ACC_Assemly_InputData.Assemly_Sta == 0){return;}
	//1、自适应结果的阵型结果
	if(0)
	{
		ACC_Assemly_InputData.Assemly_Scheme = ACC_Assemly_Scheme_TwoCar_Vertical;
	}
	//2、屏幕输入的阵型结果
	else if(0)
	{
		ACC_Assemly_InputData.Assemly_Scheme = ACC_Assemly_Scheme_TwoCar_Vertical_Subtend;
	}
	//3、代码写死
	else if(1)
	{
		ACC_Assemly_InputData.Assemly_Scheme = ACC_Assemly_Scheme_TwoCar_Vertical;
		g_Asb.Para.SiteNo = AsbSiteNo_FB;
	}
	else
	{
		ACC_Assemly_InputData.Assemly_Scheme = ACC_Assemly_Scheme_TwoCar_Vrosswise;
	}
}

uint8_t ACC_Assemly_Condition_Distance(void)
{
	uint8_t res = 0;
	ACC_Assemly_ConditionFun_StDef FunTemp = ACC_Assemly_InputData.Assemly_ConditionFun;
	//光斑传感器检测结果作为联动距离
	if(FunTemp.SpotDistance == 1)
	{
		ACC_Assemly_InputData.Assemly_Distance = (g_AssemlySensorData.LaserDis_L.Cal+g_AssemlySensorData.LaserDis_R.Cal)/2.0f;
	}
	//主车从遥控器（屏幕）得到的联动距离；从车从联动通讯得到的联动距离
	else if(0)
	{
		
	}
	//代码写死
	else if(1)
	{
		ACC_Assemly_InputData.Assemly_Distance = 7.0f;
	}
	else
	{
		
	}
	return res;
}

//底盘避障状态 输入 联动模块
bool ACC_Assemly_InputObs(void)
{
	AsbObsBits_TypeDef* p2AsbObsInput = &g_Asb.Input.Obs;
	/*输入自身障碍状态
	RADAR_STATE* p2ROASta = g_ROA.RadarState_Self;*/

	/*输入联动侧特殊处理后的自身障碍状态*/
	RADAR_STATE* p2ROASta = g_ROA.RadarState_Self_ForceCLR;

	p2AsbObsInput->FLaser = (AsbLaser_TypeDef)p2ROASta[0];
	p2AsbObsInput->BLaser = (AsbLaser_TypeDef)p2ROASta[1];
	p2AsbObsInput->LLaser = (AsbLaser_TypeDef)p2ROASta[2];
	p2AsbObsInput->RLaser = (AsbLaser_TypeDef)p2ROASta[3];

	return 1;
}

/***********************************************************
** 函数名称:	ACC_Assemly_InputDataPipeline
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//将外部传感器数据抽象后，输入到联动模块中

void ACC_Assemly_InputDataPipeline(void)
{
	
	//1、联动配置结果：主从角色、联动编号、联动整型、联动距离

	//2、遥控器遥感（控制联动的遥控器来的遥感数据  目标模式  自身模式）

	//3、联动偏差（联动纠偏使用 偏差xzy ）
	
	//4、自生底盘（当前模式、升降/行走挡位、实时平台高度、避障、触边、磁条偏差、底盘报警信息、）
	
	//5、联动通讯池 数据输入（其他车车辆的联动CBB模块结构体）
	
	/*联动的输入数据管道构建*/
	{
		AsbInput_TypeDef	*p_TargetData = &g_Asb.Input;
		//开关 选择
		{
			//ms
			p_TargetData->Ms 			= g_Sys_1ms_Counter;
			//面板选择联动
			p_TargetData->AsbSel_Panel 	= ACC_Assemly_InputData.Assemly_Sta;
			//遥控器单联动选择命令
			if(g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_A)
				p_TargetData->AsbSel_Cmd = AsbHcmMode_StrNo1;
			else if(g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_B)
				p_TargetData->AsbSel_Cmd = AsbHcmMode_StrNo2;
			else if(g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb)
				p_TargetData->AsbSel_Cmd = AsbHcmMode_Asb;
			else
				p_TargetData->AsbSel_Cmd = AsbHcmMode_Nnull;

			//有线控制
			p_TargetData->Wired = g_IOInputs.RemoteWiredSel.StableValue;
		}
		//传感器
		{
			//巡线传感器
			{
				AsbTrack_TypeDef *p_Track = &(p_TargetData->Track);

				p_Track->Front =  -0.1f*(g_VirtualMagnetic.MagOffset[0]);
				p_Track->Back  =   0.1f*(g_VirtualMagnetic.MagOffset[1]);
				p_Track->Left  =  -0.1f*g_VirtualMagnetic.MagOffset[2];
				p_Track->Right =   0.1f*g_VirtualMagnetic.MagOffset[3];
			}
			//联动传感器
			{
				AsbSensor_TypeDef *p_Sensor = &(p_TargetData->Sensor);
				p_Sensor->Distance_FL	= g_AssemlySensorData.LaserDis_L.Cal;
				p_Sensor->Distance_BR	= g_AssemlySensorData.LaserDis_R.Cal;
				p_Sensor->LaserSpot		= g_AssemlySensorData.SpotDis.Cal;
			}
			//从车位置
			{
				AsbPlace_TypeDef *p_Place = &(p_TargetData->Place);
				p_Place->Pos = (g_AssemlySensorData.LaserDis_L.Cal+g_AssemlySensorData.LaserDis_R.Cal)/2;
				p_Place->Inf =  g_AssemlySensorData.SpotDis.Cal;
				p_Place->Ang =(g_AssemlySensorData.LaserDis_L.Cal-g_AssemlySensorData.LaserDis_R.Cal);
			}
		}
		//命令
		{
			//原始控制命令
			AsbCmd_TypeDef		*p_OrgCmd = &g_Asb.Input.OrgCmd;
			//当遥感源头是双车通讯时
			if(ACC_Assemly_OutData.Assemly_OutStaRocker == ACC_AssemlyOutRocker_LinkCom)
			{
				AsbDataM2S_TypeDef 		*p_CustomM	= &(g_Asb.Comm.MasterData.CustomM);
				//模式
				p_OrgCmd->Mode = p_CustomM->Mode;
				//摇杆命令
				p_OrgCmd->Vxyz.x = p_CustomM->Vx;
				p_OrgCmd->Vxyz.y = p_CustomM->Vy;
				p_OrgCmd->Vxyz.z = p_CustomM->Vz;
				//挡位
				p_OrgCmd->WalkGear = p_CustomM->Bit.WalkGear;
				p_OrgCmd->LiftGear = p_CustomM->Bit.LiftGear;
				//按钮命令
				p_OrgCmd->Bit.LiftUp	= p_CustomM->Bit.LiftUp;
				p_OrgCmd->Bit.LiftDown	= p_CustomM->Bit.LiftDown;
			}
			//遥感源头来自调度系统或者遥控器
			else
			{
				OrgCtrlCmd_TypeDef	*p_CtrlCmdCmd = &g_ChannelSwitch.CtrlCmd;
				//模式
				p_OrgCmd->Mode = (AsbMode_TypeDef)g_ChannelSwitch.CtrlCmd.Mode;
				//摇杆命令
				p_OrgCmd->Vxyz.x = p_CtrlCmdCmd->Vxyz.x;
				p_OrgCmd->Vxyz.y = p_CtrlCmdCmd->Vxyz.y;
				p_OrgCmd->Vxyz.z = p_CtrlCmdCmd->Vxyz.z;

				//行走挡位Z[1,6]
				p_OrgCmd->WalkGear = p_CtrlCmdCmd->WalkGear;
				//升降挡位Z[1,6]
				p_OrgCmd->LiftGear = p_CtrlCmdCmd->LiftGear;
				//上升命令
				p_OrgCmd->Bit.LiftUp = p_CtrlCmdCmd->Button.LiftUp;
				//下降命令
				p_OrgCmd->Bit.LiftDown = p_CtrlCmdCmd->Button.LiftDown;
			}

			//压轴控制命令 未合并其它车的
			
			//实际执行的控制命令
			
		}
		//状态
		{
			//实际模式
			p_TargetData->ModeReal = (AsbMode_TypeDef)AccData.ModeSwitchSMT;
			//升降高度
			p_TargetData->Hg = g_LiftData.Height.QuadrantAvg[0];
			//单车解算结果
			if (AccData.ChassisType == ACC_ChassisType_Turtle)//麦轮
			{
				p_TargetData->WheelRMax = g_MecanumResolving.RotUnif.LxyMax.Self;
				p_TargetData->WheelVMax = g_MecanumResolving.SpeedZoom.SpeedMax.Self;
			}
			else
			{
				p_TargetData->WheelRMax = g_MotorResolving.RdMax.RdMax_Self;
				p_TargetData->WheelVMax = g_MotorResolving.WheelSpeedObj.WheelSpeedMax_Self;
			}
			
			//避障状态
			ACC_Assemly_InputObs();
			
			//状态位
			{
				//运动中
				if(CheckWalk(&AccData))
				{
					p_TargetData->Bit.Moving = 1;
				}
				else
				{
					p_TargetData->Bit.Moving = 0;
				}
				//上降限位触发
				if(g_LiftData.Height.Avg >= g_LiftData.CtrlPara.LimitUp)
				{
					p_TargetData->Bit.UpLimited = 1;
				}
				else
				{
					p_TargetData->Bit.UpLimited = 0;
				}
				//下降限位触发
				if(g_LiftData.Height.Avg <= g_LiftData.CtrlPara.LimitDown)
				{
					p_TargetData->Bit.DownLimited = 1;
				}
				else
				{
					p_TargetData->Bit.DownLimited = 0;
				}
			}
			//自身故障状态
			{
				//故障类型
				p_TargetData->OrgErr.Faults = (Asb_FaultsType_TypeDef)g_Faults.Output.Self.Faults;
				//限制上升
				p_TargetData->OrgErr.Bit.UpLimited = g_Faults.Output.Self.Bit.UpLimited;
				//限制下降
				p_TargetData->OrgErr.Bit.DownLimited = g_Faults.Output.Self.Bit.DownLimited;
				//限制行走
				p_TargetData->OrgErr.Bit.MoveLimited = g_Faults.Output.Self.Bit.MoveLimited;
//				if(g_Faults.Remind.Class.MoveLimit.All != 0X00)
//				{
//					p_TargetData->OrgErr.Bit.MoveLimited = 1;
//				}
//				else
//				{
//					p_TargetData->OrgErr.Bit.MoveLimited = 0;
//				}
				
			}
			//实际执行的故障状态
			//p_TargetData->

		}
		//新增主车向从车广播s16*4的信息
		{
			if(ACC_Assemly_InputData.Assemly_Role == 0)
			{
				AsbDataM2S_TypeDef 		*p_CustomM = &p_TargetData->CustomM;
				OrgCtrlCmd_TypeDef		*p_OrgCtrlCmd = &(g_ChannelSwitch.CtrlCmd);
				//主车对从车共享的遥感值共享
				{
					ACC_Joystick_TypeDef	*p_Vxyz = &p_OrgCtrlCmd->Vxyz;
					//虽然 g_ChannelSwitch.CtrlCmd.Vxyz是float类型，但是实际上是从遥控的s16 XAxisOutput得到的赋值
					//因此这里将float装进s16不会造成精度损失——master	
					p_CustomM->Vx = p_Vxyz->x;
					p_CustomM->Vy = p_Vxyz->y;
					p_CustomM->Vz = p_Vxyz->z;
				}
				//模式
				p_CustomM->Mode = (AsbMode_TypeDef)p_OrgCtrlCmd->Mode;
				//挡位
				p_CustomM->Bit.WalkGear = p_OrgCtrlCmd->WalkGear;
				p_CustomM->Bit.LiftGear = p_OrgCtrlCmd->LiftGear;
				//按钮
				{
					//上升命令
					p_CustomM->Bit.LiftUp = p_OrgCtrlCmd->Button.LiftUp;
					//下降命令
					p_CustomM->Bit.LiftDown = p_OrgCtrlCmd->Button.LiftDown;
				}
				//任务完成情况
				{
					if(g_HostData.ExecutionPathState == ExePathState_NoTask)
					{
						//p_CustomM->CurrentPointNum = 0XFF;
					}
					else
					{
						//p_CustomM->CurrentPointNum = g_HostData.PathCurrentPointNum;
					}
				}
				//是否强制进入双车通讯
				{
					if(g_GroundJustify.Output.BeginFlag == 1)
					{
						p_CustomM->Bit.LinkCom_ChannelSwitch = 1;
					}
					else
					{
						p_CustomM->Bit.LinkCom_ChannelSwitch = 0;
					}
				}
			}
			
		}

		/*主车广播 之 避障切区选择*/
		g_Asb.Input.CustomM.ObsAreaSel = (u16)g_ChannelSwitch.CtrlCmd.Button.ObsArea;
		
		//主车广播 之 激光避障使能信息
		g_Asb.Input.CustomM.Bit.RadarObsEnF = g_ChannelSwitch.CtrlCmd.RadarEn.Front;
		g_Asb.Input.CustomM.Bit.RadarObsEnB = g_ChannelSwitch.CtrlCmd.RadarEn.Back;
		g_Asb.Input.CustomM.Bit.RadarObsEnL = g_ChannelSwitch.CtrlCmd.RadarEn.Left;
		g_Asb.Input.CustomM.Bit.RadarObsEnR = g_ChannelSwitch.CtrlCmd.RadarEn.Right;

		//新增从车向主车传递s16*4的信息
		{
			AsbDataS2M_TypeDef 		*p_CustomS = &p_TargetData->CustomS;
			//PGV 定位数据
			{
				PGV_Read_StDef *p_PGV = &g_GroundPGV.PGV_Read;
				if(p_PGV->Status == 1)
				{
					p_CustomS->S16Data[0] = p_CustomS->S16Data[0]|0x01;
					p_CustomS->S16Data[1] = p_PGV->X;
					p_CustomS->S16Data[2] = p_PGV->Y;
					
				}
				else
				{
					p_CustomS->S16Data[0] = p_CustomS->S16Data[0]&0xFE;
					p_CustomS->S16Data[1] = 0;
					p_CustomS->S16Data[2] = 0;
				}
				
			}
			//SLAM 姿态数据
			{
				ROKIT_Read_StDef *p_SLAM = &g_ROKITData.ROKIT_Read;
				if(p_SLAM->LocationSta == ROKIT_LocationStae_HighReliability)
				{
					p_CustomS->S16Data[0] = p_CustomS->S16Data[0]|0x02;
					p_CustomS->S16Data[3] = p_SLAM->LocationZ;
				}
				else
				{
					p_CustomS->S16Data[0] = p_CustomS->S16Data[0]&0xFD;
					p_CustomS->S16Data[3] = 0;
				}
			}
			
		}
		//检验对象
		{
			//本地校验参数
			{
				//平滑参数
				p_TargetData->CheckData.VxAcc = g_SmoothVx.in_Acc;
				p_TargetData->CheckData.VyAcc = g_SmoothVy.in_Acc;
				p_TargetData->CheckData.VzAcc = g_SmoothVz.in_Acc;
				p_TargetData->CheckData.VxDec = g_SmoothVx.in_Dec;
				p_TargetData->CheckData.VyDec = g_SmoothVy.in_Dec;
				p_TargetData->CheckData.VzDec = g_SmoothVz.in_Dec;
				p_TargetData->CheckData.AxAcc = g_SmoothAx.in_Acc;
				p_TargetData->CheckData.AyAcc = g_SmoothAy.in_Acc;
				p_TargetData->CheckData.AzAcc = g_SmoothAz.in_Acc;
				p_TargetData->CheckData.AxDec = g_SmoothAx.in_Dec;
				p_TargetData->CheckData.AyDec = g_SmoothAy.in_Dec;
				p_TargetData->CheckData.AzDec = g_SmoothAz.in_Dec;

				//行走解算参数 实时值校验 而不是g_Para
				p_TargetData->CheckData.MaxSliAngPos = g_MotorResolving.Para.MaxSliAngPos;
				p_TargetData->CheckData.MaxSliAngInf = g_MotorResolving.Para.MaxSliAngInf;
				p_TargetData->CheckData.MaxRotDisPos = g_MotorResolving.Para.MaxRotDisPos;
				p_TargetData->CheckData.MinRotDisPos = g_MotorResolving.Para.MinRotDisPos;
				
				/*p_TargetData->CheckData.MaxSliAngPos = g_Para.Walk_Asb.MaxSliAngPos;
				p_TargetData->CheckData.MaxSliAngInf = g_Para.Walk_Asb.MaxSliAngInf;
				p_TargetData->CheckData.MaxRotDisPos = g_Para.Walk_Asb.MinRotDisPos;
				p_TargetData->CheckData.MinRotDisPos = g_Para.Walk_Asb.MinRotDisInf;*/
				//遥控器选择
				//p_TargetData->CheckData.RmcId = ?;

				//避障使能 不用检验了，主车直接广播接管从车
				//p_TargetData->CheckData.ObsEn.LaserF = ?;
				
				//Bit
				p_TargetData->CheckData.Bit.AsbSel = p_TargetData->AsbSel_Panel;
			}
			//校验结果
		}
	}

}

//障碍状态更新
bool ACC_Assemly_ObsUpdate(void)
{
	AsbObsBits_TypeDef* p2AsbObsOutput = &g_Asb.Output.Obs;
	RADAR_STATE* p2ROASta = g_ROA.RadarState_Asb;

	p2ROASta[0] = (RADAR_STATE)(p2AsbObsOutput->FLaser);
	p2ROASta[1] = (RADAR_STATE)(p2AsbObsOutput->BLaser);
	p2ROASta[2] = (RADAR_STATE)(p2AsbObsOutput->LLaser);
	p2ROASta[3] = (RADAR_STATE)(p2AsbObsOutput->RLaser);

	return 1;
}

/*各车根据不同联动位置 自定义关闭部分方向避障*/
bool ACC_Assemly_ObsEnCustom(void)
{
	AsbSiteNo_TypeDef Site = g_Asb.Para.SiteNo;
	AsbSiteSeat_TypeDef Seat = g_Asb.Para.Seat;

	switch (Site)
	{
	//前后一顺
	case AsbSiteNo_FB:
		if(Seat == AsbSiteSeat_1)
		{
			/*关闭前车的后侧*/
			AccData.OrgCmd.RadarEn.Back = 0;
		}
		else if(Seat == AsbSiteSeat_2)
		{
			/*关闭后车的后侧*/
			AccData.OrgCmd.RadarEn.Front = 0;
		}
		else {}
		break;

	//前后头对头 待后面实现
	case AsbSiteNo_FB1:
		break;
	
	default:
		break;
	}

	return 1;
}

/*各车根据不同联动位置 自定义清除联动车障碍状态 参数更新*/
bool ACC_Assemly_ObsCLRCustom(void)
{
	AsbSiteNo_TypeDef Site = g_Asb.Para.SiteNo;
	AsbSiteSeat_TypeDef Seat = g_Asb.Para.Seat;

	//先全部打开
	g_ROA.ForceCLR[Radar_Front] = State_Error;
	g_ROA.ForceCLR[Radar_Back] = State_Error;
	g_ROA.ForceCLR[Radar_Left] = State_Error;
	g_ROA.ForceCLR[Radar_Right] = State_Error;

	switch (Site)
	{
		//前后一顺
	case AsbSiteNo_FB:
		if (Seat == AsbSiteSeat_1)
		{
			/*关闭前车的后侧*/
			g_ROA.ForceCLR[Radar_Back] = State_Nomal;
		}
		else if (Seat == AsbSiteSeat_2)
		{
			/*关闭后车的前侧*/
			g_ROA.ForceCLR[Radar_Front] = State_Nomal;
		}
		else {}
	break;

		//前后头对头 待后面实现
	case AsbSiteNo_FB1:
	break;

	default:
	break;
	}

	return 1;
}

/*各车根据不同联动位置 自定义避障切区*/
bool ACC_Assemly_ObsAreaCustom(void)
{
	AsbSiteNo_TypeDef Site = g_Asb.Para.SiteNo;
	AsbSiteSeat_TypeDef Seat = g_Asb.Para.Seat;

	switch (Site)
	{
		//前后一顺
	case AsbSiteNo_FB:
		if (Seat == AsbSiteSeat_1)
		{
			/*前车
			AccData.OrgCmd.Button.ObsArea = xxx;*/
		}
		else if (Seat == AsbSiteSeat_2)
		{
			/*后车
			AccData.OrgCmd.Button.ObsArea = xxx;*/
		}
		else {}
		break;

		//前后头对头 待后面实现
	case AsbSiteNo_FB1:
		break;

	default:
		break;
	}

	return 1;
}


/***********************************************************
** 函数名称:	ACC_Assemly_OutputDataPipeline
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//用联动模块的输出，接管底盘的过程
void ACC_Assemly_OutputDataPipeline(void)
{
	//1、处理后的遥控器数据（模式切换、遥感、速度挡位、升降按钮、避障开关）
	
	//2、联动报警信息
	
	//3、底盘配置参数（运动中心设置、称重报警阈值、）
	
	//4、将 自生的联动CBB模块结构体 填充至 联动通讯数据池
	
	{
		//AsbOutput_TypeDef	*p_SourceData = &g_Asb.Output;
		//模式
		{
			// = (ACCMode_TypeDef)p_SourceData->AsbMode;
		}
		//联动故障枚举 在底盘故障检出中处理

		//汇总执行的故障结果 在底盘故障检出中处理

		//避障状态 输入到避障模块
		ACC_Assemly_ObsUpdate();

		//控制命令
		{
			//原始控制命令
			OrgCtrlCmd_TypeDef	*p_OrgCmd = &AccData.OrgCmd;

			AsbCmd_TypeDef		*p_Cmd = &g_Asb.Output.Cmd;
			//模式
			p_OrgCmd->Mode = (ACCMode_TypeDef)p_Cmd->Mode;
			//摇杆命令
			p_OrgCmd->Vxyz.x = p_Cmd->Vxyz.x;
			p_OrgCmd->Vxyz.y = p_Cmd->Vxyz.y;
			p_OrgCmd->Vxyz.z = p_Cmd->Vxyz.z;

			//行走挡位Z[1,6]
			p_OrgCmd->WalkGear = p_Cmd->WalkGear;
			//升降挡位Z[1,6]
			p_OrgCmd->LiftGear = p_Cmd->LiftGear;
			//上升命令
			p_OrgCmd->Button.LiftUp = p_Cmd->Bit.LiftUp;
			//下降命令
			p_OrgCmd->Button.LiftDown = p_Cmd->Bit.LiftDown;
		}
		
		//动作执行使能 取消设计
		
		//纠正量 行走：硬联动不纠；升降：通过操作CBB速度档位实现
		
		//巡线传感器
		
		//单车联动时的圆心坐标偏移向量坐标
		
		//解算缩放参数 在解算CBB调用时处理
		
		//激光避障使能 从主车的通信广播中获取
		AccData.OrgCmd.RadarEn.Front = g_Asb.Comm.MasterData.CustomM.Bit.RadarObsEnF;
		AccData.OrgCmd.RadarEn.Back = g_Asb.Comm.MasterData.CustomM.Bit.RadarObsEnB;
		AccData.OrgCmd.RadarEn.Left = g_Asb.Comm.MasterData.CustomM.Bit.RadarObsEnL;
		AccData.OrgCmd.RadarEn.Right = g_Asb.Comm.MasterData.CustomM.Bit.RadarObsEnR;

		/*各车根据不同联动位置 自定义关闭部分方向避障 直接关使能行不通，改为了避障模块内部关障碍状态
		ACC_Assemly_ObsEnCustom();*/
		
		/*避障切区 从主车的通信广播中获取*/
		AccData.OrgCmd.Button.ObsArea = (AccObsArea_TypeDef)g_Asb.Comm.MasterData.CustomM.ObsAreaSel;

		/*如从车需要在特殊位置切换到特殊避障区域，在此处添加逻辑*/
		ACC_Assemly_ObsAreaCustom();
	}
}

/***********************************************************
** 函数名称:	ACC_Assemly_DataInit
** 功能描述: 	联动结构体初始化
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void ACC_Assemly_DataInit(void)
{
	//ACC_Assemly_InputData_StDef *p_InitData = &ACC_Assemly_InputData;

	ACC_Assemly_InputData.Assemly_ConditionFun.SpotDistance = 1;

	//ACC_Assemly_InputData.Assemly_ConditionFun.LinkCom_Remote = 1;
	
	ACC_Assemly_InputData.Assemly_ConditionFun.FissionLineRrace = 0;

	g_Asb.Para.FuncSwitch.TrackOrg = 1;

	if(EQPMsts.Assembly_Master.CommId != CCFCN_Disable)
	{
		//p_InitData->Assemly_Master_Slave_Cn = EQPMsts.Assembly_Master.CommId;
	}
	else if(EQPMsts.Assembly_Master.CommId != CCFCN_Disable)
	{
		//p_InitData->Assemly_Master_Slave_Cn = EQPSlvs.Assembly_Slave.CommId;
	}
	
	
	{
		AsbInput_TypeDef	*p_TargetData = &g_Asb.Input;
		//平滑参数
		p_TargetData->CheckData.VxAcc = g_Para.Walk_Asb.Smooth.VxAcc;
		p_TargetData->CheckData.VyAcc = g_Para.Walk_Asb.Smooth.VyAcc;
		p_TargetData->CheckData.VzAcc = g_Para.Walk_Asb.Smooth.VzAcc;
		p_TargetData->CheckData.VxDec = g_Para.Walk_Asb.Smooth.VxDec;
		p_TargetData->CheckData.VyDec = g_Para.Walk_Asb.Smooth.VyDec;
		p_TargetData->CheckData.VzDec = g_Para.Walk_Asb.Smooth.VzDec;
		p_TargetData->CheckData.AxAcc = g_Para.Walk_Asb.Smooth.AxAcc;
		p_TargetData->CheckData.AyAcc = g_Para.Walk_Asb.Smooth.AyAcc;
		p_TargetData->CheckData.AzAcc = g_Para.Walk_Asb.Smooth.AzAcc;
		p_TargetData->CheckData.AxDec = g_Para.Walk_Asb.Smooth.AxDec;
		p_TargetData->CheckData.AyDec = g_Para.Walk_Asb.Smooth.AyDec;
		p_TargetData->CheckData.AzDec = g_Para.Walk_Asb.Smooth.AzDec;

		//行走解算参数
		p_TargetData->CheckData.MaxSliAngPos = g_Para.Walk_Asb.MaxSliAngPos;
		p_TargetData->CheckData.MaxSliAngInf = g_Para.Walk_Asb.MaxSliAngInf;
		p_TargetData->CheckData.MaxRotDisPos = g_Para.Walk_Asb.MinRotDisPos;
		p_TargetData->CheckData.MinRotDisPos = g_Para.Walk_Asb.MinRotDisInf;
	}
}
/***********************************************************
** 函数名称:		ACC_Assemly_MotorResolvingSet
** 功能描述: 	轮系运动中心在单车遥控的基础上偏移指定距离，传入0时，恢复单车遥控
** 参    数:	Offset_X：横移方向偏移 单位1mm Offset_Y：前进方向偏移 单位1mm
** 返 回 值:		None
***********************************************************/
//轮系运动中心在单车遥控的基础上偏移指定距离，传入0时，恢复单车遥控
void ACC_Assemly_MotorResolvingSet(float Offset_X,float Offset_Y)
{
	MotorResolving_TypeDef			*p_Data = &g_MotorResolving;

	uint8_t ModuleNumber = p_Data->Para.ModuleNumber;
	uint8_t i = 0;

	for(i=0;i<ModuleNumber;i++)
	{
		p_Data->Para.Mxy[i].X_Axis = g_Para.Walk.Mxy[i].x + Offset_X;
		p_Data->Para.Mxy[i].Y_Axis = g_Para.Walk.Mxy[i].y + Offset_Y;
	}
}

/***********************************************************
** 函数名称:		ACC_Assemly_EQP_Set
** 功能描述: 	动态设置本机的EQP主从通讯配置
** 参    数:	None
** 返 回 值:		None
***********************************************************/
//
void ACC_Assemly_EQP_Set(CCFID_EnumDef MasterCn ,CCFID_EnumDef SlaveCn)
{
	Para_EQPInit(Para_EQP_Assembly_Slave,	MasterCn,	0X0A);
	Para_EQPInit(Para_EQP_Assembly_Master,	SlaveCn,	0X0A);

	/*Modbus协议栈状态机 设置成复位状态*/
	mb_manger_app.mb_step = 0X04;
}
/**/
/***********************************************************
** 函数名称:	mb_ResetInit_CallBack
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//重写modbus底层协议栈的动态初始化函数
void mb_ResetInit_CallBack(void)
{
	//刷新EQP设置
	EQPComm_Init();
	//EQP和CCF和数据打通
	CommConf_Init();

	//modbus协议栈复位
	memset(&mb_manger_app,0,sizeof(MB_MANGER_APP));	
}
/***********************************************************
** 函数名称:	ACC_AssemlyCtrl_OutPut
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void ACC_AssemlyCtrl_OutPut(void)
{
	ACC_Assemly_OutData_StDef	*pAssemlyOutData = &ACC_Assemly_OutData;
	//外部访问的是否在联动控制中的状态
	{
		if(	  ACC_Assemly_InputData.Assemly_Ctrl_Fun == ACC_AssemlyCtrlFun_Linkage
			&&ACC_Assemly_InputData.Assemly_Sta == 1)
		{
			pAssemlyOutData->Assemly_OutSta = ACC_AssemlyOutSta_Linkage;
		}
		else
		{
			pAssemlyOutData->Assemly_OutSta = ACC_AssemlyOutSta_Solo;
		}
	}
	//外部访问的PGV传感器数据
	{
		ACC_Assemly_OutPGV_StDef *p_PGV = &(pAssemlyOutData->Assemly_OutPGV);
		int16_t					*p_Sour = g_Asb.Comm.SlaveData[1].CustomS.S16Data;
		if((p_Sour[0]&0x01) == 0x01)
		{
			p_PGV->Status 	= 1;
			p_PGV->X 		= p_Sour[1];
			p_PGV->Y 		= p_Sour[2];
		}
		else
		{
			p_PGV->Status 	= 0;
			p_PGV->X 		= 0;
			p_PGV->Y 		= 0;
		}
		
	}
	//外部访问的偏差数据
	{
		ACC_Assemly_OutTrack_StDef	*p_Track 	= &(pAssemlyOutData->Assemly_OutTrack);
		AsbTrack_TypeDef			*p_Sour 	= &(g_Asb.Output.Track);
		//如果使能了分体巡线，所有车都使用自己的巡线偏差
		if(ACC_Assemly_InputData.Assemly_ConditionFun.FissionLineRrace == 1)
		{
			p_Sour 	= &(g_Asb.Input.Track);
		}
		else
		{
			p_Sour 	= &(g_Asb.Output.Track);
		}
		p_Track->Front 	= p_Sour->Front;
		p_Track->Back 	= p_Sour->Back;
		p_Track->Left 	= p_Sour->Left;
		p_Track->Right 	= p_Sour->Right;
	}
	
}
/***********************************************************
** 函数名称:		ACC_AssemlyCtrl
** 功能描述:		
** 参    数:	None
** 返 回 值:		None
***********************************************************/
uint8_t 	AssemlyCtrlStep = 0;
void ACC_AssemlyCtrl(void)
{
	
	//0、模块初始化
	//1、等待系统启动成功

	//2、单动执行，检测是否触发进入联动模式
	//3、联动进入的初始化设置
	//4、检测是否设置完成
	     //5、联动功能执行，检测是否触发退出联动模式
	//6、联动退出的设置
	//7、检测是否完成设置
	//8、回到0状态

	//static uint8_t 	AssemlyCtrlStep = 0;
	//static uint64_t AssemlyCtrlMs	= 0;
	//判断联动相关条件
	ACC_Assemly_Condition_Sta();
	
	//A、联动CBB模块 需要的传感器输入管道创建（AccData的逻辑输入）
	if (AtDbug.Bit.AsbInputOff == 0)//调试时可强制输入不调用进行测试
		ACC_Assemly_InputDataPipeline();
	else {}

	//B、联动CBB调用
	Asb_Main(&g_Asb);

	//状态机
	{
		switch(AssemlyCtrlStep)
		{
			default:break;
			/*模块初始化*/
			case 0:
			{
				/*模块初始化*/
				ACC_Assemly_DataInit();
				AssemlyCtrlStep = 1;
			}break;
			/*等待系统启动成功*/
			case 1:
			{
				if(g_MC_WorkStep==MC_WorkStep_Run)
				{
					AssemlyCtrlStep = 2;
				}
			}break;
			/*单动执行，检测是否触发进入联动模式*/
			case 2:
			{
				if(ACC_Assemly_InputData.Assemly_Sta == 0)
				{
					//单动执行代码
				}
				else
				{
					AssemlyCtrlStep = 3;
				}
			}break;
			/*联动进入的初始化设置*/
			case 3:
			{
				//打开一字激光
				AccData.IOCtrl.LandscapeLaser = 1;
				//判断是否在自身角色 主车or从车
				ACC_Assemly_Condition_Role();
				//逻辑确定自身的联动编号，如果是单车项目或者处于单动状态就将车辆编号作为联动编号；
				ACC_Assemly_Condition_SN();
				//判断是否联动车型
				ACC_Assemly_Condition_Scheme();
				//设置联动距离
				//ACC_Assemly_Condition_Distance();
				//将从车遥控器接收盒信道号 切换至主车遥控器编号
				/*{
					if (g_Para.EqpID == EqpID_49101)
					{
						
					}
					else if (g_Para.EqpID == EqpID_49102)
					{
						EQP_WHA.WD[0] = 10778;    //无线地址;		
						EQP_WHA.WD[1] = 56;   //信道号;
					}
					else if (g_Para.EqpID == EqpID_49103)
					{

					}
					else if (g_Para.EqpID == EqpID_49104)
					{
						EQP_WHA.WD[0] = 10780;		
						EQP_WHA.WD[1] = 66;
					}
				}*/
				
				
				//进入条件都设置了
				if(1)
				{
					
					//设置联动的主从通讯
					{
						if(ACC_Assemly_InputData.Assemly_Role == 0)
						{
							/*主车*/
							//ACC_Assemly_EQP_Set(ACC_Assemly_InputData.Assemly_Master_Slave_Cn,CCFCN_Disable);
						}
						else
						{
							/*从车*/
							//ACC_Assemly_EQP_Set(CCFCN_Disable,ACC_Assemly_InputData.Assemly_Master_Slave_Cn);
						}
					}
					AssemlyCtrlStep = 4;
				}
				//直接退出联动
				else if(ACC_Assemly_InputData.Assemly_Sta == 0)
				{
					AssemlyCtrlStep = 6;
				}

			}break;
			/*检测是否设置完成*/
			case 4:
			{
				//联动通讯建立完成
				if(   1
					&&ACC_Assemly_InputData.Assemly_Ctrl_Fun == ACC_AssemlyCtrlFun_Linkage)
				{
					//设置联动距离
					ACC_Assemly_Condition_Distance();
					//主车关闭一字激光
					if(ACC_Assemly_InputData.Assemly_Role == 0)
					{
						AccData.IOCtrl.LandscapeLaser = 0;
					}
					AssemlyCtrlStep = 5;
				}
				//面板旋到了单动 直接退出联动
				else if(ACC_Assemly_InputData.Assemly_Sta == 0)
				{
					AssemlyCtrlStep = 6;
				}
			}break;
			/*联动功能执行，检测是否触发退出联动模式*/
			case 5:
			{
				if(ACC_Assemly_InputData.Assemly_Sta == 1
					&&ACC_Assemly_InputData.Assemly_Ctrl_Fun == ACC_AssemlyCtrlFun_Linkage)
				{
					//A、联动CBB模块 需要的传感器输入管道创建（AccData的逻辑输入）
					//ACC_Assemly_InputDataPipeline();
					//B、联动CBB调用
					
					//C、联动CBB的输出（接管遥感）
					//ACC_Assemly_OutputDataPipeline();
				}
				else if(ACC_Assemly_InputData.Assemly_Ctrl_Fun != ACC_AssemlyCtrlFun_Linkage)
				{
					AssemlyCtrlStep = 4;
				}
				else
				{
					AssemlyCtrlStep = 6;
				}
				
			}break;
			/*联动退出的设置*/
			case 6:
			{
				//复位联动通讯
				{
					//ACC_Assemly_EQP_Set(CCFCN_Disable,CCFCN_Disable);
				}
				//关闭一字激光
				{
					AccData.IOCtrl.LandscapeLaser = 0;
				}
				//
				//将遥控器接收盒信道号 切换至自生的默认
				{
					/*if (g_Para.EqpID == EqpID_49101)
					{
						
					}
					else if (g_Para.EqpID == EqpID_49102)
					{
						EQP_WHA.WD[0] = 10779;	
						EQP_WHA.WD[1] = 61;
					}
					else if (g_Para.EqpID == EqpID_49103)
					{

					}
					else if (g_Para.EqpID == EqpID_49104)
					{
						EQP_WHA.WD[0] = 10781;
						EQP_WHA.WD[1] = 71;
					}*/
				}

				AssemlyCtrlStep = 7;
			}break;
			/*检测是否完成设置*/
			case 7:
			{
				if(1)
				{
					AssemlyCtrlStep = 8;
				}
			}break;
			/*回到0状态*/
			case 8:
			{
				AssemlyCtrlStep = 0;
			}break;

		}
	}
	//实际对底盘的控制
	{
		static uint8_t MotorResolving_Init_Flag 	= 0xEE;

		//自动单动 直接响应旁路遥感
		if(ACC_Assemly_OutData.Assemly_OutStaRocker == ACC_AssemlyOutRocker_RCS)
		{
			MotorResolving_Init_Flag = 0x00;
		}
		//一起动 响应旁路遥感
		else if(ACC_Assemly_InputData.Assemly_Ctrl_Fun == ACC_AssemlyCtrlFun_Together)
		{
			MotorResolving_Init_Flag = 0x00;
		}
		//联动中，在控制此编号车单动 响应旁路遥感
		else if(ACC_Assemly_InputData.Assemly_Role+3 == ACC_Assemly_InputData.Assemly_Ctrl_Fun)
		{
			MotorResolving_Init_Flag = 0x00;
		}
		//整体联动 响应联动模块输出
		else if(ACC_Assemly_InputData.Assemly_Ctrl_Fun == ACC_AssemlyCtrlFun_Linkage)
		{
			MotorResolving_Init_Flag = 0x01;
		}
		//其他情况
		else
		{
			MotorResolving_Init_Flag = 0xAA;
		}

		//根据结果 使用不同的
		{
			if(MotorResolving_Init_Flag == 0x00)
			{
				AccData.OrgCmd = g_ChannelSwitch.CtrlCmd;
			}
			else if(MotorResolving_Init_Flag == 0x01)
			{
				//联动CBB的输出（接管遥感）
				ACC_Assemly_OutputDataPipeline();
			}
			else
			{
				g_ChannelSwitch.CtrlCmd.Mode = ACCMode_Still;
				g_ChannelSwitch.CtrlCmd.Vxyz.x = 0;
				g_ChannelSwitch.CtrlCmd.Vxyz.y = 0;
				g_ChannelSwitch.CtrlCmd.Vxyz.z = 0;
				AccData.OrgCmd = g_ChannelSwitch.CtrlCmd;
			}
		}
	}
	//外部需要同步的内容
	ACC_AssemlyCtrl_OutPut();

}

//联动通信 切换到modbus主站
bool AsbSwitchEnterInit_MB2Master(void)
{
	//开启主站
	Para_EQPInit(Para_EQP_Assembly_Master,g_Para.Asb.CN_Master,0X0A);
	//关闭从站
	Para_EQPInit(Para_EQP_Assembly_Slave,CCFCN_Disable,0X0A);
	/*Modbus协议栈状态机 设置成复位状态*/
	mb_manger_app.mb_step = 0X04;
	return 1;
}

//联动通信 切换到modbus从站
bool AsbSwitchEnterInit_MB2Slave(void)
{
	//关闭主站
	Para_EQPInit(Para_EQP_Assembly_Master, CCFCN_Disable, 0X0A);
	//开启从站
	Para_EQPInit(Para_EQP_Assembly_Slave, g_Para.Asb.CN_Slave, 0X0A);
	/*Modbus协议栈状态机 设置成复位状态*/
	mb_manger_app.mb_step = 0X04;
	return 1;
}

//覆盖联动模块内部的单/动联动模式切换进入前的底盘初始化函数
//该函数需要能访问底盘舵轮解算cbb的对象实例
//联动通信切换信号量
static bool Semaphore = false;

bool AsbSwitchEnterInit(void)
{
	if(g_Asb.P2Obj.p2Self->AsbSwitch.Cmd == AsbAsbMode_Asb)
	{
		if(g_Asb.Para.Role == AsbRole_Master)//联动选择主车
		{
			//特殊联动运动中心配置
			g_Para.Walk_Asb.Bit.FB_Axis_Forward = 0;
			g_Para.Walk_Asb.Bit.FB_Axis_Moon 	= 1;
			g_Para.Walk_Asb.Bit.LR_Axis_Fixed 	= 0;
			g_Para.Walk_Asb.Bit.LR_Axis_Moon 	= 1;
			//联动cbb初始化
			g_Asb.Para.Iam = AsbCarStrNo_1;
			g_Asb.Para.Seat = AsbSiteSeat_2;//主车在后面
			//特殊运动中心处理
			g_Para.p2Fucs.p2PrjExp_MR_Init_Asb_Axis();

			/*联动模式下，初始化轮系运动中心*/
			MotorResolving_Init_Asb(&g_MotorResolving, g_Asb.Output.Center_Cal.X, g_Asb.Output.Center_Cal.Y);//舵轮
			MecanumResolving_Init_Asb(&g_MecanumResolving, g_Asb.Output.Center.X, g_Asb.Output.Center.Y);//麦轮
			
			/*选择联动避障源*/
			g_ROA.Sel = ObsSel_Asb;

			/*各车根据不同联动位置 自定义清除联动车障碍状态 参数更新*/
			ACC_Assemly_ObsCLRCustom();

			/*切换联动通信为主站*/
			//联动下，针对主从车进行一次联动通信切换，联动切换信号量置false
			if(Semaphore == false)
			{
				AsbSwitchEnterInit_MB2Master();
				Semaphore = true;
			}

			/*阵型位置，根据阵型切换到固定位置
			超低车固定去前面主车、右边主车，是为了让超低车的主车适应特殊的运动中心变换，
			即从车的运动中心不依赖联动距离、避免联动距离通过联动通信过程，
			其他车型如果不这样，需将下方代码注释，
			*/
//			if (g_Asb.Para.SiteNo == AsbSiteNo_FB)
//				g_Asb.Para.Seat = AsbSiteSeat_1;
//			else if (g_Asb.Para.SiteNo == AsbSiteNo_FB1)
//				g_Asb.Para.Seat = AsbSiteSeat_1;
//			else if (g_Asb.Para.SiteNo == AsbSiteNo_LR)
//				g_Asb.Para.Seat = AsbSiteSeat_2;
//			else {}
		}
		else if(g_Asb.Para.Role == AsbRole_Slave)//联动选择从车
		{
			//特殊联动运动中心配置
			g_Para.Walk_Asb.Bit.FB_Axis_Forward = 1;
			g_Para.Walk_Asb.Bit.FB_Axis_Moon 	= 0;
			g_Para.Walk_Asb.Bit.LR_Axis_Fixed 	= 1;
			g_Para.Walk_Asb.Bit.LR_Axis_Moon 	= 0;
			//联动cbb初始化
			g_Asb.Para.Iam = AsbCarStrNo_2;
			g_Asb.Para.Seat = AsbSiteSeat_1;
			//特殊运动中心处理
			g_Para.p2Fucs.p2PrjExp_MR_Init_Asb_Axis();
			
			/*联动模式下，初始化轮系运动中心*/
			MotorResolving_Init_Asb(&g_MotorResolving, g_Asb.Output.Center_Cal.X, g_Asb.Output.Center_Cal.Y);//舵轮
			MecanumResolving_Init_Asb(&g_MecanumResolving, g_Asb.Output.Center.X, g_Asb.Output.Center.Y);//麦轮

			/*选择联动避障源*/
			g_ROA.Sel = ObsSel_Asb;

			/*各车根据不同联动位置 自定义清除联动车障碍状态 参数更新*/
			ACC_Assemly_ObsCLRCustom();

			/*切换联动通信为从站*/
			//联动下，针对主从车进行一次联动通信切换，联动切换信号量置false
			if(Semaphore == false)
			{
				AsbSwitchEnterInit_MB2Slave();
				Semaphore = true;
			}

			/*阵型位置，根据阵型切换到固定位置
			超低车固定去前面主车、右边主车，是为了让超低车的主车适应特殊的运动中心变换，
			即从车的运动中心不依赖联动距离、避免联动距离通过联动通信过程，
			其他车型如果不这样，需将下方代码注释，
			*/
//			if (g_Asb.Para.SiteNo == AsbSiteNo_FB)
//				g_Asb.Para.Seat = AsbSiteSeat_2;
//			else if (g_Asb.Para.SiteNo == AsbSiteNo_FB1)
//				g_Asb.Para.Seat = AsbSiteSeat_2;
//			else if (g_Asb.Para.SiteNo == AsbSiteNo_LR)
//				g_Asb.Para.Seat = AsbSiteSeat_1;
//			else {}
		}
		else{}//保留	
	}
	else//非联动模式，默认选择A车
	{
		//单动下，联动切换信号量置false
		Semaphore = false;
		/*单动模式下，初始化轮系运动中心*/
		MotorResolving_Init(&g_MotorResolving);//舵轮
		MecanumResolving_Init(&g_MecanumResolving);//麦轮

		/*选择单动避障源*/
		g_ROA.Sel = ObsSel_Self;

		//全部打开
		g_ROA.ForceCLR[Radar_Front] = State_Error;
		g_ROA.ForceCLR[Radar_Back] = State_Error;
		g_ROA.ForceCLR[Radar_Left] = State_Error;
		g_ROA.ForceCLR[Radar_Right] = State_Error;
		
		/*序号和角色固定绑定*/
		/*主从车选择，默认初始化主车，可根据屏幕选择为从车*/
		if(g_Asb.Para.Role == AsbRole_Slave)
		{
			g_Asb.Para.Role = AsbRole_Slave;
			//特殊联动运动中心配置
			g_Para.Walk_Asb.Bit.FB_Axis_Forward = 1;
			g_Para.Walk_Asb.Bit.FB_Axis_Moon 	= 0;
			g_Para.Walk_Asb.Bit.LR_Axis_Fixed 	= 1;
			g_Para.Walk_Asb.Bit.LR_Axis_Moon 	= 0;
			//联动cbb初始化
			g_Asb.Para.Iam = AsbCarStrNo_2;
			g_Asb.Para.Seat = AsbSiteSeat_1;
		}
		else
		{
			g_Asb.Para.Role = AsbRole_Master;
			//特殊联动运动中心配置
			g_Para.Walk_Asb.Bit.FB_Axis_Forward = 0;
			g_Para.Walk_Asb.Bit.FB_Axis_Moon 	= 1;
			g_Para.Walk_Asb.Bit.LR_Axis_Fixed 	= 0;
			g_Para.Walk_Asb.Bit.LR_Axis_Moon 	= 1;
			//联动cbb初始化
			g_Asb.Para.Iam = AsbCarStrNo_1;
			g_Asb.Para.Seat = AsbSiteSeat_2;//主车在后面
		}	

		/*阵型位置，根据阵型切换到固定位置
		超低车固定去前面主车、右边主车，是为了让超低车的主车适应特殊的运动中心变换，
		即从车的运动中心不依赖联动距离、避免联动距离通过联动通信过程，
		其他车型如果不这样，需将下方代码注释，
		*/
	}
	return 1;
}

/************************************END OF FILE************************************/
