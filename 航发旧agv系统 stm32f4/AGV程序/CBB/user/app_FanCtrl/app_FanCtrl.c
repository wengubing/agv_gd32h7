/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_FanCtrl.c  
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.2.1        
* Description: 散热风扇控制
*******************************************************************************/

//头文件
#include "app_FanCtrl.h"

//风扇控制函数
bool FanCtrl(FanCtrl_StDef *p)
{
	if(p==0)
		return 0;

	if(p->Mode == FanCtrl_Mode_Temperature)//温度控制
	{
		if(p->in_Temperature > p->in_TemperatureFanOn)
			p->FanCtrl = 1;
		else if(p->in_Temperature < p->in_TemperatureFanOff)
			p->FanCtrl = 0;
	}
	else if(p->Mode == FanCtrl_Mode_Time)//时间控制
	{
		if(p->FanCtrl==1)
		{
			if((p->in_Ms - p->CtrlLastTime)/1000 > p->in_TimeFanOn)//散热时间结束
			{
				p->CtrlLastTime = p->in_Ms;
				if(p->in_TimeFanOff != 0)
                	p->FanCtrl = 0;
			}
		}
		else if(p->FanCtrl==0)
		{
			if((p->in_Ms - p->CtrlLastTime)/1000 > p->in_TimeFanOff)//关闭时间结束
			{
                p->CtrlLastTime = p->in_Ms;
				if(p->in_TimeFanOn != 0)
                	p->FanCtrl = 1;
            }
		}
	}
	
	//风扇通过环境关闭 从超低车吸收
	if (p->out_Temperature_Environment > p->in_Temperature_Environment_FanOff && p->in_Temperature_Environment_FanOff < 65535)
		p->FanCtrl = 0;

	return p->FanCtrl;
}

//***********************************END OF FILE***********************************

