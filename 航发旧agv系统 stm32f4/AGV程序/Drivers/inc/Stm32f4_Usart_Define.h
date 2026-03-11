#ifndef _Stm32f4_Usart_Define_H
#define _Stm32f4_Usart_Define_H


/* Private define ------------------------------------------------------------*/
	
/*使用到该型号MCU的串口定义
	1.不使用的USARTx 将其宏定义值改为0;
	2.不适用RS485硬件方向控制如下定义,使用则按照实际引脚改变
		#define UseUSART1_485_CLK		0
		#define UseUSART1_485_PORT	0
		#define UseUSART1_485_Pin		0
	*/
	
#define USARTMAXCNT											6		//定义USART外设数量

#define	USART485_MODE										1	//0:查询模式;1:DMA模式(仅使用发送模式,接收采用中断方式)
 
 
 
// USART1端口预定义
#define UseUSART1_BASE                                      USART1
#define UseUSART1_CLK                                       RCC_APB2Periph_USART1
#define UseUSART1_RCC_CMD                                   RCC_APB2

#define UseUSART1_TX_PIN                                    GPIO_Pin_9
#define UseUSART1_TX_GPIO_PORT                              GPIOA
#define UseUSART1_TX_GPIO_CLK                               RCC_AHB1Periph_GPIOA
#define UseUSART1_TX_SOURCE                                 GPIO_PinSource9
#define UseUSART1_TX_AF                                     GPIO_AF_USART1

#define UseUSART1_RX_PIN                                    GPIO_Pin_10
#define UseUSART1_RX_GPIO_PORT                              GPIOA
#define UseUSART1_RX_GPIO_CLK                               RCC_AHB1Periph_GPIOA
#define UseUSART1_RX_SOURCE                                 GPIO_PinSource10
#define UseUSART1_RX_AF                                     GPIO_AF_USART1

#define UseUSART1_IRQn                                      USART1_IRQn
#define UseUSART1_IRQHandler                                USART1_IRQHandler

#define UseUSART1_485_CLK																	  RCC_AHB1Periph_GPIOA
#define UseUSART1_485_PORT																	GPIOA
#define UseUSART1_485_Pin                                   GPIO_Pin_11

// USART2端口预定义
#define UseUSART2_BASE                                      USART2
#define UseUSART2_CLK                                       RCC_APB1Periph_USART2
#define UseUSART2_RCC_CMD                                   RCC_APB1

#define UseUSART2_TX_PIN                                    GPIO_Pin_2
#define UseUSART2_TX_GPIO_PORT                              GPIOA
#define UseUSART2_TX_GPIO_CLK                               RCC_AHB1Periph_GPIOA
#define UseUSART2_TX_SOURCE                                 GPIO_PinSource2
#define UseUSART2_TX_AF                                     GPIO_AF_USART2

#define UseUSART2_RX_PIN                                    GPIO_Pin_3
#define UseUSART2_RX_GPIO_PORT                              GPIOA
#define UseUSART2_RX_GPIO_CLK                               RCC_AHB1Periph_GPIOA
#define UseUSART2_RX_SOURCE                                 GPIO_PinSource3
#define UseUSART2_RX_AF                                     GPIO_AF_USART2

#define UseUSART2_IRQn                                      USART2_IRQn
#define UseUSART2_IRQHandler                                USART2_IRQHandler

#define UseUSART2_485_CLK																	  RCC_AHB1Periph_GPIOB
#define UseUSART2_485_PORT																	GPIOB
#define UseUSART2_485_Pin                                   GPIO_Pin_2

// USART3端口预定义
#define UseUSART3_BASE                                      USART3
#define UseUSART3_CLK                                       RCC_APB1Periph_USART3
#define UseUSART3_RCC_CMD                                   RCC_APB1

#define UseUSART3_TX_PIN                                    GPIO_Pin_10
#define UseUSART3_TX_GPIO_PORT                              GPIOB
#define UseUSART3_TX_GPIO_CLK                               RCC_AHB1Periph_GPIOB
#define UseUSART3_TX_SOURCE                                 GPIO_PinSource10
#define UseUSART3_TX_AF                                     GPIO_AF_USART3

#define UseUSART3_RX_PIN                                    GPIO_Pin_11
#define UseUSART3_RX_GPIO_PORT                              GPIOB
#define UseUSART3_RX_GPIO_CLK                               RCC_AHB1Periph_GPIOB
#define UseUSART3_RX_SOURCE                                 GPIO_PinSource11
#define UseUSART3_RX_AF                                     GPIO_AF_USART3

#define UseUSART3_IRQn                                      USART3_IRQn
#define UseUSART3_IRQHandler                                USART3_IRQHandler

