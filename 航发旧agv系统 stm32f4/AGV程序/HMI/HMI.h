/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: HMI.h
** 创 建 人: 文小兵  
** 描    述: HMI与系统的交互
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年7月9日
**--------------------------------------------------------------------------------------
***************************************************************************************/
//笔记
//目前内部对象定义引用了全局对象定义，是为了项目快速完成，后面目标为完全解耦
// 
//保证以下所有内容只被同一源文件引用一次
#ifndef __HMI_h__
#define __HMI_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  HMI_GLOBALS
    #define HMI_EXT
#else
    #define HMI_EXT  extern
#endif

//自定义引用	
#include <stdbool.h>
#include "ObjConf.h"
#include "Acc_Project.h"
#include "drv_path.h"
//#include "Debug.h"

//宏定义
#define HMI_AngleSensor_MaxNum    							14//角度传感器数量
#define HMI_Servo_MaxNum    								8//伺服传感器数量

#define HMI_AnalogOut_MaxNum    							8//模拟量传感器数据
#define HMI_DigitalInput_MaxNum 							5
#define HMI_PGV_Num               							12
#define HMI_Cache_Num      									150//定义HMI缓存数据区大小

#define Car_Max											    4   //联动车辆数量

//测试  
#define Test								           1  

HMI_EXT volatile uint8_t enable_status;

//20 起始页面
typedef struct
{
	unsigned short int Run_Mode;			//模式
	unsigned short int System_SOC;  		//电量
	unsigned short int SLanguage_English;	//屏幕英文语系开关
	unsigned short int SLanguage_Russian;	//主界面俄罗斯语系开关
	unsigned short int SLanguage_Korean;    //主界面韩语语系开关	
	unsigned short int ang;    				//角度图层开关
	
}Screen_HMIPage_Start;	
	
//21 主界面状态
typedef struct
{
	unsigned short int Control_Mode;                	    //控制方式 0:空 1:遥控  2调度 3手柄  4HMI  
	unsigned short int Insrx_Status;                        //遥控器状态 1在线与 0离线
	unsigned short int Run_Mode;                            //运行模式 0待机 1系统故障 2运行 3暂停 4停止 5急停 6急停恢复 7报警 8报警回复
	unsigned short int WorkMode;                            //目前还未定义各个模式的编号，具体见上定义
	unsigned short int System_Voltage;                      //电压
	unsigned short int System_SOC;                          //电量
	unsigned short int Pltform_Height;                      //平台升降高度
	unsigned short int Hang_Height;                         //悬挂高度
	unsigned short int CurrentOrientation;                  //设备当前朝向
	unsigned short int CurrentPointID;                    	//设备当前点ID
	unsigned short int InputPointID;                    	//HMI输入点ID
	unsigned short int RunSta;
	unsigned short int ErrCode;		
	unsigned short int LocationSta;					
	unsigned short int HMI_NOOROFF;//屏幕组件宏  0为打开屏幕显示
}Screen_Main_Typedef;
	
//24 系统信息界面
typedef struct __attribute__((packed))
{
	u16 HardwareVersion;                                    //硬件版本
	u16 SoftVersion;                                        //软件版本
	u32 System_MsCnt;                                       //系统MS计数 
	s16 SystemCore_Temp;                                    //内核温度 0.1℃
	s16 Environment_Temp;        			                //环境温度 0.1℃
	//ACC_EqpID_EnumDef EqpID;								//设备ID
	u32 EqpID;												//设备ID

	//packed嵌套！！
	//ACC_CodeErr_BitDef CodeErr;								//代码异常
	//LogicErr_StDef LogicErr;								//逻辑异常
	//SysBitExpand_StDef SysBitExpand;						//系统状态拓展位
	u16 CodeErr;											//代码异常
	u16 LogicErr;											//逻辑异常
	u16 SysBitExpand;										//系统状态拓展位
}SystemInformation_Typedef;

//25板载模拟量输入输出界面
typedef struct
{
    u16 Digital_Input_Value[HMI_DigitalInput_MaxNum],           //数字量输入值
        Analog_Actual_Value[HMI_DigitalInput_MaxNum];           //转换后的模拟量值  ----0.1V
    u16 Analog_Out_Value[HMI_AnalogOut_MaxNum];                 //模拟量输出值 ----1mv
	
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  1为打开屏幕显示	
}Screen_AdOnboard_Typedef;

//26遥控器状态回显界面
//wxb@20240715 改为参考cbb接口设计
typedef struct
{
	u32	CommucationMsCnt;									/*通讯毫秒计数*/
	u16	CountPerSecend;										/*1s内接收到遥控器数据包数量*/
	s16	J7_X,												/*X轴原始值*/
		J7_Y,												/*Y轴原始值*/
		J7_Z;												/*Z轴原始值*/
	u16	J41,												/*遥控器J41端口原始值，值为多少则表示亚空气J41端口的第几位被置1*/
		J42,												/*遥控器J42端口原始值，值为多少则表示亚空气J42端口的第几位被置1*/
		J43,												/*遥控器J43端口原始值，值为多少则表示亚空气J43端口的第几位被置1*/
		J44;												/*遥控器J44端口原始值，值为多少则表示亚空气J44端口的第几位被置1*/

	//cbb上为u8，这里改为u16，便于通信缓存对齐
	u16	J11,												/*J11原始值,Bit0表示1脚状态，Bit1表示4脚状态*/
		J14,												/*J14原始值,Bit0表示1脚状态，Bit1表示4脚状态*/
		J17,												/*J17原始值,Bit0表示1脚状态，Bit1表示4脚状态*/
		J22;												/*J22原始值,Bit0表示1脚状态，Bit1表示4脚状态*/

	//cbb为bool，这里改为位域，便于通信缓存对齐
	struct
	{
		unsigned int J12 : 1;								/*J12原始值,1导通，0断开*/
		unsigned int J15 : 1;								/*J15原始值,1导通，0断开*/
		unsigned int J18 : 1;								/*J18原始值,1导通，0断开*/
		unsigned int J20 : 1;								/*J20原始值,1导通，0断开*/
		unsigned int J23 : 1;								/*J23原始值,1导通，0断开*/
		unsigned int J25 : 1;								/*J25原始值,1导通，0断开*/
		unsigned int J27 : 1;								/*J27原始值,1导通，0断开*/
		unsigned int J10 : 1;								/*J10原始值,1导通，0断开*/
		unsigned int J13 : 1;								/*J13原始值,1导通，0断开*/
		unsigned int J16 : 1;								/*J16原始值,1导通，0断开*/
		unsigned int J19 : 1;								/*J19原始值,1导通，0断开*/
		unsigned int J21 : 1;								/*J21原始值,1导通，0断开*/
		unsigned int J24 : 1;								/*J24原始值,1导通，0断开*/
		unsigned int J26 : 1;								/*J26原始值,1导通，0断开*/
		unsigned int J28 : 1;								/*J28原始值,1导通，0断开*/
		unsigned int WirelessSleep : 1;						/*遥控器无线休眠状态：0--未休眠  1--已休眠*/
	}Bit;
}WirelessStatus_Typedef;

//27通信状态计数
typedef struct
{
	u16 WHA;                                      			//0遥控器A
	u16 HMI;												//1HMI
	u16 BMS;												//2BMS
	u16 CommConv;											//3高度角度转接板
	u16 AD;													//4采集模块
	u16 MagF;												//5磁导航前
	u16 MagB;												//6磁导航后
	u16 MagL;												//7磁导航左
	u16 MagR;												//8磁导航右
	u16 RFID;												//9rfid
	u16 PGVUp;												//10PGV上
	u16 SerialExp;											//11拓展板
	u16 DSP;												//12调度系统
	u16 MagLift;											//13磁导航垂直升降转接板
	u16 Voice;												//14语音喇叭
	u16 MagLiftF;											//15磁导航垂直升降前
	u16 MagLiftB;											//16磁导航垂直升降后
	u16 MagLiftL;											//17磁导航垂直升降左
	u16 MagLiftR;											//18磁导航垂直升降右
	u16 PGVDown;											//19PGV下
	u16	PGVUpward_F;						//向上前pgv传感器
	u16	PGVUpward_B;						//向上后pgv传感器
	u16	PGVDownward_F;						//向下前pgv传感器
	u16	PGVDownward_B;						//向下后pgv传感器
	u16	AD1;								//模拟量采集1
	u16	DA;
	u16	CommConv1;							//通信转换板
	u16	CommConv2;							//通信转换板
	u16 NC[2];												//nc
}Screen_LinkStaus_List_Typedef;
//通信状态计数
typedef struct
{        
	Screen_LinkStaus_List_Typedef OkCnt;					//通信成功计数
	Screen_LinkStaus_List_Typedef TimeOutCnt;				//通信失败计数
	u32 AngleNum;		
}Screen_LinkStaus_Typedef;

//28IO状态
/*typedef struct __attribute__((packed))
{	
	u16   DiffAlarmJudge;									//平台高低差过大报警阈值  初始化为20mm				
	u16   OverweightAlarmJudge;								//超载阈值			 				
	u16   in_Temperature_Environment_FanOff;				//环境温度阈值		   				
	u16   in_SmoothVxyz;									//软件加速度					// 
	
}Screen_IOStatus_TypeDef;*/

//29 单个伺服----风得控，台达，步科伺服数据状态结构体
typedef struct  __attribute__((packed))
{
	//反馈信息
	u16 Actual_Voltage;                                     //电压---0.1v
	s16 Actual_Current;                                     //电流----0.1A
	s16 Actual_Temp;                 			            //温度----1℃
	s16 Actual_Speed;                                       //转速----0.1rpm
	u16 SlaveStatus;                       	                //从机状态----

	//控制信息
	u16 Control_Word;										//控制字----
	s16 Control_Speed;                                      //控制速度----0.1rpm

	u16 Status_Word;                                        //状态字----
	u16 SlaveErrCounter;                                    //从机错误计数----
	u32 Event_Word;                                         //事件字----
	u32 Faltch_Code;                                        //错误锁存---- 
}ServoStatus_Typedef;

