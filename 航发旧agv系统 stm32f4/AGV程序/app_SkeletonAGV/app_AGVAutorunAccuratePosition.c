/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_AGVAutorunAccuratePosition.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.xx.xx
  * Description: AGV自动运行的框架代码
  *******************************************************************************/
#include "app_SkeletonAGV.h"
#include "app_AGVAutorunAccuratePosition.h"

/***************************************************************************************
** 函数名称: AGV_AccPosFixQR_Z_FlagCheck
** 功能描述: 判断是不是要进行二维码Z轴精定位
** 参    数: None
** 返 回 值: 0-不需要进行精定位		其他-需要进行精定位
****************************************************************************************/
uint8_t AGV_AccPosFixQR_Z_FlagCheck(void)
{
	/*A.整车差速轮系	B.识别到了工装二维码	C.工装二维码z轴偏差大于AGV自转到位的死区上限*/
	if(	  g_HostData.FixtureQRData.PintSta != 0X00
		&&fabs(g_HostData.FixtureQRData.Offset_z) > g_HostData.Para_Index.AGV_AngAdjZoneMax
		&&g_HostData.ActualHigh <= 10
	)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}
/***************************************************************************************
** 函数名称: AGV_AccPosMode_ParkQR_XZ_FlagCheck
** 功能描述: 判断是不是要进行工装二维码放物料时的X Z轴精定位
** 参    数: None
** 返 回 值: 0-不需要进行精定位		其他-需要进行精定位
****************************************************************************************/
uint8_t	ParkQR_XZ_Sta = 0;
__weak uint8_t AGV_AccPosMode_ParkQR_XZ_FlagCheck(void)
{
    u8 Res = 0;
	//uint16_t New_Height = pPathStation->LiftHigh;
	static uint16_t ParkQR_XZ_Sequence = 0xFF;
    if((ParkQR_XZ_Sequence != g_HostData.PathCurrentPointNum)
			&&pPathStation->PathWayPointRetain[22] == 1)
	{
		pPathStation->PathWayPointRetain[22] = 0;
		ParkQR_XZ_Sequence = g_HostData.PathCurrentPointNum;
		ParkQR_XZ_Sta = 1;
		Res = 1;
	}
	else
	{
		ParkQR_XZ_Sequence = 0xFF;
		Res = 0;
	}
    return Res;
}
/***************************************************************************************
** 函数名称: AGV_AccPosQR_SideAbove_FlagCheck
** 功能描述: 判断是不是要进行侧面+上面的对接面精定位
** 参    数: None
** 返 回 值: 0-不需要进行精定位		其他-需要进行精定位
****************************************************************************************/
__weak uint8_t AGV_AccPosQR_SideAbove_FlagCheck(void)
{
	return 0;
}
/***************************************************************************************
** 函数名称: AGV_AccuratePositionStaCheck
** 功能描述: 根据路径信息和当前AGV状态，确定要不要切换至精定位状态
** 参    数: 
** 返 回 值: res 0-无需切换状态，其他-切换至精定位状态
****************************************************************************************/
uint8_t AGV_AccuratePositionStaCheck(void)
{
	uint8_t AccPosRes = 0X00;
	g_HostData.AGVAccPoNowsMode = g_HostData.GoodsFlag==0X00?g_HostData.AGVGetGoodsAccPosMode:g_HostData.AGVPutGoodsAccPosMode;
	switch (g_HostData.AGVAccPoNowsMode)
	{
		/*初始状态*/
		default:
		case AGVSpecialCommand_NULL:{}break;
		/*是否进行工装二维码纠正Z的精定位判断*/
		case AGVAccPosMode_FixQR_Z:
		{
			AccPosRes = AGV_AccPosFixQR_Z_FlagCheck();
		}
		break;
		/*此精定位功能主要是在末端时结合进行，无需单独进行精定位控制*/
		case AGVAccPosMode_ParkQR_XZ:
		{
			AccPosRes = AGV_AccPosMode_ParkQR_XZ_FlagCheck();
		}
		break;
		/*是否侧面对接的精定位判断*/
		case AGVAccPosMode_QR_SideAbove:
		{
			AccPosRes = AGV_AccPosQR_SideAbove_FlagCheck();
		}
		break;
	}
	return AccPosRes;
}
/***************************************************************************************
** 函数名称: AGV_AccPosMode_FixQR_Z
** 功能描述: 利用工装二维码纠正Z 使得取物料时没有Z偏差
** 参    数: 无
** 返 回 值: res		0-定位结束，到位		1-定位中
****************************************************************************************/
uint8_t AGV_AccPosMode_FixQR_Z(void)
{
	int8_t	res	= 0;
	float	AccPos_K	= 0;

	/*确保底盘程序已经将模式切换成了自转状态，否则返回定位中*/
	if(g_HostData.ActualAGVWorkMode != AGVMode_GoRotation)
	{
		res = 0x02;
	}		
	/*以5%的摇杆速度自转，直至工装二维码Z轴偏差小于指定值*/
	else if( fabs(g_HostData.FixtureQRData.Offset_z) > g_HostData.Para_Index.AGV_AngAdjZoneMin)
	{
		AccPos_K = 0.005*(g_HostData.FixtureQRData.Offset_z>0? (-1):(1));
		res = 0x01;
	}
	else
	{
		AccPos_K = 0;
		res = 0x00;
	}
	/*修改控制速度速度*/
	g_HostData.ActualPostureVelocity = AccPos_K*V_H;
	return res;
}
/***************************************************************************************
** 函数名称: AGV_AccPosMode_QR_SideAbove
** 功能描述: 侧面对接的精定位功能;这里只是弱函数实现，真正的函数功能在其他地方实现
** 参    数: None
** 返 回 值: 0-精定位完成		其他-精定位中
****************************************************************************************/
__weak uint8_t AGV_AccPosMode_QR_SideAbove(void)
{
	return 0;
}
/***************************************************************************************
** 函数名称: AGV_AccPosMode_ParkQR_XZ
** 功能描述: 侧面对接的精定位功能;这里只是弱函数实现，真正的函数功能在其他地方实现
** 参    数: None
** 返 回 值: 0-精定位完成		其他-精定位中
****************************************************************************************/
__weak uint8_t AGV_AccPosMode_ParkQR_XZ(void)
{
	return 0;
}