#define UseUSART3_485_CLK																	  0
#define UseUSART3_485_PORT																	0
#define UseUSART3_485_Pin                                   0


// USART4端口预定义
#define UseUSART4_BASE                                      UART4
#define UseUSART4_CLK                                       RCC_APB1Periph_UART4
#define UseUSART4_RCC_CMD                                   RCC_APB1

#define UseUSART4_TX_PIN                                    GPIO_Pin_0
#define UseUSART4_TX_GPIO_PORT                              GPIOA
#define UseUSART4_TX_GPIO_CLK                               RCC_AHB1Periph_GPIOA
#define UseUSART4_TX_SOURCE                                 GPIO_PinSource0
#define UseUSART4_TX_AF                                     GPIO_AF_UART4

#define UseUSART4_RX_PIN                                    GPIO_Pin_1
#define UseUSART4_RX_GPIO_PORT                              GPIOA
#define UseUSART4_RX_GPIO_CLK                               RCC_AHB1Periph_GPIOA
#define UseUSART4_RX_SOURCE                                 GPIO_PinSource1
#define UseUSART4_RX_AF                                     GPIO_AF_UART4

#define UseUSART4_IRQn                                      UART4_IRQn
#define UseUSART4_IRQHandler                                UART4_IRQHandler

#define UseUSART4_485_CLK				  RCC_AHB1Periph_GPIOF        
#define UseUSART4_485_PORT				GPIOF                           
#define UseUSART4_485_Pin                                   GPIO_Pin_1   


// USART5端口预定义
#define UseUSART5_BASE                                      0
#define UseUSART5_CLK                                       0
#define UseUSART5_RCC_CMD                                   0

#define UseUSART5_TX_PIN                                    0
#define UseUSART5_TX_GPIO_PORT                              0
#define UseUSART5_TX_GPIO_CLK                               0
#define UseUSART5_TX_SOURCE                                 0
#define UseUSART5_TX_AF                                     0

#define UseUSART5_RX_PIN                                    0
#define UseUSART5_RX_GPIO_PORT                              0
#define UseUSART5_RX_GPIO_CLK                               0
#define UseUSART5_RX_SOURCE                                 0
#define UseUSART5_RX_AF                                     0

#define UseUSART5_IRQn                                      0
#define UseUSART5_IRQHandler                                0

#define UseUSART5_485_CLK																	  0
#define UseUSART5_485_PORT																	0
#define UseUSART5_485_Pin                                   0


// USART6端口预定义
#define UseUSART6_BASE                                      USART6
#define UseUSART6_CLK                                       RCC_APB2Periph_USART6
#define UseUSART6_RCC_CMD                                   RCC_APB2

#define UseUSART6_TX_PIN                                    GPIO_Pin_6
#define UseUSART6_TX_GPIO_PORT                              GPIOC
#define UseUSART6_TX_GPIO_CLK                               RCC_AHB1Periph_GPIOC
#define UseUSART6_TX_SOURCE                                 GPIO_PinSource6
#define UseUSART6_TX_AF                                     GPIO_AF_USART6

#define UseUSART6_RX_PIN                                    GPIO_Pin_7
#define UseUSART6_RX_GPIO_PORT                              GPIOC
#define UseUSART6_RX_GPIO_CLK                               RCC_AHB1Periph_GPIOC
#define UseUSART6_RX_SOURCE                                 GPIO_PinSource7
#define UseUSART6_RX_AF                                     GPIO_AF_USART6

#define UseUSART6_IRQn                                      USART6_IRQn
#define UseUSART6_IRQHandler                                USART6_IRQHandler

#define UseUSART6_485_CLK				RCC_AHB1Periph_GPIOF              
#define UseUSART6_485_PORT				GPIOF                             
#define UseUSART6_485_Pin                                   GPIO_Pin_0    



#if (USART485_MODE==1)

#define UseUSART1_Tx_DMAx_Streamx                   DMA2_Stream7
#define UseUSART2_Tx_DMAx_Streamx                   DMA1_Stream6
#define UseUSART3_Tx_DMAx_Streamx                   DMA1_Stream3
#define UseUSART4_Tx_DMAx_Streamx                   DMA1_Stream4
#define UseUSART5_Tx_DMAx_Streamx                   0
#define UseUSART6_Tx_DMAx_Streamx                   DMA2_Stream6

#define UseUSART1_Tx_DMAx_Streamx_IRQn                   DMA2_Stream7_IRQn
#define UseUSART2_Tx_DMAx_Streamx_IRQn                   DMA1_Stream6_IRQn
#define UseUSART3_Tx_DMAx_Streamx_IRQn                   DMA1_Stream3_IRQn
#define UseUSART4_Tx_DMAx_Streamx_IRQn                   DMA1_Stream4_IRQn
#define UseUSART5_Tx_DMAx_Streamx_IRQn                   0
#define UseUSART6_Tx_DMAx_Streamx_IRQn                   DMA2_Stream6_IRQn

