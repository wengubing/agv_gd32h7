/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_MagneticVirtual.c
  * Version    : V1.1
  * Author     : Master
  * Date       : 2024.03.14
  * Description: 磁条虚拟模块
  *******************************************************************************/
#define  MAGNETI_CVITURL_GLOBALS

#include "app_MagneticVirtual.h"
#include <arm_math.h>

/*运算过程的中间变量的对像定义*/
typedef struct MagneticVirtualTemp 
{
	float CalcBuffer;									/*磁导航巡线中心到虚拟磁条的距离				*/
	float Buffer_XY;
	float Radian[MagVirDirEnd];							/*磁导航巡线中心到虚拟磁条虚拟磁条距离			*/
	float Buffer[MagVirXyzEnd];							/*磁导航延长线和虚拟磁条直线交点坐标			*/
	float RtSina[MagVirDirEnd],RtCosa[MagVirDirEnd];	/*磁导航巡线中心世界坐标所需坐标转换系数		*/
	float RtLineK[MagVirDirEnd];						/*磁导航延长线直线方程写斜率					*/
	float MvLineK;										/*虚拟磁条直线方程写斜率						*/
	float RtMap_XY[MagVirDirEnd][MagVirXyzEnd];			/*磁导航巡线中心在世界坐标系下的坐标			*/
}_MagneticVirtualTemp;

/*列矩阵 用于解算世界坐标系下磁条巡线中心的映射坐标 没事别改*/
static const s8 g_ColumnMatrix[MagVirDirEnd] = {1 , -1 , 1 , -1};
/*float的理论最大值，当除0出现的时候，就让结果为flot的最大值即可
(理论最大值是0x7FFFFFF 实际上10000000.0f 即可*/
static const float MagVirtualMaxFloat = 10000000.0f;
/*int的理论最大值，当float转int溢出的时候，就让int结果为0x7FFF或者0x8000 即可*/
static const float MagVirtualMaxint	 = 32767.0f;

/*内部调用函数声明*/
u8   MagVirtual_InputCheck(_MagneticVirtual_TypeDef *p);
void MagVirtual_GetOffset (_MagneticVirtual_TypeDef *p);

/**************************************************************************
* 函数名:	MagneticVirtual_DataProcess()
* 功  能: 磁条虚拟算法解算函数
* 参  数: 磁条虚拟 对像实例指针
* 返回值: 无
**************************************************************************/
void MagneticVirtual_DataProcess(_MagneticVirtual_TypeDef *p_Data)
{
	/*如果输入不合法，将输出结果置位，函数返回*/
	if(MagVirtual_InputCheck(p_Data) == 2)
	{
		u8 i=0;
		for(i=0;i < MagVirDirEnd;i++){p_Data->MagOffset[i] = 0x7FFF;}
		return;
	}
	/*计算得到虚拟磁条偏移量*/
	else
	{
		MagVirtual_GetOffset(p_Data);
	}

}


