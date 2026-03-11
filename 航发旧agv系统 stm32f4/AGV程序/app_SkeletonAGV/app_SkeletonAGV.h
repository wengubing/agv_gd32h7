/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_SkeletonAGV.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: AGV自动运行的框架代码
  *******************************************************************************/

#ifndef _APP_SKELETONAGV_H_
#define _APP_SKELETONAGV_H_

#ifdef  APP_SKELETONAGV_GLOBALS
 #define APP_SKELETONAGV_EXT
#else
 #define APP_SKELETONAGV_EXT  extern
#endif 

/*自定义引用*/
#include <stdbool.h>
#include "STM32f4xx.h"

#include "drv_path.h"
#include "drv_pid.h"

/*位操作引用*/
#ifndef M_TST_BIT
#define M_TST_BIT(a,b)		((a) &   ((u64)0x01 << (b)))		/* 位判断,a的b位*/
#endif      		
#ifndef M_SET_BIT         		
#define M_SET_BIT(a,b)		((a) |   ((u64)0x01 << (b)))		/* 置高位,a的b位置1*/
#endif          		
#ifndef M_CLR_BIT         		
#define M_CLR_BIT(a,b)		((a) & (~((u64)0x01 << (b))))	  	/* 清零位,a的b位清0*/ 
#endif
#ifndef M_K_CLR_BIT		
#define M_K_CLR_BIT(a,b)	(a	&= (~((u64)0x01 << b)))			/* 数据a的b位清0,对a数据进行操作*/
#endif
#ifndef M_K_SET_BIT	
#define M_K_SET_BIT(a,b)	(a	|= ((u64)0x01 << b))			/* 数据位置1,对a数据进行操作*/
#endif
#ifndef M_GET_BIT
#define M_GET_BIT(a,b)		((a) &   ((u64)0x01 << (b)))		/* 位判断,a的b位*/
#endif 
 
//AutoMode切换时间定义
#define    PauseToRun_ModeTime       100    //1000    //暂停状态到运行状态等待时间，单位为秒
#define    PauseToStop_ModeTime      100    //1000    //暂停状态到停止状态等待时间，单位为秒
#define    RunToStop_ModeTime        100    //1000    //运行状态到停止状态等待时间，单位为秒
#define    RunToCancel_ModeTime      100    //1000    //运行状态到撤销状态等待时间，单位为秒
#define    CancelToStop_ModeTime     100    //3000    //撤销状态到停止状态等待时间，单位为秒

 
#ifndef V_H
	#define  V_H		        		          642            //107*6
#endif

#ifndef BlindLiftStationNumMax
	#define  BlindLiftStationNumMax				10            /*默认最多允许10个工位特殊设置成盲举*/
#endif

/*PGV类型定义*/
typedef enum 
{
    AGVPGV_NULL	= 0,
	AGVPGV_DH	= 1,	/*1：大华作为对工装PGV				*/
	AGVPGV_HF	= 2,	/*2：航发自制作为对工装PGV			*/
}
AGVPGV_TypeDef;

/*AGV底盘接口定义*/
typedef enum 
{
    AGVChassisMode_NULL 			= 0,
	AGVChassisMode_McClumWheel		= 1,	/*1：麦轮							*/
	AGVChassisMode_EntiretyDifWheel	= 2,	/*2：整车差速						*/
	AGVChassisMode_SteeringWheel	= 3,	/*3：舵轮		(电机控制舵轮转向)	*/
	AGVChassisMode_DifSteeringWheel	= 4,	/*4：差动舵轮	(差速控制舵轮转向)	*/
}
AGVChassisMode_TypeDef;

/*AGV自动导航传感器穷举*/
typedef enum 
{
	AGVInPutSensor_NULL 	= 0XAF,
	AGVInPutSensor_SLAM		= 0,	/*1：SLAM激光			*/
	AGVInPutSensor_QR_DH	= 1,	/*2：地标二维码		 	*/
	AGVInPutSensor_QR_HF	= 2,	/*3：工装二维码		 	*/
	AGVInPutSensor_RFID		= 3,	/*4：RFID  				*/
	AGVInPutSensor_Mag		= 4,	/*5：前后巡线磁条		*/
	AGVInPutSensor_CrossMag	= 5,	/*6：十字磁条			*/
	AGVInPutSensor_END		   ,	/*	 结束				*/
}
AGVInPutSensor_TypeDef;

/*AGV上平台功能定义*/
typedef enum 
{
    AGVSpecialCommand_NULL 					= 0,
	AGVSpecialCommand_CylinderButtJoint		= 1,	/*滚筒对接：控制电机使得运载的物料通过滚筒侧移到对接装置中*/
	AGVSpecialCommand_ComplexPlatformLift	= 2,	/*复数平台升降：调度系统以数字量IO的方式控制复数个平台进行升降*/
}
AGVSpecialCommand_TypeDef;


/*AGV精定位方式定义*/
typedef enum 
{
    AGVAccPosMode_NULL 				= 0,
	AGVAccPosMode_FixQR_Z			= 1,	/*1：功能：		利用工装二维码纠正Z 使得取物料时没有Z偏差
											 限定条件：	整车差速车型 */
	AGVAccPosMode_ParkQR_XZ			= 2,	/*2：功能：		利用地标二维码纠正X和Z 使得放物料时没有X和Z偏差
											 限定条件：	前后舵轮车型 */
	AGVAccPosMode_QR_SideAbove		= 3,	/*3：功能：		利用侧面一前一后2个PGV和侧面测距传感器 
														使得AGV侧面平行对接设备并且平台平行对接设备
											 限定条件：	麦轮车型 */
}
AGVAccPosMode_TypeDef;


/*AGV导航方式定义*/
typedef enum 
{
    AGVNavigateMode_NULL = 0,
	AGVNavigateMode_QR 			= 1,	/*1：二维码导航*/
	AGVNavigateMode_Magnetic 	= 2,	/*2：磁条导航  */
	AGVNavigateMode_SLAM 		= 3,	/*3：激光导航  */
    AGVNavigateMode_GPS 		= 4,	/*4：GPS导航   */
}
AGVNavigateMode_TypeDef;

/*AGV减速方式定义*/
typedef enum 
{
    AGVCorrection_NULL 		= 0,
	AGVCorrection_ODO 		= 1,	/*1：里程计到位，比例减速*/
	AGVCorrection_FixQR 	= 2,	/*2：识别到工装二维码减速*/
	AGVCorrection_RFID		= 3,	/*3：识别到RFID*/
	AGVCorrection_ODO_Fixed = 4,	/*4：里程计到位，一刀切至最低速度*/
	AGVCorrection_ODO_Rfid	= 5,	/*5：里程计到位，比例减速;其中如果检测到Rfid，直接一刀切至最低速度*/
	AGVCorrection_ODO_Mag	= 6,	/*6：里程计到位，比例减速;其中如果检测到磁条，直接一刀切至最低速度*/
	AGVCorrection_ODO_MagQR	= 7,	/*7：里程计到位，比例减速;其中如果检测到磁条或者工装二维码，直接一刀切至最低速度*/
	AGVCorrection_ODO_ParkQR= 8,	/*8：里程计到位，比例减速;其中如果检测到地标二维码，直接一刀切至最低速度*/
}
AGVCorrect_TypeDef;

/*AGV停车方式定义*/
typedef enum 
{
    AGVParking_NULL			= 0,
	AGVParking_ODO			= 1,	/*1：里程计				到位停车*/
	AGVParking_FixtureQR	= 2,	/*2：工装二维码 PGV Y 	到位停车*/
	AGVParking_ParkQR		= 3,	/*3：地标二维码 PGV Y 	到位停车*/
	AGVParking_QR			= 4,	/*4：地标+工装二维码Y 	到位停车*/
	AGVParking_RFID			= 5,	/*5：RFID  				到位停车*/
	AGVParking_CrossMag		= 6,	/*6：十字磁条到位停车	到位停车
									 补充：	磁导航下的特殊停靠方式，有诸多使用限制；
											当调度系统没有下发"十字路口"标识时自动切换为
											RFID停车方式	*/
	AGVParking_RFIDSignal	= 7,	/*7：RFID  				到位停车*/
	AGVParking_FixQR_ODO	= 8,	/*8：里程计辅助二维码 PGV Y 	到位停车*/
	
	AGVParking_ParkQR_ODO	= 9,	/*9：里程计辅助二维码 地标PGV Y 	到位停车*/
}
AGVPark_TypeDef;

