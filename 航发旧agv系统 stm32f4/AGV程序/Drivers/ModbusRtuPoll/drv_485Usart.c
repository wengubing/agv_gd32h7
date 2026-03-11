/**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Drv_485Usart.c
** 创 建 人: dengyang
** 描    述: 485硬件接口驱动
**------------- 版本信息 ---------------------------------------------------------------
** 版    本:  V1.1
** 创建日期:  2015.03.18
** 描    述:  硬件描述层
**--------------------------------------------------------------------------------------   
**/

#define  USART485_GLOBALS

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "..\..\Drivers\ModbusRtuPoll\drv_485Usart.h"

#include "ACC_Project.h"
extern u32 g_Master4_Scomm_Delay_cnt;

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

USART_TypeDef   *RS485USARTx_BASE[USARTMAXCNT]    = { UseUSART1_BASE,     UseUSART2_BASE,     UseUSART3_BASE,     UseUSART4_BASE,     UseUSART5_BASE,     UseUSART6_BASE};
GPIO_TypeDef    *RS485USARTx_TX_PORT[USARTMAXCNT]   = { UseUSART1_TX_GPIO_PORT, UseUSART2_TX_GPIO_PORT, UseUSART3_TX_GPIO_PORT, UseUSART4_TX_GPIO_PORT, UseUSART5_TX_GPIO_PORT, UseUSART6_TX_GPIO_PORT};
GPIO_TypeDef    *RS485USARTx_RX_PORT[USARTMAXCNT]   = { UseUSART1_RX_GPIO_PORT, UseUSART2_RX_GPIO_PORT, UseUSART3_RX_GPIO_PORT, UseUSART4_RX_GPIO_PORT, UseUSART5_RX_GPIO_PORT, UseUSART6_RX_GPIO_PORT};
const u32    RS485USARTx_CLK[USARTMAXCNT]     = { UseUSART1_CLK,      UseUSART2_CLK,     UseUSART3_CLK,     UseUSART4_CLK,      UseUSART5_CLK,     UseUSART6_CLK};
const u32    RS485USARTx_TX_PORT_CLK[USARTMAXCNT]  = { UseUSART1_TX_GPIO_CLK,  UseUSART2_TX_GPIO_CLK, UseUSART3_TX_GPIO_CLK, UseUSART4_TX_GPIO_CLK,  UseUSART5_TX_GPIO_CLK, UseUSART6_TX_GPIO_CLK};
const u32    RS485USARTx_RX_PORT_CLK[USARTMAXCNT] = { UseUSART1_RX_GPIO_CLK,  UseUSART2_RX_GPIO_CLK, UseUSART3_RX_GPIO_CLK, UseUSART4_RX_GPIO_CLK,  UseUSART5_RX_GPIO_CLK, UseUSART6_RX_GPIO_CLK};
const u32    RS485USARTx_TX_PIN[USARTMAXCNT]    = { UseUSART1_TX_PIN,    UseUSART2_TX_PIN,    UseUSART3_TX_PIN,    UseUSART4_TX_PIN,    UseUSART5_TX_PIN,    UseUSART6_TX_PIN};
const u32    RS485USARTx_RX_PIN[USARTMAXCNT]    = { UseUSART1_RX_PIN,    UseUSART2_RX_PIN,    UseUSART3_RX_PIN,    UseUSART4_RX_PIN,    UseUSART5_RX_PIN,    UseUSART6_RX_PIN};
const u16    RS485USARTx_TX_PIN_SOURCE[USARTMAXCNT]= { UseUSART1_TX_SOURCE,   UseUSART2_TX_SOURCE,  UseUSART3_TX_SOURCE,  UseUSART4_TX_SOURCE,   UseUSART5_TX_SOURCE,  UseUSART6_TX_SOURCE};
const u16    RS485USARTx_RX_PIN_SOURCE[USARTMAXCNT]= { UseUSART1_RX_SOURCE,   UseUSART2_RX_SOURCE,  UseUSART3_RX_SOURCE,  UseUSART4_RX_SOURCE,   UseUSART5_RX_SOURCE,  UseUSART6_RX_SOURCE};
const u16    RS485USARTx_TX_AF[USARTMAXCNT]     = { UseUSART1_TX_AF,     UseUSART2_TX_AF,    UseUSART3_TX_AF,    UseUSART4_TX_AF,     UseUSART5_TX_AF,    UseUSART6_TX_AF};
const u16    RS485USARTx_RX_AF[USARTMAXCNT]     = { UseUSART1_RX_AF,     UseUSART2_RX_AF,    UseUSART3_RX_AF,    UseUSART4_RX_AF,     UseUSART5_RX_AF,    UseUSART6_RX_AF};
const u8     RS485USARTx_IRQn[USARTMAXCNT]     = { UseUSART1_IRQn,     UseUSART2_IRQn,     UseUSART3_IRQn,     UseUSART4_IRQn,     UseUSART5_IRQn,     UseUSART6_IRQn};
const u32    RS485RDE_CLK[USARTMAXCNT]       = { UseUSART1_485_CLK,   UseUSART2_485_CLK,   UseUSART3_485_CLK,   UseUSART4_485_CLK,   UseUSART5_485_CLK,   UseUSART6_485_CLK};
GPIO_TypeDef    *RS485RDE_PORT[USARTMAXCNT]      = { UseUSART1_485_PORT,   UseUSART2_485_PORT,   UseUSART3_485_PORT,   UseUSART4_485_PORT,   UseUSART5_485_PORT,   UseUSART6_485_PORT};
const u32  RS485RDE_Pin[USARTMAXCNT]       = { UseUSART1_485_Pin,   UseUSART2_485_Pin,   UseUSART3_485_Pin,   UseUSART4_485_Pin,   UseUSART5_485_Pin,   UseUSART6_485_Pin};

