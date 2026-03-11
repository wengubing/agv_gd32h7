/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_AGVAutorunPosture.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.xx.xx
  * Description: AGV自动运行的框架代码
  *******************************************************************************/
#include "app_SkeletonAGV.h"
#include "app_AGVAutorunPosture.h"

/***************************************************************************************
** 函数名称: AGV_PostureStaCheck
** 功能描述: 根据路径信息和当前AGV状态，确定要不要切换至姿态控制状态
** 参    数: 
** 返 回 值: res 0-无需切换状态，1-切换至姿态控制
****************************************************************************************/
uint8_t AGV_PostureStaCheck(AGVFunctionSta_TypeDef AGV_FuncSta_Next)
{
	float TargetPostureAngle_temp = 0.0f;
	float TargetPostureAngle_P = 0.0f;
	float TargetPostureAngle_N = 0.0f;
	int8_t PosturOrientation_temp = 0;
	/*默认使用第速度6挡位*/
	g_HostData.AGVWalkSpeedVal 		= g_HostData.Para_Index.AGVWalkDefaultSpeedVal;
	if(g_HostData.AGVNavigateMode == AGVNavigateMode_QR)
	{
		float Temp_Towards = pPathStation->Towards;
		if(  (Temp_Towards < 45.0f&&Temp_Towards >= 0.0f)
			||(Temp_Towards < 360.0f&&Temp_Towards >= 315.0f))
		{
			g_HostData.TargetToward = 0.0f;
		}
		else if(Temp_Towards < 135.0f&&Temp_Towards >= 45.0f)
		{
			g_HostData.TargetToward = 90.0f;
		}
		else if(Temp_Towards < 225.0f&&Temp_Towards >= 135.0f)
		{
			g_HostData.TargetToward = 180.0f;
		}
		else if(Temp_Towards < 315.0f&&Temp_Towards >= 225.0f)
		{
			g_HostData.TargetToward = 270.0f;
		}
	}
	else
	{
		g_HostData.TargetToward 		= pPathStation->Towards;
	}
	g_HostData.ActualToward			= g_HostData.NavigationData.NavLidar_z;
	g_HostData.StartPostureAngle	= g_HostData.ActualToward;
	
	TargetPostureAngle_temp	= (g_HostData.ActualToward)-(g_HostData.TargetToward);
	TargetPostureAngle_P	= fabs(TargetPostureAngle_temp);
	TargetPostureAngle_N	= fabs(360.0f-TargetPostureAngle_P);
	if(TargetPostureAngle_P < TargetPostureAngle_N + 5)
	{
		g_HostData.TargetPostureAngle = TargetPostureAngle_P;
		PosturOrientation_temp = 1;
	}
	else
	{
		g_HostData.TargetPostureAngle = TargetPostureAngle_N;
		PosturOrientation_temp = -1;
	}
	g_HostData.PosturOrientation	= TargetPostureAngle_temp<0?PosturOrientation_temp:-PosturOrientation_temp;

	/*如果上一次是调整Z轴的精定位，这次就不调姿态了，等举升结束后，自然会调整姿态*/
	if(	  (g_HostData.AGVGetGoodsAccPosMode == AGVAccPosMode_FixQR_Z||g_HostData.AGVGetGoodsAccPosMode == AGVAccPosMode_ParkQR_XZ)
		&&AGV_FuncSta_Next == AGV_FunctionSta_AccuratePosition)
	{
		/*什么都不做*/
	}
	else if((g_HostData.TargetPostureAngle > (g_HostData.WalkGoOnFlag == 0x00?g_HostData.Para_Index.AGV_AngAdjZoneMax:g_HostData.Para_Index.AGV_WalkAngAdjZone)))
	{
		return 1;
//				AGV_FuncSta_Next = AGV_FunctionSta_Posture;
//				break;
	}
	return 0;
}


