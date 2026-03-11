/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ACC_AutoCruise.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       :  2025-09-19
	* Description:
	******************************************************************************
**/


#define  ACC_AutoCruise_GLOBALS
#include "ACC_AutoCruise.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "CommConf.h"
#include "IOConf.h"


static uint8_t *p_STM = &(g_AutoCruise_Data.Temp.AutoCruise_STM);

/***********************************************************
** 函数名称:	AutoCruise_Ctrl_Init
** 功能描述: 	自动巡航功能相关参数初始化
** 参    数: 	None
** 返 回 值:	None
***********************************************************/
void AutoCruise_Ctrl_Init(void)
{
	
}
/***********************************************************
** 函数名称:	AutoCruise_Output_Clear
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void AutoCruise_Output_Clear(void)
{

	ACC_AutoCruise_Temp_StDef 		*p_Temp		= &(g_AutoCruise_Data.Temp);
	ACC_AutoCruise_OutputData_StDef	*p_Output	= &(g_AutoCruise_Data.Output);
	p_Output->Vx 	= 0;
	p_Output->Vy 	= 0;
	p_Output->Vz 	= 0;
	p_Temp->Vx_temp = 0;
	p_Temp->Vy_temp = 0;
	p_Temp->Vz_temp = 0;
	
}

/***********************************************************
** 函数名称:	AutoCruise_Input_Ctrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void AutoCruise_Input_Ctrl(void)
{
	//从遥控器原始数据获取
	{

		ACC_AutoCruise_371_WHData_StDef	*p_Input	= &(g_AutoCruise_Data.Input.WHData);
		p_Input->XAxisOutput 	= g_WHData.XAxisOutput;
		p_Input->YAxisOutput 	= g_WHData.YAxisOutput;
		p_Input->ZAxisOutput 	= g_WHData.ZAxisOutput;
		p_Input->EmgStop 		= g_WHData.EmgStop;
		p_Input->J41 			= g_WHData.J41;
		p_Input->J42 			= g_WHData.J42;
		p_Input->J43 			= g_WHData.J43;
		p_Input->J44			= g_WHData.J44;
	}

}

/***********************************************************
** 函数名称:	AutoCruise_Output_Ctrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void AutoCruise_Output_Ctrl(void)
{
	if(*p_STM == 2)
	{
		ACC_AutoCruise_OutputData_StDef	*p_OutData		= &(g_AutoCruise_Data.Output);
		OrgCtrlCmd_TypeDef				*p_CtrlCmdCmd	= &(g_ChannelSwitch.CtrlCmd);
		//遥感命令
		{
			ACC_Joystick_TypeDef		*p_AccVxyz = &(p_CtrlCmdCmd->Vxyz);
			p_AccVxyz->x = p_OutData->Vx;
			p_AccVxyz->y = p_OutData->Vy;
			p_AccVxyz->z = p_OutData->Vz;
		}
	}
}

/***********************************************************
** 函数名称:	AutoCruise_WHData_Check
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void AutoCruise_WHData_Check(void)
{
	
}


/***********************************************************
** 函数名称	AutoCruise_STM_Ctrl
** 功能描述:	否需要执行自动巡航的判断;
** 参    数:	None
** 返 回 值:	0-不需要巡航		其他-不需要进行巡航
***********************************************************/
__weak uint8_t AutoCruise_CtrlFlagCheck(void)
{
	static uint32_t PGVJustify_DelayTime = 0;
	uint8_t res = 0;
	uint8_t temp = 0;
	
	//先暂时复用自动对中功能按钮，需要实际给这个功能一个触发按钮
	temp = g_ChannelSwitch.CtrlCmd.Button.ShelfAutadj;
	if (temp == 1)
	{
		if (PGVJustify_DelayTime == 0)
		{
			PGVJustify_DelayTime = g_Sys_1ms_Counter;
		}
		if (g_Sys_1ms_Counter > PGVJustify_DelayTime + 2000)
		{
			res = 1;
		}
	}
	else
	{
		PGVJustify_DelayTime = 0;
	}
	return res;
}
/***********************************************************
** 函数名称:	AutoCruise_STM_Ctrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
void AutoCruise_STM_Ctrl(void)
{
	ACC_AutoCruise_Data_StDef	*p_AutoCruise = &g_AutoCruise_Data;

	//1、记录当前的真实遥控器状态，当出现变化后，立即退出巡航状态
	{

	}

	switch(*p_STM)
	{
		default:
			break;
		//初始化
		case 0:
		{
			AutoCruise_Ctrl_Init();
			*p_STM = 1;

		}break;
		//检测是触发巡航功能
		case 1:
		{
			do
			{
				OrgCtrlCmd_TypeDef			*p_CtrlCmdCmd	= &(g_ChannelSwitch.CtrlCmd);
				ACC_AutoCruise_Temp_StDef 	*p_Temp 		= &(p_AutoCruise->Temp);
					
				uint8_t Res = AutoCruise_CtrlFlagCheck();
				/*没有得到开始触发的控制，直接跳脱*/
				if(Res != 0)	{break;}

				/*判断当前模式是否是设定功能想要的模式 非0才能状态机跳转*/
				{
					ACC_AutoCruiseFun_Enum 	*p_Fun = &(p_AutoCruise->Para.AutoCruise_Fun);
					Res = 0X00;	/*复位，防止误判*/
					if(*p_Fun == AutoCruise_Straight_Fun)
					{
						if(p_CtrlCmdCmd->Mode == ACCMode_PosStraight)
						{
							p_Temp->Vx_temp = 0;
							p_Temp->Vy_temp = p_CtrlCmdCmd->Vxyz.y;
							p_Temp->Vz_temp = 0;
							Res = 0X01;
						}
						else if(p_CtrlCmdCmd->Mode == ACCMode_InfStraight)
						{
							p_Temp->Vx_temp = p_CtrlCmdCmd->Vxyz.x;
							p_Temp->Vy_temp = 0;
							p_Temp->Vz_temp = 0;
							Res = 0X02;
						}
						else
						{
							p_Temp->Vx_temp = 0;
							p_Temp->Vy_temp = 0;
							p_Temp->Vz_temp = 0;
						}
					}
					else if(*p_Fun == AutoCruise_Flexible_Fun)
					{
						if(p_CtrlCmdCmd->Mode == ACCMode_PosFlexible
							||p_CtrlCmdCmd->Mode == ACCMode_InfFlexible)
						{
							p_Temp->Vy_temp = p_CtrlCmdCmd->Vxyz.y;
							p_Temp->Vx_temp = p_CtrlCmdCmd->Vxyz.x;
							p_Temp->Vz_temp = p_CtrlCmdCmd->Vxyz.z;
							Res = 0X03;
						}
						else
						{
							p_Temp->Vx_temp = 0;
							p_Temp->Vy_temp = 0;
							p_Temp->Vz_temp = 0;
						}
					}
					else if(*p_Fun == AutoCruise_Trace_Fun)
					{
						if(p_CtrlCmdCmd->Mode == ACCMode_PosTrace)
						{
							p_Temp->Vx_temp = 0;
							p_Temp->Vy_temp = p_CtrlCmdCmd->Vxyz.y;
							p_Temp->Vz_temp = 0;
							Res = 0X05;
						}
						else if(p_CtrlCmdCmd->Mode == ACCMode_InfTrace)
						{
							p_Temp->Vx_temp = p_CtrlCmdCmd->Vxyz.x;
							p_Temp->Vy_temp = 0;
							p_Temp->Vz_temp = 0;
							Res = 0X06;
						}
						else
						{
							p_Temp->Vx_temp = 0;
							p_Temp->Vy_temp = 0;
							p_Temp->Vz_temp = 0;
						}
					}
					else
					{
						/*无需动作*/
					}
				}
				/*非0，满足模式要求*/
				if(Res != 0)	{break;}
				/**/
				*p_STM = 2;
				//锁存遥控器状态
				{
					ACC_AutoCruise_371_WHData_StDef	*p_Temp		= &(g_AutoCruise_Data.Temp.WH_temp);
					ACC_AutoCruise_371_WHData_StDef	*p_Input	= &(g_AutoCruise_Data.Input.WHData);
					p_Temp->XAxisOutput 	= p_Input->XAxisOutput;
					p_Temp->YAxisOutput 	= p_Input->YAxisOutput;
					p_Temp->ZAxisOutput 	= p_Input->ZAxisOutput;
					p_Temp->EmgStop 		= p_Input->EmgStop;
					p_Temp->J41 			= p_Input->J41;
					p_Temp->J42 			= p_Input->J42;
					p_Temp->J43 			= p_Input->J43;
					p_Temp->J44				= p_Input->J44;
				}
			}while(0);
		}break;
		//执行巡航功能
		case 2:
		{
			
			//退出条件检查
			{
				AutoCruise_WHData_Check();
				//371专属
				//if((p_AutoCruise->Input.WHData) != (p_AutoCruise->Temp.WH_temp))
				if(0)
				{
					*p_STM = 3;
				}
				
			}
			
			//自动巡航功能执行
			{
				ACC_AutoCruise_Temp_StDef 		*p_Temp		= &(p_AutoCruise->Temp);
				ACC_AutoCruise_OutputData_StDef	*p_Output	= &(p_AutoCruise->Output);
				p_Output->Vx = p_Temp->Vx_temp;
				p_Output->Vy = p_Temp->Vy_temp;
				p_Output->Vz = p_Temp->Vz_temp;
			}

		}break;
		//退出对中功能
		case 3:
		{
			//清除控制指令
			AutoCruise_Output_Clear();
			*p_STM = 1;

		}break;
	}
}

//自动对中功能
void AutoCruise_Ctrl(void)
{
	/*判断功能是否初始化*/
	if(g_AutoCruise_Data.Para.AutoCruise_Fun == AutoCruise_Null_Fun)
	{
		return;
	}
	
	//1、自动对中输入管道创建
	{
		AutoCruise_Input_Ctrl();
	}
	//2、自动对中状态机执行
	{
		AutoCruise_STM_Ctrl();
	}
	//3、自动对中输出管道接管真实遥感
	{
		AutoCruise_Output_Ctrl();
	}

}



/************************************END OF FILE************************************/
