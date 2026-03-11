/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ACC_OBS.c
** 创 建 人: Master 
** 描    述: 
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2025年4月22日
**--------------------------------------------------------------------------------------
***************************************************************************************/  

//声明对象在此文件编译时实例化
#define  ACC_OBS_GLOBALS

#include "AgvChassisController.h"
#include "ACC_OBS.h"
#include "Debug.h"

#include "ACC_Init.h"

/***********************************************************
** 函数名称:	ACC_IO_OBS_Input
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void ACC_OBS_Input(ACC_DATA_TypeDef *p)
{
	Para_ROA_StDef 	*p_ROA_Set	= &g_Para.ROA;
	ROA_TypeDef 	*p2ROA		= &g_ROA;

	/*标准应用，IO直接的雷达*/
	if(p_ROA_Set->InPutSource.MCB_IO == 1)
	{
		ACC_ObsRadarInputIO_TypeDef *p_InPutSource = AccData.InputIO.ObsRadarInput;
		p2ROA->RadarInput[0].Outer	= p_InPutSource[0].ObsRadarOuter;
		p2ROA->RadarInput[0].Middle = p_InPutSource[0].ObsRadarMiddle;
		p2ROA->RadarInput[0].Inner	= p_InPutSource[0].ObsRadarInner;
		p2ROA->RadarInput[0].Error	= p_InPutSource[0].ObsRadarErr;

		p2ROA->RadarInput[1].Outer	= p_InPutSource[1].ObsRadarOuter;
		p2ROA->RadarInput[1].Middle = p_InPutSource[1].ObsRadarMiddle;
		p2ROA->RadarInput[1].Inner	= p_InPutSource[1].ObsRadarInner;
		p2ROA->RadarInput[1].Error	= p_InPutSource[1].ObsRadarErr;
		/*位域对象操作方式*/
		if(g_Para.ROA.Install.Surround == 1)
		{
			p2ROA->RadarInput[2].Outer	= p_InPutSource[2].ObsRadarOuter;
			p2ROA->RadarInput[2].Middle = p_InPutSource[2].ObsRadarMiddle;
			p2ROA->RadarInput[2].Inner	= p_InPutSource[2].ObsRadarInner;
			p2ROA->RadarInput[2].Error	= p_InPutSource[2].ObsRadarErr;

			p2ROA->RadarInput[3].Outer	= p_InPutSource[3].ObsRadarOuter;
			p2ROA->RadarInput[3].Middle = p_InPutSource[3].ObsRadarMiddle;
			p2ROA->RadarInput[3].Inner	= p_InPutSource[3].ObsRadarInner;
			p2ROA->RadarInput[3].Error	= p_InPutSource[3].ObsRadarErr;
		}
		/*对角雷达，实际IO输入仅有2组，分别映射至CBB前后雷达即可*/
		else if(g_Para.ROA.Install.Diagonal_L == 1 || g_Para.ROA.Install.Diagonal_R == 1 ||g_Para.ROA.Install.Center == 1)
		{

		}
	}
	/*非标应用，PLC告知的雷达信号*/
	else if(p_ROA_Set->InPutSource.PLC_Community == 1)
	{
		EQP_PLC_EstopObj_StDef	*p_InPutSource = EQP_PLC_Estop.p2Data;
		
		p2ROA->RadarInput[0].Outer	= p_InPutSource->F_ObsRadarOuter;
		p2ROA->RadarInput[0].Middle = p_InPutSource->F_ObsRadarMiddle;
		p2ROA->RadarInput[0].Inner	= p_InPutSource->F_ObsRadarInner;
		p2ROA->RadarInput[0].Error	= p_InPutSource->F_ObsRadarErr;

		p2ROA->RadarInput[1].Outer	= p_InPutSource->B_ObsRadarOuter;
		p2ROA->RadarInput[1].Middle = p_InPutSource->B_ObsRadarMiddle;
		p2ROA->RadarInput[1].Inner	= p_InPutSource->B_ObsRadarInner;
		p2ROA->RadarInput[1].Error	= p_InPutSource->B_ObsRadarErr;
		/*位域对象操作方式*/
		if(g_Para.ROA.Install.Surround == 1)
		{

		}
	}
}