//Canopen
typedef struct __attribute__((packed))
//typedef struct __attribute__((aligned(2)))
{
	u16 CanopenStep;										//CANopen步骤
	u16 MasterStatus;										//主机状态
	u16 SlaveNum;											//从机数量
	u16 nc;													//占位预留
	ServoStatus_Typedef Servo[HMI_Servo_MaxNum];			//伺服状态
}CanopenStatus_Typedef;

//30手动控制按钮功能位域定义
typedef struct
{
    unsigned short LF:1;      	                            //0左前
    unsigned short F:1;      	                            //1前
    unsigned short RF:1;      	                            //2右前
    unsigned short L:1;      	                            //3左
    unsigned short Horn:1;      	                        //4喇叭
    unsigned short R:1;      	                            //5右
    unsigned short LB:1;      	                            //6左后
    unsigned short B:1;      	                            //7后
    unsigned short RB:1;      	                            //8右后
    unsigned short Anticlockwise:1;      	                //9逆时针
    unsigned short Clockwise:1;      	                    //10顺时针
    unsigned short Lighting:1;      	                    //11照明
    unsigned short Up:1;      	                            //12上升
    unsigned short Down:1;      	                        //13下降
    unsigned short NC:1;      	                            //14NC
    unsigned short Enable:1;      	                        //15使能
}ManualCtrl_Button_TypeDef;

//手动控车类型
typedef struct
{
	unsigned short RunMode;	                                //0运行模式
	unsigned short WalkSpeed; 	                            //1行走前向摇杆命令 0-100
    unsigned short WalkGear;                                //2行走挡位
	unsigned short LiftSpeed; 	                            //3举升挡位
    unsigned short NC4; 	                                //NC4
    unsigned short NC5; 	                                //NC5
    unsigned short NC6; 	                                //NC6
    unsigned short NC7; 	                                //NC7
	ManualCtrl_Button_TypeDef Button;                       //8控件按钮
}Screen_ManualControl_Typedef;

//31 一般参数设置界面
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  1为打开屏幕显示
	u16 diffALarmJudge;										//平台高低差过大报警阈值  初始化为20mm
	u16	overweightAlarmJudge;								//超载阈值			 
	u16 in_Temperature_Environment_FanOff;					//环境温度阈值		      初始化为60℃g_Fan.in_Temperature_Environment_FanOff;
	u16 in_SmoothVxyzAcc;									// 软件加速度
	u16 in_SmoothVxyzDec;									// 软件减速度
}Screen_GeneralParameter_TypeDef;

//32轮系参数界面
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;							//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
	unsigned short int	 PositiveObliqueAngle;					//正向斜行角度
	unsigned short int	 HorizontalObliqueAngle;				//横向斜行角度	
	unsigned short int	 PositiveMinimumRadius;					//正向最小半径mm
	unsigned short int	 HorizontalMinimumRadius;				//横向最小半径mm
	unsigned short int	 PositiveOverlimit;						//正转角度超限值
	unsigned short int	 ReverseOverlimit;						//反转角度超限值
	unsigned short int	 PositiveMaxiControlAngle;				//正向最大控制角度
	unsigned short int	 ReverseMaxiControlAngle;				//反向最大控制角度
	unsigned short int	 GearTrainType;							//轮系样式	
	unsigned short int	 WheelDiameter;							//轮子直径
	unsigned short int	 ReductionRatio;						//减速比0.01	
	
}Screen_WheelParameter_TypeDef;

//33高度标定界面
typedef struct
{
	unsigned short int Height_Set_LowValue;                 //高度低点值标定
	unsigned short int Height_Set_HighValue;                //高度高点值标定
	unsigned short int Height_Limit_LowValue;               //高度低点限位值标定
	unsigned short int Height_Limit_HighValue;              //高度低点限位值标定
	unsigned       int Height_LowValue_Calibration[LIFT_HG_MAX];//高度传感器低点值标定
	unsigned       int Height_HighValue_Calibration[LIFT_HG_MAX];//高度传感器高点值标定
	u16 SpeedPlanUp[GEARS_NUM_MAX];							//上升速度挡位 0.1prm 1mv
	u16 SpeedPlanDown[GEARS_NUM_MAX];						//下降速度挡位 0.1prm 1mv
	unsigned short int   HMI_NOOROFF;							//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
}Screen_HeightInstall_Typedef;

//34角度0标定页
typedef struct
{
	s16 		AngleSet[Para_WheelAngle_MAXNUM];                    //0-11标定
	short int  	ActualAngle[HMI_AngleSensor_MaxNum];	         //实际角度
	u16 		SetButton;											//12失能按钮
	u16 		ModuleNumber; 										//模组号
	u16 		SpeedSetting;										//速度设定
	u16 		CurrentSpeed;										//当前速度
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示	
	u16			CalibrateButton;							//标定按钮
	EEP_Ctrl_TypeDef EEPCtrl;								//0 EEPROM控制
	short int  	ActualAngle1[HMI_AngleSensor_MaxNum];	         //实际角度(标定后)
}Screen_AngleSet_Typedef;

//35激光测距标定
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示	
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_LDRCalibration_TypeDef;

//36通用功能控制按钮枚举与定义
typedef union __attribute__((packed))
{
	struct __attribute__((packed))//控件Bit类型，每个Bit对应一种功能按钮
	{
		//第一个DHR
		unsigned short int ForwardRadarEnable_Bit0 : 1;     //前激光避障雷达开启与关闭    1：关闭  0：开启
		unsigned short int BackRadarEnable_Bit1 : 1;        //后激光避障雷达开启与关闭    1：关闭  0：开启
		unsigned short int LeftRadarEnable_Bit2 : 1;        //左激光避障雷达开启与关闭    1：关闭  0：开启
		unsigned short int RightRadarEnable_Bit3 : 1;       //右激光避障雷达开启与关闭    1：关闭  0：开启
		unsigned short int Forward_MoveLimtEnable_Bit0 : 1;     			//
		unsigned short int Back_MoveLimtEnable_Bit1 : 1;        			//
		unsigned short int DspModeSel : 1;        				//
		unsigned short int Electromagnet : 1;        				//电磁铁开关 
		unsigned short int Bit8 : 1;        				//
		unsigned short int Bit9 : 1;        				//
		unsigned short int Bit10 : 1;        				//
		unsigned short int Bit11 : 1; 						//
		unsigned short int Bit12 : 1; 						//
		unsigned short int Bit13 : 1; 						//
		unsigned short int Bit14 : 1; 						//
		unsigned short int ManufacturerHide_Bit15 : 1; 		//厂家调试按钮不可见      
	}Bit;
	unsigned short int All;
}Screen_ButtonEnable_List;

//避障雷达与触边可保存参数设置界面
typedef struct
{
	struct __attribute__((packed))//控件Bit类型，每个Bit对应一种功能按钮
	{
		//第一个DHR
		u16 RadarEnF : 1;									//前激光避障雷达开启与关闭    1：关闭  0：开启
		u16 RadarEnB : 1;									//
		u16 RadarEnL : 1;									//
		u16 RadarEnR : 1;									//
		u16 CrashEnF : 1;									//前触边
		u16 CrashEnB : 1;									//
		u16 CrashEnL : 1;									//
		u16 CrashEnR : 1;									//
		
	}Bit;
	unsigned short int All;
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示
}Screen_ParaObs;

//避障雷达与触边不可保存参数设置界面
typedef union __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示
}Screen_NOParaObs;

//37功能强制按钮枚举与定义
typedef union __attribute__((packed))
{
	struct __attribute__((packed))
	{
		unsigned short int Bit0 : 1; 						//系统参数保存
		unsigned short int SysReboot_Bit1 : 1; 				//系统重启复位
		unsigned short int LowValueSet_Bit2 : 1;			//升降低点值标定           
		unsigned short int HighValueSet_Bit3 : 1;			//升降高点值标定 
		unsigned short int FreeUpDownEnable_Bit4 : 1;       //自由升降开启与关闭         1：开启  0：关闭
		unsigned short int UpDownTimeoutEnable_Bit5 : 1;    //升降超时取消开启与关闭     1：开启  0：关闭
		unsigned short int FreeAngleEnable_Bit6 : 1;        //轮系角度自由开启与关闭     1：开启  0：关闭
		unsigned short int CarshReset_Bit7 : 1;				//触边碰撞复位               1：复位  0：关闭
		unsigned short int LeadShineFoceFlag_Bit8 : 1; 		//雷赛伺服复位按钮
		unsigned short int AngleZeroSet_Bit9 : 1; 			//角度零点标定
		unsigned short int ForceOverErr_Bit10 : 1; 			//强制忽略所有故障
		unsigned short int Bit11 : 1; 						//预留复位按钮
		unsigned short int Bit12 : 1; 						//预留复位按钮
		unsigned short int Bit13 : 1; 						//预留复位按钮
		unsigned short int Bit14 : 1; 						//预留复位按钮
		unsigned short int Bit15 : 1; 						//预留复位按钮
	}Bit;
	unsigned short int All;
}ButtonForce_List;

//38雷赛伺服标定
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示	
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_ReinforcedCalibration_TypeDef;

//39 PGV标定
typedef struct __attribute__((packed))
{
	u16 a;
	u16 b;
	u16 c;
	u16 d;
	u16 e;
	u16 f;
	u16 g;
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示	
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_PGVCalibration_TypeDef;

//40 里程计标定
typedef struct __attribute__((packed))
{
	u16 a;
	u16 b;
	u16 c;
	u16 d;
	u16 e;
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示	
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
							//屏幕组件宏  1为打开屏幕显示					// 
}Screen_OdometerCalibration_TypeDef;

//41 控制系数
typedef struct __attribute__((packed))
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
	u16 Electromagnet; 										//电磁铁
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示
			
}Screen_ControlCoeff_TypeDef;

