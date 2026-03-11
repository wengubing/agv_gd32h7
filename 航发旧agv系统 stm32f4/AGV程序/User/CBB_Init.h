/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: CBB_Init.h
** 创 建 人: 文小兵  
** 描    述: 模块对象实例化、初始化，整理汇总到此文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年4月12日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __CBB_Init_h__
#define __CBB_Init_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  CBB_INIT_GLOBALS
	#define CBB_INIT_EXT
	#define CBB_INIT_INIT  ={0}
#else
	#define CBB_INIT_EXT  extern
	#define CBB_INIT_INIT
#endif

//全局宏定义
#define MN_NUM												4//磁导航数量
#define Color_Band_NUM										4//磁导航数量


//自定义引用	
#include "..\..\CBB\user\app_MagneticTrace\app_MagneticTrace.h"
#include "..\..\CBB\user\app_MotorResolving\app_MotorResolving.h"
#include "..\..\CBB\user\app_MecanumWheel\app_MecanumWheel.h"
#include "..\..\CBB\user\app_LiftCtrl\app_LiftCtrl.h"
#include "..\..\CBB\user\app_LoadDetection\app_LoadDetection.h"
#include "..\..\CBB\user\app_ShelfAutadj\app_ShelfAutadj.h"
#include "..\..\CBB\user\app_MagneticVirtual\app_MagneticVirtual.h"
#include "..\..\CBB\user\app_SoundLightNotice\app_SoundLightNotice.h"
#include "..\..\CBB\user\app_DataSmoothing\app_DataSmoothing.h"
#include "..\..\CBB\user\app_RadarObstacleAvoidance\app_RadarObstacleAvoidance.h"
#include "..\..\CBB\user\app_IOFilter\app_IOFilter.h"
#include "..\..\CBB\user\app_AlarmCode\app_AlarmCode.h"
#include "..\..\CBB\user\app_VoicePlayer\app_VoicePlayer.h"
#include "..\..\CBB\user\app_PairPusher\app_PairPusher.h"
#include "..\..\CBB\user\app_FixedJustify\app_FixedJustify.h"


#include "..\..\CBB\user\drv_VirtualOdometer\drv_VirtualOdometer.h"
#include "..\..\CBB\user\app_PushPullCtrl\app_PushPullCtrl.h"


#include "..\..\CBB\drivers\drv_WirelessHandle\drv_WirelessHandle.h"

#include "..\..\CBB\drivers\drv_BMSData\drv_BMSData.h"

#include "..\..\CBB\drivers\drv_ROKITData\drv_ROKITData.h"
#include "..\..\CBB\drivers\drv_RFIDData\drv_RFIDData.h"
#include "..\..\CBB\drivers\drv_MagneticNavigationData\drv_MagneticNavigationData.h"
#include "..\..\CBB\drivers\drv_PGVData\drv_PGVData.h"
#include "..\..\CBB\drivers\drv_ColorBand_PGV\drv_ColorBand_PGV.h"

#include "..\..\CBB\user\app_DiscreteMagNav\app_DiscreteMagNav.h"
#include "..\..\CBB\user\app_SlamTrackLine\app_SlamTrackLine.h"
#include "..\..\CBB\user\app_DoublePumpQuad_LiftCtrl\app_DoublePumpQuad_LiftCtrl.h"
#include "..\..\CBB\user\app_AGVAutoexcitationNode\app_AGVAutoexcitationNode.h"


#include "app_CenterPointCal.h"
#include "ObjUpdate_Assemly.h"
#include "ACC_AbsorptionCtrl.h"

//对象外部声明 drv

CBB_INIT_EXT LiftCtrl_TypeDef 	g_LiftData;							//实例化升降台控制对象 
CBB_INIT_EXT LiftCtrl_TypeDef 	g_AbsorptionData;					//实例化悬挂/减震控制对象   先完全复制平台升降的那一套，后续迭代时再特化 
CBB_INIT_EXT WH_TypeDef 		g_WHData;							//实例化遥控器数据对象

CBB_INIT_EXT PushPull_StDef		g_Push_Pull;						//实例化推拉控制对象

