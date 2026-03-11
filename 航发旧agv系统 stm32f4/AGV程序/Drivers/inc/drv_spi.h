

#ifndef __DRVSPI_H
#define __DRVSPI_H




#ifdef		SPI_GLOBALS
	#define SPI_EXT
#else
	#define SPI_EXT  extern
#endif

	
#include "includes.h"
#include "..\..\Drivers\inc\drv_timer.h"
	
typedef enum
{
		PLAN1 = 1,//模拟SPI方案
		PLAN2,		//硬件SPI方案1
		PLAN3,		//硬件SPI方案2		
		PLAN4,
		PLAN5,
}SPICONFIGPLAN;

typedef enum
{
	uSPI1=0,
	uSPI2,	
}UseSPIx;

typedef enum
{
	HW_SPI_DMA = 1,	//使用SPI外设DMA方式	
	HW_SPI_Poll,		//使用SPI外设查询方式
	SW_SPI,					//GPIO软件模拟SPI方式
}WORK_TYPE;

/* Private define ------------------------------------------------------------*/
#define		SPI_NUM								2

#define		SPI_WORK_TYPE					2																	/*
SPI工作类型定义SPI_WORK_TYPE:
																1:DMA;
																2:Poll;
																3:软件模拟SPI
																																	*/
#define		SPI_OT_TIMES					1000		//SPI读写等待超时系数值

//SPI1外设相关宏定义
#define SPI1_BASEX							SPI1
#define SPI1_RCC								RCC_APB2Periph_SPI1
#define SPI1_GPIO_AF						GPIO_AF_SPI1
	
#define SPI1_GPIO_PORT_SCK			GPIOA
#define SPI1_GPIO_PIN_SCK				GPIO_Pin_5
#define SPI1_GPIO_CLK_SCK				RCC_AHB1Periph_GPIOA
#define SPI1_GPIO_SCK_SOURCE		GPIO_PinSource5
	
#define SPI1_GPIO_PORT_MISO			GPIOA
#define SPI1_GPIO_PIN_MISO			GPIO_Pin_6
#define SPI1_GPIO_CLK_MISO			RCC_AHB1Periph_GPIOA
#define SPI1_GPIO_MISO_SOURCE		GPIO_PinSource6

#define SPI1_GPIO_PORT_MOSI			GPIOA
#define SPI1_GPIO_PIN_MOSI			GPIO_Pin_7
#define SPI1_GPIO_CLK_MOSI			RCC_AHB1Periph_GPIOA
#define SPI1_GPIO_MOSI_SOURCE		GPIO_PinSource7

//SPI2外设相关宏定义
#define SPI2_BASEX							SPI2
#define SPI2_RCC								RCC_APB1Periph_SPI2
#define SPI2_GPIO_AF						GPIO_AF_SPI2
	
#define SPI2_GPIO_PORT_SCK			GPIOB
#define SPI2_GPIO_PIN_SCK				GPIO_Pin_13
#define SPI2_GPIO_CLK_SCK				RCC_AHB1Periph_GPIOB
#define SPI2_GPIO_SCK_SOURCE		GPIO_PinSource13
	
#define SPI2_GPIO_PORT_MISO			GPIOB
#define SPI2_GPIO_PIN_MISO			GPIO_Pin_14
#define SPI2_GPIO_CLK_MISO			RCC_AHB1Periph_GPIOB
#define SPI2_GPIO_MISO_SOURCE		GPIO_PinSource14

#define SPI2_GPIO_PORT_MOSI			GPIOB
#define SPI2_GPIO_PIN_MOSI			GPIO_Pin_15
#define SPI2_GPIO_CLK_MOSI			RCC_AHB1Periph_GPIOB
#define SPI2_GPIO_MOSI_SOURCE		GPIO_PinSource15


