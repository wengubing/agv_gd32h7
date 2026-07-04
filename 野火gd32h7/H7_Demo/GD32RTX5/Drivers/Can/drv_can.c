
/**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Drv_485Usart.c
** 创 建 人: dengyang
** 描    述: 485硬件接口驱动
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: 	V1.0 
** 创建日期: 	2015.03.18
** 描    述:  硬件描述层
**--------------------------------------------------------------------------------------   
**/

#define		CAN_GLOBALS

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "drv_can.h"
#include "bsp_can_fd.h"

//============================================================================
// 名称：can_send_msg
// 功能：通过CAN口发送报文
// 参数：Can：CAN口
//      TxMessage：要发送的报文
// 返回：无
// 说明：发送邮箱
//============================================================================
u8 Set_CAN_Int(CAN_ID CANx, CAN_BAUD_TYPE can_baud, u32 addr, u8 can_pi)
{
	(void)addr;
	(void)can_pi;

	if(CANx >= CANn)
	{
		return 1;
	}

	if((can_baud < CAN_BAUDRATE_5K) || (can_baud > CAN_BAUDRATE_1M))
	{
		can_baud = CAN_BAUDRATE_500K;
	}

	UNI_CAN[CANx].ini.CANx = CANx;
	UNI_CAN[CANx].ini.Baud = can_baud;
	UNI_CAN[CANx].CanStatusFlag = CAN_NoneError;
	UNI_CAN[CANx].rx_fifo.read_adr = 0;
	UNI_CAN[CANx].rx_fifo.write_adr = 0;
	UNI_CAN[CANx].tx_fifo.read_adr = 0;
	UNI_CAN[CANx].tx_fifo.write_adr = 0;

	return 0;
}

u8 can_send_msg(CAN_ID Canx, CanTxMsg TxMessage)
{
	can_mailbox_descriptor_struct tx_msg;
	uint32_t timeout = 0xFFFFFFU;
	uint32_t tx_words[2] = {0U, 0U};
	uint8_t i;

	if(Canx >= CANn)
	{
		return 1;
	}

	if(UNI_CAN[Canx].CanStatusFlag != CAN_NoneError)
	{
		return 2;
	}

	can_struct_para_init(CAN_MDSC_STRUCT, &tx_msg);
	tx_msg.rtr = (TxMessage.RTR == CAN_RTR_REMOTE) ? 1U : 0U;
	tx_msg.ide = (TxMessage.IDE == CAN_ID_EXT) ? 1U : 0U;
	tx_msg.code = CAN_MB_TX_STATUS_DATA;
	tx_msg.brs = 0U;
	tx_msg.fdf = 0U;
	tx_msg.prio = 0U;
	tx_msg.data_bytes = (TxMessage.DLC <= 8U) ? TxMessage.DLC : 8U;
	tx_msg.id = (tx_msg.ide != 0U) ? TxMessage.ExtId : TxMessage.StdId;

	for(i = 0U; i < tx_msg.data_bytes; i++)
	{
		if(i < 4U)
		{
			tx_words[0] |= ((uint32_t)TxMessage.Data[i] << (8U * i));
		}
		else
		{
			tx_words[1] |= ((uint32_t)TxMessage.Data[i] << (8U * (i - 4U)));
		}
	}

	tx_msg.data = tx_words;
	can_mailbox_config(CANX, CAN_TRANSMIT_NUM, &tx_msg);

	while((RESET == can_flag_get(CANX, CAN_FLAG_MB0)) && (timeout != 0U))
	{
		timeout--;
	}

	if(timeout == 0U)
	{
		//UNI_CAN[Canx].CanStatusFlag = CAN_TransmitError;
		return 1;
	}

	return 0;
}



