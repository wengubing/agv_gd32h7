#ifndef _DRV_232USART_H
#define _DRV_232USART_H

 



#ifdef  USART232_GLOBALS
 #define USART232_EXT
#else
 #define USART232_EXT  extern
#endif

 
#include "..\..\Drivers\inc\Stm32f4_Usart_Define.h"



/* Private define ------------------------------------------------------------*/
// 通信错误定义
#define ERROR_NONE                  0   // 没有错误
#define ERROR_STARTCHAR             1   // 数据包开始符错误
#define ERROR_ENDCHAR               2   // 数据包结束符错误
#define ERROR_PACKAGESIZE           3   // 数据包最小长度错误
#define ERROR_DEVICETYPE            4   // 设备类型错误
#define ERROR_ADDRESS               5   // 设备地址错误
#define ERROR_CRC                   6   // CRC校验错误
#define ERROR_CMD                   7   // 错误命令
#define ERROR_PARA                  8   // 错误参数
#define ERROR_WAIT                  9   // 需要等待前面动作执行完，重复或冲突？？？
 
 
 
 
 
#define   UART_RECEIVE_OVERTIME_MAX     5   //串口接收超时最大计数值

#define   UART_MSG_LENGTH_MAX        128 //软件缓冲区深度
 
#define   UART_RXTX_FIFO_SIZE        16  // 系统串口数据缓冲区数量
 
/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

//USART初始化结构体
typedef struct USART_INI_typedef
{
	USART_ID    usartx;//usart当前的ID编号
	unsigned long  baud;//当前设置的波特率值
	unsigned char  baud_set; //波特率设置编号
	unsigned short comm_cfg_par;//通讯配置参数,见宏定义
	unsigned char  pi;//中断优先级编号
}RS232USART_Ini;
 

//=========================================================================
//                              口接收数据结构
//=========================================================================

// 一条串口消息包
typedef struct structUartMsgType
{
    u8   Data[UART_MSG_LENGTH_MAX];// 数据缓冲区
    u8   Len;// 有效数据长度
}UartMsgType;


// 串口消息包FIFO队列
typedef struct structUartMsgFifoType
{
    UartMsgType   Msg[UART_RXTX_FIFO_SIZE];
    u8           rd_addr;//当前接收数据包队列指针
    u8           wr_addr;//当前发送数据包队列指针
    u8           remain;
}UartMsgFifoType;

// st232消息队列
typedef struct ST232_UASRTType
{
  RS232USART_Ini  Ini_par;
  signed int    TimeOutVal;//RS232超时值
  UartMsgFifoType  Tx_fifo,
           Rx_fifo;
}ST232_USART;


USART232_EXT ST232_USART  RS232_USARTx[USARTMAXCNT];//232底层数据端口x


/************************232硬件通道映射宏定义(用户设置变更区域)******************************/
////定义232A端口
//#define RS232A_ID      useUART4
//#define RS232A_IRQHandler  UseUSART4_IRQHandler

////定义232B端口
//#define RS232B_ID      useUART3
//#define RS232B_IRQHandler  UseUSART3_IRQHandler

//定义232C端口
//#define RS232C_ID      useUART6
//#define RS232C_IRQHandler  UseUSART6_IRQHandler



//外部接口函数

USART232_EXT void Rs232Usart_InitHard(RS232USART_Ini *ui);

USART232_EXT void  Rs232_usart_tm_1ms(ST232_USART *su);

USART232_EXT void uart_dma_send(USART_ID Uart, u8 *pData, u32 Len);



#endif

/***************************************************************************************
**  End Of File                                                     
***************************************************************************************/
