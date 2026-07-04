/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : basecalfunc.c
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2023.12.29         
* Description: 常见通用方法的实现
*******************************************************************************/

#include <math.h>
#include "basecalfunc.h"

/**************************************************************************
* 函数名:RatioFuc
* 功  能:比例处理函数
* 参  数:din:实时输入，inmin：输入最小值，inmax：输入最大值，outmin：输出最小值，outmax：输出最大值
* 返回值:比例计算结果
**************************************************************************/
float RatioFuc(float din, float inmin,float inmax,float outmin,float outmax)
{
	float sign=0;
	float out;

	if(din>=0)
		sign=1;
	else
		sign=(-1);
	
	if(fabs(din)>inmax)
		din=inmax*sign;
	
	if(fabs(din)<=inmin)
		out=0;//到位
	else
		out=din*((outmax-outmin)/(inmax-inmin));//
	return out;
}

//绝对值大小限制处理函数
float AbsFuc(float din, float outmax)
{
	float sign=0;
	float out;

	if(din>=0)
		sign=1;
	else
		sign=(-1);
	
	if(fabs(din)>outmax)
		out=outmax*sign;
	else
		out=din;
	return out;
}

/*绝对值大小限制处理函数 限制最小值*/
float AbsFucLimitMin(float In, float OutMin)
{
	float Sign=0;
	float Out;

	if(In>=0)
		Sign=1;
	else
		Sign=(-1);
	
	if(fabs(In)<OutMin)//小于最小值
		Out=OutMin*Sign;
	else//
		Out=In;
	return Out;
}


//线性标定函数
float RatioStand(float orgin, float org1,float org2,float act1,float act2)
{
	float actout;

	if(org2-org1)
		actout=(orgin-org1)*(act2-act1)/(org2-org1) + act1;
	return actout;
}

//找最大、最小
void unifor_find_max_min(Unifor_TypeDef *p)
{
	char i=0;
	p->min=p->org[0];
	p->max=p->org[0];
	p->min_id=0;
	p->max_id=0;
	
	for(i=0;i<p->data_num;i++)
	{
		if(p->org[i]<p->min)
		{
			p->min=p->org[i];
			p->min_id=i;
		}

		if(p->org[i]>p->max)
		{
			p->max=p->org[i];
			p->max_id=i;
		}
	}
}

//归一化
void unifor_cal(Unifor_TypeDef *p)
{
	char i=0;

	p->k_unifor=p->org[p->max_id]/p->max;
	for(i=0;i<p->data_num;i++)
	{
		p->unifor[i]=p->org[i]/p->max;
	}
}

/**************************************************************************
* 函数名:SortFuc
* 功  能:排序处理
* 参  数:排序前的输入数组，要排序的数据长度
* 返回值:注意排序结果直接放到输入数组中的
**************************************************************************/
void SortFuc(float In[],char Len)
{
	char i=0,j=0;
	float temp=0;
	for(i=0;i<Len-1;i++)
	{
		for(j=0;j<Len-i-1;j++)
		{
			if(In[j]>In[j+1])
			{
				temp=In[j];
				In[j]=In[j+1];
				In[j+1]=temp;
			}
		}
	}
}

//单一化处理函数
f32 SingularizeCal(f32 Org,f32 SingularizeA,f32 SingularizeB)
{
	f32 res=0;
	res = Org*SingularizeA + SingularizeB;
	return res;
}

//综合线性标定函数
f32 LinearCal(
    f32 Org,									            //原始数据
	f32 OrgSetA,                                            //原始标定数据A
    f32 OrgSetB,							                //原始标定数据B
	f32 SetA,												//标定数据A
    f32 SetB,									            //标定数据B
    f32 Offset,									            //零点偏移
    f32 OffsetRes,									        //零点偏移结果
    
    f32 SingularizeA,										//单一化参数A
    f32 SingularizeB                                       	//单一化参数B
)
{
    f32 res=0;												//最终结果
	//f32 SetRes;									            //标定结果

	//SetRes = RatioStand(Org,OrgSetA,OrgSetB,SetA,SetB);
	//res = SetRes + Offset;
	//单一化

	return res;

}

//CRC计算函数
u16 CRCCAL(u16 CRC_ORG,u8* p,u16 length)
{
	u8 j;
	u16 reg_crc;

	reg_crc=CRC_ORG;
	do
	{
		reg_crc^=*p++;
		for(j=0;j<8;j++)
		{
			if(reg_crc&0x01)
			{
				reg_crc=(reg_crc>>1)^0xa001;
			}
			else
			{
				reg_crc=reg_crc>>1;
			}
		}
	}while(--length!=0);

	return(reg_crc);
}

//统一缩放
bool Unifor_Zoom(Unifor_TypeDef* p)
{
	char i = 0;
	
	//检查参数
	if (p == 0 || p->data_num <= 0 || p->data_num > UNIFOR_DATA_NUMMAX || p->LimitMax <= 0)
		return false;
	else {}

	//计算Max、Min 这里要找绝对值的最大值！
	//unifor_find_max_min(p);

	//如果最大值小于等于整体缩放限制值 不用缩放直接退出
	if (p->max <= p->LimitMax)
		return true;
	else {}

	//计算整体缩放比例
	p->K_Zoom2MaxLimit = fabs(p->LimitMax) / p->max;

	//整体缩放
	for (i = 0; i < p->data_num; i++)
	{
		p->ZoomOut[i] = p->org[i] * p->K_Zoom2MaxLimit;
	}

	return true;
}

