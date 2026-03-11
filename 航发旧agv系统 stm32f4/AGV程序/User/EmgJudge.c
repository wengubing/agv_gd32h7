/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: EmgJudge.c
** 创 建 人: 文小兵  
** 描    述: 故障判断按类型分类和模块分类，整理到单独源文件封装进行调用
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年4月12日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define EmgJudge_GLOBALS

//自定义引用			
#include "includes.h"
#include "CBB_Init.h"
#include "EmgJudge.h"
#include "ACC_Project.h"
#include "CommConf.h"
#include "ACC_Init.h"
#include "Debug.h"

//外部对象声明

//全局对象声明

//车体急停检出
bool EmgJudge_CarEstop(ACC_Faults_TypeDef *p2Faults)
{
	bool res=0;
	/*IO急停*/
	if(g_IOInputs.EmgStop.ID != ICN_Disable )
	{
		if(g_IOInputs.EmgStop.StableValue==0)
		{
			p2Faults->LoseControl.Bit.CarEstop = 1;
			res=1;
		}
		else
		{
			p2Faults->LoseControl.Bit.CarEstop = 0;
			res = 0;
		}
	}
	/*通讯急停*/
	else if (g_Para.EQPCommSource.EN.PLC_Estop == 1 )
	{
		if(EQP_PLC_Estop.p2Data->Estop != 0X0F)
		{
			p2Faults->LoseControl.Bit.CarEstop = 1;
			res=1;
		}
		else
		{
			p2Faults->LoseControl.Bit.CarEstop = 0;
			res = 0;
		}
	}
	
	return res;
}

//非受控异常检出
bool EmgJudge_LoseControl(ACC_Faults_TypeDef *p2Faults)
{
	bool res=0;
	char i=0;
	char errCnt=0;

    if(EmgJudge_CarEstop(&g_Faults))
		errCnt++;
	else {}

	//canopen主站异常检出
	if(CanMaster.Para.SVOType != Can_SVOType_Null)//canopen类型有效
	{
		if(CanMaster.MasterStatu == Disconnected)
			p2Faults->LoseControl.Bit.CanMstErr = 1;
	}
	else
	{
		errCnt = 0;
		//安川时，将m3板卡通信超时、核心cpu故障、单轴故障标准，临时处理为CanMstErr
		if (EQPMsts.ANC.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.ANC.CommId != CCFCN_Disable)
			errCnt++;

		if (EQP_ANC.p2RData->CPUError)
			errCnt++;

		if (EQP_ANC.p2RData->DrvError)
			errCnt++;

		if(errCnt)
			p2Faults->LoseControl.Bit.CanMstErr = 1;
	}

	//如果伺服是安川，CanMaster.Para.SVOType配置为Can_SVOType_Null，将anc的数据填到svo对象中，实现伺服掉线和伺服故障的检出
	if(CanMaster.Para.SVOType == Can_SVOType_Null)
	{
		u8 i = 0;
		//总安川伺服数量
		u8 ANC_Num = g_LiftData.CtrlPara.CtrlNnm + g_MotorResolving.Para.MotorNumber;
	
		for (i = 0; i < ANC_Num; i++)
		{
			
 			//安川伺服没有单轴的在线状态反馈,直接强制在线避免误报
			SVO[i].SlaveHeartbeatError = Operational;
	
			SVO[i].FaultCode = EQP_ANC.p2RData->ErrorCode[i];
		}
	}

	//canopen从站离线检出
	{
		static uint8_t Temp_Num = 0;
		errCnt = 0;
		if(Temp_Num == 0)
		{
			/*车型是泰坦，默认行走电机数量+3 是总的Can Open节点数量*/
			if(		g_Para.ChassisType == ACC_ChassisType_Titan
				&&	g_Para.Lift.PfType == LC_PFTP_Titan)
			{
				Temp_Num = g_Para.Walk.MotorNum + 3;
			}
			/*平台不是泰坦三点液压，默认行走电机数量+1 是总的Can Open节点数量*/
			else if(   g_Para.Lift.PfType != LC_PFTP_Null 
					&& g_Para.AppOutSource.FucBit.DACLifeMoter == 0)
			{
				Temp_Num = g_Para.Walk.MotorNum + 1;
			}
			/*其他情况下，只考虑行走电机*/
			else
			{
				Temp_Num = g_Para.Walk.MotorNum;
			}
		}
		else {}

		for (i = 0; i < Temp_Num; i++)
		{
			if(SVO[i].SlaveHeartbeatError == Disconnected) 
			{
				errCnt++;
			}
		}

		if(errCnt)
			p2Faults->LoseControl.Bit.CanSlvOffline = 1;
		else {}
	}

	//行走伺服异常检出
	{
		errCnt=0;
		for (i = 0; i < g_Para.Walk.MotorNum; i++)
		{
			if(SVO[i].FaultCode)
			{
				errCnt++;
				//记录下异常的伺服编号和故障码
				if(p2Faults->SvoErr_ID==0)
				{
					p2Faults->SvoErr_ID = i+1;
					p2Faults->SvoErr_Code = SVO[i].FaultCode;
				}
			}
			else {}
		}

		if(errCnt)
			p2Faults->LoseControl.Bit.WalkSvoErr = 1;
		else {}
	}

	//升降伺服异常检出——悬挂的举升暂未兼容
	{
		/*有举升平台 并且不是靠科润阳光*/
		if(	   g_Para.Lift.PfType != LC_PFTP_Null 
			&& g_Para.AppOutSource.FucBit.DACLifeMoter == 0)
		{
			static uint8_t Temp_Num = 0;
			errCnt=0;
			if(Temp_Num == 0)
			{
				/*车型是泰坦，默认3个举升伺服*/
				if(		g_Para.ChassisType == ACC_ChassisType_Titan
					&&	g_Para.Lift.PfType == LC_PFTP_Titan)
				{
					Temp_Num = 3;
				}
				/*其他情况下，默认1个举升伺服*/
				else
				{
					Temp_Num = 1;
				}
			}

			for (i =  g_Para.Walk.MotorNum; i < g_Para.Walk.MotorNum + Temp_Num; i++)
			{
				if(SVO[i].FaultCode)
				{
					errCnt++;
					//记录下异常的伺服编号和故障码
					if(p2Faults->SvoErr_ID==0)
					{
						p2Faults->SvoErr_ID = i+1;
						p2Faults->SvoErr_Code = SVO[i].FaultCode;
					}
				}
			}
			if(errCnt)
			{
				p2Faults->LoseControl.Bit.LiftSvoErr = 1;
			}
		}
	}

	if(p2Faults->LoseControl.All)
		res=1;
	else
		res=0;

	return res;
}

//上位机急停
bool EmgJudge_DispatchEstop(void)
{
	if(g_DHR_AGVInformation[Para_Index_AGV_EMGControl] == 0x01
		&&g_HostData.ControlMODEL == Style_Auto
	)   
	{
		g_Faults.EmgStop.Bit.Dsp = 1;
		g_DHR_AGVInformation[Para_Index_AGV_EMGControlRecovery] = 0x00;
	} 
	else
	{
		if(g_DHR_AGVInformation[Para_Index_AGV_EMGControlRecovery] == 0x01 || g_HostData.ControlMODEL != Style_Auto)
			g_Faults.EmgStop.Bit.Dsp = 0;		    
	}

	return g_Faults.EmgStop.Bit.Dsp;
}

//急停汇总检出
bool EmgJudge_Estop(void)
{
    //调度系统急停检出
	EmgJudge_DispatchEstop();

    //遥控器急停检出
	{
		//已覆盖场景：
		//1、有线+无线				单车
		//2、仅有线，但是有接收机	单车
		//3、仅有线，没有接收机		单车
		//uint8_t Bus_EmgStop 	= g_WHData.EmgStop;	//1-急停松开 0-急停按下
		uint8_t Bus_EmgStop = g_ChannelSwitch.CtrlCmd.Button.EStop;	//1-急停松开 0-急停按下
		uint8_t IO_EmgStop 		= 1;				//1-急停松开 0-急停按下
		uint8_t RemoteWiredSel 	= 1;				//1-有线 0-无线
		/*IO急停配置后，获取IO结果*/
		if(g_IOInputs.RemoteReceiverEmgStop.ID 	!= ICN_Disable)
		{IO_EmgStop 	= g_IOInputs.RemoteReceiverEmgStop.StableValue;}
		/*面板有线/无线旋钮配置后，获取旋钮结果*/
		if (g_IOInputs.RemoteWiredSel.ID 		!= ICN_Disable)
		{RemoteWiredSel	=g_IOInputs.RemoteWiredSel.StableValue;}

		/*有线急停判断，只关心通讯急停（否则联动情况下有问题）*/
		if(RemoteWiredSel == 1)
		{
			if(Bus_EmgStop == 0)
			{
				g_Faults.EmgStop.Bit.Remote = 1;
			}
			else
			{
				g_Faults.EmgStop.Bit.Remote = 0;
			}
		}
		/*无线急停判断，IO和通讯急停同时生效*/
		else
		{
			if(Bus_EmgStop == 0 || IO_EmgStop == 0)
			{
				g_Faults.EmgStop.Bit.Remote = 1;
			}
			else
			{
				g_Faults.EmgStop.Bit.Remote = 0;
			}
		}
	}

	//小急停检出
	if (g_IOInputs.Little_Estop.ID != ICN_Disable && g_IOInputs.Little_Estop.StableValue == 0)
	{
		g_Faults.EmgStop.Bit.Little = 1;
	}
	else
	{
		g_Faults.EmgStop.Bit.Little = 0;
	}

	if(g_Faults.EmgStop.All)
		return 1;
	else
		return 0;
}


