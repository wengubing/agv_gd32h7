/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: AgvChassisController.h
** 创 建 人: 文小兵  
** 描    述: AGV 底盘软件控制系统
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年4月19日
**--------------------------------------------------------------------------------------
***************************************************************************************/  

//保证以下所有内容只被同一源文件引用一次
#ifndef __AgvChassisController_h__
#define __AgvChassisController_h__

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  AgvChassisController_GLOBALS
	#define AgvChassisController_EXT
#else
	#define AgvChassisController_EXT  extern
#endif

//自定义引用																
#include <stdbool.h>
#include <math.h>
#include "..\..\..\CBB\user\basecalfunc\common_def.h"
#include "CBB_Init.h"
//#include "app_Assembly.h"


//宏定义
#define ACC_WALKMOTOR_MAX									32	//行走伺服最大数量
#define ACC_LIFTMOTOR_MAX									12	//举升伺服最大数量
#define ACC_PUSHTOLLERMOTOR_MAX								4	//推拉伺服最大数量
#define ACC_WALKGEARMAX										6	//行走挡位数量
#define ACC_OBSRADARMAX										6	//最大避障雷达数量

//-----------------------------------控制源切换过程对象定义-------------------------
//ACC调用step 定义
typedef enum 
{
    ACC_CallStep_Init=0,									//obj初始化
	ACC_CallStep_FirstRun,									//首次运行
	ACC_CallStep_Run,										//运行
}ACC_CallStep_TypeDef;

//避障雷达区域模式定义
typedef enum 
{
    AccObsArea_Null=0,							            //无效
    AccObsArea_NoloadPos,							        //1空载 正向
    AccObsArea_LoadPos,							            //2负载 正向
    AccObsArea_Charge,							            //3充电 横向
    AccObsArea_Enter_Noload_PosInf,							//4进工位 空载 正向/横向
    AccObsArea_Rotation,							        //5自转 负载
    AccObsArea_Enter_Load_PosInf,							//6进工位 负载 正向/横向
    AccObsArea_Noload_Inf,							        //7空载 横向
    AccObsArea_Load_Inf,							        //8负载 横向
	AccObsArea_Num,							                //
    AccObsArea_End=65535,							        //u16
}AccObsArea_TypeDef;


//工作模式定义
typedef enum 
{
	ACCMode_Null = 0,										//初始化
	ACCMode_PosStraight,        							//正向直线行走  
	ACCMode_PosFlexible,       	 							//正向自由行走
	ACCMode_PosTrace,										//正向巡线
	ACCMode_InfStraight,        							//横向直线行走  
	ACCMode_InfFlexible,       	 							//横向自由行走
	ACCMode_InfTrace,										//横向巡线
	ACCMode_Rotation,        								//自转
	ACCMode_Still,           								//静止
	ACCMode_Lift,       									//升降
	ACCMode_Absorption,       								//悬挂减震
	ACCMode_Push,											//推送
	ACCMode_Arm,											//运动臂
	ACCMode_Clip,											//夹持
	ACCMode_Leg, 											//支腿
	ACCMode_Roller,											//滚筒
	ACCMode_Num, 											//总数量
	ACCMode_End=65535, 										//u16
}ACCMode_TypeDef;

//三轴摇杆对象定义
typedef struct 
{
    float x;
	float y;
	float z;
}ACC_Joystick_TypeDef;

//功能按键命令 数据对象定义
typedef struct
{
	unsigned int LiftUp			:	1;	//上升
	unsigned int LiftDown		:	1;	//下降
	unsigned int ObsRadarFront	:	1;	//前方雷达避障打开
	unsigned int ObsRadarBack	:	1;	//后方雷达避障打开
	unsigned int ObsRadarLeft	:	1;	//左方雷达避障打开
	unsigned int ObsRadarRight	:	1;	//右方雷达避障打开
	unsigned int Horn 			: 	1;	//喇叭
	unsigned int Lighting 		:	1;	//照明
	unsigned int EStop 			:	1;	//急停 0急停按下 1急停恢复
	unsigned int ChargingMC		:	1;	//充电极板控制
    unsigned int ShelfAutadj 	:	1;	//自动对中按钮
	unsigned int LanMarkAutoadj	:	1;	//地标对正按钮
	unsigned int ToolingAutoadj	:	1;	//工装对正按钮

	unsigned int CableRoll_Reel	:	1;	//卷线盘收线
	unsigned int Level		 	:	1;	//调平按钮

	unsigned int Push 			:	1;	//推送
	unsigned int Pull 			:	1;	//拉取

    AccObsArea_TypeDef ObsArea;								//避障雷达区域模式
}ACC_FunctionButton_TypeDef;