//42 PID标定
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示	
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_PID_TypeDef;

//43 位置雷达标定
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示	
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_PositionRadar_TypeDef;

//44 位置雷达速度标定
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示	
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_PositionRadarSpeed_TypeDef;

//45 电量电压关联参数
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_BVRelated_TypeDef;

//46 更多参数
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
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
	
}Screen_MoreParameters_TypeDef;

//47 角度数据信息显示
typedef struct
{
	//short int  OriginlAngle[HMI_AngleSensor_MaxNum];            //原始换算角度
	short int  ActualAngle[HMI_AngleSensor_MaxNum];	            //实际角度（标定后）
	short int  TgtAngle[HMI_AngleSensor_MaxNum];	            //目标角度
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
}Screen_Angle_Typedef;

//48 升降高度
typedef struct
{
	unsigned short int High_Data[12];
	int CoreX;
	int CoreY;
	unsigned short int Weight;
	unsigned short int FL_cal;
	unsigned short int RL_cal;
	unsigned short int B_cal;
	unsigned short int tempeature;
	
    //特殊位参数
    union 
    {
        struct
        {
            u16 Tst_Temp_LF_UP_HMI : 1;							// 左前上升
            u16 Tst_Temp_RF_UP_HMI : 1;				            // 右前上升
            u16 Tst_Temp_B_UP_HMI  : 1;				            // 后侧上升
			u16 Tst_Temp_LF_Down_HMI : 1;							// 左前下降
			u16 Tst_Temp_RF_Down_HMI : 1;				            // 右前下降
			u16 Tst_Temp_B_Down_HMI : 1;				            // 后侧下降
        }Bits;
        u16 All;
    }Bit_Para;	
	
	u16 HMI_Temp_Rpm_Input;	//点动转速输入
	
	unsigned short int a;
	unsigned short int b;
	unsigned short int c;
	unsigned short int   HMI_NOOROFF;						//0显示压力 1压力，流量比，负载，中心坐标，2压力换成正三角，3压力换成四点液压 
	unsigned short int   HMI_NOOROFF1;						//0显示单动按钮，温度，补偿转速，1关闭显示，2按钮换成正三角	
}Screen_High_Typedef;

//49 悬挂高度
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
						//屏幕组件宏  1为打开屏幕显示				// 
}Screen_HangingHeight_TypeDef;

//50 支腿高度
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
							//屏幕组件宏  1为打开屏幕显示					// 
}Screen_LegHeight_TypeDef;

//51 激光测距
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
	unsigned short int	 LaserFront[2];		//激光测距前
	unsigned short int	 LaserBack[2];		//激光测距后
	unsigned short int   LaserLeft[2];		//激光测距左
	unsigned short int   LaserRight[2];		//激光测距右
					
}Screen_LaserRanging_TypeDef;

//52 超声波测距
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
	unsigned short int ultrasonic_Data[12];				//超声波测距
	
}Screen_USRanging_TypeDef;

//53 压力传感器
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  0：打开屏幕显示 1：隐藏组件
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
							//屏幕组件宏  1为打开屏幕显示				// 
}Screen_Pressure_TypeDef;

//54 模拟量采集
typedef struct __attribute__((packed))
{
	unsigned short int ADDAData[8];			//模拟量采集模块
	unsigned short int ADDAData1[8];			//模拟量采集模块
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  0：打开屏幕显示 1：隐藏组件
}Screen_AnalogCollection_TypeDef;

//55 模拟量输出
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  0：打开屏幕显示 1：隐藏组件
	unsigned short int	 OutputData[8];				//输出数据					// 
}Screen_AnalogOut_TypeDef;

//56 GPS数据
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  0：打开屏幕显示 1：隐藏组件
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_GPS_TypeDef;

//57 激光导航
typedef struct __attribute__((packed))
{
	u16 NC[10]  ;                             //预留占位
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  0：打开屏幕显示 1：隐藏组件
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_LaserNavigation_TypeDef;

//58 调度系统信息
typedef struct
{
	unsigned short int AGV_ExecutionPathState;				//AGV 执行路径状态
	unsigned short int AGV_CurrentPathChargeDir;			//AGV 当前位置充电方向
	unsigned short int AGV_ContinuePathState;			    //AGV 继续路径状态
	unsigned short int AGV_IOControlState;					//AGV IO控制状态1
	unsigned short int AGV_EMGControl;					    //AGV 调度急停控制  1急停
	unsigned short int AGV_EMGControlRecovery;				//AGV 调度急停复位
	unsigned short int AGV_AntiCcollisionRecovery;			//AGV 防撞触边回复  
	unsigned short int AGV_TaskControlCode;				    //AGV 任务控制码
	unsigned short int Host_StartStatus;				    //AGV 调度系统第一次启动标志
	unsigned short int AGV_ChargingControl;					//AGV 充电控制 --- 1DHR  
	unsigned short int AGV_SecuritySensorControl;			//AGV 安全设备开关状态 --- 1DHR 
	unsigned short int AGV_ClearAllAlalrm;					//AGV 调度系统清除设备报警
	unsigned short int AGV_IODeviceControl;					//AGV  IO状态
	unsigned short int AGV_RevCmd;							//AGV  预留指令
	unsigned short int AGV_WayPointDownloadStatus;			//AGV  路径下载状态
	unsigned short int AGV_WayPointUpdateQuantify;			//AGV  路径更新数量
	
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示	
}DispatchSystemCmd_Typedef;

//59 调度运行时当前状态
typedef struct
{
	unsigned short int AGV_AtuoRunState;					//1AGV 当前运行状态
	unsigned short int AGV_AtuoRunStep;						//2AGV 当前运行步骤
	unsigned short int AGV_AtuoRunProcee;				    //3AGV 当前运行过程
	unsigned short int AGV_NowTag;							//4AGV 当前点位
	unsigned short int AGV_NowToward;					    //5AGV 当前朝向
	unsigned short int AGV_TargetToward;					//6AGV 目标朝向
	unsigned short int AGV_TargetCourse;					//7AGV 目标航向
	unsigned short int AGV_TargetTag;						//8AGV 目标点位
	unsigned short int AGV_TargetSpeed;					    //9AGV 目标速度
	unsigned short int AGV_SafeSwitchStatus;			    //10AGV 当前路径避障开关状态
	unsigned short int AGV_RadarArea;						//11AGV 路径避障区域

	u16 NC[9];                                             //12-20预留

	//GoodsQRData
	u16 GoodsQRData_Flag;                                   //21标志
	s16 GoodsQRData_x;                                      //22数据x mm
	s16 GoodsQRData_y;                                      //23数据y mm
	s16 GoodsQRData_z;                                      //24数据z 0.01°

	//MagParkQRData
	u16 MagParkQRData_Flag;                                 //25标志
	s16 MagParkQRData_x;                                    //26数据x mm
	s16 MagParkQRData_y;                                    //27数据y mm
	s16 MagParkQRData_z;                                    //28数据z 0.01°

	//AGVMagParkZeroOffset
	u16 AGVMagParkZeroOffset_Flag;                          //29标志
	s16 AGVMagParkZeroOffset_F;                             //30前磁条纠偏量
	s16 AGVMagParkZeroOffset_B;                             //31后磁条纠偏量
	s16 AGVMagParkZeroOffset_L;                             //32左磁条纠偏量
	s16 AGVMagParkZeroOffset_R;                             //33右磁条纠偏量

	//实时值
	u16 RealTimeValue_Flag;                                 //34标志
	s16 RealTimeValue_x;                                    //35数据x mm
	s16 RealTimeValue_y;                                    //36数据y mm
	s16 RealTimeValue_z;                                    //37数据z 0.01°
	
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示
}AtuoRunStatus_Typedef;

//60 避障雷达触发状态
typedef union
{
	struct
	{
		unsigned char ForwordRadar_Inner : 1;				//前雷达最内层状态     1：触发 0：未触发
		unsigned char ForwordRadar_Middle : 1;    			//前雷达中间层状态     1：触发 0：未触发
		unsigned char ForwordRadar_Out : 1;        			//前雷达最外层状态     1：触发 0：未触发
		unsigned char BackRadar_Inner : 1;					//后雷达最内层状态     1：触发 0：未触发			          
		unsigned char BackRadar_Middle : 1; 				//后雷达中间层状态     1：触发 0：未触发		
		unsigned char BackRadar_Out : 1;       				//后雷达最外层状态     1：触发 0：未触发	
		unsigned char LeftRadar_Inner : 1;					//左雷达最内层状态     1：触发 0：未触发			
		unsigned char LeftRadar_Middle : 1;    				//左雷达中间层状态     1：触发 0：未触发
		
		unsigned char LeftRadar_Out : 1;        			//左雷达最外层状态     1：触发 0：未触发
		unsigned char RightRadar_Inner : 1;					//右雷达最内层状态     1：触发 0：未触发			
		unsigned char RightRadar_Middle : 1; 				//右雷达中间层状态     1：触发 0：未触发
		unsigned char RightRadar_Out : 1; 					//右雷达最外层状态     1：触发 0：未触发
		unsigned char Radar5_Inner : 1;						//雷达5最内层状态      1：触发 0：未触发			
		unsigned char Radar5_Middle : 1;    				//雷达5中间层状态      1：触发 0：未触发
		unsigned char Radar5_Out : 1;        				//雷达5最外层状态      1：触发 0：未触发
		
		unsigned char Radar6_Inner : 1;						//雷达6最内层状态      1：触发 0：未触发			
		unsigned char Radar6_Middle : 1; 					//雷达6中间层状态      1：触发 0：未触发
		unsigned char Radar6_Out : 1; 						//雷达6最外层状态      1：触发 0：未触发
	}RadarStatus_Bit;
	unsigned short int RadarStatus[2];		   				//安排2个DHR32个BIT按钮用于强制功能开关
}RadarStatus_List;

//避障雷达状态显示
typedef struct
{
	unsigned short int RadarArea;			                //避障雷达选择区域
	unsigned short int RadarArea_Force;			            //避障雷达强制区域
	RadarStatus_List RadarStatus;                           //避障雷达IO区域
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示	
}SafeSenorStatus_Typedef;


//61 磁导航状态
typedef struct
{
	unsigned  short int  MagneticSensor_Status;             //磁导航状态
	unsigned  short int  MagneticSensor_ReadNum;			//磁导航读取磁条数量
	short int  MagneticSensor_Location;			            //磁导航读取磁条位置
	short int  MagneticSensor_Strength;			            //磁条强度
}MagneticStripeStatus_Typedef;

//61 磁导航+RFID界面显示状态
typedef struct
{
	MagneticStripeStatus_Typedef  MagneticSensor_Data[4];	//4个磁导航状态数据
	unsigned  short int  RFID_WriteStatus;					//RFID写卡状态
	unsigned  short int  RFID_ReadStatus;					//RFID读卡状态
	unsigned  short int  RFID_SiganlStatus;					//RFID信号强度
	unsigned  short int  RFID_Data1;						//RFID第一字节数据
	unsigned  short int  RFID_Data2;						//RFID第二字节数据
	unsigned  short int  RFID_Data3;						//RFID第三字节数据
	unsigned  short int  RFID_Data4;						//RFID第四字节数据
	
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示	
}Screen_Navigation_Typedef;

//62 雷赛伺服状态
typedef struct __attribute__((packed))
{
	u16 NC;
	u16 NC1;
	u16 NC2;
	u16 NC3;
	u16 NC4;
	u16 NC5;
	u16 NC6;
	u16 NC7;
	u16 NC8;
	u16 NC9;
	u16 NCa;
	u16 NCb;
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  0：打开屏幕显示 1：隐藏组件
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_ReinforcedServo_TypeDef;

//PGV参数显示界面
//63 航发自制二维码传感器结构体----
typedef struct
{
	unsigned short int Run_Status;							//运行状态
	unsigned short int Fault_Code;							//错误码
	unsigned short int System_Ms;							//系统ms计数
	unsigned short int CodeRead_Status;						//识别二维码与色带状态
	unsigned short int Code_Value;							//二维码码值
	short int X_Offset;                    					//X轴偏差---
	short int Y_Offset;										//Y轴偏差----
	short int Z_Offset;										//Z轴偏差----
	short int Angle;
}PGVStatus_Typedef;

//航发自制二维码传感器结构体----
typedef struct
{
	PGVStatus_Typedef PGVSenorStatus[HMI_PGV_Num];
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示	
}Screen_PGV_Typedef;

//64 栈板相机
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_Camera_TypeDef;

//65 联动状态结构体
typedef struct
{
unsigned short int   HMI_NOOROFF;

}Screen_UnionStatus_Typedef;

//66 光斑状态
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
}Screen_LightSpot_TypeDef;

//67 位置定位雷达
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_Position_TypeDef;

//68 倾角传感器
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_TiltSensor_TypeDef;

//69 手摇脉冲器
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;		//屏幕组件宏  1：打开屏幕显示 0：隐藏组件
	unsigned short int	 test;				//测试写入是否正常，实际使用删掉
	
}Screen_Pulser_TypeDef;