//对象外部声明 底盘app
CBB_INIT_EXT MotorResolving_TypeDef g_MotorResolving;				//实例化舵轮解算对象
CBB_INIT_EXT Mecanum_TypeDef g_MecanumResolving;					//实例化麦轮解算对象
CBB_INIT_EXT MagneticTrace_TypeDef g_MagneticTrace;					//实例化巡线对象
CBB_INIT_EXT SoundLightNotice_TypeDef g_SLN;						//实例化声光提示对象
CBB_INIT_EXT ROA_TypeDef g_ROA;										//实例化避障对象
CBB_INIT_EXT LoadDetection_TypeDef g_LoadDetection;					//实例化负载监测对象
CBB_INIT_EXT ShelfAutadj_TypeDef g_ShelfAutadj;						//实例化自动对中对象
CBB_INIT_EXT AirCompressor_Typedef g_AirCompressor;					//实例化空气悬挂数据对象

CBB_INIT_EXT DataSmoothing_TypeDef g_SmoothVx, g_SmoothVy, 
			g_SmoothVz, g_SmoothAx, g_SmoothAy, g_SmoothAz;			//实例化数据平滑对象
CBB_INIT_EXT DataSmoothing_TypeDef g_SmoothVxyz[3];					//实例化三轴关联数据平滑对象
CBB_INIT_EXT CenterPointCal_StDef g_CPC;							//轮系跟随巡线计算对象
CBB_INIT_EXT SlamTrackLine_TypeDef g_SlamTrackLine;					//slam导航的遥控巡线功能对象

//对象外部声明 自动app
CBB_INIT_EXT _MagneticVirtual_TypeDef g_PGVAngle_MagneticVirtual;	//实例化PGV角度纠偏对象 
CBB_INIT_EXT _MagneticVirtual_TypeDef g_VirtualMagnetic;			//实例化虚拟磁条对象
CBB_INIT_EXT OdometerData_t g_VirtualOdoData;						//实例化虚拟里程计对象

CBB_INIT_EXT VoicePlayer_TypeDef g_VoicePlayer;						//实例化语音播报对象

CBB_INIT_EXT OdometerData_t g_VirtualOdoData_MG;					//实例化虚拟里程计对象巡磁钉使用
CBB_INIT_EXT DisMagNavTypedef	DisMagNavData;						//实例化巡磁钉对象

CBB_INIT_EXT BMS_TypeDef 	g_BMSData		;						//实例化BMS对象

CBB_INIT_EXT ROKIT_TypeDef	g_ROKITData		;						//实例化ROKIT对象
CBB_INIT_EXT RFID_TypeDef 	g_RFIDData		;						//实例化RFID数据对象
CBB_INIT_EXT MN_TypeDef 	g_MNData[MN_NUM];						//实例化磁导航数据对象 
CBB_INIT_EXT ColorBand_TypeDef		g_ColorBand[Color_Band_NUM];	//实例化色带巡线数据对象

CBB_INIT_EXT AutoExcitation_TypeDef		g_AutoExcitation;			//自激AGV数据对象

CBB_INIT_EXT PGV_TypeDef 	g_GroundPGV		;						//实例化PGV数据对象 对地定位PGV
CBB_INIT_EXT PGV_TypeDef 	g_ToolPGV		;						//实例化PGV数据对象 对工装定位PGV
CBB_INIT_EXT PGV_TypeDef 	g_NavigationPGV	;						//实例化PGV数据对象 导航PGV（二维码导航方式下使用）

CBB_INIT_EXT PGV_TypeDef 	g_AssemlyGroundPGV;						//实例化PGV数据对象 联动状态下的 对地定位PGV

CBB_INIT_EXT FixedJustify_TypeDef		g_ToolJustify;				//对工装定点纠偏对象
CBB_INIT_EXT FixedJustify_TypeDef		g_GroundJustify;			//对地面定点纠偏对象
CBB_INIT_EXT FixedJustify_TypeDef		g_UltrasonicJustify;		//超声波对正对象
CBB_INIT_EXT FixedJustify_TypeDef		g_LinkageJustify;			//联动找正对象
CBB_INIT_EXT FixedJustify_TypeDef		g_SLAM_CoordinatePoint;		//SLAM自动上线对象
CBB_INIT_EXT FixedJustify_TypeDef		g_MagneticJustify;			//磁条对正对象

//方法外部声明
CBB_INIT_EXT bool CBB_ObjInit(void);
bool JoystickDataSmooth_Init(void);
bool JoystickDataSmooth_Asb_Init(void);
bool MotorResolving_Init(MotorResolving_TypeDef* p);

#endif

//***********************************END OF FILE***********************************