//避障使能 数据对象定义
typedef struct
{
	unsigned char Front:1;									//前
    unsigned char Back:1;									//后
    unsigned char Left:1;									//左
    unsigned char Right:1;									//右
}ACC_ObsEn_BitDef;

//原始控制命令 对象定义
typedef struct 
{
	AsbModel_TypeDef HandleUnionVal;						//A、B、联动选择
	//bool AssemblyInfeed;									//联动模式选择横向 双向数据！！直接从HMI操作AccData的OrgCmd
    ACCMode_TypeDef Mode;									//模式
    ACCMode_TypeDef ModeLast;								//模式 上次值，内部记录用于切换状态机的初始化
	ACC_Joystick_TypeDef Vxyz;								//摇杆命令[-107,107]
	ACC_FunctionButton_TypeDef Button;						//按钮
	char WalkGear;											//行走挡位Z[1,6]
	char LiftGear;											//升降挡位Z[1,6]
    ACC_ObsEn_BitDef BarEn;								    //触边避障使能
    ACC_ObsEn_BitDef RadarEn;							    //雷达避障使能
}OrgCtrlCmd_TypeDef;

//处理后控制命令 对象定义
typedef struct 
{
	ACCMode_TypeDef Mode;									//模式
	ACCMode_TypeDef ModeLast;								//模式 上次值，内部记录用于切换状态机的初始化
	ACC_Joystick_TypeDef Vxyz;								//摇杆命令伴随挡位[-642,642]
	ACC_Joystick_TypeDef Axyz;								//A摇杆命令
	ACC_Joystick_TypeDef VOxyz;								//平滑后
	ACC_Joystick_TypeDef AOxyz;
	ACC_FunctionButton_TypeDef Button;						//按钮

    //功能参数位
    struct
    {
        unsigned short CentreCtrl : 1;				        //直接根据圆心坐标进行解算
    }BitPara;

	char WalkGear;											//行走挡位Z[1,6]
	char LiftGear;											//升降挡位Z[1,6]
    ACC_ObsEn_BitDef BarEn;								    //触边避障使能
    ACC_ObsEn_BitDef RadarEn;							    //雷达避障使能
}CalCtrlCmd_TypeDef;

//故障检出结果定义
typedef enum 
{
    ACC_FaultsType_Null = 0,								//初始化
	ACC_FaultsType_Normal,									//正常
	ACC_FaultsType_Warn,									//提示
	ACC_FaultsType_MoveLimited,								//限制运动
	ACC_FaultsType_ControlledStop,							//受控停车
	ACC_FaultsType_LoseControlStop,							//非受控停车
	ACC_FaultsType_Num,										//枚举数量
	ACC_FaultsType_End = 65535,								//u16
}ACC_FaultsType_TypeDef;

//异常结果定义
typedef struct
{
    ACC_FaultsType_TypeDef Faults;						    //故障类型

    struct
    {
        u16 UpLimited : 1;						            //限制上降
		u16 DownLimited : 1;						        //限制下降
		u16 MoveLimited : 1;						        //限制行走
    }Bit;
}EMG_Faults_TypeDef;

//故障检测输出
typedef struct
{
    EMG_Faults_TypeDef Self;								//本车异常
    EMG_Faults_TypeDef Asb_Input;							//联动汇总输入
    EMG_Faults_TypeDef Asb_Exe;							    //最终执行
}EMG_Output_TypeDef;

//伺服控制结果 数据对象定义
typedef struct
{
	bool En;
	float V;
	float V_Temp;											//用于平滑处理
	float P;												//转每分
	float VActual;
	float PActual;
}ACC_ServoCtrl_TypeDef;

//避障雷达IO输入 数据对象定义
typedef struct
{
	unsigned char ObsRadarErr 		: 1;	//避障雷达故障信号
	unsigned char ObsRadarInner 	: 1;	//避障雷达内层信号
	unsigned char ObsRadarMiddle 	: 1;	//避障雷达中间层信号
	unsigned char ObsRadarOuter 	: 1;	//避障雷达外层信号
}ACC_ObsRadarInputIO_TypeDef;

