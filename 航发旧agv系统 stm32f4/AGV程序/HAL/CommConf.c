/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: CommConf.c
** 创 建 人: 文小兵  
** 描    述: 通信接口配置
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月10日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define CommConf_GLOBALS

//自定义引用		
#include "CBB_Init.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "EmgJudge.h"
#include "CommConf.h"
#include "HMI.h"
#include "..\..\Drivers\ModbusRtuPoll\ModbusRtuPoll.h"


//外部对象声明

//内部方法声明

//QUICK_INDEXs 结构体默认值对象定义
const MB_QUICK_INDEXs mb_reg_quick_indexs_init =
{
	-1, -1,	     //ONCE_QUE在 mb_index_regx 中的起始位置
	0, 0,	       //NORMAL_QUE
	-1, -1,      //INJ_QUE
};

/**************************************************************************
* 函数名:EQPMstInit
* 功  能:从机设备通信参数初始化
* 参  数:
* 返回值:bool
**************************************************************************/
bool EQPMstInit(
	CCFEQP_Master_StDef* p,
	USART_BAUD Baud,										//波特率值
	unsigned char Pi,										//中断优先级编号
	CCF_Format_EnumDef Format,								//协议格式
	
	unsigned short Timeout,									//超时时间 ms
	unsigned short SendDelay,								//发送间隔 ms
	unsigned short Retry,									//重发次数
	u8 SlaveId,												//从站Id

	//从机通信参数列表
	MD_FUNC	func,
	u8 sel,
	u16* pHR,
	u16	addr,
	u8 rw_size,//寄存器数量 16位！
	u16* pHR2,
	u16	addr2,
	u8 rw_size2,
	LINK_QUE_TYPE linktpye, 

	CCFID_EnumDef CommId									//通信接口编号
)
{
	p->PubPara.Role = CCFMBMaster;
	p->PubPara.Baud = Baud;
	p->PubPara.Pi = Pi;
	p->PubPara.Format = Format;

	p->MstPara.Timeout = Timeout;
	p->MstPara.SendDelay = SendDelay;
	p->MstPara.Retry = Retry;

	p->SlaveId = SlaveId;

	p->MBConf.func = func;
	p->MBConf.sel = sel;
	p->MBConf.pHR = pHR;
	p->MBConf.addr = addr;
	p->MBConf.rw_size = rw_size;
	p->MBConf.pHR2 = pHR2;
	p->MBConf.addr2 = addr2;
	p->MBConf.rw_size2 = rw_size2;
	p->MBConf.linktpye = linktpye;

	p->CommId = CommId;

	return 0;

}

/**************************************************************************
* 函数名:EQPSlvInit
* 功  能:从站设备通信参数初始化
* 参  数:
* 返回值:bool
**************************************************************************/
bool EQPSlvInit(
	CCFEQP_Slave_StDef *p, 
	USART_BAUD Baud,										//波特率值
	unsigned char Pi,										//中断优先级编号
	CCF_Format_EnumDef Format,								//协议格式
	u16* p2SlaveData,										//数据指针
	unsigned short SlaveDataLen,							//数据长度
	CCFID_EnumDef CommId									//通信接口编号
)
{
	p->PubPara.Role = CCFMBSlave;
	p->PubPara.Baud = Baud;
	p->PubPara.Pi = Pi;
	p->PubPara.Format = Format;

	p->SlvPara.p2SlaveData = p2SlaveData;
	p->SlvPara.SlaveDataLen = SlaveDataLen;

	p->CommId = CommId;

	return 0;

}

