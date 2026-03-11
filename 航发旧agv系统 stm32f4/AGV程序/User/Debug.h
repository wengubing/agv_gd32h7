/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Debug.h
** 创 建 人: 文小兵  
** 描    述: 调试文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月28日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __Debug_h__
#define __Debug_h__

//确保仅在该文件编译时实例化一次对象
#ifdef Debug_GLOBALS
    #define Debug_EXT
#else
    #define Debug_EXT  extern
#endif

//自定义引用	

#include "ACC_Project.h"
#include "ModbusRtuPoll.h"
#include "drv_path.h"
#include "app_DependenceAGV.h"
#include "app_SkeletonAGV.h"
#include "HMI.h"

//代码逻辑异常位定义
typedef union
{
	struct
	{
		u16 MoveBrake_OnErr : 1;      						//抱闸打开异常，即行走伺服未使能时打开
		u16 MoveBrake_OffErr : 1;      						//抱闸关闭异常，即行走伺服有速度命令的时候，抱闸未打开
		u16 nc : 14;      									//保留
	}Bit;
	u16 All;
}LogicErr_StDef;

//代码不符合预期状态检出对象定义
typedef struct
{
	//系统拓展故障对象定义
	LogicErr_StDef LogicErr;								//逻辑错误
	bool Res;												//逻辑错误检测结果
}Debug_LogicErr_StDef;

//调试自动运行 任务控制命令定义
typedef enum 
{
    Auto_Null=0,											//无效
	Auto_Init,												//初始化
	Auto_Change2Auto,										//切换自动模式
	Auto_Load,												//载入
	Auto_Down,												//下载
	Auto_Start,												//开始
	Auto_Pause,												//暂停
	Auto_Stop,												//停止
	Auto_Cancel,											//取消
	Auto_ExitAuto,											//退出自动
}AutoCtrl_EnumDef;

//自动运行仿真调试
typedef struct
{
    bool TestRunOnce;										//测试模式运行一次
	bool Change2Auto;										//切换自动模式:初始化磁条、rfid、虚拟里程状态，切换到自动模式
	bool Load;												//载入测试路径
	bool Down;												//下载测试路径

	bool OffAllErr;											//清除所有错误 危险，禁止操作！
	u32 Password;											//危险操作口令
	struct
	{
		u32 AsbInputOff : 1;								//关闭联动输入调用
		u32 AsbErrOff : 1;									//关闭联动报警
		u32 RmcUpdataOff : 1;								//关闭遥控器更新 方便调试
		u32 ObsIO_InOff : 1;								//关闭雷达模块IO输入 方便调试
		u32 AsbCommOff : 1;									//关闭联动通信更新 方便单车上调试联动功能
		u32 AsbSensorInOff : 1;								//关闭联动传感器输入更新
		u32 AsbOff : 1;										//跳过联动模块调用 直接单车控制
		u32 Skip_EqpIDSet : 1;								//跳过启动代码中 EqpIDSet
		u32 Skip_EEP2Sys : 1;								//跳过启动代码中 EEP2Sys
		u32 CrashBar_InOff : 1;								//仿真时能模拟控制触边报警
		u32 But1 : 1;										//
		u32 But2 : 1;										//
		u32 But3 : 1;										//
		u32 But4 : 1;										//
		//u32 AsbOutputOff : 1;								//关闭联动输出调用
		//u32 nc : 14;      									//保留
	}Bit;

	//强制轮系坐标平移
	struct
	{
		s32 X;												//
		s32 Y;												
	}Mcc;
	
	AutoCtrl_EnumDef Ctrl;									//任务控制命令
}ACC_AutoDebug_StDef;

//串口数组结构体对象定义
typedef struct
{
	ST485_USART RS485_USARTx[USARTMAXCNT];
}USARTStr_StDef;

//轮系角度数组结构体对象定义
typedef struct
{
	AngleCal_StDef g_WheelAngle[WheelAngleNum_MAX];
}AngleStr_StDef;

