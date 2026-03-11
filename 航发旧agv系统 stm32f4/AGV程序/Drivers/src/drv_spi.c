/**********************************************************************************************************
*
*	模块名称 : stm32f4三线SPI底层操作
*	文件名称 : drv_spi.c
*	版    本 : V1.0
*	说    明 : 主要实现SPI总线配置和读写相关操作

2015.05.25 新建.  by 邓洋

**********************************************************************************************************/



#define		SPI_GLOBALS

/* Includes ------------------------------------------------------------------*/

#include "..\..\Drivers\inc\drv_spi.h"




/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

#if		(SPI_WORK_TYPE == 1)	//使用STM32_SPI外设---查询方式

	DMA_Stream_TypeDef*	SPIx_DMA_Streamx_TX[SPI_NUM]		= {SPI1_DMA_Streamx_TX, 		SPI2_DMA_Streamx_TX};
	DMA_Stream_TypeDef*	SPIx_DMA_Streamx_RX[SPI_NUM]		= {SPI1_DMA_Streamx_RX, 		SPI2_DMA_Streamx_RX};
	const u32						SPIx_DMA_FLAG_TCIFx_RX[SPI_NUM]	= {SPI1_DMA_FLAG_TCIFx_TX, 	SPI2_DMA_FLAG_TCIFx_TX};
	const u32						SPIx_DMA_FLAG_TCIFx_TX[SPI_NUM]	= {SPI1_DMA_FLAG_TCIFx_RX, 	SPI2_DMA_FLAG_TCIFx_RX};
	
#endif	//the end of DMA Type.

u16 TXE_cnt, RXNE_cnt;

SPI_TypeDef*	SPIx_BASE[SPI_NUM]								= {SPI1_BASEX,						SPI2_BASEX};
GPIO_TypeDef	*SPIx_GPIO_PORT_SCK[SPI_NUM]			= {SPI1_GPIO_PORT_SCK,		SPI2_GPIO_PORT_SCK};
GPIO_TypeDef	*SPIx_GPIO_PORT_MOSI[SPI_NUM]			= {SPI1_GPIO_PORT_MOSI, 	SPI2_GPIO_PORT_MOSI};
GPIO_TypeDef	*SPIx_GPIO_PORT_MISO[SPI_NUM]			= {SPI1_GPIO_PORT_MISO, 	SPI2_GPIO_PORT_MISO};
const u32			SPIx_CLK[SPI_NUM]									= {SPI1_RCC, 							SPI2_RCC};

const u32			SPIx_GPIO_PIN_SCK[SPI_NUM]				= {SPI1_GPIO_PIN_SCK,			SPI2_GPIO_PIN_SCK};
const u32			SPIx_GPIO_PIN_MISO[SPI_NUM]				= {SPI1_GPIO_PIN_MISO,		SPI2_GPIO_PIN_MISO};
const u32			SPIx_GPIO_PIN_MOSI[SPI_NUM]				= {SPI1_GPIO_PIN_MOSI,		SPI2_GPIO_PIN_MOSI};

const u32			SPIx_GPIO_AF[SPI_NUM]							= {SPI1_GPIO_AF,					SPI2_GPIO_AF};

const u32			SPIx_GPIO_CLK_SCK[SPI_NUM]				= {SPI1_GPIO_CLK_SCK,			SPI2_GPIO_CLK_SCK};
const u32			SPIx_GPIO_CLK_MISO[SPI_NUM]				= {SPI1_GPIO_CLK_MISO,		SPI2_GPIO_CLK_MISO};
const u32			SPIx_GPIO_CLK_MOSI[SPI_NUM]				= {SPI1_GPIO_CLK_MOSI,		SPI2_GPIO_CLK_MOSI};

