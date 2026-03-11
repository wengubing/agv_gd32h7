/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_SkeletonAGV.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.xx.xx
  * Description: AGV自动运行的框架代码
  *******************************************************************************/
#define  APP_SKELETONAGV_GLOBALS

#include "app_SkeletonAGV.h"
#include "app_AGVAutorunWalk.h"
#include "app_AGVAutorunPosture.h"
#include "app_AGVAutorunAltitude.h"
#include "app_AGVAutorunHangHigh.h"
#include "app_AGVAutorunLegTelescopic.h"
#include "app_AGVAutorunAccuratePosition.h"
#include "app_AGVAutorunSpecialCommand.h"
#include "app_AGVAutorunBerthObstruct.h"

HostCtrl_DATA g_HostData;
/*如果是文小兵的地盘框架，替换掉DHR的依赖并且在外部进行初始化*/
#if defined(ACC_WXB) 
uint16_t *g_DHR_AGVInformation = NULL;
#else
extern uint16_t  DHR[];
uint16_t *g_DHR_AGVInformation = &DHR[R_AGV_PARA_START_ADDR];
#endif


/***************************************************************************************
** 函数名称: AGV_ClearActionBuffer
** 功能描述: 清除遥控器动作缓存
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_ClearActionBuffer(void)
{
	memset(g_HostData.Hxyz,0,sizeof(g_HostData.Hxyz));	/*清空摇杆的输出命令*/

}
/***************************************************************************************
** 函数名称: AGV_ClearInputCmdBuffer
** 功能描述: 清除输入缓存
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_ClearInputCmdBuffer(void)
{
	//0速停止行走电机	
}

/***************************************************************************************
** 函数名称: InitHostData
** 功能描述: 上位机控制相关数据清空
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void InitHostData(void)
{
	g_HostData.NextPath_X = 0;
	g_HostData.NextPath_Y = 0;

//	g_HostData.CurrentPath_X = 0;
//	g_HostData.CurrentPath_Y = 0;

	g_HostData.PointToStart_Dis = 0;
	g_HostData.PointToEnd_Dis = 0;

	g_HostData.CurrentDelta_X = 0;
	g_HostData.CurrentDelta_Y = 0;
	g_HostData.CurrentPathDelta = 0;
	g_HostData.CurrentPathDelta_Half = 0;

	g_HostData.AGVRunDir = AGVRunDir_NULL;
}
/***************************************************************************************
** 函数名称: AGV_TskErrCheck
** 功能描述: 检查自动运行过程中，是否存在什么故障
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_TskErrCheck(void)
{
	//static AGVFunctionSta_TypeDef 	Last_AGV_FuncSta = AGV_FunctionSta_Walk;			/*当状态机退出后，此变量可以捕捉相关变化；此函数中的状态机是即将进入的控制流程*/

	switch (g_HostData.AGV_FuncSta)
	{
		/*初始状态*/
		case AGV_FunctionSta_Init:
		{		
			/*点位不匹配——磁导航的情况下，如果十字磁条停车了 但是RFID标签不吻合*/
			{
				if(
					(g_HostData.AGVNavigateMode == AGVNavigateMode_Magnetic)
					&&g_HostData.RFIDData.ReadStatus != 0X00
					&&g_HostData.CurrentPath_Tag != 0X00
					&&g_HostData.RFIDData.TagValue != g_HostData.CurrentPath_Tag
				)
				{
					/*走错了报故障*/
					M_K_SET_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_PathPointNotMatch);
				}
				else
				{
					M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_PathPointNotMatch);
				}
			}
			/*点位不匹配——二维码导航的情况下，如果二维码停车了 但是标签值不吻合*/
			{
				if(
					g_HostData.AGVNavigateMode == AGVNavigateMode_QR
					&&g_HostData.ParkQRData.PintSta != 0X00
					&&g_HostData.CurrentPath_Tag 	!= 0X00
					&&g_HostData.ParkQRData.QR_Tag	!= g_HostData.CurrentPath_Tag
				)
				{
					/*走错了报故障*/
					M_K_SET_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_PathPointNotMatch);
				}
				else
				{
					M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_PathPointNotMatch);
				}
			}
			/*自动时，不在点位上——二维码导航情况下，执行第一个路径时不在二维码上*/
			{
				if(
					g_HostData.AGVNavigateMode == AGVNavigateMode_QR
					&&g_HostData.ParkQRData.PintSta == 0X00
					&&g_HostData.CurrentPath_Tag 	!= 0X00
					&&pPathStation->PathSequence	== 1
				)
				{
					/*走错了报故障*/
					M_K_SET_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_NotFindTag);
				}
				else
				{
					M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_NotFindTag);
				}
			}
		}break;
		/*行走控制状态*/
		case AGV_FunctionSta_Walk:
		{
			/*自动时，Slam导航下 横向偏移过大——报巡线偏差过大*/
			{
				if(g_HostData.AGVNavigateMode == AGVNavigateMode_SLAM)
				{
					uint8_t err = 0X00;
					
					if((g_HostData.AGV_Course == AGVCourse_Y_Positive || g_HostData.AGV_Course == AGVCourse_Y_Negative)
						&&(g_HostData.AGVParkZeroOffset_X < -300||g_HostData.AGVParkZeroOffset_X > 300))
					{
						 err = 0X01;
					}
					else if((g_HostData.AGV_Course == AGVCourse_X_Positive || g_HostData.AGV_Course == AGVCourse_X_Negative)
						&&(g_HostData.AGVParkZeroOffset_Y < -300||g_HostData.AGVParkZeroOffset_Y > 300))
					{
						err = 0X02;
					}
					if(err)
					{
						/*走偏了报故障*/
						M_K_SET_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_LateralDeviationOver);
					}
					else
					{
						M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_LateralDeviationOver);
					}
					
				}
				else
				{
					M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_LateralDeviationOver);
				}
			}
			
			/*依据导航方式的不同,将传感器数据输入到自动运行模块*/
			{
				uint32_t Delta =  0;
				switch(g_HostData.AGVNavigateMode)
				{
					case AGVNavigateMode_Magnetic:	/*磁条导航*/
					{
						/*停车检测超距——超过里程计仍未停车报警*/
						if(   g_HostData.WalkStopFlag != 0
							&&g_HostData.PointToStart_Dis > g_HostData.CurrentPathDelta*g_HostData.Para_Index.AGV_WalkDisOverrun_K)
						{
							/*走偏了报故障*/
							M_K_SET_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_NavigationFail);
						}
						else
						{
							M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_NavigationFail);
						}
					}
					break;
					default:
					case AGVNavigateMode_NULL:		/*初始化*/
					case AGVNavigateMode_QR:
					case AGVNavigateMode_GPS:		/*GPS导航，暂时和SLAM不做区分*/
					case AGVNavigateMode_SLAM:		/*激光导航 需要地标二维码、真实里程计*/
					{
						Delta =  g_HostData.CurrentPathDelta+500;
						/*停车检测超距——超过里程计仍未停车报警*/
						if(   g_HostData.PointToStart_Dis > Delta)
						{
							/*走偏了报故障*/
							M_K_SET_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_NavigationFail);
						}
						else if(g_HostData.PointToStart_Dis <= Delta-200)
						{
							M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_NavigationFail);
						}
						else
						{
							
						}
					}
					break;
				}
			}
			
			

			/*转运中，工装移动——转运中相比转运开始前，工装 x、y、z方向的偏移大过设定阈值；报警阈值可以修改*/
			if(g_HostData.GoodsFlag == 0x01 && g_HostData.Para_Index.AGV_GoodsMove_En != 0X00)
			{
				static uint32_t  GoodsMoveErrTime = 0;
				static uint8_t   GoodsMoveErrNum = 0;
				if(g_HostData.Sys1msCounter >= GoodsMoveErrTime+500)
				{
					uint8_t res = 0x00;
					if(abs(g_HostData.FixtureQRData.QR_x-g_HostData.GoodsQRData.QR_x) >= g_HostData.Para_Index.AGV_GoodsMove_xLimit)
					{
						res = res|0x01;
					}
					if(abs(g_HostData.FixtureQRData.QR_y-g_HostData.GoodsQRData.QR_y) >= g_HostData.Para_Index.AGV_GoodsMove_yLimit)
					{
						res = res|0x02;
					}
					if(fabs(g_HostData.FixtureQRData.QR_z-g_HostData.GoodsQRData.QR_z) >= g_HostData.Para_Index.AGV_GoodsMove_zLimit)
					{
						res = res|0x04;
					}
					if(g_HostData.FixtureQRData.PintSta == 0X00)
					{
						res = res|0x08;
					}

					if(res != 0x00)
					{
						GoodsMoveErrNum++;
					}
					else
					{
						GoodsMoveErrNum = 0;
					}
					if(GoodsMoveErrNum >= 3)
					{
						GoodsMoveErrNum = 3;
						M_K_SET_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_GoodsMoveErr);
					}
					else
					{
						M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_GoodsMoveErr);
					}
				}
			}
			else
			{
				M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_GoodsMoveErr);
			}
			
		}
		break;
		/*精定位状态	*/
		case AGV_FunctionSta_AccuratePosition:
		{
			
		}break;
		/*姿态控制状态*/
		case AGV_FunctionSta_Posture:
		{
			/*旋转过程丢失磁条——目标旋转90°；实际旋转了90°*1.2都还没发现磁条的报警；*/
			if(   g_HostData.AGVPosture == AGVPosture_ODO_Mag
				&&g_HostData.ActualPostureAngle >= g_HostData.TargetPostureAngle*1.2f)
			{
				/*旋转过程中丢失定位报故障*/
				M_K_SET_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_MagPostureErr);
			}
			else
			{
				M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_MagPostureErr);
			}
		}break;
		/*高度控制状态*/
		case AGV_FunctionSta_Altitude:
		{
			/*取工装时，偏移过大——PGV读出的x偏移加上z轴偏移映射到磁条上 超过磁条量程的30%；报警阈值可以修改*/
			{
				uint8_t res = 0x00;
				if(g_HostData.AGVPendantFun.PutGoodsQRMag == 0X01)
				{
					if(g_HostData.GoodsQRData.flag	== 0x02)
					{
						res = res|0x10;
					}
					if(g_HostData.AGVMagParkZeroOffset.Flag	== 0x01)
					{
						res = res|0x20;
					}
					if(g_HostData.AGVMagParkZeroOffset.Flag	== 0x02 ||g_HostData.AGVMagParkZeroOffset.Flag == 0x04)
					{
						if(abs(g_HostData.AGVMagParkZeroOffset.Mag_F) >= g_HostData.Para_Index.AGV_MagParkZero_Limit)
						{
							res = res|0x01;
						}
						if(abs(g_HostData.AGVMagParkZeroOffset.Mag_B) >= g_HostData.Para_Index.AGV_MagParkZero_Limit)
						{
							res = res|0x02;
						}
						if(abs(g_HostData.AGVMagParkZeroOffset.Mag_L) >= g_HostData.Para_Index.AGV_MagParkZero_Limit)
						{
							res = res|0x04;
						}
						if(abs(g_HostData.AGVMagParkZeroOffset.Mag_R) >= g_HostData.Para_Index.AGV_MagParkZero_Limit)
						{
							res = res|0x08;
						}
					}
				}
				else{/*什么都不需要做*/}

				if(res != 0X00)
				{
					M_K_SET_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_LateralDeviationOver);
				}
				else
				{
					M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_LateralDeviationOver);
				}
			}
		}break;
		/*停靠锁定，等待解锁状态*/
		case AGV_FunctionSta_BerthObstruct:
		{
			
		}break;
		/*上平台特殊控制功能状态*/
		case AGV_FunctionSta_SpecialCommand:
		{
			
		}break;
		/*异常状态状态，保护即可*/
		default:
		{
			
		}
		break;
	}
	/*记录当前的状态*/
	if(g_HostData.AGV_FuncSta != AGV_FunctionSta_Init)
	{
		//Last_AGV_FuncSta = g_HostData.AGV_FuncSta;
	}
	else
	{
		/*什么都不需要做*/
	}
}


