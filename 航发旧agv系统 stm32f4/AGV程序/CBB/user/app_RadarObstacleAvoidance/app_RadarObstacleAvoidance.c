/****************************Copyright (c)**********************************************                                       
**                  ╭--------------------------╮
**                  ︱   成都航发机器人有限公司 ︱
**                  ︱   http://www.hangfa.com 	︱                     
**					︱ 	  All Rights Reserved	︱
**                  ╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: app_RadarObstacleAvoidance.h
** 创 建 人: 何维  
** 描    述: 激光避障数据处理源文件，包含切区功能函数和根据雷达状态进行控制限制函数
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2023年12月20日       
**--------------------------------------------------------------------------------------
***************************************************************************************/  
#include "app_RadarObstacleAvoidance.h"
#include <stdlib.h>

#define		Get_Bit(data, n)		((data & (1 << (n & 0x1f))) == (1 << (n & 0x1f)))	/*取第n的状态位[SET:1;RESET:0]---不对数据data操作,n:0...31*/
#define		Set_Bit(data, n)		(data |= (1 << (n & 0x1f)))							/*置第n的状态:1---n:0...31*/
#define		Clr_Bit(data, n)		(data &= ~(1 << (n & 0x1f)))						/*清第n的状态:0---n:0...31*/
#define 	Delay_Fuction(A, B, C)	((u32)(A - B) > (u32)(C))

/***************************************************************************************
** 函数名称: ROA_GetRadarState
** 功能描述: 雷达状态获取
** 参    数: InputInfo：雷达输入数据信息
** 返 回 值: 雷达当前状态信息
*
****************************************************************************************/
u8 ROA_GetRadarState_old(u8 InputInfo)
{
	u8 Sta;
	
	if(Get_Bit(InputInfo,InputState_Error))					/*雷达故障*/
	{
		Sta = State_Error;
	}
	else
	{
		if(Get_Bit(InputInfo,InputState_Warning))			/*最外层触发*/
		{
			Sta = State_Warning;
		}
		else if(Get_Bit(InputInfo,InputState_Caution))		/*中间层触发*/
		{
			Sta = State_Caution;
		}
		else if(Get_Bit(InputInfo,InputState_Emergency))	/*最内层触发*/
		{
			Sta = State_Emergency;
		}
		else
		{
			Sta = State_Nomal;
		}
	}
	
	return Sta;
}

//重写
u8 ROA_GetRadarState(RADAR_INPUT_BitDef InputInfo)
{
	u8 Sta;
	
	if(InputInfo.Error)
		//Sta = State_Error;
		Sta = State_Emergency;//解决雷达故障状态时没有停车问题
	else if(InputInfo.Inner)
        Sta = State_Emergency;
    else if(InputInfo.Middle)
        Sta = State_Caution;
    else if(InputInfo.Outer)
        Sta = State_Warning;
    else
        Sta = State_Nomal;
	
	return Sta;
}

/***************************************************************************************
** 函数名称: ROA_DiagonalSetOutputData
** 功能描述: 设置对角安装型雷达输出参数
** 形    参: Toggle.切区信号   ScanfArea.选择区域
** 返 回 值: None
****************************************************************************************/
u8 ROA_DiagonalSetOutputData(u8 ScanfArea,bool Toggle)
{
	u8 OutputData;
	/*雷达区域扫描区域设置，对角安装类型前侧和左侧使用前雷达，后侧和左侧使用后雷达；
	  当扫描前侧和后侧时，雷达设置为偶数区域，扫描左侧和右侧时，设置为奇数区域*/
	switch(ScanfArea)
	{
		case 1:
		{
			OutputData = SCAN_AREA0;
		}
		break;
		
		case 2:
		{
			OutputData = SCAN_AREA2;
		}
		break;
		
		case 3:
		{
			OutputData = SCAN_AREA4;
		}
		break;
		
		case 4:
		{
			OutputData = SCAN_AREA6;
		}
		break;
		
		case 5:
		{
			OutputData = SCAN_AREA8;
		}
		break;
		
		case 6:
		{
			OutputData = SCAN_AREA10;
		}
		break;
		
		case 7:
		{
			OutputData = SCAN_AREA12;
		}
		break;
		
		case 8:
		{
			OutputData = SCAN_AREA14;
		}
		break;
		
		default:
			break;
	}
	
	if(Toggle == true)
	{
		OutputData += 1;
	}
	else
	{
		__asm__("nop");
	}
	
	return OutputData;
}

