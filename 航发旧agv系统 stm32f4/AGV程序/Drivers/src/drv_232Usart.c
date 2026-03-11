/**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Drv_232Usart.c
** 创 建 人: dengyang
** 描    述: 232硬件接口驱动
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: 	V1.0 
** 创建日期: 	2015.03.18
** 描    述:  硬件描述层
**--------------------------------------------------------------------------------------   
**/

#define		USART232_GLOBALS

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "..\..\Drivers\inc\drv_232Usart.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
USART_TypeDef   *RS232USARTx_BASE[USARTMAXCNT]				= { UseUSART1_BASE, 				UseUSART2_BASE,					UseUSART3_BASE,					UseUSART4_BASE, 				UseUSART5_BASE,					UseUSART6_BASE};
GPIO_TypeDef    *RS232USARTx_TX_PORT[USARTMAXCNT]			= { UseUSART1_TX_GPIO_PORT,	UseUSART2_TX_GPIO_PORT,	UseUSART3_TX_GPIO_PORT,	UseUSART4_TX_GPIO_PORT,	UseUSART5_TX_GPIO_PORT,	UseUSART6_TX_GPIO_PORT};
GPIO_TypeDef    *RS232USARTx_RX_PORT[USARTMAXCNT]			= { UseUSART1_RX_GPIO_PORT,	UseUSART2_RX_GPIO_PORT,	UseUSART3_RX_GPIO_PORT,	UseUSART4_RX_GPIO_PORT,	UseUSART5_RX_GPIO_PORT,	UseUSART6_RX_GPIO_PORT};
const u32    RS232USARTx_CLK[USARTMAXCNT]					= { UseUSART1_CLK, 					UseUSART2_CLK,					UseUSART3_CLK,					UseUSART4_CLK, 					UseUSART5_CLK,					UseUSART6_CLK};
const u32    RS232USARTx_TX_PORT_CLK[USARTMAXCNT] 	= { UseUSART1_TX_GPIO_CLK, 	UseUSART2_TX_GPIO_CLK,	UseUSART3_TX_GPIO_CLK,	UseUSART4_TX_GPIO_CLK, 	UseUSART5_TX_GPIO_CLK,	UseUSART6_TX_GPIO_CLK};
const u32    RS232USARTx_RX_PORT_CLK[USARTMAXCNT]	= { UseUSART1_RX_GPIO_CLK, 	UseUSART2_RX_GPIO_CLK,	UseUSART3_RX_GPIO_CLK,	UseUSART4_RX_GPIO_CLK, 	UseUSART5_RX_GPIO_CLK,	UseUSART6_RX_GPIO_CLK};
const u32    RS232USARTx_TX_PIN[USARTMAXCNT] 			= { UseUSART1_TX_PIN, 			UseUSART2_TX_PIN,				UseUSART3_TX_PIN,				UseUSART4_TX_PIN, 			UseUSART5_TX_PIN,				UseUSART6_TX_PIN};
const u32    RS232USARTx_RX_PIN[USARTMAXCNT] 			= { UseUSART1_RX_PIN, 			UseUSART2_RX_PIN,				UseUSART3_RX_PIN,				UseUSART4_RX_PIN, 			UseUSART5_RX_PIN,				UseUSART6_RX_PIN};
const u16    RS232USARTx_TX_PIN_SOURCE[USARTMAXCNT]= { UseUSART1_TX_SOURCE, 		UseUSART2_TX_SOURCE,		UseUSART3_TX_SOURCE,		UseUSART4_TX_SOURCE, 		UseUSART5_TX_SOURCE,		UseUSART6_TX_SOURCE};
const u16    RS232USARTx_RX_PIN_SOURCE[USARTMAXCNT]= { UseUSART1_RX_SOURCE, 		UseUSART2_RX_SOURCE,		UseUSART3_RX_SOURCE,		UseUSART4_RX_SOURCE, 		UseUSART5_RX_SOURCE,		UseUSART6_RX_SOURCE};
const u16    RS232USARTx_TX_AF[USARTMAXCNT] 				= { UseUSART1_TX_AF, 				UseUSART2_TX_AF,				UseUSART3_TX_AF,				UseUSART4_TX_AF, 				UseUSART5_TX_AF,				UseUSART6_TX_AF};
const u16    RS232USARTx_RX_AF[USARTMAXCNT] 				= { UseUSART1_RX_AF, 				UseUSART2_RX_AF,				UseUSART3_RX_AF,				UseUSART4_RX_AF, 				UseUSART5_RX_AF,				UseUSART6_RX_AF};
const u8     RS232USARTx_IRQn[USARTMAXCNT]					= { UseUSART1_IRQn, 				UseUSART2_IRQn,					UseUSART3_IRQn,					UseUSART4_IRQn, 				UseUSART5_IRQn,					UseUSART6_IRQn};