/***************************************************************************************
** 函数名称: AGV_FunctionStaSpecialDataUpdate
** 功能描述: 依赖于状态机改变的一些特殊数据更新
** 参    数: FuncSta_Now-当前状态 FuncSta_Next下一个状态
** 返 回 值: None
****************************************************************************************/
void AGV_FunctionStaSpecialDataUpdate(AGVFunctionSta_TypeDef FuncSta_Next)
{
	/*当状态机退出后，此变量可以捕捉跳开初始状态的相关变化；*/
	static AGVFunctionSta_TypeDef 	FuncSta_Now = AGV_FunctionSta_Init;
	/*标记加速*/
	{
		
        /*当前不是行走状态，下个状态是行走标记需要进行加速*/
		{
			if(	  FuncSta_Now != AGV_FunctionSta_Walk
				&&FuncSta_Next == AGV_FunctionSta_Walk)
			{
				g_HostData.StartAccFlag = 0;
			}
		}
        /*调度系统下发在速度增大了*/
        {
            static uint8_t last_CurrenPer = 0;
            if(last_CurrenPer != g_HostData.CurrentPer)
            {
                if(     g_HostData.WalkGoOnFlag == 0x01
                    &&  g_HostData.CurrentPer > last_CurrenPer)
                {
                    g_HostData.StartAccFlag = 0;
                }
                last_CurrenPer = g_HostData.CurrentPer;
            }
        }
		/*停车后，没有清除加速标志;马上又需继续走*/
		{
			if(	  g_HostData.StartAccFlag != 0X00
				&&g_HostData.ActualForwardVelocity != g_HostData.TargetForwardVelocity
				&&FuncSta_Next == AGV_FunctionSta_Walk)
			{
				g_HostData.StartAccFlag = 0;
			}
		}
		
	}
	
	
	
	/*顶升前，计算纠偏*/
	{
		uint8_t res = 0x00;/*0-需要记录二维码值 其他-不满足记录的条件*/
		do
		{
			/*特殊功能开关，不废话直接更新*/
			if(g_HostData.MagParkQRData.flag == 0xAE){break;}
			/*功能开关没开，不用计算*/
			if(g_HostData.AGVPendantFun.PutGoodsQRMag != 0X01){res = 0x01;break;}		
			/*不是工装二维码取料，不用计算*/
			if(g_HostData.AGVGetParkGoodsMode != AGVParking_FixtureQR){res = 0x02;break;}
			/*有货物，已经顶升起来了，不用计算*/
			if(g_HostData.GoodsFlag != 0X00){res = 0x03;break;}
			/*接下来不是高度控制，不用计算*/
			if(FuncSta_Next != AGV_FunctionSta_Altitude){res = 0x04;break;}
		}while(0);
		/*上述判断通过，更新二维码的值*/
		if(res==0x00)
		{
			if(g_HostData.FixtureQRData.PintSta == 0X01||g_HostData.MagParkQRData.flag == 0xAE)
			{
				g_HostData.MagParkQRData.flag = 0x01;
				g_HostData.MagParkQRData.QR_x 		= g_HostData.FixtureQRData.QR_x;
				g_HostData.MagParkQRData.QR_y 		= g_HostData.FixtureQRData.QR_y;
				g_HostData.MagParkQRData.Offset_z	= g_HostData.FixtureQRData.Offset_z;
			}
			else
			{
				g_HostData.MagParkQRData.flag = 0x02;
			}
		}
		else
		{
			g_HostData.MagParkQRData.flag = 0x00;
		}
	}
	/*转运中，工装移动*/
	{
		uint8_t res = 0x00;/*0-需要记录二维码值 其他-不满足记录的条件*/
		do
		{
			/*特殊功能开关，不废话直接更新*/
			if(g_HostData.GoodsQRData.flag == 0xAE){break;}
			/*功能开关没开，不用计算*/
			if(g_HostData.AGVPendantFun.PutGoodsQRMag != 0X01){res = 0x01;break;}		
			/*不是工装二维码取料，不用计算*/
			if(g_HostData.AGVGetParkGoodsMode != AGVParking_FixtureQR){res = 0x02;break;}
			/*没有货物，还没顶升，不用计算*/
			if(g_HostData.GoodsFlag != 0X01){res = 0x03;break;}
			/*才结束的不是高度控制，不用计算*/
			if(FuncSta_Now != AGV_FunctionSta_Altitude){res = 0x04;break;}
			/*下一步进行的不是行走控制或者旋转，不用计算*/
			if(FuncSta_Next != AGV_FunctionSta_Walk || FuncSta_Next != AGV_FunctionSta_Posture){res = 0x05;break;}
		}while(0);

		/*上述判断通过，更新二维码的值*/
		if(res==0x00)
		{
			if(g_HostData.FixtureQRData.PintSta == 0X01||g_HostData.GoodsQRData.flag == 0xAE)
			{
				g_HostData.GoodsQRData.flag = 0x01;
				g_HostData.GoodsQRData.QR_x = g_HostData.FixtureQRData.QR_x;
				g_HostData.GoodsQRData.QR_y = g_HostData.FixtureQRData.QR_y;
				g_HostData.GoodsQRData.QR_z = g_HostData.FixtureQRData.QR_z;
			}
			else
			{
				g_HostData.GoodsQRData.flag = 0x02;
			}
		}
		else
		{
			g_HostData.GoodsQRData.flag = 0x00;
		}
	}
	/*记录状态机当前的状态*/
	{
		if(FuncSta_Next != AGV_FunctionSta_Init)
		{
			g_HostData.AGV_FuncStaLast = FuncSta_Now;
			FuncSta_Now = FuncSta_Next;
		}
		else
		{
			/*什么都不需要做*/
		}
	}
}

