/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Para_Sys.h
** 创 建 人: 文小兵  
** 描    述: 系统参数对象通用定义、通用操作方法
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年8月27日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __Para_Sys_h__
#define __Para_Sys_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  Para_Sys_GLOBALS
    #define Para_Sys_EXT
#else
    #define Para_Sys_EXT  extern
#endif

//自定义引用
#include <stdbool.h>
#include "common_def.h"
#include "LinearCal.h"
#include "app_MecanumWheel.h"
#include "AgvChassisController.h"
#include "LinkCanopenMaster.h"
#include "AgvChassisController.h"
#include "EQPComm_Init.h"
#include "Conf_User.h"
#include "drv_IODef.h"

//宏定义
#define Para_WheelAngle_MAXNUM								12

//电池类型定义
typedef enum
{
	ACC_LeadBatteries_TN48V			= 0 ,	//天能   48V铅酸电池 无需BMS通讯
	ACC_LeadBatteries_HBK48V		= 1 ,	//荷贝克 48V铅酸电池 无需BMS通讯
	ACC_LithiumBattery_48V_Low		= 21,	//自制   48V低压锂电 需要BMS通讯
	ACC_LithiumBattery_300V_High	= 31,	//自制  300V高压锂电 需要BMS通讯
	ACC_Cable_Power					= 41,	//外接电源供电
	ACC_LeadBatteries_TN72V			= 51,	//天能   72V铅酸电池 无需BMS通讯
}ACC_BatteryType_EnumDef;

//应用类型定义
typedef enum
{
	ACC_Application_GeneralRemote	= 0,	//单车		遥控
	ACC_Application_GeneralAGV		= 1,	//单车		AGV
	ACC_Application_AssemlyRemote	= 2,	//双车联动	遥控
	ACC_Application_AssemlyAGV		= 3,	//双车联动	AGV
}ACC_ApplicationType_EnumDef;

//应用类型定义
typedef enum
{
	ACC_ChargePileType_Passive		= 0,	//被动式充电桩 调度系统没有通讯的
	ACC_ChargePileType_Active		= 10,	//主动式充电桩 调度系统有通讯
	ACC_ChargePileType_Ground		= 20,	//地充 调度系统没有通讯，需要AGV伸出极板撞充电桩
}ACC_ChargePileType_EnumDef;

//轮系角度 参数定义
typedef struct
{
	u8 Num;													//轮系角度数量
	AngleType_EnumDef WheelAngleType;						//轮系角度类型
	f32 Prop;                                               //比例系数

	f32 OrgSetA[WheelAngleNum_MAX];                         //原始标定数据A
	f32 OrgSetB[WheelAngleNum_MAX];							//原始标定数据B
	f32 SetA;                            					//标定数据A
	f32 SetB;												//标定数据B
	f32 Offset[WheelAngleNum_MAX];							//零点偏移
}WheelAnglePara_StDef;

//行走平滑参数对象定义
typedef struct
{
	f32 VxAcc, VxDec;										//Vx加速增量、减速增量
	f32 VyAcc, VyDec;										//Vy加速增量、减速增量
	f32 VzAcc, VzDec;										//Vz加速增量、减速增量
	f32 AxAcc, AxDec;										//Ax加速增量、减速增量
	f32 AyAcc, AyDec;										//Ay加速增量、减速增量
	f32 AzAcc, AzDec;										//Az加速增量、减速增量
}SmoothPara_StDef;

