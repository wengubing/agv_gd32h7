/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: LinkCanopenMaster.h
** 创 建 人: 文小兵
** 描    述: CANopen主站任务
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0
** 创建日期: 2024年5月9日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef LinkCanopenMaster_h
#define LinkCanopenMaster_h

//保证以下所有内容只被同一源文件引用一次
#ifdef  LinkCanopenMaster_GLOBALS
	#define LinkCanopenMaster_EXT
#else
	#define LinkCanopenMaster_EXT  extern
#endif

//自定义引用		
#include "ObjectDictionary.h"
#include "drv_can.h"
#include "drv_ee_para.h"

#define SVO_Type  5		//0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器              5：Copley驱动器
	
//步科驱动器工作模式
#define  DRIVER_MODE_Pulse                  -4        //Pulse Mode --- 脉冲模式
#define  DRIVER_MODE_ImmediateVelocity      -3        //Immediate Velocity Mode --- 立即速度模式 
#define  DRIVER_MODE_POSATION                1        //Profile Position Mode --- 位置控制模式
#define  DRIVER_MODE_VELOCITY                3        //Profile Velocity Mode --- 速度控制模式
#define  DRIVER_MODE_Torque                  4        //Profile Torque Mode --- 扭矩控制模式
#define  DRIVER_MODE_Homing                  6        //Homing Mode --- 归零模式
#define  DRIVER_MODE_IntPosition             7        //Interpolation Position Mode --- 插补位置模式
#define  DRIVER_MODE_CSP                     8        //Cyclic Synchronous Position Mode --- ECAN同步位置模式
#define  DRIVER_MODE_CSV                     9        //Cyclic Synchronous Velocity Mode --- ECAN同步速度模式
#define  DRIVER_MODE_CST                     10       //Cyclic Synchronous Torque Mode --- ECAN同步力矩模式

//Copley驱动器工作模式	
//0:programmed velocity mode
//1:Profile Velocity
//2:Current Loop
#define			CopleyWorkMode			1

#define		eprintf(...)

#define		CanFestivalTimer_AHPBX_f							42//Mhz
#define		CanFestivalTimer_Base								TIM12
#define		CanFestivalTimer_CLK								RCC_APB1Periph_TIM12
#define		CanFestivalTimer_IT									TIM_IT_CC1
#define		CanFestivalTimer_FLAG								TIM_FLAG_CC1
#define 	CanFestivalTimer_IRQn               				TIM8_BRK_TIM12_IRQn 
#define		CanFestivalTimer_IRQHandler							TIM8_BRK_TIM12_IRQHandler
#define		CanFestivalTimer_Set								CanFestivalTimer_Base,CanFestivalTimer_CLK,CanFestivalTimer_IRQn

#define		CanFestival_led										b_led3

#define		USE_TPDO_CH_NUM										4//最大值为4

#if (SVO_Type == 4)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器	        5：Copley驱动器	
  #define		USE_RPDO_CH_NUM										2//最大值为4
#else
  #define		USE_RPDO_CH_NUM										1//最大值为4
#endif

//CANopen主站步骤定义
typedef enum
{
	Step_ParaIni 			= 0,
	Step_DrivePowerDown		= 1,	
	Step_DrivePowerUp		= 2,
	Step_Reset_Node			= 3,
	Step_Reset_Comunication	= 4,
	Step_WaitBootUpOk		= 5,
	Step_Active				= 6,
	Step_CanOpenFault		= 7,
}CanOpenMasterStep_Type;

//CANopen主站步骤定义
typedef enum
{
	Can_SVOType_Null 		= 0,	//无效
	Can_SVOType_Kinco		= 1,	//步科
	Can_SVOType_Delta		= 2,	//台达
	Can_SVOType_Windcon		= 3,	//风得控
	Can_SVOType_Wheatstone	= 4,	//惠斯通
	Can_SVOType_Keya		= 5,	//科亚
    Can_SVOType_Copley      = 6,	//Copley
}Can_SVOType_EnumDef;
	
/*任务信息 数据对象定义*/
typedef struct
{
	u8 					CheckQuantity	;	//检测数量
	CAN_BAUD_TYPE 		Baud			;	//波特率
	Can_SVOType_EnumDef SVOType			;	//伺服类型
	//伺服工作模式
}ACC_ParaCanOpen_StDef;

