/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_AGVAutorunWalk.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.xx.xx
  * Description: AGV自动运行的框架代码
  *******************************************************************************/
#include "app_SkeletonAGV.h"
#include "app_AGVAutorunWalk.h"

/***************************************************************************************
** 函数名称: AGV_WalkStopFlagCheck
** 功能描述: 传入的点位信息，将其自生和其下一个点位信息的内容作比较，
			 预判下个点位是否需要停车
** 参    数: *p_Path 点位信息，可以是当前点或者是下个点
** 返 回 值: res   0：无需停车 其他：需要停车
****************************************************************************************/
uint8_t	AGV_WalkStopFlagCheck(PathTypeDef *p_Path)
{
	uint8_t res = 0x00;
	static PathTypeDef *p_NextPath;              //指针，指向路径链表数据的单个站点单元
	p_NextPath = p_Path->Pnext;


	/*2.判断该点当前朝向和下个点的朝向是否一致*/
	{
		//if( p_Path->Towards != p_NextPath->Towards)
		if(fabs( p_Path->Course - p_NextPath->Course) >= 10.0f)
		{
			res = 2;
		}
	}
	/*3.判断该点当前航向和下个点的朝向是否一致*/
	{
		//if( p_Path->Course != p_NextPath->Course)
		if(fabs( p_Path->Course - p_NextPath->Course) >= 10.0f)
		{
			res = 3;
		}
	}
	/*4.判断该点当前标签值和下个点的标签值是否一致*/
	{
		if(p_Path->Label ==  p_NextPath->Label)
		{
			res = 4;
		}
	}
	/*5.判断该点目标高度和下个点的目标高度是否一致,
		如果下个点下个操作需要进入“高度控制状态”*/
	{
		if(p_Path->LiftHigh != p_NextPath->LiftHigh)
		{
			/*如果当前点高度不为0，但是下个点是高度保持这种情况，不需要停车*/
			if(	  (p_Path->LiftHigh != 0XFFFF && p_NextPath->LiftHigh == 0xFFFF)
				||(p_Path->LiftHigh == 0XFFFF && p_NextPath->LiftHigh != 0xFFFF))
			{
				
			}
			else
			{
				res = 5;
			}
		}
	}
	/*6.判断下个点位是否需要进行精定位功能*/
	{
		if(0)
		{
			res = 6;
		}
	}
	/*7.判断下个点位是否需要进行项目特殊功能*/
	{
		if(0)
		{
			res = 7;
		}
	}
	/*8.判断下个点位是否0速度*/
	{
		if( p_Path->SpeedPercentage == 0)
		{
			res = 8;
		}
	}
	/*9.判断下个点位是否需要确认信号*/
	{
		if( p_Path->ConfirmSignal == true)
		{
			res = 9;
		}
	}
	/*10.有悬挂控制的情况下，判断下个点位是否需要悬挂调整*/
	{
		if(	  g_HostData.AGVPendantFun.HangControl == 0X01
			&&p_Path->HangLiftHigh != p_NextPath->HangLiftHigh
		)
		{
			/*如果当前点悬挂高度不为0，但是下个点是悬挂高度保持这种情况，不需要停车*/
			if(	  (p_Path->HangLiftHigh != 0XFFFF && p_NextPath->HangLiftHigh == 0xFFFF)
				||(p_Path->HangLiftHigh == 0XFFFF && p_NextPath->HangLiftHigh != 0xFFFF)
				/*如果悬挂高度变化大于最大限制才调悬挂*/
				//||abs(p_Path->HangLiftHigh-p_NextPath->HangLiftHigh)>g_HostData.Para_Index.AGV_DeadZoneHighMax
			)
			{
				
			}
			else
			{
				res = 10;
			}
		}
	}
	//11.511项目的特殊功能，在工位点前的一个点，停车检查工位中是否有工件
	{
		if(p_NextPath-> PathWayPointRetain[25] != 0)
		{
			res = 11;
		}
	}
	//12.补丁：增加结束点位标记停车判断
	{
		if(p_NextPath-> PathWayPointRetain[23] != 0)
		{
			res = 12;
		}
	}
	
	//13.导航模式切换判断
	{
		if(p_NextPath->NavigationModeSwitch != 0)
		{
			res = 13;
		}
	}
	
	return res;
}

/***************************************************************************************
** 函数名称: AGV_WalkStopDisCheck
** 功能描述: 传入的点位信息，将其自生和其下一个点位信息的内容作比较，
			 最后回传
** 参    数: *p_Path 点位信息，可以是当前点或者是下个点
** 返 回 值: 
****************************************************************************************/

uint32_t Master_WalkDis[20] ;
uint8_t  Master_WalkDisNUM = 0;

uint32_t AGV_WalkStopDisCheck (PathTypeDef *p_Path)
{
	PathTypeDef *p_NextPath = p_Path->Pnext;
	uint32_t	SumDis = 0;
	uint8_t		WalkStopRes = 0;
	do
	{
		/*如果当前的标签已经为空了，递归结束*/
		if(p_Path->Label == 0)	{break;}
		/*如果当前的指针已经为空了，递归结束*/
		if(p_Path == NULL) 		{break;}

		/*把当前区间的长度根据航向记录下来*/
		if(	  (p_Path->Course >= 4500 && p_Path->Course < 13500)
			||(p_Path->Course >= 22500 && p_Path->Course < 31500))
		{
			SumDis = abs(p_Path->Map_x - p_NextPath->Map_x);
		}
		else
		{
			SumDis = abs(p_Path->Map_y - p_NextPath->Map_y);
		}
		/*判断当前点位是否需要停车*/
		WalkStopRes = AGV_WalkStopFlagCheck(p_Path);
		/*如果当前点位已经需要停车，递归结束；返回的停车前的区间长度*/
		if(WalkStopRes != 0)	{break;}
		/*均不满足，开始递归，把下个点位的区间长度通过函数返回*/
		SumDis = SumDis + AGV_WalkStopDisCheck(p_NextPath);
	}while(0);

	Master_WalkDis[Master_WalkDisNUM++] = SumDis;
	return SumDis;
}

/**********************************************************************************************************
*	函 数 名: CalcAGVMoveDir
*	功能说明: 计算当前运动的方向
*	形    参：RunMapTowards.当前朝向    RunMapCourse.当前航向
*	返 回 值: 运动方向
**********************************************************************************************************/
AGVRun_Dir CalcAGVMoveDir(float RunMapTowards,float RunMapCourse)
{
	float AngleDiff = 0.0f,AngleDiffTemp = 0.0f;
	AGVRun_Dir Dir= AGVRunDir_Forward;

	AngleDiffTemp = (RunMapTowards - RunMapCourse);

	if(AngleDiffTemp < 0)	{AngleDiff = (360.0f + AngleDiffTemp);}
	else				{AngleDiff = AngleDiffTemp;}

	//算法说明：
	//请参考《朝向与航向差值得出运动方向.excel》
	//AngleDiff 为   0，则向前运行
	//AngleDiff 为 180，则向后运行
	//AngleDiff 为 270，则向左运行
	//AngleDiff 为  90，则向右运行

	if(AngleDiff >45.0f && AngleDiff <= 135.0f)
	{
		Dir = AGVRunDir_Right;
	}  
	else if(AngleDiff >135.0f && AngleDiff <= 225.0f)
	{
		Dir = AGVRunDir_Back;
	}    		
	else if(AngleDiff >225.0f && AngleDiff <= 315.0f)
	{
		Dir = AGVRunDir_Left;
	}  
	else
	{
		Dir = AGVRunDir_Forward;
	}      
	return Dir;
}