//雷达避障状态更新
bool ROA_ObsUpdata(ROA_TypeDef *ROAData,RADAR_DEFINE Number,RADAR_DEFINE Number_Input)
{
	RADAR_STATE RadarStateTemp;

	RadarStateTemp = (RADAR_STATE)ROA_GetRadarState(ROAData->RadarInput[Number_Input]);
	if(RadarStateTemp >= ROAData->RadarState_Self[Number])//障碍靠近或不动，立即更新
	{
		ROAData->RadarState_Self[Number] = RadarStateTemp;
		ROAData->MsRecord_ObsRecver[Number] = ROAData->SysTimeCnt;
	}
	else//障碍远离，延迟恢复
	{
		if(ROAData->SysTimeCnt - ROAData->MsRecord_ObsRecver[Number] > ROAData->RecoverTime)
		{
			ROAData->RadarState_Self[Number] = RadarStateTemp;
			ROAData->MsRecord_ObsRecver[Number] = ROAData->SysTimeCnt;
		}
		else
		{
			__asm__("nop");
		}
	}

	return true;
}

/***************************************************************************************
** 函数名称: ROA_DiagonalScanAreaChange
** 功能描述: 对角安装类型雷达避障数据处理
** 参    数: *ROAData雷达避障数据结构体类型数据指针
** 返 回 值: None
****************************************************************************************/
void ROA_DiagonalScanAreaChange(ROA_TypeDef *ROAData)
{
	static bool s_AreaChangeFlag = false;
	u8 temp;
    RADAR_OUTPUT_BitDef BitTemp;
	
	if(ROAData->InstallPattern == Pattern_Diagonal_L)
	{
	if(s_AreaChangeFlag == false)
	{
		/*获取前后雷达扫描结果*/
		ROA_ObsUpdata(ROAData,Radar_Front,Radar_Front);
		ROA_ObsUpdata(ROAData,Radar_Back,Radar_Back);

		s_AreaChangeFlag = true;
	}
	else
	{
		/*获取左右雷达扫描结果*/
		ROA_ObsUpdata(ROAData,Radar_Left,Radar_Front);
		ROA_ObsUpdata(ROAData,Radar_Right,Radar_Back);

		s_AreaChangeFlag = false;
	}
	}
	else if(ROAData->InstallPattern == Pattern_Diagonal_R)
	{
		if(s_AreaChangeFlag == false)
		{
			/*获取左右雷达扫描结果*/
			ROA_ObsUpdata(ROAData,Radar_Left,Radar_Back);
			ROA_ObsUpdata(ROAData,Radar_Right,Radar_Front);
			s_AreaChangeFlag = true;
		}
		else
		{
			
			/*获取前后雷达扫描结果*/
			ROA_ObsUpdata(ROAData,Radar_Front,Radar_Front);
			ROA_ObsUpdata(ROAData,Radar_Back,Radar_Back);
			s_AreaChangeFlag = false;
		}
	}
	
	
	/*设置扫描区域*/
	temp = ROA_DiagonalSetOutputData(ROAData->ObstacleAreaSelect,s_AreaChangeFlag);
	
	/*项目使用兴颂雷达时反转雷达输出信号*/
	if(ROAData->RadarBrand == Brand_Hinson)
	{
        temp = ((~temp) & 0x3F);
	}
	else
	{

	}

    //类型转换
    BitTemp = *(RADAR_OUTPUT_BitDef*)(&temp);
    ROAData->RadarOutput[Radar_Front] = ROAData->RadarOutput[Radar_Back] = BitTemp;
}