#if (USART485_MODE==1)

const u8 	 RS485USART_DMAxTx_Streamx_IRQn[USARTMAXCNT]  = {UseUSART1_Tx_DMAx_Streamx_IRQn, UseUSART2_Tx_DMAx_Streamx_IRQn, UseUSART3_Tx_DMAx_Streamx_IRQn, UseUSART4_Tx_DMAx_Streamx_IRQn, UseUSART5_Tx_DMAx_Streamx_IRQn, UseUSART6_Tx_DMAx_Streamx_IRQn};
DMA_Stream_TypeDef		*RS485USART_DMAxTx_Streamx[USARTMAXCNT]  = {UseUSART1_Tx_DMAx_Streamx, UseUSART2_Tx_DMAx_Streamx, UseUSART3_Tx_DMAx_Streamx, UseUSART4_Tx_DMAx_Streamx, UseUSART5_Tx_DMAx_Streamx, UseUSART6_Tx_DMAx_Streamx};
const u32 	 RS485USART_DMA_Channel[USARTMAXCNT]  = {UseUSART1_Tx_DMA_Channel, UseUSART2_Tx_DMA_Channel, UseUSART3_Tx_DMA_Channel, UseUSART4_Tx_DMA_Channel, UseUSART5_Tx_DMA_Channel, UseUSART6_Tx_DMA_Channel};
const u32 	 RS485USART_DMA_IT_TCIFx[USARTMAXCNT]  = {UseUSART1_Tx_DMA_IT_TCIFx, UseUSART2_Tx_DMA_IT_TCIFx, UseUSART3_Tx_DMA_IT_TCIFx, UseUSART4_Tx_DMA_IT_TCIFx, UseUSART5_Tx_DMA_IT_TCIFx, UseUSART6_Tx_DMA_IT_TCIFx};

#endif

u32 T35_Cnt[]={
 T35_RB1200_cnt,
 T35_RB2400_cnt,
 T35_RB4800_cnt,
 T35_RB9600_cnt,
 T35_RB19200_cnt,
 T35_RB38400_cnt,
 T35_RB57600_cnt,
 T35_RB115200_cnt
};

u32 BaudList[]={
 1200,
 2400,
 4800,
 9600,
 19200,
 38400,
 57600,
 115200,
 256000
};


u32 T35_IT_VAL[]={
 64, //1200
 32, //2400
 16, //4800
 10, //9600
 8,  //19200
 6,  //38400
 4,  //57600
 2  //115200
};


/***************************************************************************************
** 函数名称: Baud_brr_cal()
** 功能描述: 波特率值计算
** 参    数: bound:波特率值
** 返 回 值: USARTX_BRR_data
****************************************************************************************/
u16 Baud_brr_cal(u32 bound)
{
 f32 temp = 0;
 u16 mantissa,       //USART_DIV的整数部分
   fraction,       //USART_DIV的小数部分
   pclk2 = 36,       //APB2时钟---36MHZ
   USARTX_BRR_data = 0; //将设置的波特比率的BRR值
 
 temp = (f32)((f32)(pclk2 * 1000000) / (f32)(bound * 16));
 mantissa = temp;//取出整数部分
 fraction = ((temp - mantissa) * 16.0f) + 0.5f;
 mantissa <<= 4;
 //mantissa += fraction;
 USARTX_BRR_data = (mantissa + fraction);
 return(USARTX_BRR_data);
}

