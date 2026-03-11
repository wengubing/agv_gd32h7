/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_AGVAutorunHangHighSta.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.xx.xx
  * Description: AGV自动运行的框架代码
  *******************************************************************************/
#include "app_SkeletonAGV.h"
#include "app_AGVAutorunHangHigh.h"


/***************************************************************************************
** 函数名称: AGV_HangHighStaCheck
** 功能描述: 根据路径信息和当前AGV状态，确定要不要切换至悬挂控制状态
** 参    数: 
** 返 回 值: res 0-无需切换状态，1-切换悬挂控制
****************************************************************************************/
uint8_t AGV_HangHighCheck(void)
{
	uint8_t 	HangHighRes = 0;
	uint16_t	HangTargetHigh	=	abs(pPathStation->HangLiftHigh - g_HostData.AGVHangControl.HangActualHigh);
	if(	HangTargetHigh > g_HostData.Para_Index.AGV_DeadZoneHangHighMax
		&& pPathStation->HangLiftHigh != 0xFFFF)
	{
		/*更新高度相关内容*/
		if(pPathStation->HangLiftHigh != 0xFFFF)
		{
			g_HostData.AGVHangControl.HangTargetHigh = pPathStation->HangLiftHigh;
		}
		g_HostData.AGVHangControl.HangTargetAdjustHigh = HangTargetHigh;
		g_HostData.AGVHangControl.HangStartAdjustHigh = g_HostData.AGVHangControl.HangActualHigh;
		//AGV_FuncSta_Next = AGV_FunctionSta_HangHigh;
		HangHighRes = 0X01;
	}
	else
	{
		g_HostData.AGVHangControl.HangTargetAdjustHigh = 0;
		g_HostData.AGVHangControl.HangTargetHigh = g_HostData.AGVHangControl.HangActualHigh;
	}
	return HangHighRes;
}

/***************************************************************************************
** 函数名称: AGV_HangLevelCheck
** 功能描述: 根据路径信息和当前AGV状态，确定要不要切换至悬挂控制状态
** 参    数: 
** 返 回 值: res 0-无需切换状态，1-切换悬挂控制
****************************************************************************************/
__weak uint8_t AGV_HangLevelCheck(void)
{
	return 0;
}
/***************************************************************************************
** 函数名称: AGV_HangLevelCheck
** 功能描述: 根据路径信息和当前AGV状态，确定要不要切换至悬挂控制状态
** 参    数: 
** 返 回 值: res 0-无需切换状态，1-切换悬挂控制
****************************************************************************************/
__weak uint8_t AGV_HangHorizonCheck(void)
{
	return 0;
}
/***************************************************************************************
** 函数名称: AGV_HangStaCheck
** 功能描述: 根据路径信息和当前AGV状态，确定要不要切换至悬挂控制状态
** 参    数: 
** 返 回 值: res 0-无需切换状态，1-切换悬挂控制
****************************************************************************************/
uint8_t AGV_HangStaCheck(void)
{
	uint8_t HangRes = 0;
	if(g_HostData.AGVPendantFun.HangControl == 0X01)
	{
		switch(g_HostData.AGVHangControl.HangAdjustMode)
		{
			default:
			case AGVHangMode_Height:	/*悬挂 高度调节模式*/
			{
				HangRes = AGV_HangHighCheck();
			}
			break;
			case AGVHangMode_Level:		/*悬挂 水平调节模式-基于倾角传感器调整*/
			{
				/*暂无功能实现*/
				HangRes = AGV_HangLevelCheck();
			}
			break;
			case AGVHangMode_Horizon:	/*悬挂 地坪调节模式-三点悬挂都达到均值*/
			{
				/*暂无功能实现*/
				HangRes = AGV_HangHorizonCheck();
			}
			break;
		}
	}
	return HangRes;
}