//避障雷达IO输输出 数据对象定义
typedef struct
{
	unsigned char ObsRadarCtrl1 : 1;	//避障雷达切区IO1
	unsigned char ObsRadarCtrl2 : 1;	//避障雷达切区IO2
	unsigned char ObsRadarCtrl3 : 1;	//避障雷达切区IO3
	unsigned char ObsRadarCtrl4 : 1;	//避障雷达切区IO4
}ACC_ObsRadarOutputIO_TypeDef;

//IO输入 数据对象定义
typedef struct
{
	unsigned int PowerOn 		: 1;	//启动开关
	unsigned int ExternalBrake	: 1;	//外部抱闸
	unsigned int WiredControl	: 1;	//有线控制
	unsigned int EmgStop		: 1;	//车体急停
	ACC_ObsRadarInputIO_TypeDef ObsRadarInput[ACC_OBSRADARMAX];//避障雷达输入
}ACC_InputIO_TypeDef;

//IO控制结果 数据对象定义
typedef struct
{
	//电源控制组
	unsigned int MainDCDC				: 1;	//主DCDC输入控制
	unsigned int CtrlVoltageBypass		: 1;	//系统24v 功率旁路控制
	unsigned int PowerOnBypass			: 1;	//系统开关 旁路控制

	unsigned int ChargingMC				: 1;	//充电极板控制
	unsigned int GroundChargingExtend 	: 1;	//地充电缸 伸出
	unsigned int GroundChargingRetract	: 1;	//地充电缸 缩回
	unsigned int GroundChargingPower		: 1;	//地充电缸 供电

	//电机控制组
	unsigned int Precharge				: 1;	//预充
	unsigned int MainContactor			: 1;	//主接触器
	unsigned int CtrllPower				: 1;	//伺服控制电源
	
	//驱动器控制组	
	unsigned int MotorPrecharge			: 1;	//驱动器预充
	unsigned int MotorContactor			: 1;	//驱动器主接触器
	unsigned int ANC					: 1;	//安川m3板卡电源
		
	//声光提示组	
	unsigned int LightPower				: 1;	//电源指示灯
	unsigned int LightSystem			: 1;	//系统
	unsigned int LightAlarm				: 1;	//报警
	unsigned int LampRed				: 1;	//三色 红
	unsigned int LampGreen				: 1;	//三色 绿
	unsigned int LampBlue				: 1;	//三色 蓝
	unsigned int LightCaution			: 1;	//爆闪警示
	unsigned int Buzzer					: 1;	//蜂鸣器
	unsigned int ExplosionProof_LED		: 1;	//防爆LED
	unsigned int AGVled_StartStop		: 1;	//AGV启动停止指示灯
	unsigned int AGVled_Reset			: 1;	//AGV复位指示灯
	unsigned int Horn					: 1;	//喇叭
	unsigned int Lighting				: 1;	//照明
	
	//卷线盘控制
	unsigned int CableRoll_ForcePowerOn	: 1;	//卷线盘强制送电
	unsigned int CableRoll_Reel			: 1;	//卷线盘收线
	//液压阀门控制
	unsigned int Platform_Titan_UpLF 	: 1;	//左前平台上升电磁阀
	unsigned int Platform_Titan_UpRF 	: 1;	//右前平台上升电磁阀
	unsigned int Platform_Titan_UpB 	: 1;	//后平台上升电磁阀
	unsigned int Platform_Titan_UpLB	: 1;	//后左平台上升电磁阀 四点时有效
	unsigned int Platform_Titan_UpRB	: 1;	//后右平台上升电磁阀 四点时有效
	unsigned int Platform_Titan_DownLF 	: 1;	//左前平台下降电磁阀
	unsigned int Platform_Titan_DownRF	: 1;	//右前平台下降电磁阀
	unsigned int Platform_Titan_DownB 	: 1;	//后平台下降电磁阀
	unsigned int Platform_Titan_DownLB	: 1;	//后左平台下降电磁阀 四点时有效
	unsigned int Platform_Titan_DownRB	: 1;	//后右平台下降电磁阀 四点时有效
	
	unsigned int Platform_Scissors_Down : 1;	//平台下降电磁阀
	
	unsigned int Platform_UnitePump_UpLF : 1;	//左前上升电磁阀
	unsigned int Platform_UnitePump_UpRF : 1;	//右前上升电磁阀
	unsigned int Platform_UnitePump_UpLB : 1;	//左后上升电磁阀
	unsigned int Platform_UnitePump_UpRB : 1;	//右后上升电磁阀
	unsigned int Platform_UnitePump_DownLF : 1;	//左前下降电磁阀
	unsigned int Platform_UnitePump_DownRF : 1;	//右前下降电磁阀
	unsigned int Platform_UnitePump_DownLB : 1;	//左后下降电磁阀
	unsigned int Platform_UnitePump_DownRB : 1;	//右后下降电磁阀

	unsigned int Platform_DoublePumpQuad_DownLF : 1;	//左前下降电磁阀
	unsigned int Platform_DoublePumpQuad_DownRF : 1;	//右前下降电磁阀
	unsigned int Platform_DoublePumpQuad_DownLB : 1;	//左后下降电磁阀
	unsigned int Platform_DoublePumpQuad_DownRB : 1;	//右后下降电磁阀

	unsigned int Platform_DoublePumpQuad_ThrottleValveF : 1;	/*前侧节流阀，开启后高度固定 无法升降*/
	unsigned int Platform_DoublePumpQuad_ThrottleValveB : 1;	/*后侧节流阀，开启后高度固定 无法升降*/

	//差速车专用升降控制
	unsigned int Platform_HydraulicUnit_Up	: 1;	//平台升
	unsigned int Platform_HydraulicUnit_Down: 1;	//平台降

	unsigned int Absorption_Titan_UpLF 		: 1;	//左前悬挂上升电磁阀
	unsigned int Absorption_Titan_UpRF 		: 1;	//右前悬挂上升电磁阀
	unsigned int Absorption_Titan_UpB 		: 1;	//后悬挂上升电磁阀
	unsigned int Absorption_Titan_DownLF 	: 1;	//左前悬挂下降电磁阀
	unsigned int Absorption_Titan_DownRF 	: 1;	//右前悬挂下降电磁阀
	unsigned int Absorption_Titan_DownB 	: 1;	//后悬挂下降电磁阀

	//其它控制
	unsigned int MoveBrake					:1;	//抱闸
	unsigned int CoolingFan					:1;	//风扇
	unsigned int PowerOffDealy				:1;	//掉电延时继电器
	unsigned int PowerOffDealy_Bms			:1;	//BMS掉电延时继电器
	unsigned int MotorBrake					:1;	//推拉抱闸
	unsigned int RollPower					:1;	//辊筒电源

	unsigned int Leg:1;										//支腿
	unsigned int Leg_Up:1;									//支腿升
	unsigned int Leg_Down:1;								//支腿降
	
	//升降的科润驱动器控制
	unsigned int KoRunReversal				:1;	//科润驱动器反转
	unsigned int KoRunBrake					:1;	//科润驱动器刹车
	unsigned int KoRunSysPower				:1;	//科润驱动器系统电源
	unsigned int KoRunContactor				:1;	//科润驱动器接触器
	
	//升降的2号科润驱动器控制
	unsigned int KoRun_2_Reversal			:1;	//科润驱动器反转
	unsigned int KoRun_2_Brake				:1;	//科润驱动器刹车
	unsigned int KoRun_2_SysPower			:1;	//科润驱动器系统电源
	unsigned int KoRun_2_Contactor			:1;	//科润驱动器接触器

	//空气悬挂控制
	unsigned int AirCompressor_In:1;						//空压机进气
	unsigned int AirCompressor_Out:1;						//空压机排气

	//推拉控制
	unsigned int PushRollerBrake			:1;	//推拉抱闸

	//联动相关
	unsigned int LandscapeLaser : 1;							//一字激光发射器
	
    //避障雷达IO输出
    ACC_ObsRadarOutputIO_TypeDef ObsRadarCtrl[ACC_OBSRADARMAX];//避障雷达输入
}ACC_OutputCtrl_TypeDef;

