/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_MotorResoling.h
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2023.12.22         
* Description: 输入控制命令，输出每个轮子的控制速度
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_MotorResolving_h__
#define __app_MotorResolving_h__


/*自定义引用*/																
#include <stdbool.h>
#include "common_def.h"
#include "basecalfunc.h"

//宏定义
//12//此版设计12 用于初始化数组大小和地址，不能任意修
//拓展到18组便于每种位置（4个象限+x正负半轴+y正负半轴+远点）、两种横向旋转方向取反的特殊模组
#define	MR_ModuleNumMxa										16//18减少到16组，挤一点RAM出来
#define MR_InfeedSkewAng              						(-90.0f)//横向转向角度
#define MR_MaxSkewHandle              						107
#define MR_K_RNew											100//

//无效的圆弧运动半径、中心坐标（无穷大 非圆弧运动）
#define InvalidRotDis										10000000.00f

/*行走模式*/
typedef enum 
{
	MR_WheelType_DifSpeed = 0,								//差速轮
	MR_WheelType_TurnIndependent,							//独立转向轮 将模组左右轮子距离一半 参数设0，取一个电机前向分速度控制驱动电机、差动分速度控制转向电机即可
	MR_WheelType_EntiretyDifSpeed,							//整车差动 
	/*整车差动时，
	（单轮子按差动模组的位置参数进行输入，即有x，y坐标，角度传感器输入无效），简化为第1个模组，只取其左右轮距参数
	输入Vx命令直接解算前向速度、Vz解算为圆弧运动半径，
	自转模式时，Vz解算为自转速度，

	输出使用，将最终电机速度下发即可
	*/
}MR_WheelType_TypeDef;

/*行走模式*/
typedef enum 
{
	MR_WALK_NULL = 0,										//0
	MR_WALK_POS,        									//正向
	MR_WALK_INF,											//横向 
	MR_WALK_ROT,											//自转
}MR_WlakModeEnum;

/*差动过程*/
typedef enum
{
	MR_ChangeIng = 0,     									//在模式切换过程中
	MR_ChangeWalk,         									//在运行过程中运行过程中
}MR_Change_TypeEnum;


/*坐标象限定义*/
typedef enum 
{
	MR_Quadrant_NULL = 0,									//0未初始化
	MR_Quadrant_Origin,        								//坐标原点
	MR_Quadrant_First,										//1象限
	MR_Quadrant_Second,										//2象限
	MR_Quadrant_Third,										//3象限
	MR_Quadrant_Fourth,										//4象限
	MR_Quadrant_XPositive,									//x正半轴
	MR_Quadrant_XMinus,										//x负半轴
	MR_Quadrant_YPositive,									//y正半轴
	MR_Quadrant_YMinus,										//y负半轴
}MR_QuadrantEnum;

/*模组坐标点 对象定义*/
typedef struct
{
	f32 X_Axis;												//mm
	f32 Y_Axis;												//mm
	MR_QuadrantEnum Quadrant;								//坐标象限
	bool Inf_Invflg;										//横向目标角度反向 1反向、2默认（顺时针90°）
	bool Rot_Invflg;										//自传目标角度反向 1反向、2默认（一三象限逆时针、二四象限顺时针）
	s16 CtrlAngMax_Left;									//解算角度范围检出阈值 向左 逆时针 正 单位1°
	s16 CtrlAngMax_Right;									//解算角度范围检出阈值 向右 顺时针 负 单位1°
	s16 RealAngMax_Left;									//实际角度范围检出阈值 向左 逆时针 正 单位1°
	s16 RealAngMax_Right;									//实际角度范围检出阈值 向右 顺时针 负 单位1°
}MR_ModulPoint_TypeDef;

//虚拟差动积分对象
//dang=dspeed/(maxspeed*2) * Wmax / （1000/CallDelayMs）
//ang=ang+dang
typedef struct
{
	f32 Wmax;												//最大角速度 °/s
	u32 LastTimeMs;											//上次时间 ms
	u32 CallDelayMs;										//ms
	f32 DiffSpeed[MR_ModuleNumMxa];
	f32 DiffW[MR_ModuleNumMxa];
	f32 VirAng[MR_ModuleNumMxa];
}MotorResolving_VirAng;

//最大半径对象
typedef struct
{
	f32 RdMax_Self;											//最大半径 mm 自己的
    f32 RdMax_AsbInput;										//最大半径 mm 联动输入
    f32 RdMax_Exe;										    //最大半径 mm 执行
}MotorResolving_RdMax_TypeDef;

//速度对象
typedef struct
{
	f32 WheelSpeedMax_Self;									//最大电机转速 rpm 自己的
    f32 WheelSpeedMax_AsbInput;								//最大电机转速 rpm 联动输入
	f32 WheelSpeedMax_Exe;									//最大电机转速 rpm 执行
	f32 K2WheelZoom;										//缩放比例
}MotorResolving_WheelSpeed_TypeDef;