/***************************************************************************************
** 函数名称: AGV_PostureMagnetic
** 功能描述: 在十字磁条上自转 90°或180°
** 参    数: DeltaAngle	设定需要旋转的角度与实际旋转的角度的差值,主要确认顺时针旋转还是逆时针
** 返 回 值: res		0-到位  1-加速中 2-匀速中 3-减速中
1、初始状态应该是高电平，如果不是高电平是需要报错的
2、检测下降沿，在高电平出现到检测到下降沿，这段时间一直加速
3、下降沿之后会一直保持低电平，这段时间就匀速
4、当检测到上升沿之后，开始减速，直至减速到0退出，停车
5、在没减速到0之前 磁条偏移量已经到0，直接停车

****************************************************************************************/
uint8_t AGV_PostureMagnetic(void)
{
	static float	Propt_K		= 0;
	static int8_t	Sta	= 0;
	static uint32_t Delay_time	= 0;
	/*	0-初始状态开始自转		1-还没离开磁条，加速中	2-离开磁条，匀速中
		3-再次检测到磁条减速中	4-减速至0，认为到位		5-磁条基本居中，到位*/
	int8_t	res	= 1;
	/*间隔20ms执行一次，确保减速加速过程都是正常的*/
	if(g_HostData.Sys1msCounter >= Delay_time + 200)
	{
		Delay_time = g_HostData.Sys1msCounter;
		/*如果自转到位,速度清零；自转开始的前10°比例加速;到位前10°比例减速*/
		switch(Sta)
		{
			case 0:
			{
				if(g_HostData.MagneticData.Sta_F != 0)
				{
					Sta = 1;
					Propt_K = 0.0f;
				}
			}
			break;
			case 1:
			{
				if(g_HostData.MagneticData.Sta_F != 0)
				{
					Propt_K += 0.01f;
					if(Propt_K > 0.2f){Propt_K = 0.2f;}
				}
				else
				{
					Sta = 2;
				}
			}
			break;
			case 2:
			{
				res = 2;
				if(g_HostData.MagneticData.Sta_F != 0)
				{
					Sta = 3;
				}
			}
			break;
			case 3:
			{
				res = 3;
				Propt_K -= 0.01f;
				if(Propt_K < 0.01f){Propt_K = 0.01f;}
				if(  g_HostData.MagneticData.Distance_F < 10
					&&g_HostData.MagneticData.Distance_F > -10)
				{
					Propt_K = 0.00f;
					Sta = 5;
				}
			}
			break;
			default:
			break;
		}
		/*修改控制速度速度*/

		g_HostData.ActualPostureVelocity = 0.1f*g_HostData.PosturOrientation*Propt_K*V_H;

		if(Sta >= 4)
		{
			res = 0;
			Sta = 0;
		}
	}

	return res;
}
/***************************************************************************************
** 函数名称: AGV_PostureOdOMagnetic
** 功能描述: 利用里程计辅助，在十字磁条上自转 90°或180°；里程计计算旋转了多少度；
** 参    数: 无
** 返 回 值: res		0-到位  1-加速中 2-匀速中 3-减速中
****************************************************************************************/
uint8_t AGV_PostureOdOMagnetic(void)
{
	int8_t		res	= 0;
	float 		CurrentAngle 			= g_HostData.ActualPostureAngle;
	float		DeltaAngle				= g_HostData.TargetPostureAngle;
	float		SlowDownAngle			= g_HostData.Para_Index.AGV_Posture_OdOMagnetic_Angle;
	float		PostureOdOMag_k			= g_HostData.Para_Index.AGV_Posture_OdOMagnetic_k;
	float		PostureOdOMag_Kmin		= g_HostData.Para_Index.AGV_Posture_OdOMagnetic_KMin;
	int16_t		OdOMagnetic_MaxOff_10	= -g_HostData.Para_Index.AGV_Posture_OdOMagnetic_MaxOff;
	int16_t		OdOMagnetic_MaxOff_1	= OdOMagnetic_MaxOff_10*0.1f;
	int16_t		OdOMagnetic_MaxOff_5	= OdOMagnetic_MaxOff_10*0.5f;
	
	int16_t 	ZeroOffset_Head = 0;
	int16_t 	ZeroOffset_Tail = 0;
	int16_t 	ZeroOffset		= 0;
	float		Propt_K			= 0;

	/*基于里程计的结果，控制加速、匀速、减速过程*/
	if(CurrentAngle <= (DeltaAngle/10))
	{
		Propt_K = CurrentAngle/(DeltaAngle/10);
		if(Propt_K < 0.10f)			{Propt_K = 0.10f;}
		else if(Propt_K > 1.00f)	{Propt_K = 1.00f;}
		res = 1;
	}
	else if(CurrentAngle <= (DeltaAngle*9/10)-SlowDownAngle)
	{
		Propt_K = 1.0f;
		res = 2;
	}
	else if(CurrentAngle < DeltaAngle)
	{
		Propt_K = (DeltaAngle-CurrentAngle-SlowDownAngle)/(DeltaAngle/10);
		if(Propt_K < PostureOdOMag_Kmin)	{Propt_K = PostureOdOMag_Kmin;}
		else if(Propt_K > 1.00f)			{Propt_K = 1.00f;}
		res = 3;
	}
	else
	{
		res = 4;
	}
	/*基于磁条检测的偏移量，确定要不要停车*/
	if(res >= 3)
	{
		/*要求十字磁条对应 磁导航传感器数据正负*/
		/*            	前方				*/
		/*									*/
		/*          + |------| -			*/
		/*    +   |               |   -		*/
		/*  HMI   |               |			*/
		/*        |               |			*/
		/*    -   |               |   +		*/
		/*          - |------| + 			*/
		/*                  				*/
		/*            	后方				*/
		uint8_t Mag_Sta = 0x00;
		int16_t Mag_diff = g_HostData.Para_Index.AGV_Postur_Orientation_diff;
		if(g_HostData.NextAGVRunDir == AGVRunDir_Forward||g_HostData.NextAGVRunDir == AGVRunDir_Back)
		{
			Mag_Sta = g_HostData.MagneticData.Sta_F|g_HostData.MagneticData.Sta_B;
			ZeroOffset_Head = g_HostData.MagneticData.Distance_F;
			ZeroOffset_Tail = g_HostData.MagneticData.Distance_B;
		}
		else if(g_HostData.NextAGVRunDir == AGVRunDir_Left||g_HostData.NextAGVRunDir == AGVRunDir_Right)
		{
			Mag_Sta = g_HostData.MagneticData.Sta_L|g_HostData.MagneticData.Sta_R;
			ZeroOffset_Head = (-1)*g_HostData.MagneticData.Distance_L;
			ZeroOffset_Tail = (-1)*g_HostData.MagneticData.Distance_R;
		}
		/*计算得到最后停车到位的偏移量*/
		ZeroOffset = (Mag_Sta!=0X00?(Mag_diff*g_HostData.PosturOrientation*(ZeroOffset_Head + ZeroOffset_Tail)):OdOMagnetic_MaxOff_10);
		/*测试代码，将在磁条上旋转的停车偏移量给到全局变量方便查看*/
		g_HostData.MagPostureZeroOffset = ZeroOffset;
		/*偏移量从负变化成正，预示着旋转到位了*/
		if(ZeroOffset >= 0)
		{
			Propt_K = 0;
			res = 0;
		}
		/*剩余10mm时2级减速*/
		else if(ZeroOffset <= 0 && OdOMagnetic_MaxOff_1 < ZeroOffset)
		{
			Propt_K = 0.3f*PostureOdOMag_Kmin;
		}
		/*剩余30mm时1级减速*/
		else if(ZeroOffset <= OdOMagnetic_MaxOff_1 && OdOMagnetic_MaxOff_5 < ZeroOffset)
		{
			Propt_K = 0.5f*PostureOdOMag_Kmin;
		}
		/*剩余30mm时1级减速*/
		else if(ZeroOffset <= OdOMagnetic_MaxOff_5 && OdOMagnetic_MaxOff_10 <= ZeroOffset)
		{
			Propt_K = PostureOdOMag_Kmin;
		}
		else
		{
			/*什么都不做*/
		}

		{
			float temp_k = (g_HostData.ActualPostureVelocity)/(g_HostData.PosturOrientation*PostureOdOMag_k*V_H);
			if(Propt_K != 0.0f && Propt_K > temp_k)
			{
				Propt_K = temp_k;
			}
		}
	}
	else
	{
		/*什么都不需要做*/
	}
	/*修改控制速度速度*/
	g_HostData.ActualPostureVelocity = PostureOdOMag_k*g_HostData.PosturOrientation*Propt_K*V_H;
	return res;
}

