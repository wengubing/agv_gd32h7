/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: EQPComm_Init.h
** 创 建 人: Master 
** 描    述: 项目配置文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.1
** 创建日期: 
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __EQPComm_Init_h__
#define __EQPComm_Init_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  EQPComm_Init_GLOBALS
    #define EQPComm_Init_EXT
#else
    #define EQPComm_Init_EXT  extern
#endif

#include "CommConf.h"


//从站组定义
typedef struct
{
	CCFEQP_Slave_StDef HMI;									//触摸屏
	CCFEQP_Slave_StDef DSP;									//调度系统
	CCFEQP_Slave_StDef Assembly_Slave;						//联动从机通信
	CCFEQP_Slave_StDef API_Slave;							//二次开发 从机通信
	unsigned short Number;									//外设数量
}CCFEQP_Slaves_StDef;

//主站组定义
typedef struct
{
	CCFEQP_Master_StDef COMMTEST;							//通信测试
	CCFEQP_Master_StDef COMMTEST1;							//不连续地址1
	CCFEQP_Master_StDef WHA;								//A遥控器
	CCFEQP_Master_StDef WHB;								//B遥控器
	CCFEQP_Master_StDef BMS_CommConv;						//卡式转接板 自制BMS
	CCFEQP_Master_StDef BMS_MCB;							//直接自制BMS
	CCFEQP_Master_StDef Voice;								//语音播报
	CCFEQP_Master_StDef AD;									//模拟量采集0
	CCFEQP_Master_StDef AD1;								//模拟量采集1
	CCFEQP_Master_StDef Thermodetector;						//温湿度检测模块
	CCFEQP_Master_StDef Inclinometer_Rion;					//倾角传感器检测模块
	CCFEQP_Master_StDef MagF;								//磁导航前
	CCFEQP_Master_StDef MagB;								//磁导航后
	CCFEQP_Master_StDef MagL;								//磁导航左
	CCFEQP_Master_StDef MagR;								//磁导航右
	CCFEQP_Master_StDef RFID;								//RFID传感器
	CCFEQP_Master_StDef RFID_Up;							//朝天RFID传感器
	CCFEQP_Master_StDef PGVUpward;							//向上pgv传感器
	CCFEQP_Master_StDef PGVDownward;						//向下pgv传感器
	CCFEQP_Master_StDef PGVUpward_F;						//向上前pgv传感器
	CCFEQP_Master_StDef PGVUpward_B;						//向上后pgv传感器
	CCFEQP_Master_StDef PGVDownward_F;						//向下前pgv传感器
	CCFEQP_Master_StDef PGVDownward_B;						//向下后pgv传感器
	CCFEQP_Master_StDef SerialExp;							//串口扩展板
	CCFEQP_Master_StDef SerialExp_2;						//第二串口扩展板
	CCFEQP_Master_StDef CommConv;							//通信转换板
	CCFEQP_Master_StDef CommConv2;							//通信转换板
	CCFEQP_Master_StDef IPC;								//SLAM工控机
	CCFEQP_Master_StDef LaserSpot;							//光斑传感器
	CCFEQP_Master_StDef Roll_Controller;					//辊筒控制板
	CCFEQP_Master_StDef Assembly_Master;					//联动主机通信
	CCFEQP_Master_StDef PLC;								//PLC
	
	/*由于兴颂不支持0x17功能码所以需要区分读写指令*/
	CCFEQP_Master_StDef Hinson_Radar_Front_R;					//兴颂modbus通讯的前雷达
	CCFEQP_Master_StDef Hinson_Radar_Front_W;					//兴颂modbus通讯的前雷达
	CCFEQP_Master_StDef Hinson_Radar_Rear_R;					//兴颂modbus通讯的后雷达
	CCFEQP_Master_StDef Hinson_Radar_Rear_W;					//兴颂modbus通讯的后雷达
	CCFEQP_Master_StDef Hinson_Radar_Left_R;					//兴颂modbus通讯的左雷达
	CCFEQP_Master_StDef Hinson_Radar_Left_W;					//兴颂modbus通讯的左雷达
	CCFEQP_Master_StDef Hinson_Radar_Righ_R;					//兴颂modbus通讯的右雷达
	CCFEQP_Master_StDef Hinson_Radar_Right_W;					//兴颂modbus通讯的右雷达
	CCFEQP_Master_StDef Hinson_Radar_FrontUp_R;					//兴颂modbus通讯的右雷达
	CCFEQP_Master_StDef Hinson_Radar_FrontUp_W;					//兴颂modbus通讯的右雷达
	CCFEQP_Master_StDef Hinson_Radar_RearUp_R;					//兴颂modbus通讯的前斜面雷达
	CCFEQP_Master_StDef Hinson_Radar_RearUp_W;					//兴颂modbus通讯的后斜面雷达

	/*由于亿佰特IO模块只支持线圈指令  所以需要区分读写指令*/
	CCFEQP_Master_StDef ME15_Near_R;	//亿佰特 无线io数据模块 安装在AGV的近端 		输入IO结果
	CCFEQP_Master_StDef ME15_Near_W;	//亿佰特 无线io数据模块 安装在AGV的近端 		输出IO结果
	CCFEQP_Master_StDef ME15_Far_R;		//亿佰特 无线io数据模块 安装在客户设备的远端 	输入IO结果
	CCFEQP_Master_StDef ME15_Far_W;		//亿佰特 无线io数据模块 安装在客户设备的远端 	输出IO结果

	CCFEQP_Master_StDef PLC_Estop;		//PLC 安全模块
	
	CCFEQP_Master_StDef API_Master;		//二次开发 主机通信

	/*串口私有协议*/
	CCFEQP_Master_StDef KXD_BMS;

	CCFEQP_Master_StDef BJF_PGV_Upward;							//向上pgv传感器
	CCFEQP_Master_StDef BJF_PGV_Downward;						//向下pgv传感器
	CCFEQP_Master_StDef BJF_PGV_Upward_F;						//向上前pgv传感器
	CCFEQP_Master_StDef BJF_PGV_Upward_B;						//向上后pgv传感器
	CCFEQP_Master_StDef BJF_PGV_Downward_F;						//向下前pgv传感器
	CCFEQP_Master_StDef BJF_PGV_Downward_B;						//向下后pgv传感器
	
	CCFEQP_Master_StDef BMS_CATL;								//宁德时代
	
	CCFEQP_Master_StDef DA;									//周正模块

	//安川m3板卡
	CCFEQP_Master_StDef ANC;
	//多倍通配置切换
	CCFEQP_Master_StDef DBSET;
	
	unsigned short Number;									//外设数量
}CCFEQP_Masters_StDef;
	

