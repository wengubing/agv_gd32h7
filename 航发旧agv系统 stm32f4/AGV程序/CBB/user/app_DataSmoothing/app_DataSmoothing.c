/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_DataSmoothing.c   
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.1.26         
* Description: 输入实时控制量，输出平滑处理后的量
*******************************************************************************/


/*包含以下头文件*/
#include "app_DataSmoothing.h"
#include <math.h>

/**************************************************************************
* 函数名:DataSmooth_Init
* 功  能:控制命令平滑处理init
* 参  数:NC
* 返回值:NC
**************************************************************************/
bool DataSmooth_Init(DataSmoothing_TypeDef *p,float Acc,float Dec,float AdsorbRange,float AccAdsorb,float DecAdsorb)
{
	p->in_Acc=Acc;
	p->in_Dec=Dec;
	p->in_AdsorbRange=AdsorbRange;
	p->in_AccAdsorb=AccAdsorb;
	p->in_DecAdsorb=DecAdsorb;
	p->in_Mode=DataSmoothingMode_Time;
	return 0;
}

/**************************************************************************
* 函数名:  DataSmoothing_Main()
* 功  能: 平滑主方法
* 参  数: 模块对象地址,系统ms，实时控制值
* 返回值: 滤波值
**************************************************************************/
float DataSmoothing_Main(DataSmoothing_TypeDef *p,u32 Ms,f32 RealTimeValue)
{	
	float res=0;
	short N=0;
	float AccTemp=0,DecTemp=0;
	p->in_Ms=Ms;
	p->in_RealTimeValue=RealTimeValue;
	
	/*判断是否位于吸附区间*/
	//if(p->out_Value < p->in_AdsorbRange && p->out_Value > -(p->in_AdsorbRange))
	if(fabs(p->out_Value) < p->in_AdsorbRange)
	{
		AccTemp=p->in_AccAdsorb;
		DecTemp=p->in_DecAdsorb;
	}
	else
	{
		AccTemp=p->in_Acc;
		DecTemp=p->in_Dec;
	}

	/*判断驱动方式*/
	if(p->in_Mode==DataSmoothingMode_Call)
	{
		AccTemp=AccTemp;
		DecTemp=DecTemp;
		p->out_Timestamp=p->in_Ms;
	}
	else if(p->in_Mode==DataSmoothingMode_Time && p->out_Timestamp<p->in_Ms)
	{
		N=p->in_Ms-p->out_Timestamp;
		AccTemp=AccTemp*N;
		DecTemp=DecTemp*N;
		p->out_Timestamp=p->in_Ms;
	}
	else
	{
		AccTemp=0;
		DecTemp=0;
	}

	p->out_Value=Smoothing(p->in_RealTimeValue,p->out_Value,AccTemp,DecTemp);
	res=p->out_Value;
	return res;
}

/**************************************************************************
* 函数名:  DataSmoothing_Unite()
* 功  能: 多轴关联平滑调用
* 参  数: 模块对象地址（对象数组），对象数组长度，系统ms，多轴平滑参数对象
* 返回值: char
**************************************************************************/
char DataSmoothing_Unite(DataSmoothing_TypeDef Array[],char n,f32 Ms,DataSmoothing_TypeDef *p2ParaObj)
{	
	char res=0;
	char ArraySize=n;
	f32 d_Temp[DataSmoothing_Unite_Max]={0};
	f32 d_Max=0;
	char i=0;
	
	if(ArraySize>DataSmoothing_Unite_Max)
		return 1;
	else	
	{
		/*找到最大*/
		for(i=0;i<ArraySize;i++)
		{
			d_Temp[i]=fabs(Array[i].in_RealTimeValue - Array[i].out_Value);
			if(fabs(d_Temp[i])>d_Max)
				d_Max=fabs(d_Temp[i]);
		}

		/*按第一个对象参数缩放其它对象*/
		for(i=0;i<ArraySize;i++)
		{
			Array[i].in_Acc=p2ParaObj->in_Acc*d_Temp[i]/d_Max;
			Array[i].in_Dec=p2ParaObj->in_Dec*d_Temp[i]/d_Max;
			Array[i].in_AccAdsorb=p2ParaObj->in_AccAdsorb*d_Temp[i]/d_Max;
			Array[i].in_DecAdsorb=p2ParaObj->in_DecAdsorb*d_Temp[i]/d_Max;
			Array[i].in_AdsorbRange=p2ParaObj->in_AdsorbRange;
			Array[i].in_Mode=p2ParaObj->in_Mode;
			//Array[i].in_Ms=p2ParaObj.in_Ms;
			Array[i].out_Value=DataSmoothing_Main(&Array[i],Ms,Array[i].in_RealTimeValue);
		}
	}
	return res;	
}


/**************************************************************************
* 函数名:  Smoothing()
* 功  能: 平滑基础方法
* 参  数: 数据输入，上次的输出，平滑增量，平滑减量
* 返回值: 输出结果
**************************************************************************/
float Smoothing(float In,float Out,float Acc,float Dec)
{
	float d=In-Out;
	
	/*异号减速*/
	if((In>=0&&Out<0) || (In<=0&&Out>0))
	{
		if(fabs(d)<=Dec)
			Out=In;//
		else
		{//-
			if(Out>0)
				Out=Out-Dec;
			else
				Out=Out+Dec;
		}
	}
	/*同号*/
	else
	{
		if((In>0&&Out>=0) || (In<0&&Out<=0))
		{
			/*同号加速*/
			if(fabs(In)>fabs(Out))
			{
				if(fabs(d)>Acc)
				{//+
					if(In>0)
						Out=Out+Acc;
					else
						Out=Out-Acc;
				}
				else
					Out=In;
			}
			/*同号减速*/
			else
			{
				/*同号减速*/
				if(fabs(In)<fabs(Out))
				{
					if(fabs(d)>Dec)
					{//-
						if(In>0)
							Out=Out-Dec;
						else
							Out=Out+Dec;
					}
					else
						Out=In;
				}
				else
					Out=In;
			}
		}
		else
			Out=In;
	}
	return Out;
}

/**************************************************************************
* 函数名:DataSmoothing_InitExample
* 功  能:模块单独测试时的初始化内容，实际外部使用模块时可参考编写
* 参  数:DataSmoothing_InitExample *p
* 返回值:无
**************************************************************************/
bool DataSmoothing_InitExample(DataSmoothing_TypeDef *p)
{
	p->in_Acc=1;
	p->in_Dec=3;
	p->in_AdsorbRange=20;
	p->in_AccAdsorb=2;
	p->in_DecAdsorb=6;
	p->in_Mode=DataSmoothingMode_Call;
	return 0;
}

/************************************END OF FILE************************************/