/***************************************************************************************
** 函数名称: Rs485_Usart_InitHard
** 功能描述: 仅初始化指定的USARTx外设用于485底层硬件接口
参数:
 ui ：初始化参数结构体指针
***************************************************************************************/
void Rs485_Usart_InitHard(RS485USART_Ini *ui)
{
 NVIC_InitTypeDef   NVIC_IS;
 USART_InitTypeDef   USART_IS;
 GPIO_InitTypeDef    GPIO_IS;

 /*波特率有效判断*/
 if (BaudList[ui->baud_set]>=BaudList[Baud_NB_MAX])
   ui->baud=BaudList[Baud_NB_MAX-1];
 else
   ui->baud=BaudList[ui->baud_set];
 
 /*使能串口外设时钟*/
 if ( (RS485USARTx_BASE[ui->usartx] == USART1) || (RS485USARTx_BASE[ui->usartx] == USART6) )
 {
   RCC_APB2PeriphClockCmd( RS485USARTx_CLK[ui->usartx], ENABLE );
 }
 else
 {
   RCC_APB1PeriphClockCmd( RS485USARTx_CLK[ui->usartx], ENABLE );
 } 

 /*txd and rxd gpio配置*/ 
 RCC_AHB1PeriphClockCmd( RS485USARTx_TX_PORT_CLK[ui->usartx] | RS485USARTx_RX_PORT_CLK[ui->usartx], ENABLE );//使能RXD和TXD端口外设时钟
 GPIO_PinAFConfig( RS485USARTx_TX_PORT[ui->usartx], RS485USARTx_TX_PIN_SOURCE[ui->usartx], RS485USARTx_TX_AF[ui->usartx] );//连接口线到串口tx脚
 GPIO_PinAFConfig( RS485USARTx_RX_PORT[ui->usartx], RS485USARTx_RX_PIN_SOURCE[ui->usartx], RS485USARTx_RX_AF[ui->usartx] );// 连接口线到串口rx脚
 
 GPIO_IS.GPIO_Mode = GPIO_Mode_AF;// 配置串口接收脚为复用功能--tx/rx 
 GPIO_IS.GPIO_OType = GPIO_OType_PP;// 配置串口tx脚为复用功能
 GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz; 
 
 GPIO_IS.GPIO_Pin = RS485USARTx_TX_PIN[ui->usartx];
 GPIO_Init( RS485USARTx_TX_PORT[ui->usartx], &GPIO_IS );

 GPIO_IS.GPIO_Pin = RS485USARTx_RX_PIN[ui->usartx];
 GPIO_Init( RS485USARTx_RX_PORT[ui->usartx], &GPIO_IS );

 USART_DeInit( RS485USARTx_BASE[ui->usartx] );
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
   USART_IS.USART_Parity = USART_Parity_No;  //无校验:NONE
  break;
  case USART_PI_EVEN:
   USART_IS.USART_Parity = USART_Parity_Even; //偶校验:Even
  break;
  case USAAT_PI_ODD:
   USART_IS.USART_Parity = USART_Parity_Odd;  //奇校验:ODD
  break;
  default:
   USART_IS.USART_Parity = USART_Parity_No;
  break;
 }
 USART_IS.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
 USART_IS.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
 USART_Init( RS485USARTx_BASE[ui->usartx], &USART_IS );
  
 if(ui->workmode==RS485_Master || ui->workmode==RS485_Slave)
 {//硬件接口为485工作方式
  if(RS485RDE_Pin[ui->usartx] != 0 && RS485RDE_CLK[ui->usartx] != 0)
  {//RS485有硬件方向控制引脚
   RCC_AHB1PeriphClockCmd(RS485RDE_CLK[ui->usartx], ENABLE);   
   GPIO_IS.GPIO_OType = GPIO_OType_PP;
   GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_IS.GPIO_Pin = RS485RDE_Pin[ui->usartx];//---RS485DE
   GPIO_Init(RS485RDE_PORT[ui->usartx], &GPIO_IS);
  }

  /*如果 MODBUS从机通讯不稳定，找不到原因，可能是3.5ms断帧处的等待时间不合适，可以将useUART3 改成需要的断开试试*/
  // 宁德时代BMS特殊处理
  if (g_Para.EQPCommSource.EN.BMS_CATL == 1 && 
	  g_Para.EQPCommSource.CN.BMS_CATL == ui->usartx)
  {
    ui->T35_val=T35_Cnt[ui->baud_set]*200;     
  }
  else
  {
    ui->T35_val=T35_Cnt[ui->baud_set];    
  }	
  if(ui->workmode==RS485_Master)
  {
   ui->T35_IT = T35_IT_VAL[ui->baud_set];
  }
 }
 
 //--------------------------------------------------------------------------------------
 /*开启USARTx全局中断*/
 NVIC_IS.NVIC_IRQChannel = RS485USARTx_IRQn[ui->usartx]; 
 NVIC_IS.NVIC_IRQChannelPreemptionPriority = ui->pi;//(0-15)
 NVIC_IS.NVIC_IRQChannelSubPriority = 0;
 NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_IS); // 初始化中断  
 
 //USART中断源配置
	USART_ITConfig(RS485USARTx_BASE[ui->usartx], USART_IT_RXNE, ENABLE); //读USART_DR数据寄存器非空使能
	USART_ITConfig(RS485USARTx_BASE[ui->usartx], USART_IT_PE, ENABLE); //校验错误中断使能
	USART_ITConfig(RS485USARTx_BASE[ui->usartx], USART_IT_FE, ENABLE); //帧错误中断使能 
	USART_ITConfig(RS485USARTx_BASE[ui->usartx], USART_IT_NE, ENABLE); //噪声错误中断使能
	USART_ITConfig(RS485USARTx_BASE[ui->usartx], USART_IT_ORE_ER, ENABLE);
	USART_ITConfig(RS485USARTx_BASE[ui->usartx], USART_IT_ERR, ENABLE);
 
	USART_Cmd(RS485USARTx_BASE[ui->usartx], ENABLE);//使能USART外设 
	
	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去,如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(RS485USARTx_BASE[ui->usartx], USART_FLAG_TC);     /* 清发送外城标志，Transmission Complete flag */
	
 