/***************************************************************************************
** 函数名称: AGV_WalkStaCheck
** 功能描述: 根据路径信息和当前AGV状态，确定要不要切换至行走控制状态
** 参    数: 
** 返 回 值: res 0-无需切换状态，1-切换至行走控制，2-状态回溯，切换至支腿控制状态
****************************************************************************************/
uint8_t AGV_WalkStaCheck(PathTypeDef *p_NextPath , PathTypeDef *p_LastPath)
{
	uint8_t res = 0x00;
	uint8_t WalkCheckRes = 0;
	g_HostData.VirtualPointFlag = pPathStation->VirtualPointSta;
	g_HostData.IntersectionFlag = pPathStation->IntersectionSta;
	
	/*计算到停车前的剩余距离*/
	g_HostData.WalkUniformSlowData.DecelerationDistance = AGV_WalkStopDisCheck(p_LastPath);
	Master_WalkDisNUM = 0;

	/*根据导航方式的不同，判断是否已经满足“行走控制状态”*/
	switch(g_HostData.AGVNavigateMode)
	{

		case AGVNavigateMode_QR:		/*二维码导航*/
		{
			if(	g_HostData.CurrentPointTag != pPathStation->Label)
			{
				/*跳码的情况下，如果是连续行走路段，直接更新标签值*/
				if(g_HostData.WalkGoOnFlag == 0x01)
				{
					g_HostData.CurrentPointTag = pPathStation->Label;
				}
				else
				{
					WalkCheckRes = 0x01;
				}
			}
		}
		break;

		case AGVNavigateMode_Magnetic:	/*磁条导航 需要工装二维码、RFID、虚拟里程计*/
		{
			if(	g_HostData.CurrentPointTag != pPathStation->Label)
			{
				/*跳码的情况下，如果是连续行走路段，直接更新标签值*/
				if(g_HostData.WalkGoOnFlag == 0x01)
				{
					g_HostData.CurrentPointTag = pPathStation->Label;
				}
				/*虚拟点位的情况下，默认完成了点位更新*/
				else if(g_HostData.WalkParkFlag == AGVWalkParkFlag_OdoPark)
				{
					g_HostData.CurrentPointTag = pPathStation->Label;
					g_HostData.WalkParkFlag = AGVWalkParkFlag_Park;
					g_HostData.VirtualOdoFixFlag = 0x01;
				}
				else
				{
					WalkCheckRes = 0x01;
				}
			}
		}
		break;

		default:
		case AGVNavigateMode_NULL:		/*初始化*/
		case AGVNavigateMode_SLAM:		/*激光导航*/
		case AGVNavigateMode_GPS:		/*GPS导航*/
		{
			int32_t 	D_Value_x	= pPathStation->Map_x - g_HostData.NavigationData.NavLidar_x;
			int32_t 	D_Value_y	= pPathStation->Map_y - g_HostData.NavigationData.NavLidar_y;
			/*将调度系统的航向指令正交化*/
			uint16_t 	Dir 		= 0;
			if(p_LastPath->Course >45.0f && p_LastPath->Course <= 135.0f)		{Dir = 90;}  
			else if(p_LastPath->Course >135.0f && p_LastPath->Course <= 225.0f)	{Dir = 180;}    		
			else if(p_LastPath->Course >225.0f && p_LastPath->Course <= 315.0f)	{Dir = 270;}  
			else																{Dir = 0;}
			D_Value_x = D_Value_x>0?D_Value_x:-D_Value_x;
			D_Value_y = D_Value_y>0?D_Value_y:-D_Value_y;
			/*GPS导航下，第一个点位 不响应位置坐标（为了保证和磁导航、二维码导航的统一）*/
			if(pPathStation->PathSequence == 1)
			{
				/*无需操作*/
			}
			/*GPS导航下，一旦在开始行走前判断是不需要停车的点位，那就充分相信行走中对点位到位的判断，
			  默认通过行走控制,这种情况下，如果没有紧接着更新链表P指针就一定有问题*/
			else if(g_HostData.WalkGoOnFlag == 0x01)
			{
				/*无需操作*/
			}
			/*GPS导航下，检查是否到达了点位*/
			else if(  (Dir == 180|| Dir == 0)
					&&(D_Value_x > g_HostData.Para_Index.AGV_WayPointOffset_X)
				)
			{WalkCheckRes = 0x02;}
			else if(  (Dir == 90|| Dir == 270)
					&&(D_Value_y > g_HostData.Para_Index.AGV_WayPointOffset_Y)
				)
			{WalkCheckRes = 0x03;}
		}
		break;

	}
	/*如果不满足点位信息对行走控制的要求，继续判断退出行走控制时，是否需要停车*/
	if(	WalkCheckRes != 0x00)
	{
		/*如果有支腿控制功能，状态回溯 检查支腿是否正常回收*/
		{
			static uint16_t WlakLegSequence = 0;
			if(		g_HostData.AGVPendantFun.LegTelescopic == 0X01
				&&WlakLegSequence != pPathStation->PathSequence
				&&g_HostData.LegTelescopicSta != 0X01)
			{
				WlakLegSequence = pPathStation->PathSequence;
				/*修改返回值，在函数外部修改状态机*/
				res = res|0X02;
				return res;
				//AGV_FuncSta_Next = AGV_FunctionSta_LegTelescopic;
				//break;
			}
			else
			{
				/*清除相关的标记*/
				WlakLegSequence = 0;
			}
		}
		/*获取当前点位速度档位*/
		/*响应调度系统到达点的速度*/
		//g_HostData.CurrentPer = pPathStation->SpeedPercentage;
		/*响应调度系统出发点的速度*/
		g_HostData.CurrentPer = p_LastPath->SpeedPercentage;
		if(g_HostData.CurrentPer>100)	{g_HostData.CurrentPer = 100;}

		/*A.如果这是倒数第二个点位，需要启用末端定位的相关功能板块*/
		if(p_NextPath->PathSequence == 0)
		{
			g_HostData.EndPathFlag = 0x01;
			g_HostData.WalkStopFlag = 0x01;
		}
		/*B.否则，根据P和P.Next,判断在P.Next点位要不要停车*/
		else if(p_LastPath != NULL)
		{
			g_HostData.WalkStopFlag = AGV_WalkStopFlagCheck(p_LastPath);
		}
		/*C.正常执行是不会到这里的，执行到这先停车再说其他的*/
		else
		{
			g_HostData.WalkStopFlag = 0xFF;
		}
		/*D.如果需要停车,更新停车需要的相关变量*/
		{
			if(g_HostData.WalkStopFlag != 0x00)
			{
				g_HostData.WalkGoOnFlag = 0x00;
				g_HostData.WalkSlowDownFlag = 0x01;
				/*需要停车，更新停车方式同时更新减速方式，供后续判断停车、减速条件*/
				{
					g_HostData.LastWalkParkFlag = g_HostData.WalkParkFlag;
					/*通过下个点位举升高度是否变化来确定是不是取放物料的停车方式*/
					if(  //pPathStation->LiftHigh != 0XFFFF
						//&&pPathStation->LiftHigh > p_LasetPath->LiftHigh
						g_HostData.AGVTaskNotifyType == AGVTaskTypeNotify_Goods
						&&pPathStation->PathWayPointRetain[24] == 4
						&&pPathStation->PathWayPointRetain[23] == 1
						)
					{
						uint8_t res = 0;
						/*首先确保没有盲举的功能设置*/
						{
							if(	  g_HostData.AGVPendantFun.BlindLift == 0X01
								&&g_HostData.BlindLiftFunc.BlindLiftStationNum != 0)
							{
								uint8_t i = 0;
								/*便利盲举工位的列表，如果成功则提前结束循环*/
								for(i=0;((res==0x00)&&(i<g_HostData.BlindLiftFunc.BlindLiftStationNum));i++)
								{
									if(pPathStation->Label == g_HostData.BlindLiftFunc.BlindLiftStation[i])
									{
										res = 0X01;
									}
									else{/*什么都不需要做*/}
								}
							}
							else{/*什么都不需要做*/}
						}
						/*统一处判断结果，如果盲举判断不成功，res就是0x00，*/
						{
							g_HostData.WalkParkFlag = AGVWalkParkFlag_GetGoods;
							if(res == 0X00)
							{
								g_HostData.AGVParkNowMode = g_HostData.AGVGetParkGoodsMode;
								g_HostData.AGVCorrectNowMode = g_HostData.AGVGetParkCorrectMode;
							}
							else{/*什么都不需要做*/}
						}
					}
							else if(  //pPathStation->LiftHigh != 0XFFFF
									//&&pPathStation->LiftHigh < p_LasetPath->LiftHigh
								pPathStation->PathWayPointRetain[24] == 4
								&&pPathStation->PathWayPointRetain[23] == 1
							)
							{
								g_HostData.AGVParkNowMode = g_HostData.AGVPutParkGoodsMode;
								g_HostData.WalkParkFlag = AGVWalkParkFlag_PutGoods;
								g_HostData.AGVCorrectNowMode = g_HostData.AGVPutParkCorrectMode;
							}
							/*通过是否为末端路径并且充电任务确定是不是充电的停车方式*/
							else if(  g_HostData.AGVTaskNotifyType == AGVTaskTypeNotify_Charge
									//&&g_HostData.EndPathFlag == 0x01
									 &&pPathStation->PathWayPointRetain[24] == 3
									 &&pPathStation->PathWayPointRetain[23] == 1
									)
							{
								g_HostData.AGVParkNowMode = g_HostData.AGVChargeParkMode;
								g_HostData.WalkParkFlag = AGVWalkParkFlag_Charge;
								g_HostData.AGVCorrectNowMode = g_HostData.AGVChargeCorrectMode;
							}
							else
							{
								g_HostData.WalkParkFlag = AGVWalkParkFlag_Park;
							}
				}
				
			}
			else
			{
				g_HostData.WalkGoOnFlag = 0x01;
			}
			/*经过上面停车方式的判断后，如果当前是磁导航十字磁条下的停车功能*/
			if(	  g_HostData.AGVNavigateMode == AGVNavigateMode_Magnetic
				&&g_HostData.AGVParkNowMode  == AGVParking_CrossMag)
			{
				/*十字点位标记失效，需要将默认的十字磁条停车修改成RFID停车*/
				if(  g_HostData.IntersectionFlag != true)
				{
					g_HostData.AGVParkNowMode = AGVParking_RFID;
					g_HostData.AGVCorrectNowMode = AGVCorrection_ODO;
					g_HostData.WalkParkFlag = AGVWalkParkFlag_RfidPark;
				}
				/*虚拟点位标记生效，需要将默认的十字磁条停车修改成里程计停车*/
				else if(  g_HostData.VirtualPointFlag == true)
				{
					g_HostData.AGVParkNowMode = AGVParking_ODO;
					g_HostData.AGVCorrectNowMode = AGVCorrection_ODO;
					g_HostData.WalkParkFlag = AGVWalkParkFlag_OdoPark;
				}
			}
		}
		/*E.需要停车，一定需要减速，更新当前区间的减速距离*/
		{
			if(g_HostData.WalkSlowDownFlag != 0x00)
			{
				float *p_SlowDownDis = g_HostData.AGVWalkSlowDownDisPlan;
				
				/*如果是有如果有货物，选用带载的停车距离*/
				if(g_HostData.GoodsFlag != 0X00)
				{
					p_SlowDownDis = g_HostData.AGVGoodsSlowDownDisPlan;
				}
				//根据档位速度选择刹车距离

				if(g_HostData.VirtualPointFlag != 0){g_HostData.SlowDownDis = p_SlowDownDis[0];}
				else if	(g_HostData.CurrentPer >75)	{g_HostData.SlowDownDis = p_SlowDownDis[3];}
				else if	(g_HostData.CurrentPer >50)	{g_HostData.SlowDownDis = p_SlowDownDis[2];}
				else if	(g_HostData.CurrentPer >25)	{g_HostData.SlowDownDis = p_SlowDownDis[1];}
				else if	(g_HostData.CurrentPer > 0)	{g_HostData.SlowDownDis = p_SlowDownDis[0];}
				else if	(g_HostData.CurrentPer ==0)	{g_HostData.SlowDownDis = p_SlowDownDis[1];}
				else								{g_HostData.SlowDownDis = p_SlowDownDis[3];}

			}
		}
		/*默认使用第速度6挡位*/
		g_HostData.AGVWalkSpeedVal = g_HostData.Para_Index.AGVWalkDefaultSpeedVal;

		//获取下段路径的属性
		//g_HostData.OldData.CurrentPathProperty = pPathStation->PathProperty;

		/*根据路径信息计算得到里程计目标值——当前只适用于直线的情况，曲线暂时没做*/
		{
			s32 D_Value_x = 0,D_Value_y = 0;
			vfp32 Buffer_XY = 0.0f;                      	//计算中间值
			vfp32 CalcBuffer = 0.0f;                     	//计算中间值
			g_HostData.NextPath_X = pPathStation->Map_x;		//获取路径下个点x值
			g_HostData.NextPath_Y = pPathStation->Map_y;		//获取路径下个点y值
			g_HostData.NextPath_Tag = pPathStation->Label;		//获取路径下个点标签值

			/*更新朝向和航向*/
			g_HostData.AGVCurrentCourse = p_LastPath->Course;
			g_HostData.AGVCurrentTowards = p_LastPath->Towards;

			/*将调度系统下发的航向转义一遍、归一化，方便进行行走到位判断(正交的直线路径下的处理，斜线和曲线暂时没考虑)*/
			{
				if(  (g_HostData.AGVCurrentCourse < 45.0f&&g_HostData.AGVCurrentCourse >= 0.0f)
					||(g_HostData.AGVCurrentCourse < 360.0f&&g_HostData.AGVCurrentCourse >= 315.0f))
				{
					g_HostData.AGV_Course = AGVCourse_X_Positive;
				}
				else if(g_HostData.AGVCurrentCourse < 135.0f&&g_HostData.AGVCurrentCourse >= 45.0f)
				{
					g_HostData.AGV_Course = AGVCourse_Y_Positive;
				}
				else if(g_HostData.AGVCurrentCourse < 225.0f&&g_HostData.AGVCurrentCourse >= 135.0f)
				{
					g_HostData.AGV_Course = AGVCourse_X_Negative;
				}
				else if(g_HostData.AGVCurrentCourse < 315.0f&&g_HostData.AGVCurrentCourse >= 225.0f)
				{
					g_HostData.AGV_Course = AGVCourse_Y_Negative;
				}
			}
			//获取AGV运行方向
			g_HostData.AGVRunDir = CalcAGVMoveDir(p_LastPath->Towards,p_LastPath->Course);           //AGV运行方向
			/*根据运行方向 横向巡线模式还是正向巡线模式*/
			{
				if(	  g_HostData.AGVRunDir == AGVRunDir_Forward
					||g_HostData.AGVRunDir == AGVRunDir_Back)
				{
					g_HostData.TargetAGVWorkMode = AGVMode_GoLine;
				}        
				else if(  g_HostData.AGVRunDir == AGVRunDir_Left
						||g_HostData.AGVRunDir == AGVRunDir_Right)
				{
					g_HostData.TargetAGVWorkMode = AGVMode_GoTransverse;
				}
			}

			D_Value_x = abs(g_HostData.CurrentPath_X - g_HostData.NextPath_X);
			D_Value_y = abs(g_HostData.CurrentPath_Y - g_HostData.NextPath_Y);

			Buffer_XY = pow(D_Value_x,2) + pow(D_Value_y,2);
			CalcBuffer = sqrt(Buffer_XY);
			g_HostData.CurrentPathDelta = CalcBuffer;                               //获取区间长度方向
			/*如果是二维码导航，二维码间的距离小于400的情况下，要考虑二维码间的±40mm检测范围以及里程计的误差*/
			if(		g_HostData.AGVNavigateMode == AGVNavigateMode_QR
				&&	g_HostData.CurrentPathDelta < g_HostData.Para_Index.AGV_QRMode_PathDeltaMin)
			{
				g_HostData.CurrentPathDelta_Half = g_HostData.CurrentPathDelta/4;		
			}
			/*如果是磁导航，十字磁条距离小于400的情况下，要考虑磁导航±160mm检测范围以及里程计的误差*/
			else if(		g_HostData.AGVNavigateMode == AGVNavigateMode_Magnetic
				&&	g_HostData.CurrentPathDelta < g_HostData.Para_Index.AGV_MagMode_PathDeltaMin)
			{
				g_HostData.CurrentPathDelta_Half = g_HostData.CurrentPathDelta*3/4;		
			}
			else
			{
				g_HostData.CurrentPathDelta_Half = g_HostData.CurrentPathDelta/2;		//当前区间长度一半值
			}

			g_HostData.PointToEnd_Dis = g_HostData.CurrentPathDelta;				/*行走区间更新*/

			g_HostData.SlowDownDis = g_HostData.SlowDownDis+g_HostData.CurrentPathDelta/20;
			/*如果减速距离大于全程，将减速距离改成全程*/
			//if(g_HostData.SlowDownDis > g_HostData.CurrentPathDelta_Half){g_HostData.SlowDownDis = g_HostData.CurrentPathDelta_Half;}
			if(g_HostData.SlowDownDis > g_HostData.CurrentPathDelta){g_HostData.SlowDownDis = g_HostData.CurrentPathDelta;}

		}

		/*更新当前区间目标速度*/
		if(g_HostData.CurrentPer == 0)
		{
			g_HostData.CurrentPer = g_HostData.Para_Index.AGV_CurrentPer_Defult;
		}
		else
		{
			/*什么都不做*/
		}
		g_HostData.TargetForwardVelocity = g_HostData.CurrentPer/100.0f*V_H;

		/*路径起点检查标志置位*/
		PathTypePara.g_MapPathStartPointCheck = 0x01;
		g_HostData.WalkPathChange = 0X00;
		/*修改返回值，在函数外部修改状态机*/
		res = res|0X01;
		return res;
//				AGV_FuncSta_Next = AGV_FunctionSta_Walk;
//				break;
	}
	/*如果满足点位信息对行走控制的要求，更新当前路径信息*/
	else
	{
		g_HostData.CurrentPath_X 	= pPathStation->Map_x;		//获取本路径的起点x值
		g_HostData.CurrentPath_Y 	= pPathStation->Map_y;		//获取本路径的起点y值
		g_HostData.CurrentPath_Tag 	= pPathStation->Label;		//获取路径下个点标签值
		g_HostData.NextPath_X 		= p_NextPath->Map_x;		//获取路径下个点x值
		g_HostData.NextPath_Y 		= p_NextPath->Map_y;		//获取路径下个点y值
		g_HostData.NextPath_Tag 	= p_NextPath->Label;		//获取路径下个点标签值
		g_HostData.NextAGVRunDir = CalcAGVMoveDir(pPathStation->Towards,pPathStation->Course);           //AGV运行方向
	}
	return 0;
}
/***************************************************************************************
** 函数名称: AGV_TskSlowDownWalk
** 功能描述: 行走控制状态下的检测是否需要减速
** 参    数: None
** 返 回 值: SlowDownFlag  0:正常返回  其他(>1)-减速中
****************************************************************************************/
uint8_t AGV_WalkSlowDown(void)
{
	uint8_t SlowDownFlag = 0;/*减速中，置位*/
	switch(g_HostData.AGVCorrectNowMode)
	{
		case AGVCorrection_FixQR:	/*通过对天的PGV进行减速*/
		{
			float K_CurrentSpeedBase = 0.0f;
			int16_t Correction_QR_Dis = g_HostData.Para_Index.AGV_Correction_QR_Dis;
			float   AGV_Correction_QR_k = g_HostData.Para_Index.AGV_Correction_QR_k;
			/*里程计减速:路径上剩余距离超过减速距离时并且没有识别到二维码时，比例减速*/
			if(  (g_HostData.PointToEnd_Dis-Correction_QR_Dis) < g_HostData.SlowDownDis
				&&g_HostData.PointToEnd_Dis >= Correction_QR_Dis
				&&g_HostData.FixtureQRData.PintSta == 0)
			{
				K_CurrentSpeedBase =1.0f*(g_HostData.PointToEnd_Dis-Correction_QR_Dis)/g_HostData.SlowDownDis;
				/*避免没加速到最大值的时候，还会因为减速距离等于剩余距离 而产生加速*/
				if(g_HostData.ForwardVelocity_K < K_CurrentSpeedBase)
				{
					K_CurrentSpeedBase = g_HostData.ForwardVelocity_K;
				}
				/*比例减速*/
				if(K_CurrentSpeedBase>1.0f){K_CurrentSpeedBase = 1.0f;}
				else if(K_CurrentSpeedBase<0.1f){K_CurrentSpeedBase = 0.1f;}
				SlowDownFlag = 0x03;
			}
			else if(g_HostData.PointToEnd_Dis < Correction_QR_Dis || g_HostData.FixtureQRData.PintSta != 0)
			{
				K_CurrentSpeedBase = AGV_Correction_QR_k*20;
				SlowDownFlag = 0x04;
			}

			/*在需要停车的情况下，如果偏移量只剩10mm*/
			if(  g_HostData.WalkStopFlag != 0X00
					&&(g_HostData.AGVParkZeroOffset_Y != 0||g_HostData.FixtureQRData.PintSta != 0X00)
					&&g_HostData.AGVParkZeroOffset_Y < 10
					&&g_HostData.AGVParkZeroOffset_Y > -10)
			{
				K_CurrentSpeedBase = AGV_Correction_QR_k;
				SlowDownFlag = 0x09;
			}
			/*在需要停车的情况下，如果偏移量只剩20mm*/
			else if(  g_HostData.WalkStopFlag != 0X00
					&&(g_HostData.AGVParkZeroOffset_Y != 0||g_HostData.FixtureQRData.PintSta != 0X00)
					&&g_HostData.AGVParkZeroOffset_Y < 20
					&&g_HostData.AGVParkZeroOffset_Y > -20)
			{
				K_CurrentSpeedBase = AGV_Correction_QR_k*2;
				SlowDownFlag = 0x08;
			}
			/*在需要停车的情况下，如果偏移量只剩50mm*/
			else if(  g_HostData.WalkStopFlag != 0X00
					&&(g_HostData.AGVParkZeroOffset_Y != 0||g_HostData.FixtureQRData.PintSta != 0X00)
					&&g_HostData.AGVParkZeroOffset_Y < 50
					&&g_HostData.AGVParkZeroOffset_Y > -50)
			{
				K_CurrentSpeedBase = AGV_Correction_QR_k*6;
				SlowDownFlag = 0x07;
			}
			
			/*在需要停车的情况下，如果偏移量只剩100mm*/
			else if(  g_HostData.WalkStopFlag != 0X00
					&&(g_HostData.AGVParkZeroOffset_Y != 0||g_HostData.FixtureQRData.PintSta != 0X00)
					&&g_HostData.AGVParkZeroOffset_Y < 100
					&&g_HostData.AGVParkZeroOffset_Y > -100)
			{
				K_CurrentSpeedBase = AGV_Correction_QR_k*10;
				SlowDownFlag = 0x06;
			}
			
			/*最多减速至满速的1%*/
			if(SlowDownFlag != 0x00)
			{
			/*避免没加速到最大值的时候，还会因为减速距离等于剩余距离 而产生加速*/
				if(SlowDownFlag > 4 && g_HostData.ForwardVelocity_K < K_CurrentSpeedBase)
				{
					K_CurrentSpeedBase = g_HostData.ForwardVelocity_K;
				}
				g_HostData.ActualForwardVelocity = K_CurrentSpeedBase*g_HostData.TargetForwardVelocity;;
				g_HostData.WalkSlowDownRes = SlowDownFlag;
			}
			else if(g_HostData.WalkStopFlag != 0X00)
			{
				g_HostData.ActualForwardVelocity = AGV_Correction_QR_k*60*g_HostData.TargetForwardVelocity;
				g_HostData.WalkSlowDownRes = 0x0A;
			}
		}
		break;
		case AGVCorrection_RFID:	/*RFID  / PGV*/
		{
			/*RFID功能减速:过半程后，检测到RFID标签开始梯形减速*/
			if(   g_HostData.WalkPathChange == 0X01
				&&(g_HostData.CurrentPointTag == g_HostData.CurrentPath_Tag))
			{
				g_HostData.ActualForwardVelocity -= g_HostData.Para_Index.AGV_MasterHANDLEAcc/10.0f;
				if(g_HostData.ActualForwardVelocity < g_HostData.TargetForwardVelocity*0.1f)
				{
					g_HostData.ActualForwardVelocity = g_HostData.TargetForwardVelocity*0.1f;
				}
				else
				{
					/*不需要操作*/
				}
				SlowDownFlag = 0x01;
			}
		}
		break;

		default:
		case AGVCorrection_NULL:	/*非正常的情况都按照里程计功能执行即可*/
		case AGVCorrection_ODO_Fixed:		/*里程计，一刀切*/
		{
			/*1.里程计功能减速:路径上剩余距离超过减速距离时，比例减速*/
			if(g_HostData.PointToEnd_Dis < g_HostData.SlowDownDis)
			{
				/*最多减速至满速的10%*/
				g_HostData.ActualForwardVelocity = g_HostData.Para_Index.AGV_Correction_ODO_KMin*100.0f/g_HostData.CurrentPer*g_HostData.TargetForwardVelocity;
				SlowDownFlag = 0x02;
			}
		}
		break;

		case AGVCorrection_ODO:		/*纯粹里程计*/
		{
			float K_CurrentSpeedBase = 0.0;
			float Correction_ODO_KMax = 100.0f/g_HostData.CurrentPer;	
			float Correction_ODO_KMin = g_HostData.Para_Index.AGV_Correction_ODO_KMin*100.0f/g_HostData.CurrentPer;	
			int16_t Correction_ODO_Dis	= g_HostData.Para_Index.AGV_Correction_ODO_Dis;
			/*1.里程计功能减速:路径上剩余距离超过减速距离时，比例减速*/
			if(g_HostData.PointToEnd_Dis-Correction_ODO_Dis < g_HostData.SlowDownDis)
			{
				K_CurrentSpeedBase =1.0f*(g_HostData.PointToEnd_Dis-Correction_ODO_Dis)/g_HostData.SlowDownDis*Correction_ODO_KMax;
				/*比例减速*/
				if(K_CurrentSpeedBase>1.0f){K_CurrentSpeedBase = 1.0f;}
				else if(K_CurrentSpeedBase < Correction_ODO_KMin){K_CurrentSpeedBase = Correction_ODO_KMin;}
				/*避免没加速到最大值的时候，还会因为减速距离等于剩余距离 而产生加速*/
				if(g_HostData.ForwardVelocity_K < K_CurrentSpeedBase)
				{
					K_CurrentSpeedBase = g_HostData.ForwardVelocity_K;
				}
				/*最多减速至满速的10%*/
				g_HostData.ActualForwardVelocity = K_CurrentSpeedBase*g_HostData.TargetForwardVelocity;
				SlowDownFlag = 0x02;
			}
			else if(  g_HostData.PointToEnd_Dis <= Correction_ODO_Dis
					&&g_HostData.PointToStart_Dis > g_HostData.CurrentPathDelta_Half)
			{
				g_HostData.ActualForwardVelocity = 1.0f*Correction_ODO_KMin*Correction_ODO_KMax*g_HostData.TargetForwardVelocity;
				SlowDownFlag = 0x03;
			}
		}
		break;
		
		case AGVCorrection_ODO_Rfid:	/*rfid*/
		{
			float K_CurrentSpeedBase = 0.0f;
			float K_CurrentSpeedMax = g_HostData.CurrentPer/100.0f;
			int16_t Correction_RFID_Dis	= g_HostData.Para_Index.AGV_Correction_RFID_Dis;
			float   Correction_RFID_k 	= g_HostData.Para_Index.AGV_Correction_RFID_k*100.0f/g_HostData.CurrentPer;
			if( g_HostData.PointToStart_Dis > g_HostData.CurrentPathDelta_Half 
				   &&g_HostData.RFIDData.ReadStatus != 0X00)
			{
				K_CurrentSpeedBase = 0.01f;
				SlowDownFlag = 0x06;
			}
			/*里程计减速:路径上剩余距离超过减速距离时，比例减速*/
			else if(  (g_HostData.PointToEnd_Dis-Correction_RFID_Dis) < g_HostData.SlowDownDis
				&&g_HostData.PointToEnd_Dis >= Correction_RFID_Dis
				)
			{
				K_CurrentSpeedBase =1.0f*(g_HostData.PointToEnd_Dis-Correction_RFID_Dis)/g_HostData.SlowDownDis*K_CurrentSpeedMax;
				/*避免没加速到最大值的时候，还会因为减速距离等于剩余距离 而产生加速*/
				if(g_HostData.ForwardVelocity_K*K_CurrentSpeedMax < K_CurrentSpeedBase)
				{
					K_CurrentSpeedBase = g_HostData.ForwardVelocity_K*K_CurrentSpeedMax;
				}
				/*比例减速*/
				if(K_CurrentSpeedBase>K_CurrentSpeedMax)		{K_CurrentSpeedBase = K_CurrentSpeedMax;}
				else if(K_CurrentSpeedBase<Correction_RFID_k)	{K_CurrentSpeedBase = Correction_RFID_k;}
				SlowDownFlag = 0x07;
			}
			else if(g_HostData.PointToEnd_Dis < Correction_RFID_Dis)
			{
				if(g_HostData.ActualForwardVelocity/V_H >= Correction_RFID_k)
				{
					K_CurrentSpeedBase = Correction_RFID_k;
				}
				else
				{
					K_CurrentSpeedBase = 0.01f;
				}
				
				SlowDownFlag = 0x08;
			}
			 
			/*最多减速至满速的1%*/
			if(SlowDownFlag != 0x00)
			{
				g_HostData.ActualForwardVelocity = K_CurrentSpeedBase*V_H;
				g_HostData.WalkSlowDownRes = SlowDownFlag;
			}
		}
		break;
		case AGVCorrection_ODO_Mag:	/*进行减速*/
		{
			float 	K_CurrentSpeedBase = 0.0f;
			int16_t Correction_Mag_Dis = g_HostData.Para_Index.AGV_Correction_Mag_Dis;
			float   AGV_Correction_Mag_k = g_HostData.Para_Index.AGV_Correction_Mag_k*100.0f/g_HostData.CurrentPer;
			uint8_t CrossMagSta = 0x00;
			if(g_HostData.WalkPathChange == 0X01)
			{
				/*正向巡线，检测左右磁条；即使z轴存在偏差 只检测到了某一个磁条，也可以正常停车*/
				if(g_HostData.ActualAGVWorkMode == AGVMode_GoLine)
				{
					if(g_HostData.MagneticData.Sta_L != 0x00)
					{
						CrossMagSta = CrossMagSta|0x01;
					}
					if(g_HostData.MagneticData.Sta_R != 0x00)
					{
						CrossMagSta = CrossMagSta|0x02;
					}
				}
				/*横向巡线，检测前后磁条；即使z轴存在偏差 只检测到了某一个磁条，也可以正常停车*/
				else if(g_HostData.ActualAGVWorkMode == AGVMode_GoTransverse)
				{
					if(g_HostData.MagneticData.Sta_F != 0x00)
					{
						CrossMagSta = CrossMagSta|0x04;
					}
					if(g_HostData.MagneticData.Sta_B != 0x00)
					{
						CrossMagSta = CrossMagSta|0x08;
					}
				}
			}
			else
			{
				/*什么都不做*/
			}
			/*里程计减速:路径上剩余距离超过减速距离时并且没有识别到二维码时，比例减速*/
			if(  (g_HostData.PointToEnd_Dis-Correction_Mag_Dis) < g_HostData.SlowDownDis
				&&g_HostData.PointToEnd_Dis >= Correction_Mag_Dis
				&&(g_HostData.FixtureQRData.PintSta == 0||CrossMagSta == 0))
			{
				K_CurrentSpeedBase =1.0f*(g_HostData.PointToEnd_Dis-Correction_Mag_Dis)/g_HostData.SlowDownDis;
				/*比例减速*/
				if(K_CurrentSpeedBase>1.0f){K_CurrentSpeedBase = 1.0f;}
				else if(K_CurrentSpeedBase<AGV_Correction_Mag_k){K_CurrentSpeedBase = AGV_Correction_Mag_k;}
				SlowDownFlag = 0x01;
			}
			else if(g_HostData.PointToEnd_Dis < Correction_Mag_Dis)
			{
				K_CurrentSpeedBase = AGV_Correction_Mag_k;
				SlowDownFlag = 0x02;
			}
			else if(CrossMagSta != 0)
			{
				K_CurrentSpeedBase = AGV_Correction_Mag_k*0.3f;
				SlowDownFlag = 0x03;
			}
			/*最多减速至满速的1%*/
			if(SlowDownFlag != 0x00)
			{
			/*避免没加速到最大值的时候，还会因为减速距离等于剩余距离 而产生加速*/
				if(g_HostData.ForwardVelocity_K < K_CurrentSpeedBase)
				{
					K_CurrentSpeedBase = g_HostData.ForwardVelocity_K;
				}
				g_HostData.ActualForwardVelocity = K_CurrentSpeedBase*g_HostData.TargetForwardVelocity;
				g_HostData.WalkSlowDownRes = SlowDownFlag;
			}

		}
		break;
		case AGVCorrection_ODO_MagQR:	/*进行减速*/
		{
			float 	K_CurrentSpeedBase = 0.0f;
			int16_t Correction_MagQR_Dis = g_HostData.Para_Index.AGV_Correction_MagQR_Dis;
			float   AGV_Correction_MagQR_k = g_HostData.Para_Index.AGV_Correction_MagQR_k*100.0f/g_HostData.CurrentPer;
			uint8_t CrossMagSta = 0x00;
			if(g_HostData.WalkPathChange == 0X01)
			{
				/*正向巡线，检测左右磁条；即使z轴存在偏差 只检测到了某一个磁条，也可以正常停车*/
				if(g_HostData.ActualAGVWorkMode == AGVMode_GoLine)
				{
					if(g_HostData.MagneticData.Sta_L != 0x00)
					{
						CrossMagSta = CrossMagSta|0x01;
					}
					if(g_HostData.MagneticData.Sta_R != 0x00)
					{
						CrossMagSta = CrossMagSta|0x02;
					}
				}
				/*横向巡线，检测前后磁条；即使z轴存在偏差 只检测到了某一个磁条，也可以正常停车*/
				else if(g_HostData.ActualAGVWorkMode == AGVMode_GoTransverse)
				{
					if(g_HostData.MagneticData.Sta_F != 0x00)
					{
						CrossMagSta = CrossMagSta|0x04;
					}
					if(g_HostData.MagneticData.Sta_B != 0x00)
					{
						CrossMagSta = CrossMagSta|0x08;
					}
				}
			}
			else
			{
				/*什么都不做*/
			}
			/*里程计减速:路径上剩余距离超过减速距离时并且没有识别到二维码时，比例减速*/
			if(  (g_HostData.PointToEnd_Dis-Correction_MagQR_Dis) < g_HostData.SlowDownDis
				&&g_HostData.PointToEnd_Dis >= Correction_MagQR_Dis
				&&(g_HostData.FixtureQRData.PintSta == 0||CrossMagSta == 0))
			{
				K_CurrentSpeedBase =1.0f*(g_HostData.PointToEnd_Dis-Correction_MagQR_Dis)/g_HostData.SlowDownDis;
				/*避免没加速到最大值的时候，还会因为减速距离等于剩余距离 而产生加速*/
				if(g_HostData.ForwardVelocity_K < K_CurrentSpeedBase)
				{
					K_CurrentSpeedBase = g_HostData.ForwardVelocity_K;
				}
				/*比例减速*/
				if(K_CurrentSpeedBase>1.0f){K_CurrentSpeedBase = 1.0f;}
				else if(K_CurrentSpeedBase<AGV_Correction_MagQR_k*20){K_CurrentSpeedBase = AGV_Correction_MagQR_k*20;}
				SlowDownFlag = 0x01;
			}
			else if(g_HostData.PointToEnd_Dis < Correction_MagQR_Dis || g_HostData.FixtureQRData.PintSta != 0)
			{
				K_CurrentSpeedBase = AGV_Correction_MagQR_k*5;
				SlowDownFlag = 0x03;
			}
			else if(g_HostData.PointToEnd_Dis < Correction_MagQR_Dis || CrossMagSta != 0)
			{
				K_CurrentSpeedBase = AGV_Correction_MagQR_k*10;
				SlowDownFlag = 0x02;
			}

			/*在需要停车的情况下，如果偏移量只剩10mm*/
			if(	g_HostData.WalkStopFlag != 0X00
				&&(g_HostData.AGVParkZeroOffset_Y != 0||g_HostData.FixtureQRData.PintSta != 0X00)
					&&g_HostData.AGVParkZeroOffset_Y < 10
					&&g_HostData.AGVParkZeroOffset_Y > -10)
			{
				K_CurrentSpeedBase = AGV_Correction_MagQR_k;
				SlowDownFlag = 0x04;
			}
			
			/*最多减速至满速的1%*/
			if(SlowDownFlag != 0x00)
			{
			/*避免没加速到最大值的时候，还会因为减速距离等于剩余距离 而产生加速*/
				if(SlowDownFlag > 1 && g_HostData.ForwardVelocity_K < K_CurrentSpeedBase)
				{
					K_CurrentSpeedBase = g_HostData.ForwardVelocity_K;
				}
				g_HostData.ActualForwardVelocity = K_CurrentSpeedBase*g_HostData.TargetForwardVelocity;;
				g_HostData.WalkSlowDownRes = SlowDownFlag;
			}
//			else if(g_HostData.WalkStopFlag != 0X00)
//			{
//				g_HostData.ActualForwardVelocity = AGV_Correction_MagQR_k*60*g_HostData.TargetForwardVelocity;
//				g_HostData.WalkSlowDownRes = 0x0A;
//			}
		}
		break;
		
		case AGVCorrection_ODO_ParkQR:	/*进行减速*/
		{
			float	K_CurrentSpeedBase 	= 0.0f;
			int16_t Correction_QR_Dis 	= g_HostData.Para_Index.AGV_Correction_QR_Dis;
			float   AGV_Correction_QR_k = g_HostData.Para_Index.AGV_Correction_QR_k*100.0f/g_HostData.CurrentPer;
			uint8_t QRSta = 0x00;

			if(g_HostData.WalkPathChange == 0X01)
			{
				if(g_HostData.AGVParkNowMode == AGVParking_FixtureQR)
				{
					QRSta = g_HostData.FixtureQRData.PintSta;
				}
				else if(g_HostData.AGVParkNowMode == AGVParking_ParkQR
					||g_HostData.AGVParkNowMode == AGVParking_ParkQR_ODO)
				{
					QRSta = g_HostData.ParkQRData.PintSta;
				}
			}
			/*里程计减速:路径上剩余距离超过减速距离时并且没有识别到二维码时，比例减速*/
			if(   (g_HostData.PointToEnd_Dis-Correction_QR_Dis) < g_HostData.SlowDownDis
				&& g_HostData.PointToEnd_Dis >= Correction_QR_Dis
				&& QRSta == 0x00)
			{
				K_CurrentSpeedBase =1.0f*(g_HostData.PointToEnd_Dis-Correction_QR_Dis)/g_HostData.SlowDownDis;
				/*比例减速*/
				if(K_CurrentSpeedBase>1.0f){K_CurrentSpeedBase = 1.0f;}
				else if(K_CurrentSpeedBase<AGV_Correction_QR_k){K_CurrentSpeedBase = AGV_Correction_QR_k;}
				SlowDownFlag = 0x01;
			}
			else if(	g_HostData.PointToEnd_Dis < Correction_QR_Dis
						&& QRSta == 0x00)
			{
				K_CurrentSpeedBase = AGV_Correction_QR_k;
				SlowDownFlag = 0x02;
			}
			else if(QRSta != 0x00)
			{
				K_CurrentSpeedBase = AGV_Correction_QR_k*0.3f;
				SlowDownFlag = 0x03;
			}
			/*最多减速至满速的1%*/
			if(SlowDownFlag != 0x00)
			{
			/*避免没加速到最大值的时候，还会因为减速距离等于剩余距离 而产生加速*/
				if(g_HostData.ForwardVelocity_K < K_CurrentSpeedBase)
				{
					
					K_CurrentSpeedBase = g_HostData.ForwardVelocity_K;
				}
				g_HostData.ActualForwardVelocity = K_CurrentSpeedBase*g_HostData.TargetForwardVelocity;;
				g_HostData.WalkSlowDownRes = SlowDownFlag;
			}
		}
		break;
	}
	
	return SlowDownFlag;
}