/***************************************************************************************
** 函数名称: ROA_SetOutputData
** 功能描述: 设置前后和四周安装型雷达输出参数
** 形    参: ScanfArea.选择区域
** 返 回 值: None
****************************************************************************************/
u8 ROA_SetOutputData(u8 ScanfArea)
{
	u8 OutputData;
	/*雷达区域扫描区域设置，前后双雷达安装方式和四周四雷达安装方式扫描时只使用前8个区域*/
	switch(ScanfArea)
	{
		case 1:
		{
			OutputData = SCAN_AREA0;
		}
		break;
		
		case 2:
		{
			OutputData = SCAN_AREA1;
		}
		break;
		
		case 3:
		{
			OutputData = SCAN_AREA2;
		}
		break;
		
		case 4:
		{
			OutputData = SCAN_AREA3;
		}
		break;
		
		case 5:
		{
			OutputData = SCAN_AREA4;
		}
		break;
		
		case 6:
		{
			OutputData = SCAN_AREA5;
		}
		break;
		
		case 7:
		{
			OutputData = SCAN_AREA6;
		}
		break;
		
		case 8:
		{
			OutputData = SCAN_AREA7;
		}
		break;
		
		default:
			break;
	}
	
	return OutputData;
}

/***************************************************************************************
** 函数名称: ROA_ScanAreaChange
** 功能描述: 前后和四周安装类型雷达避障数据处理
** 参    数: *ROAData雷达避障数据结构体类型数据指针
** 返 回 值: None
****************************************************************************************/
void ROA_ScanAreaChange(ROA_TypeDef *ROAData)
{
	u8 temp,i;
	
	/*设置避障区域*/
	temp = ROA_SetOutputData(ROAData->ObstacleAreaSelect);
	
	for(i = 0;i<Radar_MaxNum;++i)
	{
		/*项目使用兴颂雷达时反转雷达输出信号*/
		if(ROAData->RadarBrand == Brand_Hinson)
		{
			//确保 四周雷达和前后雷达的切区是同一个切区
			temp = ROA_SetOutputData(ROAData->ObstacleAreaSelect);
            //类型转换
            temp = ((~temp) & 0x3F);
            ROAData->RadarOutput[i] = *(RADAR_OUTPUT_BitDef*)(&temp);
		}
		else
		{
            //类型转换
            ROAData->RadarOutput[i] = *(RADAR_OUTPUT_BitDef*)(&temp);
		}
	}
	
	/*获取雷达扫描结果*/
	for(i = 0;i<Radar_MaxNum;++i)
	{
		ROAData->RadarState_Self[i] = (RADAR_STATE)ROA_GetRadarState(ROAData->RadarInput[i]);
	}
}

