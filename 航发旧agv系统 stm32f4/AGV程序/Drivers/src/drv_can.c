
/**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Drv_485Usart.c
** 创 建 人: dengyang
** 描    述: 485硬件接口驱动
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: 	V1.0 
** 创建日期: 	2015.03.18
** 描    述:  硬件描述层
**--------------------------------------------------------------------------------------   
**/

#define		CAN_GLOBALS

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "..\..\Drivers\inc\drv_can.h"


/* Private typedef -----------------------------------------------------------*/
// 波特率位定时参数类型
typedef struct
{
    u8 	SJW;
    u8 	BS1;
    u8 	BS2;
    u32 PR;
}CAN_BAUD_PAR;


/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/*--------------------------------------------------------------
			第1项：SJW，同步跳转宽度，取值范围:0～3
			第2项：TS1，传输段及相位缓冲段1的和，取值范围：0～15
			第3项：TS2，相位缓冲段2，取值范围：0～7
			第4项：Prescaler，分频值，取值范围1～1024
			fAPB1 = 40MHz   -----stm32f407igt6
			SJW:值固定为1tq；
			BS1:(TS1 + 1)tq;
			BS2:(TS2 + 1)tq;
			Prescaler:(PR + 1)
											        fAPB1                                     fAPB1                             fAPB1
			BaudRate = ----------------------------------------- = ------------------------------ = ------------------------------
							  	(1 + (TS1 + 1) + (TS2 + 1)) * (PR + 1)       (3 + TS1 + TS2) * Prescaler  	(1 + BS1 + BS2) * (PR + 1)  
			*************************************************************************************
			尽可能把采样点设为CIA推荐的值：
															|				1 + BS1
			Baudrate								|  --------------------
															|		  1 + BS1 + BS2
			当Baudrate:>800K,				|        75%
			当Baudrate:>500K,				|        80%
			当Baudrate:<=500K,			|       87.5%
			*************************************************************************************
-------------------------------------------------------------------*/
const CAN_BAUD_PAR can_bit_set[] =
{
		//SJW、BS1、BS2、PR
    {0, 0,  0, 0},      // CAN_BAUD_invalid
    {0, 11, 1, 480},    // CAN_BAUD_5k
    {0, 11, 1, 240},    // CAN_BAUD_10k
    {0, 11, 1, 120},    // CAN_BAUD_20k
    {0, 11, 1, 60},     // CAN_BAUD_40k
    {0, 11, 1, 48},     // CAN_BAUD_50k
    {0, 14, 1, 25},     // CAN_BAUD_80k
    {0, 12, 1, 25},     // CAN_BAUD_100k
    {0, 12, 1, 18},     // CAN_BAUD_125K
    {0, 5,  0, 23},     // CAN_BAUD_200k,87.5%
    {0, 12, 1, 9},      // CAN_BAUD_250K
    {0, 6,  0, 10},     // CAN_BAUD_400K
    {0, 12, 1, 5},      // CAN_BAUD_500K, 87.5%
    {0, 6,  1, 5},      // CAN_BAUD_800K
    {0, 4,  1, 5},      // CAN_BAUD_1M
}; 


//    // Fapb1 = 40MHz
//    { (1 - 1), (13 - 1), (2 - 1), (500) },    						// CAN_BAUDRATE_5k, 87.5%
//    { (1 - 1), (13 - 1), (2 - 1), (250) },    						// CAN_BAUDRATE_10k, 87.5%
//    { (1 - 1), (13 - 1), (2 - 1), (125) },    						// CAN_BAUDRATE_20k, 87.5%
//    { (1 - 1), (6 - 1),  (1 - 1), (125) },    						// CAN_BAUDRATE_40k, 87.5%
//    { (1 - 1), (13 - 1), (2 - 1), (50) },    	 					  // CAN_BAUDRATE_50k, 87.5%
//    { (1 - 1), (8 - 1),  (1 - 1), (50) },     						// CAN_BAUDRATE_80k, 90%
//    { (1 - 1), (13 - 1), (2 - 1), (25) },     						// CAN_BAUDRATE_100k, 87.5%
//    { (1 - 1), (13 - 1), (2 - 1), (20) },     						// CAN_BAUDRATE_125K, 87.5%
//    { (1 - 1), (6 - 1),  (1 - 1), (25) },     						// CAN_BAUDRATE_200k, 87.5%
//    { (1 - 1), (13 - 1), (2 - 1), (10) },     						// CAN_BAUDRATE_250K, 87.5%
//    { (1 - 1), (8 - 1),  (1 - 1), (10) },     						// CAN_BAUDRATE_400K, 90%
//    { (1 - 1), (13 - 1), (2 - 1), (5) },      						// CAN_BAUDRATE_500K, 87.5%
//    { (1 - 1), (7 - 1),  (2 - 1), (5) },      						// CAN_BAUDRATE_800K, 80%
//    { (1 - 1), (5 - 1),  (2 - 1), (5) },      						// CAN_BAUDRATE_1M, 75%

