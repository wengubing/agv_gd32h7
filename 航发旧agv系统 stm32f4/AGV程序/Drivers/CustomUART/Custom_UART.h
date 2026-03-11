/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	Custom_UART.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-04-09         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/


/*define-----------------------------------------------------------------------*/

#ifdef  CUSTOM_UART_GLOBALS
	#define CUSTOM_UART_GLOBALS_EXT
#else
	#define  CUSTOM_UART_GLOBALS_EXT extern
#endif
#ifndef __CUSTOM_UART_H_
#define __CUSTOM_UART_H_

#include "ModbusRtuPoll.h"

//用户数据分析指针，可处理包数据错误等功能
CUSTOM_UART_GLOBALS_EXT	uint8_t (*Custom_UART_Receive_DataAnalysis)(uint8_t* p_buff,uint16_t buff_num);

CUSTOM_UART_GLOBALS_EXT MD_MST_STA link_ptl_Custom_UART(LINK_COMM_PARA *plinkpara, LINK_QUE *plinkque);

#endif


 