//70 自制锂电池结构体转接板结构体----可适配无转接板读取
typedef struct
{
		
	u16 RedState;											// 读取状态
	u16 BeatCont;											// 心跳计数
	s16 Voltage;											// 电压 0.1v
	s16 Current;											// 电流 0.1A,正为充电，负为放电
	u16 SOC;												// SOC 0.1 %
	u16 SOH;												// SOH	
	s16 HOT_Current;										// 电池热电流 0.1A
	u16 Alarm_Code;												// 报警
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
	u32 TCC;												// 充电累计容量
	u32 TDC;												// 放电累计容量
	u16 C_TOT;												// 电池组总容量
	u16 C_REM;												// 电池组剩余容量
	u32 ACC_T_DCHG;											// 累计总放电时间
	u32 ACC_T_CHG;											// 累计总充电时间
	u32 T_OP_TOT;											// 累计总使用时间
	u16 N_CYCLES;											// 累计充放电次数
	u16	CommOkCnt;											// 通信成功次数
	u16 TimeOutMs;											// 超时计数
	u16 Alarm_In_Bit1;										//
	u16 Alarm_In_Bit2;										//
	u16 Alarm_In_Bit3;										//
	u16 Alarm_In_Bit4;										// 	
	u16 Alarm_In_Bit1_Lock;										//报警bit1
	u16 Alarm_In_Bit2_Lock;										//报警bit2
	u16 Alarm_In_Bit3_Lock;										//报警bit3
	u16 Alarm_In_Bit4_Lock;										//报警bit4
	u16 Alarm_In_CATL;
	u16 Alarm_In_CATL_Lock;
	
	unsigned short int HMI_NOOROFF;
}Screen_BMS_Typedef;

//hmi通用页面功能 位域定义
typedef struct __attribute__((packed)) _HMIButton_TypeDef
{
	unsigned short ReadEn : 1;      	                    //0页面内容读取使能
	unsigned short Write : 1;      	                   	    //1写
	unsigned short Bit2 : 1;      	                        //2
	unsigned short Bit3 : 1;      	                        //3
	unsigned short Bit4 : 1;      	                        //4
	unsigned short Bit5 : 1;      	                        //5
	unsigned short Bit6 : 1;      	                        //6
	unsigned short Bit7 : 1;      	                        //7
	unsigned short Bit8 : 1;      	                        //8
	unsigned short Bit9 : 1;      	                		//9
	unsigned short Bit10 : 1;      	                    	//10
	unsigned short Bit11 : 1;      	                    	//11
	unsigned short Bit12 : 1;      	                        //12
	unsigned short Bit13 : 1;      	                        //13
	unsigned short Bit14 : 1;      	                        //14
	unsigned short Bit15 : 1;      	                        //15
}HMIButton_TypeDef;

//71 IO强制输出
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示		
	
}Screen_ForcedOut_TypeDef;

// 72 EEPROM界面
typedef struct __attribute__((packed))
{
	EEP_Ctrl_TypeDef EEPCtrl;								//0 EEPROM控制
	u32	WriteCnt;											//1-2 EE写入次数 0~1
	u32	WriteDate;											//3-4 最后一次写入时间:1512312359,15年12月31日23时59分
	u32	ProductSerial;										//5-6 产品序列号 4~5
	u32 CodeVersion;					                    //7-8 代码版本;
	u16	CRCSum;												//9 CRC校验和 299
	ACC_EqpID_EnumDef EqpID;								//10 设备编号	
	u16 NumOfParaUser;										//11 用户参数大小
	
}Screen_EEPROM_TypeDef;

//74 安川驱动器状态显示
typedef struct __attribute__((packed))
{
	u32 ANC_CPUError;										//全局错误标志
	u32 ANC_DrvError;										//轴错误标志
	s16 ANC_Active_Speed;									//速度反馈 0.1rpm
	s16 ANC_Torque;											//力矩反馈 千分比
	u16 ANC_ErrorCode;										//轴故障代码
	u32 ANC_EN;												//轴使能命令
	u32 ANC_Start;											//轴速度执行命令
	s16 ANC_VCmd;											//轴速度命令 0.1rpm
}ANCStatus_TypeDef;

//75 多倍通设定
typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;						//屏幕组件宏  0为打开屏幕显示
	unsigned short int   DBCOMSet;							//多倍通模式选择
	unsigned short int   Enable;							//使能按钮
	unsigned short int   Status;							//切换状态机
}DBCOMSet_TypeDef;

typedef struct __attribute__((packed))
{
	unsigned short int   HMI_NOOROFF;//屏幕组件宏  0为打开屏幕显示
	ANCStatus_TypeDef ANC[HMI_Servo_MaxNum];	//安川驱动器状态
}ANCStatusPage_TypeDef;

