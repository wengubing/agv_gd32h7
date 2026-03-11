/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_SoundLightNotice.c   
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.1.18         
* Description: 输入系统状态，输出声光提示IO状态
*******************************************************************************/

//#define  __app_SoundLightNotice_globals__

/*包含以下头文件*/
#include "app_SoundLightNotice.h"
#include "..\..\CBB\user\basecalfunc\BoolFlashCtrl.h"
/*----------------------三原色混色原理-----------------------------------------
红绿->黄
红蓝->洋红
绿蓝->青
红绿蓝->白
------------------------------------------------------------------------------*/

/********************************** 设计笔记 *********************************** 
*增加声光提示闪烁PWM自定义接口ok 仿真测试ok
*
*******************************************************************************/


IOFlashCtrl_TypeDef io_system,io_red,io_green,io_blue,io_buzzer;

IOFlashCtrl_TypeDef io_ExplosionProof_LED; //防爆灯

//闪烁模式控制
void SoundLightNotice_FlashCtrl(SoundLightNotice_TypeDef *p)
{
	switch (p->flash_mode_stm)
	{
		case SLN_FLASH_null:
		{
			p->mode_in_ms_record=p->in_Ms;
			p->flash_mode_stm=p->flash_mode_in;
			
			break;
		}
		
		case SLN_FLASH_error:
		{
			if(p->flash_mode_stm!=p->flash_mode_in)
				p->flash_mode_stm=SLN_FLASH_null;
			else
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, p->FlashPara.Error.t_on, p->FlashPara.Error.t_off,0, 0, IOFlashCtrlMode_flash);
				IOFlashCtrl_Main(&io_green, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_blue, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_buzzer, p->in_Ms, p->FlashPara.Error.t_on, p->FlashPara.Error.t_off, 0,0, IOFlashCtrlMode_flash);
				//防爆灯
				IOFlashCtrl_Main(&io_ExplosionProof_LED, p->in_Ms, p->FlashPara.Error.t_on, p->FlashPara.Error.t_off, 0,0, IOFlashCtrlMode_flash);
			}
			break;
		}
		
		case SLN_FLASH_mode_switch_ok:
		{
			if(p->flash_mode_stm!=p->flash_mode_in && p->in_Ms - p->mode_in_ms_record > p->mode_switch_ok_sound_time)
				p->flash_mode_stm=SLN_FLASH_null;
			else if(p->in_Ms - p->mode_in_ms_record < p->mode_switch_ok_sound_time)
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, 0, 0,0, 0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_green, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_On);
				IOFlashCtrl_Main(&io_blue, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_buzzer, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_On);

				IOFlashCtrl_Main(&io_ExplosionProof_LED, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_On);
			}
			else//关闭声光提示
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, 0, 0,0, 0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_green, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_On);
				IOFlashCtrl_Main(&io_blue, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_buzzer, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);

				IOFlashCtrl_Main(&io_ExplosionProof_LED, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_On);
				//复位输入标志
				p->in_FlgModeSwitchOk=0;
			}
			break;
		}

		case SLN_FLASH_motion_arrive:
		{
			if(p->flash_mode_stm!=p->flash_mode_in)
				p->flash_mode_stm=SLN_FLASH_null;
			else
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, 0, 0,0, 0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_green, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_blue, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_On);
				IOFlashCtrl_Main(&io_buzzer, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_On);

				IOFlashCtrl_Main(&io_ExplosionProof_LED, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_On);
			}
			break;
		}

		case SLN_FLASH_wait_for_conf:
		{
			if(p->flash_mode_stm!=p->flash_mode_in)
				p->flash_mode_stm=SLN_FLASH_null;
			else
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, p->FlashPara.WaitForConf.t_on, p->FlashPara.WaitForConf.t_off,p->FlashPara.WaitForConf.t_loop, p->FlashPara.WaitForConf.n, IOFlashCtrlMode_FlashLoop);
				io_green.out_Ctrl=io_red.out_Ctrl;
				io_blue.out_Ctrl=io_red.out_Ctrl;
				io_buzzer.out_Ctrl=io_red.out_Ctrl;
				
				IOFlashCtrl_Main(&io_ExplosionProof_LED, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
			}
			break;
		}

		case SLN_FLASH_obs_stop:
		{
			if(p->flash_mode_stm!=p->flash_mode_in)
				p->flash_mode_stm=SLN_FLASH_null;
			else
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, 0, 0,0, 0, IOFlashCtrlMode_On);
				IOFlashCtrl_Main(&io_green, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_blue, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_buzzer, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_On);

				IOFlashCtrl_Main(&io_ExplosionProof_LED, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_On);
			}
			break;
		}

		case SLN_FLASH_obs_middle_slowdown:
		{
			if(p->flash_mode_stm!=p->flash_mode_in)
				p->flash_mode_stm=SLN_FLASH_null;
			else
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, p->FlashPara.ObsMiddleSlowdown.t_on, p->FlashPara.ObsMiddleSlowdown.t_off,0, 0, IOFlashCtrlMode_flash);
				io_green.out_Ctrl=io_red.out_Ctrl;
				IOFlashCtrl_Main(&io_blue, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				io_buzzer.out_Ctrl=io_red.out_Ctrl;

				IOFlashCtrl_Main(&io_ExplosionProof_LED, p->in_Ms,  p->FlashPara.ObsMiddleSlowdown.t_on, p->FlashPara.ObsMiddleSlowdown.t_off, 0,0, IOFlashCtrlMode_flash);
			}
			break;
		}

		case SLN_FLASH_obs_out_slowdown:
		{
			if(p->flash_mode_stm!=p->flash_mode_in)
				p->flash_mode_stm=SLN_FLASH_null;
			else
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, p->FlashPara.ObsOutSlowdown.t_on, p->FlashPara.ObsOutSlowdown.t_off,0, 0, IOFlashCtrlMode_flash);
				io_green.out_Ctrl=io_red.out_Ctrl;
				IOFlashCtrl_Main(&io_blue, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				io_buzzer.out_Ctrl=io_red.out_Ctrl;

				IOFlashCtrl_Main(&io_ExplosionProof_LED,  p->in_Ms, p->FlashPara.ObsOutSlowdown.t_on, p->FlashPara.ObsOutSlowdown.t_off,0, 0, IOFlashCtrlMode_flash);
			}
			break;
		}

		case SLN_FLASH_auto_ready:
		{
			if(p->flash_mode_stm!=p->flash_mode_in)
				p->flash_mode_stm=SLN_FLASH_null;
			else
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_green, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_blue, p->in_Ms, p->FlashPara.AutoReady.t_on, p->FlashPara.AutoReady.t_off, 0,0, IOFlashCtrlMode_flash);
				IOFlashCtrl_Main(&io_buzzer, p->in_Ms, p->FlashPara.AutoReady.t_on, p->FlashPara.AutoReady.t_off, 0,0, IOFlashCtrlMode_flash);
				
				IOFlashCtrl_Main(&io_ExplosionProof_LED, p->in_Ms,  p->FlashPara.AutoReady.t_on, p->FlashPara.AutoReady.t_off, 0,0, IOFlashCtrlMode_flash);
			}
			break;
		}

		case SLN_FLASH_auto_doing:
		{
			if(p->flash_mode_stm!=p->flash_mode_in)
				p->flash_mode_stm=SLN_FLASH_null;
			else
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_green, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_blue, p->in_Ms, p->FlashPara.AutoDoing.t_on, p->FlashPara.AutoDoing.t_off, 0,0, IOFlashCtrlMode_flash);
				IOFlashCtrl_Main(&io_buzzer, p->in_Ms, p->FlashPara.AutoDoing.t_on, p->FlashPara.AutoDoing.t_off, 0,0, IOFlashCtrlMode_flash);
				
				IOFlashCtrl_Main(&io_ExplosionProof_LED, p->in_Ms, p->FlashPara.AutoDoing.t_on, p->FlashPara.AutoDoing.t_off, 0,0, IOFlashCtrlMode_flash);
			}
			break;
		}
		case SLN_FLASH_auto_justify:
		{
			if(p->flash_mode_stm!=p->flash_mode_in)
				p->flash_mode_stm=SLN_FLASH_null;
			else
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, p->FlashPara.Auto_Justify.t_on, p->FlashPara.Auto_Justify.t_off, 0,0, IOFlashCtrlMode_flash);
				IOFlashCtrl_Main(&io_green, p->in_Ms, p->FlashPara.Auto_Justify.t_on, p->FlashPara.Auto_Justify.t_off, 0,0, IOFlashCtrlMode_flash);
				IOFlashCtrl_Main(&io_blue, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_buzzer, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				
				IOFlashCtrl_Main(&io_ExplosionProof_LED, p->in_Ms, p->FlashPara.Auto_Justify.t_on, p->FlashPara.Auto_Justify.t_off, 0,0, IOFlashCtrlMode_flash);
			}
			break;
		}

		case SLN_FLASH_normal:
		{
			if(p->flash_mode_stm!=p->flash_mode_in)
				p->flash_mode_stm=SLN_FLASH_null;
			else
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_green, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_blue, p->in_Ms, p->FlashPara.Normal.t_on, p->FlashPara.Normal.t_off, 0,0, IOFlashCtrlMode_flash);
				IOFlashCtrl_Main(&io_buzzer, p->in_Ms, p->FlashPara.Normal.t_on, p->FlashPara.Normal.t_off, 0,0, IOFlashCtrlMode_flash);

				IOFlashCtrl_Main(&io_ExplosionProof_LED, p->in_Ms, p->FlashPara.Normal.t_on, p->FlashPara.Normal.t_off, 0,0, IOFlashCtrlMode_flash);
			}
			break;
		}

		case SLN_FLASH_stil:
		{
			if(p->flash_mode_stm!=p->flash_mode_in)
				p->flash_mode_stm=SLN_FLASH_null;
			else
			{
				IOFlashCtrl_Main(&io_red, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_green, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_On);
				IOFlashCtrl_Main(&io_blue, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
				IOFlashCtrl_Main(&io_buzzer, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);

				IOFlashCtrl_Main(&io_ExplosionProof_LED, p->in_Ms, 0, 0, 0,0, IOFlashCtrlMode_Off);
			}
			break;
		}
		
		default:
		{
			//if(p->flash_mode_stm!=p->flash_mode_in)
			//	p->flash_mode_stm=SLN_FLASH_null;
			break;
		}
	}
}

