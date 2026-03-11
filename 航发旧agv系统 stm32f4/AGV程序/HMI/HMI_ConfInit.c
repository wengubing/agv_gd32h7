/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	HMI_ConfInit.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-10-27
	* Description:
	******************************************************************************
**/

#define  HMI_ConfInit_GLOBALS

#include "HMI_ConfInit.h"
#include "HMI.h"
//HMI对象初始化,hmi参数对象 从 系统参数对象中读取初始化一次

void ObjInit_HMI(void)
{
	//HMI对象指针成员初始化
    //外部him对象初始化
    ExtHmi.p2HMI_CommObj = (HMI_CommObj_TypeDef*)HMI_Buf;
	ExtHmi.p2Data = (u16*)(ExtHmi.p2HMI_CommObj->Screen_Head);

	//启动时强行指定跳到初始页
	ExtHmi.p2HMI_CommObj->PageCmd = HMIPage_Start;
	//如果没有读回？默认其为初始页
	ExtHmi.p2HMI_CommObj->PageNum = HMIPage_Start;

	//界面对象指针初始化
	ExtHmi.p2Scr_Start 						= (Screen_HMIPage_Start*)(ExtHmi.p2HMI_CommObj->Screen_Head);//20 开始界面	
	ExtHmi.p2Scr_ParaObs 					= (Screen_ParaObs*)(ExtHmi.p2HMI_CommObj->Screen_Head);//20 开始界面
	ExtHmi.p2Scr_Main 						= (Screen_Main_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//21主界面状态对象指针		
	ExtHmi.p2Scr_Sys 						= (SystemInformation_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//24系统信息	
	ExtHmi.p2Scr_AdOnboard 					= (Screen_AdOnboard_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//25板载模拟量
	ExtHmi.p2Scr_Wireless 					= (WirelessStatus_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);	//26遥控器数据

	ExtHmi.p2Scr_LinkStaus 					= (Screen_LinkStaus_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//27通信状态计数
//	ExtHmi.p2Scr_IOStatus 					= (Screen_IOStatus_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//28IO状态
//	CanopenStatus_Typedef* p2Scr_CanopenStatus;				//29canopen信息
	ExtHmi.p2Scr_GeneralParameter 			= (Screen_GeneralParameter_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//31一般参数

	ExtHmi.p2Scr_WheelPParameter 			= (Screen_WheelParameter_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head); //32 轮系参数
	ExtHmi.p2Scr_ManualControl 				= (Screen_ManualControl_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);  //30 手动控车
	ExtHmi.p2Scr_HeightInstall 				= (Screen_HeightInstall_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);  //33高度标定界面对象指针
	ExtHmi.p2Scr_AngleSet 					= (Screen_AngleSet_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);  //34角度0标定界面对象指针

	ExtHmi.pScr_LDRCalibration 				= (Screen_LDRCalibration_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//35激光测距标定
	ExtHmi.pScr_NOParaObs					= (Screen_NOParaObs*)(ExtHmi.p2HMI_CommObj->Screen_Head);//36NO参数界面对象指针			
//	//37强制开关 HMI_CommObj_TypeDef结构体内
	ExtHmi.p2Scr_ReinforcedCalibration 		= (Screen_ReinforcedCalibration_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);  //38雷赛伺服标定	
	ExtHmi.p2Scr_PGVCalibration_TypeDef 	= (Screen_PGVCalibration_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//39PGV标定
	ExtHmi.p2Scr_Odometer 					= (Screen_OdometerCalibration_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//40里程计
	ExtHmi.p2Scr_ControlCoeff 				= (Screen_ControlCoeff_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//41控制系数
	ExtHmi.p2Scr_PID 						= (Screen_PID_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//42PID
	ExtHmi.p2Scr_PositionRadar 				= (Screen_PositionRadar_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//43位置雷达标定

	ExtHmi.p2Scr_PositionRadarSpeed 		= (Screen_PositionRadarSpeed_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//44位置雷达速度标定  
	ExtHmi.p2Scr_BVRelated					= (Screen_BVRelated_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//45电量电压关联参数 
	ExtHmi.p2Scr_MoreParameters 			= (Screen_MoreParameters_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//46更多参数
	ExtHmi.p2Scr_Angle 						= (Screen_Angle_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//47角度数据

	ExtHmi.p2Scr_High 						= (Screen_High_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//47角度数据
	ExtHmi.p2Scr_HangingHeight 				= (Screen_HangingHeight_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//49悬挂高度
	ExtHmi.p2Scr_LegHeight 					= (Screen_LegHeight_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//50支腿高度
	ExtHmi.p2Scr_LaserRanging 				= (Screen_LaserRanging_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//51激光测距
	ExtHmi.p2Scr_USRanging 					= (Screen_USRanging_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//52超声波测距

	ExtHmi.p2Scr_Pressure 					= (Screen_Pressure_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//53压力传感器
	ExtHmi.p2Scr_AnalogCollection			= (Screen_AnalogCollection_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//54模拟量采集模块
	ExtHmi.p2Scr_AnalogOut					= (Screen_AnalogOut_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//55模拟量输出模块
	ExtHmi.p2Scr_GPS						= (Screen_GPS_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//56GPS

	ExtHmi.p2Scr_LaserNavigation 			= (Screen_LaserNavigation_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head); //57激光导航数据  
	ExtHmi.p2Scr_DispatchSystemCmd 			= (DispatchSystemCmd_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//58调度系统
	ExtHmi.p2Scr_AtuoRunStatus 				= (AtuoRunStatus_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//59自动运行
	ExtHmi.p2Scr_SafeSenorStatus_Typedef	= (SafeSenorStatus_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//60避障开关参数设置

	ExtHmi.p2Scr_Navigation 				= (Screen_Navigation_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);	 //61磁导航+RFID界面显示状态对象指针
	ExtHmi.p2Scr_ReinforcedServo 			= (Screen_ReinforcedServo_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head); //62雷赛伺服 
	ExtHmi.p2Scr_PGV						= (Screen_PGV_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//63PGV参数显示界面对象指针			               
	ExtHmi.p2Scr_Camera						= (Screen_Camera_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//64栈板相机

	ExtHmi.p2Scr_UnionStatus				= (Screen_UnionStatus_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//65联动状态界面对象指针
	ExtHmi.p2Scr_UnionParameter				= (Screen_UnionParameter_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);
	ExtHmi.p2Scr_UnionEMG					= (Screen_UnionEMG_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);
    ExtHmi.p2Scr_LightSpot					= (Screen_LightSpot_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//66光斑状态 

	ExtHmi.p2Scr_Position					= (Screen_Position_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//67位置定位雷达状态
	ExtHmi.p2Scr_TiltSensor					= (Screen_TiltSensor_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//68倾角传感器
	ExtHmi.p2Scr_Pulser						= (Screen_Pulser_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//69手摇脉冲器
	ExtHmi.p2Scr_BMS						= (Screen_BMS_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//70BMS

	ExtHmi.p2Scr_ForcedOut					= (Screen_ForcedOut_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//71IO强制输出
	ExtHmi.p2Scr_EEPROM						= (Screen_EEPROM_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//72EEPROM界面对象指针
	ExtHmi.p2Scr_ANCStatusPage				= (ANCStatusPage_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//74安川驱动器界面对象指针
	ExtHmi.p2Scr_DBCOMSet					= (DBCOMSet_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//75多倍通界面对象指针
	ExtHmi.p2Scr_MMU						= (Screen_MMU_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head); //内存控制页对象指针
	ExtHmi.p2Scr_Path_Comm					= (PathPoint_StDef*)(ExtHmi.p2HMI_CommObj->Screen_Head); //缓存路径信息对象指针

	ExtHmi.p2Scr_Path						= (Screen_Path_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head); //链表路径信息对象指针
	ExtHmi.p2Scr_AtuoInf					= (Screen_AtuoInf_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head);//更多自动信息对象指针
	ExtHmi.p2Scr_Button						= (Screen_Button_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head); //按钮界面对象指针
	ExtHmi.p2Scr_SysCmd						= (Screen_SysCmd_Typedef*)(ExtHmi.p2HMI_CommObj->Screen_Head); //系统控制命令对象指针

	ExtHmi.p2Scr_GeneralParameter			=(Screen_GeneralParameter_TypeDef*)(ExtHmi.p2HMI_CommObj->Screen_Head);
    //AGV初始化方向默认为未初始化	
    ExtHmi.p2HMI_CommObj->InitDirection 	= 361;

	//厂家调试按钮默认不显示
	ExtHmi.p2HMI_CommObj->ButtonEnable.Bit.ManufacturerHide_Bit15 = 1;
}



/************************************END OF FILE************************************/
