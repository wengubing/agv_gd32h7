/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_AGVAutorunSpecialCommand.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.xx.xx
  * Description: AGV自动运行的框架代码
  *******************************************************************************/
#include "app_SkeletonAGV.h"
#include "app_AGVAutorunSpecialCommand.h"




/***************************************************************************************
** 函数名称: AGV_CylinderButtJointFlagCheck
** 功能描述: 是否需要进行上平台对接的判断;这里只是弱函数实现，真正的函数功能在其他地方实现
** 参    数: None
** 返 回 值: 0-不需要进行对接		其他-需要进行对接
****************************************************************************************/
__weak uint8_t AGV_CylinderButtJointFlagCheck(void)
{
	return 0;
}
/***************************************************************************************
** 函数名称: AGV_ComplexPlatformLiftFlagCheck
** 功能描述: 是否需要进行复数平台升降控制的判断;这里只是弱函数实现，真正的函数功能在其他地方实现
** 参    数: None
** 返 回 值: 0-不需要进行对接		其他-需要进行对接
****************************************************************************************/
__weak uint8_t AGV_ComplexPlatformLiftFlagCheck(void)
{
	return 0;
}

/***************************************************************************************
** 函数名称: AGV_SpecialCommandStaCheck
** 功能描述: 根据路径信息和当前AGV状态，确定要不要切换至上平台特殊功能控制状态
** 参    数: 
** 返 回 值: res 0-无需切换状态，1-切换至上平台特殊功能控制
****************************************************************************************/
uint8_t AGV_SpecialCommandStaCheck(void)
{
	uint8_t SpecialCommandRes = 0X00;
	switch (g_HostData.AGVSpecialCommandMode)
	{
		/*初始状态*/
		default:
		case AGVSpecialCommand_NULL:{}break;
		/*是否进行滚筒对接判断*/
		case AGVSpecialCommand_CylinderButtJoint:
		{
			SpecialCommandRes = AGV_CylinderButtJointFlagCheck();
		}break;
		/*是否进行复数平台控制判断*/
		case AGVSpecialCommand_ComplexPlatformLift:
		{
			SpecialCommandRes = AGV_ComplexPlatformLiftFlagCheck();
		}break;
	}
	return SpecialCommandRes;
}

/***************************************************************************************
** 函数名称: AGV_TskFunctionStaSpecialCommand
** 功能描述: 上平台的滚筒对接功能;这里只是弱函数实现，真正的函数功能在其他地方实现
** 参    数: None
** 返 回 值: 0-功能完成，滚筒对接完成		其他-滚筒作业中
****************************************************************************************/
__weak uint8_t AGV_CylinderButtJoint(void)
{
	return 0;
}
/***************************************************************************************
** 函数名称: AGV_ComplexPlatformLift
** 功能描述: 上平台的复数平台升降功能;这里只是弱函数实现，真正的函数功能在其他地方实现
** 参    数: None
** 返 回 值: 0-功能完成，		其他-作业中
****************************************************************************************/
__weak uint8_t AGV_ComplexPlatformLift(void)
{
	return 0;
}
/***************************************************************************************
** 函数名称: AGV_TskFunctionStaSpecialCommand
** 功能描述: 
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_TskFunctionStaSpecialCommand(void)
{
	uint8_t SpecialCommandRunSta = 0x00;
	switch (g_HostData.AGVSpecialCommandMode)
	{
		/*初始状态*/
		default:
		case AGVSpecialCommand_NULL:{}break;
		/**/
		case AGVSpecialCommand_CylinderButtJoint:
		{
			SpecialCommandRunSta = AGV_CylinderButtJoint();

		}break;
		/**/
		case AGVSpecialCommand_ComplexPlatformLift:
		{
			SpecialCommandRunSta = AGV_ComplexPlatformLift();

		}break;
	}
	if(SpecialCommandRunSta == 0x00)
	{
		g_HostData.AGV_FuncSta = AGV_FunctionSta_Init;
	}
}


/************************************END OF FILE************************************/