//PGV组合方式对象定义，无需用户初始化，通过串口配置自动识别
typedef struct
{
	/*单个PGV情况*/
	uint32_t		HF_Tool_Sole			: 1	;/*对工装 单个 直连的自制*/
	uint32_t		DH_Tool_Sole			: 1	;/*对工装 单个 直连的大*/
	uint32_t		BJF_Tool_Sole			: 1	;/*对工装 单个 直连的倍加福*/
	uint32_t		SE_Tool_Sole			: 1	;/*对工装 单个 导航转接板*/

	uint32_t		HF_Groud_Sole			: 1	;/*对地 单个 直连的自制*/
	uint32_t		DH_Groud_Sole			: 1	;/*对地 单个 直连的大*/
	uint32_t		BJF_Groud_Sole			: 1	;/*对地 单个 直连的倍加福*/
	uint32_t		SE_Groud_Sole			: 1	;/*对地 单个 导航转接板*/

	uint32_t		HF_Tool_Merge			: 1	;/*对工装 两个 直连的自制*/
	uint32_t		DH_Tool_Merge			: 1	;/*对工装 两个 直连的大*/
	uint32_t		BJF_Tool_Merge			: 1	;/*对工装 两个 直连的倍加福*/
	uint32_t		SE_Tool_Merge			: 1	;/*对工装 两个 导航转接板*/

	uint32_t		HF_Groud_Merge			: 1	;/*对地 两个 直连的自制*/
	uint32_t		DH_Groud_Merge			: 1	;/*对地 两个 直连的大*/
	uint32_t		BJF_Groud_Merge			: 1	;/*对地 两个 直连的倍加福*/
	uint32_t		SE_Groud_Merge			: 1	;/*对地 两个 导航转接板*/

	/*从是否数据融合角度得到的汇总结果*/
	uint32_t		Tool_Sole				: 1	;
	uint32_t		Groud_Sole				: 1	;
	uint32_t		Tool_Merge				: 1	;
	uint32_t		Groud_Merge				: 1	;

}PGVInputTypePara_StDef;

//轮系坐标参数
typedef struct
{
    s16 x;                                                  // x坐标
    s16 y;                                                  // y坐标                                  
}ACC_ParaWalkAxisXY_TypeDef;

//行走功能参数 数据对象定义
typedef struct
{
	u8 MotorNum;											//电机个数
	ACC_ParaWalkAxisXY_TypeDef Mxy[MOTOR_NUM];				//轮系几何参数：舵轮指模组、麦轮为单个轮子
	u16 MGC_InstX;											//左右磁条间距一半
	u16 MGC_InstY;											//前后磁条间距一半
	u16 Lc;													//每组差动轮系内两个车轮的距离一半mm
	u16 MotorMaxRpm;										//电机最大速度
	f32 Ratio[MOTOR_NUM];									//电机减速器减速比
	u16 Diameter[MOTOR_NUM];								//轮子直径

	//小轮倾斜方向，1代表小轮倾斜方向为左斜，即常规4个麦轮的车，俯视时米字的左上角小轮倾斜方向
    s16 WheelType[MOTOR_NUM];
    //轮子速度方向 1电机速度命令正时，轮子转动方向为车体向前运动方向
    s16 MotorDirection[MOTOR_NUM];   

	u16 SpeedPlan[ACC_WALKGEARMAX];							//行走速度表 0.1rpm
	u16 JoystickMax;										//摇杆最大值
	u16 NC;													//之前是伺服类型，现在NC占位
	ACC_ModeSwitchMode_TypeDef ModeSwitchMode;				//模式切换模式

	//舵轮复合运动参数
	s16 CtrlAngMax_Left;									//解算超限判断
	s16 CtrlAngMax_Right;									//左正右负
	u16 SkewSpeed;											//差动速度 0.1rpm
	u16 SkewSpeed_ChangeIng;								//模式切换过程的
	u16 MaxSliAngPos;										//正向斜行最大角度 1°
	u16 MaxSliAngInf;										//横向斜行最大角度
	u16 MaxRotDisPos;										//正向圆弧运动最大半径 mm
	u16 MinRotDisPos;										//正向圆弧运动最小半径 mm
	u16 MaxRotDisInf;										//横向圆弧运动最大半径 mm
	u16 MinRotDisInf;										//横向圆弧运动最小半径 mm
	u16 AngInPlaceMin;		    							//到位角度 0.01°
	u16 AngInPlaceMin_ChangeIng;							//到位角度 0.01°
	u16 AngSlowDown;										//减速角度 0.01°
	u16 AngSlowDown_ChangeIng;								//减速角度 0.01°

	//控制命令平滑参数
	SmoothPara_StDef Smooth;								//平滑参数
	
	//力矩限制参数
	u16 TorqueLimit_Stop;									//此参数用于停止时的力矩限制 避免停下来的时候别劲过流 单位1%
	u16 LoadCntMax;											//反馈大于此值时，力矩将被限制到TorqueLimitMin
	u16 LoadCntMin;											//反馈小于此值时，力矩限制将恢复至TorqueLimitMax
	u16 TorqueLimitMax;
	u16 TorqueLimitMin;
	
	//位域参数
	struct
	{
		u32 AxyzSmoothStopWithVy : 1;						//Axyz平滑停止，在前向速度停下时
		
	}Bits;
	
}ACC_ParaWalk_TypeDef;

