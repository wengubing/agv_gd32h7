/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_AGVInit.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.xx.xx
  * Description: AGV自动运行的框架代码的初始化
  *******************************************************************************/
#define  MAGNETI_CVITURL_GLOBALS

#include "app_SkeletonAGV.h"

/***************************************************************************************
** 函数名称:		app_SkeletonAGV_FunInit
** 功能描述:		自动运行的功能设置函数
** 参    数:	无
** 返 回 值:		无
****************************************************************************************/
__weak void app_SkeletonAGV_FunInit(void)
{
	/*底盘类型：舵轮车*/
	g_HostData.AGVChassisMode = AGVChassisMode_DifSteeringWheel;
	/*导航方式：磁导航*/
	g_HostData.AGVNavigateMode = AGVNavigateMode_Magnetic;
	/*选用传感器有：无*/
	memset(g_HostData.InPutSensor,AGVInPutSensor_NULL,sizeof(g_HostData.InPutSensor));

	/*自动运行的刹车距离初始化*/
	{
		//提前减速，增加减速距离已减缓负载时停靠过冲
		float SlowDownDisPlan[4] 		= {500, 700, 900,1000 };
		float SlowDownDisPlan_Goods[4] 	= {600,1000,1200,1600 };
		memcpy(g_HostData.AGVWalkSlowDownDisPlan ,SlowDownDisPlan,sizeof(g_HostData.AGVWalkSlowDownDisPlan));
		memcpy(g_HostData.AGVGoodsSlowDownDisPlan, SlowDownDisPlan_Goods,sizeof(g_HostData.AGVGoodsSlowDownDisPlan));
	}
	/*停靠、取物料、放物料、充电停车	通过：虚拟里程计+十字磁条		进行减速判断*/
	g_HostData.AGVWalkCorrectMode		= AGVCorrection_ODO_Mag;
	g_HostData.AGVGetParkCorrectMode 	= AGVCorrection_ODO_Mag;
	g_HostData.AGVPutParkCorrectMode 	= AGVCorrection_ODO_Mag;
	g_HostData.AGVChargeCorrectMode 	= AGVCorrection_ODO_Mag;
	/*停靠、取物料、放物料、充电停车	通过：十字磁条				进行停车判断*/
	g_HostData.AGVWalkParkMode		= AGVParking_CrossMag;
	g_HostData.AGVGetParkGoodsMode	= AGVParking_CrossMag;
	g_HostData.AGVPutParkGoodsMode	= AGVParking_CrossMag;
	g_HostData.AGVChargeParkMode	= AGVParking_CrossMag;
	/*自转							通过：十字磁条+虚拟里程计		进行到位判断*/
	g_HostData.AGVPosture = AGVPosture_ODO_Mag;
	/*取放工装的时候，没有精定位*/
	g_HostData.AGVGetGoodsAccPosMode = AGVAccPosMode_NULL;
	g_HostData.AGVPutGoodsAccPosMode = AGVAccPosMode_NULL;

	/*特殊点位盲举				功能：关闭*/
	g_HostData.AGVPendantFun.BlindLift 			= 0x00;
	/*弧线路径					功能：关闭*/
	g_HostData.AGVPendantFun.ArcWalk 			= 0x00;
	/*悬挂调节					功能：关闭*/
	g_HostData.AGVPendantFun.HangControl		= 0x00;
	/*放工装时纠XZ				功能：关闭*/
	g_HostData.AGVPendantFun.PutGoodsQRMag 		= 0x00;
	/*撤销任务立即停止所有动作	功能：关闭*/
	g_HostData.AGVPendantFun.RepealTaskStopMove	= 0x00;
	/*车端暂停任务				功能：开启*/
	g_HostData.AGVPendantFun.PauseTask 			= 0x01;
	/*支腿控制					功能：关闭*/
	g_HostData.AGVPendantFun.LegTelescopic		= 0x00;
}

/***************************************************************************************
** 函数名称:		app_SkeletonAGV_ParaIndexInit
** 功能描述:		自动运行的参数设置函数
** 参    数: 	无
** 返 回 值:	 	无
****************************************************************************************/
__weak void app_SkeletonAGV_ParaIndexInit(void)
{

}


/************************************END OF FILE************************************/