/**************************************************************************
* 函数名:CCF_MstInit
* 功  能:从机设备遍历配置
* 参  数:
* 返回值:bool
**************************************************************************/
bool CCF_MstInit(void)
{
	bool res=1;
	char i=0;
	CCF_StDef *p2CCF = (CCF_StDef*) & CCFs;
	CCFEQP_Master_StDef* p2EQPMst = (CCFEQP_Master_StDef*)&EQPMsts;

	for (i = 0; i < EQPMsts.Number; i++)
	{
		if (p2EQPMst[i].CommId != CCFCN_Disable)//配置有效
		{
			uint8_t Temp_res = 0;
			if(p2CCF[p2EQPMst[i].CommId].InitRecord.SlvNum == 0)
			{
				if(p2CCF[p2EQPMst[i].CommId].InitRecord.InitRes == CCFInitRes_Init)
				{
					Temp_res = 1;
				}
			}
			else
			{
				if(p2CCF[p2EQPMst[i].CommId].InitRecord.InitRes == CCFInitRes_Mst)
				{
					Temp_res = 2;
				}
			}
			if(Temp_res != 0X00)//判断是否已经配置过
			{
				p2CCF[p2EQPMst[i].CommId].PubPara = p2EQPMst[i].PubPara;//公共参数
				p2CCF[p2EQPMst[i].CommId].MstPara = p2EQPMst[i].MstPara;//主站参数
				
				p2CCF[p2EQPMst[i].CommId].InitRecord.InitRes=CCFInitRes_Mst;//配置为主站

				//从站参数加入队列
				if(p2CCF[p2EQPMst[i].CommId].InitRecord.SlvNum <= SLVS_MAXNUM)
				{
					p2CCF[p2EQPMst[i].CommId].InitRecord.SlvNum++;//增加从站数量
					p2CCF[p2EQPMst[i].CommId].InitRecord.SlvPara[p2CCF[p2EQPMst[i].CommId].InitRecord.SlvNum-1] = p2EQPMst[i].MBConf;//加入从机参数队列
					p2CCF[p2EQPMst[i].CommId].InitRecord.ID[p2CCF[p2EQPMst[i].CommId].InitRecord.SlvNum-1] = p2EQPMst[i].SlaveId;//加入从机ID队列
					p2CCF[p2EQPMst[i].CommId].InitRecord.p2Mst[p2CCF[p2EQPMst[i].CommId].InitRecord.SlvNum-1] = &p2EQPMst[i];//加入主站指针队列
				}
				else
				{
					g_CodeErr.Bit.LengthLimited = 1;//从站数量超限	
					return 0;//从站数量超限
				}
			}
			else
			{
				g_CodeErr.Bit.InitErr = 1;//重复配置
				return 0;//重复配置
			}
			
		}

	}
	return res;
}

/**************************************************************************
* 函数名:CCF_SlvInit
* 功  能:从站遍历配置
* 参  数:
* 返回值:bool
**************************************************************************/
bool CCF_SlvInit(void)
{
	bool res=1;
	char i=0;
	CCF_StDef *p2CCF = (CCF_StDef*) & CCFs;
	CCFEQP_Slave_StDef* p2EQPSlv = (CCFEQP_Slave_StDef*)&EQPSlvs;
	
	for (i = 0; i < EQPSlvs.Number; i++)
	{
		if (p2EQPSlv[i].CommId != CCFCN_Disable)//接口配置有效
		{
			if (p2CCF[p2EQPSlv[i].CommId].InitRecord.InitRes == CCFInitRes_Init)//判断是否已经配置过
			{
				p2CCF[p2EQPSlv[i].CommId].PubPara = p2EQPSlv[i].PubPara;//公共参数
				p2CCF[p2EQPSlv[i].CommId].SlvPara = p2EQPSlv[i].SlvPara;//从站参数
				p2CCF[p2EQPSlv[i].CommId].InitRecord.p2Slv = &p2EQPSlv[i];//从站指针
				p2CCF[p2EQPSlv[i].CommId].InitRecord.InitRes=CCFInitRes_Slv;//配置为从站
			}
			else
			{
				g_CodeErr.Bit.InitErr = 1;
				return 0;
			}
		}
	}
	return res;
}