typedef enum
{
	HMIPage_Null											= 0,//无效页
	HMIPage_Start											= 20,//开始页面
	MainPage												= 21,//主页，与画面关联	
	AlarmPage												= 22,//当前报警页面		
	HistoryAlarmPage										= 23,//历史报警页面		
	SystemInfPage											= 24,//系统信息界面
	AnalogDataPage											= 25,//模拟量数据采集界面
	WirelessStatusPage_One									= 26,//无线遥控器1界面
	LinkStatusPage											= 27,//传感器通信状态
	IOStatusPage											= 28,//IO状态
	ServoStatusPage_One										= 29,//伺服状态页面1(伺服1-4）
	ManualControlPage										= 30,//手动控车页面
	ParaSetPage_One											= 31,//一般参数设置界面
	WheelParaPage_One										= 32,//轮系参数设置界面
	HeightParaSetPage_One									= 33,//高度参数设置
	HMIPage_AngleSetPage									= 34,//角度标定界面
	DistanceSetPage											= 35,//激光测距标定
	FunctionButtonPage										= 36,//功能按钮界面
	forceButtonPage											= 37,//强制功能按钮界面
	LeadShineSetPage										= 38,//雷赛伺服标定界面
	InstPage												= 39,//PGV标定界面
	OdomterParaPage											= 40,//里程计参数设置界面
	CtrlCoefParaPage										= 41,//控制系数参数
	PidSetPage												= 42,//PID标定界面
	RadarLocationPage										= 43,//位置雷达标定界面
	SpeedThresholdPage										= 44,//速度档位(行走速度，举升速度)
	VoltageParaPage											= 45,//电压与电量关联参数
	MoreParaPage											= 46,//更多参数界面
	RealTimeAnglePage										= 47,//实时角度显示
	LiftHeightPage											= 48,//升降高度显示
	HangHeightPage											= 49,//悬挂高度显示
	LegHeightPage											= 50,//支腿高度显示
	LaserRangePage											= 51,//激光测距长度
	UltrasonicDistancePage									= 52,//超声波测距信息
	PressuredDataPage										= 53,//压力传感器信息
	AnalogModulePage										= 54,//模拟量采集模块
	AnalogOutPutModulePage									= 55,//模拟量输出模块
	GpsDataPage												= 56,//GPS数据显示
	LaserNavigationPage										= 57,//激光导航数据显示
	DispatchingSystemPage									= 58,//调度系统数据
	AutoRunStatusPage										= 59,//自动运行状态
	SafeSensorStatusPage									= 60,//避障雷达
	RFIDDataPage											= 61,//磁导航数据(RFID+磁导航)
	LeadShineServoStatusPage								= 62,//雷赛伺服状态
	PGVDataPage												= 63,//PGV状态PGVDataPage									
	PalletCameraPage										= 64,//栈板相机
	UnionStatusPage											= 65,//联动状态
	LightSpotPage											= 66,//光斑状态
	PostionRadarPage										= 67,//位置定位雷达（R2000)
	InclinometerPage										= 68,//倾角传感器
	HandPulserPage											= 69,//手摇脉冲器
	BMSStatus												= 70,//BMS状态更新
	IoOutputForcePage										= 71,//IO输出强制页面
	EEPROMPage_One											= 72,//内部可保存参数（0-99)
	Electromagnet                 							= 73,//电磁铁标定
	ANCStatusPage											= 74,//安川驱动器状态显示
	DBCOMSetPage											= 75,//多倍通设定
	WirelessStatusPage_Two									= 160,//无线遥控器界面2
	//伺服故障说明码页面									172-175//被占用
	HeightParaSetPage_Two									= 180,//高度标定参数2
	HeightParaSetPage_Three									= 181,//高度标定参数3
	EEPROMPage_Two											= 200,//eeprom参数2
	EEPROMPage_Three										= 201,//eeprom参数3
	FunctionButtonSavePage									= 202,//功能开关 可保存
	HMIPage_IOTest											= 203,
	HMIPage_IOTest1											= 204,
	HMIPage_IOTest2											= 205,
	HMIPage_Para_Auto										= 206,//自动模式参数
	HMIPage_Para_SysFucBit									= 207,//系统功能位参数
	HMIPage_Path_Comm										= 208,//通信缓存路径信息
	HMIPage_Path_Stack										= 209,//下载链表路径信息
	HMIPage_MMU												= 210,//内存管理
	HMIPage_AtuoInf											= 211,//自动模式信息
	HMIPage_Button											= 212,//功能按钮
	HMIPage_SysCmd											= 213,//系统命令
	HMIPage_Manufacturer									= 220,//厂家调试
	HMIPage_Tmp_INOUT										= 221,//临时输入输出
	HMIPage_UnionEMG     									= 222,	//联动报警
	HMIPage_UnionParameter									= 223, //联动参数
	HMIPage_End												= 65535,//将被存储为u16
}HMIPage_EnumDef;

//208 路径点结构体定义 30个u16
typedef struct
{
	u16 WayPointSequence;									//122AGV 路径1位置序列号 --- 1DHR   		
	u16 WayPointLabelIndex;									//123AGV 路径1位置标签值 --- 1DHR   
	u16 WayPointXValue;										//124AGV 路径1位置x坐标值 --- 2DHR   
	u16 nc125;
	u16 WayPointYValue;										//126AGV 路径1位置y坐标值 --- 2DHR   
	u16 nc127;
	u16 WayPointZValue;										//128AGV 路径1位置z坐标值（朝向） --- 1DHR   																																								
	u16 WayPointPlatformHigh;								//129AGV 路径1位置升降高度 --- 1DHR   
	u16 WayPointWaitTime;									//130AGV 路径1位置等待时间 --- 1DHR  
	u16 WayPointCourse;										//131AGV 路径1位置航向 --- 1DHR  
	u16 WayPointSpeedPerAndLidarScanAreaSwitch;				//132AGV 路径1位置速度百分比/激光雷达区域设置 --- 1DHR   		
	u16 WayPointSecuritySensorControl;						//133AGV 路径1位置安全设备开关状态 --- 1DHR   
	u16 WayPointConfirmSignalAndManualSched;				//134AGV 路径1位置确认信号/手动调度标志/ --- 1DHR   
	u16 WayPointProperty;									//135AGV 路径1位置属性/方向/转弯/盲走标志/点位类型/十字路口 --- 1DHR  	
	u16 WayPointCurveRadius;								//136AGV 路径1位置曲线半径参数 --- 1DHR  	
	u16 WayPointCurveAngle;									//137AGV 路径1位置曲线角度参数 --- 1DHR  	
	u16 WayPointDoorFrameDis;								//138AGV 路径1位置门架移动距离 --- 1DHR  	
	u16 WayPointRetainOne;									//139AGV 路径1位置预留参数1 
	u16 WayPointRetainTwo;									//140AGV 路径1位置预留参数2 
	u16 WayPointRetainThree;								//141AGV 路径1位置预留参数3 
	u16 WayPointRetainFour;									//142AGV 路径1位置预留参数4 
	u16 WayPointRetainFive;									//143AGV 路径1位置预留参数5 		
	u16 WayPointRetainSix;									//144AGV 路径1位置预留参数6 
	u16 WayPointRetainSeven;								//145AGV 路径1位置预留参数7 
	u16 WayPointRetainEight;								//146AGV 路径1位置预留参数8 
	u16 WayPointRetainNine;									//147AGV 路径1位置预留参数9
	u16 WayPointRetainTen;									//148AGV 路径1位置预留参数10 
	u16 WayPointRetainEleven;								//149AGV 路径1位置预留参数11
	u16 WayPointRetainTwelve;								//150AGV 路径1位置预留参数12
	u16 WayPointRetainThirteen;								//151AGV 路径1位置预留参数13	
}PathPoint_StDef;