//联动行走 功能参数 数据对象定义
typedef struct
{
	//舵轮复合运动参数
	u16 MaxSliAngPos;										//正向斜行最大角度 1°
	u16 MaxSliAngInf;										//横向斜行最大角度
	u16 MinRotDisPos;										//正向圆弧运动最小半径 mm
	u16 MinRotDisInf;										//横向圆弧运动最小半径 mm

	//控制命令平滑参数
	SmoothPara_StDef Smooth;								//平滑参数

	//特殊位参数
	struct
	{
		u16 FB_Axis_Forward : 1;							//前后联动 模组坐标向前移动
		u16 FB_Axis_Moon : 1;								//前后联动 模组坐标卫星模式绕另一个车
		u16 LR_Axis_Fixed : 1;								//左右联动 模组坐标固定
		u16 LR_Axis_Moon : 1;								//左右联动 模组坐标卫星模式绕另一个车
		//u16 FB_MoveDir_Reverse : 1;							//前后联动 运动前后取反
		//u16 LR_MoveDir_Reverse : 1;							//左右联动 运动前后取反
	}Bit;
	
}ACC_ParaWalk_Asb_TypeDef;

//屏幕隐藏开关参数
typedef struct
{
	u16	GeneralParameter_Switch;						//一般参数界面       开关
	u16	HighShow_Switch;								//高度信息，压力显示  0 仅倒三角 1 倒三角 2 正三角 3 四点液压 4 全隐藏   开关
	u16	HighButton_Switch;								//高度界面单动开关和环境温度  0 倒三角 1 正三角 2 四点 3 全隐藏
	u16	AutomaticFunctions_Switch;	 					//主界面自动功能 0 全显示 1仅磁导航	2 仅激光导航 3 全隐藏
	u16 Onboard;										//板载模拟量界面模拟量输出 0 全显示 1 隐藏输出
	u16	WheelParameter_Switch;	 						//舵轮参数界面
	u16	AngleCalibration_Switch;	 					//角度标定界面
	u16 HeightCalibration_Switch;	 					//高度标定界面
	u16 LaserCalibration_Switch;	 					//激光测距标定界面
	u16 ParaObs_NO_Switch;								//避障雷达功能开关不可保存界面
	u16 ParaObs_Switch;	 								//避障雷达功能开关可保存参数界面
	u16 PGVCalibration_Switch;	 						//PGV标定界面
	u16	LeadshineServoCalibration_Switch;	 			//雷赛伺服标定界面
	u16	SensorCalibration_Switch;	 					//传感器标定界面
	u16	Odometry_Switch;	 							//里程计界面
	u16 AngleShow_Switch;								//角度显示界面
	u16 ControlCoeff_Switch;	 						//控制参数界面
	u16 MoreParameters_Switch;	 						//更多参数界面
	u16	PID_Switch;	 									//PID标定界面
	u16	PositionRadarCalibration_Switch;	 			//位置雷达标定界面
	u16	PositionRadarSpeedCalibration_Switch;	 		//位置雷达速度标定界面
	u16	SpeedGear_Switch;	 							//速度档位界面
	u16	VoltageSOC_Switch;	 							//电压电量关联界面
	u16	Suspension_Switch;	 							//悬挂高度界面
	u16	Outrigger_Switch;	 							//支腿高度界面
	u16	LaserShow_Switch;	 							//激光测距界面
	u16	Ultrasonic_Switch;	 							//超声波测距界面
	//	u16	Pressure_Switch									;	 	//压力传感器界面
	u16	AD_Switch;	 									//模拟量输入界面  0 全显示 1 模块1 2全隐藏
	u16 DA_Switch;										//模拟量输出界面  0 显示 1 隐藏
	u16	GPS_Switch;	 									//GPS信息界面
	u16	LaserNavigation_Switch;	 						//激光导航界面
	u16	DispatchingSystem_Switch;	 					//调度系统界面
	u16 AutoOperation_Switch;	 						//自动运行界面
	u16 Obs_Switch;	 									//避障雷达界面
	u16 MagneticNavigation_Switch;	 					//磁导航界面
	u16 LeadshineServo_Switch;	 						//雷赛伺服状态界面
	u16 PGV_Switch;	 									//PGV状态界面
	u16 PalletCamera_Switch;	 						//栈板相态界面
	u16 AsbShow_Switch;	 								//联动行走界面
	u16 BMSShow_Switch;	 								//电池管理系统界面
	u16 Facula_Switch;	 								//光斑状态界面
	u16 PositioningRadar_Switch;	 					//位置定位雷达界面
	u16 TiltSensor_Switch;	 							//倾角传感器界面
	u16 ManualPulseGenerator_Switch;	 				//手摇脉冲器界面
	u16 IOOutputForcing_Switch;	 						//IO输出强制界面
	u16 ANC_Switch;	 									//ANC界面
	u16 DBCOM_Switch;									//多倍通界面
}Para_HMIConceal_TypeDef;