const u32			SPIx_GPIO_SOURCE_SCK[SPI_NUM]			= {SPI1_GPIO_SCK_SOURCE,	SPI2_GPIO_SCK_SOURCE};
const u32			SPIx_GPIO_SOURCE_MISO[SPI_NUM]		= {SPI1_GPIO_MISO_SOURCE,	SPI2_GPIO_MISO_SOURCE};
const u32			SPIx_GPIO_SOURCE_MOSI[SPI_NUM]		= {SPI1_GPIO_MOSI_SOURCE,	SPI2_GPIO_MOSI_SOURCE};

#if	(SPI_WORK_TYPE == 1)	//使用STM32_SPI外设---DMA传输方式

//=============================================================================
// 名称：SPI_DMA_Config
// 功能：SPI_DMA配置
// 参数：None
// 返回：None
// 说明：None
//=============================================================================
void SPI_DMA_Config(DMA_TypeDef* DMAx, u32 DMA_Channel, 
										DMA_Stream_TypeDef 	*pDMAy_Streamx_RX, DMA_Stream_TypeDef	*pDMAy_Streamx_TX, 
										u8	DMA_IRQChannel_RX, u8	DMA_IRQChannel_TX,
										u32 PeripheralBaseAddr,
										u8	pi)
{
	DMA_InitTypeDef  		DMA_IS_SPI;
	NVIC_InitTypeDef		NVIC_IS;

	if(DMAx == DMA1)
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	}
	else if(DMAx == DMA2)
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	}

	DMA_DeInit(pDMAy_Streamx_RX);//SPI_RX
  DMA_IS_SPI.DMA_Channel 							= DMA_Channel;  
	DMA_IS_SPI.DMA_PeripheralBaseAddr 	= PeripheralBaseAddr;//源头
	DMA_IS_SPI.DMA_Memory0BaseAddr			= (u32)SPI_Rx_Buf;//目的地
	DMA_IS_SPI.DMA_DIR									= DMA_DIR_PeripheralToMemory;//方向：外设->内存
	DMA_IS_SPI.DMA_PeripheralInc				= DMA_PeripheralInc_Disable;//接收一次后外设地址不变
	DMA_IS_SPI.DMA_MemoryInc						= DMA_MemoryInc_Enable;//接收一次数据后,内存地址寄存器递增↑
	DMA_IS_SPI.DMA_BufferSize						= SPI_Buff_Size;//DMA缓存大小
	DMA_IS_SPI.DMA_PeripheralDataSize		=	DMA_PeripheralDataSize_Byte;//BYTE
	DMA_IS_SPI.DMA_MemoryDataSize				=	DMA_MemoryDataSize_Byte;
	DMA_IS_SPI.DMA_Mode									= DMA_Mode_Normal;
	DMA_IS_SPI.DMA_Priority							= DMA_Priority_VeryHigh;
  DMA_IS_SPI.DMA_FIFOMode 						= DMA_FIFOMode_Disable;         
  DMA_IS_SPI.DMA_FIFOThreshold 				= DMA_FIFOThreshold_HalfFull;
  DMA_IS_SPI.DMA_MemoryBurst 					= DMA_MemoryBurst_Single;
  DMA_IS_SPI.DMA_PeripheralBurst 			= DMA_PeripheralBurst_Single;
	DMA_Init(pDMAy_Streamx_RX, &DMA_IS_SPI);

	NVIC_IS.NVIC_IRQChannel = DMA_IRQChannel_RX;
	NVIC_IS.NVIC_IRQChannelPreemptionPriority = pi;
	NVIC_IS.NVIC_IRQChannelSubPriority = 0;
	NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_IS);
	
	DMA_DeInit(pDMAy_Streamx_TX);//SPI_TX
  DMA_IS_SPI.DMA_Channel 							= DMA_Channel;  
	DMA_IS_SPI.DMA_PeripheralBaseAddr 	= PeripheralBaseAddr;
	DMA_IS_SPI.DMA_Memory0BaseAddr			= (u32)SPI_Tx_Buf;
	DMA_IS_SPI.DMA_DIR									= DMA_DIR_MemoryToPeripheral;//方向：内存->外设
	DMA_IS_SPI.DMA_PeripheralInc				= DMA_PeripheralInc_Disable;
	DMA_IS_SPI.DMA_MemoryInc						= DMA_MemoryInc_Enable;
	DMA_IS_SPI.DMA_BufferSize						= SPI_Buff_Size;
	DMA_IS_SPI.DMA_PeripheralDataSize		= DMA_PeripheralDataSize_Byte;
	DMA_IS_SPI.DMA_MemoryDataSize				= DMA_MemoryDataSize_Byte;
	DMA_IS_SPI.DMA_Mode									= DMA_Mode_Normal;
	DMA_IS_SPI.DMA_Priority							= DMA_Priority_VeryHigh;
  DMA_IS_SPI.DMA_FIFOMode 						= DMA_FIFOMode_Disable;         
  DMA_IS_SPI.DMA_FIFOThreshold 				= DMA_FIFOThreshold_HalfFull;
  DMA_IS_SPI.DMA_MemoryBurst 					= DMA_MemoryBurst_Single;
  DMA_IS_SPI.DMA_PeripheralBurst 			= DMA_PeripheralBurst_Single;
	DMA_Init(pDMAy_Streamx_TX, &DMA_IS_SPI);

	NVIC_IS.NVIC_IRQChannel = DMA_IRQChannel_TX;
	NVIC_IS.NVIC_IRQChannelPreemptionPriority = pi;
	NVIC_IS.NVIC_IRQChannelSubPriority = 0;
	NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_IS);

	DMA_ClearFlag(pDMAy_Streamx_RX, DMA_IT_TCIF0);
	DMA_ClearFlag(pDMAy_Streamx_TX, DMA_IT_TCIF0);

	DMA_ITConfig(pDMAy_Streamx_RX, DMA_IT_TC, ENABLE);//打开DMA1_IT_TC4中断,使用DMA传输,不需用中断
	DMA_ITConfig(pDMAy_Streamx_TX, DMA_IT_TC, ENABLE);

	DMA_Cmd(pDMAy_Streamx_RX,DISABLE);//默认,关闭DMA传输
	DMA_Cmd(pDMAy_Streamx_TX,DISABLE);
}

