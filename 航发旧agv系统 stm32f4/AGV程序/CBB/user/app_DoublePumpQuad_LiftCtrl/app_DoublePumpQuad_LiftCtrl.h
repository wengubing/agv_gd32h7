/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_DoublePumpQuad_LiftCtrl.h   
* Version    : V1.0		
* Author     : master		
* Date       : 2025.10.10
* Description: 双泵四点液压升降控制
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_DoublePumpQuad_LiftCtrl_h__
#define __app_DoublePumpQuad_LiftCtrl_h__

#ifdef  app_DoublePumpQuad_LiftCtrl_GLOBALS
 #define app_DoublePumpQuad_LiftCtrl_GLOBALS_EXT
#else
 #define app_DoublePumpQuad_LiftCtrl_GLOBALS_EXT  extern
#endif 

/*包含以下头文件*/
#include <stdbool.h>

#include <stm32f4xx.h>
 
#define DoublePumpQuad_Higt_MAX		4	//高度数据最大值
 
//平台状态
typedef enum
{
	DoublePumpQuad_LiftCtrl_Null 		= 0,	//无效
	DoublePumpQuad_LiftCtrl_Normal 		= 1,	//保持当前
	DoublePumpQuad_LiftCtrl_LimitUp 	= 2,	//升限位
	DoublePumpQuad_LiftCtrl_LimitDown	= 3,	//降限位
	DoublePumpQuad_LiftCtrl_Up			= 4,	//升进行中
	DoublePumpQuad_LiftCtrl_Down 		= 5,	//降进行中
	//DoublePumpQuad_LiftCtrl_Level 		= 6,	//调平进行中
	DoublePumpQuad_LiftCtrl_LevelFalish	= 7,	//调平完成
}DoublePumpQuad_LiftCtrl_STM_TypeDef;

//控制模式
typedef enum
{
	DoublePumpQuad_Null 		= 0,	//高度调节
	DoublePumpQuad_High 		= 10,	//高度调节
	DoublePumpQuad_Pitch 		= 20,	//俯仰调节
	DoublePumpQuad_Roll 		= 30,	//横滚调节
	DoublePumpQuad_FloorLevel	= 40,	//对地调地平
}DoublePumpQuad_LiftCtrl_Mode_TypeDef;

//控制参数对象定义
typedef struct
{
	float 		Up_DA_Maximum		[DoublePumpQuad_Higt_MAX]	;	/*DA模块输出的最da值*/
	float 		Up_DA_Minimum		[DoublePumpQuad_Higt_MAX]	;	/*DA模块输出的最小值*/
	float 		Down_DA_Maximum		[DoublePumpQuad_Higt_MAX]	;	/*DA模块输出的最da值*/
	float 		Down_DA_Minimum		[DoublePumpQuad_Higt_MAX]	;	/*DA模块输出的最小值*/

	int16_t 	SpeedPlanHigh		[6]	;	//上升挡位速度表
	int16_t 	SpeedPlanPitch		[6]	;	//俯仰挡位速度表
	int16_t 	SpeedPlanRoll		[6]	;	//横滚挡位速度表

	float 		HighLimit			[DoublePumpQuad_Higt_MAX]	;	/*上限位*/
	float		LowLimit			[DoublePumpQuad_Higt_MAX]	;	/*下限位*/

	float 		PitchLimit									;	/*俯仰限位*/
	float  		Pitch_Level_Limit							;	/*俯仰 调平的最小结果 小于此认为调节完成*/
	float  		Pitch_Level_DeadZne							;	/*俯仰偏差大于 此认为需要调节*/


	float 		RollLimit									;	/*横滚限位*/
	float  		Roll_Level_Limit							;	/*横滚 调平的最小结果 小于此认为调节完成*/
	float  		Roll_Level_DeadZne							;	/*横滚偏差大于 此认为需要调节*/

	float		ShowDis										;	/*提前减速距离*/

	//双联泵 组内、组外调节阈值
	float  		Onside_Diff_Min								;
	float  		Onside_Diff_Max								;
	float  		Onside_Diff_Limit							;	/*组内调节范围*/
	float  		Without_Diff_Min							;
	float  		Without_Diff_Max							;
	float  		Without_Diff_Limit							;	/*组内调节范围*/


	uint16_t	Fun_AllPlace			: 1;	//所有高度数据达到后才停止
	uint16_t	Fun_NoShowDis			: 1;	//禁用限位前减速功能
	
	uint16_t	Fun_NoSpeedRegulation	: 1;	//禁用速度调节
	uint16_t	Fun_NoStartDelay		: 1;	//禁用启动延迟，比例阀动作后打开电磁阀，再延迟电机起转
	uint16_t	Fun_ValveControl		: 1;	//电磁阀控制，下降过程关最快的电磁阀
	//uint16_t	Fun_ThrottleValveControl: 1;	//电磁阀控制，上升过程
	uint16_t	Fun_DoublePumpQuad		: 1;	//左右侧组内比例阀调整、左右侧组间泄压阀调整
	uint16_t	Fun_DoublePumpQuad_2	: 1;	//对角组内比例阀调整、组间泄压阀调整
	
}DoublePumpQuad_LiftCtrl_CtrlPara_TypeDef;

