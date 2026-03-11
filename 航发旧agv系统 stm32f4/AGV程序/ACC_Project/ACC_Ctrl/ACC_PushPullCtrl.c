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
//声明对象在此文件编译时实例化
#define  ACCPUSHPILLCTR_GLOBALS



#include "ACC_PushPullCtrl.h"
#include "ACC_Init.h"

#include "CommConf.h"
#include "IOConf.h"

//推垃机构控制
bool Push_Pull_Ctrl(void)
{
	ACC_DATA_TypeDef				*p2Acc			= &AccData;
	PushPullCtrl_Cmd_TypeDef		*InPut_CMD 		= &g_Push_Pull.PR_Cmd;
//	PushPullCtrl_CtrlIn_TypeDef		*InPut_Data 	= &g_Push_Pull.PR_CtrlIn;
	PushPullCtrl_CtrlOut_TypeDef	*OutPut_Data 	= &g_Push_Pull.PR_CtrlOut;

	/*输出管道组织*/
	InPut_CMD->in_Push  	= p2Acc->CalCmd.Button.LiftUp;
	InPut_CMD->in_Pull		= p2Acc->CalCmd.Button.LiftDown;
	InPut_CMD->in_Gears 	= p2Acc->CalCmd.LiftGear;

//	InPut_Data->PullLimit[0] = !g_IOInputs.PullLimit.StableValue;
//	InPut_Data->PullLimit[1] = !g_IOInputs.PullLimit.StableValue;

	PushPulll_Main(&g_Push_Pull);

	/*输出管道组织*/
	switch(g_Push_Pull.PR_Type)
	{
		default:
		case PushPullType_Null:
			break;

		case PushPullType_TwoSvoRoller:
		{
			uint8_t i = 0;
			uint8_t MaxNum = g_Push_Pull.PR_CtrlPara.CtrlNum;
			//电机控制
			for (i = 0; i < MaxNum; i++)
			{
				p2Acc->PushRollerMotor[i].V = OutPut_Data->MotorSpeed[i];
			}

		}			
	}

	return 1;
}



/************************************END OF FILE************************************/
