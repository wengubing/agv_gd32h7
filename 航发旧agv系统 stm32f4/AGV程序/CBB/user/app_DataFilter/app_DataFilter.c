/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_DataFilter.c  
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.1.23         
* Description: 输入实时值，输出滤波值
*******************************************************************************/

/*包含以下头文件*/
#include <string.h>
#include "app_DataFilter.h"
#include "..\..\CBB\user\basecalfunc\basecalfunc.h"


/**************************************************************************
* 函数名:  DataFilter_Main()
* 功  能: 数据滤波主方法
* 参  数: 模块对象地址
* 返回值: 滤波值
**************************************************************************/
float DataFilter_Main(DataFilter_TypeDef *p)
{	
	float res=0;
	char i=0;
	if(p->in_FifoLength>DATAFILTER_FIFOMAX || p->in_FifoLength<3)
		return res;
	
	/*导入参数*/
	p->FilterMode = p->in_FilterMode;
	p->FifoLength = p->in_FifoLength;

	/*数据进入fifo*/
	if(p->FifoIndex < p->in_FifoLength)
	{
		p->FifoOrg[p->FifoIndex]=p->in_RealTimeValue;
		if(p->FifoIndex < (p->in_FifoLength-1))
			p->FifoIndex++;
		else
			p->FifoIndex=0;
	}
	else
		p->FifoIndex=0;

	/*排序*/
	memcpy(p->Sort,p->FifoOrg,sizeof(p->FifoOrg[0])*p->FifoLength);
	SortFuc(p->Sort,p->FifoLength);

	/*舍去极值求中位数、均值*/
	if(p->FifoLength % 2 != 0)
	{
		p->Median = p->Sort[p->FifoLength/2];
	}
	else
	{
		p->Median = (p->Sort[p->FifoLength / 2 - 1] + p->Sort[p->FifoLength / 2]) / 2.0f;
	}

	p->SumExtrme=0;
	for(i=1;i<(p->FifoLength-1);i++)
		p->SumExtrme = p->SumExtrme + p->Sort[i];

	p->Average=p->SumExtrme/(p->FifoLength-2);

	
	p->Max=p->Sort[p->in_FifoLength-1];

	if(p->in_FilterMode==DataFilter_Mode_Median)
		res=p->Median;
	else if(p->in_FilterMode==DataFilter_Mode_Average)
		res=p->Average;
	else if(p->in_FilterMode==DataFilter_Mode_Maximum)
		res=p->Max;
		
	return res;
}

/**************************************************************************
* 函数名:DataFilter_InitExample
* 功  能:模块单独测试时的初始化内容，实际外部使用模块时可参考编写
* 参  数:DataFilter_TypeDef *p
* 返回值:无
**************************************************************************/
bool DataFilter_InitExample(DataFilter_TypeDef *p)
{
	p->in_FifoLength=10;
	p->in_FilterMode=DataFilter_Mode_Median;
	return 0;
}

/************************************END OF FILE************************************/

