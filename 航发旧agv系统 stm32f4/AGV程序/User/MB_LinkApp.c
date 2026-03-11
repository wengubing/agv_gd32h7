/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: MB_LinkApp.c
** 创 建 人: 文小兵  
** 描    述: modbus应用层数据命令队列管理
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月13日 重构
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define	MBLINK_GLOBALS

//文件引用
#include "includes.h"
#include "..\..\User\MB_LinkApp.h"
#include "CommConf.h"

#include "Custom_UART.h"

//内部宏定义
#define		MB_DEALY_WORK									5000//等待485从机就绪时间值

/***************************************************************************************
** 函数名称: add_cmd_que
** 功能描述: 添加一般循环队列
** 参    数: p:应用队列指针
** 返 回 值: res:当前设置队列实际个数
***************************************************************************************/
s16 add_cmd_que(MB_MANGER_APP *p)
{
	s16 len=0;
	LINK_QUE *pLinkQue=0;
	MB_REG_INDEX_TABLE *pTable;
	s8  start_index=0,end_index=0;
	MB_REG_INDEX *pmri;
	QUE_MANGER_APP *plink= &p->link_que[p->mb_run_id];	
	u8	slv_addr = plink->SlvAddrList[plink->link_slv_addr_index];
	
	if(!plink)
		return -1;

    if(plink->linkquetype_step >= NORMAL_QUE && plink->pInjDataBuf)
    {//在进入normal模式才可进行注入数据操作
        //判断是否有注入inj数据更新
        start_index = plink->pQuickIndex[INJ_QUE].start_index,
        end_index   = plink->pQuickIndex[INJ_QUE].end_index;
        if(start_index>=0 && end_index>=0)
        {
            u16 i=0,*pU16_old, *pU16_new=0;;
            len = plink->pInjDataBuf->size;
            pU16_new = plink->pInjDataBuf->pData_new;
            pU16_old = plink->pInjDataBuf->pData_old;
            if(pU16_new && pU16_old)
            {
                for(i=0;i<len;i++)
                {
                    if(pU16_new[i] != pU16_old[i])
                    {
                        pU16_old[i] = pU16_new[i];
                        len=0;
                        goto INJ_QUE;
                    }
                }
            }
        }
    }
	
	switch (plink->linkquetype_step)
	{
		case ONCE_QUE://******************************************************单次队列
		{
			start_index = plink->pQuickIndex[ONCE_QUE].start_index,
			end_index   = plink->pQuickIndex[ONCE_QUE].end_index;
			if(start_index<0 || end_index<0)
			{
				plink->linkquetype_step=NORMAL_QUE;//无单次队列,直接跳转到循环队列执行
				goto NORMAL_QUE;
			}
		}
		break;
		case NORMAL_QUE://****************************************************循环队列	
		{
			NORMAL_QUE:
			start_index = plink->pQuickIndex[NORMAL_QUE].start_index,
			end_index   = plink->pQuickIndex[NORMAL_QUE].end_index;
			if(start_index<0 || end_index<0)
			{
				plink->linkquetype_step=NULL_QUE;//无循环队列,直接跳转到空队列(不进行队列操作)		
				goto NULL_QUE;
			}
		}
		break;
		case INJ_QUE://*******************************************************注入队列	
		{
			INJ_QUE:
			start_index = plink->pQuickIndex[INJ_QUE].start_index,
			end_index   = plink->pQuickIndex[INJ_QUE].end_index;
			if(start_index<0 || end_index<0)
			{
				plink->linkquetype_step=NORMAL_QUE;//无单次队列,直接跳转到循环队列执行	
				goto NORMAL_QUE;
			}
		}
		break;
		case NULL_QUE:
		{
			NULL_QUE:
			return 0;
		}
		default:
		break;
	}

	pLinkQue = plink->link_que_set;
	
	if(!(slv_addr>=1 && slv_addr<128))
		return -2;
	
	pTable = &plink->pMbTable[p->mb_run_id];

	while(start_index <= end_index)
	{
		u8 break_cnt=0;
		u16 *pU16=0,*p2U16=0;
		
		pmri = &pTable->pRegindex[plink->link_slv_addr_index];
		
		switch (plink->link_comm_para_set.Protocol_id)//判断协议类型
		{
			case MODBUS_RTU:
			{
				pLinkQue[len].func=pmri->func;
				pLinkQue[len].slv_addr=slv_addr;
				pLinkQue[len].sel_coil_reg=pmri->sel;	
				pLinkQue[len].rw_slv_first_hr_addr=pmri->addr;		
				pLinkQue[len].rw_num=pmri->rw_size;
				pU16 = pmri->pHR;
				p2U16 = pmri->pHR2;

				if(pU16)
				{
					/*只读的数据发送，无需关注写的寄存器*/
					if(	   pmri->func == MD_RD_HR		/*读保持寄存器*/
						|| pmri->func == MD_RD_IR		/*读输入寄存器*/
						|| pmri->func == MD_RD_COIL		/*读线圈*/
						|| pmri->func == MD_RD_DISCRETE	/*读离散输入状态*/
					)
					{
						pLinkQue[len].phr_read				=pU16;
						pLinkQue[len].phr_write				=0;
						pLinkQue[len].wr_slv_first_hr_addr	=0;
						pLinkQue[len].write_num				=0;
					}
					/*只写的数据发送，无需关注读的寄存器*/
					else if(   pmri->func == MD_FR_MHR		/*强制多个寄存器*/
							|| pmri->func == MD_FR_SHR		/*强制单个寄存器*/
					)
					{
						pLinkQue[len].phr_read=0;
						pLinkQue[len].phr_write=pU16;
						pLinkQue[len].wr_slv_first_hr_addr=0;
						pLinkQue[len].write_num=0;
					}
					/*只写的数据发送，无需关注读的寄存器*/
					else if(   (pmri->func == MD_FR_SCOIL	/*强制单个线圈*/
							|| pmri->func == MD_FR_MCOIL)	/*强制多个线圈*/
							&& p2U16
					)
					{
						pLinkQue[len].phr_read=0;
						pLinkQue[len].phr_write=p2U16;
						pLinkQue[len].wr_slv_first_hr_addr=pmri->addr2;
						pLinkQue[len].write_num=pmri->rw_size2;
					}
					else if(pmri->func == MD_FR_MHR_RDHR && p2U16)
					{
						pLinkQue[len].phr_read=pU16;
						pLinkQue[len].phr_write=p2U16;
						pLinkQue[len].wr_slv_first_hr_addr=pmri->addr2;
						pLinkQue[len].write_num=pmri->rw_size2;
					}
				}
				else
				{
					break_cnt++;
				}
			}
			break;

			case Custom_UART:
			{
				pLinkQue[len].slv_addr				=slv_addr;
				pLinkQue[len].func					=pmri->func;
				pLinkQue[len].sel_coil_reg			=pmri->sel;
				pLinkQue[len].phr_read				=pmri->pHR;
				pLinkQue[len].rw_slv_first_hr_addr	=pmri->addr;		
				pLinkQue[len].rw_num				=pmri->rw_size;
				pLinkQue[len].phr_write				=pmri->pHR2;
				pLinkQue[len].wr_slv_first_hr_addr	=pmri->addr2;
				pLinkQue[len].write_num				=pmri->rw_size2;
			}
			break;
			default:
			break;
		}
		if(break_cnt)
			break;		
		
		len++;
		start_index++;
	}

	return len;
}