//=============================================================================
// 名称：DMA_SPI_TX_IRQHandler
// 功能：SPI发送中断函数
// 参数：None
// 返回：None
// 说明：SPI_RX
//=============================================================================
void DMA_SPI1_RX_IRQHandler(void)
{
	if(DMA_GetFlagStatus(SPI1_DMA_Streamx_RX, DMA_IT_TC))
	{
		DMA_ClearFlag(SPI1_DMA_Streamx_RX, DMA_IT_TC);
	}
}

//=============================================================================
// 名称：DMA_SPI_TX_IRQHandler
// 功能：SPI接收中断函数
// 参数：None
// 返回：None
// 说明：SPI_TX
//=============================================================================
void DMA_SPI1_TX_IRQHandler(void)
{
	if(DMA_GetFlagStatus(SPI1_DMA_Streamx_TX, DMA_IT_TC))
	{
		DMA_ClearFlag(SPI1_DMA_Streamx_TX, DMA_IT_TC);
	}
}

//=============================================================================
// 名称：SPI_DMA_Send_Receive
// 功能：SPI_DMA发送/接收数据处理
// 参数：nb:发送/接收DS数
// 返回：None
// 说明：None
//=============================================================================
void SPI_DMA_TX_RX(UseSPIx ch, u16 nb)
{
	u16 ot_times = 0;

	DMA_SetCurrDataCounter(SPIx_DMA_Streamx_RX[ch], nb);//传输数量设置
	DMA_SetCurrDataCounter(SPIx_DMA_Streamx_TX[ch], nb);
	
	DMA_ClearFlag(SPIx_DMA_Streamx_RX[ch], SPIx_DMA_FLAG_TCIFx_RX[ch]);//清除相关中断标识位
	DMA_ClearFlag(SPIx_DMA_Streamx_TX[ch], SPIx_DMA_FLAG_TCIFx_TX[ch]);
	
	SPIx_BASE[ch]->DR;//接收发送前读一次SPI1->DR,保证接收缓冲区为空

	while((SPI_I2S_GetFlagStatus(SPIx_BASE[ch], SPI_I2S_FLAG_TXE) == RESET) && (ot_times < SPI_OT_TIMES))//等待SPI发送缓冲TXE_Buf为空(等待发送完成)
	{//等待发送空
		if(ot_times ++ >= SPI_OT_TIMES)
		{
			if(TXE_cnt ++ > 0xFFFF)
			{
				TXE_cnt = 0;
			}
		}
	}
	ot_times = 0;

	DMA_Cmd(SPIx_DMA_Streamx_TX[ch], ENABLE);//先开启发送通道
	DMA_Cmd(SPIx_DMA_Streamx_RX[ch], ENABLE);
	
	while((SPI_I2S_GetFlagStatus(SPIx_BASE[ch], SPI_I2S_FLAG_RXNE) == RESET) && (ot_times < SPI_OT_TIMES))//等待DMA传输完成(在此接收从机数据完成)
	{//等待接收非空
		if(ot_times ++ >= SPI_OT_TIMES)
		{
			if(RXNE_cnt ++ > 0xFFFF)
			{
				RXNE_cnt = 0;
			}
		}
	}
	
	DMA_Cmd(SPIx_DMA_Streamx_TX[ch], DISABLE);//关闭DMA传输
	DMA_Cmd(SPIx_DMA_Streamx_RX[ch], DISABLE);
}

