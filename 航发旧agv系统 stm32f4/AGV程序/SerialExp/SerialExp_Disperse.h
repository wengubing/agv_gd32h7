/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: SerialExp_Disperse.h
** 创 建 人: Master 
** 描    述: 
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2025年2月14日       
**-------------------------------------------------------------------------------------
***************************************************************************************/  
/*保证以下所有内容只被同一源文件引用一次*/
#ifndef _SERIALEXP_DISPERSE_H__
#define _SERIALEXP_DISPERSE_H__

#include "SwitchboardData.h"


//通信转换板数据内容定义 转接板程序已更新，连接状态全部直接传超时ms计数
typedef struct
{
	uint8_t						*RODataSize			;	//通讯结构体中只读数据大小
	uint8_t						*RWDataSize			;	//通讯结构体中读/写数据大小
	//读取状态
	DataPool_BitDef				*LinkSta			;	//通信状态
	DataPool_BitDef				*Err				;	//传感器故障

	/*只读数据*/
	BMSDataDef 					*BMSData			;	//BMS结构体
	ADDAObj_StDef				*ADDAData			;	//舟正模块数据
	ADDAObj_StDef				*ADDA1Data			;	//舟正模块数据
	ThermodetectorObj_StDef		*ThermodetectorData	;
	Inclinometer_RionObj_StDef	*InclinometerData	;
	MagObj_StDef				*F_MagData			;	//前磁导航信息
	MagObj_StDef				*B_MagData			;	//后磁导航信息
	MagObj_StDef				*L_MagData			;	//左磁导航信息
	MagObj_StDef				*R_MagData			;	//右磁导航信息
	RFIDObj_StDef				*RFIDData			;	//RFID信息
	PgvObjDef					*PGVUpwardData		;	//PGV
	PgvObjDef					*PGVDownwardData	;
	PgvObjDef					*PGVUpward_FData	;
	PgvObjDef					*PGVUpward_BData	;
	PgvObjDef					*PGVDownward_FData	;
	PgvObjDef					*PGVDownward_BData	;

	CommConvObj_StDef			*CommConvData		;
	IPC_Read_StDef 				*IPC_ReadData		;	//工控机的读取数据
	LaserSpot_StDef				*LaserSpotData		;
	Hinson_RadarObj_StDef		*HR_FrontData		;
	Hinson_RadarObj_StDef		*HR_RearData		;
	Hinson_RadarObj_StDef		*HR_LeftData		;
	Hinson_RadarObj_StDef		*HR_RighData		;
	Hinson_RadarObj_StDef		*HR_FrontUpData		;
	Hinson_RadarObj_StDef		*HR_RearUpData		;
	
	ME15Obj_StDef				*ME15_NearData		;
	ME15Obj_StDef				*ME15_FarData		;
	
	LS_Read_StDef				*LS_ReadData		;	//雷赛反馈数据
	SM2924BLMB_Read_StDef		*SM_ReadData		;	//舵机阻挡器 读取数据

	/***************************************以下部分为开发板下发到导航版数据***************************************************/

	VoiceObj_StDef 				*VoiceData			;	//语音数据
	IPC_Write_StDef				*IPC_WriteData		;	//工控机的写入数据
	
	PLCObj_StDef 				*PLCData			;	//PLC数据
	
	LS_Write_StDef				*LS_WriteData		;	//雷赛伺服控制数据
	SM2924BLMB_Write_StDef		*SM_WriteData		;	//舵机阻挡器 控制数据
	Roller_Write_StDef			*Roller_WriteData	;	//滚筒控制数据



}SerialExp_StDef;


#endif