//debug参数 数据对象定义
/*注意如果这里改了顺序，需要修改触摸屏界面对应的参数顺序，此条已无效*/
typedef struct
{
	unsigned int IOInputOff : 1;							//关闭功能组IO输入
	unsigned int ACCInputOff : 1;							//关闭ACC输入
	unsigned int PowerOnInputOff : 1;						//关闭PowerOn输入
	unsigned int CbbUpdateOff : 1;							//关闭CBB数据更新
	unsigned int AccStop : 1;								//停止ACC
	unsigned int AccCoreStop : 1;							//停止ACC核心
	unsigned int IOOutputOff : 1;							//关闭IO输出到HAL层
	unsigned int ACCOutputOff : 1;							//关闭ACC输出
	unsigned int ForcePowerOn : 1;							//强制启动
	unsigned int ForceOverErr : 1;							//强制忽略故障
	unsigned int ForceModeSwitch : 1;						//强制模式切换

	unsigned int ForceADOrgStop : 1;						//强制停止AD原始值更新
	unsigned int ForceDACmdStop : 1;						//强制停止DA命令执行
	unsigned int SvoDbug_AutoRun : 1;						//伺服调试模式 启动自动使能运转

	unsigned int ForceSVODisable : 1;						//强制伺服失能释放 行走
	unsigned int ForceSVODisable_Lift : 1;					//强制伺服使能释放 升降

	unsigned int MRSimEn : 1;								//解算模拟使能
	unsigned int LiftSimEn : 1;								//升降模拟使能

	//hmi
	unsigned int HmiShowOff : 1;							//关闭HMI显示更新
	unsigned int HmiParaOff : 1;							//关闭HMI参数更新

	unsigned int ADOff : 1;									//关闭功能组模拟量输入
	unsigned int DAOff : 1;									//关闭功能组模拟量输出

}ACC_ParAtDbug_TypeDef;

//代码运行方式 定义
typedef enum
{
	ACC_RunningMode_Release = 0,							//发布
	ACC_RunningMode_Test,									//测试
	ACC_RunningMode_HardwareSimulat,						//硬件仿真 连外设
	ACC_RunningMode_BareSimulat,							//硬件仿真 裸板
	ACC_RunningMode_SoftwareSimulat,						//纯软件仿真
	ACC_RunningMode_CodeTest,								//编程测试
}ACC_RunningMode_EnumDef;

//行走功能参数 数据对象定义
typedef struct
{
	u16 TimeOutJudge;										//通信超时时间
	u16 ModeSwitchTimeOutJudge;								//模式切换超时时间
	u16 WheelAngleAlarmRight;								//轮系角度报警右 度
	u16 WheelAngleAlarmLeft;								//轮系角度报警左
	u16 MagOutLineTime;										//磁条离线超时时间
	u16 MagOutLineDistance;									//磁条离线距离 mm
	u16 TimeOutDsp;											//调度系统通信超时
	u16 ColourOutLineTime;									//色带离线超时时间
	u16 TimeOutSlam;										//Slam通信超时
	u16 NC[8];												//预留 总数8个
}Para_EmgJudge_TypeDef;

//举升功能参数 数据对象定义
typedef struct
{
	u16 Num;												//高度数量
	u32 SetHighOrg[LIFT_HG_MAX];							//标定高点
	u32 SetLowOrg[LIFT_HG_MAX];								//标定低点 传感器
	u16 SetHigh;											//标定高点 实际																					
	u16 SetLow;												//标定低点 实际
	u16 LimitUp;											//上限位 mm
	u16 LimitDown;											//下限位 mm	
	u16 SpeedPlanUp[GEARS_NUM_MAX];							//上升速度表
	u16 SpeedPlanDown[GEARS_NUM_MAX];						//下降速度表 0.1rpm 或1mv
	LC_PlatformType_TypeDef PfType;							//平台类型
	/*titan 标准三点液压升降*/
	struct
	{
		float k_HgDiff2SpeedDiff;
	} Titan;
	u16 DiffMax;											// 平台高度差
	u16 nc[2];
}Para_Lift_StDef;

