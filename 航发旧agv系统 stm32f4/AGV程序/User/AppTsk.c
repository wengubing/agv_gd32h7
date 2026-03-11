/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: AppTsk.c
** 创 建 人: 文小兵  
** 描    述: 任务管理
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0  
** 创建日期: 2024年5月14日 进行重构
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define  APPTSK_GLOBALS

//包含头文件
#include "includes.h"
#include "..\..\User\AppTsk.h"
#include "..\..\User\bsp.h"
#include "..\..\Drivers\ModbusRtuPoll\ModbusRtuPoll.h"
#include "..\..\Drivers\inc\drv_led.h"
#include "..\..\Drivers\inc\drv_stm32f4rtc.h"
#include "..\..\Drivers\inc\drv_ADC.h"
#include "..\..\Drivers\inc\drv_1Wire.h"
#include "..\..\Drivers\inc\drv_WatchDog.h"
#include "..\..\Drivers\inc\drv_Common.h"
#include "..\..\Drivers\inc\drv_IO.h"
#include "..\..\Drivers\inc\drv_232Usart.h"
#include "..\..\User\MB_LinkApp.h"
#include "app_SkeletonAGV.h"
#include "AgvChassisController.h"
#include "..\..\CBB\drivers\drv_RFIDData\drv_RFIDData.h"
#include "..\..\CBB\user\app_SoundLightNotice\app_SoundLightNotice.h"
#include "ACC_Project.h"
#include "LinkCanopenMaster.h"
#include "CommConf.h"
#include "Debug.h"	

//内部对象
u8 led_cnt=0;
STM32_CANi 	*p_stCAN = &STM32_CAN[user_CAN_ID];
CO_Data 	*P_CanOD = &CanObjectDict_Data;
LinkCanOpen	*pCm = &CanMaster;

u8 mb_slave_isrcnt;
u8 can_mailbox_sta[2];      //0:接收;1:发送;邮箱值

//外部对象

//内部方法

//外部方法
extern void Tsk_SoundLightNotice_Ctrl(void);

//内核温度传感器参数
#define		V25												760	//25℃对应的电压值,unit:mV
#define		Avg_Slope										2.5	//平均斜率,unit:mV/℃
#define		K12												0.80566	//3300/4096

//用户0.05ms任务
bool User50usTsk(u16 Acc)
{
	bool res = 0;

	g_DbugRtx.SysTickCnt+=Acc;
	
	return res;
}

//ADC任务
void Tsk_ADC(void)
{
	f32 temp_data=0.0;
	
	if(ST_ADC.ADC_Cov_data[0]>10)
		temp_data = (ST_ADC.ADC_Cov_data[0] * K12 - V25) / Avg_Slope + 25.0f;
	if(temp_data >= 125.0f)
	{
		temp_data=125.0;
	}
	else if(temp_data <= -40.0f)
	{
		temp_data=-40.0f;
	}

	HardInfo.CoreTemp = temp_data;
	
}

//RTX任务计数
bool RTXTskStart(ACC_TskInf_TypeDef* p2TskInf)
{
	char res = 0;
	p2TskInf->RunCnt++;
	p2TskInf->RunDealy = g_DbugRtx.SysTickCnt - p2TskInf->RunEnd;
	p2TskInf->RunStart = g_DbugRtx.SysTickCnt;
	return res;
}

//RTX任务计数结束
bool RTXTskEnd(ACC_TskInf_TypeDef* p2TskInf)
{
	char res = 0;
	p2TskInf->RunEnd = g_DbugRtx.SysTickCnt;
	p2TskInf->RunTime = p2TskInf->RunEnd - p2TskInf->RunStart;

	//退出忙碌状态
	p2TskInf->Busy = 0;
	return res;
}

/*********************************************************************************************************
*	函 数 名: Tsk_DS18b20_Process
*	功能说明: 18b20 data process
	//dat_interger = (tmp>>4) & 0x00FF;  //整数部分
	//dat_decimal = (tmp & 0x000F) * 625 ; //小数部分
*********************************************************************************************************/
void Tsk_DS18b20_Process(u8 *data)
{
	s32 buffer=0;
	////f32 tempdata;

	buffer = data[1];
	buffer = ((buffer << 8) | data[0]);

	if(buffer < 0)		//判断温度data_bf为正还是负
	{//为负
		buffer = -buffer;					
		////tempdata = -(float)(buffer  * 0.0625);//读取的数值除16得实际温度值
	}
	else
	{//为正
		////tempdata = (float)(buffer * 0.0625);//读取的数值除16得实际温度值
	}

	////DHR[R_DS18b20_Temp] = tempdata*10;
	
}