//EQP 通讯设备使能
typedef enum
{
	Para_EQP_HMI 					= 1 ,//HMI
	Para_EQP_DSP 					= 2 ,//DSP
	Para_EQP_Assembly_Slave			= 3 ,
	Para_EQP_API_Slave				= 4 ,//二次开发 从机通信

	Para_EQP_WHA 					= 6 ,//遥控器A
	Para_EQP_BMS_CommConv			= 7 ,//通过卡式转接板得到的BMS数据
	Para_EQP_BMS_MCB				= 8 ,//通过BMS直连开发板得到的BMS数据
	Para_EQP_Voice					= 9 ,//语音播报
	Para_EQP_AD						= 10,//模拟量采集0
	Para_EQP_AD1					= 11,//模拟量采集1
	Para_EQP_Thermodetector			= 12,//温湿度检测模块
	Para_EQP_Inclinometer_Rion		= 13,//倾角传感器检测模块
	Para_EQP_MagF					= 14,//磁导航前
	Para_EQP_MagB					= 15,//磁导航后
	Para_EQP_MagL					= 16,//磁导航左
	Para_EQP_MagR					= 17,//磁导航右
	Para_EQP_RFID					= 18,//RFID
	Para_EQP_RFID_Up				= 19,//朝天RFID传感器
	Para_EQP_PGVUpward				= 20,//向上PGV
	Para_EQP_PGVDownward			= 21,//向下PGV
	Para_EQP_PGVUpward_F			= 22,//向上前PGV
	Para_EQP_PGVUpward_B			= 23,//向上后PGV
	Para_EQP_PGVDownward_F			= 24,//向下前PGV
	Para_EQP_PGVDownward_B			= 25,//向下后PGV
	Para_EQP_SerialExp				= 26,//串口拓展板 
	Para_EQP_SerialExp_2			= 27,//第二串口拓展板 
	Para_EQP_CommConv				= 28,//通信转换板 卡式转接板
	Para_EQP_CommConv2				= 29,//通信转换板 卡式转接板
	Para_EQP_IPC					= 30,//工控机 SLAM导航中间件
	Para_EQP_Roll_Controller		= 31,//辊筒控制器
	Para_EQP_WHB					= 32,//遥控器B
	Para_EQP_LaserSpot				= 33,//光斑传感器
	Para_EQP_Assembly_Master		= 34,//联动通信
	Para_EQP_Hinson_Radar_Front		= 35,//兴颂modbus通讯的前雷达 读取指令
	Para_EQP_Hinson_Radar_Rear		= 36,//兴颂modbus通讯的后雷达 读取指令
	Para_EQP_Hinson_Radar_Left		= 37,//兴颂modbus通讯的左雷达 读取指令
	Para_EQP_Hinson_Radar_Righ		= 38,//兴颂modbus通讯的右雷达 读取指令
	Para_EQP_Hinson_Radar_FrontUp	= 39,//兴颂modbus通讯的前斜面雷达 读取指令
	Para_EQP_Hinson_Radar_RearUp	= 40,//兴颂modbus通讯的后斜面雷达 读取指令
	Para_EQP_ME15_Near				= 41,//易佰特 无线io数据模块  发送--站号为近端模块站号
	Para_EQP_ME15_Far				= 42,//易佰特 无线io数据模块  发送 --站号为远端模块站号

	Para_PLC_Estop					= 43,//PLC 安全模块

	Para_EQP_BMS_CATL				= 44,//宁德时代
	
	Para_EQP_DA						= 45,//模拟量输出模块
	Para_EQP_API_Master				= 46,//二次开发 主机通信

	/*串口私有协议*/
	Para_EQP_BMS_KXD				= 100,//凯信达BMS，私有串口协议

	Para_EQP_BJF_PGV_Upward			= 101,//向上PGV
	Para_EQP_BJF_PGV_Downward		= 102,//向下PGV
	Para_EQP_BJF_PGV_Upward_F		= 103,//向上前PGV
	Para_EQP_BJF_PGV_Upward_B		= 104,//向上后PGV
	Para_EQP_BJF_PGV_Downward_F		= 105,//向下前PGV
	Para_EQP_BJF_PGV_Downward_B		= 106,//向下后PGV

	//安川m3板卡
	Para_EQP_ANC 					= 107,

	//多倍通配置切换
	Para_EQP_DBSET					= 108,

	//PLC控制面板灯
	Para_EQP_PLC					= 109,

}Para_EQPCommSource_EnumDef;


//EQP 通讯设备使能
typedef struct
{
	unsigned long HMI 					: 1;//HMI
	unsigned long DSP 					: 1;//DSP
	unsigned long Assembly_Slave		: 1;
	unsigned long API_Slave				: 1;//二次开发 从机通信

	unsigned long WHA 					: 1;//遥控器A
	unsigned long BMS_CommConv			: 1;//通过卡式转接板得到的BMS数据
	unsigned long BMS_MCB				: 1;//通过BMS直连开发板得到的BMS数据
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
	unsigned long RFID_Up				: 1;//朝天RFID
	unsigned long PGVUpward				: 1;//向上PGV
	unsigned long PGVDownward			: 1;//向下PGV
	unsigned long PGVUpward_F			: 1;//向上前PGV
	unsigned long PGVUpward_B			: 1;//向上后PGV
	unsigned long PGVDownward_F			: 1;//向下前PGV
	unsigned long PGVDownward_B			: 1;//向下后PGV
	unsigned long SerialExp				: 1;//串口拓展板 
	unsigned long SerialExp_2			: 1;//第二串口拓展板
	unsigned long CommConv				: 1;//通信转换板 卡式转接板
	unsigned long CommConv2				: 1;//通信转换板 卡式转接板
	unsigned long IPC					: 1;//工控机 SLAM导航中间件
	unsigned long Roll_Controller		: 1;//辊筒控制器
	unsigned long WHB					: 1;//遥控器B
	unsigned long LaserSpot				: 1;//光斑传感器
	unsigned long Assembly_Master		: 1;//联动通信
	unsigned long Hinson_Radar_Front	: 1;//兴颂modbus通讯的前雷达 读取指令
	unsigned long Hinson_Radar_Rear		: 1;//兴颂modbus通讯的后雷达 读取指令
	unsigned long Hinson_Radar_Left		: 1;//兴颂modbus通讯的左雷达 读取指令
	unsigned long Hinson_Radar_Righ		: 1;//兴颂modbus通讯的右雷达 读取指令
	unsigned long Hinson_Radar_FrontUp	: 1;//兴颂modbus通讯的前斜面雷达 读取指令
	unsigned long Hinson_Radar_RearUp	: 1;//兴颂modbus通讯的后斜面雷达 读取指令
	unsigned long ME15_Near				: 1;//易佰特 无线io数据模块  发送--站号为近端模块站号
	unsigned long ME15_Far				: 1;//易佰特 无线io数据模块  发送 --站号为远端模块站号
	unsigned long PLC_Estop				: 1;//PLC 安全模块
	unsigned long BMS_CATL				: 1;//宁德时代
	unsigned long DA					: 1;//DA模块
	unsigned long API_Master			: 1;//二次开发 主机通信
	

	unsigned long BMS_KXD				: 1;//凯信达私有协议得到的BMS数据
	unsigned long BJF_PGV_Upward		: 1;//向上PGV
	unsigned long BJF_PGV_Downward		: 1;//向下PGV
	unsigned long BJF_PGV_Upward_F		: 1;//向上前PGV
	unsigned long BJF_PGV_Upward_B		: 1;//向上后PGV
	unsigned long BJF_PGV_Downward_F	: 1;//向下前PGV
	unsigned long BJF_PGV_Downward_B	: 1;//向下后PGV
	
	//安川m3板卡
	unsigned long ANC					: 1;
	//多倍通配置切换
	unsigned long DBSET					: 1;
	
	//PLC控制面板灯
	unsigned long PLC					: 1;
}Para_EQPCommSource_BitDef;