//读一个DS
void SpiReadSingleDS(UseSPIx ch, u16 *buffer)
{
	if(buffer==0)
		return;

	SPI_DMA_TX_RX(ch, 1);
	*buffer = SPI_Rx_Buf[2];
}

//写一个DS
void SpiWriteSingleDS(UseSPIx ch, u16 buffer)
{	
	SPI_Tx_Buf[0] = buffer;
	SPI_DMA_TX_RX(ch, 1);
}

//读写一个DS
u16 SpiRWSingleDS(UseSPIx ch, u16 cmd)
{
	SPI_Tx_Buf[0] = cmd;
	SPI_DMA_TX_RX(ch, 1);
	
	return (SPI_Rx_Buf[1]);
}

//读多个DS
void SpiReadMultiDS(UseSPIx ch, u16 address, u16 *buffer, u8 len)
{
	u8 i = 0;
	
	if (len > 128 || len == 0) 
		return;	
	
	SPI_Tx_Buf[0] = (len & 0x7F);//byte1
	SPI_Tx_Buf[1] = address;//byte2
	for (i = 0; i < len; i ++)
	{
		SPI_Tx_Buf[2 + i] = 0xff;//byte_len,用于激活SPI_CLK	
	}
	SPI_DMA_TX_RX(ch, (2 + len));
	for (i = 0; i < len; i ++)
	{
	 	*buffer ++ = SPI_Rx_Buf[2 + i];	
	}
	spi_dma_r_dat_len = (len + 2);
}

//写多个DS
void SpiWriteMultiDS(UseSPIx ch, u16 address, u16 *buffer, u8 len)
{
	u8 i = 0;

	if (len > 0x80 || len == 0) 
		return;
	
	SPI_Tx_Buf[0] = (0x80 | (len & 0x7F));//byte1
	SPI_Tx_Buf[1] = address;//byte2
	for (i = 0; i < len; i ++)
	{
	 	SPI_Tx_Buf[2 + i] = buffer[i];//byte_len
	}
	SPI_DMA_TX_RX(ch, (2 + len));
	spi_dma_w_dat_len = (len + 2);
}

