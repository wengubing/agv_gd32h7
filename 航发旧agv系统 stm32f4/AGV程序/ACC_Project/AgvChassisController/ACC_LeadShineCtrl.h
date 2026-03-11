/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ACC_LeadShineCtrl.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-02-07         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
/*自定义引用*/
#include <stdbool.h>
#include <stm32f4xx.h>
/*define-----------------------------------------------------------------------*/

#ifndef __ACC_LEADSHINECTRL_H_
#define __ACC_LEADSHINECTRL_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ACC_LEADSHINE_GLOBALS
	#define ACC_LEADSHINE_GLOBALS_EXT
#else
	#define ACC_LEADSHINE_GLOBALS_EXT  extern
#endif


/*雷赛升降模块需要输入的底盘状态*/
typedef enum
{
	LeadShine_Null			=0,	/*初始化		*/
	LeadShine_Still			=1,	/*静止		*/
	LeadShine_PosTrace		=2,	/*正向巡线	*/
	LeadShine_InfTrace		=3,	/*横向巡线	*/
	LeadShine_Rotation		=4,	/*自转		*/
	LeadShine_Lift			=5,	/*升降		*/
	LeadShine_Absorption	=6,	/*悬挂减震	*/
}ACC_LeadShine_Mode_TypeDef;

/*雷赛升降模块运行状态*/
typedef enum
{
	LeadShine_Initialization 	= 0,	/*初始化*/
	LeadShine_InMotion			= 1,	/*动作中*/
	LeadShine_ActionCompleted	= 2,	/*动作完成*/
}ACC_LeadShine_Sta_TypeDef;

/*雷赛升降模块报警状态*/
typedef enum
{
	LeadShine_SystemNormal			= 0,	/*系统正常*/
	LeadShine_CommunicationAnomaly	= 1,	/*通信异常*/
	LeadShine_ReturnZeroException	= 2,	/*回零异常*/
	LeadShine_AbnormalMovement		= 3,	/*动作异常*/
	LeadShine_AbnormalCurrent		= 4,	/*电流异常*/
}ACC_LeadShine_Alarm_TypeDef;

/*雷赛升降模块控制模式*/
typedef enum
{
	LeadShine_EmptyMode 		= 0,	/*空模式*/
	LeadShine_FullMotionMode	= 1,	/*全动模式*/
	LeadShine_SingleActionMode	= 2,	/*单动模式*/
	LeadShine_AbnormalStopMode	= 3,	/*异常停止模式*/
}ACC_LeadShine_ControlMode_TypeDef;

/*初始化参数*/
typedef struct
{
	uint8_t  							LeadShineCtrl_FuncEn	:1	;/*雷赛伺服功能是否启用 0-没有相关功能 1-启用相关功能*/
	uint8_t  							LeadShineType_MCB		:1	;/*直连的雷赛伺服*/
	uint8_t  							LeadShineType_SerialExp	:1	;/*串口转接板的雷赛伺服*/
	uint8_t  							Tarce_Slam_Magnetic		:1	;/*Slam 磁导航融合巡线 控制逻辑和单纯的磁导航不太一样*/
	uint8_t	 							Para_NC					:5	;/*占位*/
	uint8_t								Inst_High					;/*雷赛伺服的安装高度 单位mm*/
}ACC_LeadShineCtrl_Para_StDef;	

/*输入参数*/
typedef struct
{
	uint8_t  							LeadShine_Force			:1	;/*雷赛伺服强制回收到位		*/
	uint8_t  							AGV_Remote				:1	;/*是否遥控状态 1-遥控 		0-自动	*/
	uint8_t  							AGV_Justify				:1	;/*是否对中状态 1-对中进行中	0-没有对中	*/
	uint8_t  							AGV_Extend				:1	;/*自动框架的伸出控制接口 1-伸出 0-无需伸出*/

	uint8_t	 							Input_NC				:6	;/*占位*/
	ACC_LeadShine_Mode_TypeDef			AGV_Mode					;/*AGV当前模式，雷赛伺服根据模式 自动确定是否需要伸出雷赛伺服*/
	float								AGV_Height					;/*AGV当前高度，整车高度或者悬挂高度 雷赛伺服需要同步对应高度*/

}ACC_LeadShineCtrl_Input_StDef;	

/*中间变量*/
typedef struct
{
	ACC_LeadShine_Mode_TypeDef			Last_AGV_Mode			;/*上一个模式  用状态机跳转*/
	float								Latch_AGV_Height		;/*锁存当前AGV高度 让雷赛伺服的目标控制不要时刻跳动*/
	ACC_LeadShine_ControlMode_TypeDef 	Temp_ControlMode		;
	float								Temp_ControlHeight		;/*雷赛伺服 控制高度*/
}ACC_LeadShineCtrl_Temp_StDef;

/*输出参数*/
typedef struct
{
	/*用于其他应用模块的雷赛状态*/
	ACC_LeadShine_Sta_TypeDef			LeadShine_Sta		;
	ACC_LeadShine_Alarm_TypeDef			LeadShine_Alarm		;
	/*用于雷赛伺服控制的数据*/
	ACC_LeadShine_ControlMode_TypeDef 	ControlMode			;
	float								ControlHeight		;/*雷赛伺服 控制高度*/

}ACC_LeadShineCtrl_Output_StDef;

/*结构体*/
typedef struct
{
	/*初始化参数*/
	ACC_LeadShineCtrl_Para_StDef		LS_Para;

	/*输入参数*/
	ACC_LeadShineCtrl_Input_StDef		LS_Input;

	/*中间变量*/
	ACC_LeadShineCtrl_Temp_StDef		LS_Temp;

	/*输出参数*/
	ACC_LeadShineCtrl_Output_StDef		LS_Output;

}ACC_LeadShineCtrl_StDef;	
	
ACC_LEADSHINE_GLOBALS_EXT	void ACC_LeadShineCtrl(ACC_LeadShineCtrl_StDef *p);
#endif


 