//功能使能参数
typedef struct
{
	ACC_ObsEn_BitDef BarEn;                                 //避障使能
	ACC_ObsEn_BitDef RadarEn;                               //雷达使能
}ACC_ParaFunctionSwitch_StDef;

//虚拟里程计参数 数据对象定义
typedef struct
{
	float Correction_X;                                     //X轴修正系数
	float Correction_Y;                                     //Y轴修正系数
	float Correction_Z;                                     //Z轴修正系数
	u16 NC[5];
}Para_Obo_StDef;

//控制系数参数 数据对象定义
typedef struct
{
	f32 k_TraceX;											//巡线斜行纠正系数
	f32 k_TraceZ;											//巡线自转纠正系数
	u16 VMin;												//最小速度指令 0~642
	u16 VMax;												//最大速度指令
	u16 k_TraceXMin;										//最小巡线斜行纠正系数 0.001
	u16 k_TraceXMax;										//最大巡线斜行纠正系数
	u16 k_TraceZMin;										//最小巡线自转纠正系数
	u16 k_TraceZMax;										//最大巡线自转纠正系数
	u16 TraceSkewSpeed;										//正向巡线的最大差动速度

	//功能参数位
    struct
    {
        unsigned short WheelFollow : 1;				        //使能轮系跟随巡线
		unsigned short MagneticNail : 1;					//使能标准巡磁钉功能
    }BitPara;

    u16 NC[8];
}CtrlCoef_TypeDef;

//APP对象输入数据源头 数据对象定义
typedef struct
{
	//功能参数位
    struct
    {
		/*高度数据来源*/
        unsigned short CommConvLiftHeight		: 1;		/*通过卡式转接板获取高度数据，一般是泰坦车的磁滞数据*/
		unsigned short CommConvAbsorptionHeight	: 1;		/*通过卡式转接板获取悬挂数据，一般是泰坦车的磁滞数据*/
		unsigned short AD_MCBLiftHeight			: 1;		/*通过直接的舟正模块获取高度数据，一般是拉线数据*/
		unsigned short CommConvDouble			: 1;		/*通过2个卡式转接板分别获取角度和磁滞数据*/
		

		/*压力传感器数据来源*/
		unsigned short MCBLoadPressure			: 1;		/*通过板载ADC检测获取负载压力数据（12位）；置0表明通过舟正模块获取负载压力数据（16位）*/
		/*避障雷达数据来源*/
		unsigned short ModBusROA				: 1;		/*通过modbus获取避障数据；置0表明通过IO信号控制*/

    }FucBit;

    uint16_t NC[7];
}AppInSource_TypeDef;


//APP对象输出数据源头 数据对象定义
typedef struct
{
	//功能参数位
    struct
    {
		/*语音喇叭控制结果*/
        unsigned short SerialExpVoicePlayer : 1;	/*通过导串口转接板控制语言喇叭*/
		/*升降电机控制结果*/
		unsigned short DACLifeMoter : 1;			/*通过DAC控制升降模块*/
    }FucBit;

    uint16_t NC[7];
}AppOutSource_TypeDef;


//安装标定参数 数据对象定义
typedef struct
{
	//磁导航安装偏移参数
	s16 MagF;											    //前磁导航安装偏移
	s16 MagB;											    //后磁导航安装偏移
	s16 MagL;											    //左磁导航安装偏移
	s16 MagR;											    //右磁导航安装偏移

	//向上二维码安装偏移参数
	s16 PGVUp_X;											//二维码安装偏移X
	s16 PGVUp_Y;											//二维码安装偏移Y
	s16 PGVUp_Z;											//二维码安装偏移Z

	//向下二维码安装偏移参数
	s16 PGVDown_X;											//二维码安装偏移X
	s16 PGVDown_Y;											//二维码安装偏移Y
	s16 PGVDown_Z;											//二维码安装偏移Z

	//雷赛伺服安装偏移参数
    s16 LeadShine;	

	u16 NC[9];
}Para_Inst_TypeDef;