/*********************************************************************************************************
*	函 数 名: Tsk_RTC
*	功能说明: RTC任务
*********************************************************************************************************/
void Tsk_RTC(void)
{

}

//EEPROM任务管理
void Tsk_Memory_Manage(void)
{
	//保存用户参数
	if(EEPCtrl.EEPObj2EEP)
	{
		TSK_EE_PARA(EE_CMD_SAVE_USER_PARA);
		EEPCtrl.EEPObj2EEP=0;
	}

	//读取eep到eep对象
	if(EEPCtrl.EEP2EEPObj)
	{
		if(!TSK_EE_PARA(EE_CMD_INI))
            TSK_EE_PARA(EE_CMD_PWR_UP_GET_DATA);	
		EEPCtrl.EEP2EEPObj=0;
	}

}

static u8 AppTaskCreate (void);

#ifndef MS_To_Systick_CNT

//#include "RTX_Config.h"
extern U32 os_clockrate;
#define		MS_To_Systick_CNT(ms)		((uint16_t)((ms * 1000) / os_clockrate))
//#define		MS_To_Systick_CNT(ms)		((uint16_t)(ms * 1))

#endif



/********************************************************************************************************************************************
------------------------------------------------------os_tsk分界线---------------------------------------------------------------------------
********************************************************************************************************************************************/

