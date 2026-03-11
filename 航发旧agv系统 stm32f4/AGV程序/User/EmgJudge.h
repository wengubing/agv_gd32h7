/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: EmgJudge.h
** 创 建 人: 文小兵  
** 描    述: 故障判断按类型分类和模块分类，整理到单独源文件封装进行调用
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年4月12日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __EmgJudge_h__
#define __EmgJudge_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  EmgJudge_GLOBALS
 #define EmgJudge_EXT
#else
 #define EmgJudge_EXT  extern
#endif

//自定义引用	
#include "CBB_Init.h"
#include "AgvChassisController.h"
#include "app_AlarmCode.h"

//非受控异常 位数据对象定义
typedef struct __attribute__((packed))
{
    unsigned int CarEstop : 1;								//车体急停
    unsigned int CanOpenStartTimeout  : 1;					//CANopen启动超时 或故障
	unsigned int CanSlvOffline : 1;							//can从站离线
	unsigned int CanMstErr : 1;								//can主站异常
	unsigned int WalkSvoErr : 1;							//行走伺服异常
	unsigned int LiftSvoErr : 1;							//升降伺服异常
	unsigned int ProjectExtend : 1;							//项目扩展异常
}ACC_FaultsLoseControl_BitDef;

//急停异常 位数据对象定义
typedef struct
{
	unsigned int Remote : 1;								//遥控器急停
	unsigned int Dsp : 1;									//调度急停
	unsigned int HMI_EST : 1;								//HMI急停
	unsigned int Little : 1;								//小急停
}ACC_FaultsEmgStop_BitDef;

//通信超时异常 位数据对象定义
typedef struct
{
	unsigned int DSP : 1;									//调度系统通信超时
	unsigned int HMI : 1;									//HMI通信超时
	unsigned int RemoteReceiverA : 1;						//遥控接收器A通信超时
	unsigned int RemoteReceiverB : 1;						//遥控接收器B通信超时
	unsigned int RemoteWired : 1;							//遥控有线通信超时
	unsigned int BMS : 1;									//BMS通信超时
	unsigned int Voice : 1;									//语音播报通信超时
	unsigned int AD : 1;									//AD通信超时
	unsigned int MagF : 1;									//前磁导航通信超时
	unsigned int MagB : 1;									//后磁导航通信超时
	unsigned int MagL : 1;									//左磁导航通信超时
	unsigned int MagR : 1;									//右磁导航通信超时
	unsigned int RFID : 1;									//RFID通信超时
	unsigned int PGVUpward : 1;								//向上pgv通信超时
	unsigned int PGVDownward : 1;							//向下pgv通信超时
	unsigned int SerialExp : 1;								//串口拓展板通信超时
	unsigned int CommConv : 1;								//通信转换板通信超时
	unsigned int WheelAngle : 1;							//轮系角度通信超时
	unsigned int WheelHeight : 1;							//轮系高度通信超时
	unsigned int Asb : 1;							        //联动通信超时
    unsigned int LaserSpot : 1;							    //光斑传感器通信超时
	unsigned int F_Leadshine : 1;							//前雷赛伺服通信超时
	unsigned int B_Leadshine : 1;							//后雷赛伺服通信超时
	unsigned int L_Leadshine : 1;							//左雷赛伺服通信超时
	unsigned int R_Leadshine : 1;							//右雷赛伺服通信超时
	unsigned int GPS : 1;							    	//GPS通信超时 
	unsigned int PLC : 1;							    	//PLC通信超时 
	unsigned int AD1 : 1;							    	//AD1通信超时
	unsigned int Thermodetector : 1;						//温度通信超时
}ACC_FaultsTimeOut_BitDef;


//升降异常 位数据对象定义
typedef struct
{
	unsigned int TooHigh : 1;								//平台过高
	unsigned int TooLow : 1;								//平台过低
	unsigned int DiffLimit : 1;								//高度差过大
	unsigned int DiffLimitRear : 1;							//后差过大
	unsigned int ExecTimeout : 1;							//执行超时
}ACC_FaultsLift_BitDef;