/***************************************************************************************
** 函数名称: AGV_PostureFixQRMagnetic
** 功能描述: 利用工装二维码辅助，在十字磁条上自转 90°或180°里程计计算旋转了多少度，二维码判断是否到位；
** 参    数: 无
** 返 回 值: res		0-到位  1-加速中 2-匀速中 3-减速中
****************************************************************************************/
uint8_t AGV_PostureFixQRMagnetic(void)
{
	int8_t		res	= 0;
	float 		CurrentAngle 		= g_HostData.ActualPostureAngle;
	float		DeltaAngle			= g_HostData.TargetPostureAngle;
	float		PostureOdOMag_Kmin	= g_HostData.Para_Index.AGV_Posture_OdOMagnetic_KMin;
	float		Propt_K			= 0;
	int16_t 	ZeroOffset_Head = 0;
	int16_t 	ZeroOffset_Tail = 0;
	int16_t 	ZeroOffset		= 0;
	
	/*基于里程计的结果，控制加速、匀速、减速过程*/
	if(CurrentAngle <= (DeltaAngle*0.1f))
	{
		Propt_K = CurrentAngle/(DeltaAngle*0.1f);
		if(Propt_K < 0.30f)			{Propt_K = 0.30f;}
		else if(Propt_K > 1.00f)	{Propt_K = 1.00f;}
		res = 1;
	}
	else if(CurrentAngle <= (DeltaAngle*0.9f))
	{
		Propt_K = 1.0f;
		res = 2;
	}
	else if(CurrentAngle < DeltaAngle)
	{
		Propt_K = (DeltaAngle-CurrentAngle)/(DeltaAngle*0.1f);
		if(Propt_K < 0.30f)			{Propt_K = 0.30f;}
		else if(Propt_K > 1.00f)	{Propt_K = 1.00f;}
		res = 3;
	}
	
	if(res >= 3)
	{
		/*如果工装二维码不起作用，计算磁条检测的偏移量*/
		if(g_HostData.FixtureQRData.PintSta == 0x00)
		{
			uint8_t Mag_Sta = 0x00;
			if(g_HostData.NextAGVRunDir == AGVRunDir_Forward||g_HostData.NextAGVRunDir == AGVRunDir_Back)
			{
				Mag_Sta = g_HostData.MagneticData.Sta_F&g_HostData.MagneticData.Sta_B;
				ZeroOffset_Head = g_HostData.MagneticData.Distance_F;
				ZeroOffset_Tail = g_HostData.MagneticData.Distance_B;
			}
			else if(g_HostData.NextAGVRunDir == AGVRunDir_Left||g_HostData.NextAGVRunDir == AGVRunDir_Right)
			{
				Mag_Sta = g_HostData.MagneticData.Sta_L&g_HostData.MagneticData.Sta_R;
				ZeroOffset_Head = (-1)*g_HostData.MagneticData.Distance_L;
				ZeroOffset_Tail = (-1)*g_HostData.MagneticData.Distance_R;
			}
			/*计算得到最后停车到位的偏移量*/
			ZeroOffset = Mag_Sta!=0X00?g_HostData.PosturOrientation*(ZeroOffset_Head + ZeroOffset_Tail):-70;
			/*测试代码，将在磁条上旋转的停车偏移量给到全局变量方便查看*/
			g_HostData.MagPostureZeroOffset = ZeroOffset;
			/*偏移量从负变化成正 并且最终偏差比10mm小，预示着旋转到位了*/
			if(ZeroOffset > -10 && ZeroOffset <= -30)
			{
				Propt_K = PostureOdOMag_Kmin;
			}
			else if(ZeroOffset < 0 && ZeroOffset <= -10)
			{
				Propt_K = 0.5f*PostureOdOMag_Kmin;
			}
			else if(ZeroOffset >= 0 && ZeroOffset < 10)
			{
				Propt_K = 0;
				res = 0;
			}
			/*最终偏差比10mm大，倒着转一转*/
			else if(ZeroOffset >= 10)
			{
				Propt_K = -0.5f*PostureOdOMag_Kmin;
			}
			/*否则将速度进一步降低来旋转*/
			else
			{
				Propt_K = 2*PostureOdOMag_Kmin;
			}
		}
		/*如果工装二维码起作用，计算磁条检测的偏移量*/
		else
		{			
			if(g_HostData.FixtureQRData.Offset_z > 0 && g_HostData.FixtureQRData.Offset_z > g_HostData.Para_Index.AGV_AngAdjZoneMin)
			{
				Propt_K = PostureOdOMag_Kmin;
			}
			else if(g_HostData.FixtureQRData.Offset_z < 0 && g_HostData.FixtureQRData.Offset_z < -g_HostData.Para_Index.AGV_AngAdjZoneMin)
			{
				Propt_K = PostureOdOMag_Kmin;
			}
			else
			{
				Propt_K = 0;
				res = 0;
			}
		}
	}
	else
	{
		/*什么都不需要做*/
	}

	/*修改控制速度速度*/
	g_HostData.ActualPostureVelocity = g_HostData.Para_Index.AGV_Posture_OdOMagnetic_k*g_HostData.PosturOrientation*Propt_K*V_H;
	return res;
}