/*
*********************************************************************************************************
*	函 数 名: AppTaskCanOpenRx
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
__task void AppTaskCanOpenRx(void)
{
	OS_RESULT xResult;
	const u16 usSemBlockTime = MS_To_Systick_CNT(50);

    while(1)
    {
		xResult = os_sem_wait(&sem_CanOpenRx, usSemBlockTime);
		switch (xResult)
		{
			case OS_R_OK:
			case OS_R_SEM:
			{
				tsk_lock();
				RTXTskStart(&g_DbugRtx.CanopenRx);
				Tsk_CanOpen_RxMsgPro(P_CanOD, p_stCAN);		
				RTXTskEnd(&g_DbugRtx.CanopenRx);
				tsk_unlock();	
			}
			break;
			case OS_R_TMO:
			break;
			default:                     
			break;
		}
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCanOpenTx
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
__task void AppTaskCanOpenTx(void)
{
	OS_RESULT xResult;
	const u16 usSemBlockTime = MS_To_Systick_CNT(50);

    while(1)
    {
		xResult = os_sem_wait(&sem_CanOpenTx, usSemBlockTime);
		switch (xResult)
		{
			case OS_R_OK:
			case OS_R_SEM:
			{
				tsk_lock();
				RTXTskStart(&g_DbugRtx.CanopenTx);
				Tsk_CanOpen_TxMsgPro(P_CanOD, p_stCAN);
				RTXTskEnd(&g_DbugRtx.CanopenTx);
				tsk_unlock();	
			}
			break;
			case OS_R_TMO:
			break;
			default:
			break;
		}
    }
}
/*
*********************************************************************************************************
*	函 数 名: AppTaskModbusSlave
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
__task void AppTaskModbusSlave1(void)
{
	OS_RESULT xResult;
	const u16 usSemBlockTime = MS_To_Systick_CNT(100);
	char i=0;
	CCF_StDef *p2CCF = (CCF_StDef*) & CCFs;
	MODBUS_SLV_INFO MBRes=MODBUS_SLV_NULL;
	
    while(1)
    {
		//等待3.5T事件数据包
		xResult = os_sem_wait(&sem_MosbusSlave1, usSemBlockTime);

		RTXTskStart(&g_DbugRtx.MBSlv);
		switch (xResult)
		{
			case OS_R_OK:
			case OS_R_SEM:
			{
				for (i = 0; i < CCFs.Record.SlvNum; i++)
				{
					if(p2CCF[CCFs.Record.SlvCCFID[i]].RxOk)
					{
						p2CCF[CCFs.Record.SlvCCFID[i]].RxOk=0;
						if(p2CCF[CCFs.Record.SlvCCFID[i]].PubPara.Format==CCFRTU)
						{
							MBRes=task_modbus_rtu_slv(&RS485_USARTx[CCFs.Record.SlvCCFID[i]],
								p2CCF[CCFs.Record.SlvCCFID[i]].InitRecord.p2Slv->SlvPara.p2SlaveData, MODBUS_OT_RW);
						}
						else if(p2CCF[CCFs.Record.SlvCCFID[i]].PubPara.Format==CCFTCP)
						{
							MBRes=task_modbus_tcp_slv(&RS485_USARTx[CCFs.Record.SlvCCFID[i]], 
								p2CCF[CCFs.Record.SlvCCFID[i]].InitRecord.p2Slv->SlvPara.p2SlaveData, MODBUS_OT_RW);
						}	

						if(MBRes==MODBUS_SLV_OK)
						{
							SLV_RECIVE_LED++;
							p2CCF[CCFs.Record.SlvCCFID[i]].InitRecord.p2Slv->CommOkCnt++;
							p2CCF[CCFs.Record.SlvCCFID[i]].InitRecord.p2Slv->TimeOutMs=0;
						}
					}
				}
//					tsk_unlock();	
			}
			break;
			case OS_R_TMO:
			break;
			default:
			break;
		}
		RTXTskEnd(&g_DbugRtx.MBSlv);
    }
}
/*
*********************************************************************************************************
*	函 数 名: AppTaskModbusSlave2
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
__task void AppTaskModbusSlave2(void)
{
	OS_RESULT xResult;
	const u16 usSemBlockTime = MS_To_Systick_CNT(100);
	
    while(1)
    {
		//等待3.5T事件数据包
		xResult = os_sem_wait(&sem_MosbusSlave2, usSemBlockTime);
		switch (xResult)
		{
			case OS_R_OK:
			case OS_R_SEM:
			{
				tsk_lock();
				
				tsk_unlock();	
			}
			break;
			case OS_R_TMO:
			break;
			default:
			break;
		}
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCycleScan
*	功能说明: 100ms间隔时间调用...
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
__task void AppTaskCycleScan(void)
{
	os_itv_set(MS_To_Systick_CNT(100));//// 设置延迟周期 100ms

    while(1)
    {
		bsp_LedToggle(LED1);
		Tsk_ADC(); 
	
		if(g_pBoardpara)
		{
			Tsk_Memory_Manage();
		}

		os_evt_set(TASK_CycleScan_0, HandleTaskStart);
		
		// os_itv_wait是绝对延迟，os_dly_wait是相对延迟。
		os_itv_wait();
			
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskMotionCtronl
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
__task void AppTaskMotionCtronl(void)
{
	os_itv_set(MS_To_Systick_CNT(ACC_TASK_INTERVAL_MS));
//	const u16 usBlockTime = MS_To_Systick_CNT(1);//设置延迟周期 1ms
	
	while(1)
	{
		
		RTXTskStart(&g_DbugRtx.Acc);
		if(g_Para.Debug.AccStop==0)
			Tsk_Acc();
		DevDbugOperation();
		RTXTskEnd(&g_DbugRtx.Acc);

		os_evt_set(TASK_MotionCtronl_1, HandleTaskStart);
		os_itv_wait();
	//	os_dly_wait(usBlockTime);
	}
}

//超时计数毫秒计数
bool TimeOutCntMs(u16 Acc)
{
	char res = 0;
	char i=0;
	CCFEQP_Master_StDef* p2EQPMst = (CCFEQP_Master_StDef*)&EQPMsts;
	CCFEQP_Slave_StDef* p2EQPSlv = (CCFEQP_Slave_StDef*)&EQPSlvs;

	for (i = 0; i < EQPMsts.Number; i++)
	{
		if (p2EQPMst[i].TimeOutMs < TimeOutMsMax)
			p2EQPMst[i].TimeOutMs+=Acc;
	}

	for (i = 0; i < EQPSlvs.Number; i++)
	{
		if (p2EQPSlv[i].TimeOutMs < TimeOutMsMax)
			p2EQPSlv[i].TimeOutMs+=Acc;
	}

	/*抽象传感器数据池 通讯超时计数更新*/
	{
		if(g_BMSData.BMS_Read.TimeOutMs < TimeOutMsMax)
			g_BMSData.BMS_Read.TimeOutMs++;
	}

	//RTX任务喂狗超时计数
	if (g_DbugRtx.WatchDogCnt < 65535)
		g_DbugRtx.WatchDogCnt++;
	
	return res;
}