//208通信缓存路径信息
typedef struct
{
	//设备信息类
	u16 DeviceTypeCode;										//0AGV 设备类型码 --- 1DHR  
	u16 DriverSN;	  										//1AGV 设备序列号 --- 1DHR 	
	u16 vHARDVERSION;	  									//2AGV 硬件版本 --- 1DHR  
	u16 vSOFTVERSION;	  									//3AGV 软件版本 --- 1DHR  
	u16 nc4;	  											//4

	//系统信息类
	u16 RunState;											//5AGV 运行状态 --- 1DHR
	u16 ControlMode;										//6AGV 控制模式 --- 1DHR
	u16 RunMode;											//7AGV 运行模式 --- 1DHR
	u16 MovementState;										//8AGV 运动状态 --- 1DHR
	u16 BatteryVoltage;										//9AGV 电池电压 --- 1DHR 单位V，扩大10倍，如486 = 48.6V
	u16 BatteryLevel;										//10AGV 电池电量 --- 1DHR  百分比0 --- 100    

	u16 SystemFaultCodeLevel1_1;							//11AGV 系统一级故障码 1（一级故障1字节）--- 1DHR 
	u16 SystemFaultCodeLevel1_2;							//12AGV 系统一级故障码 2（一级故障2字节）--- 1DHR 	
	u16 SystemFaultCodeLevel1_3;							//13AGV 系统一级故障码 3（一级故障1字节）--- 1DHR 
	u16 SystemFaultCodeLevel1_4;							//14AGV 系统一级故障码 4（一级故障2字节）--- 1DHR 	

	u16 SystemFaultCodeLevel2_1;							//15AGV 系统二级故障码 1（二级故障1字节）--- 1DHR 
	u16 SystemFaultCodeLevel2_2;							//16AGV 系统二级故障码 2（二级故障2字节）--- 1DHR 	
	u16 SystemFaultCodeLevel2_3;							//17AGV 系统二级故障码 3（二级故障1字节）--- 1DHR 
	u16 SystemFaultCodeLevel2_4;							//18AGV 系统二级故障码 4（二级故障2字节）--- 1DHR 		

	u16 SystemFaultCodeLevel3_1;							//19AGV 系统三级故障码 1（三级故障1字节）--- 1DHR 
	u16 SystemFaultCodeLevel3_2;							//20AGV 系统三级故障码 2（三级故障2字节）--- 1DHR 	
	u16 SystemFaultCodeLevel3_3;							//21AGV 系统三级故障码 3（三级故障1字节）--- 1DHR 
	u16 SystemFaultCodeLevel3_4;							//22AGV 系统三级故障码 4（三级故障2字节）--- 1DHR 				

	u16 SecuritySensorState1;								//23AGV 地盘避障状态 --- 1DHR    
	u16 SecuritySensorState2;								//24AGV 其他避障状态 --- 1DHR   	
	u16 PowerSwitchStatus;									//25AGV 面板开关下电状态-- 1DHR   
	u16 MotionSwitchStatus;									//26AGV 运动开关状态（锁紧/释放开关） --- 1DHR
	u16 MatchCharge;										//27AGV 配套充电桩类型 --- 1DHR		
	u16 nc28;	  											//28
	u16 nc29;	  											//29

	//AGV运行状态信息类
	u16 RunningSpeed_Vx;									//30AGV 实时速度Vx --- 1DHR 1mm/s
	u16 RunningSpeed_Vy;									//31AGV 实时速度Vy --- 1DHR 1mm/s
	u16 RunningSpeed_Vz;									//32AGV 实时速度Vz --- 1DHR 0.01°/s，放大100倍   
	//u32 CurrentXValue;										//33AGV 当前位置X值 --- 2DHR 
	u16 CurrentXValue_1;									//33 当前位置X值低位
	u16 CurrentXValue_2;									//34 当前位置X值高位
	//u32 CurrentYValue;										//35AGV 当前位置Y值 --- 2DHR 
	u16 CurrentYValue_1;									//35 当前位置Y值低位
	u16 CurrentYValue_2;									//36 当前位置Y值高位
	u16 CurrentZValue;										//37AGV 当前位置Z值（当前朝向） --- 1DHR  单位°，放大100倍
	u16 CurrentQRCode;										//38AGV 当前路径标签值 --- 1DHR 
	u16 CurrentPathHeading;									//39AGV 当前位置航向 --- 1DHR  单位°，放大100倍  
	u16 LiftingPlatformHigh;								//40AGV 升降高度 --- 1DHR  单位mm

	//1~8号电机转速
	//u16 MotorSpeed[8];										//41AGV 1#电机转速 --- 1DHR    		
															//~48AGV 8#电机转速 --- 1DHR	
	u16 MotorSpeed1;										//41 1号电机转速
	u16 MotorSpeed2;										//42 2号电机转速
	u16 MotorSpeed3;										//43 3号电机转速
	u16 MotorSpeed4;										//44 4号电机转速
	u16 MotorSpeed5;										//45 5号电机转速
	u16 MotorSpeed6;										//46 6号电机转速
	u16 MotorSpeed7;										//47 7号电机转速
	u16 MotorSpeed8;										//48 8号电机转速

	//u16 MotorEncoder[16];									//49AGV 1#电机编码器脉冲数 --- 2DHR		
															//~64AGV 8#电机编码器脉冲数 --- 2DHR
	u16 MotorEncoder1_1;									//49 1号电机位置低位
	u16 MotorEncoder1_2;									//50 1号电机位置高位
	u16 MotorEncoder2_1;									//51 2号电机位置低位
	u16 MotorEncoder2_2;									//52 2号电机位置高位
	u16 MotorEncoder3_1;									//53 3号电机位置低位
	u16 MotorEncoder3_2;									//54 3号电机位置高位
	u16 MotorEncoder4_1;									//55 4号电机位置低位
	u16 MotorEncoder4_2;									//56 4号电机位置高位
	u16 MotorEncoder5_1;									//57 5号电机位置低位
	u16 MotorEncoder5_2;									//58 5号电机位置高位
	u16 MotorEncoder6_1;									//59 6号电机位置低位
	u16 MotorEncoder6_2;									//60 6号电机位置高位
	u16 MotorEncoder7_1;									//61 7号电机位置低位
	u16 MotorEncoder7_2;									//62 7号电机位置高位
	u16 MotorEncoder8_1;									//63 8号电机位置低位
	u16 MotorEncoder8_2;									//64 8号电机位置高位

	//u16 MotorDriverErr[16];									//65AGV 1#电机驱动器故障代码 --- 2DHR		
															//~80AGV 8#电机驱动器故障代码 --- 2DHR	
	u16 MotorDriverErr1_1;									//65 1号电机驱动器故障代码低位
	u16 MotorDriverErr1_2;									//66 1号电机驱动器故障代码高位
	u16 MotorDriverErr2_1;									//67 2号电机驱动器故障代码低位
	u16 MotorDriverErr2_2;									//68 2号电机驱动器故障代码高位
	u16 MotorDriverErr3_1;									//69 3号电机驱动器故障代码低位
	u16 MotorDriverErr3_2;									//70 3号电机驱动器故障代码高位
	u16 MotorDriverErr4_1;									//71 4号电机驱动器故障代码低位
	u16 MotorDriverErr4_2;									//72 4号电机驱动器故障代码高位
	u16 MotorDriverErr5_1;									//73 5号电机驱动器故障代码低位
	u16 MotorDriverErr5_2;									//74 5号电机驱动器故障代码高位
	u16 MotorDriverErr6_1;									//75 6号电机驱动器故障代码低位
	u16 MotorDriverErr6_2;									//76 6号电机驱动器故障代码高位
	u16 MotorDriverErr7_1;									//77 7号电机驱动器故障代码低位
	u16 MotorDriverErr7_2;									//78 7号电机驱动器故障代码高位
	u16 MotorDriverErr8_1;									//79 8号电机驱动器故障代码低位
	u16 MotorDriverErr8_2;									//80 8号电机驱动器故障代码高位

	u16 WheelAngle[4];										//81AGV 1#模组角度,0.01° --- 1DHR
	//~84AGV 4#模组角度,0.01° --- 1DHR

	u16 MagneticData_F;										//85AGV 前磁导航 (现对于传感器中心),1mm --- 1DHR
	u16 MagneticData_B;										//86AGV 后磁导航 (现对于传感器中心),1mm --- 1DHR
	u16 MagneticData_L;										//87AGV 左磁导航 (现对于传感器中心),1mm --- 1DHR
	u16 MagneticData_R;										//88AGV 右磁导航 (现对于传感器中心),1mm --- 1DHR																																								
	u16 RFID_Value;											//89AGV RFID标签数据,0~65535 --- 1DHR																																								
	u16 QRCodeData;											//90AGV 二维码值 --- 1DHR 
	u16 QRCodeData_x;										//91AGV 二维码x角度,1mm --- 1DHR 
	u16 QRCodeData_y;										//92AGV 二维码y角度,1mm --- 1DHR 
	u16 QRCodeData_angle;									//93AGV 二维码角度,0.1° --- 1DHR 																																								
	u16 IOSensorState;										//94AGV IO传感器状态 --- 1DHR 																																										
	u16 BatTotalCurrent;									//95AGV 电池总电流 --- 1DHR 		
	u16 BatHealthStatus;									//96AGV 电池健康状态 --- 1DHR 	
	u16 cn97;	  											//97
	u16 cn98;	  											//98
	u16 cn99;	  											//99
	u16 cn100;	  											//100																																							
	u16 NavigationStatusFeedback;							//101AGV 定位导航反馈状态
	u16 nc102;	  											//102
	u16 nc103;	  											//103
	u16 nc104;	  											//104
	//设备控制判断类
	u16 ExecutionPathState;									//105AGV 执行路径状态 --- 1DHR
	u16 CurrentPathChargeDir;								//106AGV 当前位置充电方向	--- 1DHR			
	u16 ChargingState;										//107AGV 充电状态 --- 1DHR
	u16 ContinuePathState;									//108AGV 继续路径标志 --- 1DHR
	u16 nc109;	  											//109

	//设备控制判断类
	u16 EMGControl;											//110AGV 上位机急停控制 --- 1DHR   
	u16 EMGControlRecovery;									//111AGV 上位机急停恢复 --- 1DHR 
	u16 AntiCcollisionRecovery;								//112AGV 防撞触边恢复 --- 1DHR 			
	u16 TaskControlCode;									//113AGV 任务控制码 --- 1DHR 		
	u16 Host_StartStatus;									//114调度系统第一次启动标志 --- 1DHR 		
	u16 ChargingControl;									//115AGV 充电控制 --- 1DHR  
	u16 SecuritySensorControl;								//116AGV 安全设备开关状态 --- 1DHR  	
	u16 ClearAllAlalrm;										//117调度系统清除设备报警		
	u16 nc118;	  											//118	
	u16 TaskTypeNotify;										//119任务类型通知 --- 1DHR	0-无效	1-停靠任务	2-充电任务	3-转运任务	4-联动搬运任务

	//路径下载信息
	u16 WayPointDownloadStatus;								//120AGV 路径下载状态 --- 1DHR  
	u16 WayPointUpdateQuantify;								//121AGV 路径更新数量 --- 1DHR  	

	//路径1命令位置信息 每个路径占用30个DHR
	/*PathPoint_StDef PathPoint1;								//路径1命令位置信息 122~151
	PathPoint_StDef PathPoint2;								//路径2命令位置信息 152~181
	PathPoint_StDef PathPoint3;								//路径3命令位置信息 182~211
	PathPoint_StDef PathPoint4;								//路径4命令位置信息 212~241
	PathPoint_StDef PathPoint5;								//路径5命令位置信息 242~271
	PathPoint_StDef PathPoint6;								//路径6命令位置信息 272~301
	PathPoint_StDef PathPoint7;								//路径7命令位置信息 302~331
	PathPoint_StDef PathPoint8;								//路径8命令位置信息 332~361
	PathPoint_StDef PathPoint9;								//路径9命令位置信息 362~391
	PathPoint_StDef PathPoint10;							//路径10命令位置信息 392~421
	*/
	PathPoint_StDef PathPoint[10];							//路径1~10命令位置信息 122~421
	//u16 ObjSize;
}DSPCOMMOBJ_StDef;

//209 位控制1
typedef struct
{
	unsigned short ForRadioEnalbe : 1;      	            //0前激光雷达使能
	unsigned short BackRadioEnalbe : 1;      	            //1
	unsigned short LeftRadioEnalbe : 1;      	            //2
	unsigned short RightRadioEnalbe : 1;      	            //3
	unsigned short ForCollisionEnable : 1;      	        //4前防撞触边使能
	unsigned short BackCollisionEnable : 1;      	        //5
	unsigned short LeftCollisionEnable : 1;      	        //6
	unsigned short RightCollisionEnable : 1;      	        //7
	unsigned short BevelRadioEnalbe : 1;      	            //8斜面激光雷达使能
	unsigned short ColumnRadioEnalbe : 1;      	            //9立柱激光雷达使能
	unsigned short ConfirmSignal : 1;      	                //10路径点确认信号
	unsigned short ManualScheduling : 1;      	            //11路径点手动调度信号
	unsigned short BlindWalk : 1;      	                    //12盲走信号
	unsigned short StackCameraData : 1;      	            //13栈板相机识别数据使用
	unsigned short DirectionSign : 1;      	                //14方向标志 False.前进点    True.后退点
	unsigned short NavigationModeSwitch : 1;      	        //15导航类型切换 False.无效   True.导航类型切换
}Path_CmdBit1;

