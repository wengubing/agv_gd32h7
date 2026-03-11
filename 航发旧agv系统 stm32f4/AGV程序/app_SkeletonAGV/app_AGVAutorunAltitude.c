/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_AGVAutorunAltitude.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.xx.xx
  * Description: AGV自动运行的框架代码
  *******************************************************************************/
#include "app_SkeletonAGV.h"
#include "app_AGVAutorunAltitude.h"
/***************************************************************************************
** 函数名称: AGV_AltitudeStaCheck
** 功能描述: 根据路径信息和当前AGV状态，确定要不要切换至高度控制状态
** 参    数: 
** 返 回 值: res 0-无需切换状态，1-切换高度控制
****************************************************************************************/
uint8_t AGV_AltitudeStaCheck(AGVFunctionSta_TypeDef AGV_FuncSta_Next)
{
	uint8_t AltitudeRes = 0;
	/*如果上一次是调整高度的精定位，这次就不调姿态了，等举升结束后，自然会调整姿态*/
	if(	  (g_HostData.AGVGetGoodsAccPosMode == AGVAccPosMode_QR_SideAbove)
		&&AGV_FuncSta_Next == AGV_FunctionSta_AccuratePosition
		&&pPathStation->PathSequence != 0X01)
	{
		/*什么都不做*/
	}
	else if( (abs(pPathStation->LiftHigh - g_HostData.ActualHigh)>(g_HostData.Para_Index.AGV_DeadZoneHighMax)
		&& pPathStation->LiftHigh != 0xFFFF) 
		 )
	{
		/*更新高度相关内容*/
		if(pPathStation->LiftHigh != 0xFFFF)
		{
			g_HostData.TargetHigh = pPathStation->LiftHigh;
		}
		/*2024年7月29日-此处if判断不是很有必要，应该直接赋值；
						理论上应该永远无法进入else 等369项目验证后再优化*/
		else
		{
			g_HostData.TargetHigh = 0;
		}
		g_HostData.TargetAdjustHigh = abs(g_HostData.TargetHigh-g_HostData.ActualHigh);
		g_HostData.StartAdjustHigh = g_HostData.ActualHigh;
		AltitudeRes = AltitudeRes|0x01;
//				AGV_FuncSta_Next = AGV_FunctionSta_Altitude;
//				break;
	}
	/*2024年7月29日-如果调度系统下发的高度控制命令不通过，将目标高度刷新成当前实际高度，防止其他流程的错误响应*/
	else
	{
		g_HostData.TargetHigh = g_HostData.ActualHigh;
	}
	return AltitudeRes;
}
/***************************************************************************************
** 函数名称: AGV_TskFunctionStaAltitude
** 功能描述: 
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_TskFunctionStaAltitude(void)
{
	int16_t Altitude_Dis 		= 0;
	int16_t TargetAdjustHigh	= 0;
	int16_t CurrentAdjustHigh	= 0;
	float	Altitudet_K			= 0;
	int16_t TargetAdjustHigh_02	= 0;
	int16_t TargetAdjustHigh_08	= 0;
	int16_t Altitude_SlownDis	= 0;
	uint8_t 	res = 0;
	/*确保底盘程序已经将模式切换成了升降状态，否则这个函数直接返回即可*/
	{
		if((g_HostData.ActualAGVWorkMode != AGVMode_GoUpAndDown))
		{
			g_HostData.TargetAGVWorkMode = AGVMode_GoUpAndDown;
			return;
		}
	}
	
	/*1.获取需要调节的高度信息*/
	TargetAdjustHigh	= g_HostData.TargetHigh-g_HostData.ActualHigh ;
	CurrentAdjustHigh	= abs(TargetAdjustHigh);
	Altitudet_K			= CurrentAdjustHigh/(TargetAdjustHigh);
	Altitude_Dis 		= g_HostData.Para_Index.AGV_Altitude_Dis;
	Altitude_SlownDis	= g_HostData.Para_Index.AGV_Altitude_SlownDis;
	TargetAdjustHigh	= g_HostData.TargetAdjustHigh;
	CurrentAdjustHigh	= abs(g_HostData.StartAdjustHigh - g_HostData.ActualHigh);

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
		int16_t AdjustHigh	= g_HostData.TargetHigh - g_HostData.ActualHigh;
		if(AdjustHigh < (-1)*g_HostData.Para_Index.AGV_DeadZoneHighMin)
		{
			g_HostData.AGVHandleButtom = 2;
		}
		else if(AdjustHigh > g_HostData.Para_Index.AGV_DeadZoneHighMin)
		{
			g_HostData.AGVHandleButtom = 1;
		}
		/*4.升降按钮值为0的时候，完成姿态控制功能，状态机跳转*/
		else
		{
			g_HostData.AGVHandleButtom = 0;
			res = 0;
			g_HostData.AGV_FuncSta = AGV_FunctionSta_Init;
		}
	}
	/*A.启动停止按键响应，按键暂停时对升降按钮状态进行清除*/	
	if(g_HostData.InputPauseTaskFlag)
	{
		g_HostData.AGVHandleButtom = 0;
	}
	
	g_HostData.AdjustRunSta = res;
}



/************************************END OF FILE************************************/
