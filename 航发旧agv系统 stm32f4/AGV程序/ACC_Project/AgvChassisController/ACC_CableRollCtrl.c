/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ACC_CableRollCtrl.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-10-29
	* Description:
	******************************************************************************
**/

#define  ACC_CableRollCtrl_GLOBALS

#include "ACC_Init.h"

#include "ACC_Project.h"
#include "IOConf.h"

/***********************************************************
** 函数名称:	ACC_CableRoll_Ctrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
uint8_t ACC_CableRoll_Ctrl_init(void)
{
	uint8_t Res = 0X00;
	do
	{
		//不是外部供电不用考虑
		if(g_Para.BatteryType != ACC_Cable_Power)				{Res = 0X01; break;}
		
		//限位没有初始化，不让用
		if(g_IOInputs.CableRoll_OutLimit.ID == ICN_Disable)		{Res = 0X02;break;}
		
		//报警没有初始化，不让用
		if(g_IOInputs.CableRoll_Emg.ID == ICN_Disable)			{Res = 0X03;break;}

	}while(0);
	
	return Res;
}
/***********************************************************
** 函数名称:	ACC_CableRoll_Ctrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
void ACC_CableRoll_CtrlProcess(void)
{
	ACC_OutputCtrl_TypeDef *p_IOCtrl = &AccData.IOCtrl;
	//遥控器控制卷线盘收线
	if (AccData.CalCmd.Button.CableRoll_Reel == 1)
	{
		p_IOCtrl->CableRoll_Reel = 1;
	}
	else
	{
		p_IOCtrl->CableRoll_Reel = 0;
	}

	//屏幕控制强制送电，或者其他逻辑给的
	if (0)
	{
		p_IOCtrl->CableRoll_ForcePowerOn = 1;
	}
	else
	{
		p_IOCtrl->CableRoll_ForcePowerOn = 0;
	}

}
/***********************************************************
** 函数名称:	ACC_CableRoll_Ctrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void ACC_CableRoll_Ctrl(void)
{

	static uint8_t STM = 0;
	//
	{
		switch(STM)
		{
			case 0:		/*初始化*/
			{
				if(ACC_CableRoll_Ctrl_init() == 0)
				{
					STM = 1;
				}
				else
				{
					STM = 2;
				}

			}break;
			case 1:		/*正常运行*/
			{
				ACC_CableRoll_CtrlProcess();
			}break;
			case 2:		/*没有此功能*/
			{
				
			}break;
		}
	}
}



/************************************END OF FILE************************************/
