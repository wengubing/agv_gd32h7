/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ACC_Project.h
** 创 建 人: 文小兵  
** 描    述: 标准设备程序顶层调用
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年4月25日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __ACC_Project_h__
#define __ACC_Project_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  ACC_PROJECT_GLOBALS
	#define ACC_PROJECT_EXT
	#define ACC_PROJECT_INIT  ={0}
#else
	#define ACC_PROJECT_EXT  extern
	#define ACC_PROJECT_INIT
#endif

//自定义引用	
#include "AgvChassisController.h"							//底盘控制器
#include "drv_ee_para.h"									//EEPROM参数
#include "drv_IODef.h"
#include "LinkCanopenMaster.h"
#include "LinearCal.h"
#include "CommConf.h"
#include "EmgJudge.h"
#include "ObjConf.h"
#include "CBB_Init.h"
#include "drv_can.h"
//#include "objconf.h"
#include "ADDA.h"
#include "app_SkeletonAGV.h"
#include "drv_Servo.h"
#include "app_FanCtrl.h"
#include "PowerOn.h"
#include "IOConf.h"	
#include "Para_EEPROM.h"
#include "Para_Sys.h"
#include "CtrlSourceSwitch.h"
#include "Patch.h"
#include "app_Assembly.h"
#include "drv_MergeToPGV.h"
#include "AgvSimulator.h"
#include "ACC_LeadShineCtrl.h"

//宏定义
//ACC任务调用间隔 ms
#define ACC_TASK_INTERVAL_MS								3//20250527在超低车上发现自转结算导致平滑处理异常 改为3解决

//系统BMS 数据对象定义
typedef struct
{
	f32 Voltage;											//电压 v
	f32 Current;											//电流 A,正为充电，负为放电
	f32 SOC;												//SOC %
	f32 SOH;												//SOH	
	f32 HOT_Current;										//电池热电流 A
	u16 ALARM;												//报警
	u16 BMS_Ready;											//准备状态	
	f32 MinCellVoltage;     								//最低单体电压 
  	f32 MaxCellVoltage;     								//最高单体电压

	unsigned short CommOkCnt;								//通信成功次数
	unsigned short TimeOutMs;								//超时计数
}ACC_BMS_TypeDef;



//任务信息 数据对象定义
typedef struct
{
	u32 RunCnt;												//运行次数
	u32 RunStart;											//运行开始	
	u32 RunEnd;												//运行结束
	u32 RunTime;											//运行时间
	u32 RunDealy;											//运行延时
	bool Busy;												//任务忙碌中不接收信号量，防止RTX看门狗触发		
}ACC_TskInf_TypeDef;

//RTX调试 数据对象定义
typedef struct
{
	u32 SysTickCnt;											//系统时钟
	f32 k2Ms;												//ms系数
	u16 WatchDogCnt;										//看门狗喂狗超时计数
	ACC_TskInf_TypeDef Acc;									//Acc任务信息
	ACC_TskInf_TypeDef MBMst;								//modbus主站任务信息
	ACC_TskInf_TypeDef MBSlv;								//modbus从站任务信息
	ACC_TskInf_TypeDef CanopenRx;							//canopen任务信息
	ACC_TskInf_TypeDef CanopenTx;							//canopen任务信息
}ACC_DbugRtx_TypeDef;

//关联平滑数组 数据对象定义
typedef struct
{
	DataSmoothing_TypeDef g_SmoothVxyz[3];
}DataSmoothingStr_TypeDef;

//Slam重定位数据结构体
#define 	LayerMaxNum		2
typedef struct
{
	/*用于匹配的tag数据*/
	uint16_t	Input_Tag;
	/*重定位数据列表*/
	struct
	{
		uint16_t	Tag		[LayerMaxNum];	//匹配的Tag
		int32_t		X		[LayerMaxNum];	//重定位的X值 单位 0.1mm
		int32_t		Y		[LayerMaxNum];	//重定位的Y值 单位 0.1mm
		int16_t		Z		[LayerMaxNum];	//重定位的Z值 单位 0.01°0-36000
		int16_t		LayerNum[LayerMaxNum];	//重定位的图层
	} Location;

}SlamLayerLocation_StDef;

//主动充电
typedef struct
{
	uint8_t Step;
	uint8_t RebootFlag;
	uint16_t Tag;
	uint16_t Odo_Z;
}ACC_ActiveCharge_TypeDef;

// 枚举标记当前电动缸执行状态
typedef enum {
    TASK_IDLE = 0,    // 空闲状态
    TASK_EXTEND,      // 执行伸出任务
    TASK_RETRACT      // 执行收回任务
} Task_StateTypeDef;

typedef struct
{
	uint16_t 	Extend_Delay;
	uint16_t 	Retend_Delay;

	Task_StateTypeDef CurrentTask;    // 当前正在执行
	Task_StateTypeDef PendingTask;    // 等待执行
	uint32_t 	TaskStartTime;		  // 执行中计数
	uint8_t 	Input_Extend	: 1;
	uint8_t 	Input_Retend	: 1;
	uint8_t 	OutValue_Extend	: 1;
	uint8_t 	OutValue_Retend	: 1;
} GroundCharge_CylinderControl_TypeDef;

//EEPROM控制接口对象定义
typedef struct __attribute__((packed))
{
    unsigned short EEP2EEPObj : 1;							//0EEPROM 读取到EEP对象
    unsigned short EEPObj2Sys : 1;							//1EEP对象 写入到系统中

    unsigned short Prj2Sys : 1;								//2项目编译参数 导入到系统中
    unsigned short Sys2EEObj : 1;							//3系统 写入到EEP对象
    unsigned short EEPObj2EEP : 1;							//4EEPROM对象 写入到EEPROM中

	unsigned short Sys2EEP : 1;								//5系统对象 写入到EEPROM中
	unsigned short nc : 10;									//6-15 强行对齐 必须保留
}EEP_Ctrl_TypeDef;

