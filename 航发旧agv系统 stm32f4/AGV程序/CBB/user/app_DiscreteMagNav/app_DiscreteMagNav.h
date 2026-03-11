/********************************** Copyright ***********************************
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_DiscreteMagNav.h
  * Version    : V2.0
  * Author     : MT
  * Date       : 2025.5.14
  * Description: 巡磁钉导航模块
  *******************************************************************************/
#ifndef _APP_DISCREATEMAGNAV_H
#define _APP_DISCREATEMAGNAV_H

#include <stdbool.h>
#include <stdint.h>

typedef   int16_t*   int16p_t;

#define DEBUG      1  /*dubug开关*/

#if DEBUG
#define STATIC
#else
#define STATIC static
#endif

#ifndef  K_RadToAngle
#define  K_RadToAngle       		      57.295779513f   /*180/Π*/
#endif

#ifndef  K_AngleToRad
#define  K_AngleToRad       		      0.0174532925f   /*Π/180*/
#endif

#define Delay_Function(A, B, C)			((A - B) > (C))
#define DelayAssign(A)                  (A = *DisMagNavP->Init.P_systemCNT)

#define Delay_Fuc(A, B, C)		    	((int32_t)(A - B) > (int32_t)(C))

//运行状态
typedef enum
{
	Mode_NULL = 0,
	Forward = 1,				/*正向前进巡线*/
	Backward = 2,				/*正向后退巡线*/
	MoveLeft = 3,				/*横向前进巡线（向左）*/
	MoveRight = 4,				/*横向后退巡线（向右）*/
	
}OperatingStatusTypedef;

typedef enum
{
    DisMagNav_Init,					/*初始化*/
    DisMagNav_Running,				/*运行态*/
    DisMagNav_Error,				/*错误态*/
	DisMagNav_Standby,				/*待机*/
	
} DisMagNavStepTypedef;

typedef enum
{
	Init_Step,						/*初始化*/
	Runing_Step,					/*运行中*/
	
}RuningStepTypedef;

typedef enum
{
	Minus,							/*负强度*/
	Plus,							/*正强度*/
}MagOrientationTypedef;

typedef enum
{
	Forward_Mode,					/*前进；方向与摇杆坐标定义一致*/
	Backward_Mode,					/*后退；方向与摇杆坐标定义一致*/
}DirectionTypedef;

/*寻磁钉模式*/
typedef enum
{
	Forward_Patrol, 				/*前进巡线*/
	Back_Patrol,					/*后退巡线*/
	Left_Patrol,					/*左巡线*/
	Right_Patrol,					/*右巡线*/
}PatrolLineModeTypedef;

typedef union
{
	uint16_t ErrorWord;
	struct 
	{
		uint16_t InitDataError: 1,	/*初始化数据错误*/
				 NotFoundMag: 1,	/*没找到磁钉*/
				 bit2: 1,
				 bit3: 1,
				 bit4: 1,
				 bit5: 1,
				 bit6: 1,
				 bit7: 1,
				 bit8: 1,
				 bit9: 1,
				 bit10: 1,
				 bit11: 1,
				 bit12: 1,
				 bit13: 1,
				 bit14: 1,
				 bit15: 1;
	}ErrorBit;
}ErrorCodeTypedef;


typedef struct						/*磁导航对象*/
{	
	uint16_t MagCount,				/*当前磁钉序号*/
			 MagCount_Last;			/*上一磁钉序号*//*用于判断是否巡到磁钉*/
	bool IsFound;					/*巡到磁钉标志*/
	double	CarOffset;				/*位姿偏移*//*由磁导航数据->车位姿*/
	double	CarOffsetA;				/*位姿偏移*//*由磁导航数据->车位姿*/
	int16_t	MagOffset;				/*磁导航偏差量*/
}MagNavTypedef;

typedef struct 
{
    float Kp,
          Ki,
          Kd;
}PID_t;

typedef struct						/*初始化数据*/
{
    float SensorDisFB;    			/*前后磁导航距离*/
		  
	bool *FixFlag;					/*里程计相关*/
    int32_t *FixX,
            *FixY,
            *FixZ;
			
	int16_t MagnetStrengthMin;		/*磁导航感应磁钉强度最小值*/
	
	int16_t OffsetSlowDownLevel_1,	/*磁导航偏移减速等级阈值*//*100*/
			OffsetSlowDownLevel_2,	/*200*/
			LostMagDis;
			
	int16_t ArcDistance,   			/*弯道磁钉间距*/
			ExitArcDis;   			/*退出弯道间距*/
			
	float SlowDownLevel[3];			/*从低到高*/
			
	PID_t *PID_Arc,
		  *PID_Str;
		  
  MagOrientationTypedef MagOrientation;		/*磁钉安装方式，正强度->true，负强度->false*/
	volatile uint32_t* P_systemCNT;			/*系统毫秒计数指针*/
}InitDataTypedef;

typedef struct						/*输入数据*/
{
    int16_t FrontMagOffset,			/*前磁导航偏移*/
            BackMagOffset,			/*后磁导航偏移*/
			FrontMagStrength,		/*前磁导航磁钉强度*/
			BackMagStrength;
			
    int32_t RTx,					/*实时X坐标*/
            RTy,					/*实时Y坐标*/
            RTz;					/*实时Z坐标*/
	double Distance;				/*当前的行走距离*/
	
	double AverageSpeed;			/*当前平均速度*/
	
	float Hy;  						/*摇杆Y值*/

	DirectionTypedef MovingDirection;/*运动方向*/
	
	OperatingStatusTypedef LineWalking_Mode;		/*巡线模式*/
	OperatingStatusTypedef LineWalking_Mode_Last;	/*上一次巡线模式*/
}InputDataTypedef;

typedef struct						/*输出数据*/
{
    float FrontMagOffset,			/*前进方向前磁导航偏移*/   		/*控制无关*/
          BackMagOffset; 			/*前进方向后磁导航偏移*/
		  
	float ControlX,					/*外部直接传递给摇杆输入*/       /*控制相关*/
		  ControlY,
		  ControlZ;
		  
    bool *FixFlag;					/*里程计相关*/                  /*控制无关*/
    int32_t *FixX,
            *FixY,
            *FixZ;
			
    ErrorCodeTypedef Error;			/*错误码*/               /*控制相关*/
}OutputDataTypedef;

typedef struct
{
    InitDataTypedef Init;
    InputDataTypedef Input;
    OutputDataTypedef Output;
	
/************************中间变量************************/	

	int16p_t P_FMagOffset,         /*行进方向前磁导航数据*/  /*实时数据*/
			 P_FMagStrength,       /*磁导航强度*/
			 P_BMagOffset,         /*行进方向后磁导航数据*/
			 P_BMagStrength;
			 
	float *P_SensorDis;				/*当前方向的传感器距离*/
			 
					 
}DisMagNavTypedef;

extern float FixRatio;

extern MagNavTypedef FMagNav;
extern void DisMagNav_Main ( void );
extern bool DisMagNavInit ( DisMagNavTypedef* Pd );
extern bool DisMagNavReInit ( void );

#endif

