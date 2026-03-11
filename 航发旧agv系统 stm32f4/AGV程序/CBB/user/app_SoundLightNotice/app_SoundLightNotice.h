/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_SoundLightNotice.h   
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.1.18         
* Description: 输入系统状态，输出声光提示IO状态
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __app_SoundLightNotice_h__
#define __app_SoundLightNotice_h__

/*包含以下头文件*/
#include <stdbool.h>
#include "..\..\CBB\user\basecalfunc\common_def.h"

typedef enum
{
	SLN_SystemStat_Normal=0,								//正常
	SLN_SystemStat_Error,									//异常停车
	SLN_SystemStat_Warning,									//异常提醒
}SLN_SystemState_TypeDef;

typedef enum
{
	SLN_CtrlMode_Manual=0,									//遥控
	SLN_CtrlMode_Auto,										//自动
}SLN_CtrlMode_TypeDef;

typedef enum
{
	SLN_WorkMode_Walking=0,									//行走
	SLN_WorkMode_Still,										//静止
	SLN_WorkMode_Lifting,									//升降
	SLN_WorkMode_Others,									//其它
}SLN_WorkMode_TypeDef;

typedef enum
{
	SLN_ObsState_Normal=0,									//正常
	SLN_ObsState_OutSlowdown,								//外层触发行走减速
	SLN_ObsState_MiddleSlowdown,							//中层触发行走减速
	SLN_ObsState_Stop,										//内层触发行走停车
}SLN_ObsState_TypeDef;

//
typedef enum
{
	SLN_FLASH_null=0,										//
	SLN_FLASH_error,										//报警
	SLN_FLASH_mode_switch_ok,								//
	SLN_FLASH_motion_arrive,								//
	SLN_FLASH_wait_for_conf,								//
	SLN_FLASH_obs_stop,										//
	SLN_FLASH_obs_middle_slowdown,							//
	SLN_FLASH_obs_out_slowdown,								//
	SLN_FLASH_auto_ready,									//
	SLN_FLASH_auto_doing,									//
	SLN_FLASH_auto_justify,									//自动对正
	SLN_FLASH_normal,										//正常运行
	SLN_FLASH_stil,											//正常静止
}_SLN_FLASH;

typedef struct
{
	u16 t_on;												/*点亮时间*/
	u16 t_off;												/*熄灭时间*/
	u16 t_loop;												/*循环闪烁灭灯间隔*/
	u16 n;													/*闪烁次数*/
}SoundLightNotice_PWMPara_TypeDef;


typedef struct
{
	SoundLightNotice_PWMPara_TypeDef Normal;			/*正常动作中*/
	SoundLightNotice_PWMPara_TypeDef AutoDoing;			/**/
	SoundLightNotice_PWMPara_TypeDef AutoReady;			/**/
	SoundLightNotice_PWMPara_TypeDef ObsOutSlowdown;	/**/
	SoundLightNotice_PWMPara_TypeDef ObsMiddleSlowdown;	/**/
	SoundLightNotice_PWMPara_TypeDef ObsStop;			/**/
	SoundLightNotice_PWMPara_TypeDef WaitForConf;		/**/
	SoundLightNotice_PWMPara_TypeDef MotionArrive;		/**/
	SoundLightNotice_PWMPara_TypeDef ModeSwitchOk;		/**/
	SoundLightNotice_PWMPara_TypeDef Auto_Justify;		/*自动对正运行中*/
	SoundLightNotice_PWMPara_TypeDef Error;				/**/
	
}SoundLightNotice_FlashPara_TypeDef;


typedef struct __sound_light_notice
{
	u32 in_Ms;												//系统毫秒计数
	SLN_SystemState_TypeDef in_SystemState;					//系统状态
	SLN_CtrlMode_TypeDef in_CtrlMode;						//控制模式
	SLN_WorkMode_TypeDef in_WorkMode;						//工作模式
	/*
	避障状态
	（注意：行走时运动方向上有障碍实际触发才赋值相应状态）
	*/
	SLN_ObsState_TypeDef in_ObsState;
	bool in_FlgMotion;										//运动中标志
	bool in_FlgModeSwitchOk;								//模式切换到位标志
	bool in_FlgWaitForConf;									//等待用户确认标志
	bool in_FlgAutoReady;									//巡线/自动对中等功能满足条件标志
	bool in_FlgAutoDoing;									//巡线/自动对中等功能使能运行中标志
	bool in_FlgMotionArrive;								//升降/巡线到位/对中到位等自动调整功能到位后，使能一直有效标志
	bool in_FlgAutoJustify;									//自动对正
	SoundLightNotice_FlashPara_TypeDef FlashPara;			//项目自定义参数接口

	bool out_LightPower;									//电源指示灯
	bool out_LightSystem;									//系统
	bool out_LightAlarm;									//报警
	bool out_LampRed;										//三色 红
	bool out_LampGreen;										//三色 绿
	bool out_LampBlue;										//三色 蓝
	bool out_Buzzer;										//蜂鸣器
	bool out_LightCaution;									//爆闪警示
	bool out_ExplosionProof_LED;							//防爆指示灯

	_SLN_FLASH flash_mode_stm,flash_mode_in;				//
	u32 mode_in_ms_record;									//模式进入时间记录
	u16 mode_switch_ok_sound_time;							//
}SoundLightNotice_TypeDef;

/*外部方法声明*/
 u16 SoundLightNotice_Main(SoundLightNotice_TypeDef *p);
 u16 SoundLightNotice_InitExample(SoundLightNotice_TypeDef *p);

#endif

/************************************END OF FILE************************************/