#if (USART485_MODE==0)

#elif (USART485_MODE==1)//USARTx_DMA发送中断设置  
	 {
		u32 Timeout = 10000;
		DMA_InitTypeDef DMA_IS;
	 
		if(ui->usartx == useUART2 || ui->usartx == useUART3 || ui->usartx == useUART4 || ui->usartx == useUART5)
		{
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  //启动DMA1时钟 
		}
		else if(ui->usartx == useUART1 || ui->usartx == useUART6)
		{
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  //启动DMA2时钟 
		}

		NVIC_IS.NVIC_IRQChannel = RS485USART_DMAxTx_Streamx_IRQn[ui->usartx];  
		NVIC_IS.NVIC_IRQChannelPreemptionPriority = ui->pi + 1;  
		NVIC_IS.NVIC_IRQChannelSubPriority = 0;  
		NVIC_IS.NVIC_IRQChannelCmd = ENABLE;  
		NVIC_Init(&NVIC_IS);  

		DMA_DeInit(RS485USART_DMAxTx_Streamx[ui->usartx]); 
		DMA_IS.DMA_Channel = RS485USART_DMA_Channel[ui->usartx];//请求外设通道   
		DMA_IS.DMA_PeripheralBaseAddr = (u32)(&RS485USARTx_BASE[ui->usartx]->DR);  //外设地址 xxx
		DMA_IS.DMA_Memory0BaseAddr = (u32)(RS485_USARTx[ui->usartx].tr_buf);//指定内存地址xxx
		DMA_IS.DMA_DIR = DMA_DIR_MemoryToPeripheral;  //设置DMA在传输时缓冲区的长度  
		DMA_IS.DMA_BufferSize = 0;//暂定为空,在后发送前设置 xxx
		DMA_IS.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//设置DMA的外设递增模式
		DMA_IS.DMA_MemoryInc = DMA_MemoryInc_Enable; //设置DMA的内存递增模式 
		DMA_IS.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据字长  
		DMA_IS.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  //内存数据字长
		DMA_IS.DMA_Mode = DMA_Mode_Normal;  //设置DMA的传输模式
		DMA_IS.DMA_Priority = DMA_Priority_Medium; //设置DMA的优先级别:中 
		DMA_IS.DMA_FIFOMode = DMA_FIFOMode_Disable;//指定如果FIFO模式或直接模式将用于指定的流:不使能FIFO模式
		DMA_IS.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;//指定了FIFO阈值水平           
		DMA_IS.DMA_MemoryBurst = DMA_MemoryBurst_Single;//指定的Burst转移配置内存传输            
		DMA_IS.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//指定的Burst转移配置外围转移
		DMA_Init(RS485USART_DMAxTx_Streamx[ui->usartx], &DMA_IS); //配置DMAx的通道
		
		while ((DMA_GetCmdStatus(RS485USART_DMAxTx_Streamx[ui->usartx]) != ENABLE) && (Timeout-- > 0));
		if (!Timeout)
		{//超时
			while (1)
			{
			}
		}
	}

	USART_DMACmd(RS485USARTx_BASE[ui->usartx],USART_DMAReq_Tx,ENABLE);  //使能DMA_TX方式发送
	
	DMA_ClearFlag(RS485USART_DMAxTx_Streamx[ui->usartx],DMA_IT_TC);
	DMA_ClearFlag(RS485USART_DMAxTx_Streamx[ui->usartx],DMA_IT_TE);
	
	DMA_ITConfig(RS485USART_DMAxTx_Streamx[ui->usartx], DMA_IT_TC, DISABLE); //关闭DMA_TC中断
	DMA_Cmd(RS485USART_DMAxTx_Streamx[ui->usartx],DISABLE);//关闭DMA,开始发送
	
#endif

}                       

