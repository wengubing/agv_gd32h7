/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_DataSmoothing.h   
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.1.26         
* Description: 输入实时控制量，输出平滑处理后的量
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_DataSmoothing_h__
#define __app_DataSmoothing_h__

/*包含以下头文件*/
#include <stdbool.h>
#include "..\..\CBB\user\basecalfunc\common_def.h"

/*宏定义*/
#define DataSmoothing_Unite_Max								10/*多轴关联平滑最大轴数*/

typedef enum
{
	DataSmoothingMode_Call=0,								/*调用驱动*/
	DataSmoothingMode_Time,									/*时间驱动*/
}DataSmoothingMode_TypeDef;

typedef struct
{
	/*初始化*/
	DataSmoothingMode_TypeDef in_Mode;						/*平滑方式*/
	float in_Acc;											/*加速增量*/
	float in_Dec; 											/*减速增量*/
	float in_AccAdsorb; 									/*吸附加速增量*/
	float in_DecAdsorb; 									/*吸附减速增量*/
	float in_AdsorbRange;									/*吸附边界*/
	
	/*调用输入*/
	float in_RealTimeValue;									/*实时输入*/
	u32 in_Ms;												/*ms输入*/

	/*内部成员*/
	float out_Timestamp;									/*输出更新时间戳*/

	/*输出*/
	float out_Value;										/*平滑结果输出*/
		
}DataSmoothing_TypeDef;

/*外部方法声明*/
float Smoothing(float In,float Out,float Acc,float Dec);
float DataSmoothing_Main(DataSmoothing_TypeDef *p,u32 Ms,f32 RealTimeValue);
char DataSmoothing_Unite(DataSmoothing_TypeDef Array[],char n,f32 Ms,DataSmoothing_TypeDef *p2ParaObj);
bool DataSmoothing_InitExample(DataSmoothing_TypeDef *p);
bool DataSmooth_Init(DataSmoothing_TypeDef *p,float Acc,float Dec,float AdsorbRange,float AccAdsorb,float DecAdsorb);
#endif

/************************************END OF FILE************************************/