/***************************************************************************************
** 函数名称: AGV_WalkInPlaceJudgment
** 功能描述: 行走控制状态的检测是否到位（不关心要不要停车，有停车标志在函数外速度清零）
** 参    数: None
** 返 回 值: 0-未到位	其他-已到位
****************************************************************************************/
uint8_t AGV_WalkInPlaceJudgment(void)
{
	uint8_t flage = 0x00;
	int32_t ZeroOffset_X = 0;
	int32_t ZeroOffset_Y = 0;
	int8_t	ZeroOdiff = 0;
	int8_t	ZeroOdiff_New = 0;
	/*默认清零*/
	g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_NULL;
	/*根据航向得到偏差的符号*/
	if(g_HostData.AGV_Course == AGVCourse_X_Positive||g_HostData.AGV_Course == AGVCourse_Y_Positive)
	{
		ZeroOdiff = -1;
		ZeroOdiff_New = -1;
	}
	else if(g_HostData.AGV_Course == AGVCourse_X_Negative||g_HostData.AGV_Course == AGVCourse_Y_Negative)
	{
		ZeroOdiff = -1;
		ZeroOdiff_New = 1;
	}

	/*在点位上(路程过半并且更新了标签)，才进行停车判断*/
	if(   g_HostData.WalkPathChange == 0X01
		&&g_HostData.CurrentPointTag == g_HostData.CurrentPath_Tag
	)
	{
		flage = 1;
	}
	/*在点位上 根据停靠、取料还是放料的前提，使用不同的停车条件进行判断*/
	switch(g_HostData.AGVParkNowMode)
	{
		default:
		case AGVParking_NULL:		/*初始化说明没有特殊指明功能，按照里程计即可*/
		case AGVParking_ODO:		/*纯粹里程计停车*/
		{
			/*不考虑零偏，里程计到位就停车*/
			ZeroOffset_Y = ZeroOdiff*(g_HostData.CurrentDelta_Y-g_HostData.NextPath_Y);
			ZeroOffset_X = ZeroOdiff*(g_HostData.CurrentDelta_X-g_HostData.NextPath_X);
		}
		break;

		case AGVParking_RFID:		/*RFID*/
		{
			/*不考虑零偏，不考虑里程计识别到位就停车*/
			if(flage == 1){g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_RFID;}
			/*有连续行走的标志的时候，一直读不到RFID就盲走就行了*/
			else if(g_HostData.WalkGoOnFlag != 0X00)
			{
				ZeroOffset_Y = ZeroOdiff*(g_HostData.CurrentDelta_Y-g_HostData.NextPath_Y);
				ZeroOffset_X = ZeroOdiff*(g_HostData.CurrentDelta_X-g_HostData.NextPath_X);
			}
		}
		break;

		case AGVParking_FixtureQR:	/*工装PGV*/
		{
			/*考虑零偏，工装二维码在Y轴的偏移作为零偏*/
			if(g_HostData.FixtureQRData.PintSta != 0)
			{
				int32_t FixtureQRDataZeroOffset = -1;
				if(g_HostData.AGVRunDir == AGVRunDir_Forward)
				{
					FixtureQRDataZeroOffset = -g_HostData.FixtureQRData.QR_y;
				}
				else if(g_HostData.AGVRunDir == AGVRunDir_Back)
				{
					FixtureQRDataZeroOffset = g_HostData.FixtureQRData.QR_y;
				}
				else if(g_HostData.AGVRunDir == AGVRunDir_Left)
				{
					FixtureQRDataZeroOffset = -g_HostData.FixtureQRData.QR_x;
				}
				else if(g_HostData.AGVRunDir == AGVRunDir_Right)
				{
					FixtureQRDataZeroOffset = g_HostData.FixtureQRData.QR_x;
				}
				if(FixtureQRDataZeroOffset >= 0 && g_HostData.WalkPathChange == 0X01)
				{
					g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_FixQR;
				}
			}
		}
		break;

		case AGVParking_ParkQR:		/*地标PGV*/
		{
			if(flage == 1)
			{
				/*当不需要停车的时候，不考虑零偏 识别到地标就认为到位*/
				if(g_HostData.WalkStopFlag == 0x00)
				{
					g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_ParkQR;
                }
				/*当需要停车的时候，考虑零偏，识别到地标并且零偏为0的时候，才认为到位*/
				else if(g_HostData.ParkQRData.PintSta != 0)
				{
					ZeroOffset_Y = ZeroOdiff*(g_HostData.ParkQRData.QR_y);
					ZeroOffset_X = ZeroOdiff*(g_HostData.ParkQRData.QR_x);
				}
			}
			else if(g_HostData.ParkQRData.QR_Tag == pPathStation->Label)
            {
                g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_ParkQR;
            }
			else if(  g_HostData.WalkPathChange == 0X01
					&&g_HostData.ParkQRData.PintSta != 0X00)
			{
				g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_Err;
			}
		}
		break;

		case AGVParking_QR:			/*工装+地标PGV—可能有一点问题*/
		{
			/*考虑零偏，地标二维码在Y轴的偏移作为零偏;默认工装二维码一定是正确识别的*/
			if(flage == 1 && g_HostData.ParkQRData.PintSta != 0
				          //&& g_HostData.FixtureQRData.PintSta != 0
			)
			{
				ZeroOffset_Y = ZeroOdiff*(g_HostData.ParkQRData.QR_y-g_HostData.FixtureQRData.QR_y);
				ZeroOffset_X = ZeroOdiff*(g_HostData.ParkQRData.QR_x-g_HostData.FixtureQRData.QR_x);
			}
		}
		break;
		
		case AGVParking_CrossMag:	/*检测到十字磁条进行停车*/
		{
			uint8_t CrossMagSta = 0x00;
			uint8_t ModeCheckSta = 0x00;
			int16_t Distance1 = 0;
			int16_t Distance2 = 0;
			/*正向巡线，检测左右磁条；即使z轴存在偏差 只检测到了某一个磁条，也要输出*/
			if(g_HostData.ActualAGVWorkMode == AGVMode_GoLine)
			{
				if(g_HostData.MagneticData.Sta_L != 0x00)
				{
					CrossMagSta = CrossMagSta|0x01;
					Distance1 = (g_HostData.AGVRunDir==AGVRunDir_Forward?(-1)*g_HostData.MagneticData.Distance_L:(1)*g_HostData.MagneticData.Distance_L);
				}
				if(g_HostData.MagneticData.Sta_R != 0x00)
				{
					CrossMagSta = CrossMagSta|0x02;
					Distance2 = (g_HostData.AGVRunDir==AGVRunDir_Forward?(-1)*g_HostData.MagneticData.Distance_R:(1)*g_HostData.MagneticData.Distance_R);
				}
				/*通过十字磁条停车的模式 确定到位判断的标准*/
				if(g_HostData.AGVCrossMagParkMode == 0X01 || g_HostData.AGVCrossMagParkMode == 0X03)
				{
					ModeCheckSta = CrossMagSta!=0X00?0X01:0X00;
				}
				else if(g_HostData.AGVCrossMagParkMode == 0X00 || g_HostData.AGVCrossMagParkMode == 0X02)
				{
					ModeCheckSta = CrossMagSta==0X03?0X01:0X00;
				}
			}
			/*横向巡线，检测前后磁条；即使z轴存在偏差 只检测到了某一个磁条，也要输出*/
			else if(g_HostData.ActualAGVWorkMode == AGVMode_GoTransverse)
			{
				if(g_HostData.MagneticData.Sta_F != 0x00)
				{
					CrossMagSta = CrossMagSta|0x04;
					Distance1 = (g_HostData.AGVRunDir==AGVRunDir_Left?(-1)*g_HostData.MagneticData.Distance_F:(1)*g_HostData.MagneticData.Distance_F);
				}
				if(g_HostData.MagneticData.Sta_B != 0x00)
				{
					CrossMagSta = CrossMagSta|0x08;
					Distance2 = (g_HostData.AGVRunDir==AGVRunDir_Left?(-1)*g_HostData.MagneticData.Distance_B:(1)*g_HostData.MagneticData.Distance_B);
				}
				/*通过十字磁条停车的模式 确定到位判断的标准*/
				if(g_HostData.AGVCrossMagParkMode == 0X02 || g_HostData.AGVCrossMagParkMode == 0X03)
				{
					ModeCheckSta = CrossMagSta!=0X00?0X01:0X00;
				}
				else if(g_HostData.AGVCrossMagParkMode == 0X00 || g_HostData.AGVCrossMagParkMode == 0X01)
				{
					ModeCheckSta = CrossMagSta==0X0C?0X01:0X00;
				}
			}
			else
			{
				/*模式错误的情况，什么都不做*/
			}

			/*连续行走的时候只要检测到磁条就认为行走到位*/
			if(	g_HostData.WalkGoOnFlag != 0x00)
			{
				if(flage == 1 && ModeCheckSta == 0x01)
				{
					g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_CrossMag;
				}
			}
			else
			{
				/*考虑零偏，将两组磁条的检测结果，逻辑上做个叠加；
				  十字磁条的停车和世界坐标系没有关系，只和车的运动方向有关*/
				if(g_HostData.AGV_Course == AGVCourse_Y_Negative||g_HostData.AGV_Course == AGVCourse_Y_Positive)
				{
					if(flage == 1 &&  ModeCheckSta == 0x01)
					{
						ZeroOffset_Y = (Distance1-Distance2)*ZeroOdiff_New;
						ZeroOffset_X = ZeroOffset_Y;
					}
				}
				else if(g_HostData.AGV_Course == AGVCourse_X_Negative||g_HostData.AGV_Course == AGVCourse_X_Positive)
				{
					if(flage == 1 &&  ModeCheckSta == 0x01)
					{
						ZeroOffset_Y = (Distance1-Distance2)*ZeroOdiff_New;
						ZeroOffset_X = ZeroOffset_Y;
					}
				}
				/*要求十字磁条对应 磁导航传感器数据正负*/
				/*            	前方					*/
				/*										*/
				/*          + |------| -				*/
				/*    +		|          |   -			*/
				/*  		|          |				*/
				/*	左侧	|          |	右侧		*/
				/*    -		|          |   +			*/
				/*          - |------| + 				*/
				/*                  					*/
				/*            	后方					*/
			}
		}
		break;
		
		case AGVParking_RFIDSignal:	/*检测到RFID的信号值发生阶跃进行停车*/
		{
			{
				static uint32_t	RFIDSignalCounter = 0;
				static int32_t	RFIDSignalDis = 0;
				static uint8_t 	RFIDSignal_Sta = 0x00;
				static uint8_t 	RFIDSignal_RSSIStatus = 0x00;
				static uint8_t 	RFIDSignal_RSSIStatus_last = 0x00;

				/*A.检测到目标的RFID*/
				if(flage == 1 || RFIDSignalCounter != 0)
				{
					/*B.记录检测到RFID的时间，为检测超时、超距做准备*/
					{
						if(RFIDSignalCounter == 0)
						{
							RFIDSignal_Sta = 2;
							RFIDSignalCounter = g_HostData.Sys1msCounter;
							RFIDSignalDis = g_HostData.PointToStart_Dis;
						}
					}
					/*C.过滤掉信号值等于0的干扰数据*/
					{
						if(g_HostData.RFIDData.RSSIStatus != 0)
						{
							RFIDSignal_RSSIStatus = g_HostData.RFIDData.RSSIStatus;
						}
					}
					/*D.检测判断*/
					{
						/*检测到信号值大于等于4就直接停车*/
						if(RFIDSignal_RSSIStatus >= g_HostData.Para_Index.AGV_ParkRFIDSignal)
						{
							RFIDSignal_Sta = 4;
						}
						/*稍微走过一点:信号值从3掉回2 停车*/
						else if(  RFIDSignal_RSSIStatus > g_HostData.Para_Index.AGV_ParkRFIDSignal-2
								&&RFIDSignal_RSSIStatus < RFIDSignal_RSSIStatus_last)
						{
							RFIDSignal_Sta = 5;
						}
						/*超时距离停车（默认50mm）*/
						else if(g_HostData.PointToStart_Dis > RFIDSignalDis + g_HostData.Para_Index.AGV_ParkRFIDDisLimit)
						{
							RFIDSignal_Sta = 6;
						}
						/*超时停车（默认3秒）*/
						else if(g_HostData.Sys1msCounter > RFIDSignalCounter + g_HostData.Para_Index.AGV_ParkRFIDTimeOutLimit)
						{
							RFIDSignal_Sta = 7;
						}
						RFIDSignal_RSSIStatus_last = RFIDSignal_RSSIStatus;
					}
					/*E.达到停车条件，停车 并把静态变量归零*/
					{
						if(RFIDSignal_Sta > 2)
						{
							g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_RFIDSignal;
							RFIDSignal_RSSIStatus = 0;
							RFIDSignal_RSSIStatus_last = 0;
							RFIDSignal_RSSIStatus = 0;
							RFIDSignalCounter = 0;
							RFIDSignalDis = 0;
						}
					}
				}
				/*有连续行走的标志的时候，一直读不到RFID就盲走就行了*/
				else if(g_HostData.WalkGoOnFlag != 0X00)
				{
					ZeroOffset_Y = ZeroOdiff*(g_HostData.CurrentDelta_Y-g_HostData.NextPath_Y);
					ZeroOffset_X = ZeroOdiff*(g_HostData.CurrentDelta_X-g_HostData.NextPath_X);
				}
			}
		}
		break;
		
		case AGVParking_FixQR_ODO:	/*工装PGV辅助里程计进行停车*/
		{
			static int32_t	FixQR_ODO_TargetDis = 0;
			static int32_t	FixQR_ODO_Offset = 0;

			/*A.检测到工装二维码，才开始执行里程计停车，一旦发现二维码就更新最后停车的距离*/
			if(g_HostData.FixtureQRData.PintSta != 0)
			{
				if(g_HostData.AGV_Course == AGVCourse_Y_Negative||g_HostData.AGV_Course == AGVCourse_Y_Positive)
				{
					FixQR_ODO_Offset = (g_HostData.FixtureQRData.QR_y);
				}
				else if(g_HostData.AGV_Course == AGVCourse_X_Negative||g_HostData.AGV_Course == AGVCourse_X_Positive)
				{
					FixQR_ODO_Offset = (g_HostData.FixtureQRData.QR_x);
				}
				FixQR_ODO_TargetDis = FixQR_ODO_Offset<0?(g_HostData.PointToStart_Dis-FixQR_ODO_Offset-1):(g_HostData.PointToStart_Dis+FixQR_ODO_Offset-1);
				ZeroOffset_Y = (-1)*(FixQR_ODO_Offset);
				ZeroOffset_X = ZeroOffset_Y;
			}
			/*B.已经检测到了二维码，距离达标就停车*/
			if(	   FixQR_ODO_TargetDis != 0x00 
				&& g_HostData.PointToStart_Dis >= FixQR_ODO_TargetDis)
			{
				g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_FixQR_ODO;
				FixQR_ODO_TargetDis = 0;
			}
		}
		break;
		
		case AGVParking_ParkQR_ODO:	/*地标PGV辅助里程计进行停车*/
		{
			static int32_t	ParkQR_ODO_TargetDis = 0;
			static int32_t	ParkQR_ODO_Offset = 0;

			
			/*A.检测到工装二维码，才开始执行里程计停车，一旦发现二维码就更新最后停车的距离*/
			if(g_HostData.ParkQRData.PintSta != 0
				&&g_HostData.WalkPathChange == 0X01)
			{	
				do{
					if(g_HostData.WalkGoOnFlag != 0X00)
					{
//						ZeroOffset_Y = ZeroOdiff*(g_HostData.CurrentDelta_Y-g_HostData.NextPath_Y);
//						ZeroOffset_X = ZeroOdiff*(g_HostData.CurrentDelta_X-g_HostData.NextPath_X);
						ZeroOffset_Y = g_HostData.ParkQRData.QR_y;
						ZeroOffset_X = g_HostData.ParkQRData.QR_x;
						break;
					}				
					if(g_HostData.AGVRunDir == AGVRunDir_Back||g_HostData.AGVRunDir == AGVRunDir_Forward)
					{
						ParkQR_ODO_Offset = (g_HostData.ParkQRData.QR_y);
					}
					else if(g_HostData.AGVRunDir == AGVRunDir_Left||g_HostData.AGVRunDir == AGVRunDir_Right)
					{
						ParkQR_ODO_Offset = (g_HostData.ParkQRData.QR_x);
					}
					ParkQR_ODO_TargetDis = ParkQR_ODO_Offset<0?(g_HostData.PointToStart_Dis-ParkQR_ODO_Offset-1):(g_HostData.PointToStart_Dis+ParkQR_ODO_Offset-1);
					ZeroOffset_Y = 0;
					ZeroOffset_X = 0;
					/*B.已经检测到了二维码，距离达标就停车*/
					if(	   ParkQR_ODO_TargetDis != 0x00 
//						&& g_HostData.PointToStart_Dis >= ParkQR_ODO_TargetDis
						&& ParkQR_ODO_TargetDis <= 3)
					{
						g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_FixQR_ODO;
						ParkQR_ODO_TargetDis = 0;
					}
				}while(0);
			}
			else
			{
				/*不考虑零偏，里程计到位就停车*/
				ZeroOffset_Y = ZeroOdiff*(g_HostData.CurrentDelta_Y-g_HostData.NextPath_Y);
				ZeroOffset_X = ZeroOdiff*(g_HostData.CurrentDelta_X-g_HostData.NextPath_X);
			}
		}
		break;
	}
	/*测试代码，方便全局观测*/
	g_HostData.AGVParkZeroOffset_X = ZeroOffset_X;
	g_HostData.AGVParkZeroOffset_Y = ZeroOffset_Y;
	/*执行到此处，仍没有停车动作，需要基于里程计的结果来停车*/
	if(g_HostData.WalkInPlacerRes == 0)
	{
		do
		{
			/*沿正交方向的X或者Y到位时，正常停车*/
			if(g_HostData.AGV_Course == AGVCourse_X_Positive && (0 > (g_HostData.AGVParkZeroOffset_X)))
			{
				g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_ODO_X_P;break;
			}
			if(g_HostData.AGV_Course == AGVCourse_X_Negative && (0 < (g_HostData.AGVParkZeroOffset_X)))
			{
				g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_ODO_X_N;break;
			}
			if(g_HostData.AGV_Course == AGVCourse_Y_Positive && (0 > (g_HostData.AGVParkZeroOffset_Y)))
			{
				g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_ODO_Y_P;break;
			}
			if(g_HostData.AGV_Course == AGVCourse_Y_Negative && (0 < (g_HostData.AGVParkZeroOffset_Y)))
			{
				g_HostData.WalkInPlacerRes = AGVWalkInPlacerRes_ODO_Y_N;break;
			}
		}while(0);
	}
	return g_HostData.WalkInPlacerRes;
}
/***************************************************************************************
** 函数名称: AGV_TskFunctionStaWalk
** 功能描述: 
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void AGV_TskFunctionStaWalk(void)
{
	uint8_t WalkRunSta = 0;/*0-完成行走响应，状态机跳转*/
	uint8_t res = 0;
	
	/*确保底盘程序已经将模式切换成了合适的巡线状态，否则这个函数直接返回即可*/
	{
		if(	  (g_HostData.AGVRunDir == AGVRunDir_Forward||g_HostData.AGVRunDir == AGVRunDir_Back)
			&&(g_HostData.ActualAGVWorkMode==AGVMode_GoLine))
		{
			/*正确的结果，不需要执行什么内容*/
		}
		else if(  (g_HostData.AGVRunDir == AGVRunDir_Left||g_HostData.AGVRunDir == AGVRunDir_Right)
				&&(g_HostData.ActualAGVWorkMode==AGVMode_GoTransverse))
		{
			/*正确的结果，不需要执行什么内容*/
		}
		else
		{
			return;
		}
	}
	
	/*行走坐标更新*/
	g_HostData.CurrentDelta_X = g_HostData.NavigationData.NavLidar_x;
	g_HostData.CurrentDelta_Y = g_HostData.NavigationData.NavLidar_y;
	/*行走区间更新*/
	g_HostData.PointToEnd_Dis = g_HostData.CurrentPathDelta - g_HostData.PointToStart_Dis;

	/*1.判断是否需要加速:*/
	{
		/*有允许加速标志即并且向前的目标速度小于实际速度，*/
		float Velocity_K = g_HostData.ActualForwardVelocity/g_HostData.TargetForwardVelocity;
		float Acc_K = 1.0f*g_HostData.PointToEnd_Dis/g_HostData.SlowDownDis;
		if(Acc_K > 1.0f)
		{
			Acc_K = 1.0f;
		}
		
		if(   g_HostData.WalkStopFlag != 0x00
			&&Velocity_K < Acc_K)
		{
			/*梯形加速*/
			static uint32_t AccTimer = 0;
			if(g_HostData.Sys1msCounter > AccTimer+2)
			{
				AccTimer = g_HostData.Sys1msCounter;
				g_HostData.ActualForwardVelocity += g_HostData.Para_Index.AGV_MasterHANDLEAcc/10.0f;
			}
			WalkRunSta = 3;	
		}
		else if(   g_HostData.StartAccFlag == 0
			&&g_HostData.ActualForwardVelocity < g_HostData.TargetForwardVelocity)
		{
			/*梯形加速*/
			static uint32_t AccTimer = 0;
			if(g_HostData.Sys1msCounter > AccTimer+2)
			{
				AccTimer = g_HostData.Sys1msCounter;
				g_HostData.ActualForwardVelocity += g_HostData.Para_Index.AGV_MasterHANDLEAcc/10.0f;
			}
			WalkRunSta = 1;	
		}
		/*有任务暂停的标志的情况下，触发了减速动作，为了防止行走速度太慢，强制输出一个比较小的速度；
		  后续的减速流程会进一步限制这个速度的大小*/
		else if ( g_HostData.StartAccFlag != 0x00
				&&g_HostData.TaskPauseFlag == 0x01
				&&g_HostData.ActualForwardVelocity < 50)
		{
			g_HostData.ActualForwardVelocity += g_HostData.Para_Index.AGV_MasterHANDLEAcc/20.0f;
		}
		if (g_HostData.ActualForwardVelocity >= g_HostData.TargetForwardVelocity)
		{
			g_HostData.StartAccFlag = 1;
			g_HostData.ActualForwardVelocity = g_HostData.TargetForwardVelocity;
			WalkRunSta = 2;
		}
	}
	/*测试代码，将减速比例输出到全局变量中，方便观测*/
	g_HostData.ForwardVelocity_K = g_HostData.ActualForwardVelocity/g_HostData.TargetForwardVelocity;
	/*2.判断是否需要减速；
	   不同导航方式，减速判断不同*/
	{
		if(g_HostData.WalkSlowDownFlag != 0x00)
		{
			res = AGV_WalkSlowDown();
			if(res != 0X00)
			{
				/*强制不让其再加速了*/
				g_HostData.StartAccFlag = 1;
				WalkRunSta = 4;
			}
		}
	}
	/*3.判断是否行走超过半程；
	    过半的时候路径更新标志为1*/
	{
		if( g_HostData.PointToStart_Dis >= g_HostData.CurrentPathDelta_Half
		  &&g_HostData.WalkPathChange == 0X00)
		{
			g_HostData.WalkPathChange = 0X01;
			g_HostData.CurrentPath_Tag	= pPathStation->Label;
			g_HostData.NextPath_Tag		= pPathStation->Pnext->Label;
			/*SLAM导航直接更新tag*/
			if(g_HostData.AGVNavigateMode == AGVNavigateMode_SLAM)
			{
				g_HostData.CurrentPointTag = g_HostData.CurrentPath_Tag;
			}
		}
	}
	/*4.判断是否达到行走控制的目标点位，
	    退出行走控制状态；
	    故障状态下的停车程序不会执行到这里;如果真要停车，前面第二步已经完成了减速功能*/
	{
		res = AGV_WalkInPlaceJudgment();
		/*0-没有到位 其他-已经到位*/
		if(res != 0)
		{
			WalkRunSta = 0;
		}
		/*进入这个else if 说明地标放置的不标准，比预设位置更靠前；里程计已经超了才检测到地标*/
		else if(WalkRunSta == 0)
		{
			WalkRunSta = 0XFF;
		}
		/*需要停车达到目标点位*/
		if(	  g_HostData.WalkStopFlag != 0
			&&WalkRunSta == 0)
		{
			g_HostData.ActualForwardVelocity = 0;
		}
		/*需要掠过目标点位*/
		else
		{
			/*不用操作速度*/
		}
	}
	/*A.启动停止按键响应，按键暂停时对车进行0速*/	
	{
		if(g_HostData.InputPauseTaskFlag)
		{
			g_HostData.ActualForwardVelocity = 0;
			g_HostData.StartAccFlag = 0;
		}
		
	}
	
	/*5.根据模式区分正向和横向输出摇杆值;对遥控底盘输出摇杆控制命令;*/
	{
		if(g_HostData.ActualAGVWorkMode == AGVMode_GoLine)
		{
			if(g_HostData.AGVRunDir == AGVRunDir_Forward)
			{
				g_HostData.Hxyz[1] = g_HostData.ActualForwardVelocity;
			}
			else if(g_HostData.AGVRunDir == AGVRunDir_Back)
			{
				g_HostData.Hxyz[1] = (-1)*g_HostData.ActualForwardVelocity;
			}
			
		}
		else if(g_HostData.ActualAGVWorkMode == AGVMode_GoTransverse)
		{
			if(g_HostData.AGVRunDir == AGVRunDir_Right)
			{
				g_HostData.Hxyz[0] = g_HostData.ActualForwardVelocity;
			}
			else if(g_HostData.AGVRunDir == AGVRunDir_Left)
			{
				g_HostData.Hxyz[0] = (-1)*g_HostData.ActualForwardVelocity;
			}
		}
		/*模式不对，输出归零*/
		else
		{
			g_HostData.ActualForwardVelocity = 0.0f;
			memset(g_HostData.Hxyz,0,sizeof(g_HostData.Hxyz));
		}
	}
	/*6.判断是否到位，完成行走功能，状态机跳转*/
	{
		if(WalkRunSta == 0)
		{
			g_HostData.AGV_FuncSta = AGV_FunctionSta_Init;
		}
	}
	g_HostData.WalkRunSta = WalkRunSta;
	return;
}


/************************************END OF FILE************************************/