/***************************************************************************************
** 函数名称: AGV_PostureQR
** 功能描述: 在二维码上 旋转到指定角度
** 参    数: DeltaAngle	设定需要旋转的角度与实际旋转的角度的差值（从0开始增加到目标值）
** 返 回 值: res		0-到位  1-加速中 2-匀速中 3-减速中
****************************************************************************************/
uint8_t AGV_PostureQR(void)
{
	int8_t		res	= 0;
	float		CurrentAngle 	= g_HostData.ActualPostureAngle;
	float		DeltaAngle		= g_HostData.TargetPostureAngle;
	float		Propt_K			= 0;
	float		ProptQR_KMin	= g_HostData.Para_Index.AGV_Posture_QR_KMin;
	/*如果自转到位,速度清零；自转开始的前1/10行程比例加速;到位前前2/10行程比例减速*/
	if(   fabs(g_HostData.TargetToward - g_HostData.ActualToward) > g_HostData.Para_Index.AGV_AngAdjZoneMin
		&& DeltaAngle != 0
		)
	{
			if(CurrentAngle <= (DeltaAngle/10))
			{
				Propt_K = CurrentAngle/(DeltaAngle/10);
				if(Propt_K < ProptQR_KMin*3)	{Propt_K = ProptQR_KMin*3;}
				else if(Propt_K > 1.00f)		{Propt_K = 1.00f;}
				res = 1;
			}
			else if(CurrentAngle <= (DeltaAngle*8/10))
			{
				Propt_K = 1.0f;
				res = 2;
			}
			else if(CurrentAngle < DeltaAngle)
			{
				Propt_K = (DeltaAngle-CurrentAngle)/(DeltaAngle*2/10);
				if(Propt_K <ProptQR_KMin)			{Propt_K = ProptQR_KMin;}
				else if(Propt_K > 1.00f)	{Propt_K = 1.00f;}
				res = 3;
			}
			else
			{
				Propt_K = 0;
			}
	}
	/*如果调整目标比较小(3°以内)，小速度的调整即可*/
	if(Propt_K != 0.0f && DeltaAngle < 3.0f)
	{
		Propt_K = ProptQR_KMin*2;
	}
	/*修改控制速度速度*/
	g_HostData.ActualPostureVelocity = g_HostData.Para_Index.AGV_Posture_QR_K*g_HostData.PosturOrientation*Propt_K*V_H;
	return res;
}