//通信超时报警
bool EmgJudge_TimeOut(ACC_Faults_TypeDef *p2Faults)
{
	//bool res=0;
	char i = 0;
	//uint32_t tempval=0;
	//p2Faults->TimeOut.All=0;//清除所有通信超时报警

	//调度系统通信超时报警
	if(
		//EQPSlvs.DSP.TimeOutMs > g_Para.EmgJudge.TimeOutJudge//通信超时
        EQPSlvs.DSP.TimeOutMs > g_Para.EmgJudge.TimeOutDsp && g_Para.EmgJudge.TimeOutDsp<60000
		&& EQPSlvs.DSP.CommId != CCFCN_Disable				//通信使能
		&& g_ChannelSwitch.in_Channel == ACCCC_Auto			//自动运行模式
		&& g_Sys_1ms_Counter > 120000						//系统启动时间大于60s 实测tplink启动时间30s左右
	)
		p2Faults->TimeOut.Bit.DSP = 1;
	else
		p2Faults->TimeOut.Bit.DSP = 0;
	
	//遥控器A通信超时报警
	if(EQPMsts.WHA.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.WHA.CommId != CCFCN_Disable)
	{
		if(g_ChannelSwitch.in_Channel == ACCCC_WirelessRemote)
			p2Faults->TimeOut.Bit.RemoteReceiverA = 1;
		
		if(g_ChannelSwitch.in_Channel == ACCCC_WiredRemote)
			p2Faults->TimeOut.Bit.RemoteWired = 1;
	}
	else
	{
		p2Faults->TimeOut.Bit.RemoteReceiverA = 0;
		p2Faults->TimeOut.Bit.RemoteWired = 0;
	}

	//遥控器B通信超时报警
	if(EQPMsts.WHB.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.WHB.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.RemoteReceiverB = 1;
	else 
		p2Faults->TimeOut.Bit.RemoteReceiverB = 0;
	
	//BMS通信超时报警
	p2Faults->TimeOut.Bit.BMS = 0;//先关闭，不同接发再在下面触发
	if((EQPMsts.BMS_CommConv.TimeOutMs > g_Para.EmgJudge.TimeOutJudge || EQP_BMS_CommConv.p2Data->RedState == 0) && EQPMsts.BMS_CommConv.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.BMS = 1;
	if(EQPMsts.BMS_MCB.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.BMS_MCB.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.BMS = 1;
	if ((EQPMsts.BMS_CATL.TimeOutMs > g_Para.EmgJudge.TimeOutJudge || EQP_BMS_CATL.p2Data->RedState == 0) && EQPMsts.BMS_CATL.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.BMS = 1;

	//语音通信超时报警
	if(EQPMsts.Voice.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.Voice.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.Voice = 1;
	else
		p2Faults->TimeOut.Bit.Voice = 0;

	//AD
	if(EQPMsts.AD.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.AD.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.AD = 1;
	else
		p2Faults->TimeOut.Bit.AD = 0;

	//AD1
	if (EQPMsts.AD1.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.AD1.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.AD1 = 1;
	else
		p2Faults->TimeOut.Bit.AD1 = 0;
	
	/*温度*/
	if (EQPMsts.Thermodetector.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.Thermodetector.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.Thermodetector = 1;
	else
		p2Faults->TimeOut.Bit.Thermodetector = 0;
		

	//前磁导航
	if(EQPMsts.MagF.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.MagF.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.MagF = 1;
	else
		p2Faults->TimeOut.Bit.MagF = 0;

	//后磁导航
	if(EQPMsts.MagB.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.MagB.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.MagB = 1;
	else
		p2Faults->TimeOut.Bit.MagB = 0;

	//左磁导航
	if(EQPMsts.MagL.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.MagL.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.MagL = 1;
	else
		p2Faults->TimeOut.Bit.MagL = 0;
	
	//右磁导航
	if(EQPMsts.MagR.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.MagR.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.MagR = 1;
	else
		p2Faults->TimeOut.Bit.MagR = 0;

	//RFID
	if(EQPMsts.RFID.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.RFID.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.RFID = 1;
	else
		p2Faults->TimeOut.Bit.RFID = 0;

	//PGVUpward
	if(EQPMsts.PGVUpward.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.PGVUpward.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.PGVUpward = 1;
	else
		p2Faults->TimeOut.Bit.PGVUpward = 0;

	//PGVDownward
	if(EQPMsts.PGVDownward.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.PGVDownward.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.PGVDownward = 1;
	else
		p2Faults->TimeOut.Bit.PGVDownward = 0;

	//SerialExp
	if(EQPMsts.SerialExp.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.SerialExp.CommId != CCFCN_Disable)
		p2Faults->TimeOut.Bit.SerialExp = 1;
	else
		p2Faults->TimeOut.Bit.SerialExp = 0;

	//SerialExp串口通讯板挂载的传感器通讯超时
	if(EQPMsts.SerialExp.CommId != CCFCN_Disable)
	{
		DataPool_BitDef *p_LinkSta 	= EQP_SerialExp.p2Data.LinkSta ;
		DataPool_BitDef *p_FunEn 	= &g_Para.SerialExpObjSource ;
		//BMS通讯超时
		if(p_LinkSta->BMS == 0 && p_FunEn->BMS == 1 )
		{
			p2Faults->TimeOut.Bit.BMS = 1;
		}
		else
			p2Faults->TimeOut.Bit.BMS = 0;

		//语音喇叭通讯超时
		if(p_LinkSta->Voice == 0 && p_FunEn->Voice == 1 )
		{
			p2Faults->TimeOut.Bit.Voice = 1;
		}
		else
			p2Faults->TimeOut.Bit.Voice = 0;

		//AD模块通讯超时
		if(p_LinkSta->AD == 0 && p_FunEn->AD == 1 )
		{
			p2Faults->TimeOut.Bit.AD = 1;
		}
		else
			p2Faults->TimeOut.Bit.AD = 0;

		//AD1模块通讯超时
		if(p_LinkSta->AD1 == 0 && p_FunEn->AD1 == 1 )
		{
			p2Faults->TimeOut.Bit.AD = 1;
		}
		
		//前磁导航通讯超时
		if(p_LinkSta->MagF == 0 && p_FunEn->MagF == 1 )
		{
			p2Faults->TimeOut.Bit.MagF = 1;
		}
		else
			p2Faults->TimeOut.Bit.MagF = 0;

		//后磁导航通讯超时
		if(p_LinkSta->MagB == 0 && p_FunEn->MagB == 1 )
		{
			p2Faults->TimeOut.Bit.MagB = 1;
		}
		else
			p2Faults->TimeOut.Bit.MagB = 0;

		//左磁导航通讯超时
		if(p_LinkSta->MagL == 0 && p_FunEn->MagL == 1 )
		{
			p2Faults->TimeOut.Bit.MagL = 1;
		}
		else
			p2Faults->TimeOut.Bit.MagL = 0;

		//右磁导航通讯超时
		if(p_LinkSta->MagR == 0 && p_FunEn->MagR == 1 )
		{
			p2Faults->TimeOut.Bit.MagR = 1;
		}
		else
			p2Faults->TimeOut.Bit.MagR = 0;

		//RFID通讯超时
		if(p_LinkSta->RFID == 0 && p_FunEn->RFID == 1 )
		{
			p2Faults->TimeOut.Bit.RFID = 1;
		}
		else
			p2Faults->TimeOut.Bit.RFID = 0;

		//PGV通讯超时
		if(p_LinkSta->PGVUpward == 0 && p_FunEn->PGVUpward == 1 )
		{
			p2Faults->TimeOut.Bit.PGVUpward = 1;
		}
		else
			p2Faults->TimeOut.Bit.PGVUpward = 0;

		if(p_LinkSta->PGVDownward == 0 && p_FunEn->PGVDownward == 1 )
		{
			p2Faults->TimeOut.Bit.PGVDownward = 1;
		}
		else
			p2Faults->TimeOut.Bit.PGVDownward = 0;

		if(p_LinkSta->PGVUpward_F == 0 && p_FunEn->PGVUpward_F == 1 )
		{
			//p2Faults->TimeOut.Bit.PGVUpward = 1;
		}
		
		if(p_LinkSta->PGVUpward_B == 0 && p_FunEn->PGVUpward_B == 1 )
		{
			//p2Faults->TimeOut.Bit.PGVUpward = 1;
		}
		
		if(p_LinkSta->PGVDownward_F == 0 && p_FunEn->PGVDownward_F == 1 )
		{
			//p2Faults->TimeOut.Bit.PGVDownward = 1;
		}
		
		if(p_LinkSta->PGVDownward_B == 0 && p_FunEn->PGVDownward_B == 1 )
		{
			//p2Faults->TimeOut.Bit.PGVDownward = 1;
		}
		
		if(p_LinkSta->CommConv == 0 && p_FunEn->CommConv == 1 )
		{
			p2Faults->TimeOut.Bit.CommConv = 1;
		}
		else
			p2Faults->TimeOut.Bit.CommConv = 0;
		
		if(p_LinkSta->IPC == 0 && p_FunEn->IPC == 1 )
		{
			//p2Faults->TimeOut.Bit.BMS = 1;//临时和其他共用
		}
		
		if(p_LinkSta->LaserSpot == 0 && p_FunEn->LaserSpot == 1 )
		{
			p2Faults->TimeOut.Bit.LaserSpot = 1;
		}
		else
			p2Faults->TimeOut.Bit.LaserSpot = 0;
		
	}
	//CommConv/CommConv2
	if ((EQPMsts.CommConv.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.CommConv.CommId != CCFCN_Disable) ||
		(EQPMsts.CommConv2.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.CommConv2.CommId != CCFCN_Disable))
		p2Faults->TimeOut.Bit.CommConv = 1;
	else
		p2Faults->TimeOut.Bit.CommConv = 0;

	//差动轮系车才有角度超时判断
	if(g_Para.ChassisType == ACC_ChassisType_Titan || g_Para.ChassisType == ACC_ChassisType_Rhino)
	{
		static uint32_t WheelAngle_Bit = 0;
		if(WheelAngle_Bit == 0)
		{
			for (i = 0; i < g_Para.WheelAngle.Num; i++)
			{
				M_K_SET_BIT(WheelAngle_Bit, i);
			}
		}
		//有几个角度传感器就 WheelAngle_Bit有多少个bit为1
		if((EQP_CommConv.RD[0]&WheelAngle_Bit) != WheelAngle_Bit)
		{
			p2Faults->TimeOut.Bit.WheelAngle = 1;
		}
		else
		{
			//角度传感器掉线 不允自动恢复
		}
	}
	
	//只有泰坦升降平台 才有磁滞传感器传感器
	//仅在升降或悬挂模式下进行判断
	if (g_ChannelSwitch.CtrlCmd.Mode == ACCMode_Lift || g_ChannelSwitch.CtrlCmd.Mode == ACCMode_Absorption)
	{
		if ((g_LiftData.CtrlPara.PfType == LC_PFTP_Titan || g_AbsorptionData.CtrlPara.PfType == LC_PFTP_Titan) && g_Para.ChassisType == ACC_ChassisType_Titan)
		{
			if (g_Para.AppInSource.FucBit.CommConvLiftHeight == 1)
			{
				//磁滞和角度数量一般匹配 不用再单独计算，但是需要磁滞传感器从1站号开始编码
				if (g_Para.AppInSource.FucBit.CommConvDouble == 0)
				{
					static uint32_t WheelHig_Bit = 0;
					if (WheelHig_Bit == 0)
					{
						for (i = 0; i < g_Para.WheelAngle.Num; i++)
						{
							M_K_SET_BIT(WheelHig_Bit, i);
						}
					}

					if ((EQP_CommConv.RD[24] & WheelHig_Bit) != WheelHig_Bit)
					{
						p2Faults->TimeOut.Bit.WheelHeight = 1;
					}
					else
					{
						//磁滞传感器掉线 不允自动恢复
					}
				}
				//如果使能了分离的角度磁滞传感器 ，需要使用第二版卡判断高度数据
				else
				{
					static uint32_t WheelHig_Bit = 0;
					if (WheelHig_Bit == 0)
					{
						for (i = 0; i < g_Para.WheelAngle.Num; i++)
						{
							M_K_SET_BIT(WheelHig_Bit, i);
						}
					}

					if ((EQP_CommConv2.RD[0] & WheelHig_Bit) != WheelHig_Bit)
					{
						p2Faults->TimeOut.Bit.WheelHeight = 1;
					}
					else
					{
						//磁滞传感器掉线 不允自动恢复
					}
				}
			}
			else{}
		}
	}
	else
	{
		p2Faults->TimeOut.Bit.WheelHeight = 0;
	}

    //联动模式下
	if (g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb && AtDbug.Bit.AsbErrOff == 0)
    {
        //联动通信超时检出
        if(g_Asb.Para.Role == AsbModel_Master)//主车
        {
            if(EQPMsts.Assembly_Master.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.Assembly_Master.CommId != CCFCN_Disable)
                p2Faults->TimeOut.Bit.Asb = 1;
            else
                p2Faults->TimeOut.Bit.Asb = 0;
        }
        else if(g_Asb.Para.Role == AsbModel_Slave)//从车
        {
            if(EQPSlvs.Assembly_Slave.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPSlvs.Assembly_Slave.CommId != CCFCN_Disable)
                p2Faults->TimeOut.Bit.Asb = 1;
            else
                p2Faults->TimeOut.Bit.Asb = 0;
        }
        else {}

        //光斑传感器通信超时检出
        if(g_Asb.Para.Role == AsbModel_Master)
        {
            if(EQPMsts.LaserSpot.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.LaserSpot.CommId != CCFCN_Disable)
                p2Faults->TimeOut.Bit.LaserSpot = 1;
            else
                p2Faults->TimeOut.Bit.LaserSpot = 0;
        }
    }
    else //非联动模式下 清除故障
    {
        p2Faults->TimeOut.Bit.Asb = 0;
        p2Faults->TimeOut.Bit.LaserSpot = 0;
    }

	//非调度运行模式关闭磁导航、rfid、pgv报警
	if(g_ChannelSwitch.in_Channel != ACCCC_Auto)
	{
		p2Faults->TimeOut.Bit.MagF = 0;
		p2Faults->TimeOut.Bit.MagB = 0;
		p2Faults->TimeOut.Bit.MagL = 0;
		p2Faults->TimeOut.Bit.MagR = 0;
		p2Faults->TimeOut.Bit.RFID = 0;
		p2Faults->TimeOut.Bit.PGVUpward = 0;
		p2Faults->TimeOut.Bit.PGVDownward = 0;
	}

	if(p2Faults->TimeOut.All)
		return 1;
	else
		return 0;
}


//升降报警检出
bool EmgJudge_Lift(ACC_Faults_TypeDef *p2Faults)
{
	bool res=0;
	if(g_Para.Lift.PfType == LC_PFTP_Titan)
	{
		/*if(g_LiftData.WarningOut.TooHigh)
			p2Faults->Lift.Bit.TooHigh = 1;
		else
			p2Faults->Lift.Bit.TooHigh = 0;

		if (g_LiftData.WarningOut.TooLow)//CBB中已修改参数设0时关闭报警，开启注意进行测试
			p2Faults->Lift.Bit.TooLow = 1;
		else
			p2Faults->Lift.Bit.TooLow = 0;*/
		
		if(g_LiftData.WarningOut.DiffLimit)
			p2Faults->Lift.Bit.DiffLimit = 1;
		else
			p2Faults->Lift.Bit.DiffLimit = 0;

		/*if (g_LiftData.WarningOut.DiffLimitRear)
			p2Faults->Lift.Bit.DiffLimitRear = 1;
		else
			p2Faults->Lift.Bit.DiffLimitRear = 0;*/

		/*清除执行超时ms记录已加到模式切换完成时，开启该报警需要进行验证是否还会误触发
		if(g_LiftData.WarningOut.ExecTimeout)
			p2Faults->Lift.Bit.ExecTimeout = 1;
		else
			p2Faults->Lift.Bit.ExecTimeout = 0;	*/
	}
	

	if(p2Faults->Lift.All)
		res=1;
	else
		res=0;

	return res;
}
//悬挂报警检出
bool EmgJudge_Absorption(ACC_Faults_TypeDef *p2Faults)
{
	bool res=0;
	if(g_Para.Absorption.PfType == LC_PFTP_Titan)
	{
		if(g_AbsorptionData.WarningOut.TooHigh)
			p2Faults->Absorption.Bit.TooHigh = 1;
		else
			p2Faults->Absorption.Bit.TooHigh = 0;

		if (g_AbsorptionData.WarningOut.TooLow)//CBB中已修改参数设0时关闭报警，开启注意进行测试
			p2Faults->Absorption.Bit.TooLow = 1;
		else
			p2Faults->Absorption.Bit.TooLow = 0;
		
		if(g_AbsorptionData.WarningOut.DiffLimit)
			p2Faults->Absorption.Bit.DiffLimit = 1;
		else
			p2Faults->Absorption.Bit.DiffLimit = 0;

		if (g_AbsorptionData.WarningOut.DiffLimitRear)
			p2Faults->Absorption.Bit.DiffLimitRear = 1;
		else
			p2Faults->Absorption.Bit.DiffLimitRear = 0;

		//清除执行超时ms记录已加到模式切换完成时，开启该报警需要进行验证是否还会误触发
	//	if(g_AbsorptionData.WarningOut.ExecTimeout)
	//		p2Faults->Absorption.Bit.ExecTimeout = 1;
	//	else
	//		p2Faults->Absorption.Bit.ExecTimeout = 0;
	}

	if(p2Faults->Absorption.All)
		res=1;
	else
		res=0;

	return res;
}
//磁条巡线离线状态机
bool MagOutLineSTM(void)
{
	//模式发生变化，强制复位状态机，在模式切换里操作
	//g_Faults.TraceOutline.STM = TraceOutline_Init;

    //跟新实时里程
    g_Faults.TraceOutline.Distance_Real = g_VirtualOdoData.Distance;

	switch (g_Faults.TraceOutline.STM)
	{
	case TraceOutline_Init:
		if(ACC_IsTraceMode(AccData.OrgCmd.Mode))
		{
            g_Faults.TraceOutline.STM = TraceOutline_ModeIn;
            g_Faults.TraceOutline.Distance_Lose = 0;
        }
		break;

	case TraceOutline_ModeIn:
		//进入巡线模式后，判断是否开始运动
		if(ACC_IsTraceMode(AccData.CalCmd.Mode))
		{
            g_Faults.TraceOutline.STM = TraceOutline_Move;
            g_Faults.TraceOutline.Distance_Init = g_Faults.TraceOutline.Distance_Real;
        }

		//进入巡线模式后，判断是否有磁条
		if(AccData.OrgCmd.Mode == ACCMode_PosTrace)
		{
			if(g_MNData[0].MN_Read.Status==0 || g_MNData[1].MN_Read.Status==0)
			g_Faults.TraceOutline.STM = TraceOutline_Stop;
		}
		else if(AccData.OrgCmd.Mode == ACCMode_InfTrace)
		{
			if(g_MNData[2].MN_Read.Status==0 || g_MNData[3].MN_Read.Status==0)
			g_Faults.TraceOutline.STM = TraceOutline_Stop;
		}
		break;

	case TraceOutline_Move:
		//巡线运动中，判断是否离线
		if(AccData.OrgCmd.Mode == ACCMode_PosTrace)
		{
			if (g_MNData[0].MN_Read.Status == 0 || g_MNData[1].MN_Read.Status == 0)
			{
				g_Faults.TraceOutline.STM = TraceOutline_Lose;
				g_Faults.TraceOutline.Distance_Init = g_Faults.TraceOutline.Distance_Real;
			}
			else {}
		}
		else if(AccData.OrgCmd.Mode == ACCMode_InfTrace)
		{
			if(g_MNData[2].MN_Read.Status==0 || g_MNData[3].MN_Read.Status==0)
			{
				g_Faults.TraceOutline.STM = TraceOutline_Lose;
				g_Faults.TraceOutline.Distance_Init = g_Faults.TraceOutline.Distance_Real;
			}
			else {}
		}
		else {}
		break;

	case TraceOutline_Lose:
        //计算离线后走的距离
        g_Faults.TraceOutline.Distance_Lose = g_Faults.TraceOutline.Distance_Real - g_Faults.TraceOutline.Distance_Init;

		//判断行走里程是否超过限制
		if(g_Faults.TraceOutline.Distance_Lose > (g_Para.EmgJudge.MagOutLineDistance/1000.0f))
			g_Faults.TraceOutline.STM = TraceOutline_Stop;
		
		//判断是否重新检测到磁条
		if(AccData.OrgCmd.Mode == ACCMode_PosTrace)
		{
			if(g_MNData[0].MN_Read.Status!=0 && g_MNData[1].MN_Read.Status!=0)
			{
                //重新检测到磁条，复位记录的初始里程
                g_Faults.TraceOutline.Distance_Init = g_Faults.TraceOutline.Distance_Real;
                g_Faults.TraceOutline.STM = TraceOutline_Move;
            }
		}
		else if(AccData.OrgCmd.Mode == ACCMode_InfTrace)
		{
			if(g_MNData[2].MN_Read.Status!=0 && g_MNData[3].MN_Read.Status!=0)
			{
                //重新检测到磁条，复位记录的初始里程
                g_Faults.TraceOutline.Distance_Init = g_Faults.TraceOutline.Distance_Real;
                g_Faults.TraceOutline.STM = TraceOutline_Move;
            }
		}
		break;

	case TraceOutline_Stop:
		if(EQP_SerialExp.p2Data.LS_ReadData->Operating == 1)
		{
			g_Faults.TraceOutline.STM = TraceOutline_Init;
		}
		break;

	
	default:
		break;
	}
	return 1;
}

//磁条离线超时计数
bool Emg_MgaOutLine_Cnt(void)
{
	LS_Read_StDef	*LS_ReadData	= EQP_SerialExp.p2Data.LS_ReadData;
	//使能导航板雷赛伺服
	if(	g_Para.SerialExpObjSource.LsLiftModule_F == 1 &&
		g_Para.SerialExpObjSource.LsLiftModule_B == 1 &&
		g_Para.SerialExpObjSource.LsLiftModule_L == 1 &&
		g_Para.SerialExpObjSource.LsLiftModule_R == 1 )
	{
		if(LS_ReadData->Operating == 2)
		{
			//正向巡线时，
			if(AccData.OrgCmd.Mode==ACCMode_PosTrace)
			{
				//前侧磁导航未检测到磁条
				if(g_MNData[0].MN_Read.Status==0)
				{
					if(g_Faults.MagOutLineTime[0]<65535)
						g_Faults.MagOutLineTime[0]++;
				}
				else
					g_Faults.MagOutLineTime[0]=0;

				//后侧磁导航未检测到磁条
				if(g_MNData[1].MN_Read.Status==0)
				{
					if(g_Faults.MagOutLineTime[1]<65535)
						g_Faults.MagOutLineTime[1]++;
				}
				else
					g_Faults.MagOutLineTime[1]=0;
			}
			else
			{
				g_Faults.MagOutLineTime[0]=0;
				g_Faults.MagOutLineTime[1]=0;
			}

			//横向巡线时，
			if(AccData.OrgCmd.Mode==ACCMode_InfTrace)
			{
				//左侧磁导航未检测到磁条
				if(g_MNData[2].MN_Read.Status==0)
				{
					if(g_Faults.MagOutLineTime[2]<65535)
						g_Faults.MagOutLineTime[2]++;
				}
				else
					g_Faults.MagOutLineTime[2]=0;

				//右侧磁导航未检测到磁条
				if(g_MNData[3].MN_Read.Status==0)
				{
					if(g_Faults.MagOutLineTime[3]<65535)
						g_Faults.MagOutLineTime[3]++;
				}
				else
					g_Faults.MagOutLineTime[3]=0;
			}
			else
			{
				g_Faults.MagOutLineTime[2]=0;
				g_Faults.MagOutLineTime[3]=0;
			}
		}
		else
		{
				g_Faults.MagOutLineTime[0]=0;
				g_Faults.MagOutLineTime[1]=0;
				g_Faults.MagOutLineTime[2]=0;
				g_Faults.MagOutLineTime[3]=0;
		}
	}
	else
	{
		//正向巡线时，
		if(AccData.OrgCmd.Mode==ACCMode_PosTrace)
		{
			//前侧磁导航未检测到磁条
			if(g_MNData[0].MN_Read.Status==0)
			{
				if(g_Faults.MagOutLineTime[0]<65535)
					g_Faults.MagOutLineTime[0]++;
			}
			else
				g_Faults.MagOutLineTime[0]=0;

			//后侧磁导航未检测到磁条
			if(g_MNData[1].MN_Read.Status==0)
			{
				if(g_Faults.MagOutLineTime[1]<65535)
					g_Faults.MagOutLineTime[1]++;
			}
			else
				g_Faults.MagOutLineTime[1]=0;
		}
		else
		{
			g_Faults.MagOutLineTime[0]=0;
			g_Faults.MagOutLineTime[1]=0;
		}

		//横向巡线时，
		if(AccData.OrgCmd.Mode==ACCMode_InfTrace)
		{
			//左侧磁导航未检测到磁条
			if(g_MNData[2].MN_Read.Status==0)
			{
				if(g_Faults.MagOutLineTime[2]<65535)
					g_Faults.MagOutLineTime[2]++;
			}
			else
				g_Faults.MagOutLineTime[2]=0;

			//右侧磁导航未检测到磁条
			if(g_MNData[3].MN_Read.Status==0)
			{
				if(g_Faults.MagOutLineTime[3]<65535)
					g_Faults.MagOutLineTime[3]++;
			}
			else
				g_Faults.MagOutLineTime[3]=0;
		}
		else
		{
			g_Faults.MagOutLineTime[2]=0;
			g_Faults.MagOutLineTime[3]=0;
		}		
	}
    return 1;
}
//色带离线超时计数
bool Emg_ColourOutLine_Cnt(void)
{
	//正向巡线时，
	if(AccData.OrgCmd.Mode==ACCMode_PosTrace)
	{
		uint8_t ColorBand_F_sta = g_ColorBand[0].ColorBand_Deviation.Status;
		uint8_t ColorBand_B_sta = g_ColorBand[1].ColorBand_Deviation.Status;
		//前侧磁导航未检测到色带
		if(ColorBand_F_sta != 1)
		{
			if(g_Faults.ColourOutLineTime[0]<65535)
				g_Faults.ColourOutLineTime[0]++;
		}
		else
			g_Faults.ColourOutLineTime[0]=0;

		//后侧磁导航未检测到色带
		if(ColorBand_B_sta != 1)
		{
			if(g_Faults.ColourOutLineTime[1]<65535)
				g_Faults.ColourOutLineTime[1]++;
		}
		else
			g_Faults.ColourOutLineTime[1]=0;
	}
	else
	{
		g_Faults.ColourOutLineTime[0]=0;
		g_Faults.ColourOutLineTime[1]=0;
	}

	//横向巡线时，
	if(AccData.OrgCmd.Mode==ACCMode_InfTrace)
	{
		uint8_t ColorBand_L_sta = g_ColorBand[2].ColorBand_Deviation.Status;
		uint8_t ColorBand_R_sta = g_ColorBand[3].ColorBand_Deviation.Status;
		//左侧磁导航未检测到色带
		if(ColorBand_L_sta != 1)
		{
			if(g_Faults.ColourOutLineTime[2]<65535)
				g_Faults.ColourOutLineTime[2]++;
		}
		else
			g_Faults.ColourOutLineTime[2]=0;

		//右侧磁导航未检测到色带
		if(ColorBand_R_sta != 1)
		{
			if(g_Faults.ColourOutLineTime[3]<65535)
				g_Faults.ColourOutLineTime[3]++;
		}
		else
			g_Faults.ColourOutLineTime[3]=0;
	}
	else
	{
		g_Faults.ColourOutLineTime[2]=0;
		g_Faults.ColourOutLineTime[3]=0;
	}
    return 1;
}
//充电极板打开且未检测到充电电流，开始计时
bool Emg_ChargIngNoCurrent_Cnt(void)
{
    if(AccData.IOCtrl.ChargingMC && g_BMSData.BMS_Read.Current < 0 && g_BMSData.BMS_Read.SOC<100)
	{
		if(g_Faults.ChargIngMCTime < 65535)
			g_Faults.ChargIngMCTime++;
	}
	else
		g_Faults.ChargIngMCTime = 0;

    return 1;
}

//充电时，开始计时
bool Emg_ChargIng_Cnt(void)
{
    if(g_BMSData.BMS_Read.Current > 0)
	{
		if(g_Faults.ChargIngTime < 65535)
			g_Faults.ChargIngTime++;
	}
	else
		g_Faults.ChargIngTime = 0;

    return 1;
}

//报警检出ms计数接口
bool Emg_MsCnt(void)
{
    //磁条离线超时计数
    Emg_MgaOutLine_Cnt();

	//色带离线超时计数
	Emg_ColourOutLine_Cnt();

    //充电极板打开且未检测到充电电流，开始计时
    Emg_ChargIngNoCurrent_Cnt();

    //充电时，开始计时
    Emg_ChargIng_Cnt();

    return 1;
}

bool EmgJudge_MagOutLine(ACC_Faults_TypeDef *p2Faults)
{
	char ErrCnt=0;

	//磁条离线超里程检出
	MagOutLineSTM();
	if (g_Para.EmgJudge.MagOutLineDistance < 60000)//参数大于等于60000时关闭检测
	{
		if (g_Faults.TraceOutline.STM == TraceOutline_Stop && EQP_SerialExp.p2Data.LS_ReadData->Operating == 2)
			ErrCnt++;
		else {}
	}
	else {}

	//磁导航离线超时检出
	if (g_Para.EmgJudge.MagOutLineTime < 60000 && EQP_SerialExp.p2Data.LS_ReadData->Operating == 2)//参数大于等于60000时关闭检测
	{
		//正向巡线时，
		if (AccData.OrgCmd.Mode == ACCMode_PosTrace)
		{
			//前侧或后侧磁导航未检测到磁条
			if (g_Faults.MagOutLineTime[0] > g_Para.EmgJudge.MagOutLineTime || g_Faults.MagOutLineTime[1] > g_Para.EmgJudge.MagOutLineTime)
				ErrCnt++;
			else {}
		}
		//横向巡线时，
		else if (AccData.OrgCmd.Mode == ACCMode_InfTrace)
		{
			//左侧或右侧磁导航未检测到磁条
			if (g_Faults.MagOutLineTime[2] > g_Para.EmgJudge.MagOutLineTime || g_Faults.MagOutLineTime[3] > g_Para.EmgJudge.MagOutLineTime)
				ErrCnt++;
			else {}
		}
		else {}
	}
	else {}

    if(ErrCnt)
        return 1;
    else
        return 0;
}

//色带脱线报警
uint8_t EmgJudge_ColourOutLine(ACC_Faults_TypeDef *p2Faults)
{
	char ErrCnt=0;

	//磁导航离线超时检出
	if (g_Para.EmgJudge.ColourOutLineTime < 60000)//参数大于等于60000时关闭检测
	{
		//正向巡线时，
		if (AccData.OrgCmd.Mode == ACCMode_PosTrace)
		{
			//前侧或后侧磁导航未检测到色带
			if (g_Faults.ColourOutLineTime[0] > g_Para.EmgJudge.ColourOutLineTime || g_Faults.ColourOutLineTime[1] > g_Para.EmgJudge.ColourOutLineTime)
				ErrCnt++;
			else {}
		}
		//横向巡线时，
		else if (AccData.OrgCmd.Mode == ACCMode_InfTrace)
		{
			//左侧或右侧磁导航未检测到磁条
			if (g_Faults.ColourOutLineTime[2] > g_Para.EmgJudge.ColourOutLineTime || g_Faults.ColourOutLineTime[3] > g_Para.EmgJudge.ColourOutLineTime)
				ErrCnt++;
			else {}
		}
		else {}
	}
	else {}

    if(ErrCnt)
        return 1;
    else
        return 0;

}
/*工控机通讯超时——定位雷达通信超时*/
uint8_t EmgJudge_SlamScoomFail(void)
{
	uint8_t Res = 0;
	ACCMode_TypeDef mode = AccData.OrgCmd.Mode;
	if(mode == ACCMode_PosTrace || mode == ACCMode_InfTrace || mode == ACCMode_Rotation)
	{
		if(g_Para.SerialExpObjSource.IPC == 1)
		{
			if(EQP_SerialExp.p2Data.LinkSta->IPC == 0)
			{
				Res = 1;
			}
		}
		else if(g_Para.EQPCommSource.EN.IPC == 1)
		{
			if(EQPMsts.IPC.TimeOutMs > g_Para.EmgJudge.TimeOutJudge)
			{
				Res = 2;
			}
		}
	}
	return Res;
}
//力士乐中间件，定位失败报故障——定位导航失败
uint8_t EmgJudge_NavigationRadarErr(void)
{
	uint8_t Res = 0;
	ACCMode_TypeDef mode = AccData.OrgCmd.Mode;
	if(mode == ACCMode_PosTrace || mode == ACCMode_InfTrace)
	{
		/*力士乐中间件，定位失败报故障——定位导航失败*/
		{
			ROKIT_Read_StDef *p_ROKIT = &(g_ROKITData.ROKIT_Read);
			//输出定位不可靠的时候
			if(p_ROKIT->LocationSta != ROKIT_LocationStae_HighReliability)
			{
				Res = 1;
			}
			//中间件有故障的时候
			else if(p_ROKIT->ErrCode != ROKIT_ErrCode_Null)
			{
				Res = 2;
			}
			//定位数据同时为0时
			else if(  p_ROKIT->LocationX == 0
					&&p_ROKIT->LocationY == 0
					&&p_ROKIT->LocationZ == 0)
			{
				Res = 3;
			}
		}
	}
	return Res;
}

//功能异常检出
bool EmgJudge_Func(ACC_Faults_TypeDef *p2Faults)
{
	bool res=0;
    char i=0;
	static u16 SignalErrCnt=0;

	//模式切换 舵轮角度切换时检出是否超时
	if(AccData.ModeSwitch.SteeringStep > ACC_SteeringStep_Init && AccData.ModeSwitch.SteeringStep < ACC_SteeringStep_Finish)
	{
		if((AccData.in_Ms-AccData.ModeSwitch.MsRecord_Steering) > g_Para.EmgJudge.ModeSwitchTimeOutJudge 
			&& g_Para.EmgJudge.ModeSwitchTimeOutJudge < 65535
			)
			p2Faults->Func.Bit.ModeSwitchTimeOut = 1;
		else {}
	}
	else {}

	//无线遥控模式下，遥控器已连接，且未休眠，但无信号
	if(g_ChannelSwitch.in_Channel==ACCCC_WirelessRemote && g_ChannelSwitch.WirelessRemoteComminitOk 
		&& g_WHData.WirelessSleep==0 && g_WHData.CountPerSecend<2)
	{
		if(SignalErrCnt < 65535)
			SignalErrCnt++;
		else {}

		if(SignalErrCnt > 200)//连续200次检测到信号异常
			p2Faults->Func.Bit.RemoteWirelessSignalErr = 1;
		else {}
	}
	else
	{
		p2Faults->Func.Bit.RemoteWirelessSignalErr = 0;
		SignalErrCnt = 0;
	}

	//防撞触边触发
	if(g_IOInputs.ObsBarF.StableValue)
		//p2Faults->Func.Bit.CrashBarF = g_IOInputs.ObsBarF.StableValue;
		p2Faults->Func.Bit.CrashBarF = 1;//只触发不复位
	else {}

	if(g_IOInputs.ObsBarB.StableValue)
		p2Faults->Func.Bit.CrashBarB = 1;
	else {}

	if(g_IOInputs.ObsBarL.StableValue)
		p2Faults->Func.Bit.CrashBarL = 1;
	else {}

	if(g_IOInputs.ObsBarR.StableValue)
		p2Faults->Func.Bit.CrashBarR = 1;
	else {}

    //轮系角度限制检出
	for(i=0;i<g_Para.WheelAngle.Num;i++)
	{
		if(g_WheelAngle[i].Res > g_MotorResolving.Para.Mxy[i].RealAngMax_Left || g_WheelAngle[i].Res < g_MotorResolving.Para.Mxy[i].RealAngMax_Right)
		{
			p2Faults->Func.Bit.wheelAngleLimited = 1;
		}
	}

    //BMS相关报警
	{
		if(g_BMSData.BMS_Read.SOC<10)
		{
			p2Faults->Func.Bit.LowBatteryStop = 1;
		}

		if(g_BMSData.BMS_Read.Alarm_Code==3 || g_BMSData.BMS_Read.Alarm_Code==4)
		{
			p2Faults->Func.Bit.BMSErrStop = 1;
		}
	}

	/*依据导航方式的不同,切对于脱离轨迹的判断 调用不同的API*/
	{
		uint8_t SlamScoomFailSta 		= 0;
		uint8_t NavigationRadarErrSta 	= 0;
		uint8_t ColourOutLineSta 		= 0;
		uint8_t MagOutLineSta 			= 0;
		SlamScoomFailSta 		= EmgJudge_SlamScoomFail();
		NavigationRadarErrSta 	= EmgJudge_NavigationRadarErr();
		ColourOutLineSta 		= EmgJudge_ColourOutLine(p2Faults);
		MagOutLineSta 			= EmgJudge_MagOutLine(p2Faults);
		//确定当前应该用什么传感器的结果，送入到磁条巡线模块中
		switch(g_Para.TarceType.FunType)
		{
			default:
			case Para_TarceType_Null_Fun:{}break;

			case Para_TarceType_Magnetic:
			{
				/*磁条离线 就报警*/
				if(MagOutLineSta != 0)			{p2Faults->Func.Bit.MagOutLine = 1;}
				else							{p2Faults->Func.Bit.MagOutLine = 0;}
			}break;

			case Para_TarceType_ColorBand:
			{
				//只使用色带的时候，只关心色带是否脱线
				if(ColourOutLineSta != 0)		{p2Faults->Func.Bit.ColourOutLine = 1;}
				else							{p2Faults->Func.Bit.ColourOutLine = 0;}

			}break;

			case Para_TarceType_Slam:
			{
				//只使用slam的时候，只关心slam是否正常
				if(SlamScoomFailSta != 0)		{p2Faults->Func.Bit.SlamLostCom = 1;}
				else							{p2Faults->Func.Bit.SlamLostCom = 0;}

				if(NavigationRadarErrSta != 0)	{p2Faults->Func.Bit.SlamLostLocation = 1;}
				else							{p2Faults->Func.Bit.SlamLostLocation = 0;}
			}break;
			case Para_TarceType_Slam_Magnetic:
			{
				//当slam和磁导航同时失效时才报警
				if(    ( MagOutLineSta != 0)
					&& ( SlamScoomFailSta != 0))
				{
					p2Faults->Func.Bit.MagOutLine = 1;
					p2Faults->Func.Bit.SlamLostCom = 1;
				}
				else if(   ( MagOutLineSta != 0)
						&& ( NavigationRadarErrSta != 0))
				{
					p2Faults->Func.Bit.MagOutLine = 1;
					p2Faults->Func.Bit.SlamLostLocation = 1;
				}
				else
				{
					p2Faults->Func.Bit.ColourOutLine = 0;
					p2Faults->Func.Bit.SlamLostCom = 0;
					p2Faults->Func.Bit.SlamLostLocation = 0;
				}

			}break;
			case Para_TarceType_Slam_ColorBand:
			{
				//当slam和色带同时失效时才报警
				if(    ( ColourOutLineSta != 0)
					&& ( SlamScoomFailSta != 0))
				{
					p2Faults->Func.Bit.ColourOutLine = 1;
					p2Faults->Func.Bit.SlamLostCom = 1;
				}
				else if(   ( ColourOutLineSta != 0)
						&& ( NavigationRadarErrSta != 0))
				{
					p2Faults->Func.Bit.ColourOutLine = 1;
					p2Faults->Func.Bit.SlamLostLocation = 1;
				}
				else
				{
					p2Faults->Func.Bit.ColourOutLine = 0;
					p2Faults->Func.Bit.SlamLostCom = 0;
					p2Faults->Func.Bit.SlamLostLocation = 0;
				}

			}break;

			case Para_TarceType_QR:
			{
				
			}break;
		}
	}

    /*联动模块故障，非联动模式自动清除所有联动故障*/
	{
		uint32_t* Temp_Err = (u32*)(&g_Asb.Output.AsbErr);
		if (*Temp_Err != 0x00 && g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb)
		p2Faults->Func.Bit.AsbErr = 1;
	else
		p2Faults->Func.Bit.AsbErr = 0;
	}

	if(g_Faults.Func.All)
		res=1;

	return res;
}

//自动运行异常检出
bool EmgJudge_AutoRun()
{
	bool res=0;

	//从自动运行对象检出故障
	//如果为自动运行模式
	if(g_ChannelSwitch.in_Channel == ACCCC_Auto)
	{
		g_Faults.Auto.All = g_HostData.AGV_Alarm_Evt1;
	}
	else
	{
		g_Faults.Auto.All = 0;
	}

	/*汇总*/
	
	if(g_Faults.Auto.All)
		res=1;
	else
		res=0;

	return res;
}

//其它异常检出
bool EmgJudge_OtherErr(ACC_Faults_TypeDef *p2Faults)
{
	bool Res=0;

	//代码异常
	if(g_CodeErr.All)
		g_Faults.Other.Bit.CodeErr = 1;
	else
		g_Faults.Other.Bit.CodeErr = 0;

	//轮系角度解算错误
	if(g_MotorResolving.Output.CtrlAngLimit_Flg)
		g_Faults.Other.Bit.WheelAngCalErr = 1;
	else
		g_Faults.Other.Bit.WheelAngCalErr = 0;

	//拓展异常检出
	if (CodeLogicErr.Res && g_Para.SysFucBit.LogicErr2Emg == 1)
		g_Faults.Other.Bit.Expand = 1;
	else
		g_Faults.Other.Bit.Expand = 0;

	if(g_Faults.Other.All)
		Res=1;

	//泰坦三点液压系统才检出相关功能
	if(g_LiftData.CtrlPara.PfType == LC_PFTP_Titan || g_AbsorptionData.CtrlPara.PfType == LC_PFTP_Titan)
	{
		//负载相关提示报警
		if(g_LoadDetection.Err.OverLoad)
		{
        p2Faults->Remind.Class.MoveLimit.Bit.LimitUp_Overload = 1;
        p2Faults->Remind.Class.MoveLimit.Bit.LimitWalk_Overload = 1;
		}
		else
		{
			p2Faults->Remind.Class.MoveLimit.Bit.LimitUp_Overload = 0;
			p2Faults->Remind.Class.MoveLimit.Bit.LimitWalk_Overload = 0;
		}

		/*单组压力过大 现在无条件限制上升和行走*/
		if (g_LoadDetection.Err.UnbalanceCore)
		{
			p2Faults->Remind.Class.MoveLimit.Bit.LimitUp_UnbalancedLoad = 1;
			p2Faults->Remind.Class.MoveLimit.Bit.LimitWalk_UnbalancedLoad = 1;
		}
		else
		{
			p2Faults->Remind.Class.MoveLimit.Bit.LimitUp_UnbalancedLoad = 0;
			p2Faults->Remind.Class.MoveLimit.Bit.LimitWalk_UnbalancedLoad = 0;
		}
		
		/*中心偏移过大 有条件提示和限制行走
		if(
			g_LoadDetection.Err.SingleMax//CBB检出异常
			&& g_LoadDetection.WeightShow > p2Faults->Para.UnbalancedLoad_WeightMin//重载
			&& g_LiftData.Height.Avg > p2Faults->Para.UnbalancedLoad_HeightMin//高位
		)
			p2Faults->Remind.Class.MoveLimit.Bit.LimitWalk_UnbalancedLoad_Core = 1;
		else
			p2Faults->Remind.Class.MoveLimit.Bit.LimitWalk_UnbalancedLoad_Core = 0;*/
	}
    
	return Res;
}

//仅提醒故障检出
bool EmgJudge_RemindOnly(ACC_Faults_TypeDef *p2Faults)
{
	bool res=0;
    char err=0;

	//语音播报模块通信超时
	p2Faults->Remind.Class.RemindOnly.Bit.Voice = 0;
//	if(EQPMsts.Voice.TimeOutMs > g_Para.EmgJudge.TimeOutJudge && EQPMsts.Voice.CommId != CCFCN_Disable)
//		p2Faults->Remind.Class.RemindOnly.Bit.Voice = 1;
	//接拓展板上
	//if(EQP_SerialExp.p2Data.Voice_ReadComStatus != 1)
//	if(EQP_SerialExp.p2Data.Voice_ReadComStatus > g_Para.EmgJudge.TimeOutJudge)
//		p2Faults->Remind.Class.RemindOnly.Bit.Voice = 1;

	//充电极板未改变时不能退出静止模式
	if(AccData.OrgCmd.Mode != ACCMode_Still && AccData.IOCtrl.ChargingMC)
		p2Faults->Remind.Class.RemindOnly.Bit.Charging = 1;
	else
		p2Faults->Remind.Class.RemindOnly.Bit.Charging = 0;

	//充电极板打开后，延迟一段时间未能检测到BMS充电电流
	if(g_Faults.ChargIngMCTime > 30000)
		p2Faults->Remind.Class.RemindOnly.Bit.ChargingCurrentErr = 1;
	else
		p2Faults->Remind.Class.RemindOnly.Bit.ChargingCurrentErr = 0;

    //抱闸切换错误
    if(g_IOInputs.ExternalBrakeSel.StableValue)
        p2Faults->Remind.Class.RemindOnly.Bit.BreakFault = 1;
    else
        p2Faults->Remind.Class.RemindOnly.Bit.BreakFault = 0;

    //BMS提醒
    if(g_BMSData.BMS_Read.SOC<20)
		p2Faults->Remind.Class.RemindOnly.Bit.LowBatteryRemind = 1;
    else
        p2Faults->Remind.Class.RemindOnly.Bit.LowBatteryRemind = 0;

	if(g_BMSData.BMS_Read.Alarm_Code==1 || g_BMSData.BMS_Read.Alarm_Code==2)
		p2Faults->Remind.Class.RemindOnly.Bit.ErrRemind = 1;
    else
        p2Faults->Remind.Class.RemindOnly.Bit.ErrRemind = 0;

    err=0;
	//上线朝向未初始化
    if(g_ChannelSwitch.DirectionNotInit_Flag)
	{
        err++;
        p2Faults->Remind.Class.RemindOnly.Bit.Auto_DirectionNotInit = 1;
    }
    else
        p2Faults->Remind.Class.RemindOnly.Bit.Auto_DirectionNotInit = 0;

    //切自动时，未在十字磁条或rfid上
    if(g_ChannelSwitch.NotFindPointSign_Flag)
    {
		err++;
        p2Faults->Remind.Class.RemindOnly.Bit.Auto_NotFindPointSign = 1; 
    }
    else
        p2Faults->Remind.Class.RemindOnly.Bit.Auto_NotFindPointSign = 0;

    if(err)
        p2Faults->Remind.Class.RemindOnly.Bit.Auto_NotAllowed = 1;
    else
        p2Faults->Remind.Class.RemindOnly.Bit.Auto_NotAllowed = 0;

    //激光避障触发
    if(g_SLN.in_ObsState)
        p2Faults->Remind.Class.RemindOnly.Bit.LaserObs = 1;
    else
        p2Faults->Remind.Class.RemindOnly.Bit.LaserObs = 0;

	//拓展异常检出
	if (CodeLogicErr.Res && g_Para.SysFucBit.LogicErr2Remind == 1)
		p2Faults->Remind.Class.RemindOnly.Bit.Expand = 1;
	else
		p2Faults->Remind.Class.RemindOnly.Bit.Expand = 0;

    res = p2Faults->Remind.Class.RemindOnly.All;

	return res;
}

//控制命令有摇杆值
bool EmgJudge_CtrlCmdHaveJoyStick(void)
{
    if(g_ChannelSwitch.CtrlCmd.Vxyz.x != 0 || g_ChannelSwitch.CtrlCmd.Vxyz.y != 0 || g_ChannelSwitch.CtrlCmd.Vxyz.z != 0)
        return 1;
    else
        return 0;
}

//运动限制故障检出
bool EmgJudge_MoveLimit(void)
{
	bool res=0;
    //充电时限制行走
    if(g_Faults.ChargIngTime>2000 && EmgJudge_CtrlCmdHaveJoyStick() && ACC_IsWalkMode(AccData.OrgCmd.Mode))
        g_Faults.Remind.Class.MoveLimit.Bit.LimitWalk_Charging = 1;
    else
        g_Faults.Remind.Class.MoveLimit.Bit.LimitWalk_Charging = 0;

    res = g_Faults.Remind.Class.MoveLimit.All;
    return res;
}

//提醒检出
bool EmgJudge_Remind(void)
{
    bool res=0;

    //运动限制检出
    EmgJudge_MoveLimit();

    //仅提醒异常检出
    EmgJudge_RemindOnly(&g_Faults); 

    res = g_Faults.Remind.All;
    return res;                           
}

//受控停车汇总
bool EMG_Judge_Controlled(void)
{
    //急停
    g_Faults.ControlledStop.Bit.Remote = g_Faults.EmgStop.Bit.Remote;
    g_Faults.ControlledStop.Bit.Dsp = g_Faults.EmgStop.Bit.Dsp;
    g_Faults.ControlledStop.Bit.HMI_EST = g_Faults.EmgStop.Bit.HMI_EST;
    g_Faults.ControlledStop.Bit.Little = g_Faults.EmgStop.Bit.Little;

    //通信超时
    g_Faults.ControlledStop.Bit.DSP = g_Faults.TimeOut.Bit.DSP;
    g_Faults.ControlledStop.Bit.HMI = g_Faults.TimeOut.Bit.HMI;
    g_Faults.ControlledStop.Bit.RemoteReceiverA = g_Faults.TimeOut.Bit.RemoteReceiverA;
    g_Faults.ControlledStop.Bit.RemoteReceiverB = g_Faults.TimeOut.Bit.RemoteReceiverB;
    g_Faults.ControlledStop.Bit.RemoteWired = g_Faults.TimeOut.Bit.RemoteWired;
    g_Faults.ControlledStop.Bit.BMS = g_Faults.TimeOut.Bit.BMS;
    g_Faults.ControlledStop.Bit.Voice = g_Faults.TimeOut.Bit.Voice;
    g_Faults.ControlledStop.Bit.AD = (g_Faults.TimeOut.Bit.AD || g_Faults.TimeOut.Bit.AD1);//屏幕只有1个AD故障 暂时合在一起以免系统报警屏幕上没有提示
    g_Faults.ControlledStop.Bit.MagF = g_Faults.TimeOut.Bit.MagF;
    g_Faults.ControlledStop.Bit.MagB = g_Faults.TimeOut.Bit.MagB;
    g_Faults.ControlledStop.Bit.MagL = g_Faults.TimeOut.Bit.MagL;
    g_Faults.ControlledStop.Bit.MagR = g_Faults.TimeOut.Bit.MagR;
    g_Faults.ControlledStop.Bit.RFID = g_Faults.TimeOut.Bit.RFID;
    g_Faults.ControlledStop.Bit.PGVUpward = g_Faults.TimeOut.Bit.PGVUpward;
    g_Faults.ControlledStop.Bit.PGVDownward = g_Faults.TimeOut.Bit.PGVDownward;
    g_Faults.ControlledStop.Bit.SerialExp = g_Faults.TimeOut.Bit.SerialExp;
    g_Faults.ControlledStop.Bit.CommConv = g_Faults.TimeOut.Bit.CommConv;
    g_Faults.ControlledStop.Bit.WheelAngle = g_Faults.TimeOut.Bit.WheelAngle;
    g_Faults.ControlledStop.Bit.WheelHeight = g_Faults.TimeOut.Bit.WheelHeight;

    //升降报警
    g_Faults.ControlledStop.Bit.TooHigh = g_Faults.Lift.Bit.TooHigh;
    g_Faults.ControlledStop.Bit.TooLow = g_Faults.Lift.Bit.TooLow;
    g_Faults.ControlledStop.Bit.DiffLimit = g_Faults.Lift.Bit.DiffLimit;
    g_Faults.ControlledStop.Bit.DiffLimitRear = g_Faults.Lift.Bit.DiffLimitRear;
    g_Faults.ControlledStop.Bit.ExecTimeout = g_Faults.Lift.Bit.ExecTimeout;

	//悬挂报警
    g_Faults.ControlledStop.Bit.ASB_TooHigh 		= g_Faults.Absorption.Bit.TooHigh;
    g_Faults.ControlledStop.Bit.ASB_TooLow 			= g_Faults.Absorption.Bit.TooLow;
    g_Faults.ControlledStop.Bit.ASB_DiffLimit 		= g_Faults.Absorption.Bit.DiffLimit;
    g_Faults.ControlledStop.Bit.ASB_DiffLimitRear	= g_Faults.Absorption.Bit.DiffLimitRear;
    g_Faults.ControlledStop.Bit.ASB_ExecTimeout 	= g_Faults.Absorption.Bit.ExecTimeout;
    //功能异常
    g_Faults.ControlledStop.Bit.CanBusErr = g_Faults.Func.Bit.CanBusErr;
    g_Faults.ControlledStop.Bit.CanNoSlv = g_Faults.Func.Bit.CanNoSlv;
    g_Faults.ControlledStop.Bit.CanLessSlv = g_Faults.Func.Bit.CanLessSlv;
    g_Faults.ControlledStop.Bit.ModeSwitchTimeOut = g_Faults.Func.Bit.ModeSwitchTimeOut;
    g_Faults.ControlledStop.Bit.RemoteWirelessSignalErr = g_Faults.Func.Bit.RemoteWirelessSignalErr;
    g_Faults.ControlledStop.Bit.CrashBarF = g_Faults.Func.Bit.CrashBarF;
    g_Faults.ControlledStop.Bit.CrashBarB = g_Faults.Func.Bit.CrashBarB;
    g_Faults.ControlledStop.Bit.CrashBarL = g_Faults.Func.Bit.CrashBarL;
    g_Faults.ControlledStop.Bit.CrashBarR = g_Faults.Func.Bit.CrashBarR;
    g_Faults.ControlledStop.Bit.wheelAngleLimited = g_Faults.Func.Bit.wheelAngleLimited;
    g_Faults.ControlledStop.Bit.LowBatteryStop = g_Faults.Func.Bit.LowBatteryStop;
    g_Faults.ControlledStop.Bit.MagOutLine = g_Faults.Func.Bit.MagOutLine;
    g_Faults.ControlledStop.Bit.BMSErrStop = g_Faults.Func.Bit.BMSErrStop;

//	g_Faults.ControlledStop.Bit.ColourOutLine = g_Faults.Func.Bit.ColourOutLine;
//	g_Faults.ControlledStop.Bit.SlamLostLocation = g_Faults.Func.Bit.SlamLostLocation;
//	g_Faults.ControlledStop.Bit.SlamLostCom = g_Faults.Func.Bit.SlamLostCom;

    //其它异常
    g_Faults.ControlledStop.Bit.CodeErr = g_Faults.Other.Bit.CodeErr;
    g_Faults.ControlledStop.Bit.WheelAngCalErr = g_Faults.Other.Bit.WheelAngCalErr;
    g_Faults.ControlledStop.Bit.CtrlSysVoltageErr = g_Faults.Other.Bit.CtrlSysVoltageErr;
	g_Faults.ControlledStop.Bit.Expand = g_Faults.Other.Bit.Expand;

	//g_Faults.ControlledStop.Bit.PullApart//383上有
	g_Faults.ControlledStop.Bit.Thermodetector = g_Faults.TimeOut.Bit.Thermodetector;
	g_Faults.ControlledStop.Bit.AsbTimeOut = g_Faults.TimeOut.Bit.Asb;
	g_Faults.ControlledStop.Bit.AsbErr = g_Faults.Func.Bit.AsbErr;

	g_Faults.ControlledStop.Bit.Asb_LoseControl = g_Faults.OtherCar.Bit.Asb_LoseControl;
	g_Faults.ControlledStop.Bit.Asb_ControlStop = g_Faults.OtherCar.Bit.Asb_ControlStop;
	g_Faults.ControlledStop.Bit.Asb_MoveLimited = g_Faults.OtherCar.Bit.Asb_MoveLimited;
	g_Faults.ControlledStop.Bit.Asb_Warn = g_Faults.OtherCar.Bit.Asb_Warn;

	g_Faults.ControlledStop.Bit.ProjectExtend = g_Faults.Func.Bit.ProjectExtend;

    if(g_Faults.ControlledStop.All)
        return 1;
    else
        return 0;
}

//故障强制清除
bool EMG_ForceClear(void)
{
	ACC_Faults_TypeDef* p2Faults = &g_Faults;
    /*清除防撞触边故障*/
	if(g_SysApi.ClearObsBarLatch)
	{
		p2Faults->Func.Bit.CrashBarF = 0;
		p2Faults->Func.Bit.CrashBarB = 0;
		p2Faults->Func.Bit.CrashBarL = 0;
		p2Faults->Func.Bit.CrashBarR = 0;

		//g_SysApi.ClearObsBarLatch = 0;//hmi中为自恢复按钮
	}

    //关闭触边功能时清除防撞触边故障 触边没有临时控制
    if(g_Para.FunctionSwitch.BarEn.Front==0)
        p2Faults->Func.Bit.CrashBarF = 0;

    if(g_Para.FunctionSwitch.BarEn.Back==0)
        p2Faults->Func.Bit.CrashBarB = 0;

    if(g_Para.FunctionSwitch.BarEn.Left==0)
        p2Faults->Func.Bit.CrashBarL = 0;

    if(g_Para.FunctionSwitch.BarEn.Right==0)
        p2Faults->Func.Bit.CrashBarR = 0;

    //轮系角度限制解除
    if(g_SysApi.IgrWlAngLimit)
        p2Faults->Func.Bit.wheelAngleLimited = 0;
	
	/*自由升降时 忽略相关报警:超载、偏载、高低差等*/
	if(g_LiftData.Cmd.do_Force)
	{
		p2Faults->Lift.Bit.TooHigh = 0;
		p2Faults->Lift.Bit.TooLow = 0;
		p2Faults->Lift.Bit.DiffLimitRear = 0;
		p2Faults->Lift.Bit.ExecTimeout = 0;
		p2Faults->Lift.Bit.DiffLimit = 0;
	}

    return 1;
}

/*差动角度差过大异常检出 临时增加测试*/
bool EmgJudge_AngleCtrlErr(void)
{
	u8 TempFlg = 0, i = 0;

	if (ACC_IsWalkMode(AccData.ModeSwitchSMT))
	{//行走模式中
		g_Faults.DiffAng.Step = Faults_DiffAngStep_IsWalkMode;

		if (g_SLN.in_FlgMotion)
		{//正在行走时
			g_Faults.DiffAng.Step = Faults_DiffAngStep_Motion;

			for (i = 0; i < g_MotorResolving.Para.ModuleNumber; i++)
			{
				g_Faults.DiffAng.DiffAng[i] = g_MotorResolving.Input.ActualAng[i] - g_MotorResolving.TgtAng[i];
				if (fabs(g_Faults.DiffAng.DiffAng[i]) > g_Faults.DiffAng.Para_DiffAng)
					TempFlg++;
				else {}
			}

			if (TempFlg)
			{//角度差是有大于设定值
				g_Faults.DiffAng.Step = Faults_DiffAngStep_OverDiff;

				TempFlg = 0;
				g_Faults.DiffAng.DiffMs = g_Sys_1ms_Counter - g_Faults.DiffAng.MsStamp;
				if (g_Faults.DiffAng.DiffMs > g_Faults.DiffAng.Para_Time && g_Faults.DiffAng.Para_Time <= 60000)
					TempFlg++;
				else {}

				if (TempFlg)
				{//且持续设定时间
					g_Faults.DiffAng.Step = Faults_DiffAngStep_OverTime;
				}
				else {}
			}
			else
			{
				g_Faults.DiffAng.MsStamp = g_Sys_1ms_Counter;
			}

		}
		else
		{
			g_Faults.DiffAng.MsStamp = g_Sys_1ms_Counter;
		}
	}
	else
	{
		g_Faults.DiffAng.Step = Faults_DiffAngStep_Null;
		g_Faults.DiffAng.MsStamp = g_Sys_1ms_Counter;
	}

	if (g_Faults.DiffAng.Step == Faults_DiffAngStep_OverTime)
	{
		g_Faults.Auto.Bit.AGV_ALM_DownloadTowardsError = 1;//临时使用这一位进行测试
		return 1;
	}
	else
	{
		//g_Faults.Auto.Bit.AGV_ALM_DownloadTowardsError = 0;
		return 0;
	}
}

//各类异常检出调用
bool EMG_Judge_Call(void)
{
	ACC_Faults_TypeDef* p2Faults = &g_Faults;

	EmgJudge_LoseControl(p2Faults);                         //非受控异常检出
	EmgJudge_Estop();                                       //急停汇总检出
	EmgJudge_TimeOut(p2Faults);                             //通信超时报警
	EmgJudge_Lift(p2Faults);								//升降报警检出
	EmgJudge_Absorption(p2Faults);							//悬挂报警检出
	EmgJudge_Func(p2Faults);                                //功能异常检出
	EmgJudge_AutoRun();                                     //自动运行异常检出
	EmgJudge_OtherErr(p2Faults);                            //其它异常检出
	EmgJudge_AngleCtrlErr();								//角度差过大检出
	EmgJudge_Remind();                                      //提醒异常检出
	g_Para.p2Fucs.p2EmgJudge_ProjectExtend();               //项目拓展异常检出
	return 1;
}

//非受控停车统计
bool EMG_Judge_LoseControl_Count(ACC_Faults_TypeDef* p2Faults)
{
	ACC_FaultsLoseControl_UnionDef LoseControTemp = p2Faults->LoseControl;
	//先排除驱动器异常，因为领导要求驱动器异常改为受控停车
	LoseControTemp.Bit.WalkSvoErr = 0;
	LoseControTemp.Bit.LiftSvoErr = 0;

	if (LoseControTemp.All)//非受控停车
		p2Faults->TypeCnt.LoseControlStop++;
	else {}

	if (p2Faults->Other.Bit.CodeErr)//代码异常
		p2Faults->TypeCnt.LoseControlStop++;
	else {}

	//项目拓展异常 中的 非受控停车异常
	if (p2Faults->LoseControl.Bit.ProjectExtend)
		p2Faults->TypeCnt.LoseControlStop++;
	else {}

	if(p2Faults->TypeCnt.LoseControlStop)
		return 1;
	else
		return 0;
}

//受控停车统计
bool EMG_Judge_ControledStop_Count(ACC_Faults_TypeDef* p2Faults)
{
		//按领导要求 驱动器异常检出到受控停车里
	if (p2Faults->LoseControl.Bit.WalkSvoErr)
		p2Faults->TypeCnt.ControlledStop++;
	else {}

	if (p2Faults->LoseControl.Bit.LiftSvoErr)
		p2Faults->TypeCnt.ControlledStop++;
	else {}

	if (p2Faults->EmgStop.All)//急停
		p2Faults->TypeCnt.ControlledStop++;
	else {}

	if (p2Faults->TimeOut.All)//通信超时
		p2Faults->TypeCnt.ControlledStop++;
	else {}

	/*升降报警 不在受控停车统计里，以便改为动作限制
	if (p2Faults->Lift.All)
		p2Faults->TypeCnt.ControlledStop++;
	*/

	if (p2Faults->Func.All)//功能异常
		p2Faults->TypeCnt.ControlledStop++;
	else {}

	if (p2Faults->Auto.All)//自动运行异常
		p2Faults->TypeCnt.ControlledStop++;
	else {}

	if (p2Faults->Other.All)//轮系角度解算错误
		p2Faults->TypeCnt.ControlledStop++;
	else {}

	if (p2Faults->TypeCnt.ControlledStop)
		return 1;
	else
		return 0;
}

//动作限制统计
bool EMG_Judge_MoveLimit_Count(ACC_Faults_TypeDef* p2Faults)
{
	char cnt = 0;

	/*完全限制行走*/
	//超载
	if (g_Faults.Remind.Class.MoveLimit.Bit.LimitWalk_Overload && g_Para.SysFucBit.LimitWalk_Overload)
		cnt++;
	//偏载 单组
	if (g_Faults.Remind.Class.MoveLimit.Bit.LimitWalk_UnbalancedLoad && g_Para.SysFucBit.LimitWalk_UnbalancedLoad)
	{
		if (g_ChannelSwitch.CtrlCmd.HandleUnionVal != AsbModel_Asb)//联动时不限制
			cnt++;
	}

	/*偏载 重心超出范围
	if (g_Faults.Remind.Class.MoveLimit.Bit.LimitWalk_UnbalancedLoad_Core)
	{
		if (g_ChannelSwitch.CtrlCmd.HandleUnionVal != AsbModel_Asb)//联动时不限制
			cnt++;
	}*/

	//充电
	if (g_Faults.Remind.Class.MoveLimit.Bit.LimitWalk_Charging)
		cnt++;

	if (cnt)
		p2Faults->Output.Self.Bit.MoveLimited = 1;
	else
		p2Faults->Output.Self.Bit.MoveLimited = 0;

	//限制上升
	if (
		(p2Faults->Remind.Class.MoveLimit.Bit.LimitUp_Overload && g_Para.SysFucBit.LimitLiftUp_Overload)//超载
		|| (p2Faults->Remind.Class.MoveLimit.Bit.LimitUp_UnbalancedLoad && g_Para.SysFucBit.LimitLiftUp_UnbalancedLoad && g_ChannelSwitch.CtrlCmd.HandleUnionVal != AsbModel_Asb)//联动时不限制
		|| (p2Faults->Lift.Bit.DiffLimit)//高度差过大
		)
		p2Faults->Output.Self.Bit.UpLimited = 1;
	else
		p2Faults->Output.Self.Bit.UpLimited = 0;

	/*限制下降 暂时还有没现在下降的*/

	/*汇总*/
	//if(p2Faults->Remind.Class.MoveLimit.All)
	if (p2Faults->Output.Self.Bit.MoveLimited || p2Faults->Output.Self.Bit.UpLimited
		|| p2Faults->Output.Self.Bit.DownLimited
		)
		p2Faults->TypeCnt.MoveLimit++;
	else {}

	if (p2Faults->TypeCnt.MoveLimit)
		return 1;
	else
		return 0;
}

//单车故障类型统计
ACC_FaultsType_TypeDef EMG_Judge_Collect(void)
{
	ACC_FaultsType_TypeDef FaultsType;
	ACC_Faults_TypeDef* p2Faults = &g_Faults;
	bool DebugForceOverErr = 0;

	//故障检出统计计数清零
	p2Faults->TypeCnt.LoseControlStop = 0;
	p2Faults->TypeCnt.ControlledStop = 0;
	p2Faults->TypeCnt.MoveLimit = 0;
	p2Faults->TypeCnt.Warn = 0;

	//仿真时通过口令强制 比较危险
	if (AtDbug.Password == 985211)
	{
		if (AtDbug.OffAllErr)
		{
			DebugForceOverErr = 1;
		}
		else {}
	}
	else {}

	//危险硬件故障不响应！
	if (
		(
			g_Para.Debug.ForceOverErr == 0
			|| g_Faults.LoseControl.All //非受控停车不能屏蔽
			|| g_Faults.TimeOut.Bit.WheelAngle //角度超时不能屏蔽 接转接板的？
			)
		&& DebugForceOverErr == 0//没有调试关闭
		)
	{
		//非受控停车统计
		EMG_Judge_LoseControl_Count(p2Faults);

		//受控停车统计
		EMG_Judge_ControledStop_Count(p2Faults);

		//动作限制统计
		EMG_Judge_MoveLimit_Count(p2Faults);

		//仅提醒异常统计
		if (p2Faults->Remind.Class.RemindOnly.All)
			p2Faults->TypeCnt.Warn++;

		//故障检出统计
		if (p2Faults->TypeCnt.LoseControlStop)
			FaultsType = ACC_FaultsType_LoseControlStop;
		else if (p2Faults->TypeCnt.ControlledStop)
			FaultsType = ACC_FaultsType_ControlledStop;
		else if (p2Faults->TypeCnt.MoveLimit)
			FaultsType = ACC_FaultsType_MoveLimited;
		else if (p2Faults->TypeCnt.Warn)
			FaultsType = ACC_FaultsType_Warn;
		else
			FaultsType = ACC_FaultsType_Null;
	}
	else
		FaultsType = ACC_FaultsType_Null;

	return FaultsType;
}

//其它车异常标志检出 实际只是检出标志，真正报警的是联动模块输出的汇总结果
bool EMG_Judge_OtherCar(void)
{
	ACC_Faults_TypeDef* p2Faults = &g_Faults;

	//检出其它车异常标志
	p2Faults->OtherCar.Bit.Asb_LoseControl = 0;
	p2Faults->OtherCar.Bit.Asb_ControlStop = 0;
	p2Faults->OtherCar.Bit.Asb_MoveLimited = 0;
	p2Faults->OtherCar.Bit.Asb_Warn = 0;

	//联动模式下
	if (g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb)
	{
		if (p2Faults->Output.Asb_Input.Faults == ACC_FaultsType_LoseControlStop
			&& p2Faults->Output.Self.Faults != ACC_FaultsType_LoseControlStop)//且不是自己车非受控停车，以下同理
			p2Faults->OtherCar.Bit.Asb_LoseControl = 1;
		else if (p2Faults->Output.Asb_Input.Faults == ACC_FaultsType_ControlledStop
			&& p2Faults->Output.Self.Faults != ACC_FaultsType_ControlledStop)
			p2Faults->OtherCar.Bit.Asb_ControlStop = 1;
		else if (p2Faults->Output.Asb_Input.Faults == ACC_FaultsType_MoveLimited
			&& p2Faults->Output.Self.Faults != ACC_FaultsType_MoveLimited)
			p2Faults->OtherCar.Bit.Asb_MoveLimited = 1;
		else if (p2Faults->Output.Asb_Input.Faults == ACC_FaultsType_Warn
			&& p2Faults->Output.Self.Faults != ACC_FaultsType_Warn)
			p2Faults->OtherCar.Bit.Asb_Warn = 1;
		else {}
	}
	else//非联动模式下 清除故障标志
	{
		p2Faults->OtherCar.Bit.Asb_LoseControl = 0;
		p2Faults->OtherCar.Bit.Asb_ControlStop = 0;
		p2Faults->OtherCar.Bit.Asb_MoveLimited = 0;
		p2Faults->OtherCar.Bit.Asb_Warn = 0;
	}

	return 1;
}

//自身与联动模式下的 进行合并
bool EMG_Judge_Merge(void)
{
	ACC_Faults_TypeDef* p2Faults = &g_Faults;

	if (p2Faults->Output.Self.Bit.MoveLimited || p2Faults->Output.Asb_Input.Bit.MoveLimited)
		p2Faults->Output.Asb_Exe.Bit.MoveLimited = 1;
	else
		p2Faults->Output.Asb_Exe.Bit.MoveLimited = 0;

	if (p2Faults->Output.Self.Bit.UpLimited || p2Faults->Output.Asb_Input.Bit.UpLimited)
		p2Faults->Output.Asb_Exe.Bit.UpLimited = 1;
	else
		p2Faults->Output.Asb_Exe.Bit.UpLimited = 0;

	if (p2Faults->Output.Self.Bit.DownLimited || p2Faults->Output.Asb_Input.Bit.DownLimited)
		p2Faults->Output.Asb_Exe.Bit.DownLimited = 1;
	else
		p2Faults->Output.Asb_Exe.Bit.DownLimited = 0;

	if (p2Faults->Output.Self.Faults > p2Faults->Output.Asb_Input.Faults)
		p2Faults->Output.Asb_Exe.Faults = p2Faults->Output.Self.Faults;
	else
		p2Faults->Output.Asb_Exe.Faults = p2Faults->Output.Asb_Input.Faults;
	
	return 1;
}

//故障判断
EMG_Faults_TypeDef EMG_Judge()
{
	ACC_FaultsType_TypeDef FaultsType;
	ACC_Faults_TypeDef* p2Faults = &g_Faults;
	
	//各类异常检出
	EMG_Judge_Call();

    //故障强制清除 调用位置不可更改
    EMG_ForceClear();

	//单车异常类型统计
	FaultsType = EMG_Judge_Collect();

    //自身异常结果载人
    p2Faults->Output.Self.Faults = FaultsType;

	//联动模式下 载人联动结果
	if (g_ChannelSwitch.CtrlCmd.HandleUnionVal == AsbModel_Asb)//选择了联动
    	p2Faults->Output.Asb_Input = *((EMG_Faults_TypeDef*)(&g_Asb.Output.CarErr));
	else		
		memset(&p2Faults->Output.Asb_Input,0,sizeof(p2Faults->Output.Asb_Input));//用内存复制的方法清除p2Faults->Output.Asb_Input对象

    //自身与联动模式下的 进行合并
    EMG_Judge_Merge();

	//其它车异常标志检出
	EMG_Judge_OtherCar();
    
    g_Faults.Res = FaultsType;
	return g_Faults.Output.Asb_Exe;
}

//***********************************END OF FILE***********************************