//-------------------------------------------------------------------------------------------------------- the end DMA for SPI

#elif (SPI_WORK_TYPE == 2)	//使用STM32_SPI外设---查询方式
//=============================================================================
// 名称：SPI_Poll_TX_RX
// 功能：SPI查询方式使用的发送、接收一字节数据
// 参数：
//			My_SPIx:使用哪个SPIx
//			dat:发送的数据
// 返回：dat_buf:读取DR的值
// 说明：None
//=============================================================================
u16 SPI_Poll_TX_RX(SPI_TypeDef* My_SPIx, u16 dat)
{
	u16 ot_times = 0;
	
	My_SPIx->DR;//操作前,读一次DR
	
	while((SPI_I2S_GetFlagStatus(My_SPIx, SPI_I2S_FLAG_TXE)==RESET)  && (ot_times < SPI_OT_TIMES))
	{//等待发送空
		if(ot_times ++ >= SPI_OT_TIMES)
		{
			if(TXE_cnt ++ > 0xFFFF)
			{
				TXE_cnt = 0;
			}
		}
	}
	
	My_SPIx->DR=dat;//发送...
	
	ot_times = 0;
	while((SPI_I2S_GetFlagStatus(My_SPIx, SPI_I2S_FLAG_RXNE)==RESET)  && (ot_times < SPI_OT_TIMES))
	{//等待接收非空
		if(ot_times ++ >= SPI_OT_TIMES)
		{
			if(RXNE_cnt ++ > 0xFFFF)
			{
 				RXNE_cnt = 0;
			}
		}
	}

	return (My_SPIx->DR);
}

//读一个DS
u16 SpiReadSingleDS(UseSPIx ch)
{
	return(SPI_Poll_TX_RX(SPIx_BASE[ch], 0xFFFF));
}

//写一个DS
void SpiWriteSingleDS(UseSPIx ch, u16 cmd)
{
	SPI_Poll_TX_RX(SPIx_BASE[ch], cmd);
}

//读写一个DS
u16 SpiRWSingleDS(UseSPIx ch, u16 cmd)
{
	return (SPI_Poll_TX_RX(SPIx_BASE[ch], cmd));
}

//读多个DS
void SpiReadMultiByte(UseSPIx ch, u16 address, u16 *buffer, u8 len)
{
	if (len > 0x80 || len == 0) return;

	SPI_Poll_TX_RX(SPIx_BASE[ch], (len & 0x7F));
	SPI_Poll_TX_RX(SPIx_BASE[ch], address);

	while(len-- > 0)
	{
		*buffer++ = SPI_Poll_TX_RX(SPIx_BASE[ch], 0xFFFF);
	}
}

//写多个DS
void SpiWriteMultiDS(UseSPIx ch, u16 address, u16 *buffer, u8 len)
{
	SPI_Poll_TX_RX(SPIx_BASE[ch], (0x80 | (len & 0x7F)));
	SPI_Poll_TX_RX(SPIx_BASE[ch], address);

	while(len-- > 0)
	{
		SPI_Poll_TX_RX(SPIx_BASE[ch], *buffer++);
	}
}

//-------------------------------------------------------------------------------------------------------- the end HW_SPI_Poll
#elif (SPI_WORK_TYPE == 3)	//SW_SPI---软件模拟SPI
void sw_spi_write_byte(u8 send_dat)
{
	u8 x = 0;

	nTRX_MOSI_PIN_SET_H();
	nTRX_CLK_PIN_SET_H();
	nano_SPI_FLASH_CS_LOW();
	Delay_1us_1us(5);
	for(x = 0; x < 8; x ++)
	{
		if((send_dat & 0x01) == 0x01)//LSB
		{
			nTRX_MOSI_PIN_SET_H();//1
		}
		else// if((send_dat & 0x01) == 0x00)
		{
			nTRX_MOSI_PIN_SET_L();//0
		}
		send_dat >>= 1;	
		nTRX_CLK_PIN_SET_L();// 下降沿,将数据送入nTRX_IO
		Delay_1us(1);
		nTRX_CLK_PIN_SET_H();//上升沿,nTRX收数据锁存
		Delay_1us(1);
	}
	nano_SPI_FLASH_CS_HIGH();
	Delay_1us(5);
}