/***************************************************************************************
** 函数名称: AGV_CustomFunctionUpDate
** 功能描述: 定制功能的数据更新，需要先和调度系统约定 路径信息的某一位传递的数据含义
** 参    数: None
** 返 回 值: None
****************************************************************************************/
__weak void AGV_CustomFunctionUpDate(void * pData)
{
	//PathTypeDef *pPath = pData;
}
/***************************************************************************************
** 函数名称: AGV_Radio_Collision_Update
** 功能描述: 将调度系统下发的雷达状态进行更新
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_Radio_Collision_Update(void)
{
#if defined(ACC_WXB) 
	/*避障雷达使能*/
	g_HostData.AGVRadio.Front	= pPathStation->ForRadioEnalbe;
	g_HostData.AGVRadio.Back	= pPathStation->BackRadioEnalbe;
	g_HostData.AGVRadio.Left	= pPathStation->LeftRadioEnalbe;
	g_HostData.AGVRadio.Right	= pPathStation->RightRadioEnalbe;
	g_HostData.AGVRadio.Bevel	= pPathStation->BevelRadioEnalbe;
	g_HostData.AGVRadio.Column  = pPathStation->ColumnRadioEnalbe;
	/*获取当前路径的激光雷达区域选择*/
	g_HostData.AGVRadio.ScanningArea  = pPathStation->LidarScanAreaSelect;
	/*触边使能*/
	g_HostData.AGVCollision.Front	= pPathStation->ForCollisionEnable;
	g_HostData.AGVCollision.Back	= pPathStation->BackCollisionEnable;
	g_HostData.AGVCollision.Left	= pPathStation->LeftCollisionEnable;
	g_HostData.AGVCollision.Right	= pPathStation->RightCollisionEnable;