#if		(SPI_WORK_TYPE == 1)	//使用STM32_SPI外设---查询方式

			//SPI1的DMA配置宏
			#define		SPI1_DMAx											DMA2
			#define		SPI1_DMA_Channel							DMA_Channel_3

			#define		SPI1_DMA_Streamx_RX						DMA2_Stream0
			#define		SPI1_DMA_FLAG_TCIFx_RX				DMA_FLAG_TCIF0//:  Streamx transfer complete flag
			#define		SPI1_DMA_FLAG_HTIFx_RX				DMA_FLAG_HTIF0//:  Streamx half transfer complete flag
			#define		SPI1_DMA_FLAG_TEIFx_RX				DMA_FLAG_TEIF0//:  Streamx transfer error flag
			#define		SPI1_DMA_FLAG_DMEIFx_RX				DMA_FLAG_DMEIF0//: Streamx direct mode error flag
			#define		SPI1_DMA_FLAG_FEIFx_RX				DMA_FLAG_FEIF0//:  Streamx FIFO error flag

			#define		SPI1_DMA_Streamx_TX						DMA2_Stream5
			#define		SPI1_DMA_FLAG_TCIFx_TX				DMA_FLAG_TCIF5//:  Streamx transfer complete flag
			#define		SPI1_DMA_FLAG_HTIFx_TX				DMA_FLAG_HTIF5//:  Streamx half transfer complete flag
			#define		SPI1_DMA_FLAG_TEIFx_TX				DMA_FLAG_TEIF5//:  Streamx transfer error flag
			#define		SPI1_DMA_FLAG_DMEIFx_TX				DMA_FLAG_DMEIF5//: Streamx direct mode error flag
			#define		SPI1_DMA_FLAG_FEIFx_TX				DMA_FLAG_FEIF5//:  Streamx FIFO error flag

			#define		SPI1_DMA_IrqChannel_RX				DMA2_Stream0_IRQn
			#define		SPI1_DMA_IrqChannel_TX				DMA2_Stream5_IRQn

			#define		SPI1_DMA_SETUP								SPI1_DMAx,										\
																							SPI1_DMA_Channel,						\
																							SPI1_DMA_Streamx_RX,					\
																							SPI1_DMA_Streamx_TX,					\
																							SPI1_DMA_IrqChannel_RX,			\
																							SPI1_DMA_IrqChannel_TX

			#define		DMA_SPI1_RX_IRQHandler				DMA2_Stream0_IRQHandler
			#define		DMA_SPI1_TX_IRQHandler				DMA2_Stream5_IRQHandler
			
			//SPI2的DMA配置宏
			#define		SPI2_DMAx											DMA2
			#define		SPI2_DMA_Channel							DMA_Channel_3

			#define		SPI2_DMA_Streamx_RX						DMA2_Stream0
			#define		SPI2_DMA_FLAG_TCIFx_RX				DMA_FLAG_TCIF0//:  Streamx transfer complete flag
			#define		SPI2_DMA_FLAG_HTIFx_RX				DMA_FLAG_HTIF0//:  Streamx half transfer complete flag
			#define		SPI2_DMA_FLAG_TEIFx_RX				DMA_FLAG_TEIF0//:  Streamx transfer error flag
			#define		SPI2_DMA_FLAG_DMEIFx_RX				DMA_FLAG_DMEIF0//: Streamx direct mode error flag
			#define		SPI2_DMA_FLAG_FEIFx_RX				DMA_FLAG_FEIF0//:  Streamx FIFO error flag

			#define		SPI2_DMA_Streamx_TX						DMA2_Stream5
			#define		SPI2_DMA_FLAG_TCIFx_TX				DMA_FLAG_TCIF5//:  Streamx transfer complete flag
			#define		SPI2_DMA_FLAG_HTIFx_TX				DMA_FLAG_HTIF5//:  Streamx half transfer complete flag
			#define		SPI2_DMA_FLAG_TEIFx_TX				DMA_FLAG_TEIF5//:  Streamx transfer error flag
			#define		SPI2_DMA_FLAG_DMEIFx_TX				DMA_FLAG_DMEIF5//: Streamx direct mode error flag
			#define		SPI2_DMA_FLAG_FEIFx_TX				DMA_FLAG_FEIF5//:  Streamx FIFO error flag

			#define		SPI2_DMA_IrqChannel_RX				DMA2_Stream0_IRQn
			#define		SPI2_DMA_IrqChannel_TX				DMA2_Stream5_IRQn

			#define		SPI2_DMA_SETUP								SPI2_DMAx,										\
																							SPI2_DMA_Channel,						\
																							SPI2_DMA_Streamx_RX,					\
																							SPI2_DMA_Streamx_TX,					\
																							SPI2_DMA_IrqChannel_RX,			\
																							SPI2_DMA_IrqChannel_TX

			#define		DMA_SPI2_RX_IRQHandler			DMA2_Stream0_IRQHandler
			#define		DMA_SPI2_TX_IRQHandler			DMA2_Stream5_IRQHandler