/***************************************************************************************
** 函数名称: MasterTest
** 功能描述: 上电默认发出一个测试数据包,用于解决MCU上电无故发出乱码问题
** 参    数: 
** 返 回 值: 
***************************************************************************************/
void MasterTest(ST485_USART *p)
{
	u8 testdata[]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF}, len;

	testdata[0] = p->Ini_par.usartx + 1;

	len = sizeof(testdata);
	memcpy(p->tr_buf, testdata, len);
	Rs485_send_msg(p, len);
}

/***************************************************************************************
** 函数名称: Tsk_QueManger
** 功能描述: rs485主栈协议数据队列管理任务函数
** 参    数: 
** 返 回 值: 
***************************************************************************************/
u8 Tsk_QueManger(MB_MANGER_APP *p)
{
	u8 res=0,buffer=0;
	QUE_MANGER_APP *plink= &p->link_que[p->mb_run_id];

	if(plink->link_slv_addr_index >= plink->Slv_num)//继续检索下一个从机
	{//一次从机link完毕,重新开始link
		buffer = 1;
	}
	else
	{
		if(!plink->once_flag)
		{
			plink->once_flag=1;
			plink->wr_que_addr=0;
			plink->rd_que_addr=0;
			plink->que_len=add_cmd_que(p);
		}

		if(plink->que_len)
		{
			if(plink->rd_que_addr < plink->que_len)
			{//抽取Slave队列...
				plink->app_run_sta=true;
			}
			else
			{//一个Slave命令队列抽取完成
				buffer = 2;
			}
		}
		else
		{//一个从机的执行队列执行完
			buffer = 3;
		}
	}
	
	if(buffer)
	{
		if(buffer==1)
		{
			plink->link_slv_addr_index=0;
		}
		if(buffer==2)
		{
			plink->link_slv_addr_index++;
		}
		plink->once_flag=0;
		plink->app_run_sta=false;		
		if((plink->linkquetype_step==ONCE_QUE || plink->linkquetype_step==INJ_QUE) && buffer==1)
		{
			plink->linkquetype_step=NORMAL_QUE;
		}
	}
	
	return res;
}