/***************************************************************************************
** 函数名称: RS485_RDE_Contrl
** 功能描述: 485芯片发送接收方向控制
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void RS485_RDE_Contrl(USART_ID Ux, unsigned char sr)
{
 if(sr == send_dat)
 {//485发送控制
  GPIO_SetBits(RS485RDE_PORT[Ux], RS485RDE_Pin[Ux]);      
 }
 else if(sr == recv_dat)
 {//485接收控制
  GPIO_ResetBits(RS485RDE_PORT[Ux], RS485RDE_Pin[Ux]);  
 }
}

/***************************************************************************************
** 函数名称: Rs485_usart_tx_rx_rde
** 功能描述: 打开/关闭usartx接收功能
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void Rs485_usart_tx_rx_rde(USART_ID Usartx, unsigned char on_off_sta)
{
  if(on_off_sta == ON)
  {//接收打开!!!
   if(RS485RDE_Pin[Usartx] != 0)
   {
    RS485_RDE_Contrl(Usartx, recv_dat);//使能485接收rx1
   }
   USART_ITConfig(RS485USARTx_BASE[Usartx], USART_IT_RXNE, ENABLE);//接收中断on

  }
  else if(on_off_sta == OFF)
  {//接收关闭!!!
   if(RS485RDE_Pin[Usartx] != 0)
   {
    RS485_RDE_Contrl(Usartx, send_dat);//使能485发送tx1
   }
   USART_ITConfig(RS485USARTx_BASE[Usartx], USART_IT_RXNE, DISABLE);//接收中断off 
  }
}


/***************************************************************************************
** 函数名称: Rs485_usart_isr
** 功能描述: 硬件接口指针
** 参    数: su
** 返 回 值: None
****************************************************************************************/
void Rs485_usart_isr(ST485_USART *su)
{
  if((RS485USARTx_BASE[su->Ini_par.usartx]->SR) & (1 << 5))
  {//usart1收到数据可以读
   RS485USARTx_BASE[su->Ini_par.usartx]->SR = (u16)(~(1 << 5));// Clear RXNE
   su -> jfc = su->Ini_par.T35_val;
      
   if(su -> rx_nb >= MB_TRBUFFER_LENGTH)    
       su -> rx_nb --;//溢出保护
   
   su -> sta = (0x01 << B_USART_REC_1BYTE);//置收到1字节数据
   su -> tr_buf[su -> rx_nb ++] = (u16)(RS485USARTx_BASE[su->Ini_par.usartx]->DR);
  }
}