//CAN_TypeDef *CAN_BASE[CANn]			= { SCAN1_BASE, 	SCAN2_BASE};
//GPIO_TypeDef *CAN_GPIO_PORT[CANn] 	= { SCAN1_GPIO_PORT,SCAN2_GPIO_PORT};
//const u32    CAN_CLK[CANn]			= { SCAN1_CLK, 		SCAN2_CLK};
//const u16    CAN_RX_PIN[CANn] 		= { SCAN1_RX_PIN,	SCAN2_RX_PIN};
//const u16    CAN_TX_PIN[CANn] 		= { SCAN1_TX_PIN,	SCAN2_TX_PIN};
//const u32    CAN_GPIO_CLK[CANn] 	= { SCAN1_GPIO_CLK,	SCAN2_GPIO_CLK};
//const u8     CAN_AF_PORT[CANn] 		= { SCAN1_AF_PORT,	SCAN2_AF_PORT};
//const u8     CAN_RX_SOURCE[CANn] 	= { SCAN1_RX_SOURCE,SCAN2_RX_SOURCE};
//const u8     CAN_TX_SOURCE[CANn] 	= { SCAN1_TX_SOURCE,SCAN2_TX_SOURCE};
//const u8     CAN_RX0_IRQn[CANn]		= { SCAN1_RX0_IRQn, SCAN2_RX0_IRQn};
//const u8     CAN_RX1_IRQn[CANn]		= { SCAN1_RX1_IRQn, SCAN2_RX1_IRQn};
//const u8     CANx_SCE_IRQn[CANn]	= { CAN1_SCE_IRQn, 	CAN2_SCE_IRQn};

//2025年3月12日 by master 仅需一路CAN，来减少对系统RW Data的消耗，确保上层应用正确使用
CAN_TypeDef *CAN_BASE[CANn]			= { SCAN1_BASE		};
GPIO_TypeDef *CAN_GPIO_PORT[CANn] 	= { SCAN1_GPIO_PORT	};
const u32    CAN_CLK[CANn]			= { SCAN1_CLK		};
const u16    CAN_RX_PIN[CANn] 		= { SCAN1_RX_PIN	};
const u16    CAN_TX_PIN[CANn] 		= { SCAN1_TX_PIN	};
const u32    CAN_GPIO_CLK[CANn] 	= { SCAN1_GPIO_CLK	};
const u8     CAN_AF_PORT[CANn] 		= { SCAN1_AF_PORT	};
const u8     CAN_RX_SOURCE[CANn] 	= { SCAN1_RX_SOURCE	};
const u8     CAN_TX_SOURCE[CANn] 	= { SCAN1_TX_SOURCE	};
const u8     CAN_RX0_IRQn[CANn]		= { SCAN1_RX0_IRQn	};
const u8     CAN_RX1_IRQn[CANn]		= { SCAN1_RX1_IRQn	};
const u8     CANx_SCE_IRQn[CANn]	= { CAN1_SCE_IRQn	};
//2016.03.18添加 by dengyang
void CanSceITConfig(CAN_ID CANx, u8 sel)
{
	if(!sel)
	{//关闭中断
		CAN_ITConfig(CAN_BASE[CANx], CAN_IT_BOF | CAN_IT_ERR | CAN_IT_EWG | CAN_IT_EPV | CAN_IT_LEC, DISABLE); 
	}
	else
	{//打开中断
		CAN_ITConfig(CAN_BASE[CANx], CAN_IT_BOF | CAN_IT_ERR | CAN_IT_EWG | CAN_IT_EPV | CAN_IT_LEC, ENABLE); 
	}
}