/***************************************************************************************
** 函数名称: ROA_CtrlDataProcess
** 功能描述: 根据雷达输入数据处理控制数据
** 参    数: *ROAData雷达避障数据结构体类型数据指针
** 返 回 值: None
****************************************************************************************/
void ROA_CtrlDataProcess(ROA_TypeDef *ROAData)
{
	ROAData->XAxisDataOutput = ROAData->XAxisDataInput;
	ROAData->YAxisDataOutput = ROAData->YAxisDataInput;
	ROAData->ZAxisDataOutput = ROAData->ZAxisDataInput;
				
	if(
        //Get_Bit(ROAData->RadarInput[Radar_Front],InputState_Enable)
        ROAData->RadarInput[Radar_Front].Enable//雷达使能
	&& (ROAData->YAxisDataOutput > 0 || ROAData->ZAxisDataOutput != 0)	
	)
	{
		switch(ROAData->RadarState[Radar_Front])
		{
			/*最外层触发时慢减速，中间层触发快减速，最内层触发直接砍完*/
			case State_Warning:
			{
				if(ROAData->YAxisDataOutput > ROAData->YAxisMaxSpeedLv3)				/*Y轴前进方向限速*/
				{
					ROAData->YAxisDataOutput = ROAData->YAxisMaxSpeedLv3;
				}
				else
				{
					__asm__("nop");
				}
				
				/*Z轴两个方向都限速*/
				if(abs(ROAData->ZAxisDataOutput) > ROAData->YAxisMaxSpeedLv3)
				{
					ROAData->ZAxisDataOutput = ROAData->ZAxisDataOutput / abs(ROAData->ZAxisDataOutput) * ROAData->YAxisMaxSpeedLv3;
				}
				else
				{
					__asm__("nop");
				}
				
				ROAData->RadarCtrl[Radar_Front] = State_Warning;
			}
			break;
			
			case State_Caution:
			{
				if(ROAData->YAxisDataOutput > ROAData->YAxisMaxSpeedLv2)				/*Y轴前进方向限速*/
				{
					ROAData->YAxisDataOutput = ROAData->YAxisMaxSpeedLv2;
				}
				else
				{
					__asm__("nop");
				}
				
				/*Z轴两个方向都限速*/
				if(abs(ROAData->ZAxisDataOutput) > ROAData->YAxisMaxSpeedLv2)
				{
					ROAData->ZAxisDataOutput = ROAData->ZAxisDataOutput / abs(ROAData->ZAxisDataOutput) * ROAData->YAxisMaxSpeedLv2;
				}
				else
				{
					__asm__("nop");
				}
				
				ROAData->RadarCtrl[Radar_Front] = State_Caution;
			}
			break;
			
			case State_Emergency:
			{
				if(ROAData->XAxisDataOutput > 0
				|| ROAData->YAxisDataOutput != 0
				|| ROAData->ZAxisDataOutput != 0
				)
				{
					ROAData->XAxisDataOutput = 0;
					ROAData->YAxisDataOutput = 0;
					ROAData->ZAxisDataOutput = 0;
				}
				else
				{
					__asm__("nop");
				}
				
				ROAData->RadarCtrl[Radar_Front] = State_Emergency;
			}
			break;
			
			default:
			{
				ROAData->RadarCtrl[Radar_Front] = State_Nomal;
			}
			break;
		}
	}
	else
	{
		ROAData->RadarCtrl[Radar_Front] = State_Nomal;
	}
	
	if(
        //Get_Bit(ROAData->RadarInput[Radar_Back],InputState_Enable)
        ROAData->RadarInput[Radar_Back].Enable//雷达使能
	&& (ROAData->YAxisDataOutput < 0 || ROAData->ZAxisDataOutput != 0)	
	)
	{
		switch(ROAData->RadarState[Radar_Back])
		{
			/*最外层触发时慢减速，中间层触发快减速，最内层触发直接砍完*/
			case State_Warning:
			{
				if(ROAData->YAxisDataOutput < ((-1)*ROAData->YAxisMaxSpeedLv3))				/*Y轴前进方向限速*/
				{
					ROAData->YAxisDataOutput = ((-1)*ROAData->YAxisMaxSpeedLv3);
				}
				else
				{
					__asm__("nop");
				}
				
				/*Z轴两个方向都限速*/
				if(abs(ROAData->ZAxisDataOutput) > ROAData->YAxisMaxSpeedLv3)
				{
					ROAData->ZAxisDataOutput = ROAData->ZAxisDataOutput / abs(ROAData->ZAxisDataOutput) * ROAData->YAxisMaxSpeedLv3;
				}
				else
				{
					__asm__("nop");
				}
				
				ROAData->RadarCtrl[Radar_Back] = State_Warning;
			}
			break;
			
			case State_Caution:
			{
				if(ROAData->YAxisDataOutput < ((-1)*ROAData->YAxisMaxSpeedLv2))				/*Y轴前进方向限速*/
				{
					ROAData->YAxisDataOutput = ((-1)*ROAData->YAxisMaxSpeedLv2);
				}
				else
				{
					__asm__("nop");
				}
				
				/*Z轴两个方向都限速*/
				if(abs(ROAData->ZAxisDataOutput) > ROAData->YAxisMaxSpeedLv2)
				{
					ROAData->ZAxisDataOutput = ROAData->ZAxisDataOutput / abs(ROAData->ZAxisDataOutput) * ROAData->YAxisMaxSpeedLv2;
				}
				else
				{
					__asm__("nop");
				}
				
				ROAData->RadarCtrl[Radar_Back] = State_Caution;
			}
			break;
			
			case State_Emergency:
			{
				if(ROAData->XAxisDataOutput < 0
				|| ROAData->YAxisDataOutput != 0
				|| ROAData->ZAxisDataOutput != 0
				)
				{
					ROAData->XAxisDataOutput = 0;
					ROAData->YAxisDataOutput = 0;
					ROAData->ZAxisDataOutput = 0;
				}
				else
				{
					__asm__("nop");
				}
				
				ROAData->RadarCtrl[Radar_Back] = State_Emergency;
			}
			break;
			
			default:
			{
				ROAData->RadarCtrl[Radar_Back] = State_Nomal;
			}
			break;
		}
	}
	else
	{
		ROAData->RadarCtrl[Radar_Back] = State_Nomal;
	}
	
	if(
        //Get_Bit(ROAData->RadarInput[Radar_Left],InputState_Enable)
        ROAData->RadarInput[Radar_Left].Enable//雷达使能
	&& (ROAData->XAxisDataOutput < 0 || ROAData->ZAxisDataOutput != 0)	
	)
	{
		switch(ROAData->RadarState[Radar_Left])
		{
			/*最外层触发时慢减速，中间层触发快减速，最内层触发直接砍完*/
			case State_Warning:
			{
				if(ROAData->XAxisDataOutput < ((-1)*ROAData->XAxisMaxSpeedLv3))				/*Y轴前进方向限速*/
				{
					ROAData->XAxisDataOutput = ((-1)*ROAData->XAxisMaxSpeedLv3);
				}
				else
				{
					__asm__("nop");
				}
				
				/*Z轴两个方向都限速*/
				if(abs(ROAData->ZAxisDataOutput) > ROAData->XAxisMaxSpeedLv3)
				{
					ROAData->ZAxisDataOutput = ROAData->ZAxisDataOutput / abs(ROAData->ZAxisDataOutput) * ROAData->XAxisMaxSpeedLv3;
				}
				else
				{
					__asm__("nop");
				}
				
				ROAData->RadarCtrl[Radar_Left] = State_Warning;
			}
			break;
			
			case State_Caution:
			{
				if(ROAData->XAxisDataOutput < ((-1)*ROAData->XAxisMaxSpeedLv2))				/*Y轴前进方向限速*/
				{
					ROAData->XAxisDataOutput = ((-1)*ROAData->XAxisMaxSpeedLv2);
				}
				else
				{
					__asm__("nop");
				}
				
				/*Z轴两个方向都限速*/
				if(abs(ROAData->ZAxisDataOutput) > ROAData->XAxisMaxSpeedLv2)
				{
					ROAData->ZAxisDataOutput = ROAData->ZAxisDataOutput / abs(ROAData->ZAxisDataOutput) * ROAData->XAxisMaxSpeedLv2;
				}
				else
				{
					__asm__("nop");
				}
				
				ROAData->RadarCtrl[Radar_Left] = State_Caution;
			}
			break;
			
			case State_Emergency:
			{
				if(ROAData->XAxisDataOutput < 0
				|| ROAData->YAxisDataOutput != 0
				|| ROAData->ZAxisDataOutput != 0
				)
				{
					ROAData->XAxisDataOutput = 0;
					ROAData->YAxisDataOutput = 0;
					ROAData->ZAxisDataOutput = 0;
				}
				else
				{
					__asm__("nop");
				}
				
				ROAData->RadarCtrl[Radar_Left] = State_Emergency;
			}
			break;
			
			default:
			{
				ROAData->RadarCtrl[Radar_Left] = State_Nomal;
			}
			break;
		}
	}
	else
	{
		ROAData->RadarCtrl[Radar_Left] = State_Nomal;
	}
	
	if(
        //Get_Bit(ROAData->RadarInput[Radar_Right],InputState_Enable)
        ROAData->RadarInput[Radar_Right].Enable//雷达使能
	&& (ROAData->XAxisDataOutput > 0 || ROAData->ZAxisDataOutput != 0)	
	)
	{
		switch(ROAData->RadarState[Radar_Right])
		{
			/*最外层触发时慢减速，中间层触发快减速，最内层触发直接砍完*/
			case State_Warning:
			{
				if(ROAData->XAxisDataOutput > ROAData->XAxisMaxSpeedLv3)				/*X轴前进方向限速*/
				{
					ROAData->XAxisDataOutput = ROAData->XAxisMaxSpeedLv3;
				}
				else
				{
					__asm__("nop");
				}
				
				/*Z轴两个方向都限速*/
				if(abs(ROAData->ZAxisDataOutput) > ROAData->XAxisMaxSpeedLv3)
				{
					ROAData->ZAxisDataOutput = ROAData->ZAxisDataOutput / abs(ROAData->ZAxisDataOutput) * ROAData->XAxisMaxSpeedLv3;
				}
				else
				{
					__asm__("nop");
				}
				
				ROAData->RadarCtrl[Radar_Right] = State_Warning;
			}
			break;
			
			case State_Caution:
			{
				if(ROAData->XAxisDataOutput > ROAData->XAxisMaxSpeedLv2)				/*X轴前进方向限速*/
				{
					ROAData->XAxisDataOutput = ROAData->XAxisMaxSpeedLv2;
				}
				else
				{
					__asm__("nop");
				}
				
				/*Z轴两个方向都限速*/
				if(abs(ROAData->ZAxisDataOutput) > ROAData->XAxisMaxSpeedLv2)
				{
					ROAData->ZAxisDataOutput = ROAData->ZAxisDataOutput / abs(ROAData->ZAxisDataOutput) * ROAData->XAxisMaxSpeedLv2;
				}
				else
				{
					__asm__("nop");
				}
				
				ROAData->RadarCtrl[Radar_Right] = State_Caution;
			}
			break;
			
			case State_Emergency:
			{
				if(ROAData->XAxisDataOutput > 0
				|| ROAData->YAxisDataOutput != 0
				|| ROAData->ZAxisDataOutput != 0
				)
				{
					ROAData->XAxisDataOutput = 0;
					ROAData->YAxisDataOutput = 0;
					ROAData->ZAxisDataOutput = 0;
				}
				else
				{
					__asm__("nop");
				}
				
				ROAData->RadarCtrl[Radar_Right] = State_Emergency;
			}
			break;
			
			default:
			{
				ROAData->RadarCtrl[Radar_Right] = State_Nomal;
			}
			break;
		}
	}
	else
	{
		ROAData->RadarCtrl[Radar_Right] = State_Nomal;
	}
}