//功能异常(常见受控停车) 位数据对象定义
typedef struct
{
	unsigned int CanBusErr : 1;								//CAN总线异常 启动时
	unsigned int CanNoSlv : 1;								//没有can从站上线
	unsigned int CanLessSlv : 1;							//can从站数量不足
	unsigned int ModeSwitchTimeOut : 1;						//模式切换超时
	unsigned int RemoteWirelessSignalErr : 1;				//遥控器无线信号异常
	unsigned int CrashBarF : 1;								//防撞触边触发 前
	unsigned int CrashBarB : 1;								//防撞触边触发 后
	unsigned int CrashBarL : 1;								//防撞触边触发 左
	unsigned int CrashBarR : 1;								//防撞触边触发 右
    unsigned int wheelAngleLimited : 1;					    //轮系角度限制
    unsigned int LowBatteryStop : 1;						//低电量停止
    unsigned int MagOutLine : 1;							//磁导航离线
    unsigned int BMSErrStop : 1;							//BMS故障停止
    unsigned int AsbErr : 1;							    //联动异常
	unsigned int ProjectExtend : 1;							//项目扩展异常
    //此对象包含了除联动传感器超时以外的所有异常，
    //hmi可简化为一条信息 然后通过数据位查看
    //这些故障都是本车故障，参与故障传输汇总
	unsigned int ColourOutLine : 1;							/*色带离线*/
	unsigned int SlamLostLocation : 1;						/*slam定位丢失*/
	unsigned int SlamLostCom : 1;							/*slam通讯丢失*/
}ACC_FaultsFunc_BitDef;

//自动运行异常 位数据对象定义
typedef struct __attribute__((packed))
{
    u32 AGV_AGV_ALM_S3 : 1;                                 //调度系统急停按下
    u32 AGV_ALM_HostScomm : 1;                		        //调度系统通信超时
    u32 AGV_ALM_PathPointNotMatch : 1;          	        //自动运行路径点不匹配（AGV走错，该标签与目标标签不一致）
    u32 AGV_ALM_RotationTimeout : 1;            	        //自动运行自转超时

    u32 AGV_ALM_BlindWalkTimeout : 1;           	        //自动运行盲走超时
    u32 AGV_ALM_OnlineTowardsNotInitialized : 1;  	        //上线朝向未初始化	
    u32 AGV_ALM_StartingPointNotMatch : 1;        	        //路径起始点不匹配
    u32 AGV_ALM_LoadNotCharge : 1; 				            //充电条件不满足（有负载）

    u32 AGV_ALM_DownloadTowardsError : 1;        	        //下发航向错误
    u32 AGV_ALM_LateralDeviationOver : 1;         	        //巡线偏差过大
    u32 AGV_ALM_NavigationRadarScoom : 1;         	        //定位雷达通信超时（定位PGV通信超时）
    u32 AGV_ALM_NavigationFail : 1;              	        //自动运行导航定位失败（或未发现循迹线或定位标签） --- 行驶过程中没有发现定位标签

    u32 AGV_ALM_PathPointNoPallet : 1;    	      	        //未发现托盘（叉车具有）
    u32 AGV_ALM_RiseLimitError : 1;        		            //举升限位开关状态异常（叉车具有）
    u32 AGV_ALM_NotFindTag : 1;					            //自动条件不满足（进入自动时未发现标签）
    u32 AGV_ALM_DownoadPathError : 1;            	        //路径下载失败
    
    /*AGV新框架得出的故障*/
    u32 AGV_ALM_ParkQRScoomFail : 1;				        //地标PGV通讯超时
    u32 AGV_ALM_FixtureQRScoomFail : 1;			            //工装PGV通讯超时
    u32 AGV_ALM_RFIDScoomFail : 1;					        //RFID通讯超时
    u32 AGV_ALM_MagScoomFail : 1;					        //磁导航通讯超时
    
    u32 AGV_ALM_MagPostureErr : 1;					        //磁条旋转过程中丢失目标磁条
    u32 AGV_ALM_GoodsMoveErr : 1;					        //运载过程中工装移动

    //叉车具有
    u32 AGV_ALM_ForkPhotoeleSensor : 1;                     //光电传感器触发（叉车具有）			
    u32 AGV_ALM_PalletAngleDeviation : 1;                   //托盘放置角度偏差过大（叉车具有）		
    u32 AGV_ALM_PalletLateralDeviation : 1;                 //托盘放置横向偏差过大（叉车具有）	
    u32 AGV_ALM_ToolingCurTowards_Err : 1;                  //当前工装朝向不对

    u32 AGV_ALM_BevelRadarInside : 1;                       //斜面激光雷达最内层触发（叉车具有）		
    u32 AGV_ALM_ColumnRadarInside : 1;                      //立柱激光雷达最内层触发（叉车具有）
}ACC_FaultsAuto_BitDef;

