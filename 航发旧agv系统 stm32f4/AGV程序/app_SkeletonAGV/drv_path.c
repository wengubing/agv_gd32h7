/*
*********************************************************************************************************
*	                                  
*	文件名称 : drv_path.c
*	版    本 : V1.0
*	说    明 : AGV执行路径   
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2019-01-22 biyabc   
*
*	 Copyright (C), 2015-2020, HANGFA Co.,Ltd.
*
*********************************************************************************************************
*/
#include "drv_path.h"
#include "app_SkeletonAGV.h"
#include "string.h"

PathTypeDef PathStack[PathQueueNum];    //路径链表数据-单向链表
PathTypeDef *pPathStation;              //指针，指向路径链表数据的单个站点单元
PathType_Para PathTypePara;
u16 g_Path_PointSequenceNum[PathQueueNum]; //记录下发路径序列号


#define 	WayPointBaseNum      	30	//路径更新基准DHR数量

/**********************************************************************************************************
*	函 数 名: InitNavMapASystem
*	功能说明: 路径上电初始化
*	形    参:  	无
*	返 回 值: 	无
**********************************************************************************************************/
void InitNavMapASystem(Path_Init PathInit)
{
    u8 i = 0;

	  if(PathInit == Path_AllClear)
		{
				for(i=0;i<PathQueueNum;i++)
				{
						PathStack[i].PathSequence = 0;
						PathStack[i].Label = 0;
						PathStack[i].LiftHigh = 65535; 			
						PathStack[i].WaitTime = 0; 	
						PathStack[i].DoorFrameDistance = 0; 	
						PathStack[i].CurveParameters_Radius = 0; 	
						PathStack[i].Tray_RFID_TAG = 0; 	
						PathStack[i].WishboneSpacing	= 65535;				
					
						PathStack[i].Map_x = 0; 	
						PathStack[i].Map_y = 0; 
					
						PathStack[i].Towards = 0; 	                      //默认为0
						PathStack[i].Course = 0; 	
						PathStack[i].CurveParameters_Angle = 0; 	
					
						PathStack[i].SpeedPercentage = 0; 	
						PathStack[i].LidarScanAreaSelect = 1; 
					
						PathStack[i].PathProperty = PathProperty_Linear; //默认直线
						PathStack[i].TurnProperty = TurnProperty_NULL;   //默认直行

						PathStack[i].ForRadioEnalbe = false;             //默认为使能
						PathStack[i].BackRadioEnalbe = false;            //默认为使能
						PathStack[i].LeftRadioEnalbe = false;            //默认为使能
						PathStack[i].RightRadioEnalbe = false;           //默认为使能			
						PathStack[i].ForCollisionEnable = false;         //默认为使能
						PathStack[i].BackCollisionEnable = false;        //默认为使能
						PathStack[i].LeftCollisionEnable = false;        //默认为使能
						PathStack[i].RightCollisionEnable = false;       //默认为使能		
						PathStack[i].BevelRadioEnalbe = false;           //默认为使能
						PathStack[i].ColumnRadioEnalbe = false;          //默认为使能				
						PathStack[i].ConfirmSignal = false;              //默认不需要确认信号
						PathStack[i].StackCameraData = false;            //默认不需要栈板相机数据
						PathStack[i].ManualScheduling = false;           //默认为自动调度控制
						PathStack[i].BlindWalk = false;                  //默认为有信号
						PathStack[i].DirectionSign = false;              //默认为前进点
						PathStack[i].NavigationModeSwitch = false;       //默认为无切换
						
						PathStack[i].ShelfProperty = ShelfProperty_NULL;       //默认为无效					
						PathStack[i].ActionProperty = ActionProperty_NULL;     //默认为无效			
						
#if (ProjectName == Project_General)    
	                                    							 										 
													 
#elif (ProjectName == Project_YunNanXLTY)  												 
										 
						//混合导航方式需要初始化该参数，单一导航不需要
						PathStack[i].NavigationMode = Navigation_Laser;  //默认为激光导航	

#endif											
									
						g_Path_PointSequenceNum[i] = 0;

						//循环链表
						if(i == (PathQueueNum - 1))						
								PathStack[i].Pnext = &PathStack[0];	
						else            
								PathStack[i].Pnext = &PathStack[i+1];
				}  
											
				pPathStation = &PathStack[0];                        //路径指针指向起点位置    
				
				PathTypePara.g_WayPointSequenceNumLast = 0;	         //上一次的路径序号
				PathTypePara.g_MapPathStartPointCheck = 0;
				PathTypePara.g_WayPointBuf =0;	                     //执行路径链表长度
				
		}

		PathTypePara.g_MapPathValid = DownLoad_NULL;             //路径更新无效			
}

