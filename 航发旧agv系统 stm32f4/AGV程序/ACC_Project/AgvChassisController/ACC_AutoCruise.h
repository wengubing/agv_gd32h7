/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ACC_AutoCruise.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-09-19
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
#include <stdbool.h>
#include <stm32f4xx.h>

/*define-----------------------------------------------------------------------*/
#ifndef __ACC_AutoCruise_H_
#define __ACC_AutoCruise_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ACC_AutoCruise_GLOBALS
	#define ACC_AutoCruise_EXT
#else
	#define ACC_AutoCruise_EXT  extern
#endif

typedef enum 
{
    AutoCruise_Null_Fun		= 0,/*没有巡航功能*/
	AutoCruise_Straight_Fun	= 1,/*直线巡航功能*/
	AutoCruise_Flexible_Fun	= 2,/*自由巡航功能*/
	AutoCruise_Trace_Fun	= 3,/*巡线巡航功能*/
}
ACC_AutoCruiseFun_Enum;


typedef enum
{
	AutoCruise_Still				= 0,

	/*正向、横向直线模式*/
	AutoCruise_Mode_PosStraight 	= 1,
	AutoCruise_Mode_InfStraight 	= 2,

	/*正向、横向自由模式*/
	AutoCruise_Mode_PosFlexible 	= 11,
	AutoCruise_Mode_InfFlexible 	= 12,

	/*正向、横向巡线模式*/
	AutoCruise_Mode_PosTrace 		= 22,
	AutoCruise_Mode_InfTrace 		= 23,

}ACC_AutoCruise_Mode_EnumDef;


/*配置接口*/
typedef struct
{
	ACC_AutoCruiseFun_Enum 	AutoCruise_Fun;		/*巡航功能定义*/

}ACC_AutoCruise_ParaData_StDef;



//
typedef struct
{
	s16		XAxisOutput,					/*解算后摇杆X值*/
			YAxisOutput,					/*解算后摇杆Y值*/
			ZAxisOutput;					/*解算后摇杆Z值*/
	bool	EmgStop;						/*急停*/
	u16		J41,							/*遥控器J41端口原始值，值为多少则表示亚空气J41端口的第几位被置1*/
			J42,							/*遥控器J42端口原始值，值为多少则表示亚空气J42端口的第几位被置1*/
			J43,							/*遥控器J43端口原始值，值为多少则表示亚空气J43端口的第几位被置1*/
			J44;
}ACC_AutoCruise_371_WHData_StDef;

/*输入接口*/
typedef struct
{
	uint8_t Data;
	ACC_AutoCruise_371_WHData_StDef	WHData;

}ACC_AutoCruise_InputData_StDef;

/*中间变量*/
typedef struct
{
	uint8_t AutoCruise_STM;
	float	Vx_temp;
	float	Vy_temp;
	float	Vz_temp;
	ACC_AutoCruise_371_WHData_StDef	WH_temp;
}ACC_AutoCruise_Temp_StDef;


/*输出接口*/
typedef struct
{
	//ACC_AutoCruise_Mode_EnumDef		TargetMode;
	float 							Vx;
	float 							Vy;
	float 							Vz;
	uint8_t 						Data;
	
}ACC_AutoCruise_OutputData_StDef;

typedef struct
{
	/*配置接口*/
	ACC_AutoCruise_ParaData_StDef		Para	;
	/*输入接口*/
	ACC_AutoCruise_InputData_StDef		Input	;
	/*中间变量*/
	ACC_AutoCruise_Temp_StDef			Temp	;
	/*输出接口*/
	ACC_AutoCruise_OutputData_StDef		Output	;

}ACC_AutoCruise_Data_StDef;

ACC_AutoCruise_EXT ACC_AutoCruise_Data_StDef	g_AutoCruise_Data;

ACC_AutoCruise_EXT void AutoCentre_Ctrl(void);

#endif




 