//其它异常 位数据对象定义
typedef struct
{
	unsigned int CodeErr : 1;								//代码异常
	unsigned int WheelAngCalErr  : 1;						//轮系角度解算错误
	unsigned int CtrlSysVoltageErr  : 1;					//控制系统电压异常
	unsigned int Expand : 1;								//拓展异常
	
}ACC_FaultsOther_BitDef;

//代码异常管理 位数据对象定义 单独定义，在代码中使用
typedef struct
{
	//代码错误
	u16 InPara : 1;											//函数参数异常
	u16 BackRes : 1;										//调用返回异常
	u16 NullPointer : 1;									//空指针
	u16 OutRange : 1;										//指针或数组越界
	u16 OverFlow : 1;										//赋值溢出
	u16 LengthLimited  : 1;									//结构体或数组长度定义超限
	u16 ZeroDenominator  : 1;								//除数为0
	u16 InitErr  : 1;										//初始化错误
	u16 BoundCheckOut : 1;									//边界检查超出
	u16 Unaligned : 1;										//内存未对齐
	u16 OtherErr  : 1;										//其它错误
}ACC_CodeErr_BitDef;

//非受控异常 共用体数据对象定义
typedef union __attribute__((packed))
{
	ACC_FaultsLoseControl_BitDef Bit;						//位域
	u32 All;												//汇总
}ACC_FaultsLoseControl_UnionDef;

//通信超时异常 共用体数据对象定义
typedef union
{
	ACC_FaultsTimeOut_BitDef Bit;							//位域
	unsigned int All;										//汇总
}ACC_FaultsTimeOut_UnionDef;

//急停异常 共用体数据对象定义
typedef union
{
	ACC_FaultsEmgStop_BitDef Bit;							//位域
	unsigned int All;										//汇总
}ACC_FaultsEmgStop_UnionDef;

//升降异常 共用体数据对象定义
typedef union
{
	ACC_FaultsLift_BitDef Bit;								//位域
	unsigned int All;										//汇总
}ACC_FaultsLift_UnionDef;

//功能异常 共用体数据对象定义
typedef union
{
	ACC_FaultsFunc_BitDef Bit;								//位域
	u32 All;												//汇总
}ACC_FaultsFunc_UnionDef;

//自动运行异常 共用体数据对象定义
typedef union __attribute__((packed))
{
	ACC_FaultsAuto_BitDef Bit;								//位域
	u32 All;										        //汇总
}ACC_FaultsAuto_UnionDef;

//hmi故障一类通信缓存 共用体数据对象定义
typedef union __attribute__((packed))
{
	struct __attribute__((packed))
	{
		ACC_FaultsLoseControl_UnionDef FaultsLoseControl;	//非受控异常
		ACC_FaultsAuto_UnionDef FaultsAuto;					//自动运行异常
		//bool abc;//测试存储大小超过64位检出正常
	}Class;
	u64 All;												//汇总
}ACC_FaultsHmiErr1_UnionDef;

//其它异常 共用体数据对象定义
typedef union
{
	ACC_FaultsOther_BitDef Bit;								//位域
	unsigned int All;										//汇总
}ACC_FaultsOther_UnionDef;

