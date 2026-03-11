/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ACC_AssemlyCtrl.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-02-07         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
/*自定义引用*/
#include <stm32f4xx.h>


/*define-----------------------------------------------------------------------*/


#ifndef __ACC__ASSEMLY_CTRL_H_
#define __ACC__ASSEMLY_CTRL_H_

#ifdef  ACC_ASSEMLY_CTRL_GLOBALS
	#define ACC_ASSEMLY_CTRL_GLOBALS_EXT
#else
	#define  ACC_ASSEMLY_CTRL_GLOBALS_EXT extern
#endif

	
typedef struct
{
	uint32_t SpotDistance					: 1;	//将光斑的检测距离作为联动距离
	uint32_t LinkCom_Remote					: 1;	//从车联动的前进遥感 从双车通讯中得到
	uint32_t FissionLineRrace				: 1;	//联动下分体巡线，各自使用各自的巡线偏差
}ACC_Assemly_ConditionFun_StDef;

/*联动阵型*/
typedef enum 
{
    ACC_Assemly_Scheme_TwoCar_Vertical 				= 0,	/*两车联动，一前一后，头连尾 同向排列*/
	ACC_Assemly_Scheme_TwoCar_Vertical_Subtend		= 1,	/*两车联动，一前一后，头连头 对向排列*/
	ACC_Assemly_Scheme_TwoCar_Vrosswise				= 2,	/*两车联动，一前一后，左连右 同向排列*/
}
ACC_Assemly_Scheme_TypeDef;

typedef enum 
{
	
    ACC_AssemlyCtrlFun_Init		= 0,	/*无效*/
	ACC_AssemlyCtrlFun_Linkage	= 1,	/*整体联动*/
	ACC_AssemlyCtrlFun_Together	= 2,	/*所有车一起动*/
	ACC_AssemlyCtrlFun_Main		= 3,	/*联动主车单动*/
	ACC_AssemlyCtrlFun_SlaveONE	= 4,	/*联动从1单动*/
}
ACC_AssemlyCtrlFun_Enum;
/*输入接口*/
typedef struct
{

	//CCFID_EnumDef				Assemly_Master_Slave_Cn;//动态切换联动通讯的CN口

	ACC_Assemly_ConditionFun_StDef	Assemly_ConditionFun;

	uint8_t						Assemly_Sta 	:1	;	//联动状态 		0-非联动	1-联动
	ACC_AssemlyCtrlFun_Enum		Assemly_Ctrl_Fun	;	//联动方式 		0-无效	1-整体联动	2-所有车一起动	3-联动主车单动	4-联动从1单动 	
	uint8_t						Assemly_Role 	:2	;	//联动主从定位 	0-主车	1-从1车 	2-从2车
	uint8_t						Assemly_SN	 	:2	;	//联动中的车辆编号 "Z"型编码（从左往右 从前往后），从1开始，最多编号至4 共4台车
	ACC_Assemly_Scheme_TypeDef	Assemly_Scheme		;	//联动中的车辆阵型

	float 						Assemly_Distance	;	//联动距离 单位1mm
}ACC_Assemly_InputData_StDef;


typedef enum 
{
	
    ACC_AssemlyOutSta_Solo		= 0,	/*单车*/
	ACC_AssemlyOutSta_Linkage	= 1,	/*整体联动*/
}
ACC_AssemlyOutSta_Enum;

typedef enum 
{
    ACC_AssemlyOutTrack_Solo	= 0,	/*单车自己的纠偏*/
	ACC_AssemlyOutTrack_Linkage	= 1,	/*整体联动的纠偏*/
}
ACC_AssemlyOutTrackType_Enum;

/*从车的前进遥感数据得到方式*/
typedef enum 
{
    ACC_AssemlyOutRocker_Remote		= 0,	/*遥控器*/
	ACC_AssemlyOutRocker_RCS		= 1,	/*调度系统下发*/
	ACC_AssemlyOutRocker_LinkCom	= 2,	/*联动通讯*/
}
ACC_AssemlyOutRockerType_Enum;

/*外部访问的PGV数据接口*/
typedef struct
{
	uint8_t	Status			: 1	;/*读状态 0-没有识别 1-识别到PGV	*/
	uint8_t	Tag				: 7	;/*标签结果*/
	int16_t	X					;/*X 方向定位数据，单位1mm*/
	int16_t	Y					;/*Y 方向定位数据，单位1mm*/
}ACC_Assemly_OutPGV_StDef;


/*外部访问的巡线偏差数据接口*/
typedef struct
{
	int16_t Front;	//
	int16_t Back;	//
	int16_t Left;	//
	int16_t Right;	//
}ACC_Assemly_OutTrack_StDef;



/*输出接口*/
typedef struct
{
	/*供外部使用的状态情况*/
	ACC_AssemlyOutSta_Enum			Assemly_OutSta			;	//联动状态 		0-非联动中	1-联动中
	ACC_AssemlyOutRockerType_Enum	Assemly_OutStaRocker	;	//遥感的数据来源	0-遥控器 1-调度系统 2-双车通讯 
	ACC_AssemlyOutTrackType_Enum	Assemly_OutTrackType	;	//底盘需要的纠偏方式 0-自己纠自己的  1-联动整体纠

	/*供外部使用的功能数据*/
	ACC_Assemly_OutPGV_StDef		Assemly_OutPGV				;	//联动的PGV定位数据
	ACC_Assemly_OutTrack_StDef		Assemly_OutTrack			;		//联动的纠偏数据
	
}ACC_Assemly_OutData_StDef;

//方法与实例接口
ACC_ASSEMLY_CTRL_GLOBALS_EXT	ACC_Assemly_OutData_StDef		ACC_Assemly_OutData;
ACC_ASSEMLY_CTRL_GLOBALS_EXT	ACC_Assemly_InputData_StDef		ACC_Assemly_InputData;
ACC_ASSEMLY_CTRL_GLOBALS_EXT	void ACC_AssemlyCtrl(void);
ACC_ASSEMLY_CTRL_GLOBALS_EXT	bool ACC_Assemly_ObsUpdate(void);
ACC_ASSEMLY_CTRL_GLOBALS_EXT	bool ACC_Assemly_ObsEnCustom(void);
ACC_ASSEMLY_CTRL_GLOBALS_EXT	bool ACC_Assemly_ObsAreaCustom(void);
ACC_ASSEMLY_CTRL_GLOBALS_EXT	bool ACC_Assemly_InputObs(void);

#endif


 

