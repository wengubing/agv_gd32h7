#ifndef __DRV_CAN_H__
#define __DRV_CAN_H__

#include "..\..\Drivers\Common\DeviceDefine.h"

#ifdef		CAN_GLOBALS
	#define CAN_EXT
#else
	#define CAN_EXT  extern
#endif



/* Private define ------------------------------------------------------------*/

#define	CAN_MODE												2


#define CANn                            1

// SCAN1
#define SCAN1_BASE                      CAN1
#define SCAN1_CLK                       RCC_APB1Periph_CAN1
#define SCAN1_RX_PIN                    GPIO_Pin_8
#define SCAN1_TX_PIN                    GPIO_Pin_9
#define SCAN1_GPIO_PORT                 GPIOB
#define SCAN1_GPIO_CLK                  RCC_AHB1Periph_GPIOB
#define SCAN1_AF_PORT                   GPIO_AF_CAN1
#define SCAN1_RX_SOURCE                 GPIO_PinSource8
#define SCAN1_TX_SOURCE                 GPIO_PinSource9

#define SCAN1_RX0_IRQn                  CAN1_RX0_IRQn
#define SCAN1_RX1_IRQn                  CAN1_RX1_IRQn

#define SYS_CAN1_RX_IRQHandler					CAN1_RX0_IRQHandler
#define SYS_CAN1_SCE_IRQHandler					CAN1_SCE_IRQHandler

// SCAN2
#define SCAN2_BASE                      CAN2
#define SCAN2_CLK                       RCC_APB1Periph_CAN2
#define SCAN2_RX_PIN                    GPIO_Pin_5
#define SCAN2_TX_PIN                    GPIO_Pin_6
#define SCAN2_GPIO_PORT                 GPIOB
#define SCAN2_GPIO_CLK                  RCC_AHB1Periph_GPIOB
#define SCAN2_AF_PORT                   GPIO_AF_CAN2
#define SCAN2_RX_SOURCE                 GPIO_PinSource5
#define SCAN2_TX_SOURCE                 GPIO_PinSource6

#define SCAN2_RX0_IRQn                  CAN2_RX0_IRQn
#define SCAN2_RX1_IRQn                  CAN2_RX1_IRQn


#define CAN_RX_BUF_LENGTH_MAX						512    //定义接收报文最大长度
#define CAN_TX_BUF_LENGTH_MAX						512    //定义发送报文最大长度

#define CAN_BROADCAST_ADDRESS						0xFF	// CAN接收广播地址





/* Private macro -------------------------------------------------------------*/




/* Private typedef -----------------------------------------------------------*/
typedef enum
{
    useCAN1=0,
    useCAN2,
}CAN_ID;

typedef enum
{
	CAN_NoneError = 0,
	CAN_TransmitError,
	CAN_ReceiveError,
	CAN_GoToFault,
}CAN_Err;



// 常用过滤器组编号(取值:0-13)
typedef enum
{
		nb_filter0 = 0,
		nb_filter1,
		nb_filter2,
		nb_filter3,
		nb_filter4,
		nb_filter5,
		nb_filter6,
		nb_filter7,
		nb_filter8,
		nb_filter9,
		nb_filter10,
		nb_filter11,
		nb_filter12,
		nb_filter13
}CAN_FILTER_TYPE;

// 常用波特率值
typedef enum
{
    CAN_BAUDRATE_5K = 1,
    CAN_BAUDRATE_10K,
    CAN_BAUDRATE_20K,
    CAN_BAUDRATE_40K,
    CAN_BAUDRATE_50K,
    CAN_BAUDRATE_80K,
    CAN_BAUDRATE_100K,
    CAN_BAUDRATE_125K,
    CAN_BAUDRATE_200K,
    CAN_BAUDRATE_250K,
    CAN_BAUDRATE_400K,
    CAN_BAUDRATE_500K,
    CAN_BAUDRATE_800K,
    CAN_BAUDRATE_1M,
}CAN_BAUD_TYPE;

//CAN初始化结构体
typedef struct
{
	CAN_ID					CANx;//usart当前的ID编号
	CAN_BAUD_TYPE		Baud;//当前设置的波特率值
}CAN_Ini;

//CAN_RX_FIFO
typedef struct
{
	CanRxMsg		can_rx_msg[CAN_RX_BUF_LENGTH_MAX];//接收fifo邮箱缓冲
	u32					read_adr,		//fifo读取地址
							write_adr;	//fifo写入地址
}CAN_RX_FIFO;

//CAN_TX_FIFO
typedef struct
{
	CanTxMsg		can_Tx_msg[CAN_TX_BUF_LENGTH_MAX];//接收fifo邮箱缓冲
	u32					read_adr,		//fifo读取地址
							write_adr;	//fifo写入地址
}CAN_TX_FIFO;

// CAN接收/发送缓冲区
typedef struct
{
	CAN_Ini				ini;
	CAN_RX_FIFO		rx_fifo;
	CAN_TX_FIFO		tx_fifo;
	u8						CanStatusFlag,
								RxErrorCounter,
								TxErrorCounter,
								LastErrorCode;
}STM32_CANi;


CAN_EXT STM32_CANi		STM32_CAN[CANn];

#define		user_CAN_ID			useCAN1


CAN_EXT u8 	Set_CAN_Int(CAN_ID CANx, CAN_BAUD_TYPE can_baud, u32 addr, u8 can_pi);
CAN_EXT u8	can_send_msg(CAN_ID Canx, CanTxMsg TxMessage);





#endif
