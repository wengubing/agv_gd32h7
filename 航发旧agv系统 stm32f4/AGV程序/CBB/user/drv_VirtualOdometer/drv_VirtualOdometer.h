/********************************** Copyright ***********************************
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : drv_VirtualOdometer.h
  * Version    : V1.1
  * Author     : BBZZ、Master
  * Date       : 2023.12.22
  * Description: 此模块是虚拟里程计，依赖摇杆值进行积分，换算里程，适用于所有轮系
  *******************************************************************************/
#include "stdbool.h"
#include "stdint.h"

#ifndef _DRV_VIRTUALODOMETER_H_
#define _DRV_VIRTUALODOMETER_H_

#define Pai 3.1415926535f

#ifndef  K_RadToAngle
#define  K_RadToAngle       		      57.295779513f   /*180/Π*/
#endif

#ifndef  K_AngleToRad
#define  K_AngleToRad       		      0.0174532925f   /*Π/180*/
#endif

#define  K_MotorToWheel                   (g_odo_Data->InitData.GBRatio * Pai * g_odo_Data->InitData.Wheelradius/60) /*减速比*轮子周长*/

typedef enum      /*轮系定义*/
{
    NullWheel = 0,
    Mecanum,           /*麦轮*/
    Differential,      /*差速轮*/
    Steering,          /*单舵轮*/
    DiffSteerings,     /*差速舵轮*/
} Wheel_Type_t;


enum
{
    OdoWalkMode_NULL = 0,  
    OdoWalkMode_GoLine,		//直线行走 
	OdoWalkMode_GoTransverse,	//
    OdoWalkMode_GoRotation,	//自转
};

typedef enum
{
    OdoGyroNULL 	= 0,  
    OdoGyroA0030S	= 1,
	OdoGyroTypeEnd
}OdoGyro_TypeDef;

typedef struct
{
    /*初始化参数*/
    Wheel_Type_t	WheelType;			/*轮系类型*/
	OdoGyro_TypeDef	GyroType;			/*陀螺仪类型*/
	/*没用上的数据*/
    float			GBRatio,			/*减速比*/
					Wheelradius;		/*轮子直径*/
    int32_t			InitX,				/*初始化坐标XYZ*/
					InitY,
					InitZ;
    uint16_t		MaxSpeed,			/*电机最大转速*/
					CarLRTread,			/*车体左右轮中心距1/2，单位mm*/
					CarFBTread;			/*车体前后轮中心距1/2，单位mm*/
	/*没用上的数据*/

	/*校正数据*/
    uint8_t			WheelNum;			/*轮子数量*/
	float 			Correction_Dis;		/*里程距离修正参数*/
	float 			Correction_X;		/*里程X轴修正参数*/
	float 			Correction_Y;		/*里程Y轴修正参数*/
	float 			Correction_Z;		/*自转模式下 里程Z轴修正参数*/
	float 			Correction_Z2;		/*正向巡线下 里程Z轴修正参数*/
	float 			Correction_Z3;		/*横向巡线下 里程距离修正参数*/
	/*校正数据*/

    volatile uint32_t* SysCount;		/*系统ms计数*/
} odo_Init_t;

typedef struct
{
    odo_Init_t  InitData;

    /*实时调用输入*/
    float 		Vox,				/*实时摇杆数据*/
				Voy,
				Voz,
				Aox,
				Aoy,
				Aoz;

	float 		RotDis;				/*运动解算模块得到的圆弧运动半径*/
	float 		SliAng;				/*运动解算模块得到的斜行角度，它表示轮系的目标角度*/
	float 		GyroYaw;			/*陀螺仪的航向角，代替摇杆z积分里程数据*/
	uint16_t	WorkMode;			/*运动模式*/

	/*没用上的数据*/
	uint8_t		TurnMode;			//切换模式值
    int16_t 	SpeedWord[16],		/*Canopen下发给各个驱动器的速度字*/
				SpeedActual[16];	/*Canopen反馈各个驱动器的实际速度*/
	/*没用上的数据*/
    float 		ModuleAngle[8];		/*各驱动模组的实时角度*/


    bool 		FixFlag;			/*数据修正标志*/
    int32_t		FixX,				/*修正的坐标数据*/
				FixY,
				FixZ;

    /*查询与控制输出*/
    double 		Distance;			/*当前的行走距离*/
    int32_t 	RTx,				/*实时坐标数据*/
				RTy,
				RTz;
	int32_t 	RTxy;				/*实时坐标数据*/
    uint16_t 	Error;				/*错误码*/

} OdometerData_t;

extern bool OdometerInit ( OdometerData_t* p_Init );
extern void OdometerMain ( void );
extern void OdometerRefresh ( void );
#endif
/************************************END OF FILE************************************/