/*AGV停车到位标志定义*/
typedef enum 
{
    AGVWalkInPlacerRes_NULL			= 0,
	AGVWalkInPlacerRes_Err			= 1,	/*1：	一旦检测到RFID  			判断到位*/
	AGVWalkInPlacerRes_RFID			= 2,	/*2：	走到错误的地标上  		判断到位*/
	AGVWalkInPlacerRes_ODO_X_P		= 3,	/*3：	里程计X负方向			判断到位*/
	AGVWalkInPlacerRes_ODO_X_N		= 4,	/*4：	里程计X正方向			判断到位*/
	AGVWalkInPlacerRes_ODO_Y_P		= 5,	/*5：	里程计Y负方向			判断到位*/
	AGVWalkInPlacerRes_ODO_Y_N		= 6,	/*6：	里程计Y正方向			判断到位*/
	AGVWalkInPlacerRes_CrossMag		= 7,	/*7：	十字磁条到位				判断到位*/
	AGVWalkInPlacerRes_RFIDSignal	= 8,	/*8：	RFID信号强度				判断到位*/
	AGVWalkInPlacerRes_FixQR_ODO	= 9,	/*9：	工装二维码辅助里程计 	判断到位*/
    AGVWalkInPlacerRes_ParkQR   	= 10,	/*10：	一旦检测到地标二维码		判断到位*/
	AGVWalkInPlacerRes_FixQR	   	= 11,	/*11：	工装二维码前进方向		判断到位*/
	
}
AGVWalkInPlaRes_TypeDef;


/*AGV自转方式定义*/
typedef enum 
{
    AGVPosture_NULL 		= 0,
	AGVPosture_ODO 			= 1,	/*1：利用里程计z轴	进行自转*/
	AGVPosture_QR 			= 2,	/*2：在二维码上		进行自转*/
	AGVPosture_Magnetic		= 3,	/*3：在十字磁条上	进行自转*/
	AGVPosture_ODO_Mag		= 4,	/*4：用里程计辅助，  在磁条上	进行自转*/
	AGVPosture_FixQR_Mag	= 5,	/*5：用工装二维码辅助， 在磁条上	进行自转；
										 适用于在工装下方旋转并且需要重新对正工装的情况*/
}
AGVPosture_TypeDef;

/*AGV任务类型定义*/
typedef enum 
{
    AGVTaskTypeNotify_NULL 		= 0,
	AGVTaskTypeNotify_Park		= 1,	/*1：停靠任务*/
	AGVTaskTypeNotify_Charge	= 2,	/*2：充电任务*/
	AGVTaskTypeNotify_Goods		= 3,	/*3：转运任务*/
}
AGVTaskNotify_TypeDef;

/*AGV运行状态机枚举*/
typedef enum 
{
	AGV_FunctionSta_Init 				= 0,	/*0：初始状态*/
	AGV_FunctionSta_Walk 				= 1,	/*1：行走控制状态*/
	AGV_FunctionSta_AccuratePosition	= 2,	/*2：精定位状态	*/
	AGV_FunctionSta_Posture 			= 3,	/*3：姿态控制状态*/
    AGV_FunctionSta_Altitude 			= 4,	/*4：高度控制状态*/
	AGV_FunctionSta_BerthObstruct 		= 5,	/*5：停靠锁定，等待解锁状态*/
	AGV_FunctionSta_SpecialCommand 		= 6,	/*6：上平台特殊控制功能状态*/
	AGV_FunctionSta_LegTelescopic 		= 7,	/*7：支腿控制功能状态*/
	AGV_FunctionSta_Hang 				= 8,	/*8：悬挂高度控制功能状态*/
	AGV_FunctionSta_NavigationSwitch 	= 9,	/*9：导航模式切换功能状态*/

	AGV_FunctionSta_MaxNum				= 10,	/*10：总的状态数量*/
}
AGVFunctionSta_TypeDef;

/*AGV航向枚举*/
typedef enum 
{
	AGVCourse_Null 				= 0,	/*0：空状态*/
	AGVCourse_X_Positive		= 1,	/*1：沿X轴 正方向行走(x增大)*/
	AGVCourse_X_Negative		= 2,	/*2：沿X轴 负方向行走(x减小)*/
	AGVCourse_Y_Positive		= 3,	/*3：沿Y轴 正方向行走*/
	AGVCourse_Y_Negative		= 4,	/*4：沿Y轴 负方向行走*/
	AGVCourse_Slant 			= 5,	/*5：沿其他非XY轴方向斜着行走*/
}
AGVCourse_TypeDef;

/*AGV停车方式枚举*/
typedef enum 
{
	AGVWalkParkFlag_Park			= 0,	/*0：停靠停车*/
	AGVWalkParkFlag_GetGoods		= 1,	/*1：取料停车*/
	AGVWalkParkFlag_PutGoods		= 2,	/*2：放料停车*/
	AGVWalkParkFlag_Charge			= 3,	/*3：充电停车*/
	AGVWalkParkFlag_RfidPark		= 4,	/*4：磁导航下，将停车方式自动修改成Rfid*/
	AGVWalkParkFlag_OdoPark			= 5,	/*5：磁导航下，将停车自动修改成里程计停车*/
}
AGVWalkParkFlag_TypeDef;

//AGV自动运动控制工作模式定义
typedef enum 
{
    AGVMode_NULL 			= 0,
	AGVMode_GoStill 		= 1,	/*1：静止		*/
	AGVMode_GoLine 			= 2,	/*2：正向巡线	*/
									/*3：正向直线	*/
	AGVMode_GoWalk			= 4,	/*4：正向自由	*/
									/*5：横向自由	*/
    AGVMode_GoRotation 		= 6,	/*6：自转		*/
	AGVMode_GoUpAndDown		= 7,	/*7：升降		*/
	AGVMode_GoTransverse 	= 8,	/*8：横向巡线	*/
	AGVMode_LegTelescopic 	= 9,	/*9：支腿伸缩	*/
	AGVMode_HangHigh 		= 10,	/*10：悬挂高度	*/
	AGVMode_NumMax 			= 11,	/*	 最大数量	*/
}
AGVWorkMode_TypeDef;
//告警信息定义
//发生该报警时，排查故障后自动恢复
//g_HostData.AGV_Alarm_Evt1, AGV_
typedef enum
{
	AGV_AGV_ALM_S3,                         //ALM80:调度系统急停按下
	AGV_ALM_HostScomm,                		//ALM81:调度系统通信超时
	AGV_ALM_PathPointNotMatch,          	//ALM82:自动运行路径点不匹配（AGV走错，该标签与目标标签不一致）
	AGV_ALM_RotationTimeout,            	//ALM83:自动运行自转超时

	AGV_ALM_BlindWalkTimeout,           	//ALM84:自动运行盲走超时
	AGV_ALM_OnlineTowardsNotInitialized,  	//ALM85:上线朝向未初始化	
	AGV_ALM_StartingPointNotMatch,        	//ALM86:路径起始点不匹配
	AGV_ALM_LoadNotCharge, 					//ALM87:充电条件不满足（有负载）
	
	AGV_ALM_DownloadTowardsError,        	//ALM89:下发航向错误
	AGV_ALM_LateralDeviationOver,         	//ALM91:巡线偏差过大
	AGV_ALM_NavigationRadarScoom,         	//ALM92:定位雷达通信超时（定位PGV通信超时）
	AGV_ALM_NavigationFail,              	//ALM93:自动运行导航定位失败（或未发现循迹线或定位标签） --- 行驶过程中没有发现定位标签

	AGV_ALM_PathPointNoPallet,    	      	//ALM110:未发现托盘（叉车具有）
	AGV_ALM_RiseLimitError,        			//ALM112:举升限位开关状态异常（叉车具有）
	AGV_ALM_NotFindTag,						//ALM120:自动条件不满足（进入自动时未发现标签）
	AGV_ALM_DownoadPathError,            	//ALM142:路径下载失败
	
	/*AGV新框架得出的故障*/
	AGV_ALM_ParkQRScoomFail,				/*ALMxxx:地标PGV通讯超时*/
	AGV_ALM_FixtureQRScoomFail,				/*ALMxxx:工装PGV通讯超时*/
	AGV_ALM_RFIDScoomFail,					/*ALMxxx:RFID通讯超时*/
	AGV_ALM_MagScoomFail,					/*ALMxxx:磁导航通讯超时*/
	
	AGV_ALM_MagPostureErr,					/*ALMxxx:磁条旋转过程中丢失目标磁条*/
	AGV_ALM_GoodsMoveErr,					/*ALMxxx:运载过程中工装移动*/
	AGV_ALM_SlamScoomFail,					/*ALMxxx:Slam通讯超时*/
	

	//叉车具有
    AGV_ALM_ForkPhotoeleSensor,           //ALM123:光电传感器触发（叉车具有）			
	AGV_ALM_PalletAngleDeviation,         //ALM124:托盘放置角度偏差过大（叉车具有）		
	AGV_ALM_PalletLateralDeviation,       //ALM125:托盘放置横向偏差过大（叉车具有）	
    AGV_ALM_ToolingCurTowards_Err,        //ALM126:当前工装朝向不对

	AGV_ALM_BevelRadarInside,             //ALM127:斜面激光雷达最内层触发（叉车具有）		
	AGV_ALM_ColumnRadarInside,            //ALM128:立柱激光雷达最内层触发（叉车具有）	
	AGV_ALM_WorkstatFull,            	 /*ALMxxx:工位已满*/

	AGV_ALM_MaxNum, 					
}AGV_ALARM_EVENT_TypeDef;
 


 
//执行路径状态
typedef enum
{
	ExePathState_NULL = 0,                   //无效
	ExePathState_HaveTask,                   //有任务
	ExePathState_NoTask,                     //无任务
}Exe_PathState;