#else
	//前激光雷达								 
	g_HostData.CurrentForRadioEnalbe = pPathStation->ForRadioEnalbe;
	//后激光雷达								 
	g_HostData.CurrentBackRadioEnalbe = pPathStation->BackRadioEnalbe;
	//左激光雷达
	g_HostData.CurrentLeftRadioEnalbe = pPathStation->LeftRadioEnalbe;	
	//右激光雷达
	g_HostData.CurrentRightRadioEnalbe = pPathStation->RightRadioEnalbe;	
	//斜面激光雷达
	g_HostData.CurrentBevelRadioEnalbe = pPathStation->BevelRadioEnalbe;	
	//立柱激光雷达
	g_HostData.CurrentColumnRadioEnalbe = pPathStation->ColumnRadioEnalbe;	
	//前防撞触边
	g_HostData.CurrentForwardCollision = pPathStation->ForCollisionEnable;	
	//后防撞触边
	g_HostData.CurrentBackCollision = pPathStation->BackCollisionEnable;	
	//左防撞触边
	g_HostData.CurrentLeftCollision = pPathStation->LeftCollisionEnable;	
	//右防撞触边
	g_HostData.CurrentRightCollision = pPathStation->RightCollisionEnable;
	/*获取当前路径的激光雷达区域选择*/
	g_HostData.LidarScanningAreaSwitching = pPathStation->LidarScanAreaSelect;
