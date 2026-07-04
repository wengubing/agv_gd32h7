/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : basecalfunc.h
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2023.12.29         
* Description: 常见通用方法、类的定义
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __basecalfunc_h__
#define __basecalfunc_h__

#include <stdbool.h>
#include "common_def.h"

#define UNIFOR_DATA_NUMMAX									32//

//归一化数据处理对象
typedef struct
{
	char data_num;
	f32 org[UNIFOR_DATA_NUMMAX];
	f32 unifor[UNIFOR_DATA_NUMMAX];
	f32 k_unifor;
	f32 max,min;
	f32 Sum;//求和
	char max_id,min_id;
	f32 LimitMax;//整体缩放限制值
	f32 K_Zoom2MaxLimit;//整体缩放比例
	f32 ZoomOut[UNIFOR_DATA_NUMMAX];//整体缩放结果
}Unifor_TypeDef;

//极值
typedef struct 
{
	f32 Max;												//最大值
	char MaxID;												//最大值位置
	f32 Min;												//最小值
	char MinID;												//最小值位置
}Extreme_StDef;

//二维直角坐标点定义 单位：m
typedef struct
{
	float X;
	float Y;
}Point_XY_StDef;

//二维直角坐标点定义 单位：m 双精度
typedef struct
{
	double x, y;
}Point_XY_Double_Typdef;

//点+角度 直线定义
typedef struct
{
	//过直线的点 的直角坐标
	Point_XY_StDef P;

	//直线与直角坐标系Y轴夹角,范围(-180,+180],直线与y轴平行定义为0度，直线逆时针转动时角度定义为正，顺时针定义为负
	float Ang;
}PointAnf2Line_TypeDef;

//线性标定对象
typedef struct
{
	u32 Org;												//原始数据
	u32 SetHighOrg;											//标定高点
	u32 SetLowOrg;											//标定低点 传感器
	f32 SetHigh;											//标定高点 实际																					
	f32 SetLow;												//标定低点 实际
	f32 Cal;												//标定后
}LinearCal_TypeDef;

float RatioFuc(float din, float inmin,float inmax,float outmin,float outmax);
float AbsFuc(float din, float outmax);
float AbsFucLimitMin(float In, float OutMin);
float RatioStand(float orgin, float org1,float org2,float act1,float act2);
void unifor_find_max_min(Unifor_TypeDef *p);
void unifor_cal(Unifor_TypeDef *p);
void SortFuc(float In[],char Len);
u16 CRCCAL(u16 CRC_ORG,u8* p,u16 length);
bool Unifor_Zoom(Unifor_TypeDef* p);
bool Unifor_Zoom_Sum(Unifor_TypeDef* p);
u16 CalcLinesCrossPoint(PointAnf2Line_TypeDef* Lines, char LineNum, Point_XY_StDef* CrossP);
float CalculateBoundingBoxDiagonal(Point_XY_StDef* points, int numPoints);
char PointInPolygon(Point_XY_Double_Typdef p, Point_XY_Double_Typdef poly[], int n);
#endif

/************************************END OF FILE************************************/


