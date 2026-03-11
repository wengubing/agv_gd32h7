/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ObjUpdate_Assemly.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-06-07         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
/*自定义引用*/

/*define-----------------------------------------------------------------------*/

#ifndef __ObjUpdate_Assemly_H_
#define __ObjUpdate_Assemly_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ObjUpdate_Assemly_GLOBALS
	#define ObjUpdate_Assemly_GLOBALS_EXT
#else
	#define ObjUpdate_Assemly_GLOBALS_EXT  extern
#endif

//标定参数
typedef struct
{
	float Org1;	//标定高值
	float Org2;	//标定低值
	float Act1;	//标定量程上限
	float Act2;	//标定量程下限
	float Offset; //车体到测距传感器镜面偏移
}RatioStandPara_TypeDef;

typedef struct
{
	float 					Org;	//原始数据
	RatioStandPara_TypeDef 	Para;	//标定参数
	float 					Cal;	//标定值，单位mm
}AssemlyAdcSensor_TypeDef;


typedef struct
{
	AssemlyAdcSensor_TypeDef LaserDis_L;
	AssemlyAdcSensor_TypeDef LaserDis_R;
	AssemlyAdcSensor_TypeDef SpotDis;
	
}Acc_AssemlySensorData_TypeDef;

ObjUpdate_Assemly_GLOBALS_EXT Acc_AssemlySensorData_TypeDef 	g_AssemlySensorData;
	
ObjUpdate_Assemly_GLOBALS_EXT	void ObjUpdate_Assemly(void);
#endif


 

