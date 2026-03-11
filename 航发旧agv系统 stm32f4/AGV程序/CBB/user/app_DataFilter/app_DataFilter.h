/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_DataFilter.h   
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.1.23         
* Description: 输入实时值，输出滤波值
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_DataFilter_h__
#define __app_DataFilter_h__

#define	DATAFILTER_FIFOMAX									10//采样通道最大长度

/*包含以下头文件*/
#include <stdbool.h>
#include "..\..\CBB\user\basecalfunc\common_def.h"

typedef enum
{
	DataFilter_Mode_Median	=0,								//中位值滤波
	DataFilter_Mode_Average	=1,								//均值滤波
	DataFilter_Mode_Maximum	=2,								//最大值滤波
}DataFilter_Mode_TypeDef;

typedef struct
{
	/*初始化*/
	unsigned short in_FifoLength;							//FIFO滤波通道长度
	DataFilter_Mode_TypeDef in_FilterMode;					//滤波方式

	/*调用输入*/
	float in_RealTimeValue;									//实时数据输入

	/*内部成员*/
	float FifoOrg[DATAFILTER_FIFOMAX];						//实时数据输入
	unsigned short FifoLength;
	DataFilter_Mode_TypeDef FilterMode;
	char FifoIndex;											/*简化fifo的循环新数据写入下标位置*/
	float Sort[DATAFILTER_FIFOMAX];							/*排序结果 min->max*/
	float Median,SumExtrme,Average;							/*中位值、去极值求和、平均值*/
	float Max;												/*去极值求后的最大值*/
	
	/*输出*/
	float out_FilterValue;									//滤波输出
		
}DataFilter_TypeDef;

/*外部方法声明*/
float DataFilter_Main(DataFilter_TypeDef *p);
bool DataFilter_InitExample(DataFilter_TypeDef *p);

#endif

/************************************END OF FILE************************************/

