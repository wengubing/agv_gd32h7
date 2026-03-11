/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: SwitchboardData.h
** 创 建 人: MT 
** 描    述: 导航转接板数据处理头文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2025年2月14日       
**-------------------------------------------------------------------------------------
***************************************************************************************/  

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef _SwitchboardData_H__
#define _SwitchboardData_H__

//确保仅在该文件编译时实例化一次对象
#ifdef  SwitchboardData_GLOBALS
	#define SwitchboardData_EXT
#else
	#define SwitchboardData_EXT  extern
#endif

#include "includes.h"

#if defined(Navigation)
#include "Navigation_EQPComm_Init.h"
#endif

#define SerialExpReadMax									27//串口扩展板读取数据最大长度
#define SerialExpWriteMax									9 //串口扩展板写入数据最大长度
	
#define SerialExpReadMax_2									27//串口扩展板读取数据最大长度
#define SerialExpWriteMax_2									9 //串口扩展板写入数据最大长度
 #pragma pack (1)
//导航板使用传感器选择
//EQP 通讯设备使能
typedef struct
{
	unsigned long BMS					: 1;//BMS
	unsigned long Voice					: 1;//语音播报
	unsigned long AD					: 1;//模拟量采集0
	unsigned long AD1					: 1;//模拟量采集1
	unsigned long Thermodetector		: 1;//温湿度检测模块
	unsigned long Inclinometer_Rion		: 1;//倾角传感器检测模块
	unsigned long MagF					: 1;//磁导航前
	unsigned long MagB					: 1;//磁导航后
	unsigned long MagL					: 1;//磁导航左
	unsigned long MagR					: 1;//磁导航右
	unsigned long RFID					: 1;//RFID
	unsigned long PGVUpward				: 1;//向上PGV
	unsigned long PGVDownward			: 1;//向下PGV
	unsigned long PGVUpward_F			: 1;//向上前PGV
	unsigned long PGVUpward_B			: 1;//向上后PGV
	unsigned long PGVDownward_F			: 1;//向下前PGV
	unsigned long PGVDownward_B			: 1;//向下后PGV

	unsigned long CommConv				: 1;
	unsigned long IPC					: 1;
	unsigned long LaserSpot				: 1;
	unsigned long PLC					: 1;
	

	unsigned long Hinson_Radar_Front	: 1;//兴颂modbus通讯的前雷达 读取指令
	unsigned long Hinson_Radar_Rear		: 1;//兴颂modbus通讯的后雷达 读取指令
	unsigned long Hinson_Radar_Left		: 1;//兴颂modbus通讯的左雷达 读取指令
	unsigned long Hinson_Radar_Righ		: 1;//兴颂modbus通讯的右雷达 读取指令
	unsigned long Hinson_Radar_FrontUp	: 1;//兴颂modbus通讯的前斜面雷达 读取指令
	unsigned long Hinson_Radar_RearUp	: 1;//兴颂modbus通讯的后斜面雷达 读取指令
	unsigned long ME15_Near				: 1;//易佰特 无线io数据模块  发送--站号为近端模块站号
	unsigned long ME15_Far				: 1;//易佰特 无线io数据模块  发送 --站号为远端模块站号
	
	unsigned long LsLiftModule_F		: 1;//前雷赛伺服
	unsigned long LsLiftModule_B		: 1;//后雷赛伺服
	unsigned long LsLiftModule_L		: 1;//左雷赛伺服
	unsigned long LsLiftModule_R		: 1;//右雷赛伺服

	unsigned long Roller				: 1;//辊筒控制卡

	unsigned long SM45BL_MB_1			: 1;//舵机阻挡器
	unsigned long SM45BL_MB_2			: 1;//舵机阻挡器
	unsigned long SM45BL_MB_3			: 1;//舵机阻挡器
	unsigned long SM45BL_MB_4			: 1;//舵机阻挡器

}DataPool_BitDef;

//BMS 结构化通信内容定义
typedef struct
{
    uint16_t	Voltage			;//电池电压 --- 0.1V
    int16_t		Current			;//电池电流 --- 0.1A
    uint16_t	Soc          	;//电池soc --- %0.1
    uint8_t		Hot_Current		;
	uint8_t		Soh          	;//电池soh 
    uint16_t	Alarm          	;//电池报警

}BMSDataDef;

//语音模块数据内容定义
typedef struct
{
    uint8_t BEEP_SongNumber		;	//播放歌曲序号 --- 1DHR
	uint8_t BEEP_SongVol		;	//喇叭音量(2071)
}VoiceObj_StDef;

