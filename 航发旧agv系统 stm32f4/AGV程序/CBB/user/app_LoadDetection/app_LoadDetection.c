/********************************** Copyright ***********************************
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
*
* FileName   : app_LoadDetection.c
* Version    : V1.0
* Author     : WenXiaoBing
* Date       : 2024.1.19
* Description: 输入压力数据，计算称重、重心计算、超载偏载判断等
*******************************************************************************/

/*包含以下头文件*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\..\CBB\user\basecalfunc\common_def.h"
#include "..\..\CBB\user\basecalfunc\basecalfunc.h"
#include "app_LoadDetection.h"

/*内部实例化对象*/
LoadDetection_Point_TypeDef p;
LoadDetection_Triangle_TypeDef triangle;

//判定给定点是否在三角形内
bool LoadDetection_PointInside_Triganle(LoadDetection_TypeDef* p)
{
	LoadDetection_Point_TypeDef point_temp;
	point_temp.X = p->CoreX;
	point_temp.Y = p->CoreY;

	/*
	triangle.a.x=p->sensor[0].axis_x;
	triangle.a.y=p->sensor[0].axis_y;

	triangle.b.x=p->sensor[1].axis_x;
	triangle.b.y=p->sensor[1].axis_y;

	triangle.c.x=p->sensor[2].axis_x;
	triangle.c.y=p->sensor[2].axis_y;
	*/

	triangle = p->Errpar.JudgeTriganle;

	//计算三角形的叉积
	LoadDetection_PointInTriangle(p, point_temp);

	//判断点的位置
	if (triangle.res_Inside == onvertex_Inside)
		p->CoreInside = 1;
	else
		p->CoreInside = 0;

	return 1;
}

//判定给定点是否在四边形内
bool LoadDetection_PointInside_Quadrangle(LoadDetection_TypeDef* p)
{
	Point_XY_Double_Typdef Poly[4];
	Point_XY_Double_Typdef Point;

	//装载四边形顶点 注意按顺时针或逆时针顺序装载！
	Poly[0].x = p->Errpar.JudgeQuadrangle[0];
	Poly[0].y = p->Errpar.JudgeQuadrangle[1];
	Poly[1].x = p->Errpar.JudgeQuadrangle[2];
	Poly[1].y = p->Errpar.JudgeQuadrangle[3];
	Poly[2].x = p->Errpar.JudgeQuadrangle[4];
	Poly[2].y = p->Errpar.JudgeQuadrangle[5];
	Poly[3].x = p->Errpar.JudgeQuadrangle[6];
	Poly[3].y = p->Errpar.JudgeQuadrangle[7];
	//装载待测点
	Point.x = p->CoreX;
	Point.y = p->CoreY;

	//调用方法
	if (PointInPolygon(Point, Poly, 4) == 1)
		p->CoreInside = 1;
	else
		p->CoreInside = 0;

	return 1;
}

//判定给定点是否在设定区域内
void LoadDetection_PointInside(LoadDetection_TypeDef* p)
{
	//如果压力点数量小于4则使用三角形判定，否则使用四边形判定
	if (p->Num < 4)
		LoadDetection_PointInside_Triganle(p);
	else
	{
		LoadDetection_PointInside_Quadrangle(p);
	}
}

//异常检出
void LoadDetection_Errjdg(LoadDetection_TypeDef* p)
{
	char errflg1 = 0, errflg2 = 0, i = 0;

	//整体超载
	if (p->WeightShow > p->Errpar.OverLoad)
	{
		if (p->Err.OverLoadCnt < p->Errpar.cnt_max)
			p->Err.OverLoadCnt++;
		else
			p->Err.OverLoad = 1;
	}
	else
	{
		p->Err.OverLoad = 0;
		p->Err.OverLoadCnt = 0;
	}

	//非整体超载的三角形内局部超载 定义为偏载 单组压力
	if (p->Err.OverLoad == 0)
	{
		for (i = 0; i < p->Num; i++)
		{
			if (p->Sensor[i].Cal > p->Errpar.SingleMax)
				errflg1++;
			if (p->Sensor[i].Cal < p->Errpar.SingleMin)
				errflg2++;
		}
	}
	if (errflg1)
	{
		if (p->Err.SingleMaxCnt < p->Errpar.cnt_max)
			p->Err.SingleMaxCnt++;
		else
			p->Err.SingleMax = 1;
	}
	else
	{
		p->Err.SingleMax = 0;
		p->Err.SingleMaxCnt = 0;
	}
	if (errflg2)
	{
		if (p->Err.SingleMinCnt < p->Errpar.cnt_max)
			p->Err.SingleMinCnt++;
		else
			p->Err.SingleMin = 1;
	}
	else
	{
		p->Err.SingleMin = 0;
		p->Err.SingleMinCnt = 0;
	}

	//重心位置偏移过大
	if (p->CoreInside == 0)
	{
		if (p->Err.UnbalanceCoreCnt < p->Errpar.cnt_max)
			p->Err.UnbalanceCoreCnt++;
		else
			p->Err.UnbalanceCore = 1;
	}
	else
	{
		p->Err.UnbalanceCore = 0;
		p->Err.UnbalanceCoreCnt = 0;
	}

}