//EQP 通讯设备CN端口
typedef struct
{
	CCFID_EnumDef	HMI 					;//HMI
	CCFID_EnumDef	DSP 					;//DSP
	CCFID_EnumDef	Assembly_Slave			;
	CCFID_EnumDef	API_Slave				;//二次开发 从机通信

	CCFID_EnumDef	WHA 					;//遥控器A
	CCFID_EnumDef	BMS_CommConv			;//通过卡式转接板得到的BMS数据
	CCFID_EnumDef	BMS_MCB					;//通过BMS直连开发板得到的BMS数据
	CCFID_EnumDef	Voice					;//语音播报
	CCFID_EnumDef	AD						;//模拟量采集0
	CCFID_EnumDef	AD1						;//模拟量采集1
	CCFID_EnumDef	Thermodetector			;//温湿度检测模块
	CCFID_EnumDef	Inclinometer_Rion		;//倾角传感器检测模块
	CCFID_EnumDef	MagF					;//磁导航前
	CCFID_EnumDef	MagB					;//磁导航后
	CCFID_EnumDef	MagL					;//磁导航左
	CCFID_EnumDef	MagR					;//磁导航右
	CCFID_EnumDef	RFID					;//RFID
	CCFID_EnumDef	RFID_Up					;//朝天RFID
	CCFID_EnumDef	PGVUpward				;//向上PGV
	CCFID_EnumDef	PGVDownward				;//向下PGV
	CCFID_EnumDef	PGVUpward_F				;//向上前PGV
	CCFID_EnumDef	PGVUpward_B				;//向上后PGV
	CCFID_EnumDef	PGVDownward_F			;//向下前PGV
	CCFID_EnumDef	PGVDownward_B			;//向下后PGV
	CCFID_EnumDef	SerialExp				;//串口拓展板
	CCFID_EnumDef	SerialExp_2				;//第二串口拓展板	
	CCFID_EnumDef	CommConv				;//通信转换板 卡式转接板
	CCFID_EnumDef	CommConv2				;//通信转换板 卡式转接板
	CCFID_EnumDef	IPC						;//工控机 SLAM导航中间件
	CCFID_EnumDef	Roll_Controller			;//辊筒控制器
	CCFID_EnumDef	WHB						;//遥控器B
	CCFID_EnumDef	LaserSpot				;//光斑传感器
	CCFID_EnumDef	Assembly_Master			;//联动通信
	CCFID_EnumDef	Hinson_Radar_Front		;//兴颂modbus通讯的前雷达 读取指令
	CCFID_EnumDef	Hinson_Radar_Rear		;//兴颂modbus通讯的后雷达 读取指令
	CCFID_EnumDef	Hinson_Radar_Left		;//兴颂modbus通讯的左雷达 读取指令
	CCFID_EnumDef	Hinson_Radar_Righ		;//兴颂modbus通讯的右雷达 读取指令
	CCFID_EnumDef	Hinson_Radar_FrontUp	;//兴颂modbus通讯的前斜面雷达 读取指令
	CCFID_EnumDef	Hinson_Radar_RearUp		;//兴颂modbus通讯的后斜面雷达 读取指令
	CCFID_EnumDef	ME15_Near				;//易佰特 无线io数据模块  发送--站号为近端模块站号
	CCFID_EnumDef	ME15_Far				;//易佰特 无线io数据模块  发送 --站号为远端模块站号
	
	CCFID_EnumDef	PLC_Estop				;//PLC 安全模块
	CCFID_EnumDef	DA						;//宁德时代
	CCFID_EnumDef	API_Master				;//二次开发 主机通信

	CCFID_EnumDef	BMS_KXD					;

	CCFID_EnumDef	BJF_PGV_Upward			;//向上PGV
	CCFID_EnumDef	BJF_PGV_Downward		;//向下PGV
	CCFID_EnumDef	BJF_PGV_Upward_F		;//向上前PGV
	CCFID_EnumDef	BJF_PGV_Upward_B		;//向上后PGV
	CCFID_EnumDef	BJF_PGV_Downward_F		;//向下前PGV
	CCFID_EnumDef	BJF_PGV_Downward_B		;//向下后PGV

	//安川m3板卡
	CCFID_EnumDef	ANC						;
	//多倍通配置切换
	CCFID_EnumDef	DBSET					;
	
	CCFID_EnumDef	BMS_CATL				;//宁德时代

	//PLC控制面板灯
	CCFID_EnumDef	PLC						;
	
	
}Para_EQPCommSource_CNDef;

