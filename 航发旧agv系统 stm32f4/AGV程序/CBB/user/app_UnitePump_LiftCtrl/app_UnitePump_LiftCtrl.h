/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_UnitePump_LiftCtrl.h   
* Version    : V1.0		
* Author     : master		
* Date       : 2026.01.17
* Description: 四点液压升降控制
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_UnitePump_LiftCtrl_h__
#define __app_UnitePump_LiftCtrl_h__

#ifdef  app_UnitePump_LiftCtrl_GLOBALS
 #define app_UnitePump_LiftCtrl_GLOBALS_EXT
#else
 #define app_UnitePump_LiftCtrl_GLOBALS_EXT  extern
#endif 

/*包含以下头文件*/
#include <stdbool.h>
#include <stm32f4xx.h>

//平台状态
typedef enum
{
	UnitePump_LF	= 0,	//
	UnitePump_RF	= 1,	//
	UnitePump_LB	= 2,	//
	UnitePump_RB	= 3,	//
	UnitePump_MAX	= 4,
}UnitePump_Orientation_TypeDef; 

//平台状态
typedef enum
{
	UnitePump_LiftCtrl_Null 		= 0,	//无效
	UnitePump_LiftCtrl_Normal 		= 1,	//保持当前
	UnitePump_LiftCtrl_LimitUp 		= 2,	//升限位
	UnitePump_LiftCtrl_LimitDown	= 3,	//降限位
	UnitePump_LiftCtrl_Up			= 4,	//升进行中
	UnitePump_LiftCtrl_Down 		= 5,	//降进行中
	//UnitePump_LiftCtrl_Level 		= 6,	//调平进行中
	UnitePump_LiftCtrl_LevelFalish	= 7,	//调平完成
}UnitePump_LiftCtrl_STM_TypeDef;

//控制模式
typedef enum
{
	UnitePump_High 		= 0,	//高度调节
	UnitePump_Pitch 	= 10,	//俯仰调节
	UnitePump_Roll 		= 20,	//横滚调节
}UnitePump_LiftCtrl_Mode_TypeDef;

//到位功能判定区分
typedef enum
{
	UnitePump_Arrival_Average	= 0,	//平均抵达限位就到位
	UnitePump_Arrival_All 		= 10 ,	//全部抵达限位才到位
	UnitePump_Arrival_One 		= 20,	//一个抵达限位就到位
}UnitePump_Arrival_Fun_TypeDef;

//控制参数对象定义
typedef struct
{
	int16_t 						HighSpeedPlan		[6]	;	//升降挡位速度表
					
	float 							HighLimit			[UnitePump_MAX]	;	/*上限位*/
	float							LowLimit			[UnitePump_MAX]	;	/*下限位*/
	float  							High_Up_Limit						;	/*上升 调平的最小结果 小于此认为调节完成*/
	float  							High_Up_DeadZne						;	/*上升 偏差大于 此认为需要调节*/
	float  							High_Down_Limit						;	/*下降 调平的最小结果 小于此认为调节完成*/
	float  							High_Down_DeadZne					;	/*下降 偏差大于 此认为需要调节*/

	float 							PitchLimit							;	/*俯仰限位*/
	float  							Pitch_Level_Limit					;	/*俯仰 调平的最小结果 小于此认为调节完成*/
	float  							Pitch_Level_DeadZne					;	/*俯仰偏差大于 此认为需要调节*/
					
	float 							RollLimit							;	/*横滚限位*/
	float  							Roll_Level_Limit					;	/*横滚 调平的最小结果 小于此认为调节完成*/
	float  							Roll_Level_DeadZne					;	/*横滚偏差大于 此认为需要调节*/

	float							HighShowDis							;	/*提前减速距离*/

	UnitePump_Arrival_Fun_TypeDef	Fun_Arrival						;	//所有高度数据达到后才停止

}UnitePump_LiftCtrl_CtrlPara_TypeDef;

/*控制命令定义*/
typedef struct
{
	uint8_t 	in_Up		: 1;	//输入命令 上升
	uint8_t 	in_Down		: 1;	//输入命令 下降

	uint8_t 	do_Up		: 1;	//执行命令 上升 
	uint8_t 	do_Down		: 1;	//执行命令 下降

	uint8_t 	do_Force	: 1;	//强制执行

	uint8_t 	in_Gears	: 4;	//挡位
	uint8_t 	do_Gears	: 4;
}UnitePump_LiftCtrl_Cmd_TypeDef;
 
