#ifndef _DRV_485USART_H
#define _DRV_485USART_H



#include "..\..\Drivers\inc\Stm32f4_Usart_Define.h"

#include "..\..\Drivers\ModbusRtuPoll\ModbusRtuPoll.h"
#include "..\..\Drivers\inc\drv_232Usart.h"


#ifdef		USART485_GLOBALS
	#define USART485_EXT
#else
	#define USART485_EXT  extern
#endif



/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/************************485硬件通道映射宏定义(用户设置变更区域)******************************/

//定义485A端口
#define RS485A_ID      			useUART2
#define RS485A_IRQHandler  	UseUSART2_IRQHandler

//定义485B端口
#define RS485B_ID      			useUART1
#define RS485B_IRQHandler  	UseUSART1_IRQHandler
 
//定义485C端口
#define RS485C_ID      			useUART6
#define RS485C_IRQHandler   UseUSART6_IRQHandler
 
//定义485D端口
#define RS485D_ID      			useUART4
#define RS485D_IRQHandler  	UseUSART4_IRQHandler
 
//定义485E端口
#define RS485E_ID      			useUART3
#define RS485E_IRQHandler  	UseUSART3_IRQHandler


//定义主/从端口
#define USART_BMS   	          RS485A_ID           //  CN22
#define USART_HMI   	          RS485D_ID           //  CN13
#define USART_PGV   	          RS485B_ID           //  CN21
#define USART_AWK1137C          RS485C_ID           //  CN14		
//#define USART_Pathfinder   	    RS485E_ID           //  CN15

#if (USART485_MODE==1)

#define RS485A_DMA_TC_IRQHandler		 DMA1_Stream6_IRQHandler		//RS485A_ID
#define RS485B_DMA_TC_IRQHandler		 DMA2_Stream7_IRQHandler		//RS485B_ID
#define RS485C_DMA_TC_IRQHandler		 DMA2_Stream6_IRQHandler		//RS485C_ID
#define RS485D_DMA_TC_IRQHandler		 DMA1_Stream4_IRQHandler		//RS485D_ID
#define RS485E_DMA_TC_IRQHandler		 DMA1_Stream3_IRQHandler		//RS485E_ID

#endif


/* Private variables ---------------------------------------------------------*/


//外部接口函数
//USART485_EXT	void	Rs485_Usart_InitHard(RS485USART_Ini *ui);
//USART485_EXT	void	Rs485_usart_tx_rx_rde(USART_ID Usartx, unsigned	char on_off_sta);
//USART485_EXT	void  Rs485_send_msg(ST485_USART *su, unsigned short nb1);




#endif

/***************************************************************************************
**  End Of File                                                     
***************************************************************************************/