//
void SoundLightNotice_GetFlashMode(SoundLightNotice_TypeDef *p)
{
	if(p->in_SystemState==SLN_SystemStat_Error)
	{
		p->flash_mode_in=SLN_FLASH_error;
	}
	else if(p->in_FlgModeSwitchOk)
	{
		p->flash_mode_in=SLN_FLASH_mode_switch_ok;
	}
	else if(p->in_FlgMotionArrive)
	{
		p->flash_mode_in=SLN_FLASH_motion_arrive;
	}
	else if(p->in_FlgWaitForConf)
	{
		p->flash_mode_in=SLN_FLASH_wait_for_conf;
	}
	else if(p->in_ObsState==SLN_ObsState_Stop)
	{
		p->flash_mode_in=SLN_FLASH_obs_stop;
	}
	else if(p->in_ObsState==SLN_ObsState_MiddleSlowdown)
	{
		p->flash_mode_in=SLN_FLASH_obs_middle_slowdown;
	}
	else if(p->in_ObsState==SLN_ObsState_OutSlowdown)
	{
		p->flash_mode_in=SLN_FLASH_obs_out_slowdown;
	}
	else
	{
		if(p->in_FlgAutoReady)
		{
			p->flash_mode_in=SLN_FLASH_auto_ready;
		}
		else if(p->in_FlgAutoDoing)
		{
			p->flash_mode_in=SLN_FLASH_auto_doing;
		}
		else if(p->in_FlgMotion)
		{
			p->flash_mode_in=SLN_FLASH_normal;
		}
		else if(p->in_FlgAutoJustify)
		{
			p->flash_mode_in=SLN_FLASH_auto_justify;
		}
		else
			p->flash_mode_in=SLN_FLASH_stil;
	}
}

