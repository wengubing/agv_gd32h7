/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_MagneticTrace.h
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2023.12.21-20240706         
* Description: 输入磁条偏移，输出整车控制进行巡线
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_MagneticTrace_h__
#define __app_MagneticTrace_h__

#include <stdbool.h>
#include "..\..\CBB\user\basecalfunc\common_def.h"

/*磁导航控制相关*/
#define  MT_NumOfMagCtrl       								2//磁导航控制方向数量 前后
#define  MT_NoOfMagCtrlF       								0//前
#define  MT_NoOfMagCtrlB       								1//后

#define  MT_MaxRotDis_All         							65530//用于堵塞计算
#define  MT_MaxSkewHandle              						107.0f 

typedef enum
{
	MagneticTraceMode_Pos=0,								//正向巡线
	MagneticTraceMode_Inf,									//横向
}MagneticTraceMode_TypeDef;

typedef enum
{
	MagneticTrace_TraceMode_FrontAndBack=0,					//前后巡线
	MagneticTrace_TraceMode_FrontOnly,						//前侧巡线
	MagneticTrace_TraceMode_DiffVCar,						//差速轮 前侧传感器巡线
	MagneticTrace_TraceMode_MecanumFrontAndBack,			//麦轮前后巡线
	MagneticTrace_TraceMode_MecanumFrontOnly,				//麦轮前侧巡线

}MagneticTrace_TraceMode_TypeDef;


/*titan go line 计算数据结构体*/
typedef struct
{
	float A_AngF,A_AngB,A_FOB;				
	float L_AngF,L_AngB,L_FB; 
	float A_L_AngB,A_Move;
	float L_R,L_Lx,L_ly;
	short Sign1,Sign2,Sign3,Sign4,Signr;
	float Temp[5];
}TGLCalTemp;//

typedef struct
{
	/*初始化参数对象*/
	short in_MagMax[4];										//磁导航偏移数据最大值	
	unsigned short in_AngPosMax;							//正向斜行角度最大值
	unsigned short in_AngInfMax;							//横向斜行角度最大值
	unsigned short in_RadiusPosMax;							//
	unsigned short in_RadiusPosMin;							//
	unsigned short in_RadiusInfMax;							//
	unsigned short in_RadiusInfMin;							//
	unsigned short in_DistancePos;							//
	unsigned short in_DistanceInf;							//

	/*麦轮*/
	unsigned short in_CmdVxMax;								//前侧巡线模式时，前桥转向Vx命令最大值
	unsigned short in_CmdVzMax;								//差速轮巡线时，Vx命令最大值
	MagneticTrace_TraceMode_TypeDef in_TraceMode;			/*巡线模式*/

	/*实时调用参数*/
	/*分别前、后、左、右磁导航传感器，人站在车面前面向传感器，磁条偏右，数据为正*/
	short in_MagValue[4];									//磁导航偏移数据

	MagneticTraceMode_TypeDef in_Mode;						//
	short in_Velocity;										//

	/*过程对象*/
	float MagDiffCtrl[MT_NumOfMagCtrl];						//面向前方 磁条偏左为正,实际传感器靠近线为负 偏置后用于巡线 mm
	float FSpeed;											//前向运动速度，向前为正 ±600
	short GoMode;											//巡线模式 0静止 1正向 2横向
	float GolineAng[MT_NumOfMagCtrl];						//巡线调整角度单位 °         向左为正 ±aten（Lx/Ly） （540/2980） 10.27°
	
	float OutCtrlAng; 										//拟合控制斜行角度         向左为正，单位°
	float OutCtrlR;											//拟合控制半径 仅为正 mm
	float MaxAngLimit,MinRLimit;							//最大斜行--前后最大纠正角度限制值 最小半径检查值
	float MaxMagLimit;
	char flg_AngMaxLmt,flg_RMinLmt;							//限制触发标志位
	
	TGLCalTemp TglCalTemp;

	/*输出*/
	float out_CmdAngle;										//斜行控制命令
	float out_CmdRadius;									//圆弧控制命令
	float out_CmdCarAngle;									/*前桥转向巡线模式时，转向角度控制命令*/
}MagneticTrace_TypeDef;

/*外部方法声明*/
void MagneticTrace_ModeInit(MagneticTrace_TypeDef *p);
void MagneticTrace_LimitInit(MagneticTrace_TypeDef *p);
void MagneticTrace_CalCmd(MagneticTrace_TypeDef *p);
void MagneticTrace_SignInit(MagneticTrace_TypeDef *p);
void MagneticTrace_CalAngAndRadius(MagneticTrace_TypeDef *p);
bool MagneticTrace_Main(MagneticTrace_TypeDef *p);
bool MagneticTrace_InitExample(MagneticTrace_TypeDef *p);

#endif


/************************************END OF FILE************************************/