/***************************************************************************************
** 函数名称: Drv_Rs485_send_msg
** 功能描述: 485_usart消息包发送函数
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void  Drv_Rs485_send_msg(USART_ID Usartx, u8 *pdata, unsigned short len)
{
#if (USART485_MODE==0)

	u8 i;
	for(i = 0; i < len; )
	{       
		RS485USARTx_BASE[Usartx]->DR = pdata[i++];
		while(USART_GetFlagStatus(RS485USARTx_BASE[Usartx], USART_FLAG_TXE) == RESET);//USART_FLAG_TXE、
	}
	while(USART_GetFlagStatus(RS485USARTx_BASE[Usartx], USART_FLAG_TC) == RESET);//等待发送完成
	USART_ITConfig(RS485USARTx_BASE[Usartx], USART_IT_TXE, DISABLE);//关闭发送空
	USART_ITConfig(RS485USARTx_BASE[Usartx], USART_IT_TC,  ENABLE);//开启发送完成irq_tc1
	
#elif (USART485_MODE==1)
	
	USART_ClearFlag(RS485USARTx_BASE[Usartx], USART_FLAG_TC);//解决后面2个字节发送不完整问题(也可同时解决首包数据第1个字节发不出的问题)!!!
	DMA_ClearFlag(RS485USART_DMAxTx_Streamx[Usartx], RS485USART_DMA_IT_TCIFx[Usartx]);//清DMA发送完成标志
	DMA_ClearITPendingBit(RS485USART_DMAxTx_Streamx[Usartx], RS485USART_DMA_IT_TCIFx[Usartx]);
	DMA_Cmd(RS485USART_DMAxTx_Streamx[Usartx],DISABLE);//写len前,需禁止数据操作
	while(DMA_GetCmdStatus(RS485USART_DMAxTx_Streamx[Usartx]));  //确保DMA可以被设置 
	DMA_SetCurrDataCounter(RS485USART_DMAxTx_Streamx[Usartx],len); //设置传输数据长度byte,		
	DMA_ITConfig(RS485USART_DMAxTx_Streamx[Usartx], DMA_IT_TC, ENABLE);//使能DMA_TC中断
	DMA_Cmd(RS485USART_DMAxTx_Streamx[Usartx],ENABLE);//打开DMA,开始发送

#endif
}

/***************************************************************************************
** 函数名称: Rs485_send_msg
** 功能描述: 485消息包发送函数
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void  Rs485_send_msg(ST485_USART *su, unsigned short nb1)
{
    if(nb1<=1)  
        return; 

    if(RS485RDE_Pin[su->Ini_par.usartx] != 0)
    {
        RS485_RDE_Contrl(su->Ini_par.usartx, send_dat);
    }
    su -> sta = (0x01 << B_EV_USART_TX);//置发送事件  
    Drv_Rs485_send_msg(su->Ini_par.usartx, su->tr_buf, nb1);
}

/***************************************************************************************
** 函数名称: RS485A_IRQHandler()
** 功能描述: 主机接收中断函数
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void RS485A_IRQHandler(void)
{
  Rs485_usart_isr(&RS485_USARTx[RS485A_ID]);
 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485A_ID], USART_IT_TXE) == SET)     
  {//Tansmit Data Register empty interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485A_ID], USART_IT_TXE);
   USART_ITConfig(RS485USARTx_BASE[RS485A_ID], USART_IT_TXE,  DISABLE); 
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485A_ID], USART_IT_TC) == SET)  
  {//Transmission complete interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485A_ID], USART_IT_TC);
   USART_ITConfig(RS485USARTx_BASE[RS485A_ID], USART_IT_TC,  DISABLE);   
   Rs485_usart_tx_rx_rde(RS485A_ID, ON);//使能USARTx_BASE[RS485A_ID]接收
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485A_ID], USART_IT_ORE) == SET)  
  {//OverRun Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485A_ID], USART_IT_ORE);  
   RS485USARTx_BASE[RS485A_ID]->DR;
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485A_ID], USART_IT_PE) == SET)  
  {//Parity Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485A_ID], USART_IT_PE);  
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485A_ID], USART_IT_FE) == SET)  
  {//Framing Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485A_ID], USART_IT_FE);  
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485A_ID], USART_IT_NE) == SET)  
  {//Noise Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485A_ID], USART_IT_NE);  
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485A_ID], USART_IT_IDLE) == SET)  
  {//USART IDLE
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485A_ID], USART_IT_IDLE);
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485A_ID], USART_IT_LBD) == SET)  
  {//USART LBD
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485A_ID], USART_IT_LBD);
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485A_ID], USART_IT_CTS) == SET)  
  {//USART CTS
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485A_ID], USART_IT_CTS);
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485A_ID], USART_IT_ERR) == SET)  
  {//USART ERR
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485A_ID], USART_IT_ERR);
  }
}

/***************************************************************************************
** 函数名称: RS485B_IRQHandler()
** 功能描述: 从机接收中断函数
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void RS485B_IRQHandler(void)
{
  Rs485_usart_isr(&RS485_USARTx[RS485B_ID]);
 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485B_ID], USART_IT_TXE) == SET)     
  {//Tansmit Data Register empty interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485B_ID], USART_IT_TXE);
   USART_ITConfig(RS485USARTx_BASE[RS485B_ID], USART_IT_TXE,  DISABLE); 
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485B_ID], USART_IT_TC) == SET)  
  {//Transmission complete interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485B_ID], USART_IT_TC);
   USART_ITConfig(RS485USARTx_BASE[RS485B_ID], USART_IT_TC,  DISABLE);   
   Rs485_usart_tx_rx_rde(RS485B_ID, ON);//使能USARTx_BASE[RS485B_ID]接收
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485B_ID], USART_IT_ORE) == SET)  
  {//OverRun Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485B_ID], USART_IT_ORE);  
   RS485USARTx_BASE[RS485B_ID]->DR;
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485B_ID], USART_IT_PE) == SET)  
  {//Parity Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485B_ID], USART_IT_PE);  
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485B_ID], USART_IT_FE) == SET)  
  {//Framing Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485B_ID], USART_IT_FE);  
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485B_ID], USART_IT_NE) == SET)  
  {//Noise Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485B_ID], USART_IT_NE);  
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485B_ID], USART_IT_IDLE) == SET)  
  {//USART IDLE
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485B_ID], USART_IT_IDLE);
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485B_ID], USART_IT_LBD) == SET)  
  {//USART LBD
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485B_ID], USART_IT_LBD);
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485B_ID], USART_IT_CTS) == SET)  
  {//USART CTS
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485B_ID], USART_IT_CTS);
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485B_ID], USART_IT_ERR) == SET)  
  {//USART ERR
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485B_ID], USART_IT_ERR);
  }
}

/***************************************************************************************
** 函数名称: RS485C_IRQHandler()
** 功能描述: 从机接收中断函数
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void RS485C_IRQHandler(void)
{
  Rs485_usart_isr(&RS485_USARTx[RS485C_ID]);
 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485C_ID], USART_IT_TXE) == SET)     
  {//Tansmit Data Register empty interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485C_ID], USART_IT_TXE);
   USART_ITConfig(RS485USARTx_BASE[RS485C_ID], USART_IT_TXE,  DISABLE); 
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485C_ID], USART_IT_TC) == SET)  
  {//Transmission complete interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485C_ID], USART_IT_TC);
   USART_ITConfig(RS485USARTx_BASE[RS485C_ID], USART_IT_TC,  DISABLE);   
   Rs485_usart_tx_rx_rde(RS485C_ID, ON);//使能USARTx_BASE[RS485C_ID]接收
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485C_ID], USART_IT_ORE) == SET)  
  {//OverRun Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485C_ID], USART_IT_ORE);  
   RS485USARTx_BASE[RS485C_ID]->DR;
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485C_ID], USART_IT_PE) == SET)  
  {//Parity Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485C_ID], USART_IT_PE);  
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485C_ID], USART_IT_FE) == SET)  
  {//Framing Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485C_ID], USART_IT_FE);  
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485C_ID], USART_IT_NE) == SET)  
  {//Noise Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485C_ID], USART_IT_NE);  
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485C_ID], USART_IT_IDLE) == SET)  
  {//USART IDLE
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485C_ID], USART_IT_IDLE);
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485C_ID], USART_IT_LBD) == SET)  
  {//USART LBD
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485C_ID], USART_IT_LBD);
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485C_ID], USART_IT_CTS) == SET)  
  {//USART CTS
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485C_ID], USART_IT_CTS);
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485C_ID], USART_IT_ERR) == SET)  
  {//USART ERR
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485C_ID], USART_IT_ERR);
  }
}

/***************************************************************************************
** 函数名称: RS485D_IRQHandler()
** 功能描述: 从机接收中断函数
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void RS485D_IRQHandler(void)
{
  Rs485_usart_isr(&RS485_USARTx[RS485D_ID]);
 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485D_ID], USART_IT_TXE) == SET)     
  {//Tansmit Data Register empty interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485D_ID], USART_IT_TXE);
   USART_ITConfig(RS485USARTx_BASE[RS485D_ID], USART_IT_TXE,  DISABLE); 
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485D_ID], USART_IT_TC) == SET)  
  {//Transmission complete interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485D_ID], USART_IT_TC);
   USART_ITConfig(RS485USARTx_BASE[RS485D_ID], USART_IT_TC,  DISABLE);   
   Rs485_usart_tx_rx_rde(RS485D_ID, ON);//使能USARTx_BASE[RS485D_ID]接收
   g_Master4_Scomm_Delay_cnt = g_Sys_1ms_Counter;
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485D_ID], USART_IT_ORE) == SET)  
  {//OverRun Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485D_ID], USART_IT_ORE);  
   RS485USARTx_BASE[RS485D_ID]->DR;
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485D_ID], USART_IT_PE) == SET)  
  {//Parity Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485D_ID], USART_IT_PE);  
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485D_ID], USART_IT_FE) == SET)  
  {//Framing Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485D_ID], USART_IT_FE);  
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485D_ID], USART_IT_NE) == SET)  
  {//Noise Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485D_ID], USART_IT_NE);  
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485D_ID], USART_IT_IDLE) == SET)  
  {//USART IDLE
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485D_ID], USART_IT_IDLE);
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485D_ID], USART_IT_LBD) == SET)  
  {//USART LBD
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485D_ID], USART_IT_LBD);
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485D_ID], USART_IT_CTS) == SET)  
  {//USART CTS
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485D_ID], USART_IT_CTS);
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485D_ID], USART_IT_ERR) == SET)  
  {//USART ERR
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485D_ID], USART_IT_ERR);
  }
}

/***************************************************************************************
** 函数名称: RS485E_IRQHandler()
** 功能描述: 从机接收中断函数
** 参    数: None
** 返 回 值: None
****************************************************************************************/
void RS485E_IRQHandler(void)
{
  Rs485_usart_isr(&RS485_USARTx[RS485E_ID]);
 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485E_ID], USART_IT_TXE) == SET)     
  {//Tansmit Data Register empty interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485E_ID], USART_IT_TXE);
   USART_ITConfig(RS485USARTx_BASE[RS485E_ID], USART_IT_TXE,  DISABLE); 
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485E_ID], USART_IT_TC) == SET)  
  {//Transmission complete interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485E_ID], USART_IT_TC);
   USART_ITConfig(RS485USARTx_BASE[RS485E_ID], USART_IT_TC,  DISABLE);   
   Rs485_usart_tx_rx_rde(RS485E_ID, ON);//使能USARTx_BASE[RS485E_ID]接收
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485E_ID], USART_IT_ORE) == SET)  
  {//OverRun Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485E_ID], USART_IT_ORE);  
   RS485USARTx_BASE[RS485E_ID]->DR;
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485E_ID], USART_IT_PE) == SET)  
  {//Parity Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485E_ID], USART_IT_PE);  
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485E_ID], USART_IT_FE) == SET)  
  {//Framing Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485E_ID], USART_IT_FE);  
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485E_ID], USART_IT_NE) == SET)  
  {//Noise Error interrupt
   USART_ClearITPendingBit(RS485USARTx_BASE[RS485E_ID], USART_IT_NE);  
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485E_ID], USART_IT_IDLE) == SET)  
  {//USART IDLE
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485E_ID], USART_IT_IDLE);
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485E_ID], USART_IT_LBD) == SET)  
  {//USART LBD
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485E_ID], USART_IT_LBD);
  } 
  if(USART_GetITStatus(RS485USARTx_BASE[RS485E_ID], USART_IT_CTS) == SET)  
  {//USART CTS
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485E_ID], USART_IT_CTS);
  }
  if(USART_GetITStatus(RS485USARTx_BASE[RS485E_ID], USART_IT_ERR) == SET)  
  {//USART ERR
  USART_ClearITPendingBit(RS485USARTx_BASE[RS485E_ID], USART_IT_ERR);
  }
}