#define UseUSART1_Tx_DMA_Channel                   DMA_Channel_4
#define UseUSART2_Tx_DMA_Channel                   DMA_Channel_4
#define UseUSART3_Tx_DMA_Channel                   DMA_Channel_4
#define UseUSART4_Tx_DMA_Channel                   DMA_Channel_4
#define UseUSART5_Tx_DMA_Channel                   0
#define UseUSART6_Tx_DMA_Channel                   DMA_Channel_5

#define UseUSART1_Tx_DMA_IT_TCIFx                  DMA_IT_TCIF7
#define UseUSART2_Tx_DMA_IT_TCIFx                  DMA_IT_TCIF6
#define UseUSART3_Tx_DMA_IT_TCIFx                  DMA_IT_TCIF3
#define UseUSART4_Tx_DMA_IT_TCIFx                  DMA_IT_TCIF4
#define UseUSART5_Tx_DMA_IT_TCIFx                  0
#define UseUSART6_Tx_DMA_IT_TCIFx                  DMA_IT_TCIF6

#endif


/*	通讯参数配置位相关	*/
#define USART_RX_UP			0x0000	//Bit0:RXD设为上拉
#define USART_RX_INF		0x0001	//Bit0:RXD设为浮空

#define USART_TX_PP			0x0000	//Bit1:TXD设为PP
#define USART_TX_OD			0x0002	//Bit1:TXD设为OD

#define USART_RS485_N		0x0000	//Bit2:RS485控制极性-
#define USART_RS485_P		0x0004	//Bit2:RS485控制极性+

#define USART_WL7				0x0000	//Bit3:数据位数---7位
#define USART_WL8				0x0008	//Bit3:数据位数---8位

#define USART_PI_NONE		0x0010	//Bit4:校验方式---无校验(数据位需设置成7位)
#define USART_PI_EVEN		0x0020	//Bit5:校验方式---偶校验(数据位需设置成8位)
#define	USAAT_PI_ODD		0x0040	//Bit6:校验方式---奇校验(数据位需设置成8位)

#define USART_ST_0_5		0x0080	//Bit7:0.5停止位
#define USART_ST_1			0x0100  //Bit8:1停止位
#define USART_ST_1_5		0x0200  //Bit9:1.5停止位
#define USART_ST_2			0x0400 	//Bit10:2停止位


//波特率值id
typedef enum{
	Baud_1200 = 0,
	Baud_2400,
	Baud_4800,
	Baud_9600,
	Baud_19200,
	Baud_38400,
	Baud_57600,
	Baud_115200,
	Baud_NB_MAX
}USART_BAUD;

typedef enum
{
	useUART1=0,
	useUART2=1,
	useUART3=2,
	useUART4=3,
	useUART5=4,
	useUART6=5
}USART_ID;

//sta,通讯状态的各位的定义
enum{
	B_EV_USART_None = 0,////未收到有效间隔数据包
	B_USART_P485,				//BIT0,表示RS485控制引脚的电平极性，=1表示高电平发送
	B_USART_SRS485,			//BIT1,软RS485控制口，如果没有硬件RS485端口，使用这个位
	B_EV_USART_RX,			//BIT2,收到一包数据事件
	B_EV_USART_TX, 			//BIT3,发送事件
	B_USART_REC_1BYTE,	//BIT4,收到一个字节的数据
	B_USART_ADR_OK,			//BIT5,已经收到一个合法的地址
	B_EV_Rx_TimeOut,			  //BIT6,超时
};


enum{
	B_USART_CR1_SBK = 1,
	B_USART_CR1_RWU,
	B_USART_CR1_RE,
	B_USART_CR1_TE,
	B_USART_CR1_IDLEIE,
	B_USART_CR1_RXNEIE,
	B_USART_CR1_TCIE,
	B_USART_CR1_TXEIE,
	B_USART_CR1_PEIE,
	B_USART_CR1_PS,
	B_USART_CR1_PCE,
	B_USART_CR1_WAKE,
	B_USART_CR1_M,
	B_USART_CR1_UE
};


//通讯配置位
enum{
			usart_use_remap = 1,
			usart_other
		};


typedef enum
{
	MASK_RXD_OType=0x0001,
	MASK_TXD_OType=0x0002,
	MASK_RS485_Polar=0x0004,
	MASK_USART_Word_Length=0x0008,
	MASK_USART_Parity=0x0070,
	MASK_USART_StopBits=0x0780,
}USART_CFG_MASK;














//extern	u32		BaudList[];










#endif

/***************************************************************************************
**  End Of File                                                     
***************************************************************************************/