u8	g_DmaSendBusy = 0;				



extern	u32		BaudList[];
/***************************************************************************************
** 函数名称: Rs232Usart_InitHard
** 功能描述: 仅初始化指定的USARTx外设用于232底层硬件接口
参数:
	ui ：初始化参数结构体指针
***************************************************************************************/
void Rs232Usart_InitHard(RS232USART_Ini *ui)
{
	NVIC_InitTypeDef			NVIC_IS;
	USART_InitTypeDef			USART_IS;
	GPIO_InitTypeDef 			GPIO_IS;	
	
	/*波特率有效判断*/
	if (ui->baud_set >= Baud_NB_MAX)
			ui->baud=BaudList[Baud_NB_MAX-1];
	else 
		ui->baud=BaudList[ui->baud_set];
	
	/*使能串口外设时钟*/
	if ( (RS232USARTx_BASE[ui->usartx] == USART1) || (RS232USARTx_BASE[ui->usartx] == USART6) )
	{
			RCC_APB2PeriphClockCmd( RS232USARTx_CLK[ui->usartx], ENABLE );
	}
	else
	{
			RCC_APB1PeriphClockCmd( RS232USARTx_CLK[ui->usartx], ENABLE );
	}	

	/*txd and rxd gpio配置*/ 
	RCC_AHB1PeriphClockCmd( RS232USARTx_TX_PORT_CLK[ui->usartx] | RS232USARTx_RX_PORT_CLK[ui->usartx], ENABLE );//使能RXD和TXD端口外设时钟
	GPIO_PinAFConfig( RS232USARTx_TX_PORT[ui->usartx], RS232USARTx_TX_PIN_SOURCE[ui->usartx], RS232USARTx_TX_AF[ui->usartx] );//连接口线到串口tx脚
	GPIO_PinAFConfig( RS232USARTx_RX_PORT[ui->usartx], RS232USARTx_RX_PIN_SOURCE[ui->usartx], RS232USARTx_RX_AF[ui->usartx] );// 连接口线到串口rx脚
	
	GPIO_IS.GPIO_Mode = GPIO_Mode_AF;// 配置串口接收脚为复用功能--tx/rx	
	GPIO_IS.GPIO_OType = GPIO_OType_PP;// 配置串口tx脚为复用功能
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_IS.GPIO_Pin = RS232USARTx_TX_PIN[ui->usartx];
	GPIO_Init( RS232USARTx_TX_PORT[ui->usartx], &GPIO_IS );

//	GPIO_IS.GPIO_Mode = GPIO_Mode_IN;
	GPIO_IS.GPIO_Pin = RS232USARTx_RX_PIN[ui->usartx];
	GPIO_Init( RS232USARTx_RX_PORT[ui->usartx], &GPIO_IS );

	USART_DeInit( RS232USARTx_BASE[ui->usartx] );
	USART_IS.USART_BaudRate = ui->baud;
	switch ((ui->comm_cfg_par) & MASK_USART_Word_Length)
	{//设置在一个帧中,传输或者接收到数据的字长数(即:数据位:8bits + 停止位:1bits)
		case USART_WL7:
			USART_IS.USART_WordLength = USART_WordLength_8b;
		break;
		case USART_WL8:
			USART_IS.USART_WordLength = USART_WordLength_9b;
		break;
		default:
			USART_IS.USART_WordLength = USART_WordLength_8b;
		break;
	}
	switch ((ui->comm_cfg_par) & MASK_USART_StopBits)
	{
		case USART_ST_0_5:
			USART_IS.USART_StopBits=USART_StopBits_0_5;
		break;
		case USART_ST_1:
			USART_IS.USART_StopBits=USART_StopBits_1;
		break;
		case USART_ST_1_5:
			USART_IS.USART_StopBits=USART_StopBits_1_5;
		break;
		case USART_ST_2:
			USART_IS.USART_StopBits=USART_StopBits_2;
		break;
		default:
			USART_IS.USART_StopBits=USART_StopBits_1;
		break;
	}
	switch ((ui->comm_cfg_par) & MASK_USART_Parity)
	{
		case USART_PI_NONE:
			USART_IS.USART_Parity = USART_Parity_No;		//无校验:NONE
		break;
		case USART_PI_EVEN:
			USART_IS.USART_Parity = USART_Parity_Even;	//偶校验:Even
		break;
		case USAAT_PI_ODD:
			USART_IS.USART_Parity = USART_Parity_Odd;		//奇校验:ODD
		break;
		default:
			USART_IS.USART_Parity = USART_Parity_No;
		break;
	}
	USART_IS.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_IS.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
	USART_Init( RS232USARTx_BASE[ui->usartx], &USART_IS );
	
	//--------------------------------------------------------------------------------------
	/*开启USARTx全局中断*/
	NVIC_IS.NVIC_IRQChannel = RS232USARTx_IRQn[ui->usartx]; 
	NVIC_IS.NVIC_IRQChannelPreemptionPriority = ui->pi;//(0-15)
	NVIC_IS.NVIC_IRQChannelSubPriority = 0;
	NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_IS);	// 初始化中断      
	
	//USART中断源配置
