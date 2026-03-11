/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_DependenceAGV.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: AGV自动运行的依赖代码（底盘程序的调用接口、数据管道、依赖CBB等）
  *******************************************************************************/

#ifndef _APP_DEPENDENCEAGV_H_
#define _APP_DEPENDENCEAGV_H_

#ifdef  APP_DEPENDENCEAGV_GLOBALS
 #define APP_DEPENDENCEAGV_EXT
#else
 #define APP_DEPENDENCEAGV_EXT  extern
#endif

/*自定义引用*/
#include <stdbool.h>
#include "STM32f4xx.h"
#include "app_SkeletonAGV.h"
#include "app_AGVInit.h"
 
/*依赖的CBB头文件引用*/


//工控机控制命令 定义
typedef enum
{
	DepAGV_SlamDataSta_OK 			= 0x00,	//完全正常
	DepAGV_SlamDataSta_Initial		= 0x01,	//启动中
	DepAGV_SlamDataSta_Fail			= 0x02,	//定位失败/定位低可靠
	DepAGV_SlamDataSta_Err			= 0x03,	//其他不关心的故障
}DepAGV_SlamData_Sta_EnumDef;

typedef struct
{
	float    	X;		//X定位坐标 0.1mm
	float    	Y;		//Y定位坐标 0.1mm
	float    	Z;		//z定位坐标 0.01° -18000-+18000 
	uint8_t		Sta;	//状态 0-正常 其他-不可用
}
DepAGV_SlamData_InputTypeDef;

typedef struct
{
	float    	RTx;		//X定位坐标 1mm
	float    	RTy;		//Y定位坐标 1mm
	float    	RTz;		//z定位坐标 0.01° 0-36000.0f
	double		Distance;	//单位1m
}
DepAGV_VirtualOdoData_InputTypeDef;

typedef struct
{
	uint8_t		Status			: 1	;/*读状态 0-没有识别 1-识别到RFID	*/
	uint8_t		RSSIStatus		: 7	;/*读卡信号强度指示	0-7*/
	uint16_t	Tag					;/*标签值			*/
	uint16_t	LastTag				;/*历史标签值		*/
}
DepAGV_RFIDData_InputTypeDef;

/*磁导航数据*/
typedef struct
{
	/*常用标准数据*/
	int16_t					Strength			;/*中间磁条 磁场强度*/
	int16_t					Distance			;/*左侧磁条和传感器中心偏差距离 单位：mm*/
	uint8_t					Status			: 1	;/*读状态 0-没有识别 1-识别到磁条	*/
	/*超近磁条停车需要的数据，其他工况无需考虑*/
	uint8_t					L_or_R_Sta		: 3	;/*磁条识别结果按位编码 识别到磁条后必然不为0
													0-bit 左侧有磁条 1-bit 中间侧有磁条 2bit 右侧有磁条*/
	int16_t					DistanceLeft		;/*左侧磁条和传感器中心偏差距离 	单位：mm*/
	int16_t					DistanceRight		;/*右侧侧磁条和传感器中心偏差距离 单位：mm*/
}DepAGV_MagData_InputTypeDef;

/*PGV数据*/
typedef struct
{
	/*二维码朝向必须和调度系统朝向约定方向一致，如果不一致需要在传入前处理成一致的*/
	int16_t 	QR_x		;		/*单位1mm*/
	int16_t 	QR_y		;		/*单位1mm*/
	float 		QR_z		;		/*单位1°*/
	float 		Offset_z	;		/*工装二维码和AGV正交轴相差的最小角度 单位1°*/
	uint16_t 	QR_Tag		;
	uint8_t 	PintSta	: 1	;		/*识别状态，0-未识别 1-识别到二维码*/
}DepAGV_PGVData_InputTypeDef;

/*自动运行代码依赖对象CBB数据源*/
typedef struct
{
	HostCtrl_DATA 					*AGVHostCtrl;		/*AGV框架代码对象指针	*/
	//MotorResolving_TypeDef 			*MRData;			/*运动解算对象			*/
	//_MagneticVirtual_TypeDef 		*VirtualMagnetic;	/*虚拟磁条对象指针		*/
	//_MagneticVirtual_TypeDef 		*VirtualMagPGV;		/*虚拟磁条对象指针，用于工装二维码偏差反算磁条纠偏的零偏*/
	//OdometerData_t  				*VirtualOdo;		/*虚拟里程计对象指针		*/
	
	/*虚拟里程计的定位数据，需要外部输入时做好逻辑转换*/
	DepAGV_VirtualOdoData_InputTypeDef	*VirtualOdoData;
	/*SLAM的定位数据，需要外部输入时做好逻辑转换*/
	DepAGV_SlamData_InputTypeDef		*SlamData;
	/*Rfid的标签值数据，需要外部输入时做好逻辑转换 包括数据滤波 历史值保存等*/
	DepAGV_RFIDData_InputTypeDef		*RfidData;
	/*磁导航的数据，需要外部输入时做好逻辑转换*/
	DepAGV_MagData_InputTypeDef			*F_MagData;
	DepAGV_MagData_InputTypeDef			*B_MagData;
	DepAGV_MagData_InputTypeDef			*L_MagData;
	DepAGV_MagData_InputTypeDef			*R_MagData;
	/*二维码的数据，需要外部输入时做好逻辑转换,区分定位二维码和导航二维码*/
	DepAGV_PGVData_InputTypeDef			*Navigation_QRData;
	DepAGV_PGVData_InputTypeDef			*Location_QRData;

}
DependenceAGV_InputTypeDef;



/*外部调用*/

uint8_t AGV_DependenceInitCheck			(void);
void 	AGV_AsynchronousCBB_Dependence	(void);
void 	AGV_MotionControl_Dependence	(void);
void 	AGV_Skeleton_InData_Dependence	(void);
void 	AGV_Skeleton_Dependence			(void);

APP_DEPENDENCEAGV_EXT	DependenceAGV_InputTypeDef 	g_AGVDependenInputData;



#endif

/************************************END OF FILE************************************/
