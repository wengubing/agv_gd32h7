/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: IOConf.h
** 创 建 人: 文小兵  
** 描    述: IO抽象对象定义
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年8月23日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __IOConf_h__
#define __IOConf_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  IOConf_GLOBALS
    #define IOConf_EXT
#else
    #define IOConf_EXT  extern
#endif

//自定义引用
#include <stdbool.h>
#include "drv_IODef.h"
#include "..\..\CBB\user\app_IOFilter\app_IOFilter.h"

//---------------------------------IO对象定义--------------------------
//输入IO 数据对象定义
typedef struct
{
	bool RealtimeValue;										//实时值
	bool StableValue;										//稳定值
	IO_iCNxBit_EnumDef ID;									//IO ID
	IOFilter_TypeDef Filter;								//滤波器
}IO_Input_TypeDef;

//输入IO组 数据对象定义
typedef struct
{
	IO_Input_TypeDef PowerOn;								//上电启动输入
	IO_Input_TypeDef EmgStop;								//车体急停输入
	IO_Input_TypeDef RemoteReceiverEmgStop;					//遥控器接收器急停输入
	IO_Input_TypeDef RemoteWiredSel;						//遥控器有线选择输入
	IO_Input_TypeDef ExternalBrakeSel;						//外部抱闸选择输入
	IO_Input_TypeDef DspModeSel;							//自动调度模式输入
	IO_Input_TypeDef ObsBarF;								//前防撞触边输入
	IO_Input_TypeDef ObsBarB;								//后防撞触边输入
	IO_Input_TypeDef ObsBarL;								//左防撞触边输入
	IO_Input_TypeDef ObsBarR;								//右防撞触边输入

	IO_Input_TypeDef AutoStartStop;							//开始任务按钮
	IO_Input_TypeDef AutoStop;								//暂停任务按钮
	IO_Input_TypeDef SysReset;								//系统复位按钮
	IO_Input_TypeDef Leg_Limit_LF;							//左前支腿限位
	IO_Input_TypeDef Leg_Limit_LB;							//左后支腿限位
	IO_Input_TypeDef Leg_Limit_RF;							//右前支腿限位
	IO_Input_TypeDef Leg_Limit_RB;							//右后支腿限位
	IO_Input_TypeDef Car_Key;								//授权钥匙

	IO_Input_TypeDef GroundChargeRetractLimit;				//地充回收到位限位信号

	//
	IO_Input_TypeDef LiftUpperLimit;						//升降上限为输入
	IO_Input_TypeDef LiftLowerLimit;						//升降下限为输入
	
	//前避障雷达状态输入
	IO_Input_TypeDef ObsRadar_F_Err;					    //前雷达故障信号
	IO_Input_TypeDef ObsRadar_F_Inner;					    //前雷达内层信号
	IO_Input_TypeDef ObsRadar_F_Middle;					    //前雷达中层信号
	IO_Input_TypeDef ObsRadar_F_Outer;					    //前雷达外层信号

	//后避障雷达状态输入
	IO_Input_TypeDef ObsRadar_B_Err;					    //后雷达故障信号
	IO_Input_TypeDef ObsRadar_B_Inner;					    //后雷达内层信号
	IO_Input_TypeDef ObsRadar_B_Middle;					    //后雷达中层信号
	IO_Input_TypeDef ObsRadar_B_Outer;					    //后雷达外层信号

	//左避障雷达状态输入
	IO_Input_TypeDef ObsRadar_L_Err;					    //左雷达故障信号
	IO_Input_TypeDef ObsRadar_L_Inner;					    //左雷达内层信号
	IO_Input_TypeDef ObsRadar_L_Middle;					    //左雷达中层信号
	IO_Input_TypeDef ObsRadar_L_Outer;					    //左雷达外层信号

	//右避障雷达状态输入
	IO_Input_TypeDef ObsRadar_R_Err;					    //右雷达故障信号
	IO_Input_TypeDef ObsRadar_R_Inner;					    //右雷达内层信号
	IO_Input_TypeDef ObsRadar_R_Middle;					    //右雷达中层信号
	IO_Input_TypeDef ObsRadar_R_Outer;					    //右雷达外层信号
	
	//前斜面避障雷达状态输入
	IO_Input_TypeDef ObsRadar_F_Inclined_Err;				//前斜面雷达故障信号
	IO_Input_TypeDef ObsRadar_F_Inclined_Inner;				//前斜面雷达内层信号
	IO_Input_TypeDef ObsRadar_F_Inclined_Middle;			//前斜面雷达中层信号
	IO_Input_TypeDef ObsRadar_F_Inclined_Outer;				//前斜面雷达外层信号
	
	
	//后斜面避障雷达状态输入
	IO_Input_TypeDef ObsRadar_B_Inclined_Err;				//后斜面雷达故障信号
	IO_Input_TypeDef ObsRadar_B_Inclined_Inner;				//后斜面雷达内层信号
	IO_Input_TypeDef ObsRadar_B_Inclined_Middle;			//后斜面雷达中层信号
	IO_Input_TypeDef ObsRadar_B_Inclined_Outer;				//后斜面雷达外层信号
	
	//其它输入
	IO_Input_TypeDef Assembly;								//联动选择输入
	
	//外部供电 卷线盘相关
	IO_Input_TypeDef CableRoll_Emg;							//卷线盘 放线到超过限位后 触发一类故障的检测
	IO_Input_TypeDef CableRoll_OutLimit;					//卷线盘 放线 限位

	//小急停遥控器
	IO_Input_TypeDef Little_Estop;							//小急停遥控器

	//注意Number的计算依赖于此后的对象内容不能改变
	unsigned short Number;									//IO数量
}IO_Inputs_TypeDef;