//切换行走模式 状态机定义
typedef enum 
{
    AccEnterWalkStep_Init				= 0,	//初始化
	AccEnterWalkStep_ServoEn			= 1,	//使能伺服
	AccEnterWalkStep_WaitTurnOnBrake	= 2,	//等待打开抱闸
	AccEnterWalkStep_TurnOnBrake		= 3,	//打开抱闸
	AccEnterWalkStep_TurnWheel			= 4,	//切换舵轮方向
	AccEnterWalkStep_WaitEnter			= 5,	//等待进入行走模式
	AccEnterWalkStep_Finish				= 6,	//完成

}ACC_EnterWalkStep_TypeDef;

//退出行走模式 状态机定义
typedef enum
{
	AccExitWalkStep_Init 			= 0,	//初始化
	AccExitWalkStep_ClearCmd		= 1,	//清除平滑前的控制命令
	AccExitWalkStep_WaitMotorStop	= 2,	//判断电机停止 或 超时
	AccExitWalkStep_WaitOffBreke	= 3,	//等待关闭抱闸
	AccExitWalkStep_TurnOffBrake	= 4,	//关闭抱闸
	AccExitWalkStep_ServoOff		= 5,	//失能伺服、清零速度
	AccExitWalkStep_Wait			= 6,	//等待
	AccExitWalkStep_Finish			= 7,	//完成

}ACC_ExitWalkStep_TypeDef;