/*控制命令定义*/
typedef struct
{
	uint8_t 	in_Up		: 1;	//输入命令 上升
	uint8_t 	in_Down		: 1;	//输入命令 下降
	uint8_t 	in_Level	: 1;	//输入命令 调平

	uint8_t 	do_Up		: 1;	//执行命令 上升 
	uint8_t 	do_Down		: 1;	//执行命令 下降

	uint8_t 	do_Force	: 1;	//强制执行

	uint8_t 	in_Gears	: 4;	//挡位
	uint8_t 	do_Gears	: 4;
}DoublePumpQuad_LiftCtrl_Cmd_TypeDef;
 
/*输入传感器结果*/
typedef struct
{
	uint32_t	In_Ms			;	/*输入实时时间*/
	float	Distance	[4]	;	/*4点高度*/
	float	Sensor	[4]	;	/*4点高度*/

}DoublePumpQuad_LiftCtrl_CtrlIn_TypeDef;

/*中间过程*/
typedef struct
{
	DoublePumpQuad_LiftCtrl_Mode_TypeDef	Last_DPQ_Mode								;	
	DoublePumpQuad_LiftCtrl_STM_TypeDef		Last_DPQ_STM								;	
	int16_t									*p_SpeedPlan								;	/*指向数组的指针，不同调节模式下，指向不同的数组*/
	uint32_t								Last_Ms										;	/*上次时间*/
	float									Last_High		[DoublePumpQuad_Higt_MAX]	;	/*上次高度*/
	float									High_Speed		[DoublePumpQuad_Higt_MAX]	;	/*升降速度*/
	float									Highest										;	/*最高点的高度*/
	float									Highest_Diff	[DoublePumpQuad_Higt_MAX]	;	/*和最高点的高差*/
	float									Up_K			[DoublePumpQuad_Higt_MAX]	;	/*上升时需要的每个点调节情况 0表示需要停下等其他点位 1表示是最落后的点位*/
	
	float									Lowest										;	/*最低点的高度*/
	float									Lowest_Diff		[DoublePumpQuad_Higt_MAX]	;	/*和最低点的高差*/
	float									Down_K			[DoublePumpQuad_Higt_MAX]	;	/*上升时需要的每个点调节情况 0表示需要停下等其他点位 1表示是最落后的点位*/
	float									Average										;	/*4点平均高度*/
	float									Diff_Max									;	/*偏差最大值*/
	float									Average_F									;	/*前侧2点 平均高度*/
	float									Average_B									;	/*前侧2点 平均高度*/
	float									Average_L									;	/*前侧2点 平均高度*/
	float									Average_R									;	/*前侧2点 平均高度*/

	float									Average_L_Onside_Diff						;	/*左侧组内偏差*/
	float									Average_R_Onside_Diff						;	/*右侧组内偏差*/
	float									Up_K_New		[DoublePumpQuad_Higt_MAX]	;	/**/
	float									Average_LR_Without_Diff						;	/*左右组件偏差*/
	float									last_Average_LR_Without_Diff				;	/**/

	float									Average_LF_RB								;
    float									Average_RF_LB								;
	float									LF_RB_Onside_Diff							;	/*左侧组内偏差*/
	float									RF_LB_Onside_Diff							;	/*右侧组内偏差*/
	float									Up_K_New_2		[DoublePumpQuad_Higt_MAX]	;	/**/
	float									LF_RB_Without_RF_LB_Diff					;	/*左右组件偏差*/
	uint8_t									Temp_ThrottleValve_F					: 1	;	/**/
	uint8_t									Temp_ThrottleValve_B					: 1	;	/**/	
	uint8_t									Sta_ThrottleValveControl				: 1	;	/*0-通过比例阀控制组间偏差 1-通过泄压阀控制组间偏差*/	
	float									Pitch_Diff									;	/*俯仰偏差 前高后低 为正*/
	float									last_Pitch_Diff								;	/*当输入调平命令时，锁存偏差值*/
	float									Roll_Diff									;	/*横滚偏差 左高右低 为正*/
	float									last_Roll_Diff								;	/*当输入调平命令时，锁存偏差值*/

	uint8_t 								Ctrl_Temp		[DoublePumpQuad_Higt_MAX]	;	/*升降 动作命令 0-不动作 1-升  2-降 */

	uint8_t									Highest_Site	: 4;	//最高点所在位置，0-左前 1-右前 2-左后 3-右后
	uint8_t									Lowest_Site		: 4;	//最高点所在位置，0-左前 1-右前 2-左后 3-右后

}DoublePumpQuad_LiftCtrl_Temp_TypeDef;