//用户1ms任务
bool UserMsTsk(u16 Acc)
{
	bool res = 0;
	
	//超时计数毫秒计数
	TimeOutCntMs(Acc);
	
    //故障检出ms计数
    Emg_MsCnt();

	//ACC Ms任务
	ACC_MsTsk();

	return res;
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCycleScan1ms
*	功能说明: LED闪烁。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
__task void AppTaskCycleScan1ms(void)
{
	os_itv_set(MS_To_Systick_CNT(1));//设置延迟周期 1ms

    while(1)
    {
        //有时仿真不运行 要打个断点才行，注释掉
		/*读取DS18B20寄存器
		if(Tsk_Read_ds18b20_MREG(100) == 0x10)
		{//read temp ok!!!
			Tsk_DS18b20_Process(DS18B20_PAR.DS18b20_REG_DAT);
		}*/
		
		Tsk_GpioInputScan();
		Tsk_GpioOutputControl();
		Tsk_Analog();

//		/*SLAM导航下，需要持续对工控机写心跳 否则工控机中间件会主动断开ModBus*/
//		if(g_HostData.AGVNavigateMode == AGVNavigateMode_SLAM)
//		{
//			EQP_IPC.WD[8] = g_Sys_1ms_Counter;
//		}

		tsk_canopen_master(pCm, P_CanOD, p_stCAN);
				
		if (g_Para.RunningMode == ACC_RunningMode_SoftwareSimulat)//纯软件仿真模式
		{
			g_Sys_1ms_Counter+=g_DbugRtx.k2Ms;
			g_HostData.Sys1msCounter=g_Sys_1ms_Counter;
			UserMsTsk(g_DbugRtx.k2Ms);
			User50usTsk(g_DbugRtx.k2Ms*20);
		}	

		os_evt_set(TASK_CycleScan1ms_2, HandleTaskStart);
		os_itv_wait();
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskModbusMaster
*	功能说明: 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
__task void AppTaskModbusMaster(void)
{
		os_itv_set(MS_To_Systick_CNT(1));//// 设置延迟周期 1ms

    while(1)
    {
////	tsk_lock();
		RTXTskStart(&g_DbugRtx.MBMst);
        Tsk_MB(&mb_manger_app);    //本项目没有modbus从机
		RTXTskEnd(&g_DbugRtx.MBMst);
////			tsk_unlock();	
		os_evt_set(TASK_ModbusMaster_3, HandleTaskStart);
		os_itv_wait();
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务，也就是最高优先级任务。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
__task void AppTaskStart(void)
{
	OS_RESULT xResult;
	const u16 usBlockTime = MS_To_Systick_CNT(50);
	HandleTaskStart = os_tsk_self();	// 获取启动任务的句柄 
	// 创建任务 
	if(!AppTaskCreate())
	{
		watchdog_init(300);
		SysTimer(SysTimer_Base, ENABLE);
	}
	else
	{//创建失败
	
	}
		
	while(1)
	{
		// 等待所有任务发来的事件标志 
		xResult = os_evt_wait_and(TASK_BIT_ALL, usBlockTime);
		switch (xResult)
		{
			case OS_R_EVT:
			{
				tsk_lock();
				watchdog_feed();//喂狗!!!
				g_DbugRtx.WatchDogCnt = 0;//喂狗超时计数复位
				tsk_unlock();		
			}
			break;	
			case OS_R_TMO:
			break;
			default:                     
			break;
		}
			
//		os_dly_wait(1000);//50ms
	}
}
/*
*********************************************************************************************************
*	函 数 名: AppTaskBackgroundService
*	功能说明: 后台服务程序，不是很重要的CBB循环调用函数在此执行
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
__task void AppTaskBackgroundService(void)
{
	
	while(1)
	{
		//Tsk_AsynchronousCBB();
		os_evt_set(TASK_BackgroundService_4, HandleTaskStart);
		os_dly_wait(MS_To_Systick_CNT(10));
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建任务的函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static u8 AppTaskCreate (void)
{
	u8 res = 0;
	
	HandleTaskCycleScan = os_tsk_create_user(AppTaskCycleScan,
	                                   APP_TASK_CycleScan_PRIO,
	                                   &AppTaskCycleScanStk,
	                                   sizeof(AppTaskCycleScanStk));
	
	HandleTaskCycleScan1ms = os_tsk_create_user(AppTaskCycleScan1ms,
	                                   APP_TASK_CycleScan1ms_PRIO,
	                                   &AppTaskCycleScan1msStk,
	                                   sizeof(AppTaskCycleScan1msStk));
	
	HandleTaskMotionCtronl = os_tsk_create_user(AppTaskMotionCtronl,
	                                      APP_TASK_MotionCtronl_PRIO,
	                                      &AppTaskMotionCtronlStk,
	                                      sizeof(AppTaskMotionCtronlStk));
//	//不需要嵌在业务流程中的CBB程序调用
//	HandleTaskBackgroundService = os_tsk_create_user(AppTaskBackgroundService,
//													 APP_TASK_BackgroundService_PRIO,
//													 &AppTaskBackgroundServiceStk,
//													 sizeof(AppTaskBackgroundServiceStk));

	
	os_sem_init(&sem_MosbusSlave1, 0);
	HandleTaskModbusSlave1 = os_tsk_create_user(AppTaskModbusSlave1,
																		APP_TASK_ModbusSlave1_PRIO,
																		&AppTaskModbusSlave1Stk,
																		sizeof(AppTaskModbusSlave1Stk));
                                                                     
//    os_sem_init(&sem_MosbusSlave2, 0);
//    HandleTaskModbusSlave2 = os_tsk_create_user(AppTaskModbusSlave2,
//                                                                        APP_TASK_ModbusSlave2_PRIO,
//                                                                        &AppTaskModbusSlave2Stk,
//                                                                        sizeof(AppTaskModbusSlave2Stk));     
                                                                        
	os_sem_init(&sem_CanOpenTx, 0);
	HandleTaskCanOpenTx = os_tsk_create_user(AppTaskCanOpenTx,
																		APP_TASK_CanOpenTx_PRIO,
																		&AppTaskCanOpenTxStk,
																		sizeof(AppTaskCanOpenTxStk));

	os_sem_init(&sem_CanOpenRx, 0);
	HandleTaskCanOpenRx = os_tsk_create_user(AppTaskCanOpenRx,
																		APP_TASK_CanOpenRx_PRIO,
																		&AppTaskCanOpenRxStk,
																		sizeof(AppTaskCanOpenRxStk));

	HandleTaskModbusMaster = os_tsk_create_user(AppTaskModbusMaster,
																		APP_TASK_ModbusMaster_PRIO,
																		&AppTaskModbusMasterStk,
																		sizeof(AppTaskModbusMasterStk));
	return res;
}



//注意:主机的发送间隔时间<=10ms,否则,会导致,其他任务获得运行权减少
void modbus35t(void)
{
	unsigned char i=0;
	u8 res=0;
	CCF_StDef *p2CCF = (CCF_StDef*) & CCFs;
	char SlaveRxOkCnt=0;
	
	for(i=0;i< CCF_MAXNUM;i++)
	{
		res=Rs485_usart_tm_50us(&RS485_USARTx[i]);
		if(res == B_EV_USART_RX && p2CCF[i].PubPara.Role==CCFMBSlave)
		{
			p2CCF[i].RxOk=1;
			SlaveRxOkCnt++;
		}
	}

	if(SlaveRxOkCnt && g_DbugRtx.MBSlv.Busy==0)
	{
		isr_sem_send(&sem_MosbusSlave1);
		g_DbugRtx.MBSlv.Busy = 1;//进入忙碌状态
	}
	else {}
}

//系统定时器中断程序
void sys_timer_irq(void)
{
	if ( TIM_GetITStatus( SysTimer_Base, TIM_IT_Update ) == SET )
	{//50us
		TIM_ClearITPendingBit(SysTimer_Base, TIM_IT_Update);//清除中断标志
		modbus35t();
		User50usTsk(1);
		
		if(++g_Sys_50us_Counter >= 20)
		{
			UserMsTsk(1);
			g_Sys_50us_Counter=0;
			g_Sys_1ms_Counter++;//系统1ms计数
			//g_HostData.Sys1msCounter++;//自动代码时钟输入
			g_HostData.Sys1msCounter = g_Sys_1ms_Counter;

			if(p_stCAN->CanStatusFlag == CAN_NoneError)
			{
				// can_tx
				if (p_stCAN->tx_fifo.read_adr != p_stCAN->tx_fifo.write_adr && g_DbugRtx.CanopenTx.Busy==0)
				{
					isr_sem_send(&sem_CanOpenTx);
					g_DbugRtx.CanopenTx.Busy = 1;//进入忙碌状态
				}
				else {}	

				// can_rx 
				if (p_stCAN->rx_fifo.read_adr != p_stCAN->rx_fifo.write_adr && g_DbugRtx.CanopenRx.Busy==0)
				{
					isr_sem_send(&sem_CanOpenRx);
					g_DbugRtx.CanopenRx.Busy = 1;//进入忙碌状态
				}
				else {}					
			}							
		}		
	}
}

//***********************************END OF FILE****************************************
