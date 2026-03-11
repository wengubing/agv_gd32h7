/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_PushPullCtrl.h   
* Version    : V1.0		
* Author     : master		
* Date       : 2025.5.17         
* Description: 推拉控制模块
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_PushPullCtrl_h__
#define __app_PushPullCtrl_h__

/*包含以下头文件*/
#include <stdbool.h>

#include <stm32f4xx.h>

#define PushPullCtrl_MAX		2	//最大控制执行器数量（输入传感器小于等于它）


//
typedef struct
{
	uint8_t 	in_Push		: 1;	//输入命令 推送
	uint8_t 	in_Pull		: 1;	//输入命令 拉取

	uint8_t 	do_Push		: 1;	//执行命令 推送
	uint8_t 	do_Pull		: 1;	//执行命令 拉取

	uint8_t 	in_Gears	: 4;	//挡位
	uint8_t 	do_Gears	: 4;

	uint8_t 	do_Force	: 1;	//强制执行

}PushPullCtrl_Cmd_TypeDef;

//推拉机构类型
typedef enum
{
	PushPullType_Null 			=	0,	//无效
	PushPullType_TwoSvoRoller	=	1,	//双伺服驱动的滚筒推拉机构
}PushPullType_TypeDef;

//控制参数对象定义
typedef struct
{
	uint8_t 	CtrlNum			:	3	;	//控制点数量 	最多8个
	uint8_t 	MeasureNum		:	3	;	//检测点数量		最多8个
	float 		LimitPush						;	//推送软件限位
	float		LimitPull						;	//拉取软件限位

	uint16_t 	SpeedPlanPush	[6]	;	//推送挡位速度表
	uint16_t 	SpeedPlanPull	[6]	;	//拉取挡位速度表
	
}PushPullCtrl_CtrlPara_TypeDef;

//推拉状态
typedef enum
{
	PushPullCtrl_Null 		= 0,	//无效
	PushPullCtrl_Normal 	= 1,	//保持当前
	PushPullCtrl_LimitPush 	= 2,	//推限位
	PushPullCtrl_LimitPull 	= 3,	//拉限位
	PushPullCtrl_Push		= 4,	//推进行中
	PushPullCtrl_Pull 		= 5,	//拉进行中
}PushPullCtrl_STM_TypeDef;

/*输入传感器结果*/
typedef struct
{
	float	Distance	[PushPullCtrl_MAX]	;	/*推拉测距*/
	bool 	PushLimit	[PushPullCtrl_MAX]	;	/*推送限位*/
	bool	PullLimit	[PushPullCtrl_MAX]	;	/*拉取限位*/
}PushPullCtrl_CtrlIn_TypeDef;


/*输出执行器结果*/
typedef struct
{
	float	MotorSpeed	[PushPullCtrl_MAX]	;	/*电机转速命令 rpm*/
	bool 	PushCtrl	[PushPullCtrl_MAX]	;	/*推送 动作命令*/
	bool	PullCtrl	[PushPullCtrl_MAX]	;	/*拉取 动作命令*/
}PushPullCtrl_CtrlOut_TypeDef;

/*输出执行器结果*/
typedef struct
{
	uint8_t Ctrl_Temp	[PushPullCtrl_MAX]	;	/*推送 动作命令 0-不动作 1-推  2-拉 */
}PushPullCtrl_Temp_TypeDef;

typedef struct
{
	PushPullType_TypeDef			PR_Type		;	/*推拉平台类型*/
	PushPullCtrl_Cmd_TypeDef		PR_Cmd		;	/*推拉逻辑命令*/
	PushPullCtrl_CtrlPara_TypeDef	PR_CtrlPara	;	/*控制参数*/

	PushPullCtrl_CtrlIn_TypeDef		PR_CtrlIn	;	/*推拉逻辑输入传感器*/
	PushPullCtrl_CtrlOut_TypeDef	PR_CtrlOut	;	/*推拉逻辑输出执行器*/
	PushPullCtrl_STM_TypeDef 		PR_STM		;	/*推拉状态机*/

	PushPullCtrl_Temp_TypeDef		PR_Temp		;

}PushPull_StDef;



/*外部方法声明*/
void PushPulll_Main(PushPull_StDef* p);

#endif

/************************************END OF FILE************************************/

