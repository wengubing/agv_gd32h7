/********************************** Copyright ***********************************
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
*
* FileName   : app_LoadDetection.h
* Version    : V1.0
* Author     : WenXiaoBing
* Date       : 2024.1.19
* Description: 输入压力数据，计算称重、重心计算、超载偏载判断等
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_LoadDetection_h__
#define __app_LoadDetection_h__

/*包含以下头文件*/
#include <stdbool.h>
#include "..\..\CBB\user\basecalfunc\common_def.h"

//内部宏定义
#define HYD_SENSOR_NUMAX									4//压力传感器最大数量
#define SENSOR_LF											0//左前
#define SENSOR_RF											1
#define SENSOR_B											2

#define	K2PR												0.75//压力参数
#define	d2PR												375

//point inside
typedef enum
{
	LoadDetection_PointInside_Null = 0,              		//无效
	LoadDetection_PointInside_Outside = 1,              	//
	LoadDetection_PointInside_Online = 2,              		//
	LoadDetection_PointInside_Onvertex = 3,              	//在顶点
	onvertex_Inside = 4,              						//
}LoadDetection_PointInside_TypeDef;

//点坐标对象
typedef struct __point
{
	f32 X;
	f32 Y;
}LoadDetection_Point_TypeDef;

//三角形对象
typedef struct __triangle
{
	LoadDetection_Point_TypeDef a;
	LoadDetection_Point_TypeDef b;
	LoadDetection_Point_TypeDef c;
	f32 Rross[3];
	LoadDetection_PointInside_TypeDef res_Inside;
}LoadDetection_Triangle_TypeDef;

//异常对象
typedef struct
{
	bool SingleMax;
	u16 SingleMaxCnt;										//单点压力过大 触发ms


	bool SingleMin;
	u16 SingleMinCnt;										//单点压力过小 触发ms

	bool UnbalanceCore;
	u16 UnbalanceCoreCnt;									//重心位置偏移过大

	bool OverLoad;
	u16 OverLoadCnt;
}LoadDetection_HydErr_TypeDef;

//压力传感器对象
typedef struct
{
	f32 Org;												//压力原始值
	f32 Cal;												//压力计算值 Mpa
	f32 NoLoad;												//空载压力 
	f32 Net;												//净
	char UnitNum;											//并联数量
	f32 Dim;												//油缸直径 mm
	s16 AxisX, AxisY;										//等效坐标
	f32 LeverCoefficient;									//力臂系数
	f32 S;													//截面积
}LoadDetection_HydPre_TypeDef;

//异常参数对象
typedef struct __HydErrPar
{
	f32 SingleMin;											//报警 单组压力过小
	f32 SingleMax;
	f32 OverLoad;											//超载
	LoadDetection_Triangle_TypeDef JudgeTriganle;			//重心三角形
	s32 JudgeQuadrangle[8];									//重心四边形 顶点参数，依次两个元素组成一个点坐标，mm
	u16 cnt_max;
}LoadDetection_HydErrPar_TypeDef;

//模块对象
typedef struct
{
	/*传入部分*/
	LoadDetection_HydPre_TypeDef Sensor[HYD_SENSOR_NUMAX];
	bool CalEn;
	LoadDetection_HydErrPar_TypeDef Errpar;
	LoadDetection_HydErr_TypeDef Err;
	u16 WeightHyd3to4;										//负载超过该值不执行三点切四点

	char Num;												//传感器实际数量
	f32 WeightAll;											//总重量 吨
	f32 WeightNoload;										//空载重量
	f32 WeightShow;											//显示重量（负载）
	f32 CoreX, CoreY;										//重心坐标 平台几何中心为原点 mm
	bool CoreInside;										//在指定区域内部
	Unifor_TypeDef Extreme;									//压力极值分析

	//u16 err_time_cnt;										//异常持续计数 复用
}LoadDetection_TypeDef;


/*外部方法声明*/
void LoadDetection_PointInTriangle(LoadDetection_TypeDef* p, LoadDetection_Point_TypeDef point);
void LoadDetection_InitExample(LoadDetection_TypeDef* p);
void LoadDetection_Main(LoadDetection_TypeDef* p);
void LoadDetection_GetWeight(LoadDetection_TypeDef* p);
void LoadDetection_GetCore(LoadDetection_TypeDef* p);
void LoadDetection_PointInside(LoadDetection_TypeDef* p);
void LoadDetection_Errjdg(LoadDetection_TypeDef* p);

#endif

/************************************END OF FILE************************************/