//EQP 通讯设备地址
typedef struct
{
	uint8_t	HMI 					;//HMI
	uint8_t	DSP 					;//DSP
	uint8_t	Assembly_Slave			;
	uint8_t	API_Slave				;//二次开发 从机通信

	uint8_t	WHA 					;//遥控器A
	uint8_t	BMS_CommConv			;//通过卡式转接板得到的BMS数据
	uint8_t	BMS_MCB					;//通过BMS直连开发板得到的BMS数据
	uint8_t	Voice					;//语音播报
	uint8_t	AD						;//模拟量采集0
	uint8_t	AD1						;//模拟量采集1
	uint8_t	Thermodetector			;//温湿度检测模块
	uint8_t	Inclinometer_Rion		;//倾角传感器检测模块
	uint8_t	MagF					;//磁导航前
	uint8_t	MagB					;//磁导航后
	uint8_t	MagL					;//磁导航左
	uint8_t	MagR					;//磁导航右
	uint8_t	RFID					;//RFID
	uint8_t	RFID_Up					;//朝天RFID
	uint8_t	PGVUpward				;//向上PGV
	uint8_t	PGVDownward				;//向下PGV
	uint8_t	PGVUpward_F				;//向上前PGV
	uint8_t	PGVUpward_B				;//向上后PGV
	uint8_t	PGVDownward_F			;//向下前PGV
	uint8_t	PGVDownward_B			;//向下后PGV
	uint8_t	SerialExp				;//串口拓展板 
	uint8_t	SerialExp_2				;//第二串口拓展板 
	uint8_t	CommConv				;//通信转换板 卡式转接板
	uint8_t	CommConv2				;//通信转换板 卡式转接板
	uint8_t	IPC						;//工控机 SLAM导航中间件
	uint8_t	Roll_Controller			;//辊筒控制器
	uint8_t	WHB						;//遥控器B
	uint8_t	LaserSpot				;//光斑传感器
	uint8_t	Assembly_Master			;//联动通信
	uint8_t	Hinson_Radar_Front		;//兴颂modbus通讯的前雷达 读取指令
	uint8_t	Hinson_Radar_Rear		;//兴颂modbus通讯的后雷达 读取指令
	uint8_t	Hinson_Radar_Left		;//兴颂modbus通讯的左雷达 读取指令
	uint8_t	Hinson_Radar_Righ		;//兴颂modbus通讯的右雷达 读取指令
	uint8_t	Hinson_Radar_FrontUp	;//兴颂modbus通讯的前斜面雷达 读取指令
	uint8_t	Hinson_Radar_RearUp		;//兴颂modbus通讯的后斜面雷达 读取指令
	uint8_t	ME15_Near				;//易佰特 无线io数据模块  接收--站号为近端模块站号
	uint8_t	ME15_Far				;//易佰特 无线io数据模块  接收 --站号为远端模块站号
	uint8_t	PLC_Estop				;//PLC 安全模块
	uint8_t	DA						;//模拟量输出模块
	uint8_t	API_Master				;//二次开发 主机通信


	uint8_t	BMS_KXD					;
	
	uint8_t	BJF_PGV_Upward			;//向上PGV
	uint8_t	BJF_PGV_Downward		;//向下PGV
	uint8_t	BJF_PGV_Upward_F		;//向上前PGV
	uint8_t	BJF_PGV_Upward_B		;//向上后PGV
	uint8_t	BJF_PGV_Downward_F		;//向下前PGV
	uint8_t	BJF_PGV_Downward_B		;//向下后PGV

	//安川m3板卡
	uint8_t	ANC						;
	//多倍通配置切换
	uint8_t	DBSET					;
	
	uint8_t BMS_CATL				;//宁德时代

	//PLC控制面板灯
	uint8_t PLC						;
}Para_EQPCommSource_AddressDef;

//EQP 通讯设备初始化
typedef struct
{
	Para_EQPCommSource_BitDef 		EN;
	Para_EQPCommSource_CNDef		CN;
	Para_EQPCommSource_AddressDef	Address;
}Para_EQPCommSource_Def;


#define HMIDataMax											100//触摸屏数据最大长度
//HMI通信数据池 定义
typedef struct
{
	u16 Data[HMIDataMax];									//触摸屏数据
}EQP_HMI_StDef;

#define DSPDataMax											Para_Index_AGVEnd//调度系统数据最大长度
//调度系统通信数据池 定义
typedef struct
{
	u16 Data[DSPDataMax];									//调度系统数据
}EQP_DSP_StDef;

#define Assembly_SlaveDataMax								300//调度系统数据最大长度
//联动通信数据池 定义
typedef struct
{
	u16 Data[Assembly_SlaveDataMax];
}EQP_Assembly_Slave_StDef;

#define API_Slave_DataMax								30//二次开发 数据最大长度
//二次开发 数据池 定义
typedef struct
{
	u16 Data[API_Slave_DataMax];
}EQP_API_Slave_StDef;

//工控机 读取结构化通信内容定义
typedef struct
{
	uint16_t					RunSta;			//运行状态
	uint16_t					ErrCode;		//故障代码
	uint16_t					LocationSta;	//定位状态
	uint16_t					LocationX_H;
	uint16_t					LocationX_L;
	//int32_t						LocationX;		//单位0.1mm
	uint16_t					LocationY_H;
	uint16_t					LocationY_L;
	//int32_t						LocationY;		//单位0.1mm
	int16_t						LocationZ;		//单位0.01°（0-36000）
	uint16_t					LocationNC;		//预留
	uint16_t					CMD1_Tick;		//控制命令1反馈
	uint16_t					SyS_ms;			//系统毫秒计数	
	
}EQP_IPC_Read_Obj_StDef;
//工控机 写入结构化通信内容定义
typedef struct
{
	uint16_t			CMD1;			//控制命令1 
	uint16_t			CMD2;			//控制命令2 预留
	uint16_t			SetX_H;
	uint16_t			SetX_L;
	//int32_t				SetX;			//重定位参数X 单位0.1mm
	uint16_t			SetY_H;
	uint16_t			SetY_L;
	//int32_t				SetY;			//
	int16_t				SetZ;			//单位0.01°（0-36000）
	uint16_t			SetNC;			//重定位参数 预留
	uint16_t			CalBackSyS_ms;	//系统毫秒计数	
	uint16_t			LayerNum;		//系统毫秒计数	
}EQP_IPC_Write_Obj_StDef;
#define IPC_ReadMax											12/*工控机读取数据最大长度*/
#define IPC_WriteMax										10/*工控机写入数据最大长度*/
//工控机通信数据池 定义
typedef struct
{
	u16 RD[IPC_ReadMax];									//工控机读取数据
	u16 WD[IPC_WriteMax];									//工控机写入数据
	EQP_IPC_Read_Obj_StDef	* R_Data;
	EQP_IPC_Write_Obj_StDef	* W_Data;
}EQP_IPC_StDef;

#define Inclinometer_RionReadMax							4//倾角传感器读取数据最大长度
#define Inclinometer_RionWriteMax				    		1//倾角传感器写入数据最大长度
//瑞芬通信数据池 定义
typedef struct
{
	u16 RD[Inclinometer_RionReadMax];
	u16 WD[Inclinometer_RionWriteMax];
}EQP_Inclinometer_Rion_StDef;

#define Roll_Controller_ReadMax								15//辊筒控制器读取数据最大长度		
#define Roll_Controller_WriteMax							15//辊筒控制器写入数据最大长度
//辊筒控制器通信数据池 定义
typedef struct
{
	u16 RD[Roll_Controller_ReadMax];						//辊筒控制器读取数据
	u16 WD[Roll_Controller_WriteMax];						//辊筒控制器写入数据
}EQP_Roll_Controller_StDef;	

#define COMMTESTReadMax										10//
#define COMMTESTWriteMax									1//
//测试通信数据池 定义
typedef struct
{
	u16 RD[COMMTESTReadMax];
	u16 WD[COMMTESTWriteMax];
}EQP_COMMTEST_StDef;