//CANOPEN主站启动故障 位数据对象定义
typedef struct
{
	unsigned int CanBusErr 	: 1;	//CAN总线异常 启动时
	unsigned int CanNoSlv 	: 1;	//没有can从站上线
	unsigned int CanLessSlv : 1;	//can从站数量不足
}ACC_CanOpenErr_BitDef;

//CANopen主站对象定义
typedef struct
{
	ACC_ParaCanOpen_StDef 	Para;								//CANopen参数

	//外部输入
	bool 					in_CanOpenStart;					//CANopen启动

	CanOpenMasterStep_Type 	CanopenStep;						//CANopen主站步骤
	unsigned short 			RepeatConnectCnt;					//重连次数
	unsigned short 			SlaveOnLineNum;						//在线数量
	unsigned short 			TxCnt;								//发送计数
	enum enum_nodeState 	MasterStatu;						//主站状态
	u8 						ConfigStep[MAX_NodeId_NUM];			//配置步骤
	u8 						SlaveParaConfigId[MAX_NodeId_NUM];	//从站参数配置ID
	u8  					HeartMsgId[MAX_NodeId_NUM];			//心跳消息ID
	unsigned char 			PostrigerCnt[MAX_NodeId_NUM];		//
	s32 					HeartJfcCnt;						//心跳计数

	ACC_CanOpenErr_BitDef 	Err;								//故障代码
}LinkCanOpen;

//全局对象声明
LinkCanopenMaster_EXT LinkCanOpen	 CanMaster;//CANopen主站对象

//全局函数声明
LinkCanopenMaster_EXT obj_link_sdo Obj_Clentx_SDO[MAX_NodeId_NUM];
LinkCanopenMaster_EXT obj_link_pdo Obj_Clentx_TPDO[USE_TPDO_CH_NUM][MAX_NodeId_NUM], Obj_Clentx_RPDO[USE_RPDO_CH_NUM][MAX_NodeId_NUM];

LinkCanopenMaster_EXT	void CanObjectDict_heartbeatError(CO_Data* d, UNS8);
LinkCanopenMaster_EXT	UNS8 CanObjectDict_canSend(Message *);
LinkCanopenMaster_EXT	void CanObjectDict_initialisation(CO_Data* d);
LinkCanopenMaster_EXT	void CanObjectDict_preOperational(CO_Data* d);
LinkCanopenMaster_EXT	void CanObjectDict_operational(CO_Data* d);
LinkCanopenMaster_EXT	void CanObjectDict_stopped(CO_Data* d);
LinkCanopenMaster_EXT	void CanObjectDict_post_sync(CO_Data* d);
LinkCanopenMaster_EXT	void CanObjectDict_post_TPDO(CO_Data* d);
LinkCanopenMaster_EXT	void CanObjectDict_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);
LinkCanopenMaster_EXT	void CanObjectDict_post_SlaveBootup(CO_Data* d, UNS8 nodeid);

LinkCanopenMaster_EXT	void tsk_canopen_master(LinkCanOpen *pCm, CO_Data* d, STM32_CANi *p_stCAN);
LinkCanopenMaster_EXT	void CanFestivalTimer_Dispatch(void);
LinkCanopenMaster_EXT	void CanFestivalTimer_Ctrl(FunctionalState NewState);

LinkCanopenMaster_EXT	void canfestival_timer_start(u8 pi,  FunctionalState NewState);
LinkCanopenMaster_EXT	void can_master_init(STM32_CANi *p_stCAN, CO_Data* d, FunctionalState NewState);
LinkCanopenMaster_EXT	UNS8 canSend(CAN_PORT notused, Message *m);

LinkCanopenMaster_EXT	void Tsk_CanOpen_RxMsgPro(CO_Data* d, STM32_CANi *PCAN);
LinkCanopenMaster_EXT	void Tsk_CanOpen_TxMsgPro(CO_Data* d, STM32_CANi *PCAN);

//LinkCanopenMaster_EXT	 void SlaveHeartMessageProcess(CO_Data* d, e_nodeState NodeState, u8 NodeId);


#endif

//***********************************END OF FILE***********************************
