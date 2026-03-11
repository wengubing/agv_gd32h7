/********************************** Copyright ***********************************
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
*
* FileName   : app_LiftCtrl.h
* Version    : V1.0
* Author     : WenXiaoBing
* Date       : 2024.1.19
* Description: 输入磁条偏移，输出整车控制进行巡线
*******************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __app_LiftCtrl_h__
#define __app_LiftCtrl_h__

//自定义引用
#include <stdbool.h>
#include "..\..\CBB\user\basecalfunc\common_def.h"

//宏定义
#define LIFT_HG_MAX											12//最大组数
#define GEARS_NUM_MAX										6//升降挡位数量

//升降状态
typedef enum
{
	LiftCtrl_LiftST_Null = 0,              					//无效
	LiftCtrl_LiftST_Normal = 1,              				//
	LiftCtrl_LiftST_LimitHigh = 2,              			//
	LiftCtrl_LiftST_LimitLow = 3,              				//
	LiftCtrl_LiftST_Up = 4,              					//
	LiftCtrl_LiftST_Down = 5,              					//
}LiftCtrl_LiftST_TypeDef;

//升降状态机
typedef enum
{
	LiftCtrl_STM_Init = 0,              					//无效
	LiftCtrl_STM_Up = 1,              						//
	LiftCtrl_STM_Down = 2,              					//
}LiftCtrl_STM_TypeDef;


//quadrant
typedef enum
{
	LiftCtrl_Quadrant_LF = 0,              					//左前
	LiftCtrl_Quadrant_RF = 1,              					//右前
	LiftCtrl_Quadrant_LB = 2,              					//左后
	LiftCtrl_Quadrant_RB = 3,              					//右后
	LiftCtrl_Quadrant_Nnm = 4,              				//num
}LiftCtrl_Quadrant_TypeDef;

//ctrl
typedef enum
{
	LiftCtrl_Ctrl_LF = 0,              						//左前
	LiftCtrl_Ctrl_RF = 1,              						//右前
	LiftCtrl_Ctrl_B = 2,              						//后
	LiftCtrl_Ctrl_LB = 2,              						//左后
	LiftCtrl_Ctrl_RB = 3,              						//右后
}LiftCtrl_Ctrl_TypeDef;


//cmd_obj
typedef struct
{
	bool in_Up;												//上升命令
	bool in_Down;											//下降命令
	char in_Gears;											//挡位
	bool do_Up;												//执行命令 上升
	bool do_Down;											//下降
	char do_Gears;											//挡位
	bool do_Force;											//强制执行
	LiftCtrl_STM_TypeDef LiftSTM;							//升降状态机	
	u32 in_Ms, ExecTimeoutStartMs, MotorDelayStartMs, ValveDelayStartMs;

	//位命令
	struct
	{
		u16 ManualDown_LF : 1;								// 左前下降
		u16 ManualDown_RF : 1;								// 右前下降
		u16 ManualDown_B : 1;								// 后侧下降
		u16 ManualDown_LB : 1;								// 左后下降
		u16 ManualDown_RB : 1;								// 右后下降
	}Bits;
}LiftCtrl_Cmd_TypeDef;

//极值
typedef struct
{
	f32 Highest;											//最高点
	char HighestID;											//最该点所在组
	f32 Lowest;												//最低点
	char LowestID;											//最低点所在组
}LiftCtrl_Extreme_TypeDef;

//高度数据对象定义
typedef struct
{
	u16 Num;												//高度数据数量
	u32 Org[LIFT_HG_MAX];									//原始数据
	u32 SetHighOrg[LIFT_HG_MAX];							//标定高点
	u32 SetLowOrg[LIFT_HG_MAX];								//标定低点 传感器
	f32 SetHigh;											//标定高点 实际																					
	f32 SetLow;												//标定低点 实际
	f32 Cal[LIFT_HG_MAX];									//标定后mm
	LiftCtrl_Quadrant_TypeDef Quadrant[LIFT_HG_MAX];		//记录所在方位
	u16 QuadrantCnt[LiftCtrl_Quadrant_Nnm];					//各方位计数
	f32 QuadrantSum[LiftCtrl_Quadrant_Nnm];					//各方位 求和
	f32 QuadrantAvg[LiftCtrl_Quadrant_Nnm];					//各方位 平均
	f32 Point3Avg[LIFT_HG_MAX];								//各控制点 平均
	f32 Avg;												//总平均高度
	f32 AvgBefor;											//之前的高度

	LiftCtrl_Extreme_TypeDef Extreme;						//高度的极值独立点，非3点最高高度，也不是控制点
	LiftCtrl_Extreme_TypeDef Extreme_Titan;					//titan要用3点的高度
	//LiftCtrl_Extreme_TypeDef Extreme_Ctrl;					//控制点的极值，例如按象限分组求和的极值
	LiftCtrl_Extreme_TypeDef Extreme_Quadrant;				//按象限分组的极值
}LiftCtrl_Height_TypeDef;

//平台控制类型
typedef enum
{
	LC_PFTP_Null 		= 0,	//无效
	LC_PFTP_Titan		= 1,	//titan 标准三点升降
	LC_PFTP_Scissors	= 2,	//剪刀架升降 单油缸
	LC_PFTP_Screw		= 3,	//丝杠升降 机械同步
	LC_PFTP_UnitePump	= 4,	//联合泵 电磁阀控制同步
	LC_PFTP_SynCylinder	= 5,	//同步油缸 液压排油同步

	LC_PFTP_Scissors_Two= 6,	//双电机独立控制的同步丝杆 靠拉线软件同步

	LC_PFTP_AirSuspension=7,	//空气悬挂
	
	LC_PFTP_DoublePumpQuad=8,	//双泵四点液压,通过4个比例阀来控制4点升降；四联泵是其退化形态

}LC_PlatformType_TypeDef;

//控制参数对象定义
typedef struct
{
	LC_PlatformType_TypeDef PfType;							//平台类型
	u16 CtrlNnm;											//控制点数量
	f32 LimitUp, LimitDown;									//升降限位

	u16 SpeedPlanUp[GEARS_NUM_MAX];							//上升挡位速度表
	u16 SpeedPlanDown[GEARS_NUM_MAX];						//下降挡位速度表
	f32 Pump[LIFT_HG_MAX];									//泵算到相同油缸数量上的排量
	f32 PumpUnifor[LIFT_HG_MAX];							//归一化
	f32 PumpUnifor2Speed[LIFT_HG_MAX];						//归一化到速度
	f32 k_HgDiff2SpeedDiff;									//调整参数
	u16 PumpOnDelayMs;										//油泵启动延迟ms
	u16	ValveOffDelayMs;									//电磁阀关闭延迟ms
	u16 ValveOffTgHg;										//电磁阀同步控制 关闭触发阈值 单位与高度计算值一致
	u16 ValveReOnTgHg;										//电磁阀同步控制 重新打开触发阈值 单位与高度计算值一致
    u16 ValveReOnDelayMs;									//电磁阀同步控制 重新打开延迟ms
    //特殊位参数
    union 
    {
        struct
        {
            u16 ValveSyn_Down : 1;							//下降时采用电磁阀开关同步使能，且关闭电机
            u16 ValveSyn_Down_OnlyOneOff : 1;				//下降时采用电磁阀开关同步使能，且关闭电机，但只关闭一个电磁阀
			u16 ValveSyn_Down_AFD : 1;						//AutForceDown：下降时如果持续高度不变，强制打开全部电磁阀下降
			u16 MotorSyn_Up_Acc : 1;						//电机同步上升时，通过加速进行调平
			u16 MotorReverse : 1;							//电机速度反向 0泵电机速度正 1泵电机速度负
			u16 SingleReachStop : 1;						//单组高度到限位就关电机
			u16 Tst_Temp_LF_UP : 1;							// 左前上升
            u16 Tst_Temp_RF_UP : 1;				            // 右前上升
            u16 Tst_Temp_B_UP : 1;				            // 后侧上升
			u16 Tst_Temp_LB_UP : 1;				            // 左后上升
			u16 Tst_Temp_RB_UP : 1;				            // 右后上升
        }Bits;
        u16 All;
    }BitPara;
	u16 Tst_Temp_Rpm;           							//补偿转速
}LiftCtrl_CtrlPara_TypeDef;

typedef struct
{
	u16 DiffMax;											//高度差最大值 65535关闭
	u16 HeightLow;											//过低提醒
	u16 HeightHigh;											//过高提醒
	u16 DiffRearMax;										//后侧高度差最大值 65535关闭
	u16 TimeoutTime;										//执行超时 ms
	u16 TimeoutHeight;										//执行超时高度差 mm
	u16 EjdgDelay;											//报警延时 ms
}LiftCtrl_WarningPara_TypeDef;

typedef struct
{
	f32 MotorSpeed[LIFT_HG_MAX];							//电机转速命令 rpm
	bool ValveUp[LIFT_HG_MAX];								//上升电磁阀 0-off 1-on
	bool ValveDown[LIFT_HG_MAX];							//下降电磁阀 0-off 1-on
	LiftCtrl_LiftST_TypeDef LiftST;							//升降状态
}LiftCtrl_CtrlOut_TypeDef;

//警告输出
typedef struct
{
	bool TooHigh;											//高度过高
	bool TooLow;											//高度过低
	bool DiffLimit;											//高度差过大
	bool DiffLimitRear;										//后差过大
	bool ExecTimeout;										//执行超时
}LiftCtrl_WarningOut_TypeDef;

//模拟器对象定义
typedef struct
{
	f32 k_Mot2Hg;											//电机转速到高度的系数
	f32 CallDelayMs;										//调用间隔 ms
	f32 Hg_Diff[LIFT_HG_MAX];								//高度增量
	f32 Hg[LIFT_HG_MAX];									//高度结果
}LiftCtrl_Simulator_TypeDef;

//过程对象 时间记录
typedef struct
{
	u32 TooHigh;											//过低提醒
	u32 TooLow;												//过高提醒
	u32 DiffLimit;											//高度差过大
	u32 DiffLimitRear;										//后差过大
}LiftCtrl_MsRecord;

//下降补丁对象
typedef struct
{
	//输入
	u32 In_Ms;												//当前时间 ms
	u16 Para_Ms_Check;										//高度持续没有变化检出时长 65535关闭
	u16 Para_Ms_Continue;									//强制下架持续时间
	u32 In_Hg;												//实时高度

	//过程
	u32 In_Last;											//上次高度
	u32 MsRecord_Check;										//高度发生变化的时间戳
	u32 MsRecord_Continue;									//强制下降触发时间戳

	//结果
	bool Do_ForceDown;										//强制下降
	
}LiftCtrl_AutForceDown_TypeDef;

/*压力均衡控制对象*/
typedef struct
{
	//输入
	f32 Pr[LIFT_HG_MAX];								//输入压力值 Mpa
	u16 Load;											//负载 吨
	f32 PrMax;											//最大压力值
	f32 PrMin;											//最小压力值
	bool AsbFlag;										//联动标志

	//参数
	u16 HgMin;											//压力调节生效 最小高度值 mm
	u16 LoadMin;										//压力调节生效 最小负载值 吨
	u16 SlowDownMax;									//最大减速值 0.1rpm
	f32 k_Pr2Slow;										//减速系数

	//结果
	s16 SlowDown[LIFT_HG_MAX];							//减速 0.1rpm
} LiftCtrl_PressureBalance_TypeDef;