/*模块主方法*/
u16 SoundLightNotice_Main(SoundLightNotice_TypeDef *p)
{
	
	u16 res=0;
	p->mode_switch_ok_sound_time=p->FlashPara.ModeSwitchOk.t_on;
	
	//电源灯常亮
	p->out_LightPower=1;

	//系统指示灯常闪烁
	IOFlashCtrl_Main(&io_system, p->in_Ms, 500, 500,0,0, IOFlashCtrlMode_flash);

	//报警指示等仅在报警停车、报警提醒时通电
	if(p->in_SystemState==SLN_SystemStat_Error || p->in_SystemState==SLN_SystemStat_Warning)
		p->out_LightAlarm=1;
	else
		p->out_LightAlarm=0;

	//爆闪灯 仅在设备动作中通电
	p->out_LightCaution=p->in_FlgMotion;

	//三色灯、蜂鸣器
	SoundLightNotice_GetFlashMode(p);
	SoundLightNotice_FlashCtrl(p);

	//输出IO状态
	p->out_LightSystem=io_system.out_Ctrl;
	p->out_LampRed=io_red.out_Ctrl;
	p->out_LampGreen=io_green.out_Ctrl;
	p->out_LampBlue=io_blue.out_Ctrl;
	p->out_Buzzer=io_buzzer.out_Ctrl;
	p->out_ExplosionProof_LED=io_ExplosionProof_LED.out_Ctrl;
	return res;
}

