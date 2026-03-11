/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ACC_PushPullCtrl.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-07
	* Description:
	******************************************************************************
**/

#include "app_PushPullCtrl.h"


#include "CommConf.h"

void PushPullCtrl_Cal(PushPull_StDef* p)
{
	uint8_t i = 0;
	//float	sum_temp = 0;
	uint8_t MeasureNum = p->PR_CtrlPara.MeasureNum;

	//开始新一次计算
	for (i = 0; i < MeasureNum; i++)
	{
		p->PR_CtrlIn.Distance[i] = 0;
	}
}



void PR_Ctrl_SVOType(PushPull_StDef* p)
{
	PushPullCtrl_Cmd_TypeDef 		*Cmd 		= &p->PR_Cmd;
	PushPullCtrl_CtrlPara_TypeDef	*Para		= &p->PR_CtrlPara;	
	PushPullCtrl_CtrlIn_TypeDef		*In			= &p->PR_CtrlIn;
	PushPullCtrl_CtrlOut_TypeDef	*Out		= &p->PR_CtrlOut;
	PushPullCtrl_Temp_TypeDef		*CtrlTemp	= &p->PR_Temp;
	
	uint8_t i = 0;

	//输入命令解析
	{
		//推送挡位命令 透传
		Cmd->do_Gears = Cmd->in_Push;
		
		//未启用自由升降
		if(Cmd->do_Force == 0)
		{
			if(Cmd->in_Push == 1)
			{
				float limit 	= Para->LimitPush;
				float Distance 	= 0;
				p->PR_STM = PushPullCtrl_LimitPush;		//标记为推到位
				Cmd->do_Push = 0;
				Cmd->do_Pull = 0;
				/*遍历所有传感数据，将执行结果给到缓存中*/
				for(i=0;i<PushPullCtrl_MAX;i++)
				{
					Distance 	= In->Distance[i];
					if(	  (limit != 0 && Distance > limit)
						||(In->PushLimit[i] == 0))
					{
						CtrlTemp->Ctrl_Temp[i] = 1;		//输入缓存
						p->PR_STM = PushPullCtrl_Push;	//标记为推运行中
						Cmd->do_Push = 1;
					}
					else
					{
						CtrlTemp->Ctrl_Temp[i] = 0;
					}
				}
			}
			else if(Cmd->in_Pull == 1)
			{
				
				float limit 	= Para->LimitPull;
				float Distance 	= 0;
				p->PR_STM = PushPullCtrl_LimitPush;		//标记为推到位
				Cmd->do_Push = 0;
				Cmd->do_Pull = 0;
				/*遍历所有传感数据，将执行结果给到缓存中*/
				for(i=0;i<PushPullCtrl_MAX;i++)
				{
					Distance 	= In->Distance[i];
					if(	  (limit != 0 && Distance < limit)
						||(In->PullLimit[i] == 0))
					{
						CtrlTemp->Ctrl_Temp[i] = 2;		//输入缓存
						p->PR_STM = PushPullCtrl_Push;	//标记为推运行中
						Cmd->do_Pull = 1;
					}
					else
					{
						CtrlTemp->Ctrl_Temp[i] = 0;
					}
				}
			}
			else
			{
				p->PR_STM = PushPullCtrl_Normal;	//标记为推运行中
				Cmd->do_Push = 0;
				Cmd->do_Pull = 0;
				for(i=0;i<PushPullCtrl_MAX;i++)
				{
					CtrlTemp->Ctrl_Temp[i] = 0;		//输入缓存
				}
			}
		}
		//启用自由升降
		else
		{
			if(Cmd->in_Push == 1)
			{
				p->PR_STM = PushPullCtrl_Push;	//标记为推运行中
				Cmd->do_Push = 1;
				Cmd->do_Pull = 0;
				for(i=0;i<PushPullCtrl_MAX;i++)
				{
					CtrlTemp->Ctrl_Temp[i] = 1;		//输入缓存
				}
			}
			else if(Cmd->in_Pull == 1)
			{
				p->PR_STM = PushPullCtrl_Push;	//标记为推运行中
				Cmd->do_Push = 0;
				Cmd->do_Pull = 1;
				for(i=0;i<PushPullCtrl_MAX;i++)
				{
					CtrlTemp->Ctrl_Temp[i] = 2;		//输入缓存
				}
			}
			else
			{
				p->PR_STM = PushPullCtrl_Normal;	//标记为推运行中
				Cmd->do_Push = 0;
				Cmd->do_Pull = 0;
				for(i=0;i<PushPullCtrl_MAX;i++)
				{
					CtrlTemp->Ctrl_Temp[i] = 0;		//输入缓存
				}
			}
		}
	}
	//输出命令解析
	{
		float	PushMotorSpeed 		= Para->SpeedPlanPush	[Cmd->do_Gears];
		float	PullMotorSpeed 		= Para->SpeedPlanPull	[Cmd->do_Gears];
		for(i=0;i<PushPullCtrl_MAX;i++)
		{
			if(CtrlTemp->Ctrl_Temp[i] == 0)
			{
				Out->MotorSpeed	[i]	= 0;
				Out->PushCtrl	[i]	= 0;
				Out->PullCtrl	[i]	= 0;
			}
			else if(CtrlTemp->Ctrl_Temp[i] == 1)
			{
				Out->MotorSpeed	[i] = PushMotorSpeed;
				Out->PushCtrl	[i] = 1;
				Out->PullCtrl	[i] = 0;
			}
			else if(CtrlTemp->Ctrl_Temp[i] == 2)
			{
				Out->MotorSpeed	[i] = -PullMotorSpeed;
				Out->PushCtrl	[i] = 0;
				Out->PullCtrl	[i] = 1;
			}
		}
		
	}
}

/*推拉控制的主函数调用*/
void PushPulll_Main(PushPull_StDef* p)
{
	switch(p->PR_Type)
	{
		default:
		case PushPullType_Null:
			break;
		
		case PushPullType_TwoSvoRoller:
		{
			PR_Ctrl_SVOType(p);
		}			
	}
}




/************************************END OF FILE************************************/