//209 位控制2
typedef struct
{
	unsigned short VirtualPointSta : 1;      	            //0虚拟点位标志（0-实体点	1-虚拟点）
	unsigned short IntersectionSta : 1;      	            //1十字路口点位标志	（0-否,1-是）

}Path_CmdBit2;

//209 链表路径信息
typedef struct _Screen_Path_Typedef
{
	u16 PathSequence;										//0 路径点下发序列号（标明该路径点在该任务中的顺序号）
	u16	Label;												//1 路径点标签值
	u16	LiftHigh;											//2 路径点平台升降高度
	u16	WaitTime;											//3 等待时间 上位机下发单位为s，路径填充时，转换为ms
	u16	DoorFrameDistance;									//4 门架移动距离
	u16	CurveParameters_Radius;								//5 路径曲线半径
	u16	Tray_RFID_TAG;										//6 下发的托盘RFID号
	u16	WishboneSpacing;									//7 叉臂开合距离

	u16	Map_x_Index;    									//8 路径点 坐标 X 导航激光雷达
    u16	Map_x;                                              //9
	u16	Map_y_Index;  										//10 路径点 坐标 Y 
    u16	Map_y;                                              //11        
	s16	Towards;											//12 路径点 车体位置方向角度 
	s16	Course;												//13 路径点 航向
	s16	CurveParameters_Angle;								//14 路径曲线角度         

	u16 SpeedPercentage;									//15 速度百分比
	u16	LidarScanAreaSelect;								//16 扫描区域切换

	u16 PathProperty;										//17 该段路径属性
	u16 TurnProperty;										//18 该段路径转弯属性
	u16 NavigationMode;										//19 该路径导航模式

    Path_CmdBit1 CmdBit1;									//20位控制1
	Path_CmdBit2 CmdBit2;									//21位控制2

    u16 NC[3];                                              //22-24
	//struct   PathTypeDef* Pnext;							// 下一节点信息 
}Screen_Path_Typedef;

//210 内存控制页对象定义
typedef struct
{
	u32 Address;	                                		//0-1目标地址
	u16 nc[3];												//2-4保留
	u16 R_Byte0;											//5读0
	u16 R_Byte1;											//6读1
    u16 R_Byte2;											//7读2
    u16 R_Byte3;											//8读3
    u16 W_Byte0;											//9写0
    u16 W_Byte1;											//10写1
    u16 W_Byte2;											//11写2
    u16 W_Byte3;											//12写3
}Screen_MMU_Typedef;

//211 自动控制命令位
typedef struct __attribute__((packed))
{
	unsigned short LiftUp : 1;      						//0上升
	unsigned short LiftDown : 1;      						//1下降
	unsigned short ChargingMC : 1;      					//2充电MC
	unsigned short b3 : 1;      							//3
	unsigned short b4 : 1;      							//4
	unsigned short b5 : 1;      							//5
	unsigned short b6 : 1;      							//6
	unsigned short b7 : 1;      							//7
	unsigned short b8 : 1;      							//8
	unsigned short b9 : 1;      							//9
	unsigned short b10 : 1;      							//10
	unsigned short b11 : 1;      							//11
	unsigned short b12 : 1;      							//12
	unsigned short b13 : 1;      							//13
	unsigned short b14 : 1;      							//14
	unsigned short b15 : 1;      							//15
}AtuoInf_CmdBit;

//211 更多自动信息
typedef struct __attribute__((packed))
{
	//里程计信息
	s32 Distance;											//0-1当前的行走距离
	s32	RTx;												//2-3实时坐标数据
	s32 RTy;												//4-5
	s32	RTz;												//6-7
	s32 RTxy;												//8-9实时坐标数据
	s32 CurrentPathDelta;									//10-11当前路径点与下一路径点的距离
	s32 PointToStart_Dis;									//12-13实时运动距离
	s16 TargetPostureAngle;									//14目标姿态旋转角度
	s16 ActualPostureAngle;									//15实际姿态旋转角度
	u16 TargetAGVWorkMode;									//16目标工作模式
	s16 Hx;													//17摇杆命令x
	s16 Hy;													//18摇杆命令y
	s16 Hz;													//19摇杆命令z
	u16 WalkGear;											//20行走挡位
	u16 LiftGear;											//21举升挡位
	AtuoInf_CmdBit CmdBit;									//22控制命令位
}Screen_AtuoInf_Typedef;

//213 系统控制命令 对象定义
typedef struct
{
	ACCCC_TypeDef Channel;									//0通道
	ACCMode_TypeDef Mode;									//1模式
	s16 Vx;													//2摇杆命令x
	s16 Vy;													//3摇杆命令y
	s16 Vz;													//4摇杆命令z
	u16 WalkGear;											//5行走挡位Z[1,6]
	u16 LiftGear;											//6升降挡位Z[1,6]
	AccObsArea_TypeDef ObsArea;								//7避障区域
	//8避障开关
	struct
	{
		u16 RadarF : 1;      								//0激光雷达前
		u16 RadarB : 1;      								//1
		u16 RadarL : 1;      								//2
		u16 RadarR : 1;      								//3
		u16 BarF : 1;      									//4防撞触边前
		u16 BarB : 1;      									//5
		u16 BarL : 1;      									//6
		u16 BarR : 1;      									//7
		u16 b8 : 1;      									//8
		u16 b9 : 1;      									//9
		u16 b10 : 1;      									//10
		u16 b11 : 1;      									//11
		u16 b12 : 1;      									//12
		u16 b13 : 1;      									//13
		u16 b14 : 1;      									//14
		u16 b15 : 1;      									//15
	}ObsEnBit;

	//9命令按钮
	struct
	{
		u16 LiftUp : 1;      								//0上升
		u16 LiftDown : 1;      								//1下降
		u16 Horn : 1;      									//2喇叭
		u16 Lighting : 1;      								//3照明
		u16 EStop : 1;      								//4急停
		u16 ChargingMC : 1;      							//5充电
		u16 b6 : 1;      									//6
		u16 b7 : 1;      									//7
		u16 b8 : 1;      									//8
		u16 b9 : 1;      									//9
		u16 b10 : 1;      									//10
		u16 b11 : 1;      									//11
		u16 b12 : 1;      									//12
		u16 b13 : 1;      									//13
		u16 b14 : 1;      									//14
		u16 b15 : 1;      									//15
	}ButtonBit;
	
}Screen_SysCmd_Typedef;

//220 调试按钮
typedef struct 
{
	unsigned short WROverAuto : 1;      					//0遥控强制自动
	unsigned short b1 : 1;      							//1
	unsigned short b2 : 1;      							//2
	unsigned short b3 : 1;      							//3
	unsigned short b4 : 1;      							//4
	unsigned short b5 : 1;      							//5
	unsigned short b6 : 1;      							//6
	unsigned short b7 : 1;      							//7
	unsigned short b8 : 1;      							//8
	unsigned short b9 : 1;      							//9
	unsigned short b10 : 1;      							//10
	unsigned short b11 : 1;      							//11
	unsigned short b12 : 1;      							//12
	unsigned short b13 : 1;      							//13
	unsigned short b14 : 1;      							//14
	unsigned short b15 : 1;      							//15
}Button_DebugBit;

//按钮页
typedef struct
{
	Button_DebugBit DebugBit;								//0调试按钮
}Screen_Button_Typedef;

//222 联动报警
typedef struct
{
	u16 Faults;						    					//故障类型
	
	u32 CarPanelSelErr:1;									//车端单联动切换异常
	u32 CarPanelSelErr_Othre : 1;							//对方车未选择联动
    u32 CantWired:1;										//联动不能有线
    u32 HgDiffOverlimit:1;									//高度差过大

    //进联动时
	u32 LandscapeOverlimit_Enter:1;							//光斑偏移过大
	u32 D_LandR_Overlimit_Enter:1;							//左右差过大
    u32 LaserRangingToofar_Enter:1;							//联动测距过大
    u32 LaserRangingToonear_Enter:1;						//联动测距过小

    //联动过程中
  	u32 LandscapeLaserOverlimit:1;							//光斑偏移过大
    u32 D_LandR_OverlimitMax:1;								//联动时 左右差过大
	u32 LaserRangingChange:1;								//测距变化过大
	u32 MoveSynErr:1;										//动作同步异常
	
	u32 PointErr:1;											//存在空指针
	u32 WheelRMxaErr:1;										//单车最大轮系运动半径过大
	u32 WheelVMxaErr:1;										//最大轮系速度过大
	u32 AsbEnterErr : 1;									//进入联动条件不满足
	u32 CheckErr : 1;										//校验异常   

	u16 UpLimited : 1;						            //限制上升
	u16 DownLimited : 1;						        //限制下降
	u16 MoveLimited : 1;						        //限制行走
	
}Screen_UnionEMG_Typedef;