//切换举升模式 状态机定义
typedef enum 
{
    AccEnterLiftStep_Init			=0,	//初始化
	AccEnterLiftStep_EnterCheck		=1,	//进入模式前的确认
	AccEnterLiftStep_ServoEn		=2,	//升降伺服使能
	//非垂直麦轮悬挂需要打开行走抱闸？
	//AccEnterLiftStep_PreOperation	=3,	//进入前的预操作
	AccEnterLiftStep_WaitEnter		=4,	//等待进入模式
	AccEnterLiftStep_Finish			=5,	//完成

}ACC_EnterLiftStep_TypeDef;

//切换推拉模式 状态机定义
typedef enum 
{
    AccEnterPushRollerStep_Init			=0,	//初始化
	AccEnterPushRollerStep_ServoEn		=1,	//升降伺服使能

	AccEnterPushRollerStep_WaitEnter	=2,	//等待进入模式
	AccEnterPushRollerStep_Finish		=3,	//完成

}ACC_EnterPushRollerStep_TypeDef;

//切换到静止 状态机定义
typedef enum 
{
    AccEnterStillStep_Init		=0,	//初始化
	AccEnterStillStep_NC		=1,	//NC
	AccEnterStillStep_WaitEnter	=2,	//等待进入模式
	AccEnterStillStep_Finish	=3,	//完成

}ACC_EnterStillStep_TypeDef;

//退出举升模式 状态机定义
typedef enum
{
	AccExitLiftStep_Init 		= 0,	//初始化
	AccExitLiftStep_ServoStop	= 1,	//升降伺服失能、关闭速度
	//非垂直麦轮悬挂需要打开行走抱闸？
	AccExitLiftStep_WaitOffValve= 2,	//等待关闭电磁阀
	AccExitLiftStep_Finish		= 3,	//完成
}ACC_ExitLiftStep_TypeDef;

//退出推送模式 状态机定义
typedef enum
{
	AccExitPushRollerStep_Init 			= 	0,	//初始化
	AccExitPushRollerStep_ServoStop		=	1,	//升降伺服失能、关闭速度

	AccExitPushRollerStep_TurnOffBrake	=	2,	//关闭抱闸
	AccExitPushRollerStep_WaitOffValve	=	3,	//等待伺服失能
	AccExitPushRollerStep_Finish		=	4,	//完成

}ACC_ExitPushRollerStep_TypeDef;

//舵轮角度切换 step定义
typedef enum
{
	ACC_SteeringStep_Init 		= 0,	//初始化 计算目标角度
	ACC_SteeringStep_DiagonalLF	= 1,	//左前对角转向
	ACC_SteeringStep_DiagonalRF	= 2,	//右前对角转向
	ACC_SteeringStep_Finish		= 3,	//完成

}ACC_SteeringStep_TypeDef;