// 	USART_ITConfig(RS232USARTx_BASE[ui->usartx], USART_IT_TXE, ENABLE);	//发送中断使能
  USART_ITConfig(RS232USARTx_BASE[ui->usartx], USART_IT_RXNE, ENABLE);	//读USART_DR数据寄存器非空使能
// 	USART_ITConfig(RS232USARTx_BASE[ui->usartx], USART_IT_IDLE, ENABLE);	//空闲总线中断使能
 	USART_ITConfig(RS232USARTx_BASE[ui->usartx], USART_IT_PE, ENABLE);	//校验错误中断使能
 	USART_ITConfig(RS232USARTx_BASE[ui->usartx], USART_IT_FE, ENABLE);	//帧错误中断使能	
 	USART_ITConfig(RS232USARTx_BASE[ui->usartx], USART_IT_NE, ENABLE);	//噪声错误中断使能
	USART_ITConfig(RS232USARTx_BASE[ui->usartx], USART_IT_ORE_ER, ENABLE);//溢出中断
	USART_ITConfig(RS232USARTx_BASE[ui->usartx], USART_IT_ERR, ENABLE);//错误中断
// 	USART_DMACmd(RS232RS232USARTx_BASE[ui->usartx], USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); // 开启USART1的DMA请求

	USART_Cmd(RS232USARTx_BASE[ui->usartx], ENABLE);//使能USART外设	
}																							