//伺服数组结构体对象定义
typedef struct
{
	SVO_DATA_TypeDef SVO[SVO_MAXNUM];
}SVOStr_StDef;

//磁导航数组结构体对象定义
typedef struct
{
	MN_TypeDef Mag[4];
}MNStr_StDef;

//CBB对象汇总
typedef struct
{
	RFID_TypeDef *rfid;//RFID
	MNStr_StDef *mag;//磁导航
	LiftCtrl_TypeDef *lift;//升降台
	WH_TypeDef *whA;//遥控器

	MotorResolving_TypeDef *motor;//舵轮解算
	Mecanum_TypeDef *mecanum;//麦轮解算
	MagneticTrace_TypeDef *magTrace;//巡线
	SoundLightNotice_TypeDef *sln;//声光提示
	ROA_TypeDef *roa;//避障
	LoadDetection_TypeDef *load;//负载监测
	ShelfAutadj_TypeDef *shelf;//自动对中
	DataSmoothing_TypeDef *smoothVx;//数据平滑
	DataSmoothing_TypeDef *smoothVy;//数据平滑
	DataSmoothing_TypeDef *smoothVz;//数据平滑
	DataSmoothing_TypeDef *smoothAx;//数据平滑
	DataSmoothing_TypeDef *smoothAy;//数据平滑
	DataSmoothing_TypeDef *smoothAz;//数据平滑
	DataSmoothingStr_TypeDef *smoothVxyz;//关联平滑数组

	_MagneticVirtual_TypeDef *magVir;//磁导航虚拟
	_MagneticVirtual_TypeDef *magVirForPgv;//磁导航虚拟2
	OdometerData_t *odometer;//里程计

	HostCtrl_DATA *hostCtrl;//自动数据
    CenterPointCal_StDef *CPC;//轮系跟随巡线 中心点计算
	//...
}GOC_CBB_StDef;

//串口对象汇总
typedef struct
{
	//ST485_USART *USARTx;							//底层串口对象
	USARTStr_StDef *USARTx;							//底层串口对象
	CCFs_StDef *CCFs;								//物理接口通信对象
	CCFEQP_Masters_StDef *EQPMsts;					//做主站对象
	CCFEQP_Slaves_StDef *EQPSlvs;					//做从站对象
	MB_MANGER_APP *MBMst;							//modbus主站对象
	EQP_HMI_StDef *EQP_HMI;							//触摸屏数据池
	EQP_DSP_StDef *EQP_DSP;							//调度系统数据池
	EQP_COMMTEST_StDef* EQP_COMMTEST;				//通信测试对象
	EQP_WirelessHandle_StDef *EQP_WHA;				//遥控器A数据池
	EQP_WirelessHandle_StDef *EQP_WHB;				//遥控器B数据池
	EQP_BMS_CommConv_StDef *EQP_BMS_CommConv;							//BMS数据池
	EQP_Voice_StDef *EQP_Voice;						//语音播报数据池
	EQP_AD_StDef *EQP_AD;							//模拟量采集0数据池
	EQP_AD_StDef *EQP_AD1;							//模拟量采集1数据池
	EQP_AD_StDef *EQP_Thermodetector;				//温湿度检测模块数据池
	EQP_Inclinometer_Rion_StDef *EQP_Inclinometer_Rion;//倾角传感器检测模块数据池
	EQP_Mag_StDef *EQP_MagF;						//磁导航前数据池
	EQP_Mag_StDef *EQP_MagB;						//磁导航后数据池
	EQP_Mag_StDef *EQP_MagL;						//磁导航左数据池
	EQP_Mag_StDef *EQP_MagR;						//磁导航右数据池
	EQP_RFID_StDef *EQP_RFID;						//RFID数据池
	EQP_PGV_StDef *EQP_PGVUpward;					//向上PGV数据池
	EQP_PGV_StDef *EQP_PGVDownward;					//向下PGV数据池
	EQP_PGV_StDef *EQP_PGVUpward_F;					//向上前PGV数据池
	EQP_PGV_StDef *EQP_PGVUpward_B;					//向上后PGV数据池
	EQP_PGV_StDef *EQP_PGVDownward_F;				//向下前PGV数据池
	EQP_PGV_StDef *EQP_PGVDownward_B;				//向下后PGV数据池
	EQP_SerialExp_StDef *EQP_SerialExp;				//串口扩展板数据池
	EQP_CommConv_StDef *EQP_CommConv;				//通信转换板数据池
	EQP_CommConv_StDef *EQP_CommConv2;				//通信转换板数据池——存在 磁滞和角度分离使用两个卡式转接板的需求
	EQP_IPC_StDef *EQP_IPC;							//工控机通信数据池
	EQP_Roll_Controller_StDef *EQP_Roll_Controller;	//辊筒控制板通信数据池

	EQP_LaserSpot_StDef *EQP_LaserSpot;				//光斑传感器
	EQP_Assembly_StDef *EQP_Assembly_Master;				//联动主机通信
	EQP_HinsonRadar_StDef *EQP_Hinson_Radar_Front;			//兴颂modbus通讯的前雷达
	EQP_HinsonRadar_StDef *EQP_Hinson_Radar_Rear;			//兴颂modbus通讯的后雷达
	EQP_HinsonRadar_StDef *EQP_Hinson_Radar_Left;			//兴颂modbus通讯的左雷达
	EQP_HinsonRadar_StDef *EQP_Hinson_Radar_Right;			//兴颂modbus通讯的右雷达
	EQP_HinsonRadar_StDef *EQP_Hinson_Radar_FrontUp;		//兴颂modbus通讯的前斜面雷达
	EQP_HinsonRadar_StDef *EQP_Hinson_Radar_RearUp;			//兴颂modbus通讯的后斜面雷达
	
	
	EQP_ME15_StDef	*EQP_ME15_Near;					/*亿佰特 近端模块*/
	EQP_ME15_StDef	*EQP_ME15_Far;					/*亿佰特 远端模块*/

	//...
}GOC_UART_StDef;