//仅提醒异常 共用体数据对象定义
typedef union __attribute__((packed))
{
    struct __attribute__((packed))
    {
        //运动限制提醒位域定义 32位
        union __attribute__((packed))
        {   
            struct __attribute__((packed))
            {
                unsigned int LimitUp_Overload : 1;					//超载限制上升
                unsigned int LimitUp_UnbalancedLoad : 1;		    //偏载限制上升 2025年12月周工让改为："偏载造成局部超载"，不让上升
                unsigned int LimitWalk_Overload : 1;				//超载限制行走
                unsigned int LimitWalk_UnbalancedLoad : 1;		    //偏载限制行走 2025年12月周工让改为："偏载造成局部超载"，不让行走
				//unsigned int LimitWalk_UnbalancedLoad_Core : 1;		//偏载限制行走 2025年12月周工让改为："中心超出稳定范围"，单独的重心超稳定范围
                unsigned int LimitWalk_Charging : 1;				//正在充电限制行走
            }Bit;
            unsigned int All;
        }MoveLimit;                                                 //运动限制提醒

        //单纯提醒位域定义 32位
        union __attribute__((packed))
        {   
            struct __attribute__((packed))
            {
                unsigned int Voice : 1;								//语音播报通信超时
                unsigned int Charging : 1;							//充电中不能退出静止模式
                unsigned int ChargingCurrentErr	: 1;				//充电电流异常
                unsigned int BreakFault : 1;				        //抱闸切换错误
                unsigned int LowBatteryRemind : 1;					//电量低提醒
                unsigned int ErrRemind : 1;							//BMS故障提醒
                unsigned int Auto_NotAllowed : 1;					//自动条件不满足
                unsigned int Auto_DirectionNotInit : 1;			    //上线朝向未初始化
                unsigned int Auto_NotFindPointSign : 1;				//既未在十字磁条也未在rfid标签上
                unsigned int LaserObs : 1;				            //激光避障触发
				unsigned int Expand : 1;				            //拓展故障提醒
				unsigned int ProjectExtend : 1;				        //项目拓展故障提醒
            }Bit;
            unsigned int All;
        }RemindOnly;                                                //单纯提醒
    }Class;
    unsigned long long All;										    //汇总
}ACC_Remind_UnionDef;

//代码异常管理 共用体数据对象定义
typedef union
{
	ACC_CodeErr_BitDef Bit;									//位域
	u16 All;												//汇总
}ACC_CodeErr_UnionDef;

//受控故障汇总 共用体数据对象定义
typedef union __attribute__((packed))
{
	ACC_ControlledStop_BitDef Bit;							//位域
	unsigned long long All;									//汇总
}ACC_ControlledStop_UnionDef;

//故障检出统计定义
typedef struct 
{
    u16 Warn;												//提示
	u16 MoveLimit;										    //限制运动
	u16 ControlledStop;										//受控停车
	u16 LoseControlStop;									//非受控停车
}ACC_FaultsTypeCnt_StDef;

//磁条离线检出数据对象定义
typedef struct
{
	enum TraceOutlineSTM_EnumDef
	{
		TraceOutline_Init=0,								//初始化
		TraceOutline_ModeIn,								//进入巡线模式
		TraceOutline_Move,									//巡线运动
		TraceOutline_Lose,									//丢失磁条,判断离线时间 或 距离
		TraceOutline_Stop,									//停车
	}STM;

	f32 Distance_Init;										//离线时的里程
	f32 Distance_Real;										//实时的里程
	f32 Distance_Lose;										//离线走出的里程 m
}TraceOutline_StDef;

//不影响系统正常运行的错误状态检出 共用体数据对象定义
typedef union
{
	struct CodeRemind_Bit
	{
		u16 Screen_HeadLimit : 1;								//0hmi页面缓存限制
		u16 b1 : 1;												//1
		u16 b2 : 2;												//2
	}Bit;														//位域
	u16 All;													//汇总
}CodeRemind_UnionDef;

//其它不参与自身故障汇总的异常
typedef union
{
	struct 
	{
		u16 Asb_LoseControl : 1;								//非受控异常
		u16 Asb_ControlStop : 1;								//受控停车
		u16 Asb_MoveLimited : 1;								//运动限制
        u16 Asb_Warn : 1;										//提示
	}Bit;														//位域
	u16 All;													//汇总
}ACC_FaultsOtherCar_UnionDef;