//计算总重量
void LoadDetection_GetWeight(LoadDetection_TypeDef* p)
{
	f32 temp = 0;
	char i = 0;

	p->WeightAll = 0;
	for (i = 0; i < p->Num; i++)
	{
		p->Sensor[i].S = (p->Sensor[i].Dim / 2) * (p->Sensor[i].Dim / 2) * PI;																								//油缸直径 mm
		temp = p->Sensor[i].Cal * p->Sensor[i].S * p->Sensor[i].UnitNum;
		p->WeightAll = p->WeightAll + temp;
	}

	p->WeightAll = p->WeightAll / 10000;									//Mpa
	//计算负载重量
	p->WeightShow = p->WeightAll - p->WeightNoload;
}

//计算重心
void LoadDetection_GetCore(LoadDetection_TypeDef* p)
{
	char i = 0;
	f32 tempx = 0, tempy = 0, temp = 0;
	for (i = 0; i < p->Num; i++)
	{
		temp = temp + p->Sensor[i].Cal * p->Sensor[i].UnitNum;
		tempx = tempx + p->Sensor[i].Cal * p->Sensor[i].UnitNum * p->Sensor[i].AxisX;
		tempy = tempy + p->Sensor[i].Cal * p->Sensor[i].UnitNum * p->Sensor[i].AxisY;
	}
	p->CoreX = tempx / temp;
	p->CoreY = tempy / temp;
}

/**************************************************************************
* 函数名:LiftCtrl_InitExample
* 功  能:模块单独测试时的初始化内容，实际外部使用模块时可参考编写
* 参  数:LoadDetection_TypeDef *p
* 返回值:无
**************************************************************************/
void LoadDetection_InitExample(LoadDetection_TypeDef* p)
{
	char i = 0;
	p->Sensor[i].Cal = 10;
	p->Sensor[i].Dim = 150;
	p->Sensor[i].AxisX = -1000;
	p->Sensor[i].AxisY = 1000;
	p->Sensor[i].UnitNum = 1;

	i = 1;
	p->Sensor[i].Cal = 10;
	p->Sensor[i].Dim = 150;
	p->Sensor[i].AxisX = 1000;
	p->Sensor[i].AxisY = 1000;
	p->Sensor[i].UnitNum = 1;

	i = 2;
	p->Sensor[i].Cal = 10;
	p->Sensor[i].Dim = 150;
	p->Sensor[i].AxisX = 0;
	p->Sensor[i].AxisY = -1000;
	p->Sensor[i].UnitNum = 2;

	p->Num = 3;
}

//计算叉积
f32 crossProduct(LoadDetection_Point_TypeDef p1, LoadDetection_Point_TypeDef p2)
{
	return p1.X * p2.Y - p1.Y * p2.X;
}

//判定点是否在三角形内
void LoadDetection_PointInTriangle(LoadDetection_TypeDef* p, LoadDetection_Point_TypeDef point)
{
	LoadDetection_Point_TypeDef AB, BC, CA, AP, BP, CP;
	char cross_zero_cnt = 0, i = 0;

	AB.X = triangle.b.X - triangle.a.X;
	AB.Y = triangle.b.Y - triangle.a.Y;

	BC.X = triangle.c.X - triangle.b.X;
	BC.Y = triangle.c.Y - triangle.b.Y;

	CA.X = triangle.a.X - triangle.c.X;
	CA.Y = triangle.a.Y - triangle.c.Y;

	AP.X = point.X - triangle.a.X;
	AP.Y = point.Y - triangle.a.Y;

	BP.X = point.X - triangle.b.X;
	BP.Y = point.Y - triangle.b.Y;

	CP.X = point.X - triangle.c.X;
	CP.Y = point.Y - triangle.c.Y;

	triangle.Rross[0] = crossProduct(AB, AP);
	triangle.Rross[1] = crossProduct(BC, BP);
	triangle.Rross[2] = crossProduct(CA, CP);

	for (i = 0; i < 3; i++)
	{
		if (triangle.Rross[i] == 0)
			cross_zero_cnt++;
	}

	triangle.res_Inside = LoadDetection_PointInside_Null;
	if ((triangle.Rross[0] > 0 && triangle.Rross[1] > 0 && triangle.Rross[2] > 0) || (triangle.Rross[0] < 0 && triangle.Rross[1] < 0 && triangle.Rross[2] < 0))
		triangle.res_Inside = onvertex_Inside;
	else if (cross_zero_cnt == 1)
		triangle.res_Inside = LoadDetection_PointInside_Online;
	else if (cross_zero_cnt == 2)
		triangle.res_Inside = LoadDetection_PointInside_Onvertex;
	else
		triangle.res_Inside = LoadDetection_PointInside_Outside;
}

/*计算压力极值*/
bool LoadDetection_CalcPressureExtreme(LoadDetection_TypeDef* p)
{
	char i = 0;

	//输入数量
	p->Extreme.data_num = p->Num;
	//输入数据
	for (i = 0; i < p->Num; i++)
		p->Extreme.org[i] = p->Sensor[i].Cal;
	//计算极值
	unifor_find_max_min(&p->Extreme);
	
	return 1;
}

//主方法
void LoadDetection_Main(LoadDetection_TypeDef* p)
{
	char i = 0;
	//PressureOrgIn();
	for (i = 0; i < p->Num; i++)
	{
		p->Sensor[i].Cal = p->Sensor[i].Org * p->Sensor[i].LeverCoefficient;
	}
	//计算压力极值
	LoadDetection_CalcPressureExtreme(p);
	//计算总重量
	LoadDetection_GetWeight(p);
	//计算重心
	LoadDetection_GetCore(p);
	//重心位置判定
	LoadDetection_PointInside(p);
	//异常检出
	LoadDetection_Errjdg(p);
}

/************************************END OF FILE************************************/