/***************************************************************************************
** 函数名称: AGV_TskFunctionStaAccuratePosition
** 功能描述: 
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_TskFunctionStaAccuratePosition(void)
{
	uint8_t AccuratePositionRunSta = 0x00;

	switch (g_HostData.AGVAccPoNowsMode)
	{
		/*初始状态*/
		default:
		case AGVAccPosMode_NULL:{}break;
		/*整车差速下 利用工装二维码纠正Z 使得取物料时没有Z偏差*/
		case AGVAccPosMode_FixQR_Z:
		{
			/*AGV Z轴对正工装二维码Z轴需要将AGV切换至自转状态*/
			g_HostData.TargetAGVWorkMode = AGVMode_GoRotation;
			if(g_HostData.FixtureQRData.PintSta != 0X00)
			{
				/*0-定位结束，到位		1-定位中
				  函数内部会自动修改自转的实际速度*/
				AccuratePositionRunSta = AGV_AccPosMode_FixQR_Z();
				/*对遥控底盘输出摇杆控制命令;*/
				g_HostData.Hxyz[2] = g_HostData.ActualPostureVelocity;
			}
			else
			{
				g_HostData.Hxyz[2] = 0;
				g_HostData.ActualPostureVelocity= 0;
			}
		}
		break;
		/*前后舵轮下 利用地标二维码纠正X和Z 使得放物料时没有X和Z偏差*/
		case AGVAccPosMode_ParkQR_XZ:
		{
			AccuratePositionRunSta = AGV_AccPosMode_ParkQR_XZ();
		}
		break;
		/*麦轮下 利用侧面一前一后2个PGV和侧面测距传感器 使得AGV侧面平行对接设备并且平台平行对接设备*/
		case AGVAccPosMode_QR_SideAbove:
		{
			AccuratePositionRunSta = AGV_AccPosMode_QR_SideAbove();
		}
		break;
		
	}
	if(AccuratePositionRunSta == 0x00)
	{
		g_HostData.AGV_FuncSta = AGV_FunctionSta_Init;
		if(g_HostData.AGVNavigateMode == AGVNavigateMode_Magnetic)
		{
			g_HostData.PosturRunSta = 4;
		}
		g_HostData.ActualForwardVelocity = 0.0f;
		g_HostData.ActualPostureVelocity = 0.0f;
		memset(g_HostData.Hxyz,0,sizeof(g_HostData.Hxyz));
	}
	else
	{
		g_HostData.Hxyz[1] = g_HostData.ActualForwardVelocity;
		g_HostData.Hxyz[2] = g_HostData.ActualPostureVelocity;
	}
}


/************************************END OF FILE************************************/