#if (USART485_MODE==1)

void RS485A_DMA_TC_IRQHandler(void)
{
	if(DMA_GetITStatus(RS485USART_DMAxTx_Streamx[RS485A_ID], RS485USART_DMA_IT_TCIFx[RS485A_ID]) == SET)
  {
		DMA_ClearITPendingBit(RS485USART_DMAxTx_Streamx[RS485A_ID], RS485USART_DMA_IT_TCIFx[RS485A_ID]);
		DMA_Cmd(RS485USART_DMAxTx_Streamx[RS485A_ID],DISABLE);//关闭DMA
		USART_ITConfig(RS485USARTx_BASE[RS485A_ID],USART_IT_TC,ENABLE);//打开发送完成中断,发送最后两个字节  
//		Rs485_usart_tx_rx_rde(RS485A_ID,ON);//使能USARTx_BASE[useUART2]接收
  }
}

void RS485B_DMA_TC_IRQHandler(void)
{
	if(DMA_GetITStatus(RS485USART_DMAxTx_Streamx[RS485B_ID], RS485USART_DMA_IT_TCIFx[RS485B_ID]) == SET)
  {
		DMA_ClearITPendingBit(RS485USART_DMAxTx_Streamx[RS485B_ID], RS485USART_DMA_IT_TCIFx[RS485B_ID]);
		DMA_Cmd(RS485USART_DMAxTx_Streamx[RS485B_ID],DISABLE);//关闭DMA
		USART_ITConfig(RS485USARTx_BASE[RS485B_ID],USART_IT_TC,ENABLE);//打开发送完成中断,发送最后两个字节  
//		Rs485_usart_tx_rx_rde(RS485B_ID,ON);//使能USARTx_BASE[useUART1]接收
  }
}