//AGV运行方向
typedef enum
{
	AGVRunDir_NULL = 0,           //AGV方向为空
	AGVRunDir_Forward,            //AGV前向运动
	AGVRunDir_Back,               //AGV后退运动	
	AGVRunDir_Left,               //AGV左移运动	
	AGVRunDir_Right,              //AGV右移运动		
	AGVRunDir_Rota,               //AGV自转运动	
}AGVRun_Dir;





//AGV运行状态
typedef enum
{
	AGV_C_InitPonit = 0,          //初始点
	AGV_C_ProcessPoint,           //转折点
	AGV_C_EventPoint,             //撤销点/停止点/叉车入位点
	AGV_C_BackPoint,              //后退点    
	AGV_C_RotaPoint,              //旋转点处理  
	AGV_C_ArcPoint,               //圆弧点处理	
	AGV_C_TransversePoint,        //横移点处理  
	AGV_C_LiftPoint,              //升降点处理    
	AGV_C_BlindWalking,           //盲走中
	AGV_C_NavigationModeSwitch,   //导航切换点处理	
	AGV_C_SpacingPoint,           //叉臂开合点处理
}AGV_ContorlMode;

//AGV自动模式状态
typedef enum
{
	AutoMode_Initial = 0,         	  //上电初始化
	AutoMode_PreOperational = 1,      //预操作状态	
	AutoMode_Operational = 2,         //操作状态
	AutoMode_Run = 3,                 //运行状态
	AutoMode_Stop = 4,                //停止状态
	AutoMode_Cancel = 5,              //撤销状态
	AutoMode_Pause = 6,               //暂停状态 
}AGV_AutoMode;

//上位机控制命令
typedef enum
{
	Host_ControlInitial = 0,       //命令任务初始化
	Host_ControlStart 	= 1,       //命令任务启动
	Host_ControlPause 	= 2,       //命令任务暂停
	Host_ControlStop 	= 3,       //命令任务停止
	Host_ControlAbort 	= 4,       //命令任务撤销
	Host_ControlAutoRun = 0x7E,    //命令任务自动运行    
}Host_Control;

//上位机下载路径判断位
typedef enum
{
	DownPathJudgmentStatus_NULL = 0,    //不可下载
	DownPathJudgmentStatus_OK,          //可以下载
}Host_DownPathJudgmentStatus;

//运动控制模型定义
typedef enum
{
	Style_Control_MODEL_NULL = 0,
	Style_Remote, //遥控
	Style_Handle, //手柄
	Style_HMI,
	Style_Auto,   //自动
}Control_MODEL;

#ifndef R_AGV_PARA_START_ADDR
	#define		  R_AGV_PARA_START_ADDR		                    1100//AGV存储参数起始地址