//223 联动参数
typedef struct
{
	//角色配置
	AsbCarStrNo_TypeDef Iam;								//编号
	AsbRole_TypeDef Role;									//角色
	//尺寸参数
	u16	 HalfCar;											//车身半长(目前仅约定联动距离通过联动测距计算 非两车中心距离)
	//阵型配置
	AsbSiteNo_TypeDef SiteNo;								//阵型编号
	AsbSiteSeat_TypeDef Seat;								//阵型位置

	//调整参数
	u16 K_HgAdj;											//升降高度调试系数
	u16 K_DisAdj;											//行走距离调试系数

	//报警参数
	u16 HgDiffMax;											//
	AsbSensorAlarmPara_TypeDef SensorAlarm;					//传感器报警参数
	u16 SynTimeout;											//运动同步超时时间 ms

	//功能开关
	struct
	{
		u16 TrackOrg : 1;									//0主车汇总广播 1主车直接广播自己的
		u16 RadarObsEn_Single : 1;							//0主车广播统一的 1主车单独设定不同从车 目前无效
		u16 ObsAreaSel_Single : 1;							//0主车广播统一的 1主车单独设定不同从车 目前无效
	}FuncSwitch;

	u16	NC;													//占位
	u16	WHA_SN;												//联动遥控器编号，可以在运行过程中修改

	u16 AsbSelect;											//选择联动模式 模拟车体面板旋钮 0单动1联动
	u16 AsbDis;												//联动距离 模拟传感器检测到的输入 mm

	u16	temp[3];											//用于调试的时候观察越界情况
	
	u16	Car_SN;												//从车型号选择
	
}Screen_UnionParameter_Typedef;


typedef struct __attribute__((packed)) _HMI_CommObj_TypeDef //实测有效
//typedef struct _HMI_CommObj_TypeDef
{
	//常读：当前画面号、翻页按钮、画面控制按钮、系统功能按钮
	//常写：控制画面号、画面内容偏移地址、画面控制命令、实时信息、故障信息
	u16 SubIndex;                                           //0画面子索引
	HMIPage_EnumDef PageNum;                               	//1当前ID，切换时写一次
	Screen_ButtonEnable_List ButtonEnable;                  //2功能按钮
	ButtonForce_List ButtonForce;							//3强制按钮
	HMIButton_TypeDef FucBit;                               //4通用页面功能
	ACC_FaultsHmiErr1_UnionDef EMG_Evet1;					//5 6 7 8非受控停车32 + 自动报警32
	ACC_ControlledStop_UnionDef EMG_Evet2;                  //9 10 11 12受控停车++
	ACC_Remind_UnionDef EMG_Remind;                         //13 14 15 16运动限制32 + 仅提醒32
	u16 NC17;                                            	//17 设备编号
	u16 InitDirection;										//18上线朝向
	HMIPage_EnumDef PageCmd;                                //19画面命令 响应后会被清零

	u16 Screen_Head[128];                                   //20-147界面内容

	u16 U16_148[2];                                         //148-149
}HMI_CommObj_TypeDef;

//对象定义
typedef struct
{
	HMIPage_EnumDef ScreenID;								//当前界面ID
	HMIPage_EnumDef LastPageNum;							//最后页面ID
	u16 PageInitialized;									//页面初始化标志 wxb@20240712 add
	HMI_CommObj_TypeDef* p2HMI_CommObj;				        //HMI通信对象
    unsigned short* p2Data;                                 //界面数据指针  
	
	Screen_ParaObs* p2Scr_ParaObs;
	Screen_HMIPage_Start* p2Scr_Start;
	Screen_Main_Typedef* p2Scr_Main;						//21主界面状态对象指针
	SystemInformation_Typedef* p2Scr_Sys;					//24系统信息
	Screen_AdOnboard_Typedef* p2Scr_AdOnboard;				//25板载模拟量
	WirelessStatus_Typedef* p2Scr_Wireless;					//26遥控器数据
	Screen_LinkStaus_Typedef* p2Scr_LinkStaus;				//27通信状态计数
//	Screen_IOStatus_TypeDef* p2Scr_IOStatus;				//28IO状态
	CanopenStatus_Typedef* p2Scr_CanopenStatus;				//29canopen信息
	Screen_ManualControl_Typedef* p2Scr_ManualControl;		//30手动控车界面对象指针
	Screen_GeneralParameter_TypeDef* p2Scr_GeneralParameter; //31一般参数
	Screen_WheelParameter_TypeDef* p2Scr_WheelPParameter;	//32轮系参数
	Screen_HeightInstall_Typedef* p2Scr_HeightInstall;      //33高度标定界面对象指针
	Screen_AngleSet_Typedef* p2Scr_AngleSet;                //34角度0标定界面对象指针
	Screen_LDRCalibration_TypeDef* pScr_LDRCalibration;			//35激光测距标定
	Screen_NOParaObs* pScr_NOParaObs;             				//36功能开关不可保存
	Screen_ReinforcedCalibration_TypeDef*p2Scr_ReinforcedCalibration;	//38雷赛伺服标定	
	Screen_PGVCalibration_TypeDef* p2Scr_PGVCalibration_TypeDef;	//39PGV标定
	Screen_OdometerCalibration_TypeDef* p2Scr_Odometer;		//40里程计
	
	Screen_ControlCoeff_TypeDef*p2Scr_ControlCoeff;			//41控制系数
	Screen_PID_TypeDef*p2Scr_PID;							//42PID
	Screen_PositionRadar_TypeDef*p2Scr_PositionRadar;      	//43位置雷达标定
	Screen_PositionRadarSpeed_TypeDef*p2Scr_PositionRadarSpeed; //44位置雷达速度标定  
	Screen_BVRelated_TypeDef*p2Scr_BVRelated;    			//45电量电压关联参数 
	Screen_MoreParameters_TypeDef*p2Scr_MoreParameters;		//46更多参数
	Screen_Angle_Typedef* p2Scr_Angle;                      //47角度数据信息显示对象指针
	Screen_High_Typedef* p2Scr_High;						//48高度信息
	Screen_HangingHeight_TypeDef* p2Scr_HangingHeight;		//49悬挂高度
	Screen_LegHeight_TypeDef* p2Scr_LegHeight;				//50支腿高度
	Screen_LaserRanging_TypeDef* p2Scr_LaserRanging;		//51激光测距
	Screen_USRanging_TypeDef* p2Scr_USRanging;				//52超声波测距
	Screen_Pressure_TypeDef* p2Scr_Pressure;				//53压力传感器
	Screen_AnalogCollection_TypeDef*p2Scr_AnalogCollection; //54模拟量采集模块
	Screen_AnalogOut_TypeDef*p2Scr_AnalogOut;				//55模拟量输出模块
	Screen_GPS_TypeDef*p2Scr_GPS;							//56GPS数据	
	Screen_LaserNavigation_TypeDef*p2Scr_LaserNavigation;   //57激光导航数据  
	DispatchSystemCmd_Typedef* p2Scr_DispatchSystemCmd;		//58调度系统
	AtuoRunStatus_Typedef* p2Scr_AtuoRunStatus;				//59自动运行
	SafeSenorStatus_Typedef* p2Scr_SafeSenorStatus_Typedef;	//60避障开关参数设置
	Screen_Navigation_Typedef* p2Scr_Navigation;            //61磁导航+RFID界面显示状态对象指针
	Screen_ReinforcedServo_TypeDef*p2Scr_ReinforcedServo;   //62雷赛伺服
	Screen_PGV_Typedef* p2Scr_PGV;                          //63PGV参数显示界面对象指针
	Screen_Camera_TypeDef*p2Scr_Camera;      				//64栈板相机
	Screen_UnionStatus_Typedef* p2Scr_UnionStatus;     		//65联动状态界面对象指针
	Screen_LightSpot_TypeDef *p2Scr_LightSpot;  			//66光斑状态  
	Screen_Position_TypeDef	*p2Scr_Position; 				//67位置定位雷达状态
	Screen_TiltSensor_TypeDef*p2Scr_TiltSensor;      		//68倾角传感器
	Screen_Pulser_TypeDef*p2Scr_Pulser;						//69手摇脉冲器
	Screen_BMS_Typedef* p2Scr_BMS;                          //70BMS
	Screen_ForcedOut_TypeDef*p2Scr_ForcedOut;				//71IO强制输出
    Screen_EEPROM_TypeDef* p2Scr_EEPROM;                    //72EEPROM界面对象指针
	ANCStatusPage_TypeDef* p2Scr_ANCStatusPage;				//74安川驱动器状态
	DBCOMSet_TypeDef* p2Scr_DBCOMSet;						//75多倍通状态
	
    Screen_MMU_Typedef* p2Scr_MMU;                          //210内存控制页对象指针
    PathPoint_StDef* p2Scr_Path_Comm;                       //208缓存路径信息对象指针
    Screen_Path_Typedef* p2Scr_Path;                        //209链表路径信息对象指针
	Screen_AtuoInf_Typedef* p2Scr_AtuoInf;                  //206更多自动信息对象指针
	Screen_Button_Typedef* p2Scr_Button;                    //207按钮界面对象指针
	Screen_SysCmd_Typedef* p2Scr_SysCmd;                    //213系统控制命令对象指针
	Screen_UnionParameter_Typedef* p2Scr_UnionParameter;    //223联动参数界面对象指针
	Screen_UnionEMG_Typedef* p2Scr_UnionEMG;				//222联动报警界面对象指针
    Para_Obo_StDef Obo;                                     //多源控制中间对象

}ExternalHmi_StDef;

//对象外部声明
HMI_EXT u16 HMI_Buf[HMI_Cache_Num];							//HMI缓存
HMI_EXT ExternalHmi_StDef  ExtHmi;                          //HMI对象
//CBB_INIT_EXT HMICallBack_Typedef g_HMI CBB_INIT_INIT;				//实例化HMI对象
//HMI_EXT HMICallBack_Typedef g_HMI;							//实例化HMI对象,CBB_INIT_INIT SI识别不出来

//方法外部声明
//HMI_EXT bool HMI_ScreenShow(void);
//HMI_EXT bool HMI_ScreenReads(void);
//HMI_EXT bool HMI_Read_InitOnce(void);
HMI_EXT bool HMI_Main(void);

#endif

//***********************************END OF FILE***********************************


