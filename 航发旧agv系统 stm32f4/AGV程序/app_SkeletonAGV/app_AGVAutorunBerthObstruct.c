/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_AGVAutorunBerthObstruct.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.xx.xx
  * Description: AGV自动运行的框架代码
  *******************************************************************************/
#include "app_SkeletonAGV.h"
#include "app_AGVAutorunBerthObstruct.h"
/***************************************************************************************
** 函数名称: AGV_BerthObstructStaCheck
** 功能描述: 根据路径信息和当前AGV状态，确定要不要切换至停靠锁定状态
** 参    数: 
** 返 回 值: res 0-无需切换状态，1-切换至停靠锁定控制
****************************************************************************************/
uint8_t AGV_BerthObstructStaCheck(void)
{
	static uint16_t ConfirmSequence = 0;
	uint8_t BerthObstructRes = 0;
	if(		pPathStation->ConfirmSignal == true
		//&&	g_HostData.WaitConfirmSignal == false
		&&ConfirmSequence != pPathStation->PathSequence)
	{
		//g_HostData.WaitConfirmSignal = true;
		ConfirmSequence = pPathStation->PathSequence;
		BerthObstructRes = BerthObstructRes|0X01;
//		AGV_FuncSta_Next = AGV_FunctionSta_BerthObstruct;
//		break;
	}
	else
	{
		/*清除确认的标记*/
		ConfirmSequence = 0;
		g_HostData.InputConfirmFlag = 0X00;
	}
	return BerthObstructRes;
}


/***************************************************************************************
** 函数名称: AGV_TskFunctionStaBerthObstruct
** 功能描述: 
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_TskFunctionStaBerthObstruct(void)
{

	/*确保底盘程序已经将模式切换成了禁止状态，否则这个函数直接返回即可*/
	{
		if((g_HostData.ActualAGVWorkMode != AGVMode_GoStill))
		{
			g_HostData.TargetAGVWorkMode = AGVMode_GoStill;
			return;
		}
	}

	/*当外部输入了等待确认信号后，即可退出当前状态机*/
	if(g_HostData.InputConfirmFlag != 0X00)
	{
		g_HostData.InputConfirmFlag = 0X00;
		g_HostData.AGV_FuncSta = AGV_FunctionSta_Init;
	}
	else
	{
		/*无需操作*/
	}
}



/************************************END OF FILE************************************/