//CAN对象汇总
typedef struct
{
	STM32_CANi *STM32_CAN1;//CAN1
	CO_Data *CANOD;//CANopen对象
	LinkCanOpen *can_master;//CANopen链路
	//...
}GOC_CAN_StDef;

//IO相关对象指针汇总
typedef struct
{
	IO_Inputs_TypeDef *inpus;								//输入IO组
	IO_Outputs_TypeDef *outputs;							//输出IO组
	ACC_IOCN2DHRBit_TypeDef *io2dhrbit;						//IO2DHRBit
	//... 
}GOC_IO_StDef;

//ADDA相关对象汇总
typedef struct
{
	ADDA_ADCNs_StDef *ADCNs;								//ADDA_ADCNs
	ADDA_ADFucs_StDef *ADFucs;								//ADDA_ADFucs
	ADDA_DACNs_StDef *DACNs;								//ADDA_DACNs
	ADDA_DAFucs_StDef *DAFucs;								//ADDA_DAFucs
	//...
}GOC_ADDA_StDef;

//参数相关对象汇总
typedef struct
{
	ACC_Para_TypeDef *para;									//系统参数
	EEP_Para_TypeDef *eep;									//EEPROM参数
	EEP_Ctrl_TypeDef *eepctrl;								//EEPROM控制接口
	//...
}GOC_PARA_StDef;

//调试相关对象汇总
typedef struct
{
	ACC_DbugRtx_TypeDef *dbugrtx;							//RTX调试
	ACC_Faults_TypeDef *faults;								//故障
	ACC_CodeErr_UnionDef *codeerr;							//代码错误
	ACC_AutoDebug_StDef *AtDbug;							//自动调试
	Debug_LogicErr_StDef* CodeLogicErr;						//逻辑错误
}GOC_DBUG_StDef;