#endif
/*********************************************************************************************
定义各类参数在DHR中的索引定义
备注:
1.定义在DHR中一段存储RAM中各个参数的索引值;
            e.g.:比如要取参数："设备类型码",用如下使用即可:DHR[R_AGV_PARA_START_ADDR + Para_Index_AGV_DeviceTypeCode]
2.
**********************************************************************************************/
typedef enum
{
    //设备信息类
    Para_Index_AGV_DeviceTypeCode = 0,	  //AGV 设备类型码 --- 1DHR  
    Para_Index_AGV_DriverSN = 1,	  //AGV 设备序列号 --- 1DHR 	
    Para_Index_AGV_HARDVERSION = 2,	  //AGV 硬件版本 --- 1DHR  
    Para_Index_AGV_SOFTVERSION = 3,	  //AGV 软件版本 --- 1DHR  
    //4 Rev

	//系统信息类
	Para_Index_AGV_RunState = 5,    //AGV 运行状态 --- 1DHR
	Para_Index_AGV_ControlMode = 6,    //AGV 控制模式 --- 1DHR
	Para_Index_AGV_RunMode = 7,    //AGV 运行模式 --- 1DHR
	Para_Index_AGV_MovementState = 8,    //AGV 运动状态 --- 1DHR
	Para_Index_AGV_BatteryVoltage = 9,    //AGV 电池电压 --- 1DHR 单位V，扩大10倍，如486 = 48.6V
	Para_Index_AGV_BatteryLevel = 10,   //AGV 电池电量 --- 1DHR  百分比0 --- 100    

	Para_Index_AGV_SystemFaultCodeLevel1_1 = 11,   //AGV 系统一级故障码 1（一级故障1字节）--- 1DHR 
	Para_Index_AGV_SystemFaultCodeLevel1_2 = 12,   //AGV 系统一级故障码 2（一级故障2字节）--- 1DHR 	
	Para_Index_AGV_SystemFaultCodeLevel1_3 = 13,   //AGV 系统一级故障码 3（一级故障1字节）--- 1DHR 
	Para_Index_AGV_SystemFaultCodeLevel1_4 = 14,   //AGV 系统一级故障码 4（一级故障2字节）--- 1DHR 	

	Para_Index_AGV_SystemFaultCodeLevel2_1 = 15,   //AGV 系统二级故障码 1（二级故障1字节）--- 1DHR 
	Para_Index_AGV_SystemFaultCodeLevel2_2 = 16,   //AGV 系统二级故障码 2（二级故障2字节）--- 1DHR 	
	Para_Index_AGV_SystemFaultCodeLevel2_3 = 17,   //AGV 系统二级故障码 3（二级故障1字节）--- 1DHR 
	Para_Index_AGV_SystemFaultCodeLevel2_4 = 18,   //AGV 系统二级故障码 4（二级故障2字节）--- 1DHR 		

	Para_Index_AGV_SystemFaultCodeLevel3_1 = 19,   //AGV 系统三级故障码 1（三级故障1字节）--- 1DHR 
	Para_Index_AGV_SystemFaultCodeLevel3_2 = 20,   //AGV 系统三级故障码 2（三级故障2字节）--- 1DHR 	
	Para_Index_AGV_SystemFaultCodeLevel3_3 = 21,   //AGV 系统三级故障码 3（三级故障1字节）--- 1DHR 
	Para_Index_AGV_SystemFaultCodeLevel3_4 = 22,   //AGV 系统三级故障码 4（三级故障2字节）--- 1DHR 				

	Para_Index_AGV_SecuritySensorState1 = 23,   //AGV 地盘避障状态 --- 1DHR    
	Para_Index_AGV_SecuritySensorState2 = 24,   //AGV 其他避障状态 --- 1DHR   	
	Para_Index_AGV_PowerSwitchStatus = 25,   //AGV 面板开关下电状态-- 1DHR   
	Pata_Inxde_AGV_MotionSwitchStatus = 26,   //AGV 运动开关状态（锁紧/释放开关） --- 1DHR
	Pata_Inxde_AGV_MatchCharge = 27,   //AGV 配套充电桩类型 --- 1DHR		
	//28 Rev
	//29 Rev			

	//AGV运行状态信息类
	Para_Index_AGV_RunningSpeed_Vx = 30,	  //AGV 实时速度Vx --- 1DHR 1mm/s
	Para_Index_AGV_RunningSpeed_Vy = 31,	  //AGV 实时速度Vy --- 1DHR 1mm/s
	Para_Index_AGV_RunningSpeed_Vz = 32,	  //AGV 实时速度Vz --- 1DHR 0.01°/s，放大100倍   
	Para_Index_AGV_CurrentXValue = 33,   //AGV 当前位置X值 --- 2DHR  
	//34		
	Para_Index_AGV_CurrentYValue = 35,   //AGV 当前位置Y值 --- 2DHR 
	//36
	Para_Index_AGV_CurrentZValue = 37,   //AGV 当前位置Z值（当前朝向） --- 1DHR  单位°，放大100倍
	Para_Index_AGV_CurrentQRCode = 38,   //AGV 当前路径标签值 --- 1DHR 
	Para_Index_AGV_CurrentPathHeading = 39,   //AGV 当前位置航向 --- 1DHR  单位°，放大100倍  
	Para_Index_AGV_LiftingPlatformHigh = 40,   //AGV 升降高度 --- 1DHR  单位mm
	Para_Index_AGV_MotorSpeedNum1 = 41,   //AGV 1#电机转速 --- 1DHR    		
	Para_Index_AGV_MotorSpeedNum2 = 42,   //AGV 2#电机转速 --- 1DHR  		
	Para_Index_AGV_MotorSpeedNum3 = 43,   //AGV 3#电机转速 --- 1DHR    		
	Para_Index_AGV_MotorSpeedNum4 = 44,   //AGV 4#电机转速 --- 1DHR   		
	//45 Rev
	//46 Rev
	//47 Rev
	//48 Rev					
	Para_Index_AGV_MotorEncoderNum1 = 49,   //AGV 1#电机编码器脉冲数 --- 2DHR	
	//50		
	Para_Index_AGV_MotorEncoderNum2 = 51,   //AGV 2#电机编码器脉冲数 --- 2DHR	
	//52
	Para_Index_AGV_MotorEncoderNum3 = 53,   //AGV 3#电机编码器脉冲数 --- 2DHR	
	//54
	Para_Index_AGV_MotorEncoderNum4 = 55,   //AGV 4#电机编码器脉冲数 --- 2DHR		
	//56		
	//57 Rev
	//58 Rev																																								
	//59 Rev
	//60 Rev																																								
	//61 Rev
	//62 Rev																																								
	//63 Rev		
	//64 Rev																																									
	Para_Index_AGV_MotorDriverNum1 = 65,   //AGV 1#电机驱动器故障代码 --- 2DHR		
	//66
	Para_Index_AGV_MotorDriverNum2 = 67,   //AGV 2#电机驱动器故障代码 --- 2DHR	
	//68		
	Para_Index_AGV_MotorDriverNum3 = 69,   //AGV 3#电机驱动器故障代码 --- 2DHR	
	//70		
	Para_Index_AGV_MotorDriverNum4 = 71,   //AGV 4#电机驱动器故障代码 --- 2DHR		
	//72		
	//73 Rev
	//74 Rev																																								
	//75 Rev
	//76 Rev																																								
	//77 Rev
	//78 Rev																																								
	//79 Rev	
	//80 Rev																																								
	Para_Index_AGV_WheelAngleNum1 = 81,	  //AGV 1#模组角度,0.01° --- 1DHR
	Para_Index_AGV_WheelAngleNum2 = 82,	  //AGV 2#模组角度,0.01° --- 1DHR																																								
	//83 Rev
	//84 Rev		
	Para_Index_AGV_MagneticData_F = 85,	  //AGV 前磁导航 (现对于传感器中心),1mm --- 1DHR
	Para_Index_AGV_MagneticData_B = 86,	  //AGV 后磁导航 (现对于传感器中心),1mm --- 1DHR
	Para_Index_AGV_MagneticData_L = 87,	  //AGV 左磁导航 (现对于传感器中心),1mm --- 1DHR
	Para_Index_AGV_MagneticData_R = 88,	  //AGV 右磁导航 (现对于传感器中心),1mm --- 1DHR																																								
	Para_Index_AGV_RFID_Value = 89,	  //AGV RFID标签数据,0~65535 --- 1DHR																																								
	Para_Index_AGV_QRCodeData = 90,	  //AGV 二维码值 --- 1DHR 
	Para_Index_AGV_QRCodeData_x = 91,	  //AGV 二维码x角度,1mm --- 1DHR 
	Para_Index_AGV_QRCodeData_y = 92,	  //AGV 二维码y角度,1mm --- 1DHR 
	Para_Index_AGV_QRCodeData_angle = 93,	  //AGV 二维码角度,0.1° --- 1DHR 																																								
	Para_Index_AGV_IOSensorState = 94,	  //AGV IO传感器状态 --- 1DHR 																																										
	Para_Index_AGV_BatTotalCurrent = 95,	  //AGV 电池总电流 --- 1DHR 		
	Para_Index_AGV_BatHealthStatus = 96,	  //AGV 电池健康状态 --- 1DHR 	
	//97 Rev																																									
	//98 Rev
	//99 Rev																																											
	//100 Rev																																									
	Para_Index_AGV_NavigationStatusFeedback = 101,  //AGV 定位导航反馈状态
	//102 Rev		
	//103 Rev		
	//104 Rev																																										

	//设备控制判断类
	Para_Index_AGV_ExecutionPathState = 105,  //AGV 执行路径状态 --- 1DHR
	Para_Index_AGV_CurrentPathChargeDir = 106,  //AGV 当前位置充电方向	--- 1DHR			
	Para_Index_AGV_ChargingState = 107,  //AGV 充电状态 --- 1DHR
	Para_Index_AGV_ContinuePathState = 108,  //AGV 继续路径标志 --- 1DHR
	Para_Index_AGV_CustomStatusBits = 109,  //AGV 自定义状态位 --- 1DHR

	//设备控制判断类
	Para_Index_AGV_EMGControl = 110,  //AGV 上位机急停控制 --- 1DHR   
	Para_Index_AGV_EMGControlRecovery = 111,  //AGV 上位机急停恢复 --- 1DHR 
	Para_Index_AGV_AntiCcollisionRecovery = 112,  //AGV 防撞触边恢复 --- 1DHR 			
	Para_Index_AGV_TaskControlCode = 113,  //AGV 任务控制码 --- 1DHR 		
	Para_Index_Host_StartStatus = 114,  //调度系统第一次启动标志 --- 1DHR 		
	Para_Index_AGV_ChargingControl = 115,  //AGV 充电控制 --- 1DHR  
	Para_Index_AGV_SecuritySensorControl = 116,  //AGV 安全设备开关状态 --- 1DHR  	
	Para_Index_AGV_ClearAllAlalrm = 117,  //调度系统清除设备报警		
	//118 Rev		
	Para_Index_AGV_TaskTypeNotify = 119,  //任务类型通知 --- 1DHR	0-无效	1-停靠任务	2-充电任务	3-转运任务	4-联动搬运任务

	//路径下载信息
	Para_Index_AGV_WayPointDownloadStatus = 120,  //AGV 路径下载状态 --- 1DHR   
	Para_Index_AGV_WayPointUpdateQuantify = 121,  //AGV 路径更新数量 --- 1DHR  		
	//路径1命令位置信息
	Para_Index_AGV_WayPointSequence1 = 122,  //AGV 路径1位置序列号 --- 1DHR   	  字节1	
	Para_Index_AGV_WayPointLabelIndex1 = 123,  //AGV 路径1位置标签值 --- 1DHR   
	Para_Index_AGV_WayPointXValue1 = 124,  //AGV 路径1位置x坐标值 --- 2DHR   
	//125		
	Para_Index_AGV_WayPointYValue1 = 126,  //AGV 路径1位置y坐标值 --- 2DHR   
	//127		
	Para_Index_AGV_WayPointZValue1 = 128,  //AGV 路径1位置z坐标值（朝向） --- 1DHR   																																								
	Para_Index_AGV_WayPointPlatformHigh1 = 129,  //AGV 路径1位置升降高度 --- 1DHR   
	Para_Index_AGV_WayPointWaitTime1 = 130,  //AGV 路径1位置等待时间 --- 1DHR  
	Para_Index_AGV_WayPointCourse1 = 131,  //AGV 路径1位置航向 --- 1DHR  
	Para_Index_AGV_WayPointSpeedPerAndLidarScanAreaSwitch1 = 132,  //AGV 路径1位置速度百分比/激光雷达区域设置 --- 1DHR   		
	Para_Index_AGV_WayPointSecuritySensorControl1 = 133,  //AGV 路径1位置安全设备开关状态 --- 1DHR   
	Para_Index_AGV_WayPointConfirmSignalAndManualSched1 = 134,  //AGV 路径1位置确认信号/手动调度标志/ --- 1DHR   
	Para_Index_AGV_WayPointProperty1 = 135,  //AGV 路径1位置属性/方向/转弯/盲走标志/点位类型/十字路口 --- 1DHR  	
	Para_Index_AGV_WayPointCurveRadius1 = 136,  //AGV 路径1位置曲线半径参数 --- 1DHR  	
	Para_Index_AGV_WayPointCurveAngle1 = 137,  //AGV 路径1位置曲线角度参数 --- 1DHR  	
	Para_Index_AGV_WayPointDoorFrameDis1 = 138,  //AGV 路径1位置门架移动距离 --- 1DHR  	
	Para_Index_AGV_WayPointRetainOne1 = 139,  //AGV 路径1位置预留参数1 
	Para_Index_AGV_WayPointRetainTwo1 = 140,  //AGV 路径1位置预留参数2 
	Para_Index_AGV_WayPointRetainThree1 = 141,  //AGV 路径1位置预留参数3 
	Para_Index_AGV_WayPointRetainFour1 = 142,  //AGV 路径1位置预留参数4 
	Para_Index_AGV_WayPointRetainFive1 = 143,  //AGV 路径1位置预留参数5 		
	Para_Index_AGV_WayPointRetainSix1 = 144,  //AGV 路径1位置预留参数6 
	Para_Index_AGV_WayPointRetainSeven1 = 145,  //AGV 路径1位置预留参数7 
	Para_Index_AGV_WayPointRetainEight1 = 146,  //AGV 路径1位置预留参数8 
	Para_Index_AGV_WayPointRetainNine1 = 147,  //AGV 路径1位置预留参数9
	Para_Index_AGV_WayPointRetainTen1 = 148,  //AGV 路径1位置预留参数10 
	Para_Index_AGV_WayPointRetainEleven1 = 149,  //AGV 路径1位置预留参数11
	Para_Index_AGV_WayPointRetainTwelve1 = 150,  //AGV 路径1位置预留参数12
	Para_Index_AGV_WayPointRetainThirteen1 = 151,  //AGV 路径1位置预留参数13	

	//路径2命令位置信息        
	//.......
	//路径30命令位置信息                                                         //每个路径占用30个DHR

	Para_Index_AGVEnd = (122 + PathQueueNum*30),
}AGV_INDEX_FOR_DHR;



