/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_CenterPointCal.c   
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.10.11         
* Description: 一侧轮系上直接安装磁导航，磁条偏移直接所在轮系目标角度，
				由传感器所在两轮系目标角度交点确定整车运动圆心反解其它轮系
*******************************************************************************/

#include "app_CenterPointCal.h"
#include "common_def.h"
#include "basecalfunc.h"
#include <math.h>

//计算点斜式两直线交点
Point_XY_StDef IntersectionPoint(CPC_Line_StDef L1, CPC_Line_StDef L2)
{
	Point_XY_StDef P;
	P.X = (L1.K * L1.P.X - L2.K * L2.P.X + L2.P.Y - L1.P.Y) / (L1.K- L2.K);
	P.Y = L1.K * (P.X - L1.P.X) + L1.P.Y;

	return P;
}

//计算两轮系目标角度交点
bool CenterPointCal_Run(CenterPointCal_StDef* p)
{
	char i = 0;

    p->RunCnt ++ ;
    p->Status = CPC_ST_Init;

	//初始化与输入内容检查 轮系位置不是同一点
    if(p->WheelPos[0].X == p->WheelPos[1].X && p->WheelPos[0].Y == p->WheelPos[1].Y)
    {
        p->Status = CPC_ST_InputErr;
        return false;
    }
    else {}

	//计算巡线轮系目标角度
	for (i = 0; i < CPC_TraceNum; i++)
	{
		p->TraceAng[i] = RatioFuc(p->In_MagValue[i],
						p->MagMin, p->MagMax, p->TraceAngMin, p->TraceAngMax);//线性计算目标角度
		p->TraceAng[i] = AbsFuc(p->TraceAng[i], p->TraceAngMax);		//幅度限制
	}

	//计算轴线斜率角度
	for (i = 0; i < CPC_TraceNum; i++)
	{
		if(p->BitInput.MagInf)//横向巡线
            p->SlopeAng[i] =  -(90 - p->TraceAng[i]);
        //p->SlopeAng[i] =  - p->TraceAng[i];
        else//纵向巡线
            p->SlopeAng[i] = p->TraceAng[i];
	}

    //判断情况分类 单桥转向、斜行平移（平行无交点）、纯圆弧运动、复合运动
    if(p->SlopeAng[0] == 0 && p->SlopeAng[1] == 0)
    {
        p->Status = CPC_ST_NoCorrect;
        return true;
    }
    else if(p->SlopeAng[0] == 0 || p->SlopeAng[1] == 0)
    {
        p->Status = CPC_ST_CarCorrect;
    }
    else if(p->SlopeAng[0] == p->SlopeAng[1])
    {
        p->Status = CPC_ST_SlopeCorrect;
    }
    else if(p->SlopeAng[0] == - p->SlopeAng[1])
    {
        p->Status = CPC_ST_ArcCorrect;
    }
    else
    {
        p->Status = CPC_ST_CompositeCorrect;
    }

	//仅斜率存在时 计算直线斜率
	for (i = 0; i < CPC_TraceNum; i++)
	{
        if(1)//p->SlopeAng[i] != -90 || p->SlopeAng[i] != 90)//实际斜率都存在
		    p->Slope[i] = tan(p->SlopeAng[i] * K_AngleToRad);
        else {}
	}

	//直线不平行时 且斜率都存在时，计算交点坐标
    if(p->Status == CPC_ST_ArcCorrect || p->Status == CPC_ST_CompositeCorrect || p->Status == CPC_ST_CarCorrect)
	{
        //直线参数更新
        for (i = 0; i < CPC_TraceNum; i++)
        {
            p->Line[i].P.X = p->WheelPos[i].X;
            p->Line[i].P.Y = p->WheelPos[i].Y;
            p->Line[i].K = p->Slope[i];
        }

        //计算交点坐标
        p->IntersectionPoint = IntersectionPoint(p->Line[0], p->Line[1]);
        p->CenterPoint = p->IntersectionPoint;
    }
    else if(0)//其中一条直线无斜率时 将其轮系坐标x带入另一条直线方程计算交点坐标
    {
        //实际不存在
    }
    else {}

	return true;
}

/************************************END OF FILE************************************/