//舟正数据内容定义
typedef struct
{
	uint16_t DAQM4202_Ch0		;	//通道0数据 --- 1DHR
	uint16_t DAQM4202_Ch1		;	//通道1数据 --- 1DHR
	uint16_t DAQM4202_Ch2		;	//通道2数据 --- 1DHR
	uint16_t DAQM4202_Ch3		;	//通道3数据 --- 1DHR
	uint16_t DAQM4202_Ch4		;	//通道4数据 --- 1DHR
	uint16_t DAQM4202_Ch5		;	//通道5数据 --- 1DHR
	uint16_t DAQM4202_Ch6		;	//通道6数据 --- 1DHR
	uint16_t DAQM4202_Ch7		;	//通道7数据 --- 1DHR
}ADDAObj_StDef;


typedef struct
{
	uint16_t Data				;
}ThermodetectorObj_StDef;

typedef struct
{
	uint16_t Data				;
}Inclinometer_RionObj_StDef;

//磁导航传感器数据内容定义
typedef struct
{
	uint16_t	Status			;	//传感器状态 占用16个DHR,每个传感器占用1个DHR,其余为预留 持续为0
	uint16_t	Value			;	//感应到的磁条值 有磁条为2
	int16_t		Distance		;	//感应磁条与传感器中心的距离 -70mm----70mm (有效)   范围（-630---630）(面向车，磁条在左边输出正)
	int16_t		Intensity		;	//感应到的磁条的磁场强度 磁条槽口向上，强度为负
}MagObj_StDef;

//RFID传感器数据内容定义
typedef struct
{
	uint8_t 	ReadState		;	//RFID读状态，0为无卡，1为正在读卡
	uint8_t		Strong			;	//读卡信息强度，0-7依次增强
	uint16_t 	Data			;	//rfid标签结果
}RFIDObj_StDef;

//PGV数据内容定义
typedef struct
{									
    uint8_t		ReadSta		;	//识别状态 有没有识别到二维码
    uint8_t		Tag			;	//标签值 
    int16_t		X_Value 	;	//x轴值 
    int16_t		Y_Value 	;	//y轴值 
	int16_t		Z_Value 	;	//z轴值
}PgvObjDef;

typedef struct
{
	uint16_t Data				;
}CommConvObj_StDef;



#if defined(Navigation)
//工控机运行状态结果 定义
typedef enum
{
	IPCRunSta_Invalid	 	= 0x00,	//0：无效
	IPCRunSta_Initial 		= 0x01,	//1：初始化
	IPCRunSta_Normal		= 0x02,	//2：正常
	IPCRunSta_Stop			= 0x03,	//3：停止
	IPCRunSta_Fault			= 0x04,	//4：故障
}IPCRunSta_EnumDef;
//工控机故障代码结果 定义
typedef enum
{									/*	位定义：（0：未触发  1：触发）*/
	IPCErrCode_Null 			= 0x00,
	IPCErrCode_WriteFail 		= 0x01,	//0：配置写入失败
	IPCErrCode_ReadFail 		= 0x02,	//1：配置读取失败
	IPCErrCode_ModBusLinkFail	= 0x04,	//2：Modbus连接失败
	IPCErrCode_LoginFail		= 0x08,	//3：Locator登录失败
	IPCErrCode_LaunchFail		= 0x10,	//4：Locator启动失败
	IPCErrCode_SocketLinkFail	= 0x20,	//5：Locator Socket连接失败
	IPCErrCode_LocationFail		= 0x30,	//6：定位失败
}IPCErrCode_EnumDef;

//工控机故障代码结果 定义
typedef enum
{									/*	位定义：（0：未触发  1：触发）*/
	IPCLocationSta_Invalid 			= 0x00,	//0：无效 
	IPCLocationSta_Fail 			= 0x01,	//1：定位失败
	IPCLocationSta_Underway			= 0x02,	//2：正在定位
	IPCLocationStae_LowReliability	= 0x03,	//3：成功定位（低可靠性）
	IPCLocationStae_HighReliability	= 0x04,	//4：成功定位（高可靠性）
}IPCLocationSta_EnumDef;

//工控机控制命令 定义
typedef enum
{									/*	位定义：（0：未触发  1：触发）*/
	IPCCMD_Default 			= 0x00,	//
	IPCCMD_Reset  			= 0x01,	//0：系统复位（0：无效 1：执行）
	IPCCMD_Launch 			= 0x02,	//1：系统启动
	IPCCMD_Stop 			= 0x04,	//2：系统停止
	IPCCMD_SYS_Relocate		= 0x08,	//3：以定位系统当前实时定位坐标启动重定位
	IPCCMD_ModBus_Relocate	= 0x10,	//4：以Modbus给定重定位参数启动重定位
	IPCCMD_Now_Relocate		= 0x20,	//5：以最近保存的重定位参数启动重定位
	IPCCMD_Save_Relocate	= 0x40,	//6：执行保存参数实时坐标操作
	IPCCMD_Layer_Switch		= 0x80,	//7：启动图层切换
}IPCCMD_EnumDef;
/*在导航转接板 中的数据结构定义*/
typedef struct
{
	IPCRunSta_EnumDef		RunSta;			//运行状态	
	
	IPCLocationSta_EnumDef	LocationSta;	//定位状态
	IPCErrCode_EnumDef		ErrCode;		//故障代码	
	int32_t					LocationX;		//单位0.1mm
	int32_t					LocationY;		//单位0.1mm
	int16_t					LocationZ;		//单位0.01°（-18000-18000）
	uint8_t					NC;
}IPC_Read_StDef;