//系统功能API
typedef struct
{
	//系统操作
	unsigned int EStop : 1;									//急停
	unsigned int CloseAllOutput : 1;						//关闭所有输出
	unsigned int SysReset : 1;								//系统复位
	unsigned int SysReboot : 1;								//系统重启
	unsigned int ClearAllErr : 1;							//清除所有故障 仅操作一次
	
	//标定操作
	unsigned int LiftHighSet : 1;							//标定高点
	unsigned int LiftLowSet : 1;							//标定低点
	unsigned int WheelAngleZeroSet : 1;						//标定轮系角度零点

	//强制操作
	unsigned int ClearObsBarLatch : 1;						//清除防撞触边故障锁存
	unsigned int IgrWlAngLimit : 1;							//忽略轮系角度限制报警
	unsigned int LeadShineForce : 1;						//强制雷赛回零停止操作
}ACC_SysApi_BitDef;

//超声波对中标定对象
typedef struct
{
	u16 Num;												//标定组数 当此对象设0时关闭标定，采用原本直接比例换算
	LinearCal_TypeDef Data[NUMMAX * 2];						//数据对象数组 组大数量为自动对中对象单侧*2
	s16 OffSet[NUMMAX * 2];									//偏移量
}ShelfAutadj_Para_TypeDef;

//全局方法声明
ACC_PROJECT_EXT void Tsk_Acc(void);                         //ACC主任务
ACC_PROJECT_EXT bool Walk_MotorResolving(void);				//行走电机解析
ACC_PROJECT_EXT bool Push_Roller_Ctrl(void);				//推拉机构控制
ACC_PROJECT_EXT bool Lift_Ctrl(void);						//升降控制
ACC_PROJECT_EXT void SystemReset(void);						//系统复位
ACC_PROJECT_EXT bool ACC_MsTsk(void);						//ACC毫秒任务
ACC_PROJECT_EXT bool AsbSwitchEnterInit(void);				//单联动切换初始化

//全局对象声明
ACC_PROJECT_EXT MC_WorkStep_TypeDef g_MC_WorkStep;			//底盘控制步骤

ACC_PROJECT_EXT ACC_PowerOn_TypeDef g_PowerOn;				//上电启动控制对象
ACC_PROJECT_EXT ACCCCSwitch_TypeDef g_ChannelSwitch;		//控制源切换对象
ACC_PROJECT_EXT ACC_Para_TypeDef g_Para;					//系统参数对象


ACC_PROJECT_EXT ACC_DbugRtx_TypeDef g_DbugRtx;				//RTX调试对象
ACC_PROJECT_EXT ACC_IOCN2DHRBit_TypeDef g_IOCN2DHRBit;		//IO2DHRBit对象
ACC_PROJECT_EXT EEP_Ctrl_TypeDef EEPCtrl;					//EEPROM控制接口
//ACC_PROJECT_EXT ACC_BMS_TypeDef g_BMS;						//系统BMS对象
ACC_PROJECT_EXT FanCtrl_StDef g_Fan;                    	//风扇控制对象
ACC_PROJECT_EXT ACC_SysApi_BitDef g_SysApi;                	//系统功能API
ACC_PROJECT_EXT WalkLimit_TypeDef g_WalkLimit;              //行走加速度限制
ACC_PROJECT_EXT GearLimit_Single_TypeDef g_WalkGearLimit;   //行走挡位限制
ACC_PROJECT_EXT GearLimit_Single_TypeDef g_WalkGearLimit_Hg;   //行走挡位限制 根据高度
ACC_PROJECT_EXT GearLimit_Single_TypeDef g_LiftGearLimit;   //升降挡位限制
ACC_PROJECT_EXT MR_ManualCtrl_TypeDef g_MR_ManualCtrl;		//手动控制模组单动
ACC_PROJECT_EXT Patch_AngAndR_CmdSel_TypeDef g_Patch_AngAndR_CmdSel;	//斜行和转弯命令二选一控制
ACC_PROJECT_EXT WalkMotorSmooth_TypeDef g_WMSM;				//行走电机速度平滑对象
ACC_PROJECT_EXT WalkMotorSmooth_TypeDef g_OSSM;				//行走差动速度平滑对象

ACC_PROJECT_EXT _MergeToPGV_TypeDef g_MergeToPGV;			//PGV数据融合对象
ACC_PROJECT_EXT _MergeToPGV_TypeDef g_AssemlyMergeToPGV;	//联动中 PGV数据融合对象

ACC_PROJECT_EXT SlamLayerLocation_StDef g_SlamLayer;		/*Slam导航图层切换对象*/
ACC_PROJECT_EXT	GroundCharge_CylinderControl_TypeDef g_GroundChargeCylinder;/*地充控制*/
ACC_PROJECT_EXT ACC_LeadShineCtrl_StDef g_LeadShineCtrl;	/*雷赛控制*/

ACC_PROJECT_EXT ACC_ActiveCharge_TypeDef g_ActiveCharge;	/*主动充电*/

//联动对象
ACC_PROJECT_EXT AsbCtrl_TypeDef g_Asb;
#ifdef SSMLT
//Agv系统模拟器对象
ACC_PROJECT_EXT AgvSimulator_TypeDef g_ASM;
#endif
//超声波对中标定对象
ACC_PROJECT_EXT ShelfAutadj_Para_TypeDef ShelfAutadj_Liner;

/*临时测试的对象*/
typedef struct
{
	int t1;
}TestTemp_TypeDef;
ACC_PROJECT_EXT TestTemp_TypeDef gtt;

#endif

//***********************************END OF FILE***********************************

