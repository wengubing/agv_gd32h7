/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ACC_FaultsResponse.c
** 创 建 人: Master 
** 描    述: 
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2025年4月22日
**--------------------------------------------------------------------------------------
***************************************************************************************/  

//声明对象在此文件编译时实例化
#define  ACC_FaultsResponse_GLOBALS
#include "ACC_FaultsResponse.h"
#include "CBB_Init.h"

extern bool CLR_CBB(void);
extern bool ACC_ClearCmd(ACC_DATA_TypeDef *p);
extern void ACC_CloseAllValve(ACC_DATA_TypeDef *p);

/**************************************************************************
* 函数名:ACC_FaultsResponse_LoseControlStop
* 功  能:非受控停车处理
* 参  数:NC
* 返回值:NC
**************************************************************************/
void ACC_FaultsResponse_LoseControlStop(ACC_DATA_TypeDef *p)
{
    //清除cbb输入、状态
    CLR_CBB();

	ACC_ClearCmd(p);//清除所有控制执行的命令
	
	ACC_CloseAllValve(p);//关闭电磁阀

	//复位联动模块
	Asb_Reset(&g_Asb);

	if(p->LoseControlStop.Step==ACC_LoseControlStopStep_Init)
	{
		p->LoseControlStop.Step=ACC_LoseControlStopStep_ServoOff;
	}
	else if(p->LoseControlStop.Step==ACC_LoseControlStopStep_ServoOff)
	{
		//断电
		p->IOCtrl.MainContactor=0;
		p->LoseControlStop.MsRecord=p->in_Ms;
		p->LoseControlStop.Step=ACC_LoseControlStopStep_WaitBrake;
	}
	else if(p->LoseControlStop.Step==ACC_LoseControlStopStep_WaitBrake)
	{
		if (p->in_Ms - p->LoseControlStop.MsRecord > p->Para.LoseControlStop_BrakeTime)//延时抱闸
		{
			p->IOCtrl.MoveBrake=0;
			p->LoseControlStop.Step=ACC_LoseControlStopStep_Finish;
		}
	}
	else if(p->LoseControlStop.Step==ACC_LoseControlStopStep_Finish)
	{
		
	}
	else {}
}

//故障响应控制
void ACC_FaultsResponse_Process(ACC_DATA_TypeDef *p)
{
	//获取故障信息
	p->in_Faults = p->ExtFunc.p2EmgJudge();
	p->in_FaultsType = p->in_Faults.Faults;

	//输入变化，状态机无条件跳到ACC_FaultsType_Null复位，再跳到对应状态
    //要加上恢复时间，后面的模式切换里时间相当于这个恢复时间
	if(p->FaultsResponseSTM != p->in_FaultsType)
	{
		if(p->FaultsResponseSTM != ACC_FaultsType_LoseControlStop)//非受控停车 不能直接恢复！
		{
			p->FaultsResponseSTM=ACC_FaultsType_Null;
			p->LoseControlStop.Step=ACC_LoseControlStopStep_Init;
		}
	}
	
	switch (p->FaultsResponseSTM)
	{
		//初始化
		case ACC_FaultsType_Null:						
		{
			p->FaultsResponseSTM = p->in_FaultsType;
			/*根据单动还是联动目标模式恢复平滑增量设置
			if (g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb)
				JoystickDataSmooth_Asb_Init();
			else
				JoystickDataSmooth_Init();*/

			/*只一种加减速*/
			JoystickDataSmooth_Init();

			/*if(p->FaultsResponseSTM == ACC_FaultsType_Normal)
				printf("FaultsResponse：进入正常...\n");
			else if(p->FaultsResponseSTM == ACC_FaultsType_MoveLimited)
				printf("FaultsResponse：进入限制行走...\n");
			else if (p->FaultsResponseSTM == ACC_FaultsType_ControlledStop)
				printf("FaultsResponse：进入受控停车...\n");
			else if (p->FaultsResponseSTM == ACC_FaultsType_LoseControlStop)
				printf("FaultsResponse：进入非受控停车...\n");
			else {}*/
		}
		break;

		//正常
		case ACC_FaultsType_Normal:		
		break;

		//提示
		case ACC_FaultsType_Warn: 	
		break;
		
		//限制行走
		case ACC_FaultsType_MoveLimited:					
		{   
            //完全限制行走
            if(p->in_Faults.Bit.MoveLimited)
			{
                p->CalCmd.Vxyz.x = 0;
			    p->CalCmd.Vxyz.y = 0;
			    p->CalCmd.Vxyz.z = 0;
            }
            else {}

            //限制上升
            if(p->in_Faults.Bit.UpLimited)
            {
                p->CalCmd.Button.LiftUp = 0;
            }    
            else {}

            //限制下降
            if(p->in_Faults.Bit.DownLimited)
            {
                p->CalCmd.Button.LiftDown = 0;
            }    
            else {}
		}
        break;

		//受控停车
		case ACC_FaultsType_ControlledStop:					
		{
			p->CalCmd.Mode = ACCMode_Still;
			p->CalCmd.Button.LiftUp = 0;
			p->CalCmd.Button.LiftDown = 0;
			g_SmoothVx.in_Dec = 0.5;
			g_SmoothVy.in_Dec = 0.5;
			g_SmoothVz.in_Dec = 0.5;

			//关联平滑时
			g_SmoothVxyz[0].in_Dec = 0.5;

			//复位联动模块
			Asb_Reset(&g_Asb);
		}
		break;

		//非受控停车
		case ACC_FaultsType_LoseControlStop:					
		{
			ACC_FaultsResponse_LoseControlStop(p);
		}
		break;
		
		default:
		break;
	}
	
}
//***********************************END OF FILE***********************************