typedef struct
{
	IPCCMD_EnumDef	CMD;			//控制命令
	uint8_t			LayerNum;		//系统毫秒计数
	int32_t			SetX;			//重定位参数X 单位0.1mm	
	int32_t			SetY;			//
	int16_t			SetZ;			//单位0.01°（-18000-18000）
}IPC_Write_StDef;
#else
/*主控板 中的数据结构定义*/
typedef struct
{
	uint8_t		RunSta;			//运行状态	
	uint8_t		LocationSta;	//定位状态
	uint8_t		ErrCode;		//故障代码	
	int32_t		LocationX;		//单位0.1mm
	int32_t		LocationY;		//单位0.1mm
	int16_t		LocationZ;		//单位0.01°（-18000-18000）
	uint8_t		NC;
}IPC_Read_StDef;

typedef struct
{
	uint8_t			CMD;			//控制命令
	uint8_t			LayerNum;		//系统毫秒计数
	int32_t			SetX;			//重定位参数X 单位0.1mm	
	int32_t			SetY;			//
	int16_t			SetZ;			//单位0.01°（-18000-18000）
}IPC_Write_StDef;

#endif

typedef struct
{
	uint16_t Data[8]				;
}LaserSpot_StDef;

typedef struct
{
	uint16_t Data				;
}Hinson_RadarObj_StDef;

typedef struct
{
	uint16_t Data				;
}ME15Obj_StDef;

//PLC模块数据内容定义
typedef struct
{
	uint8_t PLC_GreenLight    : 1;	// 1：状态指示灯绿
    uint8_t PLC_YELLOWLight   : 1;	// 2：状态指示灯黄
    uint8_t PLC_REDLight      : 1;	// 3：状态指示灯红
    uint8_t PLC_sysLight      : 1;	// 4：系统指示灯
    uint8_t PLC_powerLight    : 1;	// 5：电源指示灯
    uint8_t PLC_alarmLight    : 1;	// 6：告警指示灯
}PLCObj_StDef;


/*雷赛升降模块控制指令数据*/

/*雷赛升降模块运行状态*/
typedef struct
{
	/*实时输入接口*/
	u16 ControlMode;			/*控制模式输入*/
	u16 ControlHeight;			/*控制高度输入*/

	u8 MagneticNavigationCmd;	//自动下控制指令（0：不需要磁导航，模组不下降；1：需要磁导航，模组下降）

}LS_Write_StDef;

typedef struct
{	
	/*输出接口*/
	u8 Operating;				/*运行状态 0- 1*/
	u8 LS_Alarm;				/*报警状态 */
}LS_Read_StDef;

typedef struct
{
	u16 Control_Power		: 1;	/* 1：上电标志， 0：未上电*/	
	u16 Control_Cmd			: 1;	/* 1：使能，	 	0：失能*/
	u16 NC 					: 14;	/*占位*/
	s16 SpeedCmd;					/*速度指令*/
	
}Roller_Write_StDef;
/*舵机模块控制指令数据*/

/*舵机模块运行状态*/
/*舵机模块运行状态*/
typedef struct
{
	/*实时输入接口*/

	uint16_t High_Limit_1 	: 1;	/*1-抵达高限位	0-未到限位*/
	uint16_t Low_Limit_1 	: 1;	/*1-抵达低限位	0-未到限位*/
	uint16_t Sta_1 			: 1;	/*1-动作中 		0-无动作*/
	uint16_t Err_1 			: 1;	/*1-出现故障		0-无故障*/

	uint16_t High_Limit_2 	: 1;	/*1-抵达高限位	0-未到限位*/
	uint16_t Low_Limit_2 	: 1;	/*1-抵达低限位	0-未到限位*/
	uint16_t Sta_2 			: 1;	/*1-动作中 		0-无动作*/
	uint16_t Err_2 			: 1;	/*1-出现故障		0-无故障*/

	uint16_t High_Limit_3 	: 1;	/*1-抵达高限位	0-未到限位*/
	uint16_t Low_Limit_3 	: 1;	/*1-抵达低限位	0-未到限位*/
	uint16_t Sta_3 			: 1;	/*1-动作中 		0-无动作*/
	uint16_t Err_3 			: 1;	/*1-出现故障		0-无故障*/

	uint16_t High_Limit_4 	: 1;	/*1-抵达高限位	0-未到限位*/
	uint16_t Low_Limit_4 	: 1;	/*1-抵达低限位	0-未到限位*/
	uint16_t Sta_4 			: 1;	/*1-动作中 		0-无动作*/
	uint16_t Err_4 			: 1;	/*1-出现故障		0-无故障*/
	
}SM2924BLMB_Read_StDef;