//反馈状态
typedef enum
{
	RunStatus_Null = 0,
	RunStatus_Stop,    //停止
	RunStatus_Run,     //运行
	RunStatus_Emg,     //报警
}RunStatus;

/**/
typedef enum
{
	AGV_TurnArcWalk_Straight	= 0,	/*直行*/
	AGV_TurnArcWalk_Litf		= 1,	/*左转弯*/
	AGV_TurnArcWalk_Right		= 2,	/*右转弯*/

}AGV_TurnArcWalk_TypeDef;

/*悬挂调整功能的模式*/
typedef enum
{
	AGVHangMode_Height	= 0,	/*悬挂 高度调节模式*/
	AGVHangMode_Level	= 1,	/*悬挂 水平调节模式-基于倾角传感器调整*/
	AGVHangMode_Horizon	= 2,	/*悬挂 地坪调节模式-三点悬挂都达到均值*/

}AGV_HangMode_TypeDef;

/*上位机控制数据类型定义*/
typedef struct
{
	/*避障功能*/
	struct
	{
		uint8_t	ScanningArea;           //激光雷达区域扫描设置
		unsigned short Front 	: 1;	/*当前路径点前侧避障激光雷达使能命令*/
		unsigned short Back 	: 1;    /*当前路径点后侧避障激光雷达使能命令*/
		unsigned short Left 	: 1;    /*当前路径点左侧避障激光雷达使能命令*/
		unsigned short Right 	: 1;    /*当前路径点右侧避障激光雷达使能命令*/
		unsigned short Bevel	: 1;    /*当前路径点斜面避障激光雷达使能命令*/
		unsigned short Column	: 1;    /*当前路径点立柱避障激光雷达使能命令*/
	} AGVRadio;
	/*触边功能*/
	struct
	{
		unsigned short Front 	: 1;	/*当前路径点前防撞触边使能命令*/
		unsigned short Back 	: 1;    /*当前路径点后防撞触边使能命令*/
		unsigned short Left 	: 1;    /*当前路径点左防撞触边使能命令*/
		unsigned short Right 	: 1;    /*当前路径点右防撞触边使能命令*/
	} AGVCollision;

	bool	IsInCharging;                          		//充电确认标志

	uint8_t						WaitConfirmSignal;			/*等待确认信号*/

	float						InitTEMP[2];
	/*初始化变量*/
	AGVChassisMode_TypeDef		AGVChassisMode;			/*AGV的底盘类型（会用于某些功能是否启用以及是否合法的判断）*/

	AGVNavigateMode_TypeDef		AGVNavigateMode;		/*AGV的导航方式*/
	
	uint32_t					SoftVerSion;			/*自动框架软件版本  供外部查询*/

	AGVInPutSensor_TypeDef 		InPutSensor[AGVInPutSensor_END];/*导航依赖的传感器穷举*/

	float 						AGVWalkSlowDownDisPlan[4];	/*AGV停靠的停车距离*/
	float 						AGVGoodsSlowDownDisPlan[4];	/*AGV转运的停车距离*/

	AGVCorrect_TypeDef			AGVWalkCorrectMode;		/*AGV的一般停靠减速方式*/
	AGVCorrect_TypeDef			AGVGetParkCorrectMode;	/*AGV的取料的减速方式*/
	AGVCorrect_TypeDef			AGVPutParkCorrectMode;	/*AGV的放料的减速方式*/
	AGVCorrect_TypeDef			AGVChargeCorrectMode;	/*AGV的充电的减速方式*/

	AGVPark_TypeDef				AGVWalkParkMode;		/*AGV的一般停靠停车方式*/
	AGVPark_TypeDef				AGVGetParkGoodsMode;	/*AGV的取料停车方式*/
	AGVPark_TypeDef				AGVPutParkGoodsMode;	/*AGV的放料停车方式*/
	AGVPark_TypeDef				AGVChargeParkMode;		/*AGV的充电停车方式*/

	AGVPosture_TypeDef			AGVPosture;				/*AGV的自转方式*/
	AGVAccPosMode_TypeDef		AGVGetGoodsAccPosMode;	/*AGV的取物料的精定位方式*/
	AGVAccPosMode_TypeDef		AGVPutGoodsAccPosMode;	/*AGV的放物料的精定位方式*/

	AGVSpecialCommand_TypeDef	AGVSpecialCommandMode;	/*AGV的上平台交互功能（在自动框架外部实现）*/

	AGVFunctionSta_TypeDef		AGVFunctionStaLsit[AGV_FunctionSta_MaxNum];

	/*外挂功能开关*/
	struct
	{
		unsigned int ArcWalk 			: 1;	/*AGV 弧线行驶的功能			0-AGV正交直线行驶	1-AGV正交直线+弧线 */
		unsigned int HangControl 		: 1;    /*AGV 控制悬挂的功能			0-AGV没有悬挂控制	1-AGV进行悬挂调节 */
		unsigned int BlindLift 			: 1;    /*AGV 盲举工装的功能			0-AGV不能盲举		1-AGV在特定的工位进行盲举 */
		unsigned int PutGoodsQRMag 		: 1;    /*AGV 放工装时通过工装PGV纠正xz的功能		0-AGV没有相关功能	1-AGV顶工装前计算放时的磁条目标值 */
		unsigned int RepealTaskStopMove	: 1;    /*AGV 撤销任务立即停止所有动作的功能		0-AGV撤销任务后，当前执行继续，清空下个点位			1-AGV立即停止动作 */
		unsigned int ModelAlterStopTask	: 1;    /*AGV 切出调度后，清除本地任务缓存的功能	0-AGV切出调度后，保存当前任务，下次切回调度继续执行	1-AGV切出调度时清除本地任务，下次切回调度是无任务状态 */
		unsigned int PauseTask 			: 1;    /*AGV 车端暂停任务的功能 	0-AGV不能暂停任务	1-AGV可以暂停任务*/
		unsigned int LegTelescopic		: 1;    /*AGV 控制支腿的功能			0-AGV没有支腿控制	1-AGV行走前检查支腿是否正确回收 */
		unsigned int AutoexcitationNode	: 1;    /*AGV 自发生成点位数据的功能	0-AGV受调度系统路径数据控制		1-AGV路径数据来自于单片机 */
	} AGVPendantFun;
	

	/*弧线行走的功能*/
	struct
	{
		AGV_TurnArcWalk_TypeDef	TurnFlag;				/*AGV 转弯标记				0-直线行驶	1-左转弯		2-右转弯 */
	} AGVArcWalk;


	/*悬挂高度控制的功能*/
	struct
	{
		AGV_HangMode_TypeDef	HangAdjustMode				;/*悬挂调整模式*/
		/*举升中间变量*/
		uint16_t				HangTargetHigh				;/*悬挂的目标高度，单位1mm*/
		uint16_t				HangActualHigh				;/*悬挂的实际高度，单位1mm*/
		uint16_t				HangTargetAdjustHigh		;/*悬挂高度控制的目标调节高度，单位1mm,高度控制前确定下来，不会在控制过程中发生变化*/
		uint16_t				HangStartAdjustHigh			;/*悬挂高度控制的开始前的高度，单位1mm,高度控制前确定下来，不会在控制过程中发生变化*/
		uint8_t					HangAdjustRunSta			;/*悬挂高度控制的状态，	0-初始状态  1-匀速中 2-减速中 3-到位		*/
	
	} AGVHangControl;
	

	/*盲举工装的功能*/
	struct
	{
		uint8_t  BlindLiftStationNum						;/*盲举的工位数量*/
		uint16_t BlindLiftStation[BlindLiftStationNumMax]	;/*盲举的工位列表，列表中的工位会使用一般停靠停车方式，列表外的工位点不做限制*/
	} BlindLiftFunc;
	uint8_t					AGVCrossMagParkMode;	/*AGV在磁条上停车的模式 0-正向和横向，前进方向上同时检测到左右磁条；
																			1-正向巡线时，前进方向上检测到左右任意单侧磁条或者左右都检测到；横向同0模式
																			2-横向巡线时，前进方向上检测到左右任意单侧磁条或者左右都检测到；正向同0模式
																			3-正向和横向，前进方向上检测到左右任意单侧磁条或者左右都检测到；*/
		/*诱骗调度系统的功能，让部分特殊操作回归正常业务流程 */
	struct{
		unsigned int ControlModeTrick 			: 1;	/*AGV假装切换至遥控模式；诱骗调度系统重写下发路径信息*/
	}TrickRCSFunc;

	float					InitTEMP2[2];
	/*行走中间变量*/
	uint32_t				HostFreshCnt;			/*模式切换时间*/
	uint32_t 				Position_tim_delaycnt;	/*初始位置，等待导航雷达启动时间*/

	AGV_AutoMode  			AGVAutoMode;			/*AGV命令状态*/
	Host_Control  			HostControlBuffer;		/*上位机控制命令暂存*/
	Control_MODEL 			ControlMODEL;			//控制模式	

	Exe_PathState			ExecutionPathState;		/*执行路径状态,外部DHR需要显示*/
	AGVRun_Dir 				AGVRunDir;				/*AGV运行方向*/
	AGVRun_Dir				LastAGVRunDir;			/*AGV上次运行方*/
	AGVRun_Dir				NextAGVRunDir;			/*AGV下次运行方*/
	RunStatus 				RunStatusFeedback;		/*运行状态反馈*/

	AGVFunctionSta_TypeDef 	AGV_FuncSta;			/*功能执行状态机的变量，外部观测不可修改*/
	AGVFunctionSta_TypeDef 	AGV_FuncStaLast;		/*功能执行状态机的变量，保存上一次的状态（除了初始态），用于其他业务流程使用*/
	AGVCourse_TypeDef		AGV_Course;				/*AGV的航向，归一化的结果*/
	AGVCorrect_TypeDef		AGVCorrectNowMode;		/*当前AGV的减速方式*/
	AGVPark_TypeDef			AGVParkNowMode;			/*当前AGV的停车方式*/
	AGVAccPosMode_TypeDef	AGVAccPoNowsMode;		/*当前AGV的精定位方式*/

	uint8_t					LastPointFlag;			/*最后一个点位信息响应完成标志，仅供观测不可修改*/
	uint8_t					PointUpdateFlag;		/*是否在点位上的标志，			0-不在点位 		其他-在点位上*/
	uint8_t					WalkSlowDownFlag;		/*行走巡线时的减速标志，		0-不需要减速	其他-减速*/
	uint8_t					WalkStopFlag;			/*行走巡线时的停车标志，		0-不需要停车	其他-停车*/
	uint8_t					WalkGoOnFlag;			/*行走巡线时的继续标志，		0-需要停车		1-不需要停车继续行走*/
	uint8_t					EndPathFlag;			/*行走巡线时的末端定位标志，	0-正常行走		1-已经是倒数第二个点位了需要进行末端定位*/
	AGVWalkParkFlag_TypeDef	WalkParkFlag;			/*停车方式标志，				0-停靠停车		1-取料停车		2-放料停车		
																				3-磁导航下，将RFID停车自动修改成十字磁条停车*/
	AGVWalkParkFlag_TypeDef	LastWalkParkFlag;		/*上一次的停车方式标志，当前只用作减速方式的自动延续
													  如果上一次是取料停车，那么这次很可能仍然需要取料的减速方式*/


	uint8_t					VirtualPointFlag;		/*虚拟点位标志		（0-实体点	1-虚拟点）		*/
	uint8_t					IntersectionFlag;		/*十字路口点位标志	（0-否		1-是）		*/	

	float					AGVCurrentCourse;		/*AGV当前航向*/
	float					AGVCurrentTowards;		/*AGV当前朝向*/
	uint8_t					CurrentPer;				/*当前路径速度百分比*/
	float					TargetForwardVelocity;	/*向前的目标速度，		范围:0-V_H*/
	float					ActualForwardVelocity;	/*向前的实际速度*/
	float					ActualPostureVelocity;	/*自转的实际速度；目标速度为当前挡位满摇杆的速度，调速通过修改挡位实现*/

	uint8_t					WalkPathFlag;			/**/
	uint8_t					WalkPathChange;			/**/

	uint8_t					StartAccFlag;			/*起步加速的标志位，0-需要加速	其他-加速结束*/

	int32_t					CurrentDelta_X,
							CurrentDelta_Y;         /*当前基于里程计得到的实际的路径值x，y*/
	uint16_t				CurrentPointTag;		/*当前传感器实际得到的标签值*/

	int32_t 				CurrentPath_X,
							CurrentPath_Y;          /*当前路径值，作用为进行巡线时，提供激光雷达实时值和路径起始点值的差值*/
	uint16_t				CurrentPath_Tag;		/*当前点位，调度系统给出的目标 标签值*/
	int32_t 				NextPath_X,
							NextPath_Y;	            /*下个路径值，作用为进行巡线时，提供激光雷达实时值和路径起始点值的差值*/
	uint16_t				NextPath_Tag;			/*下个点位，调度系统给出的目标 标签值*/

	int32_t 				CurrentPathDelta,		/*当前路径值的区间值*/
							CurrentPathDelta_Half;	/*当前路径值的区间值一半值*/
	int32_t 				PointToStart_Dis,		/*AGV行走实时区间值(与起点)*/
							PointToEnd_Dis;			/*AGV行走实时区间值(与终点)*/

	uint16_t 				SlowDownDis;			/*当前区间减速距离*/

	int32_t 				AGVParkZeroOffset_X		;/*当前减速的零偏，如果是0则不会停车，正常来说是从某个值将降低到0*/
	int32_t 				AGVParkZeroOffset_Y		;/**/
	uint8_t					WalkRunSta				;/*行走模式的状态，	0-初始状态  1-加速中 2-匀速中 3-减速中 4-到位		*/
	AGVWalkInPlaRes_TypeDef	WalkInPlacerRes			;/*行走到位的判断结果标志 0-初始化状态  其他-各种到位标志*/
	uint8_t					WalkSlowDownRes			;/*行走减速的判断结果标志 0-初始化状态  其他-减速标志*/

	/*梯形减速的过程控制的变量*/
	struct
	{
		uint8_t		WalkUniformSlowEn;			/*0-功能禁用 其他-通过当前速度和确定的加速度，反算减速距离；小于减速距离即开始减速*/
		uint8_t		EndAcc;				/*停止加速度（单位：m/s^2）*/
		uint8_t		MaxSpeed;			/*满摇杆最大速度（单位：m/min）*/
		float		SpeedToHxyz_k;		/*遥感到实际速度线性映射参数*/	
		uint16_t	DecelerationDistance;/*减速距离*/
		uint8_t		SpeedControlDelay;	/*速度控制函数调用时间间隔*/
	} WalkUniformSlowData;
	float					InitTEMP3[2];
	/*举升中间变量*/
	uint16_t				TargetHigh				;/*平台的目标高度，单位1mm*/
	uint16_t				ActualHigh				;/*平台的实际高度，单位1mm*/
	uint16_t				TargetAdjustHigh		;/*高度控制的目标调节高度，单位1mm,高度控制前确定下来，不会在控制过程中发生变化*/
	uint16_t				StartAdjustHigh			;/*高度控制的开始前的高度，单位1mm,高度控制前确定下来，不会在控制过程中发生变化*/
	uint8_t					AdjustRunSta			;/*高度控制的状态，	0-初始状态  1-匀速中 2-减速中 3-到位		*/

	uint8_t					GoodsFlag				;/*运载货物的标志，		0-空载状态		1-带载状态*/
	uint8_t					TaskPauseFlag			;/*任务暂停后恢复的标志，	0-初始状态		1-暂停后恢复*/
	/*举升后，行走前；保存的工装二维码结果，用于判断有没有发生工装移动*/
	struct
	{
		uint8_t		flag;			/*0-没有数据 1-数据正常，后续流程可以继续  2-没读到二维码，可能是代码设计BUG */
		/*和工装二维码一个单位*/
		int16_t 	QR_x;
		int16_t 	QR_y;
		float 		QR_z;
	} GoodsQRData;
	/*行走后，举升前；保存的工装二维码结果，用于计算放工装时，需要进行的纠偏结果*/
	struct
	{
		uint8_t		flag;			/*0-没有数据 1-数据正常，后续流程可以继续  2-没读到二维码，可能是代码设计BUG */
		/*和工装二维码一个单位*/
		int16_t 	QR_x;
		int16_t 	QR_y;
		float 		Offset_z;
	} MagParkQRData;

	float					InitTEMP4[2];
	/*自转中间变量*/
	float					TargetToward			;/*AGV的目标朝向，单位1°（0-360.00）*/
	float					ActualToward			;/*AGV的实际朝向，单位1°（0-360.00）*/
	float 					StartPostureAngle		;/*AGV开始自转的角度（在自转开始前更新，不会实时更新）*/
	int8_t					PosturOrientation		;/*AGV开始自转的方向*/
	float 					TargetPostureAngle		;/*AGV需要自转的角度（在自转开始前更新，不会实时更新）*/
	float 					ActualPostureAngle		;/*AGV已经自转的角度，实时更新（除了磁导航），0-目标*/
	uint8_t					PosturRunSta			;/*自转模式的状态，	0-初始状态  1-加速中 2-匀速中 3-减速中 4-到位		*/
	int16_t 				MagPostureZeroOffset	;/*十字磁条自转时的到位偏移判断*/

	float					InitTEMP5[2];
	/*输出变量*/
	AGVWorkMode_TypeDef		TargetAGVWorkMode		;/*目标的自动运行模式状态*/
	float					Hxyz[3]					;/*自动运行的结果，模拟了一个摇杆的原始值*/
	uint8_t  				AGVWalkSpeedVal			;/*行走速度档位值*/
	uint8_t					AGVUpDownSpeedVal		;/*升降速度档位值*/
	uint8_t					AGVHandleButtom			;/*升降按钮值  0-静止状态 1-支腿回收  2-支腿伸出*/

	uint8_t					AGVLegButtom			;/*支腿伸出按钮值  0-静止状态 1-进行伸出支腿动作  2-回收支腿动作*/

	uint64_t				AGV_Alarm_Evt1			;/*AGV专有的报警事件信息位*/
	bool					IsEMGPress				;/*自动运行车体急停的标志*/
	uint8_t					PathCurrentPointNum		;/*当前执行到路径列表的序号*/

	/*末端定位的时候，需要磁条纠偏需要加上这个值*/
	struct
	{
		uint8_t				Flag	;			/*0-没有数据 1-数据计算异常 2-数据正常，但是先别使用 4-后续流程需要将该数据拷贝一份 然后设置成8*/
		int16_t 			Mag_F	;
		int16_t 			Mag_B	;
		int16_t 			Mag_L	;
		int16_t 			Mag_R	;
	} AGVMagParkZeroOffset;


	float					ForwardVelocity_K;
	float					ActualPosture_K;

	float					InitTEMP6[2];
	/*输入变量*/
	uint32_t				Sys1msCounter;						/*系统毫秒时间*/
	AGVWorkMode_TypeDef		ActualAGVWorkMode;					/*实际的自动运行模式状态*/
	uint8_t					VirtualOdoFixFlag;                  /*虚拟里程计外部校订标志位 0-默认值，校订完成后自动变成0；1-校订中*/
	AGVTaskNotify_TypeDef	AGVTaskNotifyType;
	bool					PosValid;							/*自动运行的传感器就绪标志 true：就绪初始化完成*/
	bool					AutoOpCtrlValid;					/*自动运行是否生效标志 true：可以进行自动运行*/

	uint8_t					InputPauseTaskFlag;					/*外部输入的暂停任务的指令 0-继续任务 1-任务暂停*/
	uint8_t					InputGoOnTaskFlag;					/*外部输入的继续任务的指令 0-任务暂停 1-继续任务*/
	uint8_t					InputConfirmFlag;					/*外部输入的确认信号，调度系统下发确认标志后，
																  此标志置为非零后，才会继续执行确认标志之后的路径信息*/
	
	uint8_t					LegTelescopicSta;					/*支腿伸出状态 0-初始状态 1-支腿回收到位  2-支腿伸出到位 */
	uint8_t					NavigationSwitchResult;				/*导航模式切换结果 0-初切换成功 1-切换中  2-切换失败 3-切换超时*/
	
	/*磁条检测到的数据,最多支持4个磁条，主要用于磁导航下的十字磁条转向的功能实现*/
	struct
	{
		/*前磁条数据*/
		int16_t		Sta_F;
		int16_t 	Distance_F;
		int16_t 	Strength_F;
		/*后磁条数据*/
		int16_t		Sta_B;
		int16_t 	Distance_B;
		int16_t 	Strength_B;
		/*左磁条数据*/
		int16_t		Sta_L;
		int16_t 	Distance_L;
		int16_t 	Strength_L;
		/*磁条数据*/
		int16_t		Sta_R;
		int16_t 	Distance_R;
		int16_t 	Strength_R;
	} MagneticData;

	/*工装PGV检测到的二维码数据*/
	struct
	{
		/*工装二维码朝向必须和调度系统朝向约定方向一致，如果不一致需要在传入前处理成一致的*/
		int16_t 	QR_x;
		int16_t 	QR_y;
		float 		QR_z;
		float 		Offset_z;		/*工装二维码和AGV正交轴相差的最小角度 单位1°*/
		uint16_t 	QR_Tag;
		uint8_t 	PintSta;
	} FixtureQRData;

	/*对地PGV检测到的停车二维码数据*/
	struct
	{
		/*默认地标二维码安装时，对标的是调度系统的坐标系*/
		int16_t 	QR_x;
		int16_t 	QR_y;
		float 		QR_z;
		uint16_t 	QR_Tag;
		uint16_t 	PintSta;
	} ParkQRData;

	/*导航激光雷达数据*/
	struct
	{
		/*滤波值*/
		int64_t 	NavLidar_x;			/*激光导航雷达坐标x值 单位：1mm*/
		int64_t 	NavLidar_y;			/*激光导航雷达坐标y值 单位：1mm*/
		float 		NavLidar_z;			/*激光导航雷达坐标z值 单位：1°*/
		/*上次值*/
		uint16_t	NavLidar_Toward;	/*激光导航朝向，将Z坐标归一到最近的坐标轴 单位0.01°*/
		/*上次值*/
		float		LastNavLidar_z;		/*激光导航雷达坐标z值（上次值）*/
		uint16_t 	NavLidar_Tag;
	} NavigationData; //导航激光雷达传感器数据

	/*RFID数据*/
	struct
	{
		uint16_t ReadStatus		;/*读状态			*/
		uint8_t  RSSIStatus		;/*读卡信号强度指示	*/
		uint16_t TagValue		;/*标签值			*/
		uint16_t LastTagValue	;/*历史标签值		*/
	} RFIDData;
	

	//AGV相关需要保存参数
	struct
	{
		//AGV相关需要保存参数

		int16_t  	AGV_WayPointOffset_X             	;/*路径点匹配时X位置偏差，单位1mm，默认200*/
		int16_t  	AGV_WayPointOffset_Y             	;/*路径点匹配时Y位置偏差，单位1mm*，默认200*/

		float	 	AGV_AngAdjZoneMax                	;/*AGV自转到位的死区上限，单位1°*/
		float	 	AGV_AngAdjZoneMin                	;/*AGV自转到位的死区下限，单位1°*/
		float	 	AGV_WalkAngAdjZone               	;/*AGV行走时的允许z姿态偏差，  需要比自转到位的死区上限大，单位1°*/

		int16_t  	AGV_DeadZoneHighMax              	;/*升降死区高度上限（大于这个值，进入高度控制），当实际差值小于此高度时，认为升降到位，单位1mm			*/
		int16_t  	AGV_DeadZoneHighMin              	;/*升降死区高度下限（高度控制结束后，小于这个值），当实际差值小于此高度时，认为升降到位，单位1mm		*/
		int16_t  	AGV_LimitHigh					 	;/*升降行程，需要和AGV本身的物理升降行程保持一致，防止调度系统下发一个超出量程的值，一直卡在高度控制状态
													       单位1mm；当调节高度大于满行程的10%都是全速运转*/
		int16_t  	AGV_DeadZoneHangHighMax             ;/*悬挂升降死区高度上限（大于这个值，进入高度控制），当实际差值小于此高度时，认为升降到位，单位1mm			*/
		int16_t  	AGV_DeadZonHangeHighMin             ;/*悬挂升降死区高度下限（高度控制结束后，小于这个值），当实际差值小于此高度时，认为升降到位，单位1mm		*/
		int16_t  	AGV_LimitHangHigh					;/*悬挂升降行程，需要和AGV本身的物理升降行程保持一致，防止调度系统下发一个超出量程的值，一直卡在高度控制状态
													       单位1mm；当调节高度大于满行程的10%都是全速运转*/
		float	 	AGV_MasterHANDLEAcc              	;/*上位机摇杆数据增量值*/
		int16_t		AGV_MagStrengthLimit				;/*磁条检测的强度限制，如果低于这个值，认为没有检测到磁条，防干扰，默认200*/

		float	 	AGV_VirtualMagneticMapCoef       	;/*当存在放工装纠正xz功能时，虚拟磁导航在末端路径上，需要调整映射的参数*/

		/*报警相关参数*/
		float  		AGV_WalkDisOverrun_K           ;/*停车检测超距——需要停车的时候超过目标距离的k倍数，仍未停车就报警 默认1.2*/
		uint8_t		AGV_GoodsMove_En;
		int16_t  	AGV_GoodsMove_xLimit           ;/*工装移动检测——转运中相比转运开始前，工装 x、y、z方向的偏移大过设定阈值 单位1mm 默认30mm*/
		int16_t  	AGV_GoodsMove_yLimit           ;/*工装移动检测——转运中相比转运开始前，工装 x、y、z方向的偏移大过设定阈值 单位1mm 默认40mm*/
		float  		AGV_GoodsMove_zLimit           ;/*工装移动检测——转运中相比转运开始前，工装 x、y、z方向的偏移大过设定阈值 单位1° 默认1°*/

		int16_t  	AGV_MagParkZero_Limit			;/*取工装时偏移过大——PGV读出的x偏移加上z轴偏移映射成前后磁条后的限制值 单位1mm 默认30mm*/


		//运动参数相关

		int16_t		AGVWalkDefaultSpeedVal			;/*自动行走的默认速度，默认用6挡，不允许动态修改*/
		int16_t		AGVPostureDefaultSpeedVal		;/*自动原地自转的默认速度，默认用6挡，不允许动态修改*/

		uint16_t	AGV_QRMode_PathDeltaMin			;/*二维码导航下，所有路径中 超短路径区间，如果小于这个值，点位更新不是在半路程而是1/4路程 默认为0*/
		uint16_t	AGV_MagMode_PathDeltaMin		;/*磁导航下，    所有路径中 超短路径区间，如果小于这个值，点位更新不是在半路程而是3/4路程 默认为0*/

		uint8_t		AGV_CurrentPer_Defult			;/*当调度系统下发目标点速度为0时，AGV执行的速度百分比，一般设置为调度系统的AGV起步速度*/

		/*二维码上旋转参数相关*/
		float		AGV_Posture_QR_KMin				;/*二维码上旋转的最低速度比例，默认0.03*/
		float		AGV_Posture_QR_K				;/*二维码上旋转的整体速度比例，默认0.1*/

		/*Slam里程计上旋转参数相关*/
		float		AGV_Posture_ODO_KMin			;/*Slam里程计上旋转的最低速度比例，默认0.03*/
		float		AGV_Posture_ODO_K				;/*Slam里程计上旋转的整体速度比例，默认0.1*/

		/*磁条上旋转参数相关*/
		float		AGV_Posture_OdOMagnetic_k		;/*磁条上旋转的速度比例，默认1*/
		float		AGV_Posture_OdOMagnetic_KMin	;/*磁条上旋转的最低速度比例，默认0.1*/
		float		AGV_Posture_OdOMagnetic_Angle	;/*磁条上旋转提前减速角度，默认0*/
		int16_t		AGV_Posture_OdOMagnetic_MaxOff	;/*磁条上旋转可检测到的变化量程，单位mm 默认70mm*/
		int16_t		AGV_Postur_Orientation_diff		;/*AGV运行时，旋转的方向，默认是1；如果AGV自转方向不对，需要把其改成-1*/

		/*纯粹里程计减速参数相关*/
		int16_t  	AGV_Correction_ODO_Dis		;/*里程计减速时需要提前减速的距离，单位0.1mm，默认0*/
		float		AGV_Correction_ODO_KMin		;/*里程计减速的最低速度比例，默认0.03*/
		
		/*RFID减速参数相关*/
		int16_t		AGV_Correction_RFID_Dis		;/*识别RFID停车时需要提前减速的距离，单位0.1mm，默认0*/
		float		AGV_Correction_RFID_k		;/*识别RFID停车时的整体减速比例，默认0.03*/

		/*地标二维码减速参数相关*/
		int16_t		AGV_Correction_QR_Dis		;/*识别二维码停车时需要提前减速的距离，单位0.1mm，默认0*/
		float		AGV_Correction_QR_k			;/*识别二维码停车时的整体减速比例，默认0.05*/

		/*工装二维码和十字磁条 减速参数相关*/
		int16_t		AGV_Correction_MagQR_Dis	;/*在十字磁条上，识别二维码停车时需要提前减速的距离，单位0.1mm，默认0*/
		float		AGV_Correction_MagQR_k		;/*在十字磁条上，识别二维码停车时的整体减速比例，默认0.05*/

		/*十字磁条 减速参数相关*/
		int16_t		AGV_Correction_Mag_Dis		;/*识别十字磁条停车时需要提前减速的距离，单位0.1mm，默认0*/
		float		AGV_Correction_Mag_k		;/*识别十字磁条停车时的整体减速比例，默认0.05*/

		/*RFID停车参数相关*/
		uint16_t	AGV_ParkRFIDSignal			;/*RFID停车的信号阈值，如果RFID检测到的信号值超过它，将直接停车，默认4*/
		uint16_t	AGV_ParkRFIDDisLimit		;/*RFDI检测到之后，最长停车距离，单位1mm，默认10*/
		uint16_t	AGV_ParkRFIDTimeOutLimit	;/*RFDI检测到之后，最长停车时间，单位1ms，默认1000*/

		/*升降控制参数相关*/
		int16_t		AGV_Altitude_Dis			;/*高度控制的提前降速值，如果低于这个值，将以最低速度挡位进行升降，默认0*/
		int16_t		AGV_Altitude_SlownDis		;/*高度控制的减速距离，目标高度距离这个值，将开始从6挡位减速至1挡，默认50*/
		
		/*升降控制参数相关*/
		int16_t		AGV_HangAltitude_Dis		;/*悬挂高度控制的提前降速值，如果低于这个值，将以最低速度挡位进行升降，默认0*/
		int16_t		AGV_HangAltitude_SlownDis	;/*悬挂高度控制的减速距离，目标高度距离这个值，将开始从6挡位减速至1挡，默认50*/
		

	}Para_Index;
	//g_HostData.

}HostCtrl_DATA;

void Tsk_MasterAutoControl(void);

extern uint16_t 		*g_DHR_AGVInformation;
extern HostCtrl_DATA 	g_HostData;

#endif

/************************************END OF FILE************************************/