void RS485C_DMA_TC_IRQHandler(void)
{
	if(DMA_GetITStatus(RS485USART_DMAxTx_Streamx[RS485C_ID], RS485USART_DMA_IT_TCIFx[RS485C_ID]) == SET)
  {
		DMA_ClearITPendingBit(RS485USART_DMAxTx_Streamx[RS485C_ID], RS485USART_DMA_IT_TCIFx[RS485C_ID]);
		DMA_Cmd(RS485USART_DMAxTx_Streamx[RS485C_ID],DISABLE);//关闭DMA
		USART_ITConfig(RS485USARTx_BASE[RS485C_ID],USART_IT_TC,ENABLE);//打开发送完成中断,发送最后两个字节  
//		Rs485_usart_tx_rx_rde(RS485C_ID,ON);//使能USARTx_BASE[useUART60]接收
  }
}

void RS485D_DMA_TC_IRQHandler(void)
{
	if(DMA_GetITStatus(RS485USART_DMAxTx_Streamx[RS485D_ID], RS485USART_DMA_IT_TCIFx[RS485D_ID]) == SET)
  {
		DMA_ClearITPendingBit(RS485USART_DMAxTx_Streamx[RS485D_ID], RS485USART_DMA_IT_TCIFx[RS485D_ID]);
		DMA_Cmd(RS485USART_DMAxTx_Streamx[RS485D_ID],DISABLE);//关闭DMA
		USART_ITConfig(RS485USARTx_BASE[RS485D_ID],USART_IT_TC,ENABLE);//打开发送完成中断,发送最后两个字节  
//		Rs485_usart_tx_rx_rde(RS485D_ID,ON);//使能USARTx_BASE[useUART4]接收
  }
}

void RS485E_DMA_TC_IRQHandler(void)
{
	if(DMA_GetITStatus(RS485USART_DMAxTx_Streamx[RS485E_ID], RS485USART_DMA_IT_TCIFx[RS485E_ID]) == SET)
  {
		DMA_ClearITPendingBit(RS485USART_DMAxTx_Streamx[RS485E_ID], RS485USART_DMA_IT_TCIFx[RS485E_ID]);
		DMA_Cmd(RS485USART_DMAxTx_Streamx[RS485E_ID],DISABLE);//关闭DMA
		USART_ITConfig(RS485USARTx_BASE[RS485E_ID],USART_IT_TC,ENABLE);//打开发送完成中断,发送最后两个字节  
//		Rs485_usart_tx_rx_rde(RS485E_ID,ON);//使能USARTx_BASE[useUART3]接收
  }
}

#endif


/************************************END OF FILE*****************************************/
