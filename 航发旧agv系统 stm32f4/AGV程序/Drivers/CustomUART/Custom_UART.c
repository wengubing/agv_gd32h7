/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	Custom_UART.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-04-09
	* Description:
	******************************************************************************
**/
#define  CUSTOM_UART_GLOBALS

#include "Custom_UART.h"

#include "string.h"

/***************************************************************************************
** 函数名称: task_Custom_UART_master_receive
** 功能描述: modbus_rtu主机接收任务
** 参    数: 
      p:指向串行端口结构体的指针
      pm:指向对应主模式命令结构体指针
** 返 回 值: none
****************************************************************************************/
MD_MST_STA task_Custom_UART_master_receive(ST485_USART *p, ST_MODBUS_MASTER *pm)
{
	uint8_t					DataAnalysisRes	=	0;
	uint8_t					*re_buf			=	0;	//接收缓冲区指针
	uint16_t				uart_rec_num	=	0;
	uint16_t				*p_hr_read		=	0;	//(从)读/(向)写从机HR的指针buf
	MD_MST_STA				mb_sta			= 	MD_MST_STA_NO_REC;

	do{
		if(M_TST_BIT(p -> sta, B_EV_USART_RX) == 0)	{break;} 	// 判断是否有接收标志

		mb_sta		= 	MD_MST_STA_REC_ERR;
		M_K_CLR_BIT(p -> sta, B_EV_USART_RX); 					// 清接收标志 
		p_hr_read	=	pm -> phr_read;
		uart_rec_num=	p -> rx_nb;
		p -> rx_nb	=	0;
		re_buf		=	&(p->tr_buf[0]);						//获取接收区起始地址
		//基于原始数据进行解析
		if(Custom_UART_Receive_DataAnalysis != NULL)
		{
			DataAnalysisRes = Custom_UART_Receive_DataAnalysis(re_buf,uart_rec_num);
		}
		if(DataAnalysisRes != 0){break;}

		//解析完成拷贝至指定变量处
		/*取接收数据和缓存大小更小的值作为内存拷贝的界限*/
		if(uart_rec_num-1 > pm->dat_num)
		{
			uart_rec_num = pm->dat_num;
		}
		memcpy((uint8_t*)p_hr_read,(uint8_t*)re_buf,uart_rec_num);
		
		mb_sta = MD_MST_STA_OK;//置未收到数据包
	}while(0);

	return mb_sta;
}

/***********************************************************
** 函数名称:	task_Custom_UART_master_send
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
void task_Custom_UART_master_send(LINK_QUE *plinkque,ST485_USART *p, ST_MODBUS_MASTER *pm)
{
	pm->slv 		= plinkque->slv_addr;
	pm->dat_num		= plinkque->rw_num;		//发送数量
	pm->pwr_write	= plinkque->phr_write;	//发送地址
	pm->wr_n		= plinkque->write_num;	//接收数量
	pm->phr_read	= plinkque->phr_read;	//接收地址

	pm->mb_sta = MD_MST_STA_READY;//可以开始发送数据

	memcpy((uint8_t*)p->tr_buf,(uint8_t*)plinkque->phr_write,plinkque->write_num);

	Rs485_send_msg(p,plinkque->write_num);

	pm -> mb_sta = MD_MST_STA_INPROCESS;//置主机状态
}

/***************************************************************************************
** 函数名称: link_ptl_modbus_Custom_UART
** 功能描述: 
1.通讯连接任务,实现数据包的发送和接收一体，并返回执行的状态
2.本函数需要在大循环中非阻塞调用;
***************************************************************************************/
MD_MST_STA link_ptl_Custom_UART(LINK_COMM_PARA *plinkpara, LINK_QUE *plinkque)
{
	ST485_USART      *pSTu=plinkpara->pST;
	ST_MODBUS_MASTER *pSTpm = &plinkpara->mb_master;

	pSTpm->mb_sta=MD_MST_STA_NULL;

	switch (pSTpm->Master_link_Sta)
	{
		case STA_TX://master发送
		{
			if(Delay_Fuction(Dylinkcomm_1ms_cnt, plinkpara->MasterSend_dealy_cnt, plinkpara->comm_Send_delay))
			{//发送间隔时间到....

				task_Custom_UART_master_send(plinkque,pSTu,pSTpm);

				if(pSTpm->mb_sta == MD_MST_STA_INPROCESS)
				{//进行中...
					pSTpm->Master_link_Sta = STA_RX;//置等待数据回应 
				}
				else
				{//如果发送数据准备有错
					pSTpm->Master_link_Sta = STA_TX;//重新启动发送
				}

				if(pSTpm->Master_link_Sta==STA_RX)
				{//成功发送
					plinkpara->master_send_cnt++;//发送次数
					plinkpara->wait_recive_timeout_cnt=Dylinkcomm_1ms_cnt;      
				}
				else if(pSTpm->Master_link_Sta==STA_TX)
				{
					plinkpara->master_send_cnt = 0;
					pSTpm->mb_sta=MD_MST_STA_NULL;
				}
				pSTpm->mb_sta=master_send_ready(plinkpara, pSTu, pSTpm); //---------------------------->>>      

			}
		}
		break;
		case STA_RX://master接收
		{
			pSTpm->mb_sta = task_Custom_UART_master_receive(pSTu, pSTpm);
			link_TimeoutProcess(plinkpara, pSTu, pSTpm);
		}
		break;
		default:
		break;
	}

	return pSTpm->mb_sta;
}




/************************************END OF FILE************************************/
