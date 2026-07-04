/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_IOFlashCtrl.c   
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.1.1         
* Description: IO不同闪烁方式实现
*******************************************************************************/

#include "BoolFlashCtrl.h"


void IOFlashCtrl_Flash(IOFlashCtrl_TypeDef *p)
{
	if(p->out_Ctrl==1)
	{
		if(p->in_Ms - p->CtrlMsRecord > p->t_on)
		{
			p->out_Ctrl=0;
		}
	}
	else if(p->out_Ctrl==0)
	{
		if(p->in_Ms - p->CtrlMsRecord > p->t_off)
		{
			p->out_Ctrl=1;
			p->n_record++;
		}
	}
}

/*闪烁控制*/
bool IOFlashCtrl_Main(IOFlashCtrl_TypeDef *p, u32 ms, u16 t_on, u16 t_off, u16 t_loop, u16 n, IOFlashCtrlMode_TypeDef mode)
{
	p->in_Ms=ms;
	p->in_Mode=mode;
	p->t_on=t_on;
	p->t_off=t_off;
	p->t_loop=t_loop;
	p->n=n;

	//复位io控制对象内部状态
	if(p->ModePre != p->in_Mode)
	{
		p->ModePre = p->in_Mode;
		p->n_record=0;
		p->CtrlMsRecord = p->in_Ms;
	}
	
	if(p->in_Mode==IOFlashCtrlMode_Off)
	{
		p->out_Ctrl=0;
	}
	else if(p->in_Mode==IOFlashCtrlMode_On)
	{
		p->out_Ctrl=1;
	}
	else if(p->in_Mode==IOFlashCtrlMode_flash)
	{
		IOFlashCtrl_Flash(p);
	}
	else if(p->in_Mode==IOFlashCtrlMode_FlashLimit)
	{
		if(p->n_record<p->n)
			IOFlashCtrl_Flash(p);
		else
			p->out_Ctrl=0;	
	}
	else if(p->in_Mode==IOFlashCtrlMode_FlashLoop)
	{
		if(p->n_record<=p->n)
		{
			IOFlashCtrl_Flash(p);
		}
		else
		{
			p->out_Ctrl=0;
			if(p->in_Ms - p->CtrlMsRecord > p->t_loop)
			{
				p->CtrlMsRecord = p->in_Ms;
				p->n_record=0;
			}
		}
	}

	if(p->out_CtrlPre != p->out_Ctrl)
	{
		p->out_CtrlPre = p->out_Ctrl;
		p->CtrlMsRecord = p->in_Ms;
	}
	return p->out_Ctrl;
}

/************************************END OF FILE************************************/