/***************************************************************************************
** 函数名称: AGV_TskFunctionStaHangHeight
** 功能描述: 悬挂高度调整；将悬挂的实际高度调节到目标高度
** 参    数: None
** 返 回 值: None
****************************************************************************************/
uint8_t AGV_TskFunctionStaHangHeight(void)
{
	int16_t Altitude_Dis 		= 0;
	int16_t TargetAdjustHigh	= 0;
	int16_t CurrentAdjustHigh	= 0;
	float	Altitudet_K			= 0;
	int16_t TargetAdjustHigh_02	= 0;
	int16_t TargetAdjustHigh_08	= 0;
	int16_t Altitude_SlownDis	= 0;
	uint8_t	res = 0;
	/*确保底盘程序已经将模式切换成了支腿控制状态，否则这个函数直接返回即可*/
	{
		if((g_HostData.ActualAGVWorkMode != AGVMode_HangHigh))
		{
			g_HostData.TargetAGVWorkMode = AGVMode_HangHigh;
			return 0xFF;
		}
	}
	/*1.获取需要调节的高度信息*/
	TargetAdjustHigh	= g_HostData.AGVHangControl.HangTargetHigh-g_HostData.AGVHangControl.HangActualHigh;
	CurrentAdjustHigh	= abs(TargetAdjustHigh);
	Altitudet_K			= CurrentAdjustHigh/(TargetAdjustHigh);
	Altitude_Dis 		= g_HostData.Para_Index.AGV_HangAltitude_Dis;
	Altitude_SlownDis	= g_HostData.Para_Index.AGV_HangAltitude_SlownDis;
	TargetAdjustHigh	= g_HostData.AGVHangControl.HangTargetAdjustHigh;
	CurrentAdjustHigh	= abs(g_HostData.AGVHangControl.HangStartAdjustHigh - g_HostData.AGVHangControl.HangActualHigh);

	TargetAdjustHigh_02 = TargetAdjustHigh*2/10;
	TargetAdjustHigh_08 = (TargetAdjustHigh*8/10)-Altitude_Dis;
	if(TargetAdjustHigh_02 > Altitude_SlownDis)
	{
		TargetAdjustHigh_02 = Altitude_SlownDis;
		TargetAdjustHigh_08 = TargetAdjustHigh-Altitude_SlownDis-Altitude_Dis;
	}

	/*调节距离过小，以最低速度调节*/
	if((TargetAdjustHigh*5/10) <= Altitude_Dis)
	{
		Altitudet_K = 0.0f;
		res = 1;
	}
	/*80%-提前减速距离 行程，以6挡位全速调节*/
	else if(CurrentAdjustHigh <= TargetAdjustHigh_08)
	{
		Altitudet_K = 1.0f;
		res = 1;
	}
	/*剩余20%-提前减速距离 行程，比例减速调节*/
	else if(CurrentAdjustHigh < TargetAdjustHigh)
	{
		Altitudet_K = (TargetAdjustHigh-CurrentAdjustHigh-Altitude_Dis)/(TargetAdjustHigh_02);
		res = 2;
	}
	/*到位，以最低挡位反向调节*/
	else
	{
		Altitudet_K = 0.0f;
		res = 3;
	}

	if(Altitudet_K > 1.0f){Altitudet_K = 1.0f;}
	else if(Altitudet_K < 0.0f){Altitudet_K =  0.0f;}

	/*2.根据高度信息情况，设置升降速度挡位*/
	g_HostData.AGVUpDownSpeedVal = Altitudet_K*5.0f + 1;

	/*3.根据高度信息情况，设置升降按钮值*/
	{
		int16_t AdjustHigh	= g_HostData.AGVHangControl.HangTargetHigh-g_HostData.AGVHangControl.HangActualHigh;
		if(AdjustHigh < (-1)*g_HostData.Para_Index.AGV_DeadZonHangeHighMin)
		{
			g_HostData.AGVHandleButtom = 2;
		}
		else if(AdjustHigh > g_HostData.Para_Index.AGV_DeadZonHangeHighMin)
		{
			g_HostData.AGVHandleButtom = 1;
		}
		/*4.升降按钮值为0的时候，完成姿态控制功能，状态机跳转*/
		else
		{
			g_HostData.AGVHandleButtom = 0;
			res = 0;
			//g_HostData.AGV_FuncSta = AGV_FunctionSta_Init;
		}
	}
	return res;
}

/***************************************************************************************
** 函数名称: AGV_TskFunctionStaHangLevel
** 功能描述: 悬挂高度调整；将悬挂在当前高度下，基于倾角传感器的数据调节成水平
** 参    数: None
** 返 回 值: res		0-控制结束		其他-控制中
****************************************************************************************/
__weak uint8_t AGV_TskFunctionStaHangLevel(void)
{
	return 0;
}
/***************************************************************************************
** 函数名称: AGV_TskFunctionStaHangHorizon
** 功能描述: 悬挂高度调整；将悬挂在当前高度下，基于悬挂高度的数据调节成地平
** 参    数: None
** 返 回 值: res		0-控制结束		其他-控制中
****************************************************************************************/
__weak uint8_t AGV_TskFunctionStaHangHorizon(void)
{
	return 0;
}
/***************************************************************************************
** 函数名称: AGV_TskFunctionStaHang
** 功能描述: 
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_TskFunctionStaHang(void)
{
	uint8_t	HangAdjustRunSta = 0;
	switch(g_HostData.AGVHangControl.HangAdjustMode)
	{
		default:
		case AGVHangMode_Height:	/*悬挂 高度调节模式*/
		{
			HangAdjustRunSta = AGV_TskFunctionStaHangHeight();
		}
		break;
		case AGVHangMode_Level:		/*悬挂 水平调节模式-基于倾角传感器调整*/
		{
			/*暂无功能实现*/
			HangAdjustRunSta = AGV_TskFunctionStaHangLevel();
		}
		break;
		case AGVHangMode_Horizon:	/*悬挂 地坪调节模式-三点悬挂都达到均值*/
		{
			/*暂无功能实现*/
			HangAdjustRunSta = AGV_TskFunctionStaHangHorizon();
		}
		break;
	}
	/*执行函数返回0，结束当前自动环节*/
	if(HangAdjustRunSta == 0)
	{
		g_HostData.AGV_FuncSta = AGV_FunctionSta_Init;
	}
	g_HostData.AGVHangControl.HangAdjustRunSta = HangAdjustRunSta;

}

/************************************END OF FILE************************************/