/**********************************************************************************************************
*	函 数 名: WalkPathProgram
*	功能说明: 根据上位机下发路径，自动生成链表
*	形    参：None
*	返 回 值: 路径生成成功,  0.无效,  1.有效
*   说    明：校验两个相邻点参数是否超限
*
**********************************************************************************************************/
u8 WalkPathProgram(void)
{
    /************************* 逻辑说明  *******************************************
	  *1.调度系统下发路径时，会将g_DHR_AGVInformation[Para_Index_AGV_WayPointUpdateQuantify]设置为下发的路径点数
	  *2.AGV收到该寄存器不为0（且不为0xFF），说明有新路径更新；由于该寄存器的状态需要下次做逻辑判断，而且调度系统也会读取做发送正确校验
	  *3.所以双方约定该寄存器初始化为0，AGV收到后，将该寄存器置为0xFF，通知调度系统AGV已经收到。
	  *******************************************************************************/
    //流程说明：
    //1.查看g_DHR_AGVInformation[Para_Index_AGV_WayPointUpdateQuantify]是否为0
    //2.不为0，则说明有路径更新
    //3.设置路径下载状态为1 繁忙
    //4.获取更新数量，压入路径链表中
    //5.设置路径下载状态为0 空闲/下载成功
	  //6.将g_DHR_AGVInformation[Para_Index_AGV_WayPointUpdateQuantify]设置为0xFF
    //7.重复1---5		
    u16 WayPointSequenceNum = 0;
   	u8 WayPointUpdateNum = 0, i = 0, j = 0;	

	  //更新序列号
	  WayPointSequenceNum = g_DHR_AGVInformation[Para_Index_AGV_WayPointSequence1];
		
		//更新数量  
	  WayPointUpdateNum   = g_DHR_AGVInformation[Para_Index_AGV_WayPointUpdateQuantify];
			
    //新的一个任务到来
    if(WayPointSequenceNum == 0x01 && WayPointUpdateNum != 0xFF)
    {
        InitNavMapASystem(Path_AllClear);

        //恢复路径下载空闲
        PathTypePara.g_DownloadStatus = DownLoad_NULL;   			
    }	

		if(WayPointUpdateNum !=0 && WayPointUpdateNum != 0xFF)          //说明有新的任务下载
		{
        for(i=0;i<WayPointUpdateNum;i++)
			  {
					  //获取序列号
	          WayPointSequenceNum = g_DHR_AGVInformation[Para_Index_AGV_WayPointSequence1 + i*WayPointBaseNum];
					
					  if(WayPointSequenceNum == (g_Path_PointSequenceNum[i + PathTypePara.g_WayPointBuf] + 1))
					  {
						    //正常压入程序
						}
					  else
						{
						    //路径下载出现问题
							  //查找新的序列号在之前的队列中是否出现过
						for(j=0;j<=PathTypePara.g_WayPointBuf;j++)
						{
									  //查找到序列号
								    if(WayPointSequenceNum == g_Path_PointSequenceNum[j])
										{
										    PathTypePara.g_WayPointSequenceNumLast = PathTypePara.g_WayPointBuf = j;

											  break;											
										}
								}							
						}

						//判断序列是否跳列存在
						if(WayPointSequenceNum != (PathTypePara.g_WayPointSequenceNumLast +1))
						{
								//路径序号存在跳列，路径下发不完整，如：1 2 3 5
								PathTypePara.g_MapPathValid = 0;
							
								//提示路径下载失败
								PathTypePara.g_DownloadStatus = DownLoad_SequenceError;
							
								goto errorreturn;        
						}
						
						//循环链表
						if((i + PathTypePara.g_WayPointBuf) == (PathQueueNum -1))						
										  PathStack[i + PathTypePara.g_WayPointBuf].Pnext = &PathStack[0];
						else if((i + PathTypePara.g_WayPointBuf) >= PathQueueNum)
										  PathStack[i + PathTypePara.g_WayPointBuf-PathQueueNum].Pnext = &PathStack[i + PathTypePara.g_WayPointBuf-PathQueueNum+1];
						else							
										PathStack[i + PathTypePara.g_WayPointBuf].Pnext = &PathStack[i + PathTypePara.g_WayPointBuf + 1];
				
						PathStack[i + PathTypePara.g_WayPointBuf].PathSequence           = WayPointSequenceNum;
						
						PathStack[i + PathTypePara.g_WayPointBuf].Label                  = g_DHR_AGVInformation[Para_Index_AGV_WayPointLabelIndex1 + i*WayPointBaseNum];
						
						if(PathStack[i + PathTypePara.g_WayPointBuf].Label == 0)
						{
								//提示路径下载失败
								PathTypePara.g_DownloadStatus = DownLoad_LableInvalid;
							
								goto errorreturn;						
						}
						
						g_Path_PointSequenceNum[i + PathTypePara.g_WayPointBuf] = WayPointSequenceNum;
						
						PathStack[i + PathTypePara.g_WayPointBuf].LiftHigh               = g_DHR_AGVInformation[Para_Index_AGV_WayPointPlatformHigh1 + i*WayPointBaseNum];
						PathStack[i + PathTypePara.g_WayPointBuf].WaitTime               = g_DHR_AGVInformation[Para_Index_AGV_WayPointWaitTime1 + i*WayPointBaseNum];
						PathStack[i + PathTypePara.g_WayPointBuf].DoorFrameDistance      = g_DHR_AGVInformation[Para_Index_AGV_WayPointDoorFrameDis1 + i*WayPointBaseNum];
						PathStack[i + PathTypePara.g_WayPointBuf].CurveParameters_Radius = g_DHR_AGVInformation[Para_Index_AGV_WayPointCurveRadius1 + i*WayPointBaseNum];
						PathStack[i + PathTypePara.g_WayPointBuf].Tray_RFID_TAG          = g_DHR_AGVInformation[Para_Index_AGV_WayPointRetainOne1 + i*WayPointBaseNum]&0x00ff;
						PathStack[i + PathTypePara.g_WayPointBuf].WishboneSpacing        = g_DHR_AGVInformation[Para_Index_AGV_WayPointRetainThirteen1 + i*WayPointBaseNum];
						PathStack[i + PathTypePara.g_WayPointBuf].WayPointRetain		 = g_DHR_AGVInformation[Para_Index_AGV_WayPointRetainThirteen1 + i*WayPointBaseNum];
						
						PathStack[i + PathTypePara.g_WayPointBuf].Map_x                  = (s32)(g_DHR_AGVInformation[Para_Index_AGV_WayPointXValue1 + i*WayPointBaseNum]
						                                                                + (g_DHR_AGVInformation[Para_Index_AGV_WayPointXValue1 + i*WayPointBaseNum +1] << 16));					
						PathStack[i + PathTypePara.g_WayPointBuf].Map_y                  = (s32)(g_DHR_AGVInformation[Para_Index_AGV_WayPointYValue1 + i*WayPointBaseNum]
						                                                                + (g_DHR_AGVInformation[Para_Index_AGV_WayPointYValue1 + i*WayPointBaseNum +1] << 16));

						PathStack[i + PathTypePara.g_WayPointBuf].Towards                = ((f32)g_DHR_AGVInformation[Para_Index_AGV_WayPointZValue1 + i*WayPointBaseNum])/100;
						PathStack[i + PathTypePara.g_WayPointBuf].Course                 = ((f32)g_DHR_AGVInformation[Para_Index_AGV_WayPointCourse1 + i*WayPointBaseNum])/100;		
						PathStack[i + PathTypePara.g_WayPointBuf].CurveParameters_Angle  = ((f32)g_DHR_AGVInformation[Para_Index_AGV_WayPointCurveAngle1 + i*WayPointBaseNum])/10;		

						
						
						PathStack[i + PathTypePara.g_WayPointBuf].SpeedPercentage        = g_DHR_AGVInformation[Para_Index_AGV_WayPointSpeedPerAndLidarScanAreaSwitch1 + i*WayPointBaseNum]&0x00FF;
						PathStack[i + PathTypePara.g_WayPointBuf].LidarScanAreaSelect    = (g_DHR_AGVInformation[Para_Index_AGV_WayPointSpeedPerAndLidarScanAreaSwitch1 + i*WayPointBaseNum]>>8)&0x00FF;
						
						PathStack[i + PathTypePara.g_WayPointBuf].PathProperty           = (Path_Property)(g_DHR_AGVInformation[Para_Index_AGV_WayPointProperty1 + i*WayPointBaseNum]&0x000F);				
						PathStack[i + PathTypePara.g_WayPointBuf].DirectionSign          = (Direction_Sign)((g_DHR_AGVInformation[Para_Index_AGV_WayPointProperty1 + i*WayPointBaseNum]>>4)&0x0001);
						PathStack[i + PathTypePara.g_WayPointBuf].NavigationModeSwitch   = (g_DHR_AGVInformation[Para_Index_AGV_WayPointProperty1 + i*WayPointBaseNum]>>5)&0x0001;
						PathStack[i + PathTypePara.g_WayPointBuf].NavigationMode         = (Navigation_Mode)((g_DHR_AGVInformation[Para_Index_AGV_WayPointProperty1 + i*WayPointBaseNum]>>6)&0x0003);						
						PathStack[i + PathTypePara.g_WayPointBuf].TurnProperty           = (Turn_Property)((g_DHR_AGVInformation[Para_Index_AGV_WayPointProperty1 + i*WayPointBaseNum]>>8)&0x000F);				
						PathStack[i + PathTypePara.g_WayPointBuf].BlindWalk              = (g_DHR_AGVInformation[Para_Index_AGV_WayPointProperty1 + i*WayPointBaseNum]>>12)&0x000F;	
						
						PathStack[i + PathTypePara.g_WayPointBuf].VirtualPointSta		= (g_DHR_AGVInformation[Para_Index_AGV_WayPointProperty1 + i*WayPointBaseNum]>>13)&0x0001;
						PathStack[i + PathTypePara.g_WayPointBuf].IntersectionSta		= (g_DHR_AGVInformation[Para_Index_AGV_WayPointProperty1 + i*WayPointBaseNum]>>14)&0x0001;

						PathStack[i + PathTypePara.g_WayPointBuf].ConfirmSignal          = g_DHR_AGVInformation[Para_Index_AGV_WayPointConfirmSignalAndManualSched1 + i*WayPointBaseNum]&0x000F;
						PathStack[i + PathTypePara.g_WayPointBuf].StackCameraData        = (g_DHR_AGVInformation[Para_Index_AGV_WayPointConfirmSignalAndManualSched1 + i*WayPointBaseNum]>>4)&0x000F;   
						
						//手动调度信号只在第一个路径点有效
						if(WayPointSequenceNum == 0x01)
						    PathStack[i + PathTypePara.g_WayPointBuf].ManualScheduling   = (g_DHR_AGVInformation[Para_Index_AGV_WayPointConfirmSignalAndManualSched1 + i*WayPointBaseNum]>>8)&0x000F;
						else
							  PathStack[i + PathTypePara.g_WayPointBuf].ManualScheduling   = false;
						
						PathStack[i + PathTypePara.g_WayPointBuf].ForRadioEnalbe         = M_GET_BIT(g_DHR_AGVInformation[Para_Index_AGV_WayPointSecuritySensorControl1 + i*WayPointBaseNum],SecuritySensor_ForRadio);
						PathStack[i + PathTypePara.g_WayPointBuf].BackRadioEnalbe        = M_GET_BIT(g_DHR_AGVInformation[Para_Index_AGV_WayPointSecuritySensorControl1 + i*WayPointBaseNum],SecuritySensor_BackRadio);
						PathStack[i + PathTypePara.g_WayPointBuf].LeftRadioEnalbe        = M_GET_BIT(g_DHR_AGVInformation[Para_Index_AGV_WayPointSecuritySensorControl1 + i*WayPointBaseNum],SecuritySensor_LeftRadio);
						PathStack[i + PathTypePara.g_WayPointBuf].RightRadioEnalbe       = M_GET_BIT(g_DHR_AGVInformation[Para_Index_AGV_WayPointSecuritySensorControl1 + i*WayPointBaseNum],SecuritySensor_RightRadio);
						PathStack[i + PathTypePara.g_WayPointBuf].ForCollisionEnable     = M_GET_BIT(g_DHR_AGVInformation[Para_Index_AGV_WayPointSecuritySensorControl1 + i*WayPointBaseNum],SecuritySensor_ForCollision);
						PathStack[i + PathTypePara.g_WayPointBuf].BackCollisionEnable    = M_GET_BIT(g_DHR_AGVInformation[Para_Index_AGV_WayPointSecuritySensorControl1 + i*WayPointBaseNum],SecuritySensor_BackCollision);
						PathStack[i + PathTypePara.g_WayPointBuf].LeftCollisionEnable    = M_GET_BIT(g_DHR_AGVInformation[Para_Index_AGV_WayPointSecuritySensorControl1 + i*WayPointBaseNum],SecuritySensor_LeftCollision);
						PathStack[i + PathTypePara.g_WayPointBuf].RightCollisionEnable   = M_GET_BIT(g_DHR_AGVInformation[Para_Index_AGV_WayPointSecuritySensorControl1 + i*WayPointBaseNum],SecuritySensor_RightCollision);
						PathStack[i + PathTypePara.g_WayPointBuf].BevelRadioEnalbe       = M_GET_BIT(g_DHR_AGVInformation[Para_Index_AGV_WayPointSecuritySensorControl1 + i*WayPointBaseNum],SecuritySensor_BevelRadio);
						PathStack[i + PathTypePara.g_WayPointBuf].ColumnRadioEnalbe      = M_GET_BIT(g_DHR_AGVInformation[Para_Index_AGV_WayPointSecuritySensorControl1 + i*WayPointBaseNum],SecuritySensor_ColumnRadio);	
						
						//货架方位
						PathStack[i + PathTypePara.g_WayPointBuf].ShelfProperty          = (Shelf_Property)g_DHR_AGVInformation[Para_Index_AGV_WayPointRetainTen1 + i*WayPointBaseNum];
						
						//动作属性
						PathStack[i + PathTypePara.g_WayPointBuf].ActionProperty         = (Action_Property)g_DHR_AGVInformation[Para_Index_AGV_WayPointRetainEleven1 + i*WayPointBaseNum];
						
						//动作属性
						PathStack[i + PathTypePara.g_WayPointBuf].ActionProperty         = (Action_Property)g_DHR_AGVInformation[Para_Index_AGV_WayPointRetainEleven1 + i*WayPointBaseNum];
						
						/*特殊功能将源地址的*/

						memcpy(&PathStack[i + PathTypePara.g_WayPointBuf].PathWayPointRetain,&g_DHR_AGVInformation[Para_Index_AGV_WayPointSequence1 + i*WayPointBaseNum],sizeof(uint16_t)*30);

						//上一次的路径序号更新,为下一个循环判断提供依据
						PathTypePara.g_WayPointSequenceNumLast = WayPointSequenceNum; 				
				}
        
				if((i + PathTypePara.g_WayPointBuf) >= PathQueueNum)					
				{	
					  PathStack[i + PathTypePara.g_WayPointBuf - PathQueueNum].PathSequence = 0;                       //赋值下个节点的序列号为0
					  PathStack[i + PathTypePara.g_WayPointBuf - PathQueueNum].Label = 0;                              //赋值下个节点的标签号为0
					  PathTypePara.g_WayPointBuf = (PathTypePara.g_WayPointBuf + WayPointUpdateNum - PathQueueNum);    //更新链表数组下标序列号
				}
				else
				{					
					  PathStack[i + PathTypePara.g_WayPointBuf].PathSequence = 0;                       //赋值下个节点的序列号为0	
					  PathStack[i + PathTypePara.g_WayPointBuf].Label = 0;                              //赋值下个节点的标签号为0
					  PathTypePara.g_WayPointBuf = (PathTypePara.g_WayPointBuf + WayPointUpdateNum);    //更新链表数组下标序列号
				}

        //确定链表起点
        if(WayPointSequenceNum == 1)    
        {
            pPathStation = &PathStack[0];                           //路径指针指向起点位置    
        }

				//提示路径下载成功
				PathTypePara.g_DownloadStatus = DownLoad_Success;
		
        //指示地图路径更新成功        
        PathTypePara.g_MapPathValid = 1;   				
		}
	
    errorreturn:
		
		//根据逻辑，更新数量设置为最大值
		g_DHR_AGVInformation[Para_Index_AGV_WayPointUpdateQuantify] = 0xFF;				
      		
    return PathTypePara.g_MapPathValid;
}


/*****************************************************************************************************************************/