//自动运行参数 数据对象定义
typedef struct
{
	struct
	{
		unsigned int GoodsMove_En : 1;						//负载移动报警检出使能
	}FucBit;												//功能位 u16*2

	u16 GoodsMove_x;										//横向移动报警检出距离 mm
	u16 GoodsMove_y;										//纵向移动报警检出距离 mm
	u16 GoodsMove_z;										//旋转移动报警检出角度 0.1°
	u16 MagParkZero_Limit;									//磁条停车纠偏零点限制 mm
	u16 AGVCrossMagParkMode;								//十字磁条停车方式 定义见自动部分代码

	u16 NC[9];												//预留
}Para_Auto_StDef;

//压力参数 数据对象定义
typedef struct
{
	u16		NC0;
	float 	WeightNoload;	//空载压力		单位：1T
	float 	SingleMax;		//单杠超载压力	单位：1MP
	float 	OverLoad;		//超载报警值（额定吨位）单位：1T

	u16 	NC[6];			//预留
}Para_Load_StDef;

/*避障雷达参数 数据对象定义*/
typedef struct
{
	struct
	{
		unsigned int Diagonal_L 					: 1;	/*左对角安装方式*/
		unsigned int Diagonal_R 					: 1;	/*右对角安装方式*/
		unsigned int Center 						: 1;	/*前后双雷达安装方式*/
		unsigned int Surround 						: 1;	/*四周四雷达安装方式*/
		unsigned int FourCorners 					: 1;	/*四角四雷达安装方式*/
		unsigned int ForwardBackInclined			: 1;	/*前后斜面雷达安装方式*/
	}Install;

	struct
	{
		unsigned int Hinson_HE_3091					: 1;	/*避障导航二合一雷达 银白色壳体 网线配置 Err常闭*/
		unsigned int Hinson_LE_50821				: 1;	/*避障导航二合一雷达 类似R2000  网线配置 Err常开*/
		unsigned int Hinson_LE_XX 					: 1;	/**/
		unsigned int Hinson_SE_XX 					: 1;	/*兴颂标准避障雷达   亮蓝色壳体 usb线配置 Err常开*/
		unsigned int Sick_XXX_XXX 					: 1;	/*西克雷达（未适配）*/
	}Brand;

	struct
	{
		unsigned int MCB_IO 						: 1;	/*IO直连输入				*/
		unsigned int MCB_Modbus						: 1;	/*数字通讯——ModBus直连	*/
		unsigned int MCB_Can						: 1;	/*数字通讯——Can直连输入	*/
		unsigned int SerialExp_Community			: 1;	/*数字通讯——导航转接板	*/
		unsigned int PLC_Community 					: 1;	/*数字通讯——PLC安全模块	*/
	}InPutSource;
	
	u16 NC[1];	//预留
}Para_ROA_StDef;


//系统自动对中功能定义
typedef enum
{
	Para_AutoCentre_Null_Fun	= 0,	/*没有自动对中功能*/
	Para_AutoCentre_ToolPGV		= 1,	/*对天PGV 自动对中*/
	Para_AutoCentre_GroudPGV	= 11,	/*对地PGV 自动对中*/
	Para_AutoCentre_Ultrasonic	= 21,	/*超声波  自动对中*/
	Para_AutoCentre_Linkage		= 22,	/*一字光斑+激光测距  联动找正*/
	Para_AutoCentre_SLAMOnline	= 25,	/*SLAM自动上线*/
	Para_AutoCentre_Magnetic	= 26,	/*磁条对正*/
}Para_AutoCentreFun_EnumDef;
//系统自动对中传感器定义
typedef enum
{
	Para_AutoCentre_Null_Input		= 0,	/*没有输入*/
	Para_AutoCentre_BJF_SoleTool	= 1,	/*倍加福对天PGV		进行对中*/
	Para_AutoCentre_HF_SoleTool		= 2,	/*航发自制对天PGV 	进行对中*/
}Para_AutoCentreIntput_EnumDef;
//系统自动对中 数据对象定义
typedef struct
{
	Para_AutoCentreFun_EnumDef		FunType;
	Para_AutoCentreIntput_EnumDef	IntPutType;
}Para_AutoCentre_Def;