u8 sw_spi_read_byte(void)
{
	u8 x = 0, dat_buf = 0;
	
	nTRX_MOSI_PIN_SET_H();
	nTRX_CLK_PIN_SET_H();
	nano_SPI_FLASH_CS_LOW();
	Delay_1us(5);
	for(x = 0; x < 8; x ++)
	{
		nTRX_CLK_PIN_SET_L();//每个CLK下降沿,nTRX送数据
		Delay_1us(1);
		dat_buf |=  ((u8)BIT_MISO_status1 << x);
		nTRX_CLK_PIN_SET_H();
		Delay_1us(1);
	}
	nano_SPI_FLASH_CS_HIGH();
	Delay_1us(5);
	return	dat_buf;
}

void sw_spi_write(u8 *buf2, u8 num)
{
	u8 m = 0;
	
	for(m = 0; m < num; m ++)
	{
		sw_spi_write_byte(*buf2 ++);
	}
}

void sw_spi_read(u8 *buf, u8 num)
{
	u8 m = 0;
	
	for(m = 0; m < num; m ++)
	{
		*(buf ++) = sw_spi_read_byte();
	}
}

void SpiReadSingleDS(u8 address, u8 *buffer)
{
	sw_spi_write_byte(0x01);//byte1
	
 	sw_spi_write_byte(address);//byte2

	*buffer = sw_spi_read_byte();	
}

void SpiWriteSingleDS(u16 buffer)
{
	sw_spi_write_byte(buffer);
}

void SpiReadMultiDS(u8 address, u8 *buffer, u8 len)
{
	if (len > 0x80 || len == 0) return;

	sw_spi_write_byte((len & 0x7F));//byte1
	
 	sw_spi_write_byte(address);//byte2
	
	sw_spi_read(buffer, len);

}

void SpiWriteMultiDS(u8 address, u16 *buffer, u8 len)
{
	if (len > 0x80 || len == 0) return;

	sw_spi_write_byte((0x80 | (len & 0x7F)));//byte1
	
 	sw_spi_write_byte(address);//byte2

	sw_spi_write(buffer, len);
}

unsigned char ReadReg(u8 address)
{
	unsigned char buf;

	sw_spi_write_byte(0x01);

 	sw_spi_write_byte(address);
	
	buf = sw_spi_read_byte();

	return buf;
}


#endif