/***************************************************************************************
** 函数名称: AGV_PostureSLAM
** 功能描述: 通过SLAM的定位数据 旋转到指定角度
** 参    数: 无
** 返 回 值: res		0-到位  1-加速中 2-匀速中 3-减速中
****************************************************************************************/
uint8_t AGV_PostureSLAM(void)
{
	int8_t		res	= 0;
	float		CurrentAngle 			= g_HostData.ActualPostureAngle;
	float		DeltaAngle				= g_HostData.TargetPostureAngle;
	float		Propt_K					= 0;
	float		PostureOdO_k			= g_HostData.Para_Index.AGV_Posture_ODO_K;
	float		PostureOdO_Kmin			= g_HostData.Para_Index.AGV_Posture_ODO_KMin;
	float		Posture_WalkAngAdjZone	= g_HostData.Para_Index.AGV_WalkAngAdjZone;
	/*如果自转到位,速度清零；自转开始的前1/10行程比例加速;到位前前2/10行程比例减速*/
	if(   fabs(g_HostData.TargetToward - g_HostData.ActualToward) > g_HostData.Para_Index.AGV_AngAdjZoneMin
		&& DeltaAngle != 0
		)
	{
			if(CurrentAngle <= (DeltaAngle/10))
			{
				Propt_K = CurrentAngle/(DeltaAngle/10);
				if(Propt_K < PostureOdO_Kmin*2)	{Propt_K = PostureOdO_Kmin*2;}
				else if(Propt_K > 1.00f)		{Propt_K = 1.00f;}
				res = 1;
			}
			else if(CurrentAngle <= (DeltaAngle*8/10))
			{
				Propt_K = 1.0f;
				res = 2;
			}
			else if(CurrentAngle < DeltaAngle)
			{
				Propt_K = (DeltaAngle-CurrentAngle)/(DeltaAngle*2/10);
				if(Propt_K < PostureOdO_Kmin)	{Propt_K = PostureOdO_Kmin;}
				else if(Propt_K > 1.00f)		{Propt_K = 1.00f;}
				res = 3;
			}
			else
			{
				Propt_K = 0;
			}
	}
	/*如果调整目标比较小(行走调整角度以内)，小速度的调整即可*/
	if(Propt_K != 0.0f && DeltaAngle < Posture_WalkAngAdjZone)
	{
		Propt_K = PostureOdO_Kmin;
	}
	/*修改控制速度速度*/
	g_HostData.ActualPostureVelocity = PostureOdO_k*g_HostData.PosturOrientation*Propt_K*V_H;
	return res;
}
/***************************************************************************************
** 函数名称: AGV_TskFunctionStaPosture
** 功能描述: 
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_TskFunctionStaPosture(void)
{
	uint8_t		res	= 0;
	float 		ActualPostureAngle_Temp = 0.0f;
	
	/*确保底盘程序已经将模式切换成了自转状态，否则这个函数直接返回即可*/
	{
		if((g_HostData.ActualAGVWorkMode != AGVMode_GoRotation))
		{
			g_HostData.TargetAGVWorkMode = AGVMode_GoRotation;
			return;
		}
	}
	/*自转的相关角度更新*/
	ActualPostureAngle_Temp = fabs((g_HostData.ActualToward) - (g_HostData.StartPostureAngle));
	g_HostData.ActualToward = g_HostData.NavigationData.NavLidar_z;
	g_HostData.ActualPostureAngle = ActualPostureAngle_Temp > 180.0f?360.0f-ActualPostureAngle_Temp:ActualPostureAngle_Temp;

	/*1.旋转控制，不同的方式需要通过不同的接口进入*/
	{
		switch(g_HostData.AGVPosture)
		{
			case AGVPosture_Magnetic:	/*在十字磁条上自转*/
			{
				res = AGV_PostureMagnetic();
				if(res == 0)
				{
					g_HostData.NavigationData.NavLidar_z = g_HostData.TargetToward;
					g_HostData.ActualPostureAngle = 0;
				}
			}
			break;
			case AGVPosture_ODO_Mag:	/*在里程计辅助在磁条上自转*/
			{
				res = AGV_PostureOdOMagnetic();
				if(res == 0)
				{
					g_HostData.NavigationData.NavLidar_z = g_HostData.TargetToward;
					g_HostData.ActualPostureAngle = 0;
				}
			}
			break;
			case AGVPosture_FixQR_Mag:	/*在里程计辅助在磁条上自转*/
			{
				res = AGV_PostureFixQRMagnetic();
				if(res == 0)
				{
					g_HostData.NavigationData.NavLidar_z = g_HostData.TargetToward;
					g_HostData.ActualPostureAngle = 0;
				}
			}
			break;
			case AGVPosture_QR:			/*在二维码上自转*/
			{
				/*函数内部会修改自转的实际速度，到位了会返回0；返回其他的都是运动状态*/
				res = AGV_PostureQR();
			}break;
			default:
			case AGVPosture_NULL:		/*初始化		*/
			case AGVPosture_ODO:		/*激光导航	*/
			{
				/*函数内部会修改自转的实际速度，到位了会返回0；返回其他的都是运动状态*/
				res = AGV_PostureSLAM();
			}
			break;
		}
	}

	/*2.判断是否到位，完成姿态控制功能，状态机跳转*/
	{
		if(res == 0)
		{
			g_HostData.PosturRunSta = 4;
			g_HostData.ActualPostureVelocity = 0;
			g_HostData.AGV_FuncSta = AGV_FunctionSta_Init;
		}
	}

	/*3.自转速度输出，对遥控底盘输出摇杆控制命令;*/
	{
		g_HostData.ActualForwardVelocity = 0.0f;
		memset(g_HostData.Hxyz,0,sizeof(g_HostData.Hxyz));
		g_HostData.Hxyz[2] = g_HostData.ActualPostureVelocity;
	}
}



/************************************END OF FILE************************************/