//============================================================================
// 名称：uart_dma_send
// 功能：串口DMA初始化
// 参数：Uart：串口号
//      *pData：要发送的数据
//      Len：要发送的数据字节数
// 返回：无
// 说明：无
//============================================================================
void uart_dma_send(USART_ID Uart, u8 *pData, u32 Len)
{
		DMA_InitTypeDef	DMA_InitStructure;
	
    if ( RS232USARTx_BASE[Uart] == USART1 )
    {
        // 发送DMA：DMA2_Stream7_Channel4
        // 配置DMA结构
        DMA_InitStructure.DMA_BufferSize = Len;
        DMA_InitStructure.DMA_PeripheralBaseAddr = ( uint32_t )(&(USART1->DR));
        DMA_InitStructure.DMA_Channel = DMA_Channel_4;
        DMA_InitStructure.DMA_Memory0BaseAddr = ( uint32_t )pData;
        DMA_Init( DMA2_Stream7, &DMA_InitStructure );

        // 使能DMA发送请求
        USART_DMACmd( USART1, USART_DMAReq_Tx, ENABLE );
        // 开始一次DMA传输
        DMA_ITConfig( DMA2_Stream7, DMA_IT_TC, ENABLE );
        DMA_Cmd( DMA2_Stream7, ENABLE );
        g_DmaSendBusy = 1;
    }
    else if ( RS232USARTx_BASE[Uart] == USART2 )
    {
        // 发送DMA：DMA1_Stream6_Channel4
        // 配置DMA结构
        DMA_InitStructure.DMA_BufferSize = Len;
        DMA_InitStructure.DMA_PeripheralBaseAddr = ( uint32_t )(&(USART2->DR));
        DMA_InitStructure.DMA_Channel = DMA_Channel_4;
        DMA_InitStructure.DMA_Memory0BaseAddr = ( uint32_t )pData;
        DMA_Init( DMA1_Stream6, &DMA_InitStructure );

        // 使能DMA发送请求
        USART_DMACmd( USART2, USART_DMAReq_Tx, ENABLE );
        // 开始一次DMA传输
        DMA_ITConfig( DMA1_Stream6, DMA_IT_TC, ENABLE );
        DMA_Cmd( DMA1_Stream6, ENABLE );
        g_DmaSendBusy = 1;
    }
    else if ( RS232USARTx_BASE[Uart] == USART3 )
    {
        // 发送DMA：DMA1_Stream3_Channel4
        // 配置DMA结构
        DMA_InitStructure.DMA_BufferSize = Len;
        DMA_InitStructure.DMA_PeripheralBaseAddr = ( uint32_t )(&(USART3->DR));
        DMA_InitStructure.DMA_Channel = DMA_Channel_4;
        DMA_InitStructure.DMA_Memory0BaseAddr = ( uint32_t )pData;
        DMA_Init( DMA1_Stream3, &DMA_InitStructure );

        // 使能DMA发送请求
        USART_DMACmd( USART3, USART_DMAReq_Tx, ENABLE );
        // 开始一次DMA传输
        DMA_ITConfig( DMA1_Stream3, DMA_IT_TC, ENABLE );
        DMA_Cmd( DMA1_Stream3, ENABLE );
        g_DmaSendBusy = 1;
    }
    else if ( RS232USARTx_BASE[Uart] == UART4 )
    {
        // 发送DMA：DMA1_Stream4_Channel4
        // 配置DMA结构
        DMA_InitStructure.DMA_BufferSize = Len;
        DMA_InitStructure.DMA_PeripheralBaseAddr = ( uint32_t )(&(UART4->DR));
        DMA_InitStructure.DMA_Channel = DMA_Channel_4;
        DMA_InitStructure.DMA_Memory0BaseAddr = ( uint32_t )pData;
        DMA_Init( DMA1_Stream4, &DMA_InitStructure );

        // 使能DMA发送请求
        USART_DMACmd( UART4, USART_DMAReq_Tx, ENABLE );
        // 开始一次DMA传输
        DMA_ITConfig( DMA1_Stream4, DMA_IT_TC, ENABLE );
        DMA_Cmd( DMA1_Stream4, ENABLE );
        g_DmaSendBusy = 1;
    }
    else if ( RS232USARTx_BASE[Uart] == UART5 )
    {
        // 发送DMA：DMA1_Stream7_Channel4
        // 配置DMA结构
        DMA_InitStructure.DMA_BufferSize = Len;
        DMA_InitStructure.DMA_PeripheralBaseAddr = ( uint32_t )(&(UART5->DR));
        DMA_InitStructure.DMA_Channel = DMA_Channel_4;
        DMA_InitStructure.DMA_Memory0BaseAddr = ( uint32_t )pData;
        DMA_Init( DMA1_Stream7, &DMA_InitStructure );

        // 使能DMA发送请求
        USART_DMACmd( UART5, USART_DMAReq_Tx, ENABLE );
        // 开始一次DMA传输
        DMA_ITConfig( DMA1_Stream7, DMA_IT_TC, ENABLE );
        DMA_Cmd( DMA1_Stream7, ENABLE );

        g_DmaSendBusy = 1;
    }
    else if ( RS232USARTx_BASE[Uart] == USART6 )
    {
        // 发送DMA：DMA2_Stream6_Channel5
        // 配置DMA结构
        DMA_InitStructure.DMA_BufferSize = Len;
        DMA_InitStructure.DMA_PeripheralBaseAddr = ( uint32_t )(&(USART6->DR));
        DMA_InitStructure.DMA_Channel = DMA_Channel_5;
        DMA_InitStructure.DMA_Memory0BaseAddr = ( uint32_t )pData;
        DMA_Init( DMA2_Stream6, &DMA_InitStructure );

        // 使能DMA发送请求
        USART_DMACmd( USART6, USART_DMAReq_Tx, ENABLE );
        // 开始一次DMA传输
        DMA_ITConfig( DMA2_Stream6, DMA_IT_TC, ENABLE );
        DMA_Cmd( DMA2_Stream6, ENABLE );
        g_DmaSendBusy = 1;
    }
}