/**************************************************************************
* 函数名:CCF_Analyse
* 功  能:通信接口遍历分析，得到主站和从站数量，主站和从站端口ID队列
* 参  数:
* 返回值:bool
**************************************************************************/
bool CCF_Analyse()
{
	bool res=1;
	char i=0;
	CCFs_StDef *p2CCFs = &CCFs;
	CCF_StDef *p2CCF = (CCF_StDef*) & CCFs;

	for (i = 0; i < CCFs.Number; i++)
	{
		//硬件绑定编号初始化
		p2CCF[i].UartID = (CCFID_EnumDef)i;
		if (p2CCF[i].InitRecord.InitRes == CCFInitRes_Mst)//主站
		{
			p2CCFs->Record.MstNum++;//主站数量增加
			p2CCFs->Record.MstCCFID[p2CCFs->Record.MstNum - 1] = (CCFID_EnumDef)i;//主站ID加入队列
			
			//从机参数加入队列
			p2CCFs->Record.MstCCF[p2CCFs->Record.MstNum - 1].pRegindex = p2CCF[i].InitRecord.SlvPara;//从站参数队列
			p2CCFs->Record.MstCCF[p2CCFs->Record.MstNum - 1].Count = p2CCF[i].InitRecord.SlvNum;//从站数量
			p2CCFs->Record.MstCCF[p2CCFs->Record.MstNum - 1].mb_num = p2CCFs->Record.MstNum - 1;//主站编号
			p2CCFs->Record.QuickIndexs[p2CCFs->Record.MstNum - 1] = mb_reg_quick_indexs_init;//初始化快速索引
		}
		else if (p2CCF[i].InitRecord.InitRes == CCFInitRes_Slv)//从站
		{
			p2CCFs->Record.SlvNum++;
			p2CCFs->Record.SlvCCFID[p2CCFs->Record.SlvNum - 1] = (CCFID_EnumDef)i;
		}
	}
	return res;
}

/**************************************************************************
* 函数名:USARTxInit
* 功  能:物理接口参数导入
* 参  数:
* 返回值:bool
**************************************************************************/
bool USARTxInit(ST485_USART* p2USARTx, CCF_StDef* p2CCF)
{
	bool res = 1;
	char i = 0;

	if(p2USARTx == NULL || p2CCF == NULL)
	{
		g_CodeErr.Bit.NullPointer = 1;
		return 0;
	}

	for (i = 0; i < CCF_MAXNUM; i++)
	{
		p2USARTx[i].Ini_par.usartx = (USART_ID)i;
		p2USARTx[i].Ini_par.workmode = (RS485USART_WORKMODE)p2CCF[i].PubPara.Role;
		p2USARTx[i].Ini_par.baud_set = (unsigned char)p2CCF[i].PubPara.Baud;

		//临时补丁 9600的modbus主站 无校验
		if(p2CCF[i].PubPara.Baud == Baud_9600&&p2CCF[i].PubPara.Format == CCFRTU)
		{
			p2USARTx[i].Ini_par.comm_cfg_par = USART_WL7 | USART_PI_NONE | USART_ST_1;
		}
		//临时补丁 9600的私有协议 主站 无校验
		else if(p2CCF[i].PubPara.Baud == Baud_9600&&p2CCF[i].PubPara.Format == CCFCustomUART)
		{
			p2USARTx[i].Ini_par.comm_cfg_par = USART_WL7 | USART_PI_NONE | USART_ST_1;
		}
		//8位数据/偶校验/1个停止位
		else
		{
			p2USARTx[i].Ini_par.comm_cfg_par = USART_WL8 | USART_PI_EVEN | USART_ST_1;
		}
		p2USARTx[i].Ini_par.pi = p2CCF[i].PubPara.Pi;
	}
	return res;
}


//ccf复位
void CCF_InitReset(void)
{
	//CCF复位
	memset(&CCFs,0,sizeof(CCFs_StDef));
	CCFs.Number = sizeof(CCFs_StDef) / sizeof(CCF_StDef);
}
//通信接口配置初始化
bool CommConf_Init(void)
{
	ST485_USART* p2USARTx = RS485_USARTx;
	CCF_StDef* p2CCF = (CCF_StDef*)&CCFs;
	
	CCF_InitReset();			//ccf复位

	CCF_MstInit();				//从机设备遍历配置
	CCF_SlvInit();				//从站设备遍历配置
	CCF_Analyse();				//通信接口遍历分析
	USARTxInit(p2USARTx, p2CCF);//物理接口参数导入
	return 1;
}

//***********************************END OF FILE***********************************