/***********************************************************
** 函数名称:	ACC_IO_OBS_Output
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void ACC_OBS_Output(ACC_DATA_TypeDef *p)
{
	ROA_TypeDef 	*p2ROA		= &g_ROA;

	if(1)
	{
		if(g_Para.ROA.Install.Surround == 1)
			{
				p->IOCtrl.ObsRadarCtrl[0].ObsRadarCtrl1 = p2ROA->RadarOutput[0].Ctrl1;
				p->IOCtrl.ObsRadarCtrl[0].ObsRadarCtrl2 = p2ROA->RadarOutput[0].Ctrl2;
				p->IOCtrl.ObsRadarCtrl[0].ObsRadarCtrl3 = p2ROA->RadarOutput[0].Ctrl3;
				p->IOCtrl.ObsRadarCtrl[0].ObsRadarCtrl4 = p2ROA->RadarOutput[0].Ctrl4;

				p->IOCtrl.ObsRadarCtrl[1].ObsRadarCtrl1 = p2ROA->RadarOutput[2].Ctrl1;
				p->IOCtrl.ObsRadarCtrl[1].ObsRadarCtrl2 = p2ROA->RadarOutput[2].Ctrl2;
				p->IOCtrl.ObsRadarCtrl[1].ObsRadarCtrl3 = p2ROA->RadarOutput[2].Ctrl3;
				p->IOCtrl.ObsRadarCtrl[1].ObsRadarCtrl4 = p2ROA->RadarOutput[2].Ctrl4;

				p->IOCtrl.ObsRadarCtrl[2].ObsRadarCtrl1 = p2ROA->RadarOutput[1].Ctrl1;
				p->IOCtrl.ObsRadarCtrl[2].ObsRadarCtrl2 = p2ROA->RadarOutput[1].Ctrl2;
				p->IOCtrl.ObsRadarCtrl[2].ObsRadarCtrl3 = p2ROA->RadarOutput[1].Ctrl3;
				p->IOCtrl.ObsRadarCtrl[2].ObsRadarCtrl4 = p2ROA->RadarOutput[1].Ctrl4;

				p->IOCtrl.ObsRadarCtrl[3].ObsRadarCtrl1 = p2ROA->RadarOutput[3].Ctrl1;
				p->IOCtrl.ObsRadarCtrl[3].ObsRadarCtrl2 = p2ROA->RadarOutput[3].Ctrl2;
				p->IOCtrl.ObsRadarCtrl[3].ObsRadarCtrl3 = p2ROA->RadarOutput[3].Ctrl3;
				p->IOCtrl.ObsRadarCtrl[3].ObsRadarCtrl4 = p2ROA->RadarOutput[3].Ctrl4;
			}
			/*对角雷达，实际IO输出仅有2组，分别映射至CBB前后雷达即可*/
			else if(g_Para.ROA.Install.Diagonal_L == 1 || g_Para.ROA.Install.Diagonal_R == 1 ||g_Para.ROA.Install.Center == 1)
			{
				p->IOCtrl.ObsRadarCtrl[0].ObsRadarCtrl1 = p2ROA->RadarOutput[0].Ctrl1;
				p->IOCtrl.ObsRadarCtrl[0].ObsRadarCtrl2 = p2ROA->RadarOutput[0].Ctrl2;
				p->IOCtrl.ObsRadarCtrl[0].ObsRadarCtrl3 = p2ROA->RadarOutput[0].Ctrl3;
				p->IOCtrl.ObsRadarCtrl[0].ObsRadarCtrl4 = p2ROA->RadarOutput[0].Ctrl4;

				p->IOCtrl.ObsRadarCtrl[1].ObsRadarCtrl1 = p2ROA->RadarOutput[1].Ctrl1;
				p->IOCtrl.ObsRadarCtrl[1].ObsRadarCtrl2 = p2ROA->RadarOutput[1].Ctrl2;
				p->IOCtrl.ObsRadarCtrl[1].ObsRadarCtrl3 = p2ROA->RadarOutput[1].Ctrl3;
				p->IOCtrl.ObsRadarCtrl[1].ObsRadarCtrl4 = p2ROA->RadarOutput[1].Ctrl4;
			}
	}

}
/**************************************************************************
* 函数名:ACC_OBS_Process
* 功  能:
* 参  数:NC
* 返回值:NC
**************************************************************************/
void ACC_OBS_Process(ACC_DATA_TypeDef *p)
{

	ROA_TypeDef *p2ROA=&g_ROA;

	//CBB输入
	p2ROA->SysTimeCnt=p->in_Ms;

    //控制命令输入
	//if(p->ModeSwitchSMT != ACCMode_Null)
	if (ACC_IsWalkMode(p->ModeSwitchSMT))//非行走模式时 避免误报
    {
        p2ROA->XAxisDataInput = p->CalCmd.Vxyz.x;
        p2ROA->YAxisDataInput = p->CalCmd.Vxyz.y;
        p2ROA->ZAxisDataInput = p->CalCmd.Vxyz.z;
    }
    else
    {//防止在模式切换过程中出现错误的避障状态提示
        p2ROA->XAxisDataInput = 0;
        p2ROA->YAxisDataInput = 0;
        p2ROA->ZAxisDataInput = 0;
    }

    //雷达区域选择
	p2ROA->ObstacleAreaSelect = (u8)p->CalCmd.Button.ObsArea;				

	/*雷达传感器原始信号输入到CBB模块中*/
    if(AtDbug.Bit.ObsIO_InOff == 0)
		ACC_OBS_Input(p);
	else {}

    /*使能输入 多源控制*/
    p2ROA->RadarInput[0].Enable = p->CalCmd.RadarEn.Front;
    p2ROA->RadarInput[1].Enable = p->CalCmd.RadarEn.Back;
    p2ROA->RadarInput[2].Enable = p->CalCmd.RadarEn.Left;
    p2ROA->RadarInput[3].Enable = p->CalCmd.RadarEn.Right;

    /*使能输入 单源控制
    p2ROA->RadarInput[0].Enable = p->CalCmd.Button.ObsRadarFront;
    p2ROA->RadarInput[1].Enable = p->CalCmd.Button.ObsRadarBack;
    p2ROA->RadarInput[2].Enable = p->CalCmd.Button.ObsRadarLeft;
    p2ROA->RadarInput[3].Enable = p->CalCmd.Button.ObsRadarRight;*/

    //雷达避障处理
	ROA_DataProcess(&g_ROA);								

	//CBB 避障结果输出
	p->CalCmd.Vxyz.x=p2ROA->XAxisDataOutput;
	p->CalCmd.Vxyz.y=p2ROA->YAxisDataOutput;
	p->CalCmd.Vxyz.z=p2ROA->ZAxisDataOutput;

	/*雷达切区的CBB输出给到雷达外设控制*/
	ACC_OBS_Output(p);
}

//***********************************END OF FILE***********************************
