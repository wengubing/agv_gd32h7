
#ifndef  __APPTSK_H__
#define  __APPTSK_H__


#ifdef  APPTSK_GLOBALS
 #define APPTSK_EXT
#else
 #define APPTSK_EXT  extern
#endif



 
// Private typedef -----------------------------------------------------------

// Private define ------------------------------------------------------------

// Private macro -------------------------------------------------------------

// Private variables ---------------------------------------------------------


//任务优先级定义
#define APP_TASK_START_PRIO						4
#define	APP_TASK_CanOpenTx_PRIO					14
#define	APP_TASK_CanOpenRx_PRIO					15	//优先级需要比Tx优先级高
#define	APP_TASK_ModbusSlave1_PRIO				8
#define	APP_TASK_ModbusSlave2_PRIO				9 
#define	APP_TASK_ModbusMaster_PRIO				13
#define	APP_TASK_MotionCtronl_PRIO				11
#define	APP_TASK_CycleScan_PRIO					2
#define	APP_TASK_CycleScan1ms_PRIO				5
#define	APP_TASK_BackgroundService_PRIO			12	//优先级比运动解算任务略高，较为合理





APPTSK_EXT __task void AppTaskStart(void);//start
APPTSK_EXT OS_TID HandleTaskStart; 
APPTSK_EXT uint64_t AppTaskStartStk[1024/8];   // 启动任务栈大小定义 

APPTSK_EXT __task void AppTaskMotionCtronl(void);//MotionCtronl
APPTSK_EXT OS_TID HandleTaskMotionCtronl;
APPTSK_EXT uint64_t AppTaskMotionCtronlStk[2048/8];

APPTSK_EXT __task void AppTaskCanOpenTx(void);//canopen_tx
APPTSK_EXT OS_TID HandleTaskCanOpenTx;
APPTSK_EXT uint64_t AppTaskCanOpenTxStk[1024/8];
APPTSK_EXT OS_SEM sem_CanOpenTx;

APPTSK_EXT __task void AppTaskCanOpenRx(void);//canopen_rx
APPTSK_EXT OS_TID HandleTaskCanOpenRx;
APPTSK_EXT uint64_t AppTaskCanOpenRxStk[1024/8];
APPTSK_EXT OS_SEM sem_CanOpenRx;

APPTSK_EXT __task void AppTaskModbusSlave1(void);//modbusslave1
APPTSK_EXT OS_TID HandleTaskModbusSlave1;
APPTSK_EXT uint64_t AppTaskModbusSlave1Stk[1024/8];
APPTSK_EXT OS_SEM sem_MosbusSlave1;

APPTSK_EXT __task void AppTaskModbusSlave2(void);//modbusslave
APPTSK_EXT OS_TID HandleTaskModbusSlave2;
APPTSK_EXT uint64_t AppTaskModbusSlave2Stk[1024/8];
APPTSK_EXT OS_SEM sem_MosbusSlave2;

APPTSK_EXT __task void AppTaskModbusMaster(void);//modbusslave
APPTSK_EXT OS_TID HandleTaskModbusMaster;
APPTSK_EXT uint64_t AppTaskModbusMasterStk[1024/8];

APPTSK_EXT __task void AppTaskCycleScan(void);
APPTSK_EXT OS_TID HandleTaskCycleScan;
APPTSK_EXT uint64_t AppTaskCycleScanStk[1024/8];

APPTSK_EXT __task void AppTaskCycleScan1ms(void);
APPTSK_EXT OS_TID HandleTaskCycleScan1ms;
APPTSK_EXT uint64_t AppTaskCycleScan1msStk[1024/8];


APPTSK_EXT __task void AppTaskBackgroundService(void);	//不重要的CBB循环调用函数
APPTSK_EXT OS_TID HandleTaskBackgroundService;
APPTSK_EXT uint64_t AppTaskBackgroundServiceStk[2048/8];


#define TASK_CycleScan_0			(1 << 0)
#define TASK_MotionCtronl_1			(1 << 1)
#define TASK_CycleScan1ms_2	 		(1 << 2)
#define TASK_ModbusMaster_3		 	(1 << 3)
#define TASK_BackgroundService_4	(1 << 4)

#define TASK_BIT_ALL (TASK_CycleScan_0 | TASK_MotionCtronl_1 | TASK_CycleScan1ms_2 | TASK_ModbusMaster_3)


APPTSK_EXT  u32 g_Master4_Scomm_Delay_cnt,g_Master4_Delay_Timers;

// Exported functions ------------------------------------------------------- 




#endif