/**************************************************************************
* 函数名:	MagVirtual_InputCheck()
* 功  能: 检查输入数据合法性检查
* 参  数: 磁条虚拟 对像实例指针
* 返回值: res 0:输入无误 其他：输入有误
**************************************************************************/
u8 MagVirtual_InputCheck(_MagneticVirtual_TypeDef *p_Data)
{
	u8 res = 0,i = 0;

	/*如果初始化输入数据是0，强行使用默认值1000*/
	for(i=0;i<4;i++)
	{
		if(p_Data->MapDistance[i] == 0)
		{
			p_Data->MapDistance[i] = 1000;
			res = 1;
		}
		else
		{
			__asm__("nop");
		}
	}
	
	/*如果输入数据的起点坐标和终点坐标一致，无法进行解算输出错误值*/
	if(p_Data->StartSxy[MagVir_x] == p_Data->EndSxy[MagVir_x]
		&&p_Data->StartSxy[MagVir_y] == p_Data->EndSxy[MagVir_y])
	{
		res = 2;
	}
	else
	{
		__asm__("nop");
	}
	return res;
}
/**************************************************************************
* 函数名:	MagVirtual_GetOffset()
* 功  能: 计算的得到虚拟磁导航的偏离值
* 参  数: 磁条虚拟 对像实例指针
* 返回值: 无
**************************************************************************/
void MagVirtual_GetOffset(_MagneticVirtual_TypeDef *p_Data)
{
	_MagneticVirtualTemp TempBuff;
	u8 i = 0;
	/*得到巡线中的映射坐标 和对应的直线方程*/
	{
		/*得到虚拟巡线的直线方程斜率 */
		if(p_Data->EndSxy[MagVir_x]-p_Data->StartSxy[MagVir_x] != 0)
		{
			TempBuff.MvLineK = 	(p_Data->EndSxy[MagVir_y]-p_Data->StartSxy[MagVir_y])/
								(p_Data->EndSxy[MagVir_x]-p_Data->StartSxy[MagVir_x]);
		}
		/*当虚拟巡线的直线 平行y轴时，k取float的最大值*/
		else
		{
			TempBuff.MvLineK = MagVirtualMaxFloat;
		}

		/*得到磁导航所在的直线方程斜率和坐标映射系数*/
		for(i=0;i < MagVirDirEnd;i++)
		{
			/*三目运算符，确保前、后 和 左、右是用的不同的偏转角度*/
			TempBuff.Radian[i] = ((i < MagVir_L?(p_Data->RTxy[MagVir_z]):(p_Data->RTxy[MagVir_z]+90.0f))*PI/180.0f);
			TempBuff.RtSina[i] = sin(TempBuff.Radian[i]);
			TempBuff.RtCosa[i] = cos(TempBuff.Radian[i]);
			/*当磁导航所在的直线 平行y轴时，k取float的最大值*/
			if(TempBuff.Radian[i] != 0.0f)
			{
				TempBuff.RtLineK[i] = -1.0f/tan(TempBuff.Radian[i]);
			}
			else
			{
				TempBuff.RtLineK[i] = MagVirtualMaxFloat;
			}
		}

		/*磁导航巡线中心在世界坐标系下的映射点*/
		for(i=0;i < MagVirDirEnd;i++)
		{
			/*坐标系旋转平移，依赖于g_ColumnMatrix 数组的正确；MapDistance单位是0.1mm 所以需要÷10*/
			TempBuff.RtMap_XY[i][MagVir_x] = p_Data->RTxy[MagVir_x] + g_ColumnMatrix[i]*TempBuff.RtCosa[i]*p_Data->MapDistance[i]/10.0f;
			TempBuff.RtMap_XY[i][MagVir_y] = p_Data->RTxy[MagVir_y] + g_ColumnMatrix[i]*TempBuff.RtSina[i]*p_Data->MapDistance[i]/10.0f;
		}
	}

	{
		/*两直线方程联立求唯一解*/
		for(i=0;i < MagVirDirEnd;i++)
		{
			/*磁导航延长线和虚拟直线交点*/
			/*直线方程连列求X，	虽然看起来很恶心	但是没事别动这个表达式*/
			if(TempBuff.MvLineK-TempBuff.RtLineK[i] != 0)
			{
				TempBuff.Buffer[MagVir_x] = ( (p_Data->StartSxy[MagVir_x]*TempBuff.MvLineK)-(TempBuff.RtMap_XY[i][MagVir_x]*TempBuff.RtLineK[i]) 
											+ (TempBuff.RtMap_XY[i][MagVir_y]-p_Data->StartSxy[MagVir_y]))/(TempBuff.MvLineK-TempBuff.RtLineK[i]);
			}
			else
			{
				/*除0特殊处理*/
				TempBuff.Buffer[MagVir_x] = ( (p_Data->StartSxy[MagVir_x]*TempBuff.MvLineK)-(TempBuff.RtMap_XY[i][MagVir_x]*TempBuff.RtLineK[i]) 
											+ (TempBuff.RtMap_XY[i][MagVir_y]-p_Data->StartSxy[MagVir_y]))/(MagVirtualMaxFloat);
			}
			/*已知X带入求Y，	虽然看起来很恶心	但是没事别动这个表达式*/
			//TempBuff.Buffer[MagVir_y] = (p_Data->StartSxy[MagVir_x] - TempBuff.Buffer[MagVir_x])*TempBuff.MvLineK + p_Data->StartSxy[MagVir_y];
			/*2024年3月14日-Master 发现BUG，这里应该不能用轨迹直线方程来确定求Y*/
			TempBuff.Buffer[MagVir_y] = (TempBuff.Buffer[MagVir_x] - TempBuff.RtMap_XY[i][MagVir_x])*TempBuff.RtLineK[i] + TempBuff.RtMap_XY[i][MagVir_y];
			/*磁导航偏离距离，两点间距离公式*/
			TempBuff.Buffer_XY = pow((TempBuff.RtMap_XY[i][MagVir_x]-TempBuff.Buffer[MagVir_x]),2) 
										 + pow((TempBuff.RtMap_XY[i][MagVir_y]-TempBuff.Buffer[MagVir_y]),2);
			TempBuff.CalcBuffer= sqrt(TempBuff.Buffer_XY);

			/*磁导航偏离距离不为0的时候判断偏离极性*/
			if(TempBuff.CalcBuffer != 0.0f)
			{
				/*计算磁导航偏离极性：
										磁导航和虚拟磁条的交点(P： X=Buffer[X],Y=Buffer[Y])
										在磁导航运动中连线到车体几何中心（A：X=RtMap_XY[F][X],Y=RtMap_XY[F][Y] B：X=p_Data->RTxy[X],Y=p_Data->RTxy[Y]）向量的左侧还是右侧*/
				/*						虽然看起来很恶心	但是没事别动这个表达式*/
				if(( (p_Data->RTxy[MagVir_x]-TempBuff.RtMap_XY[i][MagVir_x])*(TempBuff.Buffer[MagVir_y]-TempBuff.RtMap_XY[i][MagVir_y])
					-(p_Data->RTxy[MagVir_y]-TempBuff.RtMap_XY[i][MagVir_y])*(TempBuff.Buffer[MagVir_x]-TempBuff.RtMap_XY[i][MagVir_x])) < 0)
				{
					/*MagOffset 单位0.1mm 需要*10.0f*/
					/*MagOffset 超过nt的阈值，就输出对应的0x7FFF或者0x8000*/
					p_Data->MagOffset[i] = (TempBuff.CalcBuffer*10.0f)>MagVirtualMaxint?(0x8000):((-1)*TempBuff.CalcBuffer*10);
				}
				else
				{
					p_Data->MagOffset[i] = (TempBuff.CalcBuffer*10.0f)>MagVirtualMaxint?(0x7FFF):(TempBuff.CalcBuffer*10);
				}
			}
			/*磁导航偏离距离为0直接输出*/
			else
			{
				p_Data->MagOffset[i] = 0;
			}
			
			
		}
	}
}

/************************************END OF FILE************************************/