#endif
	
}
/***************************************************************************************
** 函数名称: AGV_TskFunctionStaInit
** 功能描述: 
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_TskFunctionStaInit(void)
{
	static	AGVFunctionSta_TypeDef 	AGV_FuncSta_Next = AGV_FunctionSta_Init;	/*下一个点位应该做的功能，如果不是行走控制状态，就需要停车*/

	static PathTypeDef *p_NextPath;              //指针，指向路径链表数据的单个站点单元
	static PathTypeDef *p_LasetPath;              //指针，指向路径链表数据的单个站点单元

	p_NextPath = pPathStation->Pnext;

	/*0.这是最后一个点位或者错误停靠，自动任务结束*/
	if(g_HostData.LastPointFlag != 0x00)
	{
		g_HostData.AGVAutoMode = AutoMode_Stop;
		g_HostData.LastPointFlag = 0x00;
		return;
	}
	else if(g_HostData.WalkInPlacerRes == AGVWalkInPlacerRes_Err)
	{
		g_HostData.AGVAutoMode = AutoMode_Stop;
		g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_NULL;
	}

	do
	{
		/*需要持续清零的变量*/
		{
			g_HostData.EndPathFlag = 0x00;
			g_HostData.WalkStopFlag = 0x00;
			g_HostData.WalkSlowDownFlag = 0x00;
			//g_HostData.PosturRunSta = 0x00;
			g_HostData.AGVParkNowMode = g_HostData.AGVWalkParkMode;
			g_HostData.AGVCorrectNowMode = g_HostData.AGVWalkCorrectMode;
		}
		/*提取该路径点的避障激光雷达和防撞触边*/
		{
			AGV_Radio_Collision_Update();
		}
		/*用户数据更新，回调到外部实现*/
		{
			AGV_CustomFunctionUpDate(pPathStation);
		}
		
		/*通过高度判断当前有没有负载*/
		{
			if(g_HostData.ActualHigh > 15)
			{
				g_HostData.GoodsFlag =0X01;
			}
			else
			{
				g_HostData.GoodsFlag =0X00;
			}
		}
		
		
		/*1.通过标签是否一致判断是否满足点位要求
			需要的话跳转“行走控制状态”*/
		{
			uint8_t WalkSta = AGV_WalkStaCheck(p_NextPath,p_LasetPath);
			/*返回1，需要伸出支腿*/
			if(WalkSta == 0X01)
			{
				AGV_FuncSta_Next = AGV_FunctionSta_Walk;
				break;
			}
			/*返回2，需要响应行走控制*/
			else if(WalkSta == 0X02)
			{
				AGV_FuncSta_Next = AGV_FunctionSta_LegTelescopic;
				break;
			}
		}
		/*2.判断该点当前朝向和目标朝向的误差在运行范围内,
		    需要的话跳转“姿态控制状态”,此处确保斜向上线可以先将朝向调整了*/
		{
			uint8_t PostureSta = AGV_PostureStaCheck(AGV_FuncSta_Next);
			/*返回1，需要控制姿态*/
			if(PostureSta == 0X01)
			{
				AGV_FuncSta_Next = AGV_FunctionSta_Posture;
				break;
			}
		}
		/*3.特殊标志判断，是否需要进行精定位功能,需要的话跳转“精定位状态”*/
		{
			uint8_t AccPosSta = AGV_AccuratePositionStaCheck();
			/*精定位判断通过，直接退出当前的状态机*/
			if(AccPosSta != 0X00)
			{
				AGV_FuncSta_Next = AGV_FunctionSta_AccuratePosition;
				break;
			}
		}
		/*3.判断该点举升高度是否符合路径信息要求,0xFFFF的作用为调度系统下发该数值，说明AGV在该点不需要进行升降动作
		    需要的话跳转“悬挂控制状态”*/
		{
			uint8_t HangSta = AGV_HangStaCheck();
			/*返回1，需要控制姿态*/
			if(HangSta == 0X01)
			{
				AGV_FuncSta_Next = AGV_FunctionSta_Hang;
				break;
			}
		}
		/*4.判断该点举升高度是否符合路径信息要求,0xFFFF的作用为调度系统下发该数值，说明AGV在该点不需要进行升降动作
		    需要的话跳转“高度控制状态”*/
		{
			uint8_t AltitudeSta = AGV_AltitudeStaCheck(AGV_FuncSta_Next);
			/*返回1，需要控制姿态*/
			if(AltitudeSta == 0X01)
			{
				AGV_FuncSta_Next = AGV_FunctionSta_Altitude;
				break;
			}
		}


		/*5.特殊标志判断，是否需要进行项目特殊功能,需要的话跳转“上平台特殊控制功能状态”*/
		{
			uint8_t SpecialCommandSta = AGV_SpecialCommandStaCheck();
			if(SpecialCommandSta != 0X00)
			{
				AGV_FuncSta_Next = AGV_FunctionSta_SpecialCommand;
				break;
			}
		}

		/*6.判断当前这个点是否需要确认信号,需要的话跳转“停靠锁定，等待解锁状态”*/
		{
			uint8_t BerthObstructSta = AGV_BerthObstructStaCheck();
			if(BerthObstructSta != 0X00)
			{
				AGV_FuncSta_Next = AGV_FunctionSta_BerthObstruct;
				break;
			}
		}
		/*如果运行到这里，说明AGV已经达到P点的所有要求，路径信息更新至下个点位(还有下个点位的情况下)
		  更新条件：AGV满足当前P点的阻塞、朝向、高度、点位、精定位、上平台 6个方面的所有要求*/
		if(p_NextPath->PathSequence != 0)
		{
			p_LasetPath = pPathStation;
			pPathStation = pPathStation->Pnext;
			/*将当前执行的序列号 同步至自动运行外部*/
			g_HostData.PathCurrentPointNum = pPathStation->PathSequence;
		}
		else
		{
			g_HostData.LastPointFlag = 0x01;
		}
		/*P指针更新，连续行走标志复位*/
		if(g_HostData.WalkGoOnFlag == 0x01)
		{
			g_HostData.WalkGoOnFlag = 0x00;
		}
		/*如果是磁导航下的虚拟点位的里程计停车，是强制刷新了tag的因此需要在路径更新后清除相关调节*/
		if(   g_HostData.WalkParkFlag == AGVWalkParkFlag_OdoPark
			&&g_HostData.AGVNavigateMode == AGVNavigateMode_Magnetic
			&&g_HostData.VirtualPointFlag != 0X00)
		{
			g_HostData.WalkParkFlag = AGVWalkParkFlag_Park;
		}
		AGV_FuncSta_Next = AGV_FunctionSta_Init;
	}
	while(0);

	/*依赖于状态机改变的一些特殊数据更新*/
	AGV_FunctionStaSpecialDataUpdate(AGV_FuncSta_Next);

	/*状态机更新*/
	g_HostData.AGV_FuncSta = AGV_FuncSta_Next;

}
/***************************************************************************************
** 函数名称: Tsk_MasterAutoControl
** 功能描述: 自动控制函数
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void Tsk_AutoFunctionControl(void)
{
	/*故障判断功能*/
	{
		AGV_TskErrCheck();
	}
	/*自定运行功能状态机*/
	switch (g_HostData.AGV_FuncSta)
	{

		default:								/*异常状态状态，按初始状态执行即可*/
		case AGV_FunctionSta_Init:				/*初始状态*/
		{
			/*函数内会修改FuncSta状态值 然后到指定状态机里面继续执行*/
			AGV_TskFunctionStaInit();
		}
		break;

		case AGV_FunctionSta_Walk:				/*行走控制状态*/
		{
			AGV_TskFunctionStaWalk();
		}break;

		case AGV_FunctionSta_AccuratePosition:	/*精定位状态	*/
		{
			AGV_TskFunctionStaAccuratePosition();
		}break;

		case AGV_FunctionSta_Posture:			/*姿态控制状态*/
		{
			AGV_TskFunctionStaPosture();
		}break;

		case AGV_FunctionSta_Altitude:			/*高度控制状态*/
		{
			AGV_TskFunctionStaAltitude();
		}break;

		case AGV_FunctionSta_BerthObstruct:		/*停靠锁定，等待解锁状态*/
		{
			AGV_TskFunctionStaBerthObstruct();
		}break;

		case AGV_FunctionSta_SpecialCommand:	/*上平台特殊控制功能状态*/
		{
			AGV_TskFunctionStaSpecialCommand();
		}break;
		
		case AGV_FunctionSta_LegTelescopic:		/*支腿控制功能状态*/
		{
			AGV_TskFunctionStaLegTelescopic();
		}break;

		case AGV_FunctionSta_Hang:			/*悬挂高度控制功能状态*/
		{
			AGV_TskFunctionStaHang();
		}break;
		
		case AGV_FunctionSta_NavigationSwitch:			/*导航切换功能状态*/
		{
			if(g_HostData.NavigationSwitchResult == 0)
			{
				g_HostData.AGV_FuncSta = AGV_FunctionSta_Init;
			}
		}break;		
		
	}

}
/***************************************************************************************
** 函数名称: GetHostControl
** 功能描述: 获取上位机命令
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void GetHostControl(void)
{
    uint8_t res = 0;
	static Host_Control HostControlBuffer_Temp = Host_ControlInitial;
	static Host_Control HostControlBuffer_TempLast = Host_ControlInitial;
	
    //上获取位机命令
    if(g_DHR_AGVInformation[Para_Index_AGV_TaskControlCode] == Host_ControlInitial)
    {
        HostControlBuffer_Temp = Host_ControlInitial;
    }
    else if(g_DHR_AGVInformation[Para_Index_AGV_TaskControlCode] == Host_ControlStart)
    {
       HostControlBuffer_Temp = Host_ControlStart;
    }
    else if(g_DHR_AGVInformation[Para_Index_AGV_TaskControlCode] == Host_ControlPause)
    {
        HostControlBuffer_Temp = Host_ControlPause;
    }      
    else if(g_DHR_AGVInformation[Para_Index_AGV_TaskControlCode] == Host_ControlStop)
    {
        HostControlBuffer_Temp = Host_ControlStop;
    }   
    else if(g_DHR_AGVInformation[Para_Index_AGV_TaskControlCode] == Host_ControlAbort)
    {
        HostControlBuffer_Temp = Host_ControlAbort;
    } 
    else if(g_DHR_AGVInformation[Para_Index_AGV_TaskControlCode] == Host_ControlAutoRun)
    {
        HostControlBuffer_Temp = Host_ControlAutoRun;
    }    
    else
    {
        HostControlBuffer_Temp = Host_ControlInitial;    
    }

	//调度系统端暂停任务
	if(g_HostData.AGVPendantFun.PauseTask == 0X01)
	{
		if(	  HostControlBuffer_Temp == Host_ControlInitial
			||HostControlBuffer_Temp == Host_ControlStop
			||HostControlBuffer_Temp == Host_ControlAbort
			||HostControlBuffer_Temp == Host_ControlAutoRun
		)
		{
			g_HostData.HostControlBuffer = HostControlBuffer_Temp;
		}
		else
		{
			if(HostControlBuffer_TempLast != Host_ControlStart&&HostControlBuffer_Temp == Host_ControlStart)
			{
				res = 0x01;
			}
			else if(HostControlBuffer_TempLast != Host_ControlPause&&HostControlBuffer_Temp == Host_ControlPause)
			{
				res = 0x02;
			}
			else
			{
				
			}
		}
		HostControlBuffer_TempLast = HostControlBuffer_Temp;

		if(res == 0x02)
		{
			g_HostData.HostControlBuffer = Host_ControlPause;
			if(	g_DHR_AGVInformation[Para_Index_AGV_TaskControlCode] != Host_ControlPause)
			{
				g_DHR_AGVInformation[Para_Index_AGV_TaskControlCode] = Host_ControlPause;
			}
		}
		if(res == 0x01)
		{
			g_HostData.HostControlBuffer = Host_ControlStart;
			if(	  g_DHR_AGVInformation[Para_Index_AGV_TaskControlCode] != Host_ControlStart)
			{
				g_DHR_AGVInformation[Para_Index_AGV_TaskControlCode] = Host_ControlStart;
			}
		}
	}
	else
	{
		g_HostData.HostControlBuffer = HostControlBuffer_Temp;
	}
	/*获取当前任务类型*/
	{
		if(g_DHR_AGVInformation[Para_Index_AGV_TaskTypeNotify] == 1)
		{
			g_HostData.AGVTaskNotifyType = AGVTaskTypeNotify_Park;
		}
		else if(g_DHR_AGVInformation[Para_Index_AGV_TaskTypeNotify] == 2)
		{
			g_HostData.AGVTaskNotifyType = AGVTaskTypeNotify_Charge;
		}
		else if(g_DHR_AGVInformation[Para_Index_AGV_TaskTypeNotify] == 3)
		{
			g_HostData.AGVTaskNotifyType = AGVTaskTypeNotify_Goods;
		}
		else
		{
			g_HostData.AGVTaskNotifyType = AGVTaskTypeNotify_NULL;
		}
	}
}
/***************************************************************************************
** 函数名称: AGV_TaskPauseControlCallBack
** 功能描述: 任务暂停后，允许外部进行的控制命令清除
			（遥感和升降按钮和AGV模式切换自动框架内部已处理）
** 参    数: None
** 返 回 值: None
****************************************************************************************/
__weak void AGV_TaskPauseControlCallBack(void)
{
	return;
}
/***************************************************************************************
** 函数名称: AGV_TaskStopControlCallBack
** 功能描述: 任务停止（正常结束或者撤销）后，允许外部进行的控制命令清除
			（遥感和升降按钮和AGV模式切换自动框架内部已处理）
** 参    数: None
** 返 回 值: None
****************************************************************************************/
__weak void AGV_TaskStopControlCallBack(void)
{
	return;
}
/***************************************************************************************
** 函数名称: Tsk_MasterAutoControl
** 功能描述: 自动控制函数
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void Tsk_MasterAutoControl(void)
{
	/*将DHR中的内容，拷贝至函数内部的变量中*/
	//memcpy(g_DHR_AGVInformation,DHR+R_AGV_PARA_START_ADDR,sizeof(g_DHR_AGVInformation));

	uint8_t res = 0;
	do{

		/*获取上位机命令*/
		GetHostControl();
		
		/*获取上位机命令*/
		res = WalkPathProgram();

		/*路径解析结果是0，标志置位并返回*/
		if(res == 0)
		{
			g_HostData.WalkPathFlag = false;
			//M_K_SET_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_DownoadPathError);
			break;
		}
		/*否则，标志置零 并清除路径下载错误的故障*/
		g_HostData.WalkPathFlag = true;
		M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_DownoadPathError);
		
	}while(0);

	switch (g_HostData.AGVAutoMode)
	{
		case AutoMode_Initial://上电初始化
		{
			//上电初始化检测外部传感器是不是就绪了；			
			if (g_HostData.PosValid  == true)///g_HostData.NavigationData.NavLidar_PosValid == true)
			{
				//参数设置
				
				//默认设备为静止模式
				g_HostData.AGVAutoMode = AutoMode_PreOperational;

				//自动控制相关清空
				//AGV_ClaerAGVAlarm();

				//所有安全传感器使能
				///SecuritySensorEnable();

				//AGV上次运行方向默认为空
				g_HostData.LastAGVRunDir = AGVRunDir_NULL;
			}
			else
			{
				//95s进行检查,等待导航激光雷达启动 没启动就算了，直接报故障
				if (g_HostData.Position_tim_delaycnt++ > 95000)   							
				{
					M_K_SET_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_NavigationFail);
					g_HostData.Position_tim_delaycnt = 95000;
				}
			}
		}
		break;

		case AutoMode_PreOperational:  //预操作状态
		case AutoMode_Operational:     //操作状态
		{
			AGV_ClearActionBuffer();	//清命令缓存
			AGV_ClearInputCmdBuffer();

			g_HostData.ExecutionPathState = ExePathState_NoTask;
			g_HostData.TargetAGVWorkMode = AGVMode_GoStill;

			//跳转说明：
			//1.上位机命令为启动或者自动运行状态
			//2.路径计算标志为真
			if (   (g_HostData.HostControlBuffer == Host_ControlStart || g_HostData.HostControlBuffer == Host_ControlAutoRun)
				&& (g_HostData.WalkPathFlag == true)
				)
			{
				g_HostData.AGVAutoMode = AutoMode_Run;
				g_HostData.InputGoOnTaskFlag = 0x1F;
				g_HostData.StartAccFlag = 0;
			}
		}
		break;

		case AutoMode_Run://运行状态
		{
			g_HostData.ExecutionPathState = ExePathState_HaveTask;
			
			//不在自动框架下反馈
			//g_HostData.RunStatusFeedback = RunStatus_Run;

			{
					g_HostData.AutoOpCtrlValid = true;
			}
			if (   g_HostData.AutoOpCtrlValid == true	//当前自动操作标志是否有效判断 
					&& g_HostData.WalkPathFlag    == true	//路径计算标志为真
					)
			{
				//AGVAutoRun(&pPathStation, V_H);//根据输入命令解算行走执行轮系数据... 
				Tsk_AutoFunctionControl();
			}
			else
			{
				//进入此处，说明在AGV运行中，进行了路径下载，但是下载的路径点存在错误
				//故对后续路径进行限速运行
				//选择这种的原因为当导航为二维码导航时，不能检测到下载出问题而原地停止，万一在路途中间，会导致地标丢失
				//对于激光导航，同样也需要停在下个点，因为调度系统会做点位匹配，在路途中间时，无法匹配
				//AGVAutoRun( &pPathStation, V_H * 0.5);//根据输入命令解算行走执行轮系数据... 
				
				//M_K_SET_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_DownloadTowardsError);		
			}

			//1.根据上位机命令(或者外部控制命令)进行状态切换
			//2.MasterAutoRun运动控制执行函数
			if (g_HostData.HostControlBuffer == Host_ControlPause)    //任务暂停
			{
				AGV_ClearInputCmdBuffer();
				
				/*输出摇杆值置0*/
				memset(g_HostData.Hxyz,0,sizeof(g_HostData.Hxyz));
				/*根据AGV的执行状态，分别执行不同的动作
					行走控制状态：可控停车，清除加速标志，任务恢复后重新加速
					*/
				if(g_HostData.AGV_FuncSta == AGV_FunctionSta_Walk)
				{
					g_HostData.StartAccFlag = 0;
					g_HostData.ActualForwardVelocity = 0.0f;
				}
				g_HostData.AGVAutoMode = AutoMode_Pause;
			}
			else if (g_HostData.HostControlBuffer == Host_ControlAbort)   //任务撤销
			{
				if(g_HostData.AGVPendantFun.RepealTaskStopMove == 0X01)
				{
					/*直接切到任务停止状态*/
					g_HostData.AGVAutoMode = AutoMode_Stop;
				}
				else
				{
					/*根据AGV的执行状态，分别执行不同的执行任务
					初始化状态：直接到AGV任务停止状态
					行走控制状态：停当前目标点后，让状态机自动切换至初始状态
					其他：暂无特殊处理，执行完成后，让状态机自动切换至初始状态
					*/
					if(g_HostData.AGV_FuncSta == AGV_FunctionSta_Init)
					{
						g_HostData.AGVAutoMode = AutoMode_Cancel;
					}
					else if(g_HostData.AGV_FuncSta == AGV_FunctionSta_Walk)
					{
						g_HostData.WalkStopFlag = 0xFE;
						g_HostData.WalkGoOnFlag = 0x00;
						g_HostData.WalkSlowDownFlag = 0x01;
						pPathStation->Pnext->PathSequence = 0;
					}
					else
					{
						//g_HostData.AGVAutoMode = AutoMode_Stop;
					}
				}
				
			}
			else if (g_HostData.HostControlBuffer == Host_ControlStop)    //任务停止
			{
				if(g_HostData.AGVPendantFun.RepealTaskStopMove == 0X01)
				{
					/*直接切到任务停止状态*/
					g_HostData.AGVAutoMode = AutoMode_Stop;
				}
			}
			else
			{
				
			}
		}
		break;

		case AutoMode_Stop://停止状态
		{
			//不在自动框架下反馈
			//g_HostData.RunStatusFeedback = RunStatus_Stop;
			//1.清空路径
			//2.切换为AutoMode_Operational
			InitNavMapASystem(Path_Clear);                  //路径清空    
			InitHostData();                              //调度系统控制数据清空
			AGV_ClearActionBuffer();                          //清命令缓存	

			/*清除任务暂停的标记*/
			g_HostData.TaskPauseFlag = 0x00;
			

			/*如果任务执行结束，末端路径纠偏结果的标志置位*/
			if(   g_HostData.AGVMagParkZeroOffset.Flag != 0x08
				&&g_HostData.GoodsFlag == 0x00)
			{
				g_HostData.AGVMagParkZeroOffset.Flag = 0x08;
			}
			g_HostData.TargetAGVWorkMode = AGVMode_GoStill;

			//报警清空
			M_K_CLR_BIT(g_HostData.AGV_Alarm_Evt1, AGV_ALM_DownloadTowardsError);

			g_HostData.AGVAutoMode = AutoMode_Operational;

			if (g_DHR_AGVInformation[Para_Index_AGV_TaskControlCode] != Host_ControlAutoRun)
			{
				g_DHR_AGVInformation[Para_Index_AGV_TaskControlCode] = Host_ControlInitial;
			}
			/*外部控制命令结束状态的清除*/
			AGV_TaskStopControlCallBack();
		}
		break;

		case AutoMode_Cancel://撤销状态
		{
			//1.等待RunToCancel_ModeTime秒后，上位机命令为启动或自动运行
			//2.路径计算标志为真
			//3.切换为AutoMode_Run

			//1.等待RunToCancel_ModeTime + CancelToStop_ModeTime秒后
			//2.切换为AutoMode_Stop
			if (g_HostData.HostControlBuffer == Host_ControlStart)    //任务启动
			{
				if (Delay_Fuction(g_HostData.Sys1msCounter, g_HostData.HostFreshCnt, RunToCancel_ModeTime))
				{//延迟PauseToRun_ModeTime秒,去执行状态切换任务!!!
					g_HostData.HostFreshCnt = g_HostData.Sys1msCounter;
					g_HostData.AGVAutoMode = AutoMode_Run;
				}
			}
			else
			{
				if (Delay_Fuction(g_HostData.Sys1msCounter, g_HostData.HostFreshCnt, (RunToCancel_ModeTime + CancelToStop_ModeTime)))
				{//延迟RunToCancel_ModeTime + CancelToStop_ModeTime秒,去执行状态切换任务!!!
					g_HostData.HostFreshCnt = g_HostData.Sys1msCounter;
					g_HostData.AGVAutoMode = AutoMode_Stop;
				}
			}
		}
		break;

		case AutoMode_Pause://暂停状态
		{
			//1.清空输入命令
			//2.等待上位机命令

			if (g_HostData.HostControlBuffer == Host_ControlPause)         //任务暂停
			{
				if (   g_HostData.AutoOpCtrlValid == true	//当前自动操作标志是否有效判断 
					&& g_HostData.WalkPathFlag    == true	//路径计算标志为真
					)
				{//AGV自动控制有效
					//AGVAutoRun(&pPathStation, 0);                  //根据输入命令解算行走执行轮系数据... 
					/*输出摇杆值置0*/
					memset(g_HostData.Hxyz,0,sizeof(g_HostData.Hxyz));
					/*输出升降按钮置0*/
					g_HostData.AGVHandleButtom = 0x00;
				}
			}
			else if (	  g_HostData.HostControlBuffer == Host_ControlStart    	//任务启动
						||g_HostData.HostControlBuffer == Host_ControlAutoRun  	//任务自动运行
						||g_HostData.HostControlBuffer == Host_ControlAbort		/*任务撤销*/
					)
			{
				if (Delay_Fuction(g_HostData.Sys1msCounter, g_HostData.HostFreshCnt, PauseToRun_ModeTime))
				{//延迟PauseToRun_ModeTime秒,去执行状态切换任务!!!
					g_HostData.HostFreshCnt = g_HostData.Sys1msCounter;
					g_HostData.AGVAutoMode = AutoMode_Run;
					/*标记任务暂停*/
					g_HostData.TaskPauseFlag = 0x01;
				}
			}
			else if (g_HostData.HostControlBuffer == Host_ControlStop)    //任务停止
			{
				if (   g_HostData.AutoOpCtrlValid == true	//当前自动操作标志是否有效判断 
					&& g_HostData.WalkPathFlag    == true	//路径计算标志为真
					)
				{//AGV自动控制有效					
					//AGVAutoRun(&pPathStation, 0);                  //根据输入命令解算行走执行轮系数据... 
					/*输出摇杆值置0*/
					memset(g_HostData.Hxyz,0,sizeof(g_HostData.Hxyz));
					/*输出升降按钮置0*/
					g_HostData.AGVHandleButtom = 0x00;
				}

				if (Delay_Fuction(g_HostData.Sys1msCounter, g_HostData.HostFreshCnt, PauseToStop_ModeTime))
				{//延迟PauseToRun_ModeTime秒,去执行状态切换任务!!!
					g_HostData.HostFreshCnt = g_HostData.Sys1msCounter;
					g_HostData.AGVAutoMode = AutoMode_Stop;
				}
			}
			/*外部控制命令暂停状态的清除*/
			AGV_TaskPauseControlCallBack();
		}
		break;

		default:
		{
			AGV_ClearActionBuffer();                          //清命令缓存				
			///pH->McWalkStatus = MC_STOP2;										//0速停止行走电机
		}
		break;
	}
}
/************************************END OF FILE************************************/
