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

/*包含对象头文件*/
#include "app_ShelfAutadj.h"
#include "..\..\CBB\user\basecalfunc\basecalfunc.h"

//单边状态归一化处理
int ShelfAutadj_SideStatusFuc(ShelfAutadj_TypeDef *p)
{
	char i=0;
	p->SidestatusLeft=ShelfAutadj_Sidestatus_Null;
	p->SidestatusRight=ShelfAutadj_Sidestatus_Null;
	for(i=0;i<p->Num;i++)
	{
		//左边
		if(p->in_DistanceLeft[i]>p->DistanceMin && p->in_DistanceLeft[i]<p->DistanceMax)
		{
			if(p->SidestatusLeft==ShelfAutadj_Sidestatus_Null)
			{
				p->SidestatusLeft=ShelfAutadj_Sidestatus_One;
				p->ValueLeftFront=p->in_DistanceLeft[i];
				p->ValueLeftBack=p->ValueLeftFront;//仅一个传感器有效时，其值对后、平均值进行填充
				p->ValueLeftAvg=p->ValueLeftFront;
			}
			else if(p->SidestatusLeft>=ShelfAutadj_Sidestatus_One)
			{
				p->SidestatusLeft=ShelfAutadj_Sidestatus_Two;
				p->ValueLeftBack=p->in_DistanceLeft[i];
				p->ValueLeftAvg=(p->ValueLeftFront+p->ValueLeftBack)/2;
			}
		}

		//右边
		if(p->in_DistanceRight[i]>p->DistanceMin && p->in_DistanceRight[i]<p->DistanceMax)
		{
			if(p->SidestatusRight==ShelfAutadj_Sidestatus_Null)
			{
				p->SidestatusRight=ShelfAutadj_Sidestatus_One;
				p->ValueRightFront=p->in_DistanceRight[i];
				p->ValueRightBack=p->ValueRightFront;
				p->ValueRightAvg=p->ValueRightFront;
			}
			else if(p->SidestatusRight>=ShelfAutadj_Sidestatus_One)
			{
				p->SidestatusRight=ShelfAutadj_Sidestatus_Two;
				p->ValueRightBack=p->in_DistanceRight[i];
				p->ValueRightAvg=(p->ValueRightFront+p->ValueRightBack)/2;
			}
		}
	}
	return 1;
}

//场景模式处理
int ShelfAutadj_ModeFuc(ShelfAutadj_TypeDef *p)
{
	if(p->in_Velocity>0)//向前开
	{
		if(p->SidestatusLeft==ShelfAutadj_Sidestatus_Two && p->SidestatusRight==ShelfAutadj_Sidestatus_Two)
		{
			p->out_Mode=ShelfAutadj_Mode_Complex;
			p->out_Vx=(p->ValueRightAvg-p->ValueLeftAvg)*p->k2vx;
			p->out_Vz=(p->ValueLeftFront-p->ValueLeftBack)*p->k2vz;
			p->out_CarAng=0;
		}
		else if(p->SidestatusLeft>=ShelfAutadj_Sidestatus_One && p->SidestatusRight>=ShelfAutadj_Sidestatus_One)
		{
			if(p->in_DistanceLeft[0]<p->DistanceMax && p->in_DistanceRight[0]<p->DistanceMax)
			{
				p->out_Mode=ShelfAutadj_Mode_Carturn;
				p->out_CarAng=(p->ValueRightFront-p->ValueLeftFront)*p->k2r;
				p->out_Vx=0;
				p->out_Vz=0;
			}
			else if(p->in_DistanceLeft[p->Num]<p->DistanceMax && p->in_DistanceRight[p->Num]<p->DistanceMax)
			{
				p->out_Mode=ShelfAutadj_Mode_Parallel;
				p->out_Vx=(p->ValueRightBack-p->ValueLeftBack)*p->k2vx;
				p->out_Vz=0;
				p->out_CarAng=0;
			}
			else
			{
				p->out_Mode=ShelfAutadj_Mode_Null;
				p->out_Vx=0;
				p->out_Vz=0;
				p->out_CarAng=0;
			}
		}
		else
		{
			p->out_Mode=ShelfAutadj_Mode_Null;
			p->out_Vx=0;
			p->out_Vz=0;
			p->out_CarAng=0;
		}
	}
	else if(p->in_Velocity<0)//向后
	{
		if(p->SidestatusLeft==ShelfAutadj_Sidestatus_Two && p->SidestatusRight==ShelfAutadj_Sidestatus_Two)
		{
			p->out_Mode=ShelfAutadj_Mode_Complex;
			p->out_Vx=(p->ValueRightAvg-p->ValueLeftAvg)*p->k2vx;
			p->out_Vz=(p->ValueLeftFront-p->ValueLeftBack)*p->k2vz;
			p->out_CarAng=0;
		}
		else if(p->SidestatusLeft>=ShelfAutadj_Sidestatus_One && p->SidestatusRight>=ShelfAutadj_Sidestatus_One)
		{
			if(p->in_DistanceLeft[0]<p->DistanceMax && p->in_DistanceRight[0]<p->DistanceMax)
			{
				p->out_Mode=ShelfAutadj_Mode_Parallel;
				p->out_Vx=(p->ValueRightFront-p->ValueLeftFront)*p->k2vx;
				p->out_Vz=0;
				p->out_CarAng=0;
			}
			else if(p->in_DistanceLeft[p->Num]<p->DistanceMax && p->in_DistanceRight[p->Num]<p->DistanceMax)
			{
				p->out_Mode=ShelfAutadj_Mode_Carturn;
				p->out_CarAng=(p->ValueRightBack-p->ValueLeftBack)*p->k2r;
				p->out_Vx=0;
				p->out_Vz=0;
			}
			else
			{
				p->out_Mode=ShelfAutadj_Mode_Null;
				p->out_Vx=0;
				p->out_Vz=0;
				p->out_CarAng=0;
			}
		}
		
		else
		{
			p->out_Mode=ShelfAutadj_Mode_Null;
			p->out_Vx=0;
			p->out_Vz=0;
			p->out_CarAng=0;
		}
	}
	else
	{
		p->out_Mode=ShelfAutadj_Mode_Null;
		p->out_Vx=0;
		p->out_Vz=0;
		p->out_CarAng=0;
	}
	return 1;
}


/**************************************************************************
* 函数名:SoundLightNotice_InitExample
* 功  能:模块单独测试时的初始化内容，实际外部使用模块时可参考编写
* 参  数:ShelfAutadj_TypeDef *p,dmax_in、num_in分别为测距最大值、单边测距个数
* 返回值: 返回0表示初始化错误 请检查参数
**************************************************************************/
int ShelfAutadj_InitExample(ShelfAutadj_TypeDef *p,s16 in_DistanceMax,s16 in_Num)
{
	char res=0;
	
	/*初始化方法指针*/
	p->p_ShelfAutadj_SideStatusFuc=ShelfAutadj_SideStatusFuc;
	p->p_ShelfAutadj_ModeFuc=ShelfAutadj_ModeFuc;

	//初始化固定参数
	if(in_DistanceMax<30000 && in_DistanceMax<=NUMMAX)
	{
		p->DistanceMin=in_DistanceMax;
		p->Num=in_Num;
		res=1;
	}
	else
		res=0;
	return res;
}


/**/
int ShelfAutadj_Main(ShelfAutadj_TypeDef *p)
{
	/*单侧状态归一化*/
	p->p_ShelfAutadj_SideStatusFuc(p);

	/*判断纠正模式与参数*/
	p->p_ShelfAutadj_ModeFuc(p);

	return 1;
}


/************************************END OF FILE************************************/