u8 xor_sum(u8 *p, u8 len)
{
	u8 i;
	u8 add_sum=0x00;
	
	for(i=0;i<len;i++)
	{
		add_sum ^= p[i];
	}
	
	return add_sum;
}

/*********************************************************************************************************
*	函 数 名: Tsk_RS4850_Master
*	功能说明: RS485主/从机数据处理
*********************************************************************************************************/
void Tsk_RS485_Master(MB_MANGER_APP *p)
{
	QUE_MANGER_APP *plink= &p->link_que[p->mb_run_id];

	if(plink->app_run_sta==false)
	{//查询link队列
		Tsk_QueManger(p);
	}
	else if(plink->app_run_sta==true)
	{//加载队列数据
		MD_MST_STA		Status=MD_MST_STA_NULL;
		switch (plink->link_comm_para_set.Protocol_id)//判断协议类型
		{
			case MODBUS_RTU:
			{
				Status=link_ptl_modbus_rtu(&plink->link_comm_para_set, &plink->link_que_set[plink->rd_que_addr]);
			}
			break;
			
			case Custom_UART:
			{
				Status=link_ptl_Custom_UART(&plink->link_comm_para_set, &plink->link_que_set[plink->rd_que_addr]);
			}
			break;
            
			default:
			break;
		}

		switch (Status)
		{
			case MD_MST_STA_OK://1次连接通讯是否OK
			case MD_MST_STA_RETRYED://link次数超过设定值
			case MD_MST_STA_INVALID_FUNC://无效指令
			{
				if(Status==MD_MST_STA_OK)
				{//正确通讯1次
						(*p->t_mb_ScommOK_process)(plink, p->mb_run_id);
				}
				plink->app_run_sta=false;
				plink->rd_que_addr++;				
			}
			break;
            
			case MD_MST_STA_TIMEOUT://1次连接通讯超时
			{
				(*p->t_mb_timout_process)(plink, p->mb_run_id);
			}
			break;
            
			case MD_MST_STA_SEND_ERR:
            case MD_MST_STA_REC_ERR:    
			{
				(*p->t_mb_ScommErr_process)(plink, p->mb_run_id);
			}
			break;            
            
			default:
			break;
		}
	}
}

//find index
u8 MBScommFindIndex(u8 id,u8 SlvId)
{
	unsigned char i = 0;
	u8 index = SLVS_MAXNUM_NOTFIND;//未找到
	CCF_StDef* p2CCF = (CCF_StDef*)&CCFs;

	for (i = 0; i < p2CCF[CCFs.Record.MstCCFID[id]].InitRecord.SlvNum; i++)
	{
		if (SlvId == p2CCF[CCFs.Record.MstCCFID[id]].InitRecord.ID[i])
			index = i;
	}

	return index;
}

//通讯1次OK回调处理
void mb_ScommOK_process(QUE_MANGER_APP *p, u8 id)
{
	u8 Index=0;
	CCF_StDef *p2CCF = (CCF_StDef*) & CCFs;

	//find index
	Index = MBScommFindIndex(id, p->link_comm_para_set.mb_master.slv);

	if(Index!=SLVS_MAXNUM_NOTFIND)
	{
		p2CCF[CCFs.Record.MstCCFID[id]].InitRecord.p2Mst[Index]->CommOkCnt++;
		p2CCF[CCFs.Record.MstCCFID[id]].InitRecord.p2Mst[Index]->TimeOutMs = 0;
	}
}

//超时回调处理
void mb_timout_process(QUE_MANGER_APP *p, u8 id)
{

}

//无效链接处理
void mb_ScommErr_process(QUE_MANGER_APP *p, u8 id)
{
	
}