#define WirelessHandleReadMax								17//遥控器读取数据最大长度
#define WirelessHandleWriteMax								2//遥控器写入数据最大长度
//遥控器通信数据池 定义
typedef struct
{
	u16 RD[WirelessHandleReadMax];							//遥控器读取数据
	u16 WD[WirelessHandleWriteMax];							//遥控器写入数据
}EQP_WirelessHandle_StDef;

//BMS 结构化通信内容定义
typedef struct __attribute__((packed))
{
	u16 RedState;											// 读取状态
	u16 BeatCont;											// 心跳计数
	s16 Voltage;											// 电压 0.1v
	s16 Current;											// 电流 0.1A,正为充电，负为放电
	u16 SOC;												// SOC 0.1 %
	u16 SOH;												// SOH	
	s16 HOT_Current;										// 电池热电流 0.1A
	u16 Alarm_Code;											// 报警
	u16 BMS_Ready;											// 准备状态	
	s16 MaxCellVoltage;     								// 最高单体电压 0.1
	s16 MinCellVoltage;     								// 最低单体电压 0.1v
  	s16 Temperature_High;									// 最高温度
	s16 Temperature_Low;									// 最低温度
	u16 AlarmBit;											// 报警码
	u16 Bit_Infor_1;										// 电池位信息
	u16 P_IR;												// 正极绝缘电阻
	u16 N_IR;												// 负极绝缘电阻
	u16 Max_VCell_Diff;										// 电池阻最大压差
	u16 Max_T_Diff;											// 电池组最大温差
	u16 Max_VCell_Idx;										// 最高单体电压逻辑编号
	u16 Min_VCell_Idx;										// 最低单体电压逻辑编号
	u16 Max_Temp_Idx;										// 最高温度逻辑编号
	u16 Min_Temp_Idx;										// 最低温度逻辑编号
	u16 Relay_Sta_1;										// 继电器状态_DHR1
	u16 Relay_Sta_2;										// 继电器状态_DHR2
	u16 Relay_Sta_3;										// 继电器状态_DHR3
	u16 DCHG_DI_1;											// 放电诊断信息1
	u16 DCHG_DI_2;											// 放电诊断信息2
	u16 CHG_DI_1;											// 充电诊断信息1
	u16 CHG_DI_2;											// 充电诊断信息2
	u16 GB_Fault_Code;										// 国标通信故障代码
	u16 V_CHG;												// 充电机输出电压
	u16 I_CHG;												// 充电机输出电流
	u16 Time_Charged;										// 已充电时间
	u16 CHG_485_INFO;										// 485-充电信息
	u16 CHG_CAN_INFO;										// Can-充电信息
	u16 TCC;												// 充电累计容量
	u16 TDC;												// 放电累计容量
	u16 C_TOT;												// 电池组总容量
	u16 C_REM;												// 电池组剩余容量
	u16 ACC_T_DCHG;											// 累计总放电时间
	u16 ACC_T_CHG;											// 累计总充电时间
	u16 T_OP_TOT;											// 累计总使用时间
	u16 N_CYCLES;											// 累计充放电次数
}EQP_BMS_CommConvObj_StDef;

#define BMS_CommConv_ReadMax											49//BMS读取数据最大长度,7+1+1
#define BMS_CommConv_WriteMax											1//BMS写入数据最大长度
//BMS通信数据池 定义
typedef struct
{
	u16 RD[BMS_CommConv_ReadMax];							
	u16 WD[BMS_CommConv_WriteMax];
	EQP_BMS_CommConvObj_StDef * p2Data;								//结构化指针
}EQP_BMS_CommConv_StDef;


//BMS 结构化通信内容定义
typedef struct
{
	s16 Voltage;											//电压 0.1v
	s16 Current;											//电流 0.1A,正为充电，负为放电
	u16 SOC;												//SOC 0.1 %
	u16 SOH;												//SOH	
	s16 HOT_Current;										//电池热电流 0.1A
	u16 ALARM;												//报警
	u16 BMS_Ready;											//准备状态	
	s16 MinCellVoltage;     								//最低单体电压 0.1v
  	s16 MaxCellVoltage;     								//最高单体电压 0.1
}EQP_BMS_MCBObj_StDef;

#define BMS_MCB_ReadMax											9//BMS读取数据最大长度,7+1+1
#define BMS_MCB_WriteMax											1//BMS写入数据最大长度
//BMS通信数据池 定义
typedef struct
{
	u16 RD[BMS_MCB_ReadMax];							
	u16 WD[BMS_MCB_WriteMax];
	EQP_BMS_MCBObj_StDef * p2Data;								//结构化指针
}EQP_BMS_MCB_StDef;

#define VoiceReadMax										4//语音播报读取数据最大长度
#define VoiceWriteMax										4//语音播报写入数据最大长度
//Voice通信数据池 定义
typedef struct
{
	u16 RD[VoiceReadMax];							
	u16 WD[VoiceWriteMax];
}EQP_Voice_StDef;

#define ADReadMax											8//模拟量采集读取数据最大长度
#define ADWriteMax											1//模拟量采集写入数据最大长度
//AD通信数据池 定义
typedef struct
{
	u16 RD[ADReadMax];							
	u16 WD[ADWriteMax];
}EQP_AD_StDef;

//磁导航传感器数据内容定义
typedef struct
{
	u16 Status;      										//传感器状态 占用16个DHR,每个传感器占用1个DHR,其余为预留 持续为0
	u16 Value;												//感应到的磁条值 有磁条为2
	s16 Distance;											//感应磁条与传感器中心的距离 -70mm----70mm (有效)   范围（-630---630）(面向车，磁条在左边输出正)
	s16 Intensity;											//感应到的磁条的磁场强度 磁条槽口向上，强度为负
}EQP_MagObj_StDef;
#define MagReadMax											4//磁导航读取数据最大长度
#define MagWriteMax											1//磁导航写入数据最大长度
//Mag通信数据池 定义
typedef struct
{
	u16 RD[MagReadMax];							
	u16 WD[MagWriteMax];
	EQP_MagObj_StDef* p2Data;								//磁导航数据内容指针
}EQP_Mag_StDef;

//RFID传感器数据内容定义
typedef struct
{
	u16 WriteState;											//RFID写状态，0为写入正常，1为写入失败，2为无卡											
	u16 ReadState;											//RFID读状态，0为无卡，1为正在读卡
	u16 Strong;												//读卡信息强度，0-7依次增强
	u16 Data1;												//rfid第一字节数据，用于存储RFID号
	u16 Data2;
}EQP_RFIDObj_StDef;
#define RFIDReadMax											5//RFID读取数据最大长度
#define RFIDWriteMax										1//RFID写入数据最大长度
//RFID通信数据池 定义
typedef struct
{
	u16 RD[RFIDReadMax];							
	u16 WD[RFIDWriteMax];
	EQP_RFIDObj_StDef* p2Data;								//数据内容指针
}EQP_RFID_StDef;