/*
*********************************************************************************************************
*	函 数 名: bsp_UsartSendStr
*	功能说明: 串口发送字符串
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_UsartSendStr(USART_ID Uart,uint8_t *str,uint8_t len)
{
	while(len--)
	{
		USART_SendData(RS232USARTx_BASE[Uart],*str);
		while (USART_GetFlagStatus(RS232USARTx_BASE[Uart], USART_FLAG_TXE) == RESET);	/* 等待发送结束 */
		str++;
	}
}

/***************************************************************************************
** 函数名称: Rs232_usart_tm_1ms
** 功能描述: 232接口数据包判断
** 参    数: ST232_USART结构体指针
** 返 回 值: None
****************************************************************************************/
void Rs232_usart_tm_1ms(ST232_USART *su)
{
    if((su->TimeOutVal--) >= 1)
    {//启动超时判断....
        if(!(su->TimeOutVal))
        {//收到一包数据,将再次来的数据包放入下一个FIFO队列
            if(++su->Rx_fifo.wr_addr >= UART_RXTX_FIFO_SIZE)
                su->Rx_fifo.wr_addr =0;
            
            su->Rx_fifo.Msg[su->Rx_fifo.wr_addr].Len=0;
        }
    }
    else if(su->TimeOutVal < 0)
    {
        su->TimeOutVal = 0;
    }
}

/***************************************************************************************
** 函数名称: Rs232_usart_isr
** 功能描述: 232接口指针
** 参    数: su
** 返 回 值: None
****************************************************************************************/
void Rs232_usart_isr(ST232_USART *su)
{
	if((RS232USARTx_BASE[su->Ini_par.usartx]->SR) & (1 << 5))
	{//usartX收到数据可以读
		RS232USARTx_BASE[su->Ini_par.usartx]->SR = (u16)(~(1 << 5));// Clear RXNE

		su->TimeOutVal=UART_RECEIVE_OVERTIME_MAX;

		su->Rx_fifo.Msg[su->Rx_fifo.wr_addr].Data[su->Rx_fifo.Msg[su->Rx_fifo.wr_addr].Len]
		=(u16)RS232USARTx_BASE[su->Ini_par.usartx]->DR;
		
		if(++su->Rx_fifo.Msg[su->Rx_fifo.wr_addr].Len >= UART_MSG_LENGTH_MAX)
				su->Rx_fifo.Msg[su->Rx_fifo.wr_addr].Len=0;
	}
}