//模式切换 数据对象定义
typedef struct
{
	ACCMode_TypeDef ModeTarget;								//目标模式

	u32 MsRecord;											//ms记录
	u32 MsRecord_Steering;									//ms记录 舵轮转向
	ACC_EnterWalkStep_TypeDef EnterWalkStep;				//切换行走模式 step
	ACC_EnterPushRollerStep_TypeDef EnterPushRollerStep;	//切换推拉模式 step
	ACC_EnterLiftStep_TypeDef EnterLiftStep;				//切换举升模式 step
	ACC_EnterStillStep_TypeDef EnterStillStep;				//切换静止模式 step
	ACC_ExitWalkStep_TypeDef ExitWalkStep;					//退出行走模式 step
	ACC_ExitLiftStep_TypeDef ExitLiftStep;					//退出举升模式 step
	ACC_ExitPushRollerStep_TypeDef ExitPushRollerStep;		//退出举升模式 step
	ACC_SteeringStep_TypeDef SteeringStep;					//舵轮角度切换 step
	bool Exiting;											//已经进入退出状态机

	bool Finished;											//完成标志
}ACC_ModeSwitch_TypeDef;

//遥控底盘 遥控功能外部实现 方法指针,外部定义，内部调用
typedef struct
{
	EMG_Faults_TypeDef (*p2EmgJudge)(void);		//异常检出函数
	bool (*p2WalkFunc)(void);					//行走解算函数
	bool (*p2LiftFunc)(void);					//升降解算函数
	bool (*p2LegFunc)(void);					//支腿解算函数
	bool (*p2AbsorptionFunc)(void);				//悬挂/减震解算函数
	bool (*p2PushPull_Func)(void);				//推拉机构解算函数
	bool (*p2Roller_Func)(void);				//滚筒机构解算函数
}ACC_ExtFunc_TypeDef;

//受控停车 状态机定义
typedef enum 
{
	ACC_LoseControlStopStep_Init		= 0,	//初始化
    ACC_LoseControlStopStep_ServoOff	= 1,	//关闭伺服
	ACC_LoseControlStopStep_WaitBrake	= 2,	//等待抱闸
	ACC_LoseControlStopStep_Finish		= 3,	//完成
}ACC_LoseControlStopStep_TypeDef;

//非受控停车 数据对象定义
typedef struct
{
	ACC_LoseControlStopStep_TypeDef Step;	//Step

	u32 MsRecord;							//ms记录
}ACC_LoseControlStop_TypeDef;

//底盘类型定义
typedef enum
{
	ACC_ChassisType_Turtle	= 0,	//麦轮底盘
	ACC_ChassisType_Titan	= 1,	//Titan底盘
	ACC_ChassisType_Rhino	= 2,	//Rhino底盘
	ACC_ChassisType_Ant		= 3,	//Ant底盘
	ACC_ChassisType_DiffVCar= 4,	//差速轮底盘
	ACC_ChassisType_Single	= 5,	//单轮RGV底盘
}ACC_ChassisType_TypeDef;

//标准底盘 延时计数对象
typedef struct
{
	u16 ModeChange;											//模式切换延时
}ACC_DelayMs_TypeDef;

//舵轮转向模式定义 目前只实现了对角和一起转
typedef enum
{
	ACC_ModeSwitchMode_Null 	= 0,	//无效
	ACC_ModeSwitchMode_Direct	= 1,	//直接切换
	ACC_ModeSwitchMode_Single	= 2,	//单个模组转
	ACC_ModeSwitchMode_Quadrant	= 3,	//单个象限
	ACC_ModeSwitchMode_Diagonal	= 4,	//对角转
	ACC_ModeSwitchMode_All		= 5,	//一起转
}ACC_ModeSwitchMode_TypeDef;

//联动控制
typedef struct
{
    ACC_Joystick_TypeDef VxyzMax;							//联动时 最大速度命令
    ACC_Joystick_TypeDef AxyzMax;							//联动时 最大复合运动命令
    f32 LiftMotorMax;										//联动时 升降电机最大转速
    f32 AdjVx;											    //Vx纠正量
 	f32 AdjVy;											    //Vy纠正量
 	f32 AdjVz;												//Vz纠正量
    f32 AdjUpDown;											//升降纠正量
    bool AdjEn;											    //纠正使能
	bool MoveDir_Reverse;									//前进方向取反
}ACC_AsbCtrl_TypeDef;