typedef struct
{
	/*输出接口*/
	uint16_t High_Do_1 	: 1;	/*1-向高位动作	0-不动作*/
	uint16_t Low_Do_1 	: 1;	/*1-向低位动作	0-不动作*/

	uint16_t High_Do_2	: 1;	/*1-向高位动作	0-不动作*/
	uint16_t Low_Do_2 	: 1;	/*1-向低位动作	0-不动作*/

	uint16_t High_Do_3 	: 1;	/*1-向高位动作	0-不动作*/
	uint16_t Low_Do_3 	: 1;	/*1-向低位动作	0-不动作*/

	uint16_t High_Do_4 	: 1;	/*1-向高位动作	0-不动作*/
	uint16_t Low_Do_4 	: 1;	/*1-向低位动作	0-不动作*/
	
	uint16_t NC 		: 8;	/*占位*/

}SM2924BLMB_Write_StDef;


#pragma pack () // 取消结构体对齐

#pragma pack (1)
//通信转换板数据内容定义 转接板程序已更新，连接状态全部直接传超时ms计数
typedef struct
{
    //读取状态
	uint8_t						RODataSize			;	//通讯结构体中只读数据大小
	uint8_t						RWDataSize			;	//通讯结构体中读/写数据大小
	DataPool_BitDef				LinkSta				;	//通信状态
	DataPool_BitDef				Err					;	//传感器故障

	/*只读数据*/
	BMSDataDef 					BMSobj				;	//BMS结构体
	ADDAObj_StDef				ADDAobj				;	//舟正模块数据
	ADDAObj_StDef				ADDA1obj			;	//舟正模块数据
	ThermodetectorObj_StDef		Thermodetectorobj	;
	Inclinometer_RionObj_StDef	Inclinometerobj		;
	MagObj_StDef				F_Magobj			;	//前磁导航信息
	MagObj_StDef				B_Magobj			;	//后磁导航信息
	MagObj_StDef				L_Magobj			;	//左磁导航信息
	MagObj_StDef				R_Magobj			;	//右磁导航信息
	RFIDObj_StDef				RFIDobj				;	//RFID信息
	PgvObjDef					PGVUpwardobj		;	//PGV
	PgvObjDef					PGVDownwardobj		;
	PgvObjDef					PGVUpward_Fobj		;
	PgvObjDef					PGVUpward_Bobj		;
	PgvObjDef					PGVDownward_Fobj	;
	PgvObjDef					PGVDownward_Bobj	;

	CommConvObj_StDef			CommConvobj			;
	IPC_Read_StDef 				IPC_Readobj			;	//工控机的读取数据
	LaserSpot_StDef				LaserSpotobj		;
	Hinson_RadarObj_StDef		HR_Frontobj			;
	Hinson_RadarObj_StDef		HR_Rearobj			;
	Hinson_RadarObj_StDef		HR_Leftobj			;
	Hinson_RadarObj_StDef		HR_Righobj			;
	Hinson_RadarObj_StDef		HR_FrontUpobj		;
	Hinson_RadarObj_StDef		HR_RearUpobj		;

	ME15Obj_StDef				ME15_Nearobj		;
	ME15Obj_StDef				ME15_Farobj			;
	
	LS_Read_StDef				LS_Readobj			;	//雷赛反馈数据
	SM2924BLMB_Read_StDef		SM_Readobj			;	//舵机阻挡器控制数据

	/***************************************以下部分为开发板下发到导航版数据***************************************************/

	VoiceObj_StDef 				Voiceobj			;	//语音数据
	IPC_Write_StDef				IPC_Writeobj		;	//工控机的写入数据

	PLCObj_StDef 				PLCobj				;	//PLC数据
	
	LS_Write_StDef				LS_Writeobj			;	//雷赛伺服控制数据
	SM2924BLMB_Write_StDef		SM_Writeobj			;	//舵机阻挡器控制数据
	Roller_Write_StDef			Roller_Writeobj		;	//滚筒控制数据
	
}SerialExpObj_StDef;
#pragma pack () // 取消结构体对齐

SwitchboardData_EXT void SwitchboardData_Init(void);

#endif