//参数对象
typedef struct
{
	/*需初始化参数 其中速度的单位 被挡位表决定 内部无实际意义*/
	MR_ModulPoint_TypeDef Mxy[MR_ModuleNumMxa];				//轮系位置参数
	f32 Lc;													//每组差动轮系内两个车轮的距离一半mm
	f32 CtrlAngMax_Left, CtrlAngMax_Right;					//目标角度的控制范围 °
	f32 MotorMaxRpm;										//电机最大速度 rpm，最小差动速度10%
	f32 SkewSpeed;											//最大差动速度 rpm
	f32 SkewSpeed_ChangeIng;								//模式切换过程的
	f32 MaxSliAngPos, MaxSliAngInf;							//斜行最大控制映射角度 °
	f32 MaxRotDisPos, MinRotDisPos;							//正向圆弧运动控制映射半径 最大 最小mm
	f32 MaxRotDisInf, MinRotDisInf;							//横向
	f32	AngInPlaceMin;										//角度控制 到位角度 °
	f32 AngInPlaceMin_ChangeIng;							//角度控制 到位角度 ° 在模式切换过程中
	f32 AngSlowDown;  										//减速角度 °
	f32 AngSlowDown_ChangeIng;  							//减速角度 °在模式切换过程中
	u16 ModuleNumber; 										//解算模组数量
	u16 MotorNumber;										//解算电机数量=模组数量*2
	f32 Ksm;												//不同档位映射最大转速的映射系数
	MR_WheelType_TypeDef WheelType;
	struct
    {
        u16 WALK_CAR : 1;							        //汽车模式
        u16 CentreCtrl : 1;							        //直接圆心坐标控制模式
		u16 EntiretyDifSpeedRotModeEn : 1;					//差速轮 自转 独立模式
    }Bit;
}MR_Para_TypeDef;

//输入对象
typedef struct
{
	MR_WlakModeEnum WalkMode;								//行走模式
	f32 ActualAng[MR_ModuleNumMxa];							//实时角度[-180,180)°
	f32 Oxyz[3],Aoxyz[3];                           		//平滑处理后的摇杆堵塞值实时控制量
    Point_XY_StDef RotPoint;								//指定的圆弧运动中心点 坐标
	bool GolineFlag;										//巡线模式标志,1:处于巡线模式；0：未处于巡线模式
}MR_Input_TypeDef;

//输出对象
typedef struct
{
	bool CtrlAngLimit_Flg;									//解算超出控制角度标志
	f32 WheelLineSpeed[MR_ModuleNumMxa*2];					//电机的前向分速度 rpm
	f32 WheelDiffSpeed[MR_ModuleNumMxa*2];					//电机的差动分速度 rpm
	f32 WheelSpeed[MR_ModuleNumMxa*2];						//电机的下发合速度 rpm 左前第一个轮子面向轮子看顺时针为正，即前进时速度为负
}MR_output_TypeDef;

//模组解算
typedef struct
{
	//ms
	u32 Ms;													//ms
	//参数
	MR_Para_TypeDef Para;									//

	//实时解算输入
	MR_Input_TypeDef Input;

	//过程变量
	u8	AngAdjEn;											//差动速度计算使能
	char DrivingAxis;										//驱动轴选择
	//各模式 Lc参数、目标角度符号
	short PosModeRdSign[MR_ModuleNumMxa*2];					//
	short RotModeRdSign[MR_ModuleNumMxa*2];					//
	short RotModeAngSign[MR_ModuleNumMxa];					//
	//前向运动不同位置电机速度方向定义
	s8 WheelDirPos[MR_ModuleNumMxa * 2];
	s8 WheelDirInf[MR_ModuleNumMxa * 2];
	s8 WheelDirRot[MR_ModuleNumMxa * 2];
	//复合运动限制相关变量
	f32 ModuleAxisXMax;										//最远模组 X
	f32 ModuleAxisYMax;										//最远模组 Y
	f32 RadiusMinPos,RadiusMinInf;							//正向 横向复合运动最小圆弧运动半径
	
	//摇杆命令映射结果
	f32 SliAng;												//当前平移角度
	f32 RotDis;												//当前圆弧运动半径
	f32 AozNew; 											//非线性映射结果
	//运动几何计算
	f32 RotPointX,RotPointY;								//圆弧运动中心坐标 mm
	f32 WheelRd[MR_ModuleNumMxa*2];							//各轮子离圆心的半径mm
	f32 WheelRdUnifor[MR_ModuleNumMxa*2];					//归一化值
    MotorResolving_RdMax_TypeDef RdMax;					    //最大半径对象
	f32 TgtAng[MR_ModuleNumMxa];							//模组的目标角度°
	MotorResolving_WheelSpeed_TypeDef WheelSpeedObj;		//电机速度对象 待进一步优化

	//输出
	MR_output_TypeDef Output;

	//差动积分对象
	MotorResolving_VirAng VirAng;
}MotorResolving_TypeDef;


/*外部方法声明*/
bool MotorResolving_Main(MotorResolving_TypeDef *p,u32 in_Ms);
void MotorResolving_SignInit(MotorResolving_TypeDef *p);
void MotorResolving_VirAngRun(MotorResolving_TypeDef *p,u32 in_Ms);
void MotorResolving_WhellRdAndTgtAngCal_Rot(MotorResolving_TypeDef *p);
float MotorResolving_OffsetModCalc(float SetAng,float CurAng,MR_Change_TypeEnum Mode,MotorResolving_TypeDef *p,u8 i);
f32 MotorResolving_WheelSpeedUpDate(f32 Vls,f32 Vls1,f32 Rd,u8 ModeN);//速度合成函数
void MotorResolving_GetRadiusMin(MotorResolving_TypeDef *p);


#endif

/************************************END OF FILE************************************/