//RFID传感器数据内容定义
typedef struct
{
	u16 PGV_ReadComStatus;   					            //读取状态	
	u16 PGV_ErrorCode;   									//故障代码
	u16 PGV_Syscnt; 										//系统ms	
	u16 PGV_IdentifyStatus;  								//识别状态	
    u16 PGV_ControlCode;   									//二维码码值		
    s16 PGV_Xpositon;    									//二维码码x坐标	单位0.1mm
	s16 PGV_Ypositon;    									//二维码码y坐标	
	s16 PGV_Zpositon;    									//二维码码z坐标	单位0.01°

}EQP_HFPGVObj_StDef;

#define PGVReadMax											8//PGV读取数据最大长度
#define PGVWriteMax											1//PGV写入数据最大长度
//PGV通信数据池 定义
typedef struct
{
	u16 RD[PGVReadMax];							
	u16 WD[PGVWriteMax];
	EQP_HFPGVObj_StDef *p2Data;							//PGV数据内容指针
}EQP_PGV_StDef;


#include "SerialExp_Disperse.h"
typedef struct
{
	u16 RD[SerialExpReadMax];							
	u16 WD[SerialExpWriteMax];
	SerialExp_StDef 	p2Data;
}EQP_SerialExp_StDef;

typedef struct
{
	u16 RD[SerialExpReadMax_2];							
	u16 WD[SerialExpWriteMax_2];
	SerialExp_StDef 	p2Data;
}EQP_SerialExp_2_StDef;

#define CommConvReadMax										40//串口扩展板读取数据最大长度
#define CommConvWriteMax									1//串口扩展板写入数据最大长度
//通信转换板通信数据池 定义
typedef struct
{
	u16 RD[CommConvReadMax];							
	u16 WD[CommConvWriteMax];
	//EQP_CommConvObj_StDef* p2Data;							//通信转换板数据内容指针
}EQP_CommConv_StDef;


///兴颂雷达传感器数据内容定义
typedef struct
{
	//读
	uint16_t TangentRegion;	//寄存器1000.传感器当前通道值 范围：0~63
    uint16_t InputIO_1;		//寄存器1001.传感Out1 输出状态 我们使用常闭模式 00：检测到物体；01：未检测到物体
    uint16_t InputIO_2;		//寄存器1002.传感Out2 输出状态 我们使用常闭模式 00：检测到物体；01：未检测到物体
    uint16_t InputIO_3;		//寄存器1003.传感Out3 输出状态 我们使用常闭模式 00：检测到物体；01：未检测到物体
    uint16_t InputIO_Err;	//寄存器1004.传感器当前状态 输出状态 00：传感器正常状态；01-07：传感器故障状态；
	//写
	uint16_t WorkMode;		//寄存器2000.传感器工作模式 00：通道指定模式；01：智能通道选择模式
    uint16_t SetRegion;		//寄存器2001.通道指定模式下控制传感器通道值 范围：0~63号通道
}EQP_HinsonRadarObj_StDef;
#define Hinson_Radar_ReadMax								5/*兴颂雷达读取数据最大长度*/
#define Hinson_Radar_WriteMax								2/*兴颂雷达入数据最大长度*/
//兴颂雷达数据池 定义
typedef struct
{
	u16 RD[Hinson_Radar_ReadMax];							
	u16 WD[Hinson_Radar_WriteMax];
	EQP_HinsonRadarObj_StDef* p2Data;	//兴颂雷达数据内容指针
}EQP_HinsonRadar_StDef;


//亿佰特数据池 定义
typedef struct
{
	u16 RD[1];
	u16 WD[1];
}EQP_ME15_StDef;

//光斑传感器数据内容定义
typedef struct
{
	u16 Max0;
	u16 DataStart;
	u16 MsL;
	u16 MsH;
	u16 Max;
	u16 Avg;
	u16 MaxPoint;
	u16 ErrCnt;
	//u16 right_cnt;
}EQP_LaserSpotObj_StDef;

#define LaserSpotReadMax									8//光斑传感器
#define LaserSpotWriteMax									1
//光斑传感器通信数据池 定义
typedef struct
{
	u16 RD[LaserSpotReadMax];
	u16 WD[LaserSpotWriteMax];
	EQP_LaserSpotObj_StDef* p2Data;							
}EQP_LaserSpot_StDef;

//联动通信数据池 定义
typedef struct
{
	u16 RD[26];
	u16 WD[28];
	//EQP_AssemblyObj_StDef* p2Data;
}EQP_Assembly_StDef;

#define PLC_EstopReadMax									1//PLC 安全模块
#define PLC_EstopWriteMax									1
//PLC 安全模块数据内容定义
typedef struct
{
	uint16_t F_ObsRadarOuter	:1;	/*左前雷达 0:未触发	  1：外层障碍触发*/
	uint16_t F_ObsRadarMiddle	:1;	/*左前雷达 0:未触发	  1：中间层障碍触发*/
	uint16_t F_ObsRadarInner	:1;	/*左前雷达 0:未触发	  1：内层障碍触发*/
	uint16_t F_ObsRadarErr		:1;	/*左前雷达 0:未触发	  1：激光雷达异常触发*/


	uint16_t F_ObsBar			:1;	/*前触边 0:未触发	  1：触发*/
	uint16_t B_ObsBar			:1;	/*左触边*/
	uint16_t L_ObsBar			:1;	/*后触边*/
	uint16_t R_ObsBar			:1;	/*右触边*/
	
	uint16_t Estop				:4;	/*急停	0:未触发	  1：触发*/

	uint16_t B_ObsRadarOuter	:1;	/*右后雷达 0:未触发	  1：外层障碍触发*/
	uint16_t B_ObsRadarMiddle	:1;	/*右后雷达 0:未触发	  1：中间层障碍触发*/
	uint16_t B_ObsRadarInner	:1;	/*右后雷达 0:未触发	  1：内层障碍触发*/
	uint16_t B_ObsRadarErr		:1;	/*右后雷达 0:未触发	  1：激光雷达异常触发*/


}EQP_PLC_EstopObj_StDef;
//PLC 安全模块通信数据池 定义
typedef struct
{
	u16 RD[PLC_EstopReadMax];
	u16 WD[PLC_EstopWriteMax];
	EQP_PLC_EstopObj_StDef *p2Data;
}EQP_PLC_Estop_StDef;