//将各侧障碍状态 转换为 三层独立状态
bool ROA_GitObsSta(ROA_TypeDef *ROAData)
{
    //将雷达状态转换为三层状态
    //前方雷达
    if(ROAData->RadarState[Radar_Front] == State_Error || ROAData->RadarState[Radar_Front] == State_Emergency)
    {
        ROAData->RadarObsSta.ForwordRadar_Inner = 1;
        ROAData->RadarObsSta.ForwordRadar_Middle = 1;
        ROAData->RadarObsSta.ForwordRadar_Out = 1;
    }
    else if(ROAData->RadarState[Radar_Front] == State_Caution)
    {
        ROAData->RadarObsSta.ForwordRadar_Inner = 0;
        ROAData->RadarObsSta.ForwordRadar_Middle = 1;
        ROAData->RadarObsSta.ForwordRadar_Out = 1;
    }
    else if(ROAData->RadarState[Radar_Front] == State_Warning)
    {
        ROAData->RadarObsSta.ForwordRadar_Inner = 0;
        ROAData->RadarObsSta.ForwordRadar_Middle = 0;
        ROAData->RadarObsSta.ForwordRadar_Out = 1;
    }
    else
    {
        ROAData->RadarObsSta.ForwordRadar_Inner = 0;
        ROAData->RadarObsSta.ForwordRadar_Middle = 0;
        ROAData->RadarObsSta.ForwordRadar_Out = 0;
    }

    //后方雷达
    if(ROAData->RadarState[Radar_Back] == State_Error || ROAData->RadarState[Radar_Back] == State_Emergency)
    {
        ROAData->RadarObsSta.BackRadar_Inner = 1;
        ROAData->RadarObsSta.BackRadar_Middle = 1;
        ROAData->RadarObsSta.BackRadar_Out = 1;
    }
    else if(ROAData->RadarState[Radar_Back] == State_Caution)
    {
        ROAData->RadarObsSta.BackRadar_Inner = 0;
        ROAData->RadarObsSta.BackRadar_Middle = 1;
        ROAData->RadarObsSta.BackRadar_Out = 1;
    }
    else if(ROAData->RadarState[Radar_Back] == State_Warning)
    {
        ROAData->RadarObsSta.BackRadar_Inner = 0;
        ROAData->RadarObsSta.BackRadar_Middle = 0;
        ROAData->RadarObsSta.BackRadar_Out = 1;
    }
    else
    {
        ROAData->RadarObsSta.BackRadar_Inner = 0;
        ROAData->RadarObsSta.BackRadar_Middle = 0;
        ROAData->RadarObsSta.BackRadar_Out = 0;
    }

    //左方雷达
    if(ROAData->RadarState[Radar_Left] == State_Error || ROAData->RadarState[Radar_Left] == State_Emergency)
    {
        ROAData->RadarObsSta.LeftRadar_Inner = 1;
        ROAData->RadarObsSta.LeftRadar_Middle = 1;
        ROAData->RadarObsSta.LeftRadar_Out = 1;
    }
    else if(ROAData->RadarState[Radar_Left] == State_Caution)
    {
        ROAData->RadarObsSta.LeftRadar_Inner = 0;
        ROAData->RadarObsSta.LeftRadar_Middle = 1;
        ROAData->RadarObsSta.LeftRadar_Out = 1;
    }
    else if(ROAData->RadarState[Radar_Left] == State_Warning)
    {
        ROAData->RadarObsSta.LeftRadar_Inner = 0;
        ROAData->RadarObsSta.LeftRadar_Middle = 0;
        ROAData->RadarObsSta.LeftRadar_Out = 1;
    }
    else
    {
        ROAData->RadarObsSta.LeftRadar_Inner = 0;
        ROAData->RadarObsSta.LeftRadar_Middle = 0;
        ROAData->RadarObsSta.LeftRadar_Out = 0;
    }

    //右方雷达
    if(ROAData->RadarState[Radar_Right] == State_Error || ROAData->RadarState[Radar_Right] == State_Emergency)
    {
        ROAData->RadarObsSta.RightRadar_Inner = 1;
        ROAData->RadarObsSta.RightRadar_Middle = 1;
        ROAData->RadarObsSta.RightRadar_Out = 1;
    }
    else if(ROAData->RadarState[Radar_Right] == State_Caution)
    {
        ROAData->RadarObsSta.RightRadar_Inner = 0;
        ROAData->RadarObsSta.RightRadar_Middle = 1;
        ROAData->RadarObsSta.RightRadar_Out = 1;
    }
    else if(ROAData->RadarState[Radar_Right] == State_Warning)
    {
        ROAData->RadarObsSta.RightRadar_Inner = 0;
        ROAData->RadarObsSta.RightRadar_Middle = 0;
        ROAData->RadarObsSta.RightRadar_Out = 1;
    }
    else
    {
        ROAData->RadarObsSta.RightRadar_Inner = 0;
        ROAData->RadarObsSta.RightRadar_Middle = 0;
        ROAData->RadarObsSta.RightRadar_Out = 0;
    }

    return 1;
}

