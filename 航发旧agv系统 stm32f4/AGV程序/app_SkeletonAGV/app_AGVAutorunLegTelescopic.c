/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_AGVAutorunLegTelescopic.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.xx.xx
  * Description: AGV自动运行的框架代码
  *******************************************************************************/
#include "app_SkeletonAGV.h"
#include "app_AGVAutorunLegTelescopic.h"
/***************************************************************************************
** 函数名称: AGV_LegTelescopiStaCheckc
** 功能描述: 根据路径信息和当前AGV状态，确定要不要切换至支腿控制状态
** 参    数: 
** 返 回 值: res 0-无需切换状态，1-切换至支腿控制
****************************************************************************************/
uint8_t AGV_LegTelescopiStaCheck(void)
{
	return 0;
}
/***************************************************************************************
** 函数名称: AGV_TskFunctionStaLegTelescopic
** 功能描述: 
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_TskFunctionStaLegTelescopic(void)
{
	/*确保底盘程序已经将模式切换成了支腿控制状态，否则这个函数直接返回即可*/
	{
		if((g_HostData.ActualAGVWorkMode != AGVMode_LegTelescopic))
		{
			g_HostData.TargetAGVWorkMode = AGVMode_LegTelescopic;
			return;
		}
	}
	/*当前自动代码只考虑把支腿收回，不考虑什么时候伸出支腿*/
	if(g_HostData.LegTelescopicSta != 0X01)
	{
		g_HostData.AGVLegButtom = 0X02;
	}
	else
	{
		g_HostData.AGVLegButtom = 0X00;
		g_HostData.AGV_FuncSta = AGV_FunctionSta_Init;
	}
}



/************************************END OF FILE************************************/