//CBB对象定义
typedef struct
{
	//u32 Ms;													//当前时间 ms 在Cmd里
	LiftCtrl_Cmd_TypeDef Cmd;								//命令对象
	LiftCtrl_Height_TypeDef Height;							//高度对象
	LiftCtrl_CtrlPara_TypeDef CtrlPara;						//控制参数
	LiftCtrl_WarningPara_TypeDef WarningPara;				//警告参数

	LiftCtrl_MsRecord MsRecord;								//时间记录
	LiftCtrl_AutForceDown_TypeDef AFD;						//自动强制下降补丁

	LiftCtrl_CtrlOut_TypeDef CtrlOut;						//控制输出
	LiftCtrl_WarningOut_TypeDef WarningOut;					//警告输出
	LiftCtrl_Simulator_TypeDef Simulator;					//模拟器

	/*压力均衡控制*/
	LiftCtrl_PressureBalance_TypeDef PrBalance;

}LiftCtrl_TypeDef;

//外部方法声明
bool LiftCtrl_Cal(LiftCtrl_Height_TypeDef* p);				//计算
bool LiftCtrl_Extreme(LiftCtrl_Height_TypeDef* p);			//均值极值分析
void LiftCtrl_CmdPrep(LiftCtrl_TypeDef* p);					//命令准备
void LiftCtrl_Ctrl(LiftCtrl_TypeDef* p);					//控制
void LiftCtrl_AllMotorOff(LiftCtrl_TypeDef* p);				//所有电机关闭
void LiftCtrl_AllValveDown(LiftCtrl_TypeDef* p);			//所有电磁阀下降
void LiftCtrl_AllValveUp(LiftCtrl_TypeDef* p);				//所有电磁阀上升
void LiftCtrl_ValveOff(LiftCtrl_TypeDef* p);				//电磁阀关闭
void LiftCtrl_AllValveMotorOff(LiftCtrl_TypeDef* p);		//所有电磁阀关闭，所有电机关闭
bool LiftCtrl_Ejdg(LiftCtrl_TypeDef* p);					//警告
bool LiftCtrl_PumpUnifor(LiftCtrl_CtrlPara_TypeDef* p);		//泵归一化
bool LiftCtrl_MotorReverse(LiftCtrl_TypeDef* p);			//电机速度反向
void LiftCtrl_LiftTestInit(LiftCtrl_TypeDef* p);			//升降测试初始化
void LiftCtrl_Main(LiftCtrl_TypeDef* p);					//主函数
void LiftCtrl_InitExample(LiftCtrl_TypeDef* p);				//初始化示例
bool LiftSimulator(LiftCtrl_TypeDef* p);					//模拟器

#endif

/************************************END OF FILE************************************/