//设置参数
typedef struct
{
	//非受控停车 抱闸延时 ms
	u16 LoseControlStop_BrakeTime;

	//联动距离限制 在面板联动、遥控器单动下，自己的传感器数据太远或太近时，强制清除运动命令
	u16 DisLimit_Min;										//最小距离限制 mm 65535关闭
	u16 DisLimit_Min_SlowDown;								//减速距离 mm 小于该值时，减速
	u16 DisLimit_Min_SlowDown_V;							//减速遥感值
	u16 DisLimit_Max;
	u16 DisLimit_Max_temp;
	u16 DisLimit_Min_temp;

}ACC_SetPara_TypeDef;

//状态机组
typedef struct
{
	bool Car_AsbSel;										//面板选择联动
}ACC_STMs_TypeDef;

//标准底盘 数据对象定义
typedef struct
{
	//初始化参数
	ACC_SetPara_TypeDef Para;
	ACC_ChassisType_TypeDef	ChassisType;					//底盘车型
	u16 JoystickMax;										//最大摇杆命令值 107
	u16 SpeedPlan[ACC_WALKGEARMAX];							//行走挡位速度表
	ACC_ModeSwitchMode_TypeDef ModeSwitchMode;				//模式切换模式

	//实时输入
	u32 in_Ms;												//输入ms
	ACC_InputIO_TypeDef InputIO;							//IO输入量 暂时未在核心代码中使用
	ACC_FaultsType_TypeDef in_FaultsType;					//故障检出结果
    EMG_Faults_TypeDef in_Faults;							//异常检出结果 含具体限制位
	OrgCtrlCmd_TypeDef OrgCmd;								//原始控制命令
	ACC_DelayMs_TypeDef DelayMs;							//延时计数对象

    //联动控制信息
    ACC_AsbCtrl_TypeDef AsbCtrl ;							//控制信息

	//过程对象
	ACC_STMs_TypeDef STMs;									//状态机组
	bool SM_RunFlg;											//控制命令平滑调用标志 Vxyz
	bool SM_RunFlg_Axyz;									//控制命令平滑调用标志 Axyz
	//ACC_CallStep_TypeDef RunStep;							ACC调用step
	CalCtrlCmd_TypeDef CalCmd;								//处理后的执行控制命令
	ACC_LoseControlStop_TypeDef LoseControlStop;			//非受控停车
	
	ACCMode_TypeDef ModeSwitchSMT;							//模式切换状态机
	ACC_ModeSwitch_TypeDef ModeSwitch;						//模式切换数据对象
	ACC_FaultsType_TypeDef FaultsResponseSTM;				//故障响应状态机
    //FaultsResponseObj_TypeDef FaultsResponseObj;			//故障响应对象
	
	bool WalkServoEn, WalkServoEnLast;						//行走伺服使能标志
	bool LiftServoEn, LiftServoEnLast;						//举升伺服使能标志 用科润驱动器时无效
	bool PushRollerServoEn, PushRollerServoEnLast;			//推送伺服使能标志

	//输出状态
	ACC_OutputCtrl_TypeDef IOCtrl;							//IO控制量

	ACC_ServoCtrl_TypeDef WalkMotor[ACC_WALKMOTOR_MAX];		//行走电机控制
	ACC_ServoCtrl_TypeDef LiftMotor[ACC_LIFTMOTOR_MAX];		//升降电机电机控制
	ACC_ServoCtrl_TypeDef PushRollerMotor[ACC_PUSHTOLLERMOTOR_MAX];

	//调试内容
	u16 MoveBrake_Patch_Cnt;								//抱闸补丁执行计数

	//方法
	ACC_ExtFunc_TypeDef ExtFunc;							//外部函数接口
}ACC_DATA_TypeDef;


//对象声明

//函数声明
AgvChassisController_EXT void AgvChassisControl(u32 in_Ms,ACC_DATA_TypeDef *p);

AgvChassisController_EXT bool ACC_ModeSMTIsWalk(ACC_DATA_TypeDef *p);

AgvChassisController_EXT bool ACC_IsPosWalkMode(ACCMode_TypeDef in_Mode);
AgvChassisController_EXT bool ACC_IsInfWalkMode(ACCMode_TypeDef in_Mode);
AgvChassisController_EXT bool ACC_IsTraceMode(ACCMode_TypeDef in_Mode);
AgvChassisController_EXT bool ACC_IsWalkMode(ACCMode_TypeDef in_Mode);
AgvChassisController_EXT bool Acc_MsCnt(void);										//ms计数接口
#endif

//***********************************END OF FILE***********************************