///***************************************************************************************
//** 函数名称: RS232A_IRQHandler()
//** 功能描述: 232接收中断函数1
//** 参    数: None
//** 返 回 值: None
//****************************************************************************************/
//void RS232A_IRQHandler(void)
//{
//		Rs232_usart_isr(&RS232_USARTx[RS232A_ID]);
//	
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232A_ID], USART_IT_TXE) == SET) 				
//		{//Tansmit Data Register empty interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232A_ID], USART_IT_TXE);
//			//USART_ITConfig(RS232USARTx_BASE[RS232A_ID], USART_IT_TXE,  DISABLE);	
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232A_ID], USART_IT_TC) == SET) 	
//		{//Transmission complete interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232A_ID], USART_IT_TC);
//			//USART_ITConfig(RS232USARTx_BASE[RS232A_ID], USART_IT_TC,  DISABLE);			
//		}
//		if(USART_GetFlagStatus(RS232USARTx_BASE[RS232A_ID], USART_IT_ORE) == SET) 	
//		{//OverRun Error溢出-如果发生溢出需要先读SR,再读DR寄存器 则可清除不断入中断的问题
//			USART_ClearFlag(RS232USARTx_BASE[RS232A_ID], USART_IT_ORE);		
//			RS232USARTx_BASE[RS232A_ID]->DR;
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232A_ID], USART_IT_PE) == SET) 	
//		{//Parity Error interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232A_ID], USART_IT_PE);		
//		}	
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232A_ID], USART_IT_FE) == SET) 	
//		{//Framing Error interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232A_ID], USART_IT_FE);		
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232A_ID], USART_IT_NE) == SET) 	
//		{//Noise Error interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232A_ID], USART_IT_NE);		
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232A_ID], USART_IT_IDLE) == SET) 	
//		{//USART IDLE
//		USART_ClearITPendingBit(RS232USARTx_BASE[RS232A_ID], USART_IT_IDLE);
//		}	
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232A_ID], USART_IT_LBD) == SET) 	
//		{//USART LBD
//		USART_ClearITPendingBit(RS232USARTx_BASE[RS232A_ID], USART_IT_LBD);
//		}	
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232A_ID], USART_IT_CTS) == SET) 	
//		{//USART CTS
//		USART_ClearITPendingBit(RS232USARTx_BASE[RS232A_ID], USART_IT_CTS);
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232A_ID], USART_IT_ERR) == SET) 	
//		{//USART ERR
//		USART_ClearITPendingBit(RS232USARTx_BASE[RS232A_ID], USART_IT_ERR);
//		}
//}