//输出IO 数据对象定义
typedef struct
{
	bool OutValue;											//实时值
	IO_oCNxBit_EnumDef ID;									//IO ID
}IO_Output_TypeDef;

//输出IO组 数据对象定义
typedef struct
{
	//系统启动控制
	IO_Output_TypeDef MainDCDC;								//主DCDC输入控制
	IO_Output_TypeDef CtrlVoltageBypass;					//系统旁路控制
	IO_Output_TypeDef PowerOnBypass;						//系统开关 旁路控制

	IO_Output_TypeDef ChargingMC;							//充电极板控制
	IO_Output_TypeDef GroundChargingExtend;					//地充电缸 伸出
	IO_Output_TypeDef GroundChargingRetract;				//地充电缸 缩回
	IO_Output_TypeDef GroundChargingPower;					//地充电缸 供电

	//伺服电源控制
	IO_Output_TypeDef Precharge;							//预充
	IO_Output_TypeDef MainContactor;						//主接触器
	IO_Output_TypeDef CtrllPower;							//伺服控制电源

	//驱动器电源控制
	IO_Output_TypeDef MotorPrecharge;						//驱动器预充
	IO_Output_TypeDef MotorContactor;						//驱动器主接触器
	IO_Output_TypeDef ANC;									//驱动器ANC控制

	//提示输出控制
	IO_Output_TypeDef LightPower;							//电源指示灯
	IO_Output_TypeDef LightSystem;							//系统
	IO_Output_TypeDef LightAlarm;							//报警
	IO_Output_TypeDef LampRed;								//三色 红
	IO_Output_TypeDef LampGreen;							//三色 绿
	IO_Output_TypeDef LampBlue;								//三色 蓝
	//三色指示灯同步端口
	IO_Output_TypeDef LampRed2;								//三色 红
	IO_Output_TypeDef LampGreen2;							//三色 绿
	IO_Output_TypeDef LampBlue2;							//三色 蓝
	
	//防爆LED
	IO_Output_TypeDef ExplosionProof_LED_1;					//防爆LED 左前
	IO_Output_TypeDef ExplosionProof_LED_2;					//防爆LED 右前
	IO_Output_TypeDef ExplosionProof_LED_3;					//防爆LED 左后
	IO_Output_TypeDef ExplosionProof_LED_4;					//防爆LED 右后

	IO_Output_TypeDef Buzzer;								//蜂鸣器
	IO_Output_TypeDef LightCaution;							//爆闪警示
	IO_Output_TypeDef AGVled_StartStop;						//AGV启动停止指示灯
	IO_Output_TypeDef AGVled_Reset;							//AGV复位指示灯
	IO_Output_TypeDef Horn;									//喇叭
	IO_Output_TypeDef Lighting;								//照明灯
	
	//卷线盘相关
	IO_Output_TypeDef CableRoll_ForcePowerOn;				//卷线盘 出现一类报警后 强制供电的继电器
	IO_Output_TypeDef CableRoll_Reel;						//卷线盘收线

	//电磁阀控制
	//泰坦三点液压平台
	IO_Output_TypeDef Platform_Titan_UpLF;					//左前平台上升电磁阀
	IO_Output_TypeDef Platform_Titan_UpRF;					//右前平台上升电磁阀
	IO_Output_TypeDef Platform_Titan_UpB;					//后平台上升电磁阀
	IO_Output_TypeDef Platform_Titan_UpLB;					//后左平台上升电磁阀 仅四点时有效
	IO_Output_TypeDef Platform_Titan_UpRB;					//后右平台上升电磁阀 仅四点时有效

	IO_Output_TypeDef Platform_Titan_DownLF;				//左前平台下降电磁阀
	IO_Output_TypeDef Platform_Titan_DownRF;				//右前平台下降电磁阀
	IO_Output_TypeDef Platform_Titan_DownB;					//后平台下降电磁阀
	IO_Output_TypeDef Platform_Titan_DownLB;				//后左平台下降电磁阀 仅四点时有效
	IO_Output_TypeDef Platform_Titan_DownRB;				//后右平台下降电磁阀 仅四点时有效

	//剪刀架单杠液压平台
	IO_Output_TypeDef Platform_Scissors_Down;				//平台下降
	
	//联合泵 液压平台
	IO_Output_TypeDef Platform_UnitePump_UpLF;				//左前上升电磁阀
	IO_Output_TypeDef Platform_UnitePump_UpRF;				//右前上升电磁阀
	IO_Output_TypeDef Platform_UnitePump_UpLB;				//左后上升电磁阀
	IO_Output_TypeDef Platform_UnitePump_UpRB;				//右后上升电磁阀

	IO_Output_TypeDef Platform_UnitePump_DownLF;			//左前下降电磁阀
	IO_Output_TypeDef Platform_UnitePump_DownRF;			//右前下降电磁阀
	IO_Output_TypeDef Platform_UnitePump_DownLB;			//左后下降电磁阀
	IO_Output_TypeDef Platform_UnitePump_DownRB;			//右后下降电磁阀
	
	
	//双泵四点液压 液压平台
	IO_Output_TypeDef Platform_DoublePumpQuad_DownLF;			//左前下降电磁阀
	IO_Output_TypeDef Platform_DoublePumpQuad_DownRF;			//右前下降电磁阀
	IO_Output_TypeDef Platform_DoublePumpQuad_DownLB;			//左后下降电磁阀
	IO_Output_TypeDef Platform_DoublePumpQuad_DownRB;			//右后下降电磁阀

	IO_Output_TypeDef Platform_DoublePumpQuad_ThrottleValveF;	/*前侧节流阀，开启后高度固定 无法升降*/
	IO_Output_TypeDef Platform_DoublePumpQuad_ThrottleValveB;	/*后侧节流阀，开启后高度固定 无法升降*/
	

	//泰坦三点液压平台
	IO_Output_TypeDef Absorption_Titan_UpLF;				//左前平台上升电磁阀
	IO_Output_TypeDef Absorption_Titan_UpRF;				//右前平台上升电磁阀
	IO_Output_TypeDef Absorption_Titan_UpB;					//后平台上升电磁阀
	IO_Output_TypeDef Absorption_Titan_DownLF;				//左前平台下降电磁阀
	IO_Output_TypeDef Absorption_Titan_DownRF;				//右前平台下降电磁阀
	IO_Output_TypeDef Absorption_Titan_DownB;				//后平台下降电磁阀

	//差速车专用升降控制
	IO_Output_TypeDef Platform_HydraulicUnit_Up;			//平台升
	IO_Output_TypeDef Platform_HydraulicUnit_Down;			//平台降

	//平台机构
	IO_Output_TypeDef LiftBrake;							//平台抱闸

	//推拉机构
	IO_Output_TypeDef PushRollerBrake;						//推拉抱闸


	//其他输出
	IO_Output_TypeDef MoveBrake;							//移动抱闸
	IO_Output_TypeDef CoolingFan;                           //散热风扇

	IO_Output_TypeDef PowerOffDealy;                        /*SLAM导航下的掉电延时继电器*/
	IO_Output_TypeDef PowerOffDealy_Bms;                    /*SLAM导航下的BMS掉电延时继电器*/

	//升降的科润阳光驱动器控制
	IO_Output_TypeDef KoRunReversal;						//转速换向
	IO_Output_TypeDef KoRunBrake;							//失能
	IO_Output_TypeDef KoRunSysPower;						//控制器电门
	IO_Output_TypeDef KoRunContactor;						//接触器
	
	//升降的2号科润阳光驱动器控制
	IO_Output_TypeDef KoRun_2_Reversal;						//转速换向
	IO_Output_TypeDef KoRun_2_Brake;						//失能
	IO_Output_TypeDef KoRun_2_SysPower;						//控制器电门
	IO_Output_TypeDef KoRun_2_Contactor;					//接触器

	//前避障雷达切区控制
	IO_Output_TypeDef ObsRadar_F_Ctrl1;						//前雷达切区IO1
	IO_Output_TypeDef ObsRadar_F_Ctrl2;						//前雷达切区IO2
	IO_Output_TypeDef ObsRadar_F_Ctrl3;						//前雷达切区IO3
	IO_Output_TypeDef ObsRadar_F_Ctrl4;						//前雷达切区IO4

	//后避障雷达切区控制
	IO_Output_TypeDef ObsRadar_B_Ctrl1;						//后雷达切区IO1
	IO_Output_TypeDef ObsRadar_B_Ctrl2;						//后雷达切区IO2
	IO_Output_TypeDef ObsRadar_B_Ctrl3;						//后雷达切区IO3
	IO_Output_TypeDef ObsRadar_B_Ctrl4;						//后雷达切区IO4

	//左避障雷达切区控制
	IO_Output_TypeDef ObsRadar_L_Ctrl1;						//左雷达切区IO1
	IO_Output_TypeDef ObsRadar_L_Ctrl2;						//左雷达切区IO2
	IO_Output_TypeDef ObsRadar_L_Ctrl3;						//左雷达切区IO3
	IO_Output_TypeDef ObsRadar_L_Ctrl4;						//左雷达切区IO4

	//右避障雷达切区控制
	IO_Output_TypeDef ObsRadar_R_Ctrl1;						//右雷达切区IO1
	IO_Output_TypeDef ObsRadar_R_Ctrl2;						//右雷达切区IO2
	IO_Output_TypeDef ObsRadar_R_Ctrl3;						//右雷达切区IO3
	IO_Output_TypeDef ObsRadar_R_Ctrl4;						//右雷达切区IO4

	//前斜面避障雷达切区控制
	IO_Output_TypeDef ObsRadar_F_Inclined_Ctrl1;			//前斜面雷达切区IO1
	IO_Output_TypeDef ObsRadar_F_Inclined_Ctrl2;			//前斜面雷达切区IO2
	IO_Output_TypeDef ObsRadar_F_Inclined_Ctrl3;			//前斜面雷达切区IO3
	IO_Output_TypeDef ObsRadar_F_Inclined_Ctrl4;			//前斜面雷达切区IO4
	
	//后斜面避障雷达切区控制
	IO_Output_TypeDef ObsRadar_B_Inclined_Ctrl1;			//后斜面雷达切区IO1
	IO_Output_TypeDef ObsRadar_B_Inclined_Ctrl2;			//后斜面雷达切区IO2
	IO_Output_TypeDef ObsRadar_B_Inclined_Ctrl3;			//后斜面雷达切区IO3
	IO_Output_TypeDef ObsRadar_B_Inclined_Ctrl4;			//后斜面雷达切区IO4

	//其它控制信号
	IO_Output_TypeDef LandscapeLaser;						//一字激光发射器
	IO_Output_TypeDef AirCompressor_In;						//空压机进气
	IO_Output_TypeDef AirCompressor_Out;					//空压机排气

	//注意Number的计算依赖于此后的对象内容不能改变
	unsigned short Number;									//IO数量
}IO_Outputs_TypeDef;

//IO2DHRBit 数据对象定义
typedef struct
{
	IO_IO2DHRBit_StDef* pIO2DHRBit;							//IO2DHRBit对象
	IO_CN2DHRBit_StDef* pCN2DHRBit;							//CN2DHRBit对象
}ACC_IOCN2DHRBit_TypeDef;


//对象外部声明
IOConf_EXT IO_Inputs_TypeDef g_IOInputs;					//输入IO组对象
IOConf_EXT IO_Outputs_TypeDef g_IOOutputs;					//输出IO组对象


//方法外部声明
IOConf_EXT bool ObjInit_IOInputs(void);						//输入io对象初始化
IOConf_EXT bool ObjInit_IOOutputs(void);					//输出io对象初始化
#endif

//***********************************END OF FILE***********************************