#elif (SPI_WORK_TYPE == 2)	//使用STM32_SPI外设---DMA方式

#endif	//the end of DMA Type.




//EEPROM的相关GPIO定义
#define EE_GPIO_PORT_CS					GPIOB
#define EE_GPIO_PIN_CS					GPIO_Pin_12
#define EE_GPIO_CLK_CS					RCC_AHB1Periph_GPIOB
#define	EE_GPIO_CS							EE_GPIO_PORT_CS, EE_GPIO_PIN_CS
	
#define EE_GPIO_PORT_WP					GPIOH
#define EE_GPIO_PIN_WP					GPIO_Pin_11
#define EE_GPIO_CLK_WP					RCC_AHB1Periph_GPIOH
#define	EE_GPIO_WP							EE_GPIO_PORT_WP, EE_GPIO_PIN_WP
	
#define EE_GPIO_PORT_HOLD				GPIOH
#define EE_GPIO_PIN_HOLD				GPIO_Pin_12
#define EE_GPIO_CLK_HOLD				RCC_AHB1Periph_GPIOH
#define	EE_GPIO_HOLD						EE_GPIO_PORT_HOLD, EE_GPIO_PIN_HOLD
	
#define EE_GPIO_PORT_EE_PWR			GPIOF
#define EE_GPIO_PIN_EE_PWR			GPIO_Pin_10
#define EE_GPIO_CLK_EE_PWR			RCC_AHB1Periph_GPIOF
#define	EE_GPIO_EE_PWR					EE_GPIO_PORT_EE_PWR, EE_GPIO_PIN_EE_PWR





//SPI通道指定
#define				SPIA			uSPI1		//A接口给模拟量
#define				SPIB			uSPI2		//B接口给EEPROM


/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/


#if	(SPI_WORK_TYPE == 1)

#define		SPI_Buff_Size			250

SPI_EXT u16 	SPI_Rx_Buf[SPI_Buff_Size],//SPI_DMA发送Buf
							SPI_Tx_Buf[SPI_Buff_Size],//SPI_DMA接收Buf
							spi_dma_w_dat_len,//写的长度
							spi_dma_r_dat_len;//读的长度
#endif



/*
1.DS-------DataSize数据长度尺(Byte or HalfWord)
2.根据SPI外设配置其长度选择为：16bit或8bit

*/
//读单个DS
SPI_EXT u16 SpiReadSingleDS(UseSPIx ch);

//写单个DS
SPI_EXT void SpiWriteSingleDS(UseSPIx ch, u16 cmd);

//读写单个DS
SPI_EXT u16 SpiRWSingleDS(UseSPIx ch, u16 cmd);



//SPI外设初始化
SPI_EXT void Spi_InitHard(UseSPIx ch, SPICONFIGPLAN config);


SPI_EXT	u8 EEPROM_InitHard(void);





SPI_EXT	SPI_TypeDef*	SPIx_BASE[SPI_NUM];
SPI_EXT	GPIO_TypeDef	*SPIx_GPIO_PORT_SCK[SPI_NUM];
SPI_EXT	GPIO_TypeDef	*SPIx_GPIO_PORT_MOSI[SPI_NUM];
SPI_EXT	GPIO_TypeDef	*SPIx_GPIO_PORT_MISO[SPI_NUM];
SPI_EXT	const u32			SPIx_CLK[SPI_NUM];

SPI_EXT	const u32			SPIx_GPIO_PIN_SCK[SPI_NUM];
SPI_EXT	const u32			SPIx_GPIO_PIN_MISO[SPI_NUM];
SPI_EXT	const u32			SPIx_GPIO_PIN_MOSI[SPI_NUM];

SPI_EXT	const u32			SPIx_GPIO_AF[SPI_NUM];

SPI_EXT	const u32			SPIx_GPIO_CLK_SCK[SPI_NUM];
SPI_EXT	const u32			SPIx_GPIO_CLK_MISO[SPI_NUM];
SPI_EXT	const u32			SPIx_GPIO_CLK_MOSI[SPI_NUM];

SPI_EXT	const u32			SPIx_GPIO_SOURCE_SCK[SPI_NUM];
SPI_EXT	const u32			SPIx_GPIO_SOURCE_MISO[SPI_NUM];
SPI_EXT	const u32			SPIx_GPIO_SOURCE_MOSI[SPI_NUM];




#endif


