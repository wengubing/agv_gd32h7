/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ACC_LeadShineCtrl.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  ACC_LEADSHINE_GLOBALS

#include "ACC_LeadShineCtrl.h"


/***********************************************************
** 函数名称:	LS_CmdPrep
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void LS_CmdPrep(ACC_LeadShineCtrl_StDef *p)
{
	ACC_LeadShineCtrl_Input_StDef 	*p_Input = &(p->LS_Input);
	ACC_LeadShineCtrl_Temp_StDef 	*p_Temp  = &(p->LS_Temp);
	
	
	uint8_t Inst_High	= p->LS_Para.Inst_High;
	//逻辑控制
	do
	{
		/*如果有强制，直接模式强制回零*/
		if(p_Input->LeadShine_Force == 1)
		{
			p_Temp->Temp_ControlMode = LeadShine_AbnormalStopMode;
			p_Temp->Temp_ControlHeight = 0;
			break;
		}
		/*自动对中进行中，直接全部伸到高度匹配的位置*/
		if(p_Input->AGV_Justify == 1)
		{
			p_Temp->Temp_ControlMode = LeadShine_FullMotionMode;
			p_Temp->Temp_ControlHeight = p_Input->AGV_Height + Inst_High;
			break;
		}
		/*slam+磁导航的自动下，雷赛是否伸出收自动框架控制*/
		if(p->LS_Para.Tarce_Slam_Magnetic == 1 && p_Input->AGV_Remote == 0)
		{
			p_Temp->Temp_ControlMode = LeadShine_FullMotionMode;
			p_Temp->Temp_ControlHeight = p_Input->AGV_Height + Inst_High;
			break;
		}
		/*区分自动还是遥控执行不同的应用逻辑*/
		{
			//当输入模式发生变化时，将状态机复位成null进行相关跳转动作
			if(p_Temp->Last_AGV_Mode != p_Input->AGV_Mode)
			{
				float Latch_Height = 0;
				/*上一次的模式是平台控制或者悬挂控制的情况下*/
				if(	  p_Temp->Last_AGV_Mode == LeadShine_Lift
					||p_Temp->Last_AGV_Mode == LeadShine_Absorption)
				{
					p_Temp->Latch_AGV_Height = p_Input->AGV_Height;
				}
				/*更新目标控制高度：锁存高度+标定高度*/
				Latch_Height = p_Temp->Latch_AGV_Height + Inst_High;
				/*根据新的模式，执行相对应的雷赛控制*/
				switch(p_Input->AGV_Mode)
				{
					default:
					case LeadShine_Null:
					{
						/*禁止模式不对模组进行控制*/
						p_Temp->Temp_ControlMode = LeadShine_EmptyMode;
						p_Temp->Temp_ControlHeight = 0;
					}break;
					case LeadShine_Still:
					{
						/*禁止模式不对模组进行控制*/
						p_Temp->Temp_ControlMode = LeadShine_EmptyMode;
						p_Temp->Temp_ControlHeight = 0;
					}
					break;
					case LeadShine_PosTrace:
					{
						/*暂时先全部动，后续考虑是否需要前后动*/
						p_Temp->Temp_ControlMode = LeadShine_FullMotionMode;
						p_Temp->Temp_ControlHeight = Latch_Height;
					}
					break;
					case LeadShine_InfTrace:
					{
						/*暂时先全部动，后续考虑是否需要左右动*/
						p_Temp->Temp_ControlMode = LeadShine_FullMotionMode;
						p_Temp->Temp_ControlHeight = Latch_Height;
					}
					break;
					case LeadShine_Rotation:
					{
						/*全部动*/
						p_Temp->Temp_ControlMode = LeadShine_FullMotionMode;
						p_Temp->Temp_ControlHeight = Latch_Height;
					}
					break;
					case LeadShine_Absorption:
					case LeadShine_Lift:
					{
						/*升降模式直接全部回0*/
						p_Temp->Temp_ControlMode = LeadShine_FullMotionMode;
						p_Temp->Temp_ControlHeight = 0;
					}
					break;
				}
				p_Temp->Last_AGV_Mode = LeadShine_Null;
			}
		}
	}while(0);
	//更新输入模式
	p_Temp->Last_AGV_Mode = p_Input->AGV_Mode;

}
/***********************************************************
** 函数名称:	LS_OutPut
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void LS_OutPut(ACC_LeadShineCtrl_StDef *p)
{

	ACC_LeadShineCtrl_Output_StDef 	*p_Output	= &(p->LS_Output);
	ACC_LeadShineCtrl_Temp_StDef 	*p_Temp  	= &(p->LS_Temp);
	
	p_Output->ControlHeight = p_Temp->Temp_ControlHeight;
	p_Output->ControlMode 	= p_Temp->Temp_ControlMode;

}
/***********************************************************
** 函数名称:	ACC_LeadShineCtrl
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/

void ACC_LeadShineCtrl(ACC_LeadShineCtrl_StDef *p)
{

	LS_CmdPrep(p);
	
	LS_OutPut(p);
}
/************************************END OF FILE************************************/
