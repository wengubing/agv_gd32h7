/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ACC_AbsorptionCtrl.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-02-07         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
/*自定义引用*/
#include <stdbool.h>
#include "common_def.h"	
#include "..\..\CBB\user\app_DataFilter\app_DataFilter.h"

/*define-----------------------------------------------------------------------*/

#ifndef __ACC__ABSORPTIONCTRL_H_
#define __ACC__ABSORPTIONCTRL_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ACCABSORPTIONCTRL_GLOBALS
	#define ACCABSORPTIONCTRL_GLOBALS_EXT
#else
	#define ACCABSORPTIONCTRL_GLOBALS_EXT  extern
#endif
	
//空气悬挂参数 数据对象定义
typedef struct
{
	u16 CylinderDiameter;									//气缸直径 单位：mm
	u16 CylinderNum;										//气缸数量
	u16 K_Arm;											    //力臂系数
	u16 DeadWeight;											//自重 单位：kg
	u16 LowWeight;											//轻载重量 单位：kg
	u16 HighWeight;											//重载重量 单位：kg
	float K_Down;											//下压力系数
	u16 SetLowOrg;
	u16 SetHighOrg;
	float SetLow;
	float SetHigh;
}AirCompressorPara_TypeDef;

typedef struct
{
	AirCompressorPara_TypeDef Para;								//参数结构体

	u16 Org;													//原始值
	u16 S;														//气缸截面积
	
	DataFilter_TypeDef P_Fiter;									//原始值滤波
	
	float P_RealTime;											//实时压力 单位：MPa
	float P_SetPoint;											//设定压力 单位：MPa
	float Disvalue;
}AirCompressor_Typedef;

ACCABSORPTIONCTRL_GLOBALS_EXT bool AbsorptionCtrl_Ctrl(void);

#endif


 

