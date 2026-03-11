/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ACC_AutoCentre.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-02-07         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
#include <stdbool.h>
#include "includes.h"

/*define-----------------------------------------------------------------------*/
#ifndef __ACC_AutoCentre_H_
#define __ACC_AutoCentre_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ACC_AutoCentre_GLOBALS
	#define ACC_AutoCentre_EXT
#else
	#define ACC_AutoCentre_EXT  extern
#endif
	
	
//目标点位坐标定义
typedef struct
{
	int32_t						LocationX;		//单位0.1mm
	int32_t						LocationY;		//单位0.1mm
	int16_t						LocationZ;		//单位0.01°（0-36000）
	
}SLAM_CoordinatePoint_StDef;

//整车差速对正参数定义
typedef struct
{
    float rotation_angle_threshold;  // 旋转角度阈值
    uint16_t max_rotation_angle;     // 最大旋转角度(度)
	float target_rotation_angle;  // 目标旋转角度
	
}XAdjust_ParameterStDef;


ACC_AutoCentre_EXT SLAM_CoordinatePoint_StDef SLAM_CoordinatePoint;	//实例化SLAM自动上线目标点对象
ACC_AutoCentre_EXT XAdjust_ParameterStDef XAdjust_Parameter;	//实例化整车差速对正参数对象

ACC_AutoCentre_EXT void AutoCentre_Ctrl(void);

#endif




 