//角度差过大检出步骤
typedef enum
{
	Faults_DiffAngStep_Null = 0,							//无效
	Faults_DiffAngStep_IsWalkMode,							//在行走模式
	Faults_DiffAngStep_Motion,								//在运动中
	Faults_DiffAngStep_OverDiff,							//有角度差过大
	Faults_DiffAngStep_OverTime,							//持续时间过长
}Faults_DiffAngStep_TypeDef;

//角度差过大检出对象
typedef struct
{
	f32 DiffAng[MR_ModuleNumMxa];							//角度差 实际-目标
	Faults_DiffAngStep_TypeDef Step;						//步骤
	u32 MsStamp;											//恢复时间戳
	u32 DiffMs;												//超过阈值持续时间
	f32 Para_DiffAng;										//角度差过大阈值 度
	u16 Para_Time;											//持续时间阈值 ms
}DiffAngFaults_TypeDef;

//项目拓展异常
typedef struct
{
	u32 LoseControl;										//非受控异常
	u32 ControlledStop;										//受控停车
	u32 Warn;												//提示
}Acc_Faults_ProjectExtend_TypeDef;

//故障检出相关参数
typedef struct
{
	//偏载报警条件参数
	f32 UnbalancedLoad_WeightMin;							//偏载检出最小重量 t
	f32 UnbalancedLoad_HeightMin;							//偏载检出最小高度 mm
	u16 ButtonTimeOutJudge;									//按钮切换超时时间 ms

}Acc_Faults_Para_TypeDef;

//异常数据对象定义
typedef struct
{
	Acc_Faults_Para_TypeDef Para;							//参数
	ACC_FaultsLoseControl_UnionDef LoseControl;				//非受控异常
	ACC_FaultsTimeOut_UnionDef TimeOut;						//通信超时异常
	ACC_FaultsEmgStop_UnionDef EmgStop;						//急停异常
	ACC_FaultsLift_UnionDef Lift;							//升降异常
	ACC_FaultsLift_UnionDef Absorption;						//悬挂/减震异常  暂时完全拷贝升降功能
	ACC_FaultsFunc_UnionDef Func;							//功能异常
	ACC_FaultsAuto_UnionDef Auto;							//自动运行异常
	ACC_FaultsOther_UnionDef Other;							//其它异常
	Acc_Faults_ProjectExtend_TypeDef ProjectExtend;			//项目拓展异常

	CodeRemind_UnionDef CodeRemind;							//代码提醒

    ACC_ControlledStop_UnionDef ControlledStop;				//受控停车汇总

	ACC_Remind_UnionDef Remind;								//仅提醒异常
	ACC_FaultsTypeCnt_StDef TypeCnt;						//故障检出统计

    ACC_FaultsOtherCar_UnionDef OtherCar;					//其它不参与自身故障汇总的异常

	//故障数据
	TraceOutline_StDef TraceOutline;						//磁条离线检出
	u16 MagOutLineTime[4];									//磁导航离线时间
	u16 ColourOutLineTime[4];								//色帶离线时间

	u16 ChargIngMCTime;										//充电极板打开计时
    u16 ChargIngTime;								        //正在充电计数 ms

	char SvoErr_ID;											//伺服异常ID
	int SvoErr_Code;										//伺服异常故障码

	DiffAngFaults_TypeDef DiffAng;							//角度差过大检出

    ACC_FaultsType_TypeDef Res;							    //故障类型
    EMG_Output_TypeDef Output;							    //异常输出
	u16 Button_Asb_TimeOut_Ms;
}ACC_Faults_TypeDef;

//对象外部声明
EmgJudge_EXT ACC_Faults_TypeDef g_Faults;					//故障数据对象
EmgJudge_EXT ACC_CodeErr_UnionDef g_CodeErr;				//代码异常管理

//方法外部声明
EmgJudge_EXT EMG_Faults_TypeDef EMG_Judge(void);			//故障判断
EmgJudge_EXT bool Emg_MsCnt(void);                          //报警检出ms计数接口
EmgJudge_EXT bool EMG_Judge_Controlled(void);
#endif

//***********************************END OF FILE***********************************


