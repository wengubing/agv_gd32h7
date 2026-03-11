/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_MagneticVirtual.h   
  * Version    : V1.1
  * Author     : Master
  * Date       : 2024.03.14
  * Description: 磁条虚拟模块
  *******************************************************************************/

#ifndef _APP_MAGNETICVITURL_H_
#define _APP_MAGNETICVITURL_H_

#ifdef  MAGNETI_CVITURL_GLOBALS
 #define MAGNETI_CVITURL_EXT
#else
 #define MAGNETI_CVITURL_EXT  extern
#endif

#include "STM32f4xx.h"

enum
{
	MagVir_x=0,								/*传入数组中的X脚标*/
	MagVir_y=1,								/*传入数组中的Y脚标*/
	MagVir_z=2,								/*传入数组中的Z脚标*/
	MagVirXyzEnd=3,							/*传入数组中的结束脚标*/
};

enum
{
	MagVir_F=0,								/*传入数组中的前磁导航脚标*/
	MagVir_B=1,								/*传入数组中的后磁导航脚标*/
	MagVir_L=2,								/*传入数组中的左磁导航脚标*/
	MagVir_R=3,								/*传入数组中的右磁导航脚标*/
	MagVirDirEnd=4,							/*传入数组中的结束脚标*/
};

/*磁条虚拟 对像定义*/
typedef struct Magnetic_Virtual
{
	/*初始化输入*/
	u32 MapDistance[MagVirDirEnd];			/*磁导航据运动中心距离	单位:0.1mm
											如果参数设置为0即磁条检测中心和运动中心重合*/

	/*实时调用输入*/
	float RTxy[MagVirXyzEnd];				/*世界坐标系定位信息		单位:1mm 1°
											x轴正方向与车头朝向的夹角为.RTxy[MagVir_z]的输入值；逆时针方向为正*/
	float StartSxy[MagVirXyzEnd];			/*虚拟磁条上的起点坐标	单位:1mm
											StartSxy[MagVir_z] 		不需要传参,传参也无用*/
	float EndSxy[MagVirXyzEnd];				/*虚拟磁条上的终点坐标	单位:1mm
											EndSxy[MagVir_z] 		不需要传参,传参也无用*/

	/*查询与控制输出*/
	s16 MagOffset[MagVirDirEnd];			/*虚拟磁条上的偏移量	单位:0.1mm
											在运动中心朝向磁条巡线中心观测，
											虚拟磁条在传感器巡线中心的左侧为负值；右侧为正值。*/

}_MagneticVirtual_TypeDef;

/*外部方法声明*/
MAGNETI_CVITURL_EXT void MagneticVirtual_DataProcess(_MagneticVirtual_TypeDef *p);  

/***********************************************************************************/
///*MC_init 函数中添加如下代码:*/
//{
//	magnetic_virtual.MapDistance[MagVir_F]=10000;		//1000mm
//	magnetic_virtual.MapDistance[MagVir_B]=10000;		//1000mm
//	magnetic_virtual.MapDistance[MagVir_L]=5000;		//500mm
//	magnetic_virtual.MapDistance[MagVir_R]=5000;		//500mm
//}
///*主循环中添加如下代码:*/
//{
//	/*数据输入管道*/
//	{
//		magnetic_virtual.StartSxy[MagVir_x] = 0;		//0mm
//		magnetic_virtual.StartSxy[MagVir_y] = 0;		//0mm
//		
//		magnetic_virtual.EndSxy[MagVir_x] = 1000;		//1000mm
//		magnetic_virtual.EndSxy[MagVir_y] = 6000;		//6000mm
//		
//		magnetic_virtual.RTxy[MagVir_x] = 3;			//3mm
//		magnetic_virtual.RTxy[MagVir_y] = 230;			//230mm
//		magnetic_virtual.RTxy[MagVir_z] = 25.92845;		//25.92845°
//	}
//	/*算法调用*/
//	{
//		MagneticVirtual_DataProcess(&magnetic_virtual);
//	}
//	/*数据输出管道*/
//	{
//		/*理论输出*/
//		//magnetic_virtual.MagOffset[MagVir_F]=-805;	//虚拟磁条在前磁导航左侧 80.5mm
//		//magnetic_virtual.MagOffset[MagVir_B]=2009;	//虚拟磁条在后磁导航右侧200.9mm
//		//magnetic_virtual.MagOffset[MagVir_L]=782;		//虚拟磁条在左磁导航右侧 78.2mm
//		//magnetic_virtual.MagOffset[MagVir_R]=72;		//虚拟磁条在右磁导航右侧  7.2mm
//	}
//}
/***********************************************************************************/

#endif

/************************************END OF FILE************************************/
