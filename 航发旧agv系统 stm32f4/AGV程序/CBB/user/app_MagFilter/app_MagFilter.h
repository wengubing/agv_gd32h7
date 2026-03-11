/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_MagFilter.h   
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.1.23         
* Description: 输入磁条传感器实时数据，输出滤波后的数据
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_MagFilter_h__
#define __app_MagFilter_h__

/*包含以下头文件*/
#include <stdbool.h>
#include "..\..\CBB\user\basecalfunc\common_def.h"
#include "..\..\CBB\user\app_DataFilter\app_DataFilter.h"

/*typedef enum
{
}DataFilter_MoTypeDef;*/

typedef struct
{
	/*初始化*/
	short in_FindStrength;									/*发现磁条强度阈值*/
	short in_LoseStrength;									/*离开磁条强度阈值*/
	unsigned short in_StrengthFifoLength;					/*强度滤波通道采样长度*/
	unsigned short in_DistanceFifoLength;					/*位置滤波通道采样长度*/
	unsigned short in_DistanceErrCheck;						/*位置变化异常检出阈值*/
	unsigned short in_DistanceErrRecover;					/*位置变化异常恢复阈值*/

	/*调用输入*/
	bool in_MagState;										/*传感器状态*/
	short in_MagStrength;									/*磁条强度*/	
	short in_MagDistance;									/*磁条位置*/	

	/*内部成员*/
	DataFilter_TypeDef StrengthFiter;
	DataFilter_TypeDef DistanceFiter;
	short out_DistanceLast;									/*上一次的数据*/
	
	/*输出*/
	bool out_StrengthStable;								/*磁条强度稳定标志*/	
	bool out_DistanceStable;								/*磁条位置稳定标志*/	
	short out_Strength;										/*滤波后的磁条强度*/	
	short out_Distance;										/*滤波后的磁条位置*/	
}MagFilter_TypeDef;

/*外部方法声明*/
float MagFilter_Main(MagFilter_TypeDef *p);
bool MagFilter_InitExample(MagFilter_TypeDef *p);

#endif

/************************************END OF FILE************************************/