//侧面有障碍，不管有没有侧向速度，前后方向速度都置0
bool ROA_SideObsStop(ROA_TypeDef *ROAData)
{
    //x有速度
    if(ROAData->XAxisDataInput != 0)
    {
        if(ROAData->RadarState[Radar_Front] == State_Emergency && ROAData->RadarInput[Radar_Front].Enable)
        {
            ROAData->XAxisDataOutput = 0;
            ROAData->YAxisDataOutput = 0;
            ROAData->ZAxisDataOutput = 0;
            ROAData->RadarCtrl[Radar_Front] = State_Emergency;

        }
        
        if(ROAData->RadarState[Radar_Back] == State_Emergency && ROAData->RadarInput[Radar_Back].Enable)
        {
            ROAData->XAxisDataOutput = 0;
            ROAData->YAxisDataOutput = 0;
            ROAData->ZAxisDataOutput = 0;
            ROAData->RadarCtrl[Radar_Back] = State_Emergency;
        }
        
    }

    //y有速度
    if(ROAData->YAxisDataInput != 0)
    {
        if(ROAData->RadarState[Radar_Left] == State_Emergency && ROAData->RadarInput[Radar_Left].Enable)
        {
            ROAData->XAxisDataOutput = 0;
            ROAData->YAxisDataOutput = 0;
            ROAData->ZAxisDataOutput = 0;
            ROAData->RadarCtrl[Radar_Left] = State_Emergency;
        }
        
        if(ROAData->RadarState[Radar_Right] == State_Emergency && ROAData->RadarInput[Radar_Right].Enable)
        {
            ROAData->XAxisDataOutput = 0;
            ROAData->YAxisDataOutput = 0;
            ROAData->ZAxisDataOutput = 0;
            ROAData->RadarCtrl[Radar_Right] = State_Emergency;
        }
    }

    return 1;
}