//统一缩放 对求和进行限制
bool Unifor_Zoom_Sum(Unifor_TypeDef* p)
{
	f32 temp = 0;
	char i = 0;

	//先原样更新out
	for (i = 0; i < p->data_num; i++)
	{
		p->ZoomOut[i] = p->org[i];
	}

	//检查参数
	if (p == 0 || p->data_num <= 0 || p->data_num > UNIFOR_DATA_NUMMAX || p->LimitMax <= 0)
		return false;
	else {}

	//求和 注意是绝对值的和
	p->Sum = 0;
	for (i = 0; i < p->data_num; i++)
	{
		p->Sum += fabs(p->org[i]);
	}

	temp = fabs(p->Sum);

	//如果最大值小于等于整体缩放限制值 不用缩放直接退出
	if (temp <= p->LimitMax)
		return true;
	else {}

	//计算整体缩放比例
	p->K_Zoom2MaxLimit = fabs(p->LimitMax) / temp;

	//整体缩放
	for (i = 0; i < p->data_num; i++)
	{
		p->ZoomOut[i] = p->org[i] * p->K_Zoom2MaxLimit;
	}

	return true;
}

/*计算多条直线交点
*输入为直线数组，直线数量
*计算所有直线的交点，返回交点数量，输出交点坐标数组
*/
u16 CalcLinesCrossPoint(PointAnf2Line_TypeDef* Lines, char LineNum, Point_XY_StDef* CrossP)
{
	u16 count = 0;
	int i = 0,j = 0;
	float ang1_rad;
	float ang2_rad;
	float sin1;
	float cos1;
	float sin2;
	float cos2;
	float det;
	float dx;
	float dy;
	float t;
	
	for (i = 0; i < LineNum - 1; i++)
	{
		for (j = i + 1; j < LineNum; j++)
		{
			ang1_rad = Lines[i].Ang * PI / 180.0f;
			ang2_rad = Lines[j].Ang * PI / 180.0f;

			sin1 = sinf(ang1_rad);
			cos1 = cosf(ang1_rad);
			sin2 = sinf(ang2_rad);
			cos2 = cosf(ang2_rad);

			det = sin2 * cos1 - cos2 * sin1;

			/*平行
			if (fabs(det) < 0.001f)
				continue;*/
			/*平行 角度接近*/
			if (fabs(Lines[i].Ang - Lines[j].Ang) < 1 || fabs(fabs(Lines[i].Ang - Lines[j].Ang) - 180) < 1)
				continue;

			dx = Lines[j].P.X - Lines[i].P.X;
			dy = Lines[j].P.Y - Lines[i].P.Y;

			t = (-dx * cos2 + dy * sin2) / det;

			CrossP[count].X = Lines[i].P.X + t * sin1;
			CrossP[count].Y = Lines[i].P.Y + t * cos1;
			count++;
		}
	}
	return count;
}

/*找到所有点的最小外接矩形的对角线长度*/
float CalculateBoundingBoxDiagonal(Point_XY_StDef* points, int numPoints)
{
	// 初始化最小和最大值
	float minX = points[0].X, maxX = points[0].X;
	float minY = points[0].Y, maxY = points[0].Y;
	float width = maxX - minX;
	float height = maxY - minY;
	int i = 0;

	if (numPoints <= 0)
		return 0.0f;

	// 遍历所有点，找到边界
	for (i = 0; i < numPoints; i++)
	{
		if (points[i].X < minX)
			minX = points[i].X;
		if (points[i].X > maxX)
			maxX = points[i].X;
		if (points[i].Y < minY)
			minY = points[i].Y;
		if (points[i].Y > maxY)
			maxY = points[i].Y;
	}

	// 计算宽度、高度和对角线长度
	width = maxX - minX;
	height = maxY - minY;
	return sqrtf(width * width + height * height);
}

/*
功能：判断点p是否在多边形内
输入：p为待测点，poly为多边形顶点数组，n为多边形顶点数
输出：0=外部, 1=内部, 2=边上
*/
char PointInPolygon(Point_XY_Double_Typdef p, Point_XY_Double_Typdef poly[], int n)
{
	char Res = 0;
	char i = 0, j = 0;
	double x1 = 0, y1 = 0;
	double x2 = 0, y2 = 0;
	double cross = 0, dot = 0;
	char intersect = 0;

	for (i = 0, j = n - 1; i < n; j = i++)
	{
		x1 = poly[j].x, y1 = poly[j].y;
		x2 = poly[i].x, y2 = poly[i].y;

		// ====== 1. 判断是否在边上 ======
		cross = (x2 - x1) * (p.y - y1) - (y2 - y1) * (p.x - x1);
		dot = (p.x - x1) * (p.x - x2) + (p.y - y1) * (p.y - y2);
		if (fabs(cross) < EPS && dot <= EPS)
			return 2; // 在边上

		// ====== 2. 射线交点统计 ======
		intersect = ((y1 > p.y) != (y2 > p.y)) &&
			(p.x < (x2 - x1) * (p.y - y1) / (y2 - y1 + EPS) + x1);
		if (intersect)
			Res = !Res;
	}

	return Res ? 1 : 0; // 1=内部, 0=外部
}

/************************************END OF FILE************************************/