/**************************************************************************
* 函数名:SoundLightNotice_InitExample
* 功  能:模块单独测试时的初始化内容，实际外部使用模块时可参考编写
* 参  数:SoundLightNotice_TypeDef *p
* 返回值:无
**************************************************************************/
u16 SoundLightNotice_InitExample(SoundLightNotice_TypeDef *p)
{
	/*标准参数，非特殊项目按以下值设置（除待用户确认模式支持t_loop和n参数外，其它模式不支持，不用设置*/
	p->FlashPara.Normal.t_on=500;
	p->FlashPara.Normal.t_off=500;

	p->FlashPara.AutoDoing.t_on=300;
	p->FlashPara.AutoDoing.t_off=300;

	p->FlashPara.AutoReady.t_on=100;
	p->FlashPara.AutoReady.t_off=500;
	p->FlashPara.AutoReady.t_loop=0;

	p->FlashPara.ObsOutSlowdown.t_on=300;
	p->FlashPara.ObsOutSlowdown.t_off=300;

	p->FlashPara.ObsMiddleSlowdown.t_on=200;
	p->FlashPara.ObsMiddleSlowdown.t_off=200;

	/*内层触发停车（摇杆推着时）：红色常亮（蜂鸣器长鸣），设计已固定，参数设置了也不会起作用，不用去设置
	p->FlashPara.ObsStop.t_on=300;
	p->FlashPara.ObsStop.t_off=300;*/

	p->FlashPara.WaitForConf.t_on=150;
	p->FlashPara.WaitForConf.t_off=150;
	p->FlashPara.WaitForConf.t_loop=1000;
	p->FlashPara.WaitForConf.n=2;

	/*模式切换到位:绿色常亮,蜂鸣器通电长鸣1000ms后关闭，仅这个1000ms可设置*/
	p->FlashPara.ModeSwitchOk.t_on=1000;

	/*升降/巡线到位/对中到位等自动调整功能到位后，使能一直有效时：绿色常亮、蜂鸣长鸣。不支持修改设置
	p->FlashPara.MotionArrive.t_on=150;*/

	p->FlashPara.Error.t_on=150;
	p->FlashPara.Error.t_off=300;
	/*自动对中*/
	p->FlashPara.Auto_Justify.t_on = 300;
	p->FlashPara.Auto_Justify.t_off = 300;
	
	
	return 1;
}

/************************************END OF FILE************************************/