//根据控制参数，将输入的某些位置障碍状态清除
bool ROA_ObsMerge_CLR(RADAR_STATE* p2Ctrl, RADAR_STATE* p2In, RADAR_STATE* p2Out)
{
	char i = 0;

	for (i = 0;i < Radar_MaxNum;++i)
	{
		if (p2In[i] > p2Ctrl[i])
			p2Out[i] = p2Ctrl[i];
		else
			p2Out[i] = p2In[i];
	}
	return 1;
}

//根据控制参数，将两个输入的某些位置障碍状态合并 一个有就输出有
bool ROA_ObsMerge_OR(RADAR_STATE* p2In1, RADAR_STATE* p2In2, RADAR_STATE* p2Out)
{
	char i = 0;

	for (i = 0;i < Radar_MaxNum;++i)
	{
		if (p2In1[i] > p2In2[i])
			p2Out[i] = p2In1[i];
		else
			p2Out[i] = p2In2[i];
	}
	return 1;
}

//执行的障碍状态更新
bool ROA_ObsSelUpdate(ROA_TypeDef *ROAData)
{
	char i = 0;
	
	if(ROAData->Sel == ObsSel_Self)
	{
		//根据自身状态更新
		for(i = 0;i<Radar_MaxNum;++i)
		{
			ROAData->RadarState[i] = ROAData->RadarState_Self[i];
		}
	}
	else if(ROAData->Sel == ObsSel_Asb)
	{
		//根据联动状态更新
		for(i = 0;i<Radar_MaxNum;++i)
		{
			ROAData->RadarState[i] = ROAData->RadarState_Asb[i];
		}
	}
	else if(ROAData->Sel == ObsSel_Gather)
	{
		//根据融合状态更新
		for(i = 0;i<Radar_MaxNum;++i)
		{
			/*暂时联动完全直接接管
			ROAData->RadarState[i] = ROAData->RadarState_Asb[i];*/

			/*合并得到*/
			ROA_ObsMerge_OR(&(ROAData->RadarState_Self[i]),&(ROAData->RadarState_Asb[i]), &(ROAData->RadarState[i]));
		}
	}
	else {}

	return 1;
}