#define PLCReadMax										6//PLC读取数据最大长度
#define PLCWriteMax										6//PLC写入数据最大长度
typedef struct
{
	u16 RD[PLCReadMax];
	u16 WD[PLCWriteMax];
}EQP_PLC_StDef;

typedef struct __attribute__((packed))
{
	u16 RedState;											// 读取状态
	u16 BeatCont;											// 心跳计数
	s16 Voltage;											// 电压 0.1v
	s16 Current;											// 电流 0.1A,正为充电，负为放电
	u16 SOC;												// SOC 0.1 %
	u16 SOH;												// SOH	
	s16 HOT_Current;										// 电池热电流 0.1A
	u16 Alarm_Code;											// 报警
	u16 BMS_Ready;											// 准备状态	
	s16 MinCellVoltage;     								// 最低单体电压 0.1v
	s16 MaxCellVoltage;     								// 最高单体电压 0.1
  	s16 Temperature_High;									// 最高温度
	s16 Temperature_Low;									// 最低温度
	u16 AlarmBit;											// 报警码
	u16 Bit_Infor_1;										// 电池位信息
	u16 P_IR;												// 正极绝缘电阻
	u16 N_IR;												// 负极绝缘电阻
	u16 Max_VCell_Diff;										// 电池阻最大压差
	u16 Max_T_Diff;											// 电池组最大温差
	u16 Max_VCell_Idx;										// 最高单体电压逻辑编号
	u16 Min_VCell_Idx;										// 最低单体电压逻辑编号
	u16 Max_Temp_Idx;										// 最高温度逻辑编号
	u16 Min_Temp_Idx;										// 最低温度逻辑编号
	u16 Relay_Sta_1;										// 继电器状态_DHR1
	u16 Relay_Sta_2;										// 继电器状态_DHR2
	u16 Relay_Sta_3;										// 继电器状态_DHR3
	u16 DCHG_DI_1;											// 放电诊断信息1
	u16 DCHG_DI_2;											// 放电诊断信息2
	u16 CHG_DI_1;											// 充电诊断信息1
	u16 CHG_DI_2;											// 充电诊断信息2
	u16 GB_Fault_Code;										// 国标通信故障代码
	u16 V_CHG;												// 充电机输出电压
	u16 I_CHG;												// 充电机输出电流
	u16 Time_Charged;										// 已充电时间
	u16 CHG_485_INFO;										// 485-充电信息
	u16 CHG_CAN_INFO;										// Can-充电信息
	u16 TCC;												// 充电累计容量
	u16 TDC;												// 放电累计容量
	u16 C_TOT;												// 电池组总容量
	u16 C_REM;												// 电池组剩余容量
	u16 ACC_T_DCHG;											// 累计总放电时间
	u16 ACC_T_CHG;											// 累计总充电时间
	u16 T_OP_TOT;											// 累计总使用时间
	u16 N_CYCLES;											// 累计充放电次数
	u16 Alarm_In_CATL;                                      //宁德时代内部报警
	u16 Alarm_In_Bit1;										//
	u16 Alarm_In_Bit2;										//占位
	u16 Alarm_In_Bit3;										//占位
	u16 Alarm_In_Bit4;										//占位 										

}EQP_BMS_CATLObj_StDef;

#define BMS_CATL_ReadMax											58//BMS读取数据最大长度,7+1+1
#define BMS_CATL_WriteMax											1//BMS写入数据最大长度
//BMS通信数据池 定义
typedef struct
{
	u16 RD[BMS_CATL_ReadMax];							
	u16 WD[BMS_CATL_WriteMax];
	EQP_BMS_CATLObj_StDef * p2Data;								//结构化指针
}EQP_BMS_CATL_StDef;

#define DAReadMax											1//模拟量输出读取数据最大长度
#define DAWriteMax											8//模拟量输出写入数据最大长度
//AD通信数据池 定义
typedef struct
{
	u16 RD[ADReadMax];							
	u16 WD[ADWriteMax];
}EQP_DA_StDef;

//安川m3板卡相关内容 注意这里读写的数量，可以根据实际轴数进行调整，以便加快串口的通讯速度
#define ANCReadMax											100//板卡故障*2+轴故障标准*2+速度反馈*32+力矩反馈*32+故障码2*16
#define ANCWriteMax											36//使能*2+启动*2+速度命令*32
//读数据结构化定义
typedef struct
{
	u32 CPUError; 											//全局错误标志
	u32 DrvError; 											//轴错误标志
	s16 ActiveSpeed[32];									//速度反馈 0.1rpm
	s16 Torque[32];											//力矩反馈 千分比
	u32 ErrorCode[32];										//轴故障代码
}EQP_ANC_RObj_StDef;
//写数据结构化定义
typedef struct
{
	u32 En; 												//轴使能命令
	u32 Start; 												//轴速度执行命令
	s16 VCmd[32]; 											//轴速度命令 0.1prm
}EQP_ANC_WObj_StDef;
typedef struct
{
	u16 RD[ANCReadMax];							
	u16 WD[ANCWriteMax];
	EQP_ANC_RObj_StDef * p2RData;							//读数据结构化指针
	EQP_ANC_WObj_StDef * p2WData;							//写数据结构化指针
}EQP_ANC_StDef;

//13个char 7个u16来装载
#define DBSETReadMax										7//
#define DBSETWriteMax										7//
//读数据结构化定义 DBSET 01 MODE 11
//注意是字符串 需要内存紧凑对齐 默认就是char对齐的
typedef struct
{
	char Cmd[5];											//命令前缀
	char Id[2];												//目标设备编号
	char Type[4];											//参数类型
	char Val[2];											//参数值
}EQP_DBSET_RObj_StDef;
//写数据结构化定义
typedef struct
{
	char Cmd[5];											//命令前缀
	char Id[2];												//目标设备编号
	char Type[4];											//参数类型
	char Val[2];											//参数值
}EQP_DBSET_WObj_StDef;
typedef struct
{
	u16 RD[DBSETReadMax];							
	u16 WD[DBSETWriteMax];
	EQP_DBSET_RObj_StDef * p2RData;							//读数据结构化指针
	EQP_DBSET_WObj_StDef * p2WData;							//写数据结构化指针
}EQP_DBSET_StDef;

#define API_Master_ReadMax	10//二次开发 输出读取数据最大长度
#define API_Master_WriteMax	20//二次开发 输出写入数据最大长度
//AD通信数据池 定义
typedef struct
{
	u16 RD[API_Master_ReadMax];							
	u16 WD[API_Master_WriteMax];
}EQP_API_Master_StDef;

CommConf_EXT CCFEQP_Masters_StDef			EQPMsts;						//做主站对象
CommConf_EXT CCFEQP_Slaves_StDef			EQPSlvs;						//做从站对象

