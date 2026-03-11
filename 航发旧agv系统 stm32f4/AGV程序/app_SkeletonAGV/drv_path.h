/*
*********************************************************************************************************
*	                                  
*	文件名称 : drv_path.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C), 2015-2016, HANGFA Co.,Ltd.
*
*********************************************************************************************************
*/
#ifndef __DRV_PATH_H__
#define __DRV_PATH_H__

#ifdef		DRV_PATH_GLOBALS
	#define DRV_PATH_EXT
#else
	#define DRV_PATH_EXT  extern
#endif
    
#include "includes.h"
#if defined(ACC_WXB) 
#define     PathQueueNum    30               // 路径数目
#else
#define     PathQueueNum    50               // 路径数目
#endif

//路径清空操作
typedef enum
{
    Path_AllClear = 0,                  //全部清空
	  Path_Clear,                         //部分清空
}Path_Init;

//安全类传感器定义
typedef enum
{
	SecuritySensor_ForRadio=0,			      //前激光雷达      
	SecuritySensor_BackRadio,			        //后激光雷达   
	SecuritySensor_LeftRadio,			        //左激光雷达      
	SecuritySensor_RightRadio,			      //右激光雷达  
	SecuritySensor_ForCollision,			    //前防撞触边 
	SecuritySensor_BackCollision,			    //后防撞触边    
	SecuritySensor_LeftCollision,			    //左防撞触边 
	SecuritySensor_RightCollision,			  //右防撞触边  	
	SecuritySensor_BevelRadio,			      //斜面激光雷达  	
	SecuritySensor_ColumnRadio,			      //立柱激光雷达  	
}SecuritySensor;

//路径下载状态	
typedef enum
{
    DownLoad_NULL = 0,                        //空闲  
    DownLoad_Success,                         //下载成功	
    DownLoad_Busy,                            //下载中/繁忙  
    DownLoad_SequenceError,                   //路径下载序列号错误  
    DownLoad_WayPointNumInvalid,              //下发路径点数量错误（数量为0）  
    DownLoad_LableInvalid,                    //下载路径标签错误	
}WapDownLoad;

//路径段属性
typedef enum
{
    PathProperty_Linear = 0,                  //直线
	PathProperty_Arc,                         //圆弧
	PathProperty_BezierCurve,                 //三阶贝塞尔曲线
}Path_Property;

//路径段方向
typedef enum
{
    Direction_Forward = 0,                    //前进
    Direction_Backwards,                      //后退
}Direction_Sign;

//转弯属性
typedef enum
{
    TurnProperty_NULL = 0,                    //无效/直行
	TurnProperty_Left,                        //左转
	TurnProperty_Right,                       //右转
}Turn_Property;

//导航模式定义
typedef enum 
{
    Navigation_Laser = 0,                   //激光导航
    Navigation_Magnetic,                    //磁导航
    Navigation_DifferentialGPS,             //差分GPS
	Navigation_Mix,                         //混合导航
}Navigation_Mode;

//货架属性
typedef enum
{
    ShelfProperty_NULL = 0,                   //无效	
    ShelfProperty_Former,                     //前
    ShelfProperty_Rear,                       //后
    ShelfProperty_Left,                       //左
    ShelfProperty_Right,                      //右
}Shelf_Property;

//动作属性
typedef enum
{
    ActionProperty_NULL = 0,                 //无效	
    ActionProperty_PickUp,                   //取货
	  ActionProperty_Release,                  //放货
}Action_Property;

/*用于表示车体在逻辑数组中（导航数据阵列）所在的位置*********/
typedef struct  PathTypeDef
{  
	u16		PathSequence,                    // 路径点下发序列号（标明该路径点在该任务中的顺序号）
			Label,                           // 路径点标签值
			LiftHigh,                        // 路径点平台升降高度
			WaitTime,                        // 等待时间 上位机下发单位为s，路径填充时，转换为ms
			DoorFrameDistance,               // 门架移动距离
			CurveParameters_Radius,          // 路径曲线半径
			Tray_RFID_TAG,                   // 下发的托盘RFID号
			WishboneSpacing;                 // 叉臂开合距离
	u16		WayPointRetain;					 //

	s32		Map_x,    						 // 路径点 坐标 X                    导航激光雷达
			Map_y;  		                 // 路径点 坐标 Y                    导航激光雷达

	f32		Towards,                         // 路径点 车体位置方向角度           导航激光雷达
			Course,                          // 路径点 航向
			CurveParameters_Angle;           // 路径曲线角度         

	u8		SpeedPercentage,                 // 速度百分比
			LidarScanAreaSelect;             // 扫描区域切换
	
	Shelf_Property ShelfProperty;             //货架方位
	Action_Property ActionProperty;           //动作属性	
	Path_Property PathProperty;               // 该段路径属性
	Turn_Property TurnProperty;               // 该段路径转弯属性
	Navigation_Mode NavigationMode;           // 该路径导航模式

	bool	ForRadioEnalbe,					 // 前激光雷达使能/失能
			BackRadioEnalbe,                 // 后激光雷达使能/失能			
			LeftRadioEnalbe,                 // 左激光雷达使能/失能
			RightRadioEnalbe,                // 右激光雷达使能/失能	
			ForCollisionEnable,              // 前防撞触边使能/失能		
			BackCollisionEnable,             // 后防撞触边使能/失能	
			LeftCollisionEnable,             // 左防撞触边使能/失能	
			RightCollisionEnable,            // 右防撞触边使能/失能			
			BevelRadioEnalbe,                // 斜面激光雷达使能/失能
			ColumnRadioEnalbe,               // 立柱激光雷达使能/失能							 
			ConfirmSignal,                   // 路径点确认信号		
			ManualScheduling,                // 路径点手动调度信号
			BlindWalk,                       // 盲走信号
			StackCameraData,                 // 栈板相机识别数据使用
			DirectionSign,                   // 方向标志 False.前进点    True.后退点	
			NavigationModeSwitch;            // 导航类型切换 False.无效   True.导航类型切换
	
	bool		VirtualPointSta		;/*虚拟点位标志		（0-实体点	1-虚拟点）	*/
	bool		IntersectionSta		;/*十字路口点位标志	（0-否		1-是）		*/
	uint16_t	HangLiftHigh		;/*路径点悬挂升降高度*/ 
	struct   PathTypeDef * Pnext;			// 下一节点信息 

	uint16_t	PathWayPointRetain[30];/*路径点位的参数表；定制应用的时候使用*/


}PathTypeDef;   

//路径相关参数
typedef struct
{
	u16 g_WayPointBuf;                      //路径数存放区

	u8  g_MapPathStartPointCheck,           //路径起点检查标志   
		g_WayPointSequenceNumLast,          //记录发送序列，如存在跳列情况，报错 
		g_MapPathValid;                     //路径更新是否有效，0.无效    1.有效	

	WapDownLoad g_DownloadStatus;           //路径下载状态
}PathType_Para;

DRV_PATH_EXT PathTypeDef PathStack[PathQueueNum]; 
DRV_PATH_EXT PathTypeDef *pPathStation; 
DRV_PATH_EXT PathType_Para PathTypePara;

void InitNavMapASystem(Path_Init PathInit);
DRV_PATH_EXT u8 WalkPathProgram(void);

#endif

/*****************************************************************************************************************************/


