/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_CenterPointCal.h  
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.10.11         
* Description: 一侧轮系上直接安装磁导航，磁条偏移直接所在轮系目标角度，
				由传感器所在两轮系目标角度交点确定整车运动圆心反解其它轮系
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_CenterPointCal_h__
#define __app_CenterPointCal_h__

#include <stdbool.h>
#include "basecalfunc.h"

//宏定义
#define CPC_TraceFront 0									//前轮系
#define CPC_TraceBack 1										//后轮系
#define CPC_TraceNum 2										//巡线轮系数量

//点线式直线方程定义
typedef struct
{
	Point_XY_StDef P;
	float K;
}CPC_Line_StDef;

//输入合理性判断结果 运动模式 无纠正、斜行纠正、圆弧纠正、复合纠正
typedef enum
{
	CPC_ST_Init = 0,										//初始化
    CPC_ST_InputErr,										//输入错误
	CPC_ST_NoCorrect,										//无纠正
    CPC_ST_CarCorrect,								        //汽车纠正
	CPC_ST_SlopeCorrect,									//斜行纠正
	CPC_ST_ArcCorrect,									    //圆弧纠正
	CPC_ST_CompositeCorrect,								//复合纠正
	CPC_ST_End = 65536,										//int
}CPC_ST_EnumDef;

//巡线轮系交点计算对象
typedef struct
{
    unsigned short RunCnt;                                  //运行计数器

	/*----------------需初始化----------------*/
	//轮系位置坐标 0:前轮系 1后轮系
	Point_XY_StDef WheelPos[CPC_TraceNum];
	//磁导航偏差范围
	short MagMax;
	short MagMin;
	//巡线轮系角度范围
	short TraceAngMax;
	short TraceAngMin;

	/*----------------实时输入----------------*/
    //位参数
    struct
    {
        u16 MagInf : 1;							            //横向模式下巡线
    }BitInput;

	//运动速度
	short In_Velocity;
	//磁导航偏差 分别前、后磁导航传感器，人站在车后面，面向车，车偏右，数据为正
	short In_MagValue[CPC_TraceNum];

	/*----------------过程状态----------------*/
	//巡线轮系目标角度
	float TraceAng[CPC_TraceNum];
	//轴线斜率角度
	float SlopeAng[CPC_TraceNum];
	//交线斜率
	float Slope[CPC_TraceNum];
	//直线
	CPC_Line_StDef Line[CPC_TraceNum];
	//直线交点
	Point_XY_StDef IntersectionPoint;

	/*----------------实时输出----------------*/
	//状态输出
	CPC_ST_EnumDef Status;
	//圆心坐标
    Point_XY_StDef CenterPoint;

}CenterPointCal_StDef;

/*外部方法声明*/
bool CenterPointCal_Run(CenterPointCal_StDef* p);

#endif

/************************************END OF FILE************************************/