//其它对象汇总
typedef struct
{
	MsRecord_StDef *p2MsRecord;                             //记录
	ACC_DATA_TypeDef *Acc;                                  //Acc
	ACC_PowerOn_TypeDef *poweron;                           //上电启动
	ACCCCSwitch_TypeDef *channel;                           //控制源
//	ACC_BMS_TypeDef *bms;                                   //BMS
//	ACC_PGV_TypeDef *pgvUpward;                             //PGVUpward
//	ACC_PGV_TypeDef *pgvDownward;                           //PGVDownward
	AngleStr_StDef	*angle;                                 // AngleCal_StDef数组类型指针
	HardWareInfo_StDef *hwinfo;                             //硬件信息
	SVOStr_StDef *svo;                                      //伺服
	SVOs_StDef *SVOs;                                       //伺服分析统计
	FanCtrl_StDef *fan;                                     //风扇控制
	VoicePlayer_TypeDef *Voice;                             //语音播报
}GOC_ACC_StDef;

//自动部分对象汇总
typedef struct
{
	//path相关对象
	PathType_Para *PathTypePara;//路径参数
	PathTypeDef *p2PathStack;//路径栈
	struct PathTypeDefStr{PathTypeDef PathStack[PathQueueNum];} *p2PathStation;//路径站

	//AGV框架对象
	HostCtrl_DATA *g_HostData;//主控数据
	uint16_t *p2AGVInf;//AGV信息
	DSPCOMMOBJ_StDef *p2CommObj;//结构化AGV信息

	//AGV依赖对象
	DependenceAGV_InputTypeDef *g_AGVDependenCBB;//AGV依赖对象

	//GyroA0030S_TypeDef *g_GyroA0030SData;//陀螺仪数据

	//自动对象关键接口摘要
	struct KeyStateStDef
	{
		Control_MODEL *p2ControlMODEL;
		Exe_PathState *p2ExecutionPathState;
		AGV_AutoMode *p2AGVAutoMode;
		AGVWorkMode_TypeDef *p2TargetAGVWorkMode;
		struct HxyzStDef {float Hxyz[3];} *p2Hxyz;
		uint8_t *p2CheckSucc;//AGV依赖初始化成功标志
	} KeyState;
	
}GOC_AUTO_StDef;

//HMI对象汇总
typedef struct
{
	struct HMI_Buf_StDef {u16 HMI_Buf[HMI_Cache_Num];} *p2HMI_Buf;//HMI缓存
    ExternalHmi_StDef* p2ExtHmi;
}GOC_HMI_StDef;

//其它对象汇总
typedef struct
{
	AgvSimulator_TypeDef* p2ASM;							//AGV模拟器
	Acc_AssemlySensorData_TypeDef* p2ASD;					//g_AssemlySensorData
}GOC_Others;

//全局对象汇总
typedef struct
{
	vu32 *Sysms;											//系统ms
	ACC_SysApi_BitDef *sysapi;								//系统API
	MC_WorkStep_TypeDef *workstep;							//工作步骤
	GOC_CBB_StDef cbb;										//CBB
	GOC_UART_StDef uart;									//串口
	GOC_CAN_StDef can;										//can
	GOC_IO_StDef io;										//io
	GOC_ADDA_StDef adda;									//ADDA
	GOC_PARA_StDef para;									//参数
	GOC_DBUG_StDef Dbug;									//调试
	GOC_ACC_StDef Acc;										//sys
	GOC_AUTO_StDef Auto;									//自动
	GOC_HMI_StDef HMI;										//HMI
    AsbCtrl_TypeDef *Asb;								    //联动控制
    GOC_Others Others;										//其它对象
}GOC_StDef;

//对象外部声明
Debug_EXT GOC_StDef goc;              						//系统全局对象汇总
Debug_EXT ACC_AutoDebug_StDef AtDbug;						//自动运行仿真调试
Debug_EXT Debug_LogicErr_StDef CodeLogicErr;				//逻辑错误对象

//方法外部声明
Debug_EXT bool GocInit(void);								//初始化
Debug_EXT bool DevDbugOperation(void);						//设备调试操作
Debug_EXT bool LogicErrDetect(void);						//逻辑错误检测
Debug_EXT void ITM_Init(void);								//初始化ITM

#endif

//***********************************END OF FILE***********************************