//系统自动对中功能定义
typedef enum
{
	Para_TarceType_Null_Fun			= 0,	/*没有巡线功能				*/

	Para_TarceType_Magnetic			= 1,	/*磁导航巡线					*/

	Para_TarceType_ColorBand		= 11,	/*色带巡线					*/

	Para_TarceType_Slam				= 21,	/*Slam巡线					*/
	Para_TarceType_Slam_Magnetic	= 22,	/*Slam巡线为主、磁导航辅助	*/
	Para_TarceType_Slam_ColorBand	= 23,	/*Slam巡线为主、色带辅助		*/
	
	Para_TarceType_QR				= 31,	/*二维码巡线					*/
	
}Para_TarceType_EnumDef;

//巡线功能 数据对象定义
typedef struct
{
	Para_TarceType_EnumDef		FunType;
	
}Para_Tarce_Def;

//系统功能位开关32位 数据对象定义
typedef struct
{
	u32 Buzzer2LightAlarm : 1;								//打开报警指示灯兼顾运行提示
	u32 ErrVoice : 1;										//打开故障报警语音
	u32 LogicErr2Emg : 1;									//打开逻辑异常故障停车
	u32 LogicErr2Remind : 1;								//打开逻辑异常提醒
	u32 SelCarB : 1;										//选择为B车
	u32 AssemblyInfeed : 1;									//选择横向联动、
	u32 Asb_OR : 1;											//仅遥控联动
	u32 Asb_CommDirect : 1;									//直接使用对象作为数据池
	u32 Asb_Comm2Cmd : 1;									//控制命令来着联动通信
	u32 LimitWalk_Overload : 1;								//超载限制行走
	u32 LimitWalk_UnbalancedLoad : 1;						//偏载限制行走
	u32 LimitLiftUp_Overload : 1;							//超载限制上升
	u32 LimitLiftUp_UnbalancedLoad : 1;						//偏载限制上升
}Para_SysFuc_BitDef;

//其他配置参数
typedef struct
{
	u16	OverLoad;										// 超载阈值			 
	u16 in_Temperature_Environment_FanOff;				// 环境温度阈值，初始化为60℃
	u16 in_SmoothVxyzAcc;								// 软件加速度
	u16 in_SmoothVxyzDec;								// 软件减速度
}Para_Other_StDef;

//项目参数配置 函数指针
typedef struct
{
	bool (*p2PrjParaInit)(void);							//项目参数初始化
	bool (*p2ACC_GetCmdFormWirelessRemote)(void);			//项目遥控器解析
	void (*p2ACC_GetCmdFormWirelessRemote_Auto)(void);		//项目遥控器解析 自动
	void (*p2SkeletonAGV_Init)(void);						//项目自动框架初始化
	bool (*p2EQPInit)(void);								//项目外设通信配置对象初始化

	bool (*p2EmgJudge_ProjectExtend)(void);					//项目拓展异常检出函数
	bool (*p2ParaInit_Sys2EEPObj)(void);					//系统参数对象初始化到EEPROM
	bool (*p2HMI_Screen_PrjExp)(void);						//项目自定义界面

	bool (*p2PrjExp_AccStart)(void);						//项目自定义 AccStart函数
	bool (*p2PrjExp_AccEnd)(void);							//项目自定义 AccEnd函数
	bool (*p2PrjExp_TskAccStart)(void);						//项目自定义 TskAccStart函数

	bool (*p2PrjExp_MR_Init_Asb_Axis)(void);				//项目自定义 PrjExp_MR_Init_Asb_Axis
}Para_p2Fuc_Def;

//特殊联动参数
typedef struct
{
	u16 Channel_Single;										//信道号
	u16 Channel_Asb;										//
	u16 Address_Single;										//地址
	u16 Address_Asb;										//
	
	u16 Channel_Asb_1A;										//1A信道号
	u16 Address_Asb_1A;										//1A无线地址
		
	u16 Channel_Asb_1B;										//1B信道号
	u16 Address_Asb_1B;                                     //1B无线地址
	
	u16 Channel_Asb_2A;                                     //2A信道号
	u16 Address_Asb_2A;                                     //2A无线地址
	
	u16 Channel_Asb_2B;                                     //2B信道号
	u16 Address_Asb_2B;                                     //2B无线地址
	
	u16 Channel_Asb_3A;                                     //3A信道号
	u16 Address_Asb_3A;                                     //3A无线地址
	
	u16 Channel_Asb_3B;                                     //3B信道号
	u16 Address_Asb_3B;                                     //3B无线地址
	
	u16 Channel_Asb_4A;                                     //4A信道号
	u16 Address_Asb_4A;                                     //4A无线地址
	
	u16 Channel_Asb_4B;                                     //4B信道号
	u16 Address_Asb_4B;                                     //4B无线地址

	//用两个参数变量来设置联动通信的CN口，以便在程序运行中动态切换
	CCFID_EnumDef CN_Master;								//主站CN口
	CCFID_EnumDef CN_Slave;									//从站CN口

	//位参数
	struct
	{
		u32 Channel_Write : 1;								//使能写入信道号
		u32 Channel_Write_OnlyWireless : 1;					//仅无线时写入信道号 暂未实现
		u32 AsbSelect_FromHMI : 1;							//从HMI选择联动还是单动，面板上无实体旋钮
	}Bit;
}Para_Asb_TypeDef;