///***************************************************************************************
//** 函数名称: RS232B_IRQHandler()
//** 功能描述: 232接收中断函数2
//** 参    数: None
//** 返 回 值: None
//****************************************************************************************/
//void RS232B_IRQHandler(void)
//{
//		Rs232_usart_isr(&RS232_USARTx[RS232B_ID]);
//	
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232B_ID], USART_IT_TXE) == SET) 				
//		{//Tansmit Data Register empty interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232B_ID], USART_IT_TXE);
//			USART_ITConfig(RS232USARTx_BASE[RS232B_ID], USART_IT_TXE,  DISABLE);	
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232B_ID], USART_IT_TC) == SET) 	
//		{//Transmission complete interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232B_ID], USART_IT_TC);
//			USART_ITConfig(RS232USARTx_BASE[RS232B_ID], USART_IT_TC,  DISABLE);			
//		}
//		if(USART_GetFlagStatus(RS232USARTx_BASE[RS232B_ID], USART_IT_ORE) == SET) 	
//		{//OverRun Error interrupt
//			USART_ClearFlag(RS232USARTx_BASE[RS232B_ID], USART_IT_ORE);		
//			RS232USARTx_BASE[RS232B_ID]->DR;
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232B_ID], USART_IT_PE) == SET) 	
//		{//Parity Error interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232B_ID], USART_IT_PE);		
//		}	
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232B_ID], USART_IT_FE) == SET) 	
//		{//Framing Error interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232B_ID], USART_IT_FE);		
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232B_ID], USART_IT_NE) == SET) 	
//		{//Noise Error interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232B_ID], USART_IT_NE);		
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232B_ID], USART_IT_IDLE) == SET) 	
//		{//USART IDLE
//		USART_ClearITPendingBit(RS232USARTx_BASE[RS232B_ID], USART_IT_IDLE);
//		}	
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232B_ID], USART_IT_LBD) == SET) 	
//		{//USART LBD
//		USART_ClearITPendingBit(RS232USARTx_BASE[RS232B_ID], USART_IT_LBD);
//		}	
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232B_ID], USART_IT_CTS) == SET) 	
//		{//USART CTS
//		USART_ClearITPendingBit(RS232USARTx_BASE[RS232B_ID], USART_IT_CTS);
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232B_ID], USART_IT_ERR) == SET) 	
//		{//USART ERR
//		USART_ClearITPendingBit(RS232USARTx_BASE[RS232B_ID], USART_IT_ERR);
//		}
//}

/***************************************************************************************
** 函数名称: RS232C_IRQHandler()
** 功能描述: 232接收中断函数3
** 参    数: None
** 返 回 值: None
****************************************************************************************/
//void RS232C_IRQHandler(void)
//{
//		Rs232_usart_isr(&RS232_USARTx[RS232C_ID]);
//	
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232C_ID], USART_IT_TXE) == SET) 				
//		{//Tansmit Data Register empty interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232C_ID], USART_IT_TXE);
//			USART_ITConfig(RS232USARTx_BASE[RS232C_ID], USART_IT_TXE,  DISABLE);	
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232C_ID], USART_IT_TC) == SET) 	
//		{//Transmission complete interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232C_ID], USART_IT_TC);
//			USART_ITConfig(RS232USARTx_BASE[RS232C_ID], USART_IT_TC,  DISABLE);			
//		}
//		if(USART_GetFlagStatus(RS232USARTx_BASE[RS232C_ID], USART_IT_ORE) == SET) 	
//		{//OverRun Error interrupt
//			USART_ClearFlag(RS232USARTx_BASE[RS232C_ID], USART_IT_ORE);		
//			RS232USARTx_BASE[RS232C_ID]->DR;
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232C_ID], USART_IT_PE) == SET) 	
//		{//Parity Error interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232C_ID], USART_IT_PE);		
//		}	
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232C_ID], USART_IT_FE) == SET) 	
//		{//Framing Error interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232C_ID], USART_IT_FE);		
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232C_ID], USART_IT_NE) == SET) 	
//		{//Noise Error interrupt
//			USART_ClearITPendingBit(RS232USARTx_BASE[RS232C_ID], USART_IT_NE);		
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232C_ID], USART_IT_IDLE) == SET) 	
//		{//USART IDLE
//		USART_ClearITPendingBit(RS232USARTx_BASE[RS232C_ID], USART_IT_IDLE);
//		}	
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232C_ID], USART_IT_LBD) == SET) 	
//		{//USART LBD
//		USART_ClearITPendingBit(RS232USARTx_BASE[RS232C_ID], USART_IT_LBD);
//		}	
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232C_ID], USART_IT_CTS) == SET) 	
//		{//USART CTS
//		USART_ClearITPendingBit(RS232USARTx_BASE[RS232C_ID], USART_IT_CTS);
//		}
//		if(USART_GetITStatus(RS232USARTx_BASE[RS232C_ID], USART_IT_ERR) == SET) 	
//		{//USART ERR
//		USART_ClearITPendingBit(RS232USARTx_BASE[RS232C_ID], USART_IT_ERR);
//		}
//}








/************************************END OF FILE*****************************************/
