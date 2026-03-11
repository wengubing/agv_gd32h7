/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_IOFilter.c  
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2024.2.1        
* Description: 输入实时IO值，输出滤波值
*******************************************************************************/

/*包含以下头文件*/
#include <string.h>
#include <stdio.h>
#include "app_IOFilter.h"

/**************************************************************************
* 函数名:  IOFilter_ButtonScan()
* 功  能: 按钮扫描任务(定时调用10ms)
* 参  数: 模块对象地址
* 返回值: 无
**************************************************************************/
void IOFilter_ButtonScan(IOFilter_TypeDef *p, u32 in_Ms)
{
	p->in_Ms=in_Ms;
	if(p->in_RealTimeStatus)
	{/*按钮按下*/
		p->ButtonReleaseCnt = 0;
		if(p->ButtonPressCnt < p->PressTimePara.PressLongDown)
			p->ButtonPressCnt++;

		if(p->ButtonPressCnt >= p->PressTimePara.PressLongDown)
			p->out_Status = IOFilterButtonStatus_PressLongDown;
		else if(p->ButtonPressCnt >= p->PressTimePara.PressShortLongDown)
			p->out_Status = IOFilterButtonStatus_PressShortLongDown;
		else if (p->ButtonPressCnt >= p->PressTimePara.PressNormalDown)
			p->out_Status = IOFilterButtonStatus_PressNormalDown;
		else if (p->ButtonPressCnt >= p->PressTimePara.PressMidDown)
			p->out_Status = IOFilterButtonStatus_PressMidDown;
		else if (p->ButtonPressCnt >= p->PressTimePara.PressShortDown)
			p->out_Status = IOFilterButtonStatus_PressShortDown;
//		else
//			p->out_Status = IOFilterButtonStatus_Release;
	}
	else
	{/*按钮未按下或按下后释放*/
		if (p->out_Status >= IOFilterButtonStatus_PressShortDown)
		{/*上次,有效按下*/
			if (p->ButtonReleaseCnt++ >= 2)
			{
				p->ButtonPressCnt = 0;
				p->ButtonReleaseCnt = 0;
				p->out_Status = IOFilterButtonStatus_Release;/*置释放*/
			}
		}
		else if (p->out_Status == IOFilterButtonStatus_Release)
		{
			if (p->ButtonReleaseCnt++ >= 1)
			{
				p->ButtonReleaseCnt = 0;
				/*从"置释放"->"无操作",延迟20ms，给cpu去查询"置释放"这个状态*/
				p->out_Status = IOFilterButtonStatus_None;
			}
		}
		else
		{
			if (p->ButtonReleaseCnt++ >= 1)
			{
				p->ButtonReleaseCnt = 0;
				p->out_Status = IOFilterButtonStatus_None;
			}
		}
	}
}

/**************************************************************************
* 函数名:IOFilter_InitExample
* 功  能:模块单独测试时的初始化内容，实际外部使用模块时可参考编写
* 参  数:DataFilter_TypeDef *p
* 返回值:无
**************************************************************************/
bool IOFilter_InitExample(IOFilter_TypeDef *p)
{
	p->PressTimePara.PressShortDown = 100;
	p->PressTimePara.PressMidDown = 500;
	p->PressTimePara.PressNormalDown = 1000;
	p->PressTimePara.PressShortLongDown = 2000;
	p->PressTimePara.PressLongDown = 3000;
	return 0;
}

/*cpp 调用c函数测试*/
void CppCallC(void)
{
	printf("Hello,C! \n");
	//return 0;
}

/************************************END OF FILE************************************/