/***********************************************************
** 函数名称:		mb_ResetInit_CallBack
** 功能描述:		在modbus协议栈重启时 动态更改cn口配置的函数
				，在应用层按需重写
** 参    数:	None
** 返 回 值:		None
***********************************************************/
__weak void mb_ResetInit_CallBack(void)
{

}
/***************************************************************************************
** 函数名称: LinkCommIni
** 功能描述: 主通讯连接参数列表初始化
** 参    数: 
** 返 回 值: event_flag
***************************************************************************************/
u8 LinkCommIni(QUE_MANGER_APP *plink, u8 index)
{
	u8 i;
	u16 send_dlay=0,timeout=0;
	ProtocolType pt=MODBUS_NULL;
	CCF_Format_EnumDef pt_Temp = CCFRTU;
	CCF_StDef *p2CCF = (CCF_StDef*) & CCFs;
	
	plink->wr_que_addr=0;
	plink->rd_que_addr=0;
	plink->que_len=0;
	plink->Slv_num =0;
	

	plink->link_comm_para_set.mb_master.Master_link_Sta=STA_TX;
	plink->pMbTable = CCFs.Record.MstCCF;

	for(i=0;i<p2CCF[CCFs.Record.MstCCFID[index]].InitRecord.SlvNum;i++)
	{
		plink->SlvAddrList[i]=p2CCF[CCFs.Record.MstCCFID[index]].InitRecord.ID[i];
	}

	plink->link_comm_para_set.pST=&RS485_USARTx[CCFs.Record.MstCCFID[index]];
	plink->pQuickIndex = CCFs.Record.QuickIndexs[index].QuickIndex;
	pt_Temp = p2CCF[CCFs.Record.MstCCFID[index]].PubPara.Format;
	if(pt_Temp == CCFASCII)
	{
		pt = MODBUS_ASCII;
	}
	else if(pt_Temp == CCFRTU)
	{
		pt = MODBUS_RTU;
	}
	else if(pt_Temp == CCFTCP)
	{
		pt = MODBUS_TCP;
	}
	else if(pt_Temp == CCFCustomUART)
	{
		pt = Custom_UART;
	}
	else
	{
		pt = MODBUS_NULL;
	}
	send_dlay=p2CCF[CCFs.Record.MstCCFID[index]].MstPara.SendDelay;
	timeout=p2CCF[CCFs.Record.MstCCFID[index]].MstPara.Timeout;

	//link主机参数初始化
	if(link_para_init(
											&(plink->link_comm_para_set), 
											timeout, //超时值
											send_dlay,//通讯间隔时间值
											2, //重发次数
											pt	//协议类型
										))
		return 1;
	
	
	//初始化从机地址列表
	for(i=0;i<Master_SLV_MAX_NUM;i++)
	{
		if(plink->SlvAddrList[i])
		{
			plink->Slv_num++;
		}
	}

	plink->linkquetype_step=ONCE_QUE;
	
	return 0;
}

/*********************************************************************************************************
*	函 数 名: Tsk_MB
*	功能说明: 上电初始化bsp后,需要定时1ms调用一次
*********************************************************************************************************/
void Tsk_MB(MB_MANGER_APP *pa)
{
	switch (pa->mb_step)
	{
		case 0://MB初始化
		{
			u8 i;
			for(i=0;i<MBLink_MASTER_NUM;i++)
			{
				if(LinkCommIni(&pa->link_que[i], i))
				{//参数初始化错误
					pa->mb_step=3;
				}
				else if(pa->link_que[i].link_comm_para_set.pST)
				{//ok
					pa->t_mb_timout_process = mb_timout_process;//初始化回调函数
					pa->t_mb_ScommOK_process = mb_ScommOK_process;
					pa->t_mb_ScommErr_process = mb_ScommErr_process;
					MasterTest(pa->link_que[i].link_comm_para_set.pST);//初始化后,发送一包测试数据
					pa->mb_nb++;
				}
			}

			if(pa->mb_nb>=1)
			{
				pa->mb_flag=true;//使能主机工作
				pa->mb_PowerUp_cnt=g_Sys_1ms_Counter;
				pa->mb_step=1;			
			}
			else
			{
				pa->mb_step=3;
			}
		}
		break;
		case 1://等待时间到
		{
			if(Delay_Fuction(g_Sys_1ms_Counter, pa->mb_PowerUp_cnt, MB_DEALY_WORK))
			{
				pa->mb_step=2;
			}
		}
		break;
		case 2://MB工作
		{
			if(pa->mb_flag==true)
			{
				for(pa->mb_run_id=0; pa->mb_run_id < MBLink_MASTER_NUM;pa->mb_run_id++)
				{
					Tsk_RS485_Master(pa);
				}
			}
			else
			{
				pa->mb_step=3;
			}
		}
		break;
		case 3://MB退出
		{

		}
		break;
		case 4://MB复位
		{
			mb_ResetInit_CallBack();
			pa->mb_step = 0;
		}
		break;
		default:
		break;
	}	
}

//***********************************END OF FILE***********************************