//=============================================================================
// 名称：Set_CAN_Int
// 功能：CAN参数配置
// 参数：CANx,
// 参数：bit_rate,
// 参数：nb_filter,
// 参数：can_gpio_config_bit,
// 参数：can_pi
// 返回：None
// 说明：None
//=============================================================================
u8 Set_CAN_Int(CAN_ID CANx, CAN_BAUD_TYPE can_baud, u32 addr, u8 can_pi)
{
  GPIO_InitTypeDef				GPIO_IS_CAN;
	NVIC_InitTypeDef				NVIC_IS_CAN;
	CAN_InitTypeDef					CAN_IS;
	CAN_FilterInitTypeDef		CAN_FilterIS;
	STM32_CANi							*pCAN=STM32_CAN;
	u8 i = 0;
	
	if(can_baud > CAN_BAUDRATE_1M)
	{
		can_baud = CAN_BAUDRATE_500K;//比特率参数超范围,指定默认值:500K
	}
	
	pCAN[CANx].ini.Baud=can_baud;
	pCAN[CANx].ini.CANx=CANx;

	RCC_APB1PeriphClockCmd( CAN_CLK[CANx], ENABLE);	//使能CAN外设时钟	
	
	RCC_AHB1PeriphClockCmd( CAN_GPIO_CLK[CANx], ENABLE);// 使能端口时钟	
	GPIO_PinAFConfig( CAN_GPIO_PORT[CANx], CAN_RX_SOURCE[CANx], CAN_AF_PORT[CANx] );	// 连接引脚到复用功能
	GPIO_PinAFConfig( CAN_GPIO_PORT[CANx], CAN_TX_SOURCE[CANx], CAN_AF_PORT[CANx] );		

	GPIO_IS_CAN.GPIO_Pin = CAN_RX_PIN[CANx] | CAN_TX_PIN[CANx];// 配置收发引脚
	GPIO_IS_CAN.GPIO_Mode = GPIO_Mode_AF;
	GPIO_IS_CAN.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_IS_CAN.GPIO_OType = GPIO_OType_PP;
	GPIO_IS_CAN.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init( CAN_GPIO_PORT[CANx], &GPIO_IS_CAN );

	/* CANx寄存器恢复默认 */
  CAN_DeInit(CAN_BASE[CANx]);
  
  /* CAN外设寄存器设置 */
//  CAN_StructInit(&CAN_IS);
	//CAN单元设置
  CAN_IS.CAN_TTCM = DISABLE;// 关闭时间触发通讯
  CAN_IS.CAN_ABOM = DISABLE; // 使能自动离线管理(在进入总线关闭状态后自动启动恢复序列)
  CAN_IS.CAN_AWUM = DISABLE;// 关闭自动唤醒模式
  CAN_IS.CAN_NART = DISABLE;// 关闭禁止报文自动重传
  CAN_IS.CAN_RFLM = DISABLE;// 关闭接收FIFO锁定(后面的报文覆盖前面的报文,新收到的报文不会被丢弃)
  CAN_IS.CAN_TXFP = DISABLE;// 关闭发送FIFO优先级(由报文的标示符决定)  
  CAN_IS.CAN_Mode = CAN_Mode_Normal;// CAN的工作模式:正常模式
  CAN_IS.CAN_SJW = can_bit_set[can_baud].SJW;// 重新同步跳跃宽度(SJW)，即在每位中可以延长或缩短多少个时间单位的上限:1tq
  CAN_IS.CAN_BS1 = can_bit_set[can_baud].BS1;// 时间段1,采样点:BS1
  CAN_IS.CAN_BS2 = can_bit_set[can_baud].BS2;// 时间段2,:发送点:BS2
  CAN_IS.CAN_Prescaler = can_bit_set[can_baud].PR;// 时间单位长度:PR
	while((CAN_Init(CAN_BASE[CANx], &CAN_IS) == CAN_InitStatus_Failed) && (i != 0xff))
	{//将初始化值写入、请求CAN退出睡眠模式->请求CAN初始化化->让硬件进入正常工作模式
		if((i ++) >= 0xFF)
		{
			return CAN_InitStatus_Failed;		
		}
	}//等待初始化成功!

  /* CAN验收过滤器寄存器设置(必须在CAN初始化模式中进入正常模式前配置,其实不需要再初始化模式中进行配置,必须在它处于非激活状态) */
	
	//滤波器组0,接收本机地址
  CAN_FilterIS.CAN_FilterNumber = nb_filter0; // 选择过滤器组
  CAN_FilterIS.CAN_FilterMode = CAN_FilterMode_IdMask;// 过滤器组"nb_filter"的2个32位寄存器,工作在"标识符屏蔽位模式"
  CAN_FilterIS.CAN_FilterScale = CAN_FilterScale_32bit;// 设置过滤器位宽为单个32位宽	
	if(addr)
	{
		/*设定Filter-ID寄存器-FiR1设值,
		这里ID和下面的MSK共同配合完成总线上数据帧的过滤*/	
		CAN_FilterIS.CAN_FilterIdHigh		 	= ((addr << 3) & 0xFFFF0000) >> 16;//高段
		CAN_FilterIS.CAN_FilterIdLow			= ((addr << 3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xFFFF;//低段
		/*设定Filter-屏蔽码寄存器-FiR2设值,
		备注:此寄存器做屏蔽用,此MSK的每一位决定进来的"数据帧"是否要和上面设置的ID进行位匹配(1:必须匹配,0:无须关心),
		在此处,接收数据帧的ID的addr必须和设置的数据帧ID包含的addr一致才进行匹配*/
		CAN_FilterIS.CAN_FilterMaskIdHigh = (((u32)0xFF << 3) & 0xFFFF0000) >> 16;
		CAN_FilterIS.CAN_FilterMaskIdLow	= (((u32)0xFF << 3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xFFFF;	
	}
	else
	{
		CAN_FilterIS.CAN_FilterIdHigh 		= 0;
		CAN_FilterIS.CAN_FilterIdLow			= 0;
		CAN_FilterIS.CAN_FilterMaskIdHigh = 0;
		CAN_FilterIS.CAN_FilterMaskIdLow	= 0;	
	}
  CAN_FilterIS.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//设定此过滤器组关联到接收FIFO0
  CAN_FilterIS.CAN_FilterActivation = ENABLE;//激活nb_filter过滤器组
  CAN_FilterInit(&CAN_FilterIS);//设置过滤器

	// 滤波器组1,接收广播地址
	CAN_FilterIS.CAN_FilterNumber = nb_filter1;
	CAN_FilterIS.CAN_FilterIdHigh = (((u32)DeviceBroadcastAddress << 3) & 0xFFFF0000) >> 16;
	CAN_FilterIS.CAN_FilterIdLow = (((u32)DeviceBroadcastAddress << 3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xFFFF;
	CAN_FilterIS.CAN_FilterMaskIdHigh = (((u32)DeviceBroadcastAddress << 3) & 0xFFFF0000) >> 16;//0xFFFF;
	CAN_FilterIS.CAN_FilterMaskIdLow = (((u32)DeviceBroadcastAddress << 3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xFFFF;//0xFFFF;
	CAN_FilterIS.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1;//CAN_Filter_FIFO0
	CAN_FilterInit(&CAN_FilterIS);

	/* CAN接收终端NVIC配置 */
	if(can_pi)
	{
		NVIC_IS_CAN.NVIC_IRQChannel = CAN_RX0_IRQn[CANx];
		NVIC_IS_CAN.NVIC_IRQChannelPreemptionPriority = can_pi;
		NVIC_IS_CAN.NVIC_IRQChannelSubPriority = 0;
		NVIC_IS_CAN.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_IS_CAN);	
		
		//使能SCE Interrupt...by dengyang 2016.03.22
		NVIC_IS_CAN.NVIC_IRQChannel = CANx_SCE_IRQn[CANx];
		NVIC_Init(&NVIC_IS_CAN);
	}

	CAN_ClearFlag(CAN_BASE[CANx], CAN_IT_FMP0);

	CAN_ITConfig(CAN_BASE[CANx], CAN_IT_FMP0, ENABLE);// 使能FIFO0消息挂号(fifo存入一个报文)中断,接收本机地址和广播地址
	CAN_ITConfig(CAN_BASE[CANx], CAN_IT_FF0, ENABLE);// 使能FIFO0收满中断,接收本机地址和广播地址
	CAN_ITConfig(CAN_BASE[CANx], CAN_IT_FOV0, ENABLE);// 使能FIFO0溢出中断,接收本机地址和广播地址

	CanSceITConfig(CANx, 1);
	
	return CAN_InitStatus_Success;
}

//============================================================================
// 名称：SYS_CAN1_RX_IRQHandler
// 功能：系统CAN1中断接收函数
// 参数：无
// 返回：无
// 说明：无
//============================================================================
void SYS_CAN1_RX_IRQHandler(void)
{
//	CanRxMsg DD;
	if(CAN_GetITStatus(CAN_BASE[useCAN1], CAN_IT_FMP0) == SET)
	{//FIFO0有收到新报文,
    while(CAN_MessagePending(CAN_BASE[useCAN1], CAN_FIFO0))// 读出FIFO中所有的数据,直达FIFO0变为空
    {
			CAN_Receive(CAN_BASE[useCAN1], CAN_FIFO0, &STM32_CAN[useCAN1].rx_fifo.can_rx_msg[STM32_CAN[useCAN1].rx_fifo.write_adr]);
			//读出邮箱中的有效报文(有fifo释放邮箱处理,以便存放下一个有效报文)
			if ((++ STM32_CAN[useCAN1].rx_fifo.write_adr) >= CAN_RX_BUF_LENGTH_MAX)
			{
					STM32_CAN[useCAN1].rx_fifo.write_adr = 0;
			}
    }
		CAN_FIFORelease(CAN_BASE[useCAN1], CAN_FIFO0);		
	}
	if(CAN_GetITStatus(CAN_BASE[useCAN1], CAN_IT_FF0) == SET)
	{//FIFO0收满
		CAN_ClearITPendingBit(CAN_BASE[useCAN1], CAN_IT_FF0);
	}
	if(CAN_GetITStatus(CAN_BASE[useCAN1], CAN_IT_FOV0) == SET)
	{//FIFO0溢出
		CAN_ClearITPendingBit(CAN_BASE[useCAN1], CAN_IT_FOV0);
	}
}

//============================================================================
// 名称：SYS_CAN1_SCE_IRQHandler
// 功能：系统SCE中断接收函数
// 参数：无
// 返回：无
// 说明：无
//============================================================================
void SYS_CAN1_SCE_IRQHandler(void)
{
	if(CAN_GetITStatus(CAN_BASE[useCAN1], CAN_IT_BOF) == SET)
	{//Bus-off Interrupt
		CAN_ClearITPendingBit(CAN_BASE[useCAN1], CAN_IT_BOF);
	}
	if(CAN_GetITStatus(CAN_BASE[useCAN1], CAN_IT_ERR) == SET)
	{//Error Interrupt
		CAN_ClearITPendingBit(CAN_BASE[useCAN1], CAN_IT_ERR);
	}
	if(CAN_GetITStatus(CAN_BASE[useCAN1], CAN_IT_EWG) == SET)
	{//Error warning Interrupt
		CAN_ClearITPendingBit(CAN_BASE[useCAN1], CAN_IT_EWG);
	}
	if(CAN_GetITStatus(CAN_BASE[useCAN1], CAN_IT_EPV) == SET)
	{//Error passive Interrupt
		CAN_ClearITPendingBit(CAN_BASE[useCAN1], CAN_IT_EPV);
	}
	if(CAN_GetITStatus(CAN_BASE[useCAN1], CAN_IT_LEC) == SET)
	{//Last error code Interrupt
		CAN_ClearITPendingBit(CAN_BASE[useCAN1], CAN_IT_LEC);
	}
	
	STM32_CAN[useCAN1].RxErrorCounter=CAN_GetReceiveErrorCounter(CAN_BASE[useCAN1]);
	STM32_CAN[useCAN1].TxErrorCounter=CAN_GetLSBTransmitErrorCounter(CAN_BASE[useCAN1]);
	STM32_CAN[useCAN1].LastErrorCode=CAN_GetLastErrorCode(CAN_BASE[useCAN1]);
	
	if(STM32_CAN[useCAN1].TxErrorCounter >= 64 && STM32_CAN[useCAN1].CanStatusFlag == CAN_NoneError)
	{
		CanSceITConfig(useCAN1, 0);//关闭SCE中断
		STM32_CAN[useCAN1].CanStatusFlag=CAN_TransmitError;//置tx错误标志
	}
	
}

//============================================================================
// 名称：can_send_msg
// 功能：通过CAN口发送报文
// 参数：Can：CAN口
//      TxMessage：要发送的报文
// 返回：无
// 说明：发送邮箱
//============================================================================
u8 can_send_msg(CAN_ID Canx, CanTxMsg TxMessage)
{
	u8 res=0,TransmitMailbox_Num=CANTXFAILED;
	u16 timeout=0;

	if(STM32_CAN[Canx].CanStatusFlag == CAN_NoneError)
	{
		TransmitMailbox_Num = CAN_Transmit(CAN_BASE[Canx], &TxMessage);
		do
		{
			if(++timeout >= 0xFFFE)
			{
				res=1;
				break;
			}
		}while(CAN_TransmitStatus(CAN_BASE[Canx], TransmitMailbox_Num) != CANTXOK);//等待1包消息发送是否成功		
	}
	else
	{
		res=2;
	}
	
	return ( res );
}