/**********************************************************************************************************
*	函 数 名: Spi_InitHard
*	功能说明: spi初始化
*	形    参:  无
*	返 回 值:  无
**********************************************************************************************************/
void Spi_InitHard(UseSPIx ch, SPICONFIGPLAN config)
{
  GPIO_InitTypeDef		GPIO_IS;

	SPI_I2S_DeInit(SPIx_BASE[ch]);
	
	if(SPIx_BASE[ch]==SPI1)
	{//SPI1
		RCC_APB2PeriphClockCmd(SPIx_CLK[ch], ENABLE);//80MHz
	}
	else 
	{//SPI2、SPI3...
		RCC_APB1PeriphClockCmd(SPIx_CLK[ch], ENABLE);//40MHz
	}	

	RCC_AHB1PeriphClockCmd( SPIx_GPIO_CLK_SCK[ch] | SPIx_GPIO_CLK_MISO[ch] | SPIx_GPIO_CLK_MOSI[ch], ENABLE);

	if(config==PLAN1)
	{//端口模拟SPI
		GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;	
		GPIO_IS.GPIO_OType = GPIO_OType_PP;
		GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_IS.GPIO_Pin = SPIx_GPIO_PIN_SCK[ch];//CLK
		GPIO_Init(SPIx_GPIO_PORT_SCK[ch], &GPIO_IS);

		GPIO_IS.GPIO_Pin = SPIx_GPIO_PIN_MOSI[ch];//MOSI
		GPIO_Init(SPIx_GPIO_PORT_MOSI[ch], &GPIO_IS);
			
		GPIO_IS.GPIO_Mode = GPIO_Mode_IN;
		GPIO_IS.GPIO_Pin = SPIx_GPIO_PIN_MISO[ch];//MISO
		GPIO_Init(SPIx_GPIO_PORT_MISO[ch], &GPIO_IS);	
		
		SPIx_GPIO_PORT_SCK[ch]->BSRRL=SPIx_GPIO_PIN_SCK[ch];
		SPIx_GPIO_PORT_MOSI[ch]->BSRRL=SPIx_GPIO_PIN_MOSI[ch];
		SPIx_GPIO_PORT_MISO[ch]->BSRRL=SPIx_GPIO_PIN_MISO[ch];
	}
	else
	{
	#if 	((SPI_WORK_TYPE == 1) || (SPI_WORK_TYPE == 2))
		
			SPI_InitTypeDef			SPI_IS;
		
			GPIO_IS.GPIO_Mode = GPIO_Mode_AF;	
			GPIO_IS.GPIO_OType = GPIO_OType_PP;
			GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
			GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
			
			GPIO_IS.GPIO_Pin = SPIx_GPIO_PIN_SCK[ch];//CLK
			GPIO_Init(SPIx_GPIO_PORT_SCK[ch], &GPIO_IS);

			GPIO_IS.GPIO_Pin = SPIx_GPIO_PIN_MOSI[ch];//MOSI
			GPIO_Init(SPIx_GPIO_PORT_MOSI[ch], &GPIO_IS);
				
			GPIO_IS.GPIO_Pin = SPIx_GPIO_PIN_MISO[ch];//MISO
			GPIO_Init(SPIx_GPIO_PORT_MISO[ch], &GPIO_IS);	
		
			GPIO_PinAFConfig(SPIx_GPIO_PORT_SCK[ch],  SPIx_GPIO_SOURCE_SCK[ch],  SPIx_GPIO_AF[ch]);
			GPIO_PinAFConfig(SPIx_GPIO_PORT_MISO[ch], SPIx_GPIO_SOURCE_MISO[ch], SPIx_GPIO_AF[ch]);
			GPIO_PinAFConfig(SPIx_GPIO_PORT_MOSI[ch], SPIx_GPIO_SOURCE_MOSI[ch], SPIx_GPIO_AF[ch]);

			//SPIx寄存器设置
			if(config==PLAN2)
			{
				SPI_IS.SPI_Direction					= SPI_Direction_2Lines_FullDuplex;//全双工模式
				SPI_IS.SPI_Mode								= SPI_Mode_Master;
				SPI_IS.SPI_DataSize 					= SPI_DataSize_16b;//数据长度16bit
				SPI_IS.SPI_CPHA 							= SPI_CPHA_2Edge;//数据采集从第2个CLK边沿开始:1(上升沿采样输出)
				SPI_IS.SPI_CPOL 							= SPI_CPOL_Low;//SPI总线空闲时刻为:0
				SPI_IS.SPI_NSS 								= SPI_NSS_Soft;//软件值CS脚的高低电平
				SPI_IS.SPI_BaudRatePrescaler	= SPI_BaudRatePrescaler_32;//5MHz
				SPI_IS.SPI_FirstBit 					= SPI_FirstBit_MSB;//MSB在前
				SPI_IS.SPI_CRCPolynomial			= 7;//CRC值计算的多项式最高位为7次
			}
			else if(config==PLAN3)
			{
				SPI_IS.SPI_Direction					= SPI_Direction_2Lines_FullDuplex;//全双工模式
				SPI_IS.SPI_Mode								= SPI_Mode_Master;
				SPI_IS.SPI_DataSize 					= SPI_DataSize_8b;//数据长度8bit
				SPI_IS.SPI_CPHA 							= SPI_CPHA_2Edge;//数据采集从第2个CLK边沿开始:1(上升沿采样输出)
				SPI_IS.SPI_CPOL 							= SPI_CPOL_High;//SPI总线空闲时刻为:1
				SPI_IS.SPI_NSS 								= SPI_NSS_Soft;//软件值CS脚的高低电平
				SPI_IS.SPI_BaudRatePrescaler	= SPI_BaudRatePrescaler_4;//10MHz
				SPI_IS.SPI_FirstBit 					= SPI_FirstBit_MSB;//MSB在前
				SPI_IS.SPI_CRCPolynomial			= 7;//CRC值计算的多项式最高位为7次
			}

			SPI_Init(SPIx_BASE[ch], &SPI_IS);

			SPI_Cmd(SPIx_BASE[ch], ENABLE);//Enable SPIx
			//SPI_SSOutputCmd(SPI_BASE, ENABLE);//开启主模式下的SS输出(非多主设备)SPI_NSS_Hard模式用.

		#if		(SPI_WORK_TYPE == 1)		//HW_SPI_DMA
			
				SPI_DMA_Config(SPI1_DMA_SETUP, SPIx_BASE[ch]->DR, 5);//SPI_DMA配置
				SPI_I2S_DMACmd(SPIx_BASE[ch], SPI_I2S_DMAReq_Rx, ENABLE);//使能SPIx_Rx_DMA请求
				SPI_I2S_DMACmd(SPIx_BASE[ch], SPI_I2S_DMAReq_Tx, ENABLE);//使能SPIx_Tx_DMA请求	
				
		#endif

		#else //未定义有效类型
			
		#endif	
	
	}

}