//参数检查
typedef struct
{
	IO_IO2DHRBit_StDef IO_Mapped;							//IO已映射标记

	//位结果
	struct
	{
		u32 IO_MapErr : 1;									//IO映射配置错误
	}Bit;

	bool ErrFlag;											//0无错误 1有错误
}Para_Check_TypeDef;

//系统调整参数 数据对象定义
typedef struct
{
	bool 							Initiated;		//初始化完成

	//函数指针
	Para_p2Fuc_Def 					p2Fucs;

	//基本参数
	u32 							CodeVersion;		//代码版本
	ACC_RunningMode_EnumDef 		RunningMode;		//代码运行方式
	ACC_EqpID_EnumDef 				EqpID;				//设备编号
	ACC_ChassisType_TypeDef 		ChassisType;		//底盘类型
	ACC_BatteryType_EnumDef 		BatteryType;		/*电池类型*/
	ACC_ApplicationType_EnumDef		ApplicationType;	/*应用类型 遥控、AGV、联动*/

	ACC_ChargePileType_EnumDef		ChargePileType;		/*充电桩类型*/
	Para_Tarce_Def					TarceType;			/*巡线方式*/
	//标定参数
	Para_Inst_TypeDef 				Inst;				//安装标定参数

	//功能参数
	ACC_ParaWalk_TypeDef			Walk;				//行走参数
	ACC_ParaWalk_Asb_TypeDef 		Walk_Asb;			//行走参数 联动
	WheelAnglePara_StDef 			WheelAngle;			//轮系角度参数
	Para_Lift_StDef 				Lift;				//升降参数
	Para_Lift_StDef 				Absorption;			//悬挂/减震参数   先完全复制平台升降的那一套，后续迭代时再特化 
	Para_Load_StDef					Load;				//称重参数
	ACC_ParAtDbug_TypeDef 			Debug;				//调试参数
	Para_EmgJudge_TypeDef 			EmgJudge;			//故障检出参数
	CtrlCoef_TypeDef 				CtrlCoef;			//控制系数参数
	
	AppInSource_TypeDef  			AppInSource;		/*传感器输入数据源选择参数*/
	AppOutSource_TypeDef 			AppOutSource;		/*传感器输处数据源选择参数*/
	
	//CBB参数
	Para_Obo_StDef 					Obo;				//虚拟里程计参数
	Para_Auto_StDef 				Auto;				//自动运行参数
	Para_ROA_StDef 					ROA;				/*避障雷达参数*/
	//功能使能参数
	ACC_ParaFunctionSwitch_StDef 	FunctionSwitch;		//功能使能参数
	//系统功能位开关
	Para_SysFuc_BitDef 				SysFucBit;			//系统功能位开关

	//接口参数
	ACC_ParaCanOpen_StDef			CanMst;				//canopen参数
	
	DataPool_BitDef					SerialExpObjSource;		/*导航转接板数据选择*/
	DataPool_BitDef					SerialExp_2_ObjSource;	/*第二导航转接板数据选择*/
	Para_EQPCommSource_Def			EQPCommSource;		/*EQP对象数据选择*/

	PGVInputTypePara_StDef			PGVInputType;
	
	Para_AutoCentre_Def				AutoCentre;			/*自动对中功能*/

	Para_Asb_TypeDef				Asb;				//特殊联动参数

	//参数检查
	Para_Check_TypeDef 				Check;
	
	// 其他参数
    Para_Other_StDef 				Other;				// 其他参数
	Para_HMIConceal_TypeDef			HmiConceal;			// HMI界面隐藏参数

	//测试示例参数
	u16 xxx1;
}ACC_Para_TypeDef;

#endif

//***********************************END OF FILE***********************************


