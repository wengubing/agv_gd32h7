/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_ShelfAutadj.c
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.1.19         
* Description: 根据车体与工装的测距传感器信息，计算车体的相对位置并输出纠偏控制量
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __SHELF_AUTADJ_H__
#define __SHELF_AUTADJ_H__
															//自定义引用
#include <stdbool.h>
#include "..\..\CBB\user\basecalfunc\common_def.h"
#define NUMMAX 												5//单侧传感器最大个数

//测距进工装场景模式
typedef enum
{
	ShelfAutadj_Mode_Null=0,								//不满足自动条件 
	ShelfAutadj_Mode_Carturn,								//前桥转向模式
	ShelfAutadj_Mode_Parallel,								//平移
	ShelfAutadj_Mode_Complex,								//复合运动
	ShelfAutadj_Mode_End=65535,								//枚举型结束
}ShelfAutadj_Mode_TypeDef;
	
/*测距进工装场景 单侧归一化状态*/
typedef enum
{
	ShelfAutadj_Sidestatus_Null=0,							//没有 
	ShelfAutadj_Sidestatus_One,								//仅一个测距有效
	ShelfAutadj_Sidestatus_Two,								//至少两个测距有效
	ShelfAutadj_Sidestatus_End=65535,						//枚举型结束
}ShelfAutadj_Sidestatus_TypeDef;

//测距自动对中 对像定义
typedef struct _ShelfAutadj_TypeDef
{
	/*传入部分*/
	s16 in_DistanceLeft[NUMMAX];							//左侧传感器测距值 mm
	s16 in_DistanceRight[NUMMAX];							//右侧传感器测距值 mm
	s16 DistanceMin,DistanceMax;							//传感器最 测量行程 mm
	s16 Num;												//实际单侧传感器个数
	s16 in_Velocity;										//大于0表示向前
	f32 k2vx,k2vz,k2r;										//偏移到纠正输出的计算系数

	/*计算和输出部分*/
	ShelfAutadj_Sidestatus_TypeDef SidestatusLeft;			//左侧状态
	ShelfAutadj_Sidestatus_TypeDef SidestatusRight;			//右侧状态
	f32 ValueLeftFront,ValueLeftBack,ValueLeftAvg;			//左侧前方、后方、平均
	f32 ValueRightFront,ValueRightBack,ValueRightAvg;		//右侧...
	ShelfAutadj_Mode_TypeDef out_Mode;						//场景模式
	f32 out_Vx,out_Vz,out_CarAng;							//复合运动纠正的Vxz、平移纠正的Vx
															//汽车转向纠正的ang
	bool out_AdjStart,out_AdjArriveStop;
	
	/*方法*/
	int (*p_ShelfAutadj_SideStatusFuc)(struct _ShelfAutadj_TypeDef *p);//单边状态归一化处理
	int (*p_ShelfAutadj_ModeFuc)(struct _ShelfAutadj_TypeDef *p);//场景模式处理
}ShelfAutadj_TypeDef;

/*外部方法声明*/
int ShelfAutadj_SideStatusFuc(ShelfAutadj_TypeDef *p);
int ShelfAutadj_ModeFuc(ShelfAutadj_TypeDef *p);
int ShelfAutadj_Main(ShelfAutadj_TypeDef *p);
int ShelfAutadj_ModeFuc(ShelfAutadj_TypeDef *p);
int ShelfAutadj_InitExample(ShelfAutadj_TypeDef *p,s16 in_DistanceMax,s16 in_Num);


#endif

/************************************END OF FILE************************************/