/**********************************************************************************************************
*	函 数 名: EEPROM_InitHard
*	功能说明: EEPROM硬件初始化
*	形    参:  无
*	返 回 值:  res
**********************************************************************************************************/
u8 EEPROM_InitHard(void)
{
	u8 res=0;
  GPIO_InitTypeDef	GPIO_IS;
	
	/*EEPROM相关SPI外设配置*/
	Spi_InitHard(SPIB, PLAN3);
	
	/*EEPROM相关GPIO配置*/
	RCC_AHB1PeriphClockCmd(EE_GPIO_CLK_CS | EE_GPIO_CLK_WP  | EE_GPIO_CLK_HOLD, ENABLE);
	GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_IS.GPIO_OType = GPIO_OType_PP;
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_IS.GPIO_Pin = EE_GPIO_PIN_CS;//CS
	GPIO_Init(EE_GPIO_PORT_CS, &GPIO_IS);

	GPIO_IS.GPIO_Pin = EE_GPIO_PIN_WP;//WP
	GPIO_Init(EE_GPIO_PORT_WP, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = EE_GPIO_PIN_HOLD;//HOLD
	GPIO_Init(EE_GPIO_PORT_HOLD, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = EE_GPIO_PIN_EE_PWR;//EEPWR
	GPIO_Init(EE_GPIO_PORT_EE_PWR, &GPIO_IS);
	
	GPIO_CTRL(EE_GPIO_CS, SET);
	GPIO_CTRL(EE_GPIO_HOLD, SET);
	GPIO_CTRL(EE_GPIO_WP, RESET);//置EEPROM写保护
//	GPIO_CTRL(EE_GPIO_EE_PWR, RESET);//EEPROM断电
	GPIO_CTRL(EE_GPIO_EE_PWR, SET);
	
	return res;
}

/***************************** hangfa (END OF FILE) *********************************/