CommConf_EXT EQP_HMI_StDef 					EQP_HMI 					;	//触摸屏数据池
CommConf_EXT EQP_DSP_StDef 					EQP_DSP 					;	//调度系统数据池
CommConf_EXT EQP_Assembly_Slave_StDef		EQP_Assembly_Slave			;	//调度系统数据池
CommConf_EXT EQP_API_Slave_StDef			EQP_API_Slave				;	//二次开发数据池

CommConf_EXT EQP_COMMTEST_StDef 			EQP_COMMTEST;					//通信测试
CommConf_EXT EQP_WirelessHandle_StDef 		EQP_WHA;						//遥控器A数据池
CommConf_EXT EQP_WirelessHandle_StDef 		EQP_WHB;						//遥控器B数据池
CommConf_EXT EQP_BMS_CommConv_StDef			EQP_BMS_CommConv;				//卡式转接板 BMS数据池
CommConf_EXT EQP_BMS_MCB_StDef				EQP_BMS_MCB;					//直连 BMS数据池
CommConf_EXT EQP_Voice_StDef 				EQP_Voice;						//语音播报数据池
CommConf_EXT EQP_AD_StDef 					EQP_AD;							//模拟量采集0数据池
CommConf_EXT EQP_AD_StDef 					EQP_AD1;						//模拟量采集1数据池
CommConf_EXT EQP_AD_StDef 					EQP_Thermodetector;				//温湿度检测模块数据池
CommConf_EXT EQP_Inclinometer_Rion_StDef 	EQP_Inclinometer_Rion;			//倾角传感器检测模块数据池
CommConf_EXT EQP_Mag_StDef 					EQP_MagF;						//磁导航前数据池
CommConf_EXT EQP_Mag_StDef 					EQP_MagB;						//磁导航后数据池
CommConf_EXT EQP_Mag_StDef 					EQP_MagL;						//磁导航左数据池
CommConf_EXT EQP_Mag_StDef 					EQP_MagR;						//磁导航右数据池
CommConf_EXT EQP_RFID_StDef 				EQP_RFID;						//朝下RFID数据池
CommConf_EXT EQP_RFID_StDef 				EQP_RFID_Up;					//朝上RFID数据池
CommConf_EXT EQP_PGV_StDef 					EQP_PGVUpward;					//向上PGV数据池
CommConf_EXT EQP_PGV_StDef					EQP_PGVDownward;				//向下PGV数据池
CommConf_EXT EQP_PGV_StDef 					EQP_PGVUpward_F;				//向上前PGV数据池
CommConf_EXT EQP_PGV_StDef 					EQP_PGVUpward_B;				//向上后PGV数据池
CommConf_EXT EQP_PGV_StDef 					EQP_PGVDownward_F;				//向下前PGV数据池
CommConf_EXT EQP_PGV_StDef 					EQP_PGVDownward_B;				//向下后PGV数据池
CommConf_EXT EQP_SerialExp_StDef 			EQP_SerialExp;					//串口扩展板数据池
CommConf_EXT EQP_SerialExp_2_StDef 			EQP_SerialExp_2;				//第二串口扩展板数据池
CommConf_EXT EQP_CommConv_StDef 			EQP_CommConv;					//通信转换板数据池
CommConf_EXT EQP_CommConv_StDef 			EQP_CommConv2;					//通信转换板数据池——分离的磁滞传感器数据
CommConf_EXT EQP_IPC_StDef 					EQP_IPC;						//工控机通信数据池
CommConf_EXT EQP_Roll_Controller_StDef 		EQP_Roll_Controller;			//辊筒控制板通信数据池
CommConf_EXT EQP_LaserSpot_StDef 			EQP_LaserSpot;					//光斑传感器数据池
CommConf_EXT EQP_Assembly_StDef 			EQP_Assembly_Master;			//联动通信数据池

CommConf_EXT EQP_HinsonRadar_StDef 			EQP_Hinson_Radar_Front;			//兴颂modbus通讯的前雷达
CommConf_EXT EQP_HinsonRadar_StDef 			EQP_Hinson_Radar_Rear;			//兴颂modbus通讯的后雷达
CommConf_EXT EQP_HinsonRadar_StDef 			EQP_Hinson_Radar_Left;			//兴颂modbus通讯的左雷达
CommConf_EXT EQP_HinsonRadar_StDef 			EQP_Hinson_Radar_Right;			//兴颂modbus通讯的右雷达
CommConf_EXT EQP_HinsonRadar_StDef 			EQP_Hinson_Radar_FrontUp;		//兴颂modbus通讯的前斜面雷达
CommConf_EXT EQP_HinsonRadar_StDef 			EQP_Hinson_Radar_RearUp;		//兴颂modbus通讯的后斜面雷达

CommConf_EXT EQP_ME15_StDef					EQP_ME15_Near;					/*亿佰特 近端模块*/
CommConf_EXT EQP_ME15_StDef					EQP_ME15_Far;					/*亿佰特 远端模块*/

CommConf_EXT EQP_PLC_Estop_StDef			EQP_PLC_Estop;					//PLC 安全模块（急停、雷达、触边）数据池

CommConf_EXT EQP_DA_StDef 					EQP_DA;							//模拟量输出模块 数据池
CommConf_EXT EQP_API_Master_StDef			EQP_API_Master;					//二次开发 主机通讯

#include "drv_KxdBMS.h"
CommConf_EXT EQP_KXD_BMS_StDef				EQP_KXD_BMS;					//凯信达 BMS数据池

#include "drv_BjfPGV.h"
CommConf_EXT EQP_BJF_PGV_StDef				EQP_BJF_PGV_Upward;				//倍加福 PGV
CommConf_EXT EQP_BJF_PGV_StDef				EQP_BJF_PGV_Downward;	
CommConf_EXT EQP_BJF_PGV_StDef				EQP_BJF_PGV_Upward_F;	
CommConf_EXT EQP_BJF_PGV_StDef				EQP_BJF_PGV_Upward_B;	
CommConf_EXT EQP_BJF_PGV_StDef				EQP_BJF_PGV_Downward_F;
CommConf_EXT EQP_BJF_PGV_StDef				EQP_BJF_PGV_Downward_B;

CommConf_EXT EQP_BMS_CATL_StDef				EQP_BMS_CATL;

//安川m3板卡
CommConf_EXT EQP_ANC_StDef					EQP_ANC;
//多倍通配置切换
CommConf_EXT EQP_DBSET_StDef				EQP_DBSET;

//PLC控制面板灯
CommConf_EXT EQP_PLC_StDef					EQP_PLC;

//自定义引用
EQPComm_Init_EXT void EQPComm_Init(void);
EQPComm_Init_EXT void Para_EQPInit(Para_EQPCommSource_EnumDef Id,CCFID_EnumDef Cn,uint8_t Address);
//方法外部声明


#endif

//***********************************END OF FILE***********************************