/*输出执行器结果  1电机 4个比例阀 8个电磁阀 2个节流阀*/
typedef struct
{
	float	MotorSpeed			;	/*上升 电机转速命令			单位 rpm*/

	float	UpProportionalValve[DoublePumpQuad_Higt_MAX];/*升降速度控制 比例阀命令   0-1.00 	单位 1%*/

	float	DownProportionalValve[DoublePumpQuad_Higt_MAX];/*升降速度控制 比例阀命令   0-1.00 	单位 1%*/

	uint8_t	Valve_LF	: 1	;	/*下降电磁阀 动作命令*/
	uint8_t	Valve_RF	: 1	;
	uint8_t	Valve_LB	: 1	;
	uint8_t	Valve_RB	: 1	;
	
	
	uint8_t	ThrottleValve_F	: 1	;	/*前侧泄压阀，关闭后高度固定 无法上升*/
	uint8_t	ThrottleValve_B	: 1	;	/*后侧泄压阀，关闭后高度固定 无法上升*/

}DoublePumpQuad_LiftCtrl_CtrlOut_TypeDef;

typedef struct
{
	//初始化参数
	DoublePumpQuad_LiftCtrl_CtrlPara_TypeDef	DPQ_Para	;	/*配置参数*/

	//输入参数
	DoublePumpQuad_LiftCtrl_Mode_TypeDef		DPQ_Mode	;
	DoublePumpQuad_LiftCtrl_CtrlIn_TypeDef		DPQ_In		;	/*高度传感器输入*/
	DoublePumpQuad_LiftCtrl_Cmd_TypeDef	 		DPQ_Cmd		;	/*升降逻辑命令*/
	//中间参数
	DoublePumpQuad_LiftCtrl_STM_TypeDef			PS_STM		;
	DoublePumpQuad_LiftCtrl_Temp_TypeDef		DPQ_Temp	;

	//输出参数
	DoublePumpQuad_LiftCtrl_CtrlOut_TypeDef	 	DPQ_Out		;	/*升降逻辑命令*/

}DoublePumpQuad_LiftCtrl_TypeDef;

app_DoublePumpQuad_LiftCtrl_GLOBALS_EXT void DoublePumpQuad_LiftCtrl_Main(DoublePumpQuad_LiftCtrl_TypeDef *p);
/*外部方法声明*/


#endif

/************************************END OF FILE************************************/

