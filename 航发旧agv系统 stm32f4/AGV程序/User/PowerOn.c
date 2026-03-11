/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: PowerOn.c
** 创 建 人: 文小兵  
** 描    述: 上电启动过程分离到该文件中
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月28日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define PowerOn_GLOBALS

//自定义引用			
#include "includes.h"
#include "CBB_Init.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "EmgJudge.h"
#include "PowerOn.h"

//外部对象声明

//内部方法声明

/**************************************************************************
* 函数名:ACC_PowerOn_Process
* 功  能:上电过程控制
* 参  数:
* 返回值:NC
**************************************************************************/
void ACC_PowerOn_Process(u32 in_Ms,ACC_PowerOn_TypeDef *p)
{
	p->in_Ms=in_Ms;

	//车体急停按下时，直接关闭所有输出并报警
	//ACC_PowerOnStep_Init 时，不检测急停
	if(p->in_CarEmgStop && p->PowerOnStep > ACC_PowerOnStep_Init)
	{
		//p->Ctrl_CanOpenStart=0;
		//p->Ctrl_PowerOnBypass=0;
		//p->Ctrl_CtrlVoltageBypass=0;
		//p->Ctrl_MainDCDC=0;
		p->Ctrl_Precharge=0;
		p->Ctrl_MainContactor=0;
		p->Ctrl_MotorPrecharge=0;
		p->PowerOnStep=ACC_PowerOnStep_StartupFail;
		g_Faults.LoseControl.Bit.CarEstop=1;
		return;
	}

	switch (p->PowerOnStep)
	{
		case ACC_PowerOnStep_Init://初始化
		{
			//铅酸电池
			if(p->BatteryType == ACC_BatteryType_LeadBatteries)
			{
				p->Ctrl_CanOpenStart=1;
				p->PowerOnStep=ACC_PowerOnStep_CanopenCheck;
			}
			//低压锂电
			else if(p->BatteryType == ACC_CanopenStep_Self_LithiumBattery_Low)
			{
				p->Ctrl_PowerOnBypass=1;//启动旁路

				//等待BMS预充完成
				//392 48v自制bms不需要等待2000ms，等待反而启动失败
				//10ms  392的角度传感器转接板软件重启异常、改500ms，启动正常
				if(p->in_Ms>2000)
					p->Ctrl_CtrlVoltageBypass=1;
				
				//等待串口通信完成
				if(p->in_Ms>6000)
					p->PowerOnStep=ACC_PowerOnStep_BatteryCheck;
				

			}
			//高压锂电
			else if(p->BatteryType == ACC_CanopenStep_Self_LithiumBattery_High)
			{
				p->Ctrl_PowerOnBypass=1;
				if(p->in_Ms>500)
					p->Ctrl_CtrlVoltageBypass=1;

				//等待串口通信完成
				if(p->in_Ms>1000)
					p->PowerOnStep=ACC_PowerOnStep_BatteryCheck;
				
			}
			//外部供电
			else if (p->BatteryType == ACC_CanopenStep_Cable_Power_Supply)
			{
				p->Ctrl_CtrlVoltageBypass = 1;

				if (p->in_Ms > 4000)
				{
					p->Ctrl_MainContactor = 1;
					p->Ctrl_CtrllPower = 1; 
				}
				if (p->in_Ms > 5000)
				{
					p->PowerOnStep = ACC_PowerOnStep_CanopenCheck;          //直接跳转canopen的启动，无需等BMS
					p->Ctrl_CanOpenStart = 1;       
				}
			}
			p->MsRecord=p->in_Ms;
		}
		break;

		case ACC_PowerOnStep_BatteryCheck:
		{
			if(p->in_BatteryReady)
			{
				//p->Ctrl_CtrlVoltageBypass=1;
				p->Ctrl_MainDCDC=1;
				p->MsRecord=p->in_Ms;
				p->PowerOnStep=ACC_PowerOnStep_ControlVoltageCheck;
				/*启动预充*/
				p->Ctrl_Precharge = 1;
				p->Ctrl_MotorPrecharge = 1;
			}
			else if(p->in_Ms-p->MsRecord>10000)//电池启动检测超时
			{
				g_Faults.TimeOut.Bit.BMS=1;
				p->PowerOnStep=ACC_PowerOnStep_StartupFail;
			}
		}
		break;

		case ACC_PowerOnStep_ControlVoltageCheck:
		{
			if(p->BatteryType == ACC_CanopenStep_Self_LithiumBattery_Low)
			{
				p->Ctrl_CanOpenStart=1;
				p->MsRecord=p->in_Ms;
				p->PowerOnStep=ACC_PowerOnStep_CanopenCheck;
			}
			else if(p->in_Ms-p->MsRecord>1000)
			{
				if(p->in_ControlVoltageReady)
				{
					p->MsRecord=p->in_Ms;

					if (CanMaster.Para.SVOType != Can_SVOType_Null)
					{
						p->Ctrl_CanOpenStart = 1;
						p->PowerOnStep = ACC_PowerOnStep_CanopenCheck;
					}
					else
						p->PowerOnStep = ACC_PowerOnStep_ANC_PowerON;//安川伺服通过m3板卡启动
				}
				else if(p->in_Ms-p->MsRecord>1000)//系统电压检测异常
				{
					g_Faults.Other.Bit.CtrlSysVoltageErr=1;
					p->PowerOnStep=ACC_PowerOnStep_StartupFail;
				}
			}
		}
		break;

		case ACC_PowerOnStep_ANC_PowerON:
		{
			p->Ctrl_ANC=1;//安川m3板卡上电
			//等待时间到 检查ANC通信和状态
			if (p->in_Ms - p->MsRecord > 2000)
			{
				p->MsRecord = p->in_Ms;
				p->PowerOnStep = ACC_PowerOnStep_ANC_Check;
			}
		}
		break;

		case ACC_PowerOnStep_ANC_Check:
		{
			//检查安川m3板卡通信和状态
			if (EQPMsts.ANC.CommOkCnt > 10 && EQP_ANC.p2RData->CPUError == 0 && EQP_ANC.p2RData->DrvError == 0)
			{
				p->MsRecord = p->in_Ms;
				p->PowerOnStep = ACC_PowerOnStep_PrechargeOff;
			}
			else if (p->in_Ms - p->MsRecord > 5000)//安川m3板卡启动检测超时
			{
				p->MsRecord = p->in_Ms;
				//g_Faults.LoseControl.Bit.ANCStartTimeout = 1;
				p->PowerOnStep = ACC_PowerOnStep_StartupFail;
			}
		}
		break;

		case ACC_PowerOnStep_CanopenCheck:
		{
			if(p->in_CanopenStep==ACC_CanopenStep_Precharge)
			{
				/*维持预充*/
				p->Ctrl_Precharge=1;
				p->Ctrl_MotorPrecharge = 1;
				/*系统24V维持开启*/
				p->Ctrl_MainDCDC = 1;
			}
			else if(p->in_CanopenStep==ACC_CanopenStep_Active)
			{
				if(p->in_Ms-p->MsRecord>1000)
				{
					p->MsRecord=p->in_Ms;
					p->Ctrl_MainContactor=1;
					p->Ctrl_MotorContactor=1;
					p->PowerOnStep=ACC_PowerOnStep_PrechargeOff;
				}
			}
			else if(p->in_CanopenStep==ACC_CanopenStep_Fault || p->in_Ms-p->MsRecord>8000)//canopen启动失败 或 超时
			{
				p->MsRecord=p->in_Ms;
				g_Faults.LoseControl.Bit.CanOpenStartTimeout=1;
				p->Ctrl_Precharge=0;
				p->Ctrl_MainContactor=0;
				p->Ctrl_MotorPrecharge=0;
				p->Ctrl_MotorContactor=0;
				p->PowerOnStep=ACC_PowerOnStep_StartupFail;
			}
		}
		break;

		case ACC_PowerOnStep_PrechargeOff:
		{
			if(p->in_Ms-p->MsRecord>1000)
			{
				p->Ctrl_Precharge=0;
				p->Ctrl_MotorPrecharge=0;
				AccData.IOCtrl.Precharge=0;//直接关闭IO，不用等待统一接口执行
				AccData.IOCtrl.MotorPrecharge=0;
				
				//等待主接触器闭合在外部执行
				//if(p->in_Ms-p->MsRecord>2001)
				{
					p->MsRecord=p->in_Ms;
					p->PowerOnStep=ACC_PowerOnStep_StartupComplete;
					//g_VoicePlayer.BitInput.InitOk = 1;//启动完成播报
				}
			}
		}
		break;

		case ACC_PowerOnStep_StartupComplete:
		{

		}
		break;

		case ACC_PowerOnStep_StartupFail:
		{
			
		}
		break;

		default:
		break;
	}
	
}

//***********************************END OF FILE***********************************