/*输入传感器结果*/
typedef struct
{
	float		Distance	[4]	;	/*4点高度	单位mm*/
	float		Sensor		[4]	;	/*4点压力 	单位 Mpa*/

}UnitePump_LiftCtrl_CtrlIn_TypeDef;


/*输出执行器结果  1电机 4个比例阀 8个电磁阀 2个节流阀*/
typedef struct
{
	float	MotorSpeed			;	/*电机转速命令			单位 rpm*/

	uint8_t	Up_Valve_LF		: 1	;	/*上升电磁阀 动作命令*/
	uint8_t	Up_Valve_RF		: 1	;
	uint8_t	Up_Valve_LB		: 1	;
	uint8_t	Up_Valve_RB		: 1	;
	
	uint8_t	Down_Valve_LF	: 1	;	/*下降电磁阀 动作命令*/
	uint8_t	Down_Valve_RF	: 1	;
	uint8_t	Down_Valve_LB	: 1	;
	uint8_t	Down_Valve_RB	: 1	;

}UnitePump_LiftCtrl_CtrlOut_TypeDef;

/*中间过程*/
typedef struct
{
	UnitePump_LiftCtrl_CtrlOut_TypeDef		Temp_CtrlOut					;
	uint32_t								Last_Ms							;	/*上次时间*/
	float									Last_High		[UnitePump_MAX]	;	/*上次高度*/
	float									High_Speed		[UnitePump_MAX]	;	/*升降速度*/

	float									Average							;	/*4点平均高度	单位mm*/
	float									Average_F						;	/*前侧2点 平均高度	单位mm*/
	float									Average_B						;	/*前侧2点 平均高度	单位mm*/
	float									Average_L						;	/*前侧2点 平均高度	单位mm*/
	float									Average_R						;	/*前侧2点 平均高度	单位mm*/

	uint8_t 								High_Limit_Sta	[UnitePump_MAX]	;	/*升降限位触发状态 0-未抵达限位 1-抵达上限位  2-抵达下限位
																					3-抵达上限速位置 4-抵达下限速位置*/
	float									Highest							;	/*最高点的高度	单位mm*/
	float									Highest_Diff	[UnitePump_MAX]	;	/*和最高点的高差,最低点比此点位高多少mm*/
	float									Lowest							;	/*最低点的高度	单位mm*/
	float									Lowest_Diff		[UnitePump_MAX]	;	/*和最低点的高差,此点位比最低点高多少mm*/
	float									High_Diff						;	/*最高点比最低点  高多少mm*/

	float									Pitch_Diff						;	/*俯仰偏差 前高后低 为正	单位mm*/
	float									Roll_Diff						;	/*横滚偏差 左高右低 为正	单位mm*/

	uint8_t 								Ctrl_Temp		[UnitePump_MAX]	;	/*升降 动作命令 0-不动作 1-升  2-降 */

	uint8_t									Highest_Site		: 4;	//最高点所在位置，0-左前 1-右前 2-左后 3-右后
	uint8_t									Lowest_Site			: 4;	//最高点所在位置，0-左前 1-右前 2-左后 3-右后

	uint8_t									ArrivalHighLimit	: 1;	//抵达软限位， 1-抵达 0-未抵达
	uint8_t									ArrivalLowLimit		: 1;
	uint8_t									ArrivalHighShow		: 1;	//抵达限速距离 1-抵达 0-未抵达
	uint8_t									ArrivalLowShow		: 1;

}UnitePump_LiftCtrl_Temp_TypeDef;



typedef struct
{
	//初始化参数
	UnitePump_LiftCtrl_CtrlPara_TypeDef	UPL_Para	;	/*配置参数*/

	//输入参数
	uint32_t							UPL_Ms		;
	UnitePump_LiftCtrl_Mode_TypeDef		UPL_Mode	;
	UnitePump_LiftCtrl_CtrlIn_TypeDef	UPL_In		;	/*高度传感器输入*/
	UnitePump_LiftCtrl_Cmd_TypeDef		UPL_Cmd		;	/*升降逻辑命令*/
	//中间参数
	UnitePump_LiftCtrl_STM_TypeDef		UPL_STM		;
	UnitePump_LiftCtrl_Temp_TypeDef		UPL_Temp	;

	//输出参数
	UnitePump_LiftCtrl_CtrlOut_TypeDef	UPL_Out		;	/*升降逻辑命令*/

}UnitePump_LiftCtrl_TypeDef;

app_UnitePump_LiftCtrl_GLOBALS_EXT void UnitePump_LiftCtrl_Main(UnitePump_LiftCtrl_TypeDef *p);
/*外部方法声明*/


#endif

/************************************END OF FILE************************************/