/***************************************************************************************
** 函数名称: ROA_DataProcess
** 功能描述: 雷达避障数据处理
** 参    数: *ROAData雷达避障数据结构体类型数据指针
** 返 回 值: None
****************************************************************************************/
void ROA_DataProcess(ROA_TypeDef *ROAData)
{
//	u8 i;
	
	/*扫描区域有效性判断*/
	if(ROAData->ObstacleAreaSelect > 8 || ROAData->ObstacleAreaSelect == 0)
	{
		ROAData->ObstacleAreaSelect = 1;
	}
	else
	{
		__asm__("nop");
	}
	
	/*根据是否为对角安装处理数据*/
	if(ROAData->InstallPattern == Pattern_Diagonal_L || ROAData->InstallPattern == Pattern_Diagonal_R)
	{
		if(Delay_Fuction(ROAData->SysTimeCnt, ROAData->DelayTimeCnt, ROAData->ChangeTime))
		{
			ROA_DiagonalScanAreaChange(ROAData);
			
			ROAData->DelayTimeCnt = ROAData->SysTimeCnt;
		}
		else
		{
			__asm__("nop");
		}
	}
	else
	{
		ROA_ScanAreaChange(ROAData);
	}
	
//	/*项目使用兴颂雷达时反转雷达输出信号*/
//	if(ROAData->RadarBrand == Brand_Hinson)
//	{
//		for(i = 0;i<Radar_MaxNum;++i)
//		{
//			if(Get_Bit(ROAData->RadarInput[i],InputState_Enable))
//			{
//				ROAData->RadarOutput[i] = ((~ROAData->RadarOutput[i]) & 0x3F);
//			}
//			else
//			{
//				__asm__("nop");
//			}
//		}
//	}
//	else
//	{
//		__asm__("nop");
//	}

	//联动时强制关闭自身的某些位置的障碍状态
	ROA_ObsMerge_CLR(&(ROAData->ForceCLR[0]), &(ROAData->RadarState_Self[0]), &(ROAData->RadarState_Self_ForceCLR[0]));

	//执行的障碍状态更新
	ROA_ObsSelUpdate(ROAData);

    //将各侧障碍状态 转换为 三层独立状态
    ROA_GitObsSta(ROAData);
    //根据雷达状态处理控制数据
	ROA_CtrlDataProcess(ROAData);

	//wxb@20240718 392现场更新：侧面触发内层时，不管有没有侧向速度，前后方向速度都置0
    ROA_SideObsStop(ROAData);
    
}

/*****************************************************END OF FILE*********************************************/
