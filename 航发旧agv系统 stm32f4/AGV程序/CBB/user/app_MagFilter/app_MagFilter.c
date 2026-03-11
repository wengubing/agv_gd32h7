/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_MagFilter.c  
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.1.30         
* Description: 输入磁条传感器实时数据，输出滤波后的数据
*******************************************************************************/

/*包含以下头文件*/
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "app_MagFilter.h"
#include "..\..\CBB\user\app_DataFilter\app_DataFilter.h"


/**************************************************************************
* 函数名:  MagFilter_Main()
* 功  能: 数据滤波主方法
* 参  数: 模块对象地址
* 返回值: 滤波值
**************************************************************************/
float MagFilter_Main(MagFilter_TypeDef *p)
{	
	float res=0;
	int d_Distance=0;

	/*非检测时到一个磁条时不进行处理*/
	if(p->in_MagState==0)
	{
		p->out_DistanceStable=0;
		p->out_StrengthStable=0;
	}
	/*分别对强度和位置进行滤波处理*/
	else
	{
		/*强度连续采样 滤波后，通过阈值确定，是否输出磁条标志*/
		p->StrengthFiter.in_RealTimeValue = p->in_MagStrength;
		p->out_Strength = DataFilter_Main(&(p->StrengthFiter));
		if(p->out_Strength<p->in_LoseStrength)
			p->out_StrengthStable=0;
		else if(p->out_Strength>p->in_FindStrength)
			p->out_StrengthStable=1;

		/*位置连续采样 滤波*/
		p->DistanceFiter.in_RealTimeValue = p->in_MagDistance;
		p->out_Distance = DataFilter_Main(&(p->DistanceFiter));

		/*紧临两次结果变化判断是否输出位置稳定标志*/
		d_Distance=p->out_Distance - p->out_DistanceLast;
		if(abs(d_Distance)>p->in_DistanceErrCheck)
			p->out_DistanceStable=0;
		else if(abs(d_Distance)<p->in_DistanceErrRecover)
			p->out_DistanceStable=1;

		p->out_DistanceLast = p->out_Distance;
	}

	res=p->out_Distance;
		
	return res;
}

/**************************************************************************
* 函数名:MagFilter_InitExample
* 功  能:模块单独测试时的初始化内容，实际外部使用模块时可参考编写
* 参  数:MagFilter_TypeDef *p
* 返回值:无
**************************************************************************/
bool MagFilter_InitExample(MagFilter_TypeDef *p)
{
	p->in_FindStrength=50;
	p->in_LoseStrength=40;
	p->in_StrengthFifoLength=8;
	p->in_DistanceFifoLength=8;
	p->in_DistanceErrCheck=30;
	p->in_DistanceErrRecover=20;

	p->StrengthFiter.in_FifoLength=p->in_StrengthFifoLength;
	p->StrengthFiter.in_FilterMode=DataFilter_Mode_Average;

	p->DistanceFiter.in_FifoLength=p->in_DistanceFifoLength;
	p->DistanceFiter.in_FilterMode=DataFilter_Mode_Average;
	return 0;
}

/************************************END OF FILE************************************/

