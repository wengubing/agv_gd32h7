/****************************Copyright (c)**********************************************
**					�q--------------------------�r
**					��	成都航发机器人有限公司	��
**					��	http://www.hangfa.com	��
**					�t--------------------------�s
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: LinkCanopenMaster.c
** 创 建 人: 文小兵
** 描    述: CANopen主站任务
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V2.0.0
** 创建日期: 2024年5月9日 重构
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define LinkCanopenMaster_GLOBALS

/* 历史版本记录

___dy dengyang ___2015.12.25

by biyabc   2018.03.13
1.修改主站PDO接收

*/

//自定义引用
#include "gd32h7xx.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "timer.h"
#include "canfestival.h"
#include "../drv_servo_gd32h7.h"
#include "LinkCanopenMaster.h"

//宏定义
#define   Config_step_Last_Kinco								45
#define   Config_step_Last_Delta								51
#define   Config_step_Last_Windcon							51
#define   Config_step_Last_Wheatstone					  53
#define   Config_step_Last_Keya     					  54
#define   Config_step_Last_Copley								45

#define   TRANS_EVERY_N_SYNC_TPDO1							35
#define   TRANS_EVERY_N_SYNC_TPDO2							30
#define   TRANS_EVERY_N_SYNC_TPDO3							22//故障代码
#define   TRANS_EVERY_N_SYNC_TPDO4							20//过载计数


//外部方法声明
extern void CanFestivalTimer_Config(uint8_t pi, FunctionalState NewState);
extern void CanFestivalTimer_Enable(FunctionalState NewState);
static UNS8 ConfigureSlaveNode_Kinco(CO_Data* d, UNS8 nodeId);
static UNS8 ConfigureSlaveNode_Delta(CO_Data* d, UNS8 nodeId);
static UNS8 ConfigureSlaveNode_Windcon(CO_Data* d, UNS8 nodeId);
static UNS8 ConfigureSlaveNode_Wheatstone(CO_Data* d, UNS8 nodeId);
static UNS8 ConfigureSlaveNode_Keya(CO_Data* d, UNS8 nodeId);
static UNS8 ConfigureSlaveNode_Copley(CO_Data* d, UNS8 nodeId);

//内部定义对象声明
UNS32 InitDataBuffer=0;

/*驱动器工作模式 --- 速度模式
signed char g_ucDriverMode[MAX_NodeId_NUM]={
                                            DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
	                                          DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
	                                          DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
	                                          DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
											                     };*/

signed char g_ucDriverMode[MAX_NodeId_NUM]={
	                                          DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
	                                          DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
	
	                                          DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
	                                          DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
											
											  DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
											  DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,

											  DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
											  DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,

											  DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
											  DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,

											  DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
											  DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,


											  DRIVER_MODE_VELOCITY,DRIVER_MODE_VELOCITY,
											                      DRIVER_MODE_VELOCITY,
                                            };

u8 NMT_Reset_NodeIDCnt = 0;

//依次交换4字节数据的位置
u32 u32_xch_hl(u32 i)
{
	union{
	u8 ch[4];
	u32  s;
	}u;
	u.ch[0]=(i>>24) & 0xff;
	u.ch[1]=(i>>16) & 0xff;
	u.ch[2]=(i>>8) & 0xff;
	u.ch[3]=i & 0xff;
	return(u.s);
}


/**************************************************************************/
/* Declaration of value range types                                       */
/**************************************************************************/
unsigned char canSend(CAN_PORT notused, Message *m)
{
    uint32_t i;
    GD32_CAN_TX_FIFO_t *pTxFifo = &GD32_CAN.tx_fifo;

    if (m->len <= 8U)
    {
        pTxFifo->msg[pTxFifo->write_adr].id  = m->cob_id;
        pTxFifo->msg[pTxFifo->write_adr].ide = 0U;    /* 标准帧 */
        pTxFifo->msg[pTxFifo->write_adr].rtr = m->rtr;
        pTxFifo->msg[pTxFifo->write_adr].dlc = m->len;
        for (i = 0U; i < m->len; i++) {
            pTxFifo->msg[pTxFifo->write_adr].data[i] = m->data[i];
        }
        if (++pTxFifo->write_adr >= CAN_TX_BUF_LENGTH_MAX) {
            pTxFifo->write_adr = 0U;
        }
    }

    return 0x00;
}

static void CheckSDOAndContinue_Read(CO_Data* d, UNS8 nodeId)
{
	UNS32 abortCode, size;
	void	*pdata=0;
	u8 id = nodeId-1,sta;

	//if(g_DHR_LinkCanOpen)
	{
		if(CanMaster.ConfigStep[id]==0)
		{//sn.
			size = 4;
			//size = 2;
			//pdata = (UNS32 *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_Driver_SN + 2*id];
			pdata = (UNS32*)&(SVO[id].SN);//0x2000测试读出来为21600,
			//测试size2 UNS32 读0x2002 得到1
			//读0x2D13 启动失败 2d04也不行
			//pdata = (UNS16*)&(SVO[id].SN);
		}

		if(pdata)
		{
			sta = getReadResultNetworkDict(d, nodeId, pdata, &size, &abortCode);
			if(sta == SDO_FINISHED)
			{
				CanMaster.ConfigStep[id]++;
			}			
			else if(sta != SDO_FINISHED)
			{
				eprintf("Master : Failed in initializing slave %2.2x, step %d, AbortCode :%4.4x \n", nodeId, init_step, abortCode);
			}
		}
		
		if(CanMaster.Para.SVOType == Can_SVOType_Kinco)
		{
		    ConfigureSlaveNode_Kinco(d, nodeId);	
		}
		else if(CanMaster.Para.SVOType == Can_SVOType_Delta)
		{
		    ConfigureSlaveNode_Delta(d, nodeId);	
		}
		else if(CanMaster.Para.SVOType == Can_SVOType_Windcon)
		{
		    ConfigureSlaveNode_Windcon(d, nodeId);	
		}	
		else if(CanMaster.Para.SVOType == Can_SVOType_Wheatstone)
		{
		    ConfigureSlaveNode_Wheatstone(d, nodeId);	
		}		
		else if(CanMaster.Para.SVOType == Can_SVOType_Keya)
		{
		    ConfigureSlaveNode_Keya(d, nodeId);	
		}				
		else if(CanMaster.Para.SVOType == Can_SVOType_Copley)
		{
		    ConfigureSlaveNode_Copley(d, nodeId);	
		}				
	}
}

static void CheckSDOAndContinue_Write(CO_Data* d, UNS8 nodeId)
{
		UNS32 abortCode;	
		u8 id = nodeId-1,sta;

		sta = getWriteResultNetworkDict (d, nodeId, &abortCode);
	
		if(sta == SDO_FINISHED)
		{
		    CanMaster.ConfigStep[id]++;
		}	
		else if(sta != SDO_FINISHED)
		{
		    eprintf("Master : Failed in initializing slave %2.2x, step %d, AbortCode :%4.4x \n", nodeId, init_step, abortCode);	
		}

		if(CanMaster.Para.SVOType == Can_SVOType_Kinco)
		{
		    ConfigureSlaveNode_Kinco(d, nodeId);	
		}
		else if(CanMaster.Para.SVOType == Can_SVOType_Delta)
		{
		    ConfigureSlaveNode_Delta(d, nodeId);	
		}	
		else if(CanMaster.Para.SVOType == Can_SVOType_Windcon)
		{
		    ConfigureSlaveNode_Windcon(d, nodeId);	
		}		
		else if(CanMaster.Para.SVOType == Can_SVOType_Wheatstone)
		{
		    ConfigureSlaveNode_Wheatstone(d, nodeId);	
		}		
		else if(CanMaster.Para.SVOType == Can_SVOType_Keya)
		{
		    ConfigureSlaveNode_Keya(d, nodeId);			
		}			
		else if(CanMaster.Para.SVOType == Can_SVOType_Copley)
		{
		    ConfigureSlaveNode_Copley(d, nodeId);	
		}			
}

/***************配置从机节点的相关参数...步科驱动器****************/
static UNS8 ConfigureSlaveNode_Kinco(CO_Data* d, UNS8 nodeId)
{
	UNS8 res;
	u8 id = nodeId-1;

		switch(CanMaster.ConfigStep[id])
		{
				/*-----------------------读操作-----------------------*/
				case 0:
				{//读取sn		
					res = readNetworkDictCallback (d,nodeId,0x1018,0x03,0x00,CheckSDOAndContinue_Read,0);
				}
				break;
				
				/*-----------------------写操作-----------------------*/		
				case 1:
				{//工作模式设置...
						UNS8 SetMode = g_ucDriverMode[id];	
						UNS16 index = 0x6060;
						INTEGER8 size = sizeof(SetMode);
						res = writeNetworkDictCallBack (d,nodeId,index,0x00,size,0,&SetMode,CheckSDOAndContinue_Write,0);	
				}
				break;
				
				case 2:
				{
						UNS8 SetMode = 0x01;//设置旋转方向 顺时针方向为正：1   逆时针为正:0
						UNS8 size = sizeof(SetMode);
						res = writeNetworkDictCallBack (d,nodeId,0x607E,0x00,size,0,&SetMode,CheckSDOAndContinue_Write,0);				
				}
				break;
				
				case 3:
				{
					UNS32 	Speed_Up	= 0;
					UNS8	size		= 0;
					//4代 步科 和 5代步科的编码器设计兼容
					if(SVO[id].SvoType == SvoType_Kinco_5)
					{
//						Speed_Up = 53687;	//加速度 50rps/s				
						Speed_Up = 26843;	//加速度 25rps/s			
					}
					else
					{

						//单位换算关系为DEC=[(rps/s*65536*编码器分辨率)/4000000]
						//编码器分辨率为2500*4
//						Speed_Up = 163840;//加速度 1000rps/s	
//						Speed_Up = 81920;//加速度 500rps/s			
//						Speed_Up = 16384;//加速度 100rps/s					
//						Speed_Up = 8192;//加速度 50rps/s				
						Speed_Up = 4096;//加速度 25rps/s				
//						Speed_Up = 3276;//加速度 20rps/s			
//						Speed_Up = 1638;//加速度 10rps/s
					}

					size 	= sizeof(Speed_Up);
					res 	= writeNetworkDictCallBack (d,nodeId,0x6083,0x00,size,0,&Speed_Up,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 4:
				{
					UNS32 	Speed_Down 	= 0;
					UNS8 	size		= 0;
					//4代 步科 和 5代步科的编码器设计兼容
					if(SVO[id].SvoType == SvoType_Kinco_5)
					{
//						Speed_Down = 53687;	//加速度 50rps/s				
						Speed_Down = 26843;	//加速度 25rps/s			
					}
					else
					{

						//单位换算关系为DEC=[(rps/s*65536*编码器分辨率)/4000000]
						//编码器分辨率为2500*4				
//						 Speed_Down = 245760;//减速度 1500rps/s	
//						 Speed_Down = 81920;//减速度 500rps/s		
//						 Speed_Down = 16384;//减速度 100rps/s						
						 Speed_Down = 8192;//减速度 50rps/s			
//						 Speed_Down = 4096;//减速度 25rps/s					
//						 Speed_Down = 3276;//减速度 20rps/s			
//						 Speed_Down = 1638;//减速度 10rps/s		
					}
			
						size 	= sizeof(Speed_Down);
						res		= writeNetworkDictCallBack (d,nodeId,0x6084,0x00,size,0,&Speed_Down,CheckSDOAndContinue_Write,0);
				}
				break;

				/*-----------------------------------配置PDO参数:通讯参数和映射参数-------------------------------------------------------------------*/
				/*---------------------------------------Slave的TPDO1配置------------------------------------------------------------*/
				case 5:
				{//设置TPDO1 mappingPara_index_0x1A00---subindex_number
						UNS8 subindex_number = 0;//disable the tpdo1.
						UNS8 size = sizeof(subindex_number);	
						CanMaster.ConfigStep[id]=5;//前面有空缺的操作时,需要添加该赋值
						res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 6:
				{//设置TPDO1 ComunicationPara_index_0x1800,发送类型:循环同步方式
						UNS8 TransType = TRANS_SYNC_MIN;//定义同步包数量为:2(收到2个同步包后才发送)	
						UNS8 size = sizeof(TransType);		
						res = writeNetworkDictCallBack (d,nodeId,0x1800,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 7:
				{//母线电压:0.1V
						UNS32 MappIndexData = CanObjectDict_TPDO1_Kinco_Map[0];//映射到index:6079的subindex:00,对象是：32位
						UNS8 size = sizeof(MappIndexData);
						res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 8:
				{//电机实际电流:0.01A
						UNS32 MappIndexData = CanObjectDict_TPDO1_Kinco_Map[1];//映射到index:6078的subindex:00,对象是：16位
						UNS8 size = sizeof(MappIndexData);
						CanMaster.ConfigStep[id]=8;
						res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 9:
				//		{
				//		}
				//		break;
				case 10:
				//		{
				//		}
				//		break;
				
				case 11:
				{//设置TPDO1 mappingPara_index_0x1A00---subindex_number
						UNS8 subindex_number = 2;//enable the tpdo1.
						UNS8 size = sizeof(subindex_number);			
						CanMaster.ConfigStep[id]=11;			
						res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
				}
				break;
				
				/*---------------------------------------Slave的TPDO2配置------------------------------------------------------------*/
				case 12:
				{//设置TPDO2 mappingPara_index_0x1A01---subindex_number
						UNS8 subindex_number = 0;//disable the tpdo2.
						UNS8 size = sizeof(subindex_number);			
						CanMaster.ConfigStep[id]=12;//前面有空缺的操作时,需要添加该赋值			
						res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 13:
				{//设置TPDO2 ComunicationPara_index_0x1801,发送类型:循环同步方式
						UNS8 TransType = TRANS_SYNC_MIN;			
						UNS8 size = sizeof(TransType);		
						res = writeNetworkDictCallBack (d,nodeId,0x1801,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 14:
				{//状态字:0x6041
						UNS32 MappIndexData = CanObjectDict_TPDO2_Kinco_Map[0];//映射到index:6041的subindex:00,对象是：16位
						UNS8 size = sizeof(MappIndexData);
						res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 15:
				{
						//工作模式:0x6060
						UNS32 MappIndexData = CanObjectDict_TPDO2_Kinco_Map[1];//映射到index:6061的subindex:00,对象是：8位
						UNS8 size = sizeof(MappIndexData);
						res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 16:
				{
						//实际位置:0x6063
						UNS32 MappIndexData = CanObjectDict_TPDO2_Kinco_Map[2];//映射到index:6063的subindex:00,对象是：32位
						UNS8 size = sizeof(MappIndexData);
						res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x03,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 17:
				{//设置TPDO2 mappingPara_index_0x1A01---subindex_number
						UNS8 subindex_number = 3;//enable the tpdo2.
						UNS8 size = sizeof(subindex_number);	
						res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
				}
				break;
				
				/*---------------------------------------Slave的TPDO3配置------------------------------------------------------------*/
				case 18:
				{//设置TPDO3 mappingPara_index_0x1A02---subindex_number
						UNS8 subindex_number = 0;//disable the tpdo3.
						UNS8 size = sizeof(subindex_number);			
						res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 19:
				{//设置TPDO3 ComunicationPara_index_0x1802,发送类型:循环同步方式
						UNS8 TransType = TRANS_SYNC_MIN;			
						UNS8 size = sizeof(TransType);		
						res = writeNetworkDictCallBack (d,nodeId,0x1802,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 20:
				{//电机的实际转速:0x606C
						UNS32 MappIndexData = CanObjectDict_TPDO3_Kinco_Map[0];//映射到index:606C的subindex:00,对象是：32位
						UNS8 size = sizeof(MappIndexData);
						res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);			
				}
				break;
				
				case 21:
				{//电机的故障锁存:0x2601
						UNS32 MappIndexData = CanObjectDict_TPDO3_Kinco_Map[1];//映射到index:2601的subindex:00,对象是：16位
						UNS8 size = sizeof(MappIndexData);
						res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);			
				}
				break; 
				
				case 22:
				{//电机的故障锁存:0x2602
						UNS32 MappIndexData = CanObjectDict_TPDO3_Kinco_Map[2];//映射到index:2602的subindex:00,对象是：16位
						UNS8 size = sizeof(MappIndexData);
						res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x03,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);			
				}
				break; 	
				
				case 23:
				{//设置TPDO3 mappingPara_index_0x1A02---subindex_number
						UNS8 subindex_number = 3;//enable the tpdo3.
						UNS8 size = sizeof(subindex_number);			
						res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
				}
				break;  
				
				/*---------------------------------------Slave的TPDO4配置------------------------------------------------------------*/        
				case 24:
				{//设置TPDO4 mappingPara_index_0x1A03---subindex_number
						UNS8 subindex_number = 0;//disable the tpdo4.
						UNS8 size = sizeof(subindex_number);			
						CanMaster.ConfigStep[id]=24;//前面有空缺的操作时,需要添加该赋值			
						res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
				}
				break;   

				/*---------------------------------------Slave的TPDO配置__COB_ID------------------------------------------------------------*/
				case 25:
				{
						UNS32 COB_ID = 0x181+id;  //TPDO1_COB_ID
						UNS8 size = sizeof(COB_ID);
						res = writeNetworkDictCallBack (d,nodeId,0x1800,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
				}		
				break;    
				
				case 26:
				{
						UNS32 COB_ID = 0x281+id;  //TPDO2_COB_ID
						UNS8 size = sizeof(COB_ID);
						res = writeNetworkDictCallBack (d,nodeId,0x1801,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 27:
				{
						UNS32 COB_ID = 0x381+id;  //TPDO3_COB_ID
						UNS8 size = sizeof(COB_ID);
						res = writeNetworkDictCallBack (d,nodeId,0x1802,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
				}
				break; 
				
				/*---------------------------------------Slave的RPDO配置__COB_ID------------------------------------------------------------*/    
				case 28:
				{
						UNS32 COB_ID = 0x201+id;  //RPDO1_COB_ID
						UNS8 size = sizeof(COB_ID);
						res = writeNetworkDictCallBack (d,nodeId,0x1400,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
				}
				break; 
				
				case 29:
				{
						if((g_ucDriverMode[id]==DRIVER_MODE_VELOCITY) || (g_ucDriverMode[id]==DRIVER_MODE_ImmediateVelocity))   //速度模式 || 立即速度模式
						{
								UNS16 Kp = 0;//位置环Kp
								UNS8 size = sizeof(Kp);
								res = writeNetworkDictCallBack (d,nodeId,0x60FB,0x01,size,0,&Kp,CheckSDOAndContinue_Write,0);
						}
						else if(g_ucDriverMode[id]==DRIVER_MODE_POSATION)
						{
								UNS16 Kp = 1;//位置环Kp
								UNS8 size = sizeof(Kp);
								res = writeNetworkDictCallBack (d,nodeId,0x60FB,0x01,size,0,&Kp,CheckSDOAndContinue_Write,0);
						}				
				}
				//		break;     
				case 30:
				{

				}
				//		break;  
				
				/*---------------------------------------Slave的RPDO1配置------------------------------------------------------------*/
				case 31:
				{//设置RPDO1 mappingPara_index_0x1600---subindex_number
						UNS8 subindex_number = 0;//disable the rpdo1.
						UNS8 size = sizeof(subindex_number);
						CanMaster.ConfigStep[id]=31;//前面有空缺的操作时,需要添加该赋值
						res = writeNetworkDictCallBack (d,nodeId,0x1600,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 32:
				{//设置RPDO1 ComunicationPara_index_0x1400,发送类型:循环同步方式
						UNS8 TransType = TRANS_SYNC_MIN;			
						UNS8 size = sizeof(TransType);		
						res = writeNetworkDictCallBack (d,nodeId,0x1400,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 33://控制输入  0x6040 or 0x2191
				{
						UNS32 MappIndexData = CanObjectDict_RPDO1_Map[0];//映射到index:xxxx的subindex:00,对象是：16位
						UNS8 size = sizeof(MappIndexData);
						res = writeNetworkDictCallBack (d,nodeId,0x1600,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 34:
				{//速度命令输入  0x60FF or 0x2134 or 0x2340
						if((g_ucDriverMode[id]==DRIVER_MODE_VELOCITY) || (g_ucDriverMode[id]==DRIVER_MODE_ImmediateVelocity))   //速度模式 || 立即速度模式
						{
								//速度命令输入  0x60FF or 0x2134 or 0x2340
								UNS32 MappIndexData = CanObjectDict_RPDO1_Map[1];//映射到index:XXXX的subindex:00,对象是：32位
								UNS8 size = sizeof(MappIndexData);
								res = writeNetworkDictCallBack (d,nodeId,0x1600,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
						}
						else if(g_ucDriverMode[id]==DRIVER_MODE_POSATION)
						{
								//位置命令输入  0x607A
								UNS32 MappIndexData = CanObjectDict_RPDO1_Map_FPMode[1];//CanObjectDict_RPDO1_Map[1];//映射到index:XXXX的subindex:00,对象是：32位
								UNS8 size = sizeof(MappIndexData);
								res = writeNetworkDictCallBack(d,nodeId,0x1600,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
						}	
				}
				break;
				
				case 35:
				{//设置RPDO1 mappingPara_index_0x1600---subindex_number
						UNS8 subindex_number = 2;//enable the rpdo1.
						UNS8 size = sizeof(subindex_number);		
						res = writeNetworkDictCallBack (d,nodeId,0x1600,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
				}
				break;
				
				/*---------------------------------------Slave的RPDO2配置------------------------------------------------------------*/
				case 36:
				{//设置RPDO2 mappingPara_index_0x1601---subindex_number
						UNS8 subindex_number = 0;//disable the rpdo2.
						UNS8 size = sizeof(subindex_number);		
						res = writeNetworkDictCallBack (d,nodeId,0x1601,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
				}
				break;
				
				/*---------------------------------------Slave的RPDO3配置------------------------------------------------------------*/
				case 37:
				{//设置RPDO3 mappingPara_index_0x1602---subindex_number
						UNS8 subindex_number = 0;//disable the rpdo3.
						UNS8 size = sizeof(subindex_number);		
						res = writeNetworkDictCallBack (d,nodeId,0x1602,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
				}
				break;
				
				/*---------------------------------------Slave的RPDO4配置------------------------------------------------------------*/
				case 38:
				{//设置RPDO4 mappingPara_index_0x1603---subindex_number
						UNS8 subindex_number = 0;//disable the rpdo4.
						UNS8 size = sizeof(subindex_number);		
						res = writeNetworkDictCallBack (d,nodeId,0x1603,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
				}
				break;
				
				/*---------------------------------------事件相关参数------------------------------------------------------------*/
				case 39:
				{//设置Slave心跳发送周期--ms
				    UNS16 ProducerHeatbeatTimePeriod = Producer_Heartbeat_Times;				
						UNS8  size = sizeof(ProducerHeatbeatTimePeriod);
						CanMaster.ConfigStep[id]=39;//前面有空缺的操作时,需要添加该赋值
						res = writeNetworkDictCallBack (d,nodeId,0x1017,0x00,size,0,&ProducerHeatbeatTimePeriod,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 40:
				{//设置Slave同步通讯循环周期(及同步报文的接收间隔时间值)--ms
						UNS32 CommunicationCyclePeriod = SYNC_CommunicationCyclePeriod;//-----A value of zero in this object disables SYNC message production.
//						UNS32 CommunicationCyclePeriod = 0;//-----A value of zero in this object disables SYNC message production.				
						UNS8  size = sizeof(CommunicationCyclePeriod);
						res = writeNetworkDictCallBack (d,nodeId,0x1006,0x00,size,0,&CommunicationCyclePeriod,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 41:
				{//设置Slave节点守护时间值--ms
						UNS16 Node_Guardingtime = NMT_GuardTime;
						UNS8  size = sizeof(Node_Guardingtime);
						res = writeNetworkDictCallBack (d,nodeId,0x100C,0x00,size,0,&Node_Guardingtime,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 42:
				{//设置Slave节点守护寿命因子
						UNS8 Node_LifeTimeFactor = NMT_LifeTimeFactor;
						UNS8  size = sizeof(Node_LifeTimeFactor);
						res = writeNetworkDictCallBack (d,nodeId,0x100D,0x00,size,0,&Node_LifeTimeFactor,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 43:
				{//开启节点守护报警功能 开启：1   关闭：0
						UNS16 Node_GuardingErrorAction = 1;//调试时，关闭驱动器报警；发货或交付时，必须打开该功能     
						UNS8  size = sizeof(Node_GuardingErrorAction);
						res = writeNetworkDictCallBack (d,nodeId,0x6007,0x00,size,0,&Node_GuardingErrorAction,CheckSDOAndContinue_Write,0);
				}
				break;
				
				case 44:
				{
						if((g_ucDriverMode[id]==DRIVER_MODE_VELOCITY) || (g_ucDriverMode[id]==DRIVER_MODE_ImmediateVelocity))   //速度模式 || 立即速度模式
						{		
								CanMaster.ConfigStep[id] = Config_step_Last_Kinco;				
						}
						else if(g_ucDriverMode[id]==DRIVER_MODE_POSATION)
						{
								//设置位置环工作速度
								//单位换算关系为DEC=[(rpm*512*编码器分辨率)/1875]
								//对于回转电机，分辨率为65536
								//							UNS32 Node_velocity = 8947848;					//500rpm	
								//							UNS32 Node_velocity = 42949017;					//2400rpm，3000rpm*0.8	
								UNS32 Node_velocity = 17895697;					  //1000rpm，3000rpm*0.8						  
								UNS8  size = sizeof(Node_velocity);
								res = writeNetworkDictCallBack (d,nodeId,0x6081,0x00,size,0,&Node_velocity,CheckSDOAndContinue_Write,0);

								break;
						}
				}
				
				/*-----------------------启动从机-----------------------*/
				case 45:
				{
						CanMaster.SlaveParaConfigId[id]=nodeId;
						CanMaster.ConfigStep[id] = Config_step_Last_Kinco;
//						masterSendNMTstateChange (d, nodeId, NMT_Start_Node);
				}
				break;
				
				default:
				    break;
		} 
	
		if(CanMaster.ConfigStep[id] > Config_step_Last_Kinco)
		{
		    CanMaster.ConfigStep[id] = Config_step_Last_Kinco;
		}
		
	  return res;
}

/***************配置从机节点的相关参数...台达驱动器****************/
static UNS8 ConfigureSlaveNode_Delta(CO_Data* d, UNS8 nodeId)
{
	UNS8 res;
	u8 id = nodeId-1;

	switch(CanMaster.ConfigStep[id])
	{
		/*-----------------------读操作-----------------------*/
		case 0:
		{//读取固件版本
			res = readNetworkDictCallback (d,nodeId,0x2000,0x00,0x00,CheckSDOAndContinue_Read,0);
		}
		break;  
		
		/*-----------------------写操作-----------------------*/		
		case 1:
		{//设置P2_30辅助寄存器
			INTEGER16 SetData = 5;//启动后,各个写入参数不断电保存!!!
			UNS16 index = 0x221E;
			UNS8 size = sizeof(SetData);
			res = writeNetworkDictCallBack (d,nodeId,index,0x00,size,0,&SetData,CheckSDOAndContinue_Write,0);
		}
		break;   
		
		case 2:
		{//工作模式设置...
			UNS8 SetMode = 3;//Profile Velocity mode---0x03	
			UNS16 index = 0x6060;
			UNS8 size = sizeof(SetMode);
			res = writeNetworkDictCallBack (d,nodeId,index,0x00,size,0,&SetMode,CheckSDOAndContinue_Write,0);
		}
		break;     
		
		case 3:
		{//设置:Profile acceleration:6083h
			UNS32 SetData = 3000;  //millisecond (time from 0 rpm to 3000 rpm)
			UNS16 index = 0x6083;
			UNS8 size = sizeof(SetData);
			res = writeNetworkDictCallBack (d,nodeId,index,0x00,size,0,&SetData,CheckSDOAndContinue_Write,0);
		}
		break;  
		
		case 4:
		{//设置:Profile deceleration:6084h
			UNS32 SetData = 1000;  //millisecond (time from 0 rpm to 3000 rpm)
			UNS16 index = 0x6084;
			UNS8 size = sizeof(SetData);
			res = writeNetworkDictCallBack (d,nodeId,index,0x00,size,0,&SetData,CheckSDOAndContinue_Write,0);
		}
		break;   
		
		/*通过通信配置力矩限制P参数*/
		case 5:
		{
			//设置:P1 - 02 打开力矩限制 : 2102h
			INTEGER16 SetData = 0x0010;
			UNS16 index = 0x2102;
			UNS8 size = sizeof(SetData);
			res = writeNetworkDictCallBack(d, nodeId, index, 0x00, size, 0, &SetData, CheckSDOAndContinue_Write, 0);
		}
		break;
		
		case 6:
		{
			//设置:P2 - 11 常闭节点指定扭矩命令 : 220Bh，B3M可以，B3E不行；但B3M 6072h无效，B3E却有效
			//6072h默认3500，单位0.1%，210Ch默认100，单位1%，平时设到400,0速度时设到90看效果，P2-034默认速度跟踪误差报警为5000，不会报警
			INTEGER16 SetData = 0x016;
			UNS16 index = 0x220B;
			UNS8 size = sizeof(SetData);
			res = writeNetworkDictCallBack(d, nodeId, index, 0x00, size, 0, &SetData, CheckSDOAndContinue_Write, 0);
		}
		break;
		
		case 7:
		{
			/*
			//设置PM.019 负载上升增益 参数 默认100
			UNS16 SetData = SVO[id].LoadAdd;
			UNS16 index = 0x2D13;
			UNS8 size = sizeof(SetData);
			//res = writeNetworkDictCallBack(d, nodeId, index, 0x00, size, 0, &SetData, CheckSDOAndContinue_Write, 0);
			//UNS16 、INTEGER16 都无法成功
			*/

			//P2.034 S16 0~30000,rpm 默认5000，改大尝试解决AL007
			//UNS16 SetData = 6000;
			//UNS16 index = 0x2222;
			//UNS8 size = sizeof(SetData);
			//res = writeNetworkDictCallBack(d, nodeId, index, 0x00, size, 0, &SetData, CheckSDOAndContinue_Write, 0);
		}
		//break;
		
		case 8:
		{
			/*
			//设置PM.020 负载下降增益 参数 默认100
			INTEGER16 SetData = SVO[id].LoadReduce;
			UNS16 index = 0x2D14;
			UNS8 size = sizeof(SetData);
			//res = writeNetworkDictCallBack(d, nodeId, index, 0x00, size, 0, &SetData, CheckSDOAndContinue_Write, 0);
			*/

			//P2.006 S16 0~1023,rpm 默认100，改小尝试解决电机去追位置
			//UNS16 SetData = 80;
			//UNS16 index = 0x2206;
			//UNS8 size = sizeof(SetData);
			//res = writeNetworkDictCallBack(d, nodeId, index, 0x00, size, 0, &SetData, CheckSDOAndContinue_Write, 0);
		}
		//break;
		
		case 9:
		{
			//设置 P0_20（对象字典0x2014，INT16）为 -91，让0x200C里装载保护计数
			INTEGER16 SetData = -91;
			//INTEGER16 SetData = 7;//读速度ok
			UNS16 index = 0x2014;
			UNS8 size = sizeof(SetData);
			CanMaster.ConfigStep[id]=9;//前面有空缺的操作时,需要添加该赋值			
			res = writeNetworkDictCallBack(d, nodeId, index, 0x00, size, 0, &SetData, CheckSDOAndContinue_Write, 0);
		}
		break; 
		
		/*-----------------------------------配置PDO参数:通讯参数和映射参数-------------------------------------------------------------------*/
		/*---------------------------------------Slave的TPDO1配置------------------------------------------------------------*/
		case 10:
		{//设置TPDO1 mappingPara_index_0x1A00---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo1.
			UNS8 size = sizeof(subindex_number);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 11:
			{//设置TPDO1 ComunicationPara_index_0x1800,COB_ID配置:PDO exists / is valid
			UNS32 CobId = 0x80000180 + nodeId;//MSB为"0",开启PDO传输；MSB为"1",关闭PDO传输
			UNS8 size = sizeof(CobId);
			res = writeNetworkDictCallBack (d,nodeId,0x1800,0x01,size,0,&CobId,CheckSDOAndContinue_Write,0);
		}
		break;			
		
		case 12:
		{//设置TPDO1 ComunicationPara_index_0x1800,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(20);//定义同步包数量为:x(收到x个同步包后才发送)
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO1;
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1800,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 13:
		{//P0_09:母线电压
			UNS32 MappIndexData = CanObjectDict_TPDO1_Delta_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 14:
		{//P0_10:IGBT温度
			UNS32 MappIndexData = CanObjectDict_TPDO1_Delta_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 15:
		{//设置TPDO1 mappingPara_index_0x1A00---subindex_number
			UNS8 subindex_number = 2;//enable the tpdo1.
			UNS8 size = sizeof(subindex_number);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 16:
			{//设置TPDO1 ComunicationPara_index_0x1800,COB_ID配置:PDO exists / is valid
			UNS32 CobId = 0x180 + nodeId;//MSB为"0",开启PDO传输；MSB为"1",关闭PDO传输
			UNS8 size = sizeof(CobId);
			res = writeNetworkDictCallBack (d,nodeId,0x1800,0x01,size,0,&CobId,CheckSDOAndContinue_Write,0);
		}
		break;			
				
		/*---------------------------------------Slave的TPDO2配置------------------------------------------------------------*/
		case 17:
		{//设置TPDO2 mappingPara_index_0x1A01---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo2.
			UNS8 size = sizeof(subindex_number);           
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 18:
			{//设置TPDO1 ComunicationPara_index_0x1801,COB_ID配置:PDO exists / is valid
			UNS32 CobId = 0x80000280 + nodeId;//MSB为"0",开启PDO传输；MSB为"1",关闭PDO传输
			UNS8 size = sizeof(CobId);
			res = writeNetworkDictCallBack (d,nodeId,0x1801,0x01,size,0,&CobId,CheckSDOAndContinue_Write,0);
		}
		break;

		case 19:
		{//设置TPDO2 ComunicationPara_index_0x1801,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(19);//定义同步包数量为:x(收到x个同步包后才发送)		
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO2;
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1801,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 20:
		{//P0_11:电机转速
			UNS32 MappIndexData = CanObjectDict_TPDO2_Delta_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 21:
		{//P0_12:回授电流
			UNS32 MappIndexData = CanObjectDict_TPDO2_Delta_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 22:
		{//设置TPDO2 mappingPara_index_0x1A01---subindex_number
			UNS8 subindex_number = 2;//enable the tpdo2.
			UNS8 size = sizeof(subindex_number);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;   
		
		case 23:
			{//设置TPDO2 ComunicationPara_index_0x1801,COB_ID配置:PDO exists / is valid
			UNS32 CobId = 0x280 + nodeId;//MSB为"0",开启PDO传输；MSB为"1",关闭PDO传输
			UNS8 size = sizeof(CobId);
			res = writeNetworkDictCallBack (d,nodeId,0x1801,0x01,size,0,&CobId,CheckSDOAndContinue_Write,0);
		}
		break;	
				
		/*---------------------------------------Slave的TPDO3配置------------------------------------------------------------*/
		case 24:
		{//设置TPDO3 mappingPara_index_0x1A02---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo3.
			UNS8 size = sizeof(subindex_number);	          
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;	
		
		case 25:
			{//设置TPDO2 ComunicationPara_index_0x1802,COB_ID配置:PDO exists / is valid
			UNS32 CobId = 0x80000380 + nodeId;//MSB为"0",开启PDO传输；MSB为"1",关闭PDO传输
			UNS8 size = sizeof(CobId);
			res = writeNetworkDictCallBack (d,nodeId,0x1802,0x01,size,0,&CobId,CheckSDOAndContinue_Write,0);
		}
		break;	

		case 26:
		{//设置TPDO3 ComunicationPara_index_0x1802,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(18);//定义同步包数量为:x(收到x个同步包后才发送)		
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO3;
			UNS8 size = sizeof(TransType);		
			res = writeNetworkDictCallBack (d,nodeId,0x1802,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 27:
		{//状态字(RO)
			UNS32 MappIndexData = CanObjectDict_TPDO3_Delta_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);			
		}
		break;
		
		case 28:
		{//错误代码(RO)
			UNS32 MappIndexData = CanObjectDict_TPDO3_Delta_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);	
		}
		break;
		
		case 29:
		{//工作模式(RO)
			UNS32 MappIndexData = CanObjectDict_TPDO3_Delta_Map[2];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x03,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);	
		}
		break;
		
		case 30:
		{//设置TPDO4 mappingPara_index_0x1A02---subindex_number
			UNS8 subindex_number = 3;//enable the tpdo3.
			UNS8 size = sizeof(subindex_number);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;  

		case 31:
			{//设置TPDO2 ComunicationPara_index_0x1802,COB_ID配置:PDO exists / is valid
			UNS32 CobId = 0x380 + nodeId;//MSB为"0",开启PDO传输；MSB为"1",关闭PDO传输
			UNS8 size = sizeof(CobId);
			res = writeNetworkDictCallBack (d,nodeId,0x1802,0x01,size,0,&CobId,CheckSDOAndContinue_Write,0);
		}
		break;	

		/*---------------------------------------Slave的TPDO4配置------------------------------------------------------------*/
		case 32:
		{//设置TPDO4 mappingPara_index_0x1A03---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo4.
			UNS8 size = sizeof(subindex_number);            
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;	
		
		case 33:
			{//设置TPDO4 ComunicationPara_index_0x1803,COB_ID配置:PDO exists / is valid
			UNS32 CobId = 0x80000480 + nodeId;//MSB为"0",开启PDO传输；MSB为"1",关闭PDO传输
			UNS8 size = sizeof(CobId);
			res = writeNetworkDictCallBack (d,nodeId,0x1803,0x01,size,0,&CobId,CheckSDOAndContinue_Write,0);
		}
		break;			

		case 34:
		{//设置TPDO4 ComunicationPara_index_0x1803,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(17);//定义同步包数量为:x(收到x个同步包后才发送)		
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO4;
			UNS8 size = sizeof(TransType);		
			res = writeNetworkDictCallBack (d,nodeId,0x1803,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 35:
		{//P0_11:驱动器错误码
			UNS32 MappIndexData = CanObjectDict_TPDO4_Delta_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);			
		}
		break;
		
		case 36:
		{//P0_12:保护计数
			UNS32 MappIndexData = CanObjectDict_TPDO4_Delta_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);			
		}
		break;
		
		case 37:
		{//设置TPDO3 mappingPara_index_0x1A02---subindex_number
			UNS8 subindex_number = 2;//enable the tpdo4.
			UNS8 size = sizeof(subindex_number);			
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 38:
			{//设置TPDO4 ComunicationPara_index_0x1803,COB_ID配置:PDO exists / is valid
			UNS32 CobId = 0x480 + nodeId;//MSB为"0",开启PDO传输；MSB为"1",关闭PDO传输
			UNS8 size = sizeof(CobId);
			res = writeNetworkDictCallBack (d,nodeId,0x1803,0x01,size,0,&CobId,CheckSDOAndContinue_Write,0);
		}
		break;	
				
		/*---------------------------------------Slave的RPDO1配置------------------------------------------------------------*/
		case 39:
		{//设置RPDO1 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo1.
			UNS8 size = sizeof(subindex_number);
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
	
		case 40:
			{//设置RPDO1 ComunicationPara_index_0x1400,COB_ID配置:PDO exists / is valid
			UNS32 CobId = 0x80000200 + nodeId;//MSB为"0",开启PDO传输；MSB为"1",关闭PDO传输
			UNS8 size = sizeof(CobId);
			res = writeNetworkDictCallBack (d,nodeId,0x1400,0x01,size,0,&CobId,CheckSDOAndContinue_Write,0);
		}
		break;			

		case 41:
		{//设置RPDO1 ComunicationPara_index_0x1400,发送类型:循环同步方式
			UNS8 TransType = TRANS_EVERY_N_SYNC(1);			
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1400,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;

		case 42:
		{//输入控制字
			UNS32 MappIndexData = CanObjectDict_RPDO1_Map[0];//映射到index:xxxx的subindex:00,对象是：16位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 43:
		{//速度命令输入
			UNS32 MappIndexData = CanObjectDict_RPDO1_Map[1];//映射到index:XXXX的subindex:00,对象是：32位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 44:
		{//力矩限制输入
			UNS32 MappIndexData = CanObjectDict_RPDO1_Map[2];//映射到index:XXXX的subindex:00,对象是：16位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x03,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 45:
		{//设置RPDO1 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 3;//enable the rpdo1.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 46:
			{//设置RPDO1 ComunicationPara_index_0x1400,COB_ID配置:PDO exists / is valid
			UNS32 CobId = 0x200 + nodeId;//MSB为"0",开启PDO传输；MSB为"1",关闭PDO传输
			UNS8 size = sizeof(CobId);
			res = writeNetworkDictCallBack (d,nodeId,0x1400,0x01,size,0,&CobId,CheckSDOAndContinue_Write,0);
		}
		break;			
				
		/*---------------------------------------事件相关参数------------------------------------------------------------*/
		case 47:
		{//设置Slave心跳发送周期--ms
			UNS16 ProducerHeatbeatTimePeriod = Producer_Heartbeat_Times;
			UNS8  size = sizeof(ProducerHeatbeatTimePeriod);
			res = writeNetworkDictCallBack (d,nodeId,0x1017,0x00,size,0,&ProducerHeatbeatTimePeriod,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 48:
		{//设置Slave同步通讯循环周期(及同步报文的接收间隔时间值)--ms
//			UNS32 CommunicationCyclePeriod = SYNC_CommunicationCyclePeriod;    //台达A2系列，该对象字典单位为ms
			UNS32 CommunicationCyclePeriod = SYNC_CommunicationCyclePeriod*1000; //台达A3和B3系列，该对象字典单位为us        
			UNS8  size = sizeof(CommunicationCyclePeriod);
			res = writeNetworkDictCallBack (d,nodeId,0x1006,0x00,size,0,&CommunicationCyclePeriod,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 49:
		{//设置Slave节点守护时间值--ms
			UNS16 Node_Guardingtime = NMT_GuardTime;
			UNS8  size = sizeof(Node_Guardingtime);
			res = writeNetworkDictCallBack (d,nodeId,0x100C,0x00,size,0,&Node_Guardingtime,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 50:
		{//设置Slave节点守护寿命因子
			UNS8 Node_LifeTimeFactor = NMT_LifeTimeFactor;
			UNS8  size = sizeof(Node_LifeTimeFactor);
			res = writeNetworkDictCallBack (d,nodeId,0x100D,0x00,size,0,&Node_LifeTimeFactor,CheckSDOAndContinue_Write,0);
		}
		break;

		/*-----------------------启动从机-----------------------*/
		case 51:
		{
		    CanMaster.SlaveParaConfigId[id]=nodeId;
				CanMaster.ConfigStep[id] = Config_step_Last_Delta;
//				masterSendNMTstateChange (d, nodeId, NMT_Start_Node);
		}
		break;	

		default:
		break;
	}
	
	if(CanMaster.ConfigStep[id] > Config_step_Last_Delta)
	{
		CanMaster.ConfigStep[id] = Config_step_Last_Delta;
	}    
	
	return res;
}


/***************配置从机节点的相关参数...风得控驱动器****************/
static UNS8 ConfigureSlaveNode_Windcon(CO_Data* d, UNS8 nodeId)
{
	UNS8 res;
	u8 id = nodeId-1;

	switch(CanMaster.ConfigStep[id])
	{
		/*-----------------------读操作-----------------------*/
		case 0:
		{//读取固件版本
			res = readNetworkDictCallback (d,nodeId,0x1018,0x04,0x00,CheckSDOAndContinue_Read,0);
		}
		break;  
		
		/*-----------------------写操作-----------------------*/		
		case 1:
		{//工作模式设置...
			UNS8 SetMode = 3;//Profile Velocity mode---0x03	
			UNS16 index = 0x6060;
			UNS8 size = sizeof(SetMode);
			res = writeNetworkDictCallBack (d,nodeId,index,0x00,size,0,&SetMode,CheckSDOAndContinue_Write,0);
		}
		break;     
		
		case 2:
		{//设置:Profile acceleration:6083h
			UNS32 SetData = 1000;  //millisecond (time from 0 rpm to 3000 rpm)
			UNS16 index = 0x6083;
			UNS8 size = sizeof(SetData);
			res = writeNetworkDictCallBack (d,nodeId,index,0x00,size,0,&SetData,CheckSDOAndContinue_Write,0);
		}
		break;  
		
		case 3:
		{//设置:Profile deceleration:6084h
			UNS32 SetData = 1500;  //millisecond (time from 0 rpm to 3000 rpm)
			UNS16 index = 0x6084;
			UNS8 size = sizeof(SetData);
			res = writeNetworkDictCallBack (d,nodeId,index,0x00,size,0,&SetData,CheckSDOAndContinue_Write,0);
		}
		break;   
		
		case 4:
		{
		
		}
//		break;
		
		case 5:
		{
		
		}
//		break;
		
		case 6:
		{

		}
//		break;
		
		case 7:
		{

		}
//		break;
		
		case 8:
		{

		}
//		break; 
		
		/*-----------------------------------配置PDO参数:通讯参数和映射参数-------------------------------------------------------------------*/
		/*---------------------------------------Slave的TPDO1配置------------------------------------------------------------*/
		case 9:
		{//设置TPDO1 mappingPara_index_0x1A00---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo1.
			UNS8 size = sizeof(subindex_number);
			CanMaster.ConfigStep[id]=9;//前面有空缺的操作时,需要添加该赋值	
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

		case 10:
		{//设置TPDO1 ComunicationPara_index_0x1800,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(20);//定义同步包数量为:x(收到x个同步包后才发送)
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO1;
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1800,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 11:
		{//电机实际电流:0.01A
			UNS32 MappIndexData = CanObjectDict_TPDO1_Windcon_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 12:
		{//母线电压:0.1V
			UNS32 MappIndexData = CanObjectDict_TPDO1_Windcon_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 13:
		{//驱动器温度:°
			UNS32 MappIndexData = CanObjectDict_TPDO1_Windcon_Map[2];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x03,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;		
		
		case 14:
		{//设置TPDO1 mappingPara_index_0x1A00---subindex_number
			UNS8 subindex_number = 3;//enable the tpdo1.
			UNS8 size = sizeof(subindex_number);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 15:
		{

		}
//		break; 	

		case 16:
		{

		}
//		break; 		
				
		/*---------------------------------------Slave的TPDO2配置------------------------------------------------------------*/
		case 17:
		{//设置TPDO2 mappingPara_index_0x1A01---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo2.
			UNS8 size = sizeof(subindex_number);       
			CanMaster.ConfigStep[id]=17;//前面有空缺的操作时,需要添加该赋值				
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

		case 18:
		{//设置TPDO2 ComunicationPara_index_0x1801,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(19);//定义同步包数量为:x(收到x个同步包后才发送)		
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO2;
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1801,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 19:
		{//状态字:0x6041
			UNS32 MappIndexData = CanObjectDict_TPDO2_Windcon_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 20:
		{//工作模式:0x6061
			UNS32 MappIndexData = CanObjectDict_TPDO2_Windcon_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 21:
		{//错误代码(RO)
			UNS32 MappIndexData = CanObjectDict_TPDO2_Windcon_Map[2];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x03,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;	
		
		case 22:
		{//设置TPDO2 mappingPara_index_0x1A01---subindex_number
			UNS8 subindex_number = 3;//enable the tpdo2.
			UNS8 size = sizeof(subindex_number);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;   
		
		case 23:
		{
			
		}
//		break;	
				
		/*---------------------------------------Slave的TPDO3配置------------------------------------------------------------*/
		case 24:
		{//设置TPDO3 mappingPara_index_0x1A02---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo3.
			UNS8 size = sizeof(subindex_number);	
			CanMaster.ConfigStep[id]=24;//前面有空缺的操作时,需要添加该赋值				
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;	

		case 25:
		{//设置TPDO3 ComunicationPara_index_0x1802,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(18);//定义同步包数量为:x(收到x个同步包后才发送)		
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO3;
			UNS8 size = sizeof(TransType);		
			res = writeNetworkDictCallBack (d,nodeId,0x1802,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 26:
		{//电机的实际转速:0x606C
			UNS32 MappIndexData = CanObjectDict_TPDO3_Windcon_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);			
		}
		break;
		
		case 27:
		{//错误代码(RO)
			UNS32 MappIndexData = CanObjectDict_TPDO3_Windcon_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);	
		}
		break;

		case 28:
		{//设置TPDO4 mappingPara_index_0x1A02---subindex_number
			UNS8 subindex_number = 2;//enable the tpdo3.
			UNS8 size = sizeof(subindex_number);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;  
		
		case 29:
		{
			
		}
//		break;		

		case 30:
		{
			
		}
//		break;	
		
		case 31:
		{
			
		}
//		break;		

		/*---------------------------------------Slave的TPDO4配置------------------------------------------------------------*/
		case 32:
		{//设置TPDO4 mappingPara_index_0x1A03---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo4.
			UNS8 size = sizeof(subindex_number);       
			CanMaster.ConfigStep[id]=32;//前面有空缺的操作时,需要添加该赋值					
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;	

		case 33:
		{//设置TPDO4 ComunicationPara_index_0x1803,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(17);//定义同步包数量为:x(收到x个同步包后才发送)		
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO4;
			UNS8 size = sizeof(TransType);		
			res = writeNetworkDictCallBack (d,nodeId,0x1803,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 34:
		{//电机实际位置:0x6064
			UNS32 MappIndexData = CanObjectDict_TPDO4_Windcon_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);			
		}
		break;

		case 35:
		{//设置TPDO3 mappingPara_index_0x1A02---subindex_number
			UNS8 subindex_number = 1;//enable the tpdo4.
			UNS8 size = sizeof(subindex_number);			
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

		/*---------------------------------------Slave的TPDO配置__COB_ID------------------------------------------------------------*/			
		case 36:
		{
			UNS32 COB_ID = 0x181+id;  //TPDO1_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1800,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break; 
			
		case 37:
		{
			UNS32 COB_ID = 0x281+id;  //TPDO2_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1801,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break; 
			
		case 38:
		{
			UNS32 COB_ID = 0x381+id;  //TPDO3_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1802,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break;    

		case 39:
		{
			UNS32 COB_ID = 0x481+id;  //TPDO4_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1803,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break;  		
			
 	  /*---------------------------------------Slave的RPDO配置__COB_ID------------------------------------------------------------*/     
		case 40:
		{
			UNS32 COB_ID = 0x201+id;  //RPDO1_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1400,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break;	
		
		/*---------------------------------------Slave的RPDO1配置------------------------------------------------------------*/
		case 41:
		{//设置RPDO1 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo1.
			UNS8 size = sizeof(subindex_number);					
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

		case 42:
		{//设置RPDO1 ComunicationPara_index_0x1400,发送类型:循环同步方式
			UNS8 TransType = TRANS_EVERY_N_SYNC(1);			
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1400,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;

		case 43:
		{//输入控制字
			UNS32 MappIndexData = CanObjectDict_RPDO1_Map[0];//映射到index:xxxx的subindex:00,对象是：16位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 44:
		{//速度命令输入
			UNS32 MappIndexData = CanObjectDict_RPDO1_Map[1];//映射到index:XXXX的subindex:00,对象是：32位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;

		case 45:
		{//设置RPDO1 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 2;//enable the rpdo1.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

    /*---------------------------------------Slave的RPDO2配置------------------------------------------------------*/
		case 46:
		{//设置RPDO2 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo2.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1601,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;		

    /*---------------------------------------Slave的RPDO3配置------------------------------------------------------*/
		case 47:
		{//设置RPDO3 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo3.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1602,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;			
		
    /*---------------------------------------Slave的RPDO4配置------------------------------------------------------*/
		case 48:
		{//设置RPDO4 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo4.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1603,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;			
				
		/*---------------------------------------事件相关参数------------------------------------------------------------*/
		case 49:
		{
			
		}
//		break;	
		
		case 50:
		{
			
		}
//		break;	

		/*-----------------------启动从机-----------------------*/
		case 51:
		{
		    CanMaster.SlaveParaConfigId[id]=nodeId;
				CanMaster.ConfigStep[id] = Config_step_Last_Windcon;
//				masterSendNMTstateChange (d, nodeId, NMT_Start_Node);
		}
		break;	

		default:
		break;
	}
	
	if(CanMaster.ConfigStep[id] > Config_step_Last_Windcon)
	{
		CanMaster.ConfigStep[id] = Config_step_Last_Windcon;
	}    
	
	return res;
}


/***************配置从机节点的相关参数...惠斯通驱动器****************/
static UNS8 ConfigureSlaveNode_Wheatstone(CO_Data* d, UNS8 nodeId)
{
	UNS8 res;
	u8 id = nodeId-1;

	switch(CanMaster.ConfigStep[id])
	{
		/*-----------------------读操作-----------------------*/
		case 0:
		{//读取固件版本

		}
//		break;  
		
		/*-----------------------写操作-----------------------*/		
		case 1:
		{//工作模式设置...
			UNS8 SetMode = 3;//Profile Velocity mode---0x03	
			UNS16 index = 0x6060;
			UNS8 size = sizeof(SetMode);
			CanMaster.ConfigStep[id]=1;//前面有空缺的操作时,需要添加该赋值					
			res = writeNetworkDictCallBack (d,nodeId,index,0x00,size,0,&SetMode,CheckSDOAndContinue_Write,0);
		}
		break;     
		
		case 2:
		{//设置:Profile acceleration:6083h  加速时间斜度：为0rpm加速到3000rpm所需时间
			UNS32 SetData = 2000;  //millisecond (time from 0 rpm to 3000 rpm)
			UNS16 index = 0x6083;
			UNS8 size = sizeof(SetData);
			res = writeNetworkDictCallBack (d,nodeId,index,0x00,size,0,&SetData,CheckSDOAndContinue_Write,0);
		}
		break;  
		
		case 3:
		{//设置:Profile deceleration:6084h  减速时间斜度：为3000rpm减速到0rpm所需时间
			UNS32 SetData = 2000;  //millisecond (time from 0 rpm to 3000 rpm)
			UNS16 index = 0x6084;
			UNS8 size = sizeof(SetData);
			res = writeNetworkDictCallBack (d,nodeId,index,0x00,size,0,&SetData,CheckSDOAndContinue_Write,0);
		}
		break;   
		
		case 4:
		{//设置：2009h subindex01为驱动器输出电流，此时需要读取2009，subindex04h
			UNS16 SetData = 900;           
			UNS16 index = 0x2009;
			UNS8 subindex = 0x01;
			UNS8 size = sizeof(SetData);
			res = writeNetworkDictCallBack (d,nodeId,index,subindex,size,0,&SetData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 5:
		{
		
		}
//		break;
		
		case 6:
		{

		}
//		break;
		
		case 7:
		{

		}
//		break;
		
		case 8:
		{

		}
//		break; 
		
		/*-----------------------------------配置PDO参数:通讯参数和映射参数-------------------------------------------------------------------*/
		/*---------------------------------------Slave的TPDO1配置------------------------------------------------------------*/
		case 9:
		{//设置TPDO1 mappingPara_index_0x1A00---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo1.
			UNS8 size = sizeof(subindex_number);
			CanMaster.ConfigStep[id]=9;//前面有空缺的操作时,需要添加该赋值	
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

		case 10:
		{//设置TPDO1 ComunicationPara_index_0x1800,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(20);//定义同步包数量为:x(收到x个同步包后才发送)
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO1;
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1800,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 11:
		{//母线电压
			UNS32 MappIndexData = CanObjectDict_TPDO1_Wheatstone_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 12:
		{

		}
//		break;
		
		case 13:
		{

		}
//		break;		
		
		case 14:
		{//设置TPDO1 mappingPara_index_0x1A00---subindex_number
			UNS8 subindex_number = 1;//enable the tpdo1.	
			UNS8 size = sizeof(subindex_number);
			CanMaster.ConfigStep[id]=14;//前面有空缺的操作时,需要添加该赋值 					
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 15:
		{

		}
//		break; 	

		case 16:
		{

		}
//		break; 		
				
		/*---------------------------------------Slave的TPDO2配置------------------------------------------------------------*/
		case 17:
		{//设置TPDO2 mappingPara_index_0x1A01---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo2.
			UNS8 size = sizeof(subindex_number);       
			CanMaster.ConfigStep[id]=17;//前面有空缺的操作时,需要添加该赋值				
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

		case 18:
		{//设置TPDO2 ComunicationPara_index_0x1801,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(19);//定义同步包数量为:x(收到x个同步包后才发送)		
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO2;
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1801,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 19:
		{//电机转速
			UNS32 MappIndexData = CanObjectDict_TPDO2_Wheatstone_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 20:
		{//回授电流
			UNS32 MappIndexData = CanObjectDict_TPDO2_Wheatstone_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 21:
		{

		}
//		break;	
		
		case 22:
		{//设置TPDO2 mappingPara_index_0x1A01---subindex_number
			UNS8 subindex_number = 2;//enable the tpdo2.
			UNS8 size = sizeof(subindex_number);
			CanMaster.ConfigStep[id]=22;//前面有空缺的操作时,需要添加该赋值			
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;   
		
		case 23:
		{
			
		}
//		break;	
				
		/*---------------------------------------Slave的TPDO3配置------------------------------------------------------------*/
		case 24:
		{//设置TPDO3 mappingPara_index_0x1A02---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo3.
			UNS8 size = sizeof(subindex_number);	
			CanMaster.ConfigStep[id]=24;//前面有空缺的操作时,需要添加该赋值				
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;	

		case 25:
		{//设置TPDO3 ComunicationPara_index_0x1802,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(18);//定义同步包数量为:x(收到x个同步包后才发送)		
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO3;
			UNS8 size = sizeof(TransType);		
			res = writeNetworkDictCallBack (d,nodeId,0x1802,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 26:
		{//状态字(RO)
			UNS32 MappIndexData = CanObjectDict_TPDO3_Wheatstone_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);			
		}
		break;
		
		case 27:
		{//错误代码(RO)
			UNS32 MappIndexData = CanObjectDict_TPDO3_Wheatstone_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);	
		}
		break;
		
		case 28:
		{//工作模式(RO)
			UNS32 MappIndexData = CanObjectDict_TPDO3_Wheatstone_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x03,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);	
		}
		break;		

		case 29:
		{//设置TPDO4 mappingPara_index_0x1A02---subindex_number
			UNS8 subindex_number = 3;//enable the tpdo3.
			UNS8 size = sizeof(subindex_number);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;  

		case 30:
		{
			
		}
//		break;	
		
		case 31:
		{
			
		}
//		break;		

		/*---------------------------------------Slave的TPDO4配置------------------------------------------------------------*/
		case 32:
		{//设置TPDO4 mappingPara_index_0x1A03---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo4.
			UNS8 size = sizeof(subindex_number);       
			CanMaster.ConfigStep[id]=32;//前面有空缺的操作时,需要添加该赋值					
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;	

		case 33:
		{

		}
//		break;
		
		case 34:
		{
		
		}
//		break;

		case 35:
		{

		}
//		break;

		/*---------------------------------------Slave的TPDO配置__COB_ID------------------------------------------------------------*/			
		case 36:
		{
			UNS32 COB_ID = 0x181+id;  //TPDO1_COB_ID
			UNS8 size = sizeof(COB_ID);
			CanMaster.ConfigStep[id]=36;//前面有空缺的操作时,需要添加该赋值						
			res = writeNetworkDictCallBack (d,nodeId,0x1800,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break; 
			
		case 37:
		{
			UNS32 COB_ID = 0x281+id;  //TPDO2_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1801,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break; 
			
		case 38:
		{
			UNS32 COB_ID = 0x381+id;  //TPDO3_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1802,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break;    

		case 39:
		{
			UNS32 COB_ID = 0x481+id;  //TPDO4_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1803,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break;  		
			
 	  /*---------------------------------------Slave的RPDO配置__COB_ID------------------------------------------------------------*/     
		case 40:
		{
			UNS32 COB_ID = 0x201+id;  //RPDO1_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1400,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break;	
		
		/*---------------------------------------Slave的RPDO1配置------------------------------------------------------------*/
		case 41:
		{//设置RPDO1 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo1.
			UNS8 size = sizeof(subindex_number);					
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

		case 42:
		{//设置RPDO1 ComunicationPara_index_0x1400,发送类型:循环同步方式
			UNS8 TransType = TRANS_EVERY_N_SYNC(1);			
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1400,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;

		case 43:
		{//输入控制字
			UNS32 MappIndexData = CanObjectDict_RPDO1_Map[0];//映射到index:xxxx的subindex:00,对象是：16位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 44:
		{//速度命令输入
			UNS32 MappIndexData = CanObjectDict_RPDO1_Map[1];//映射到index:XXXX的subindex:00,对象是：32位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;

		case 45:
		{//设置RPDO1 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 2;//enable the rpdo1.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

    /*---------------------------------------Slave的RPDO2配置------------------------------------------------------*/
		case 46:
		{//设置RPDO2 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo2.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1601,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;		

    /*---------------------------------------Slave的RPDO3配置------------------------------------------------------*/
		case 47:
		{//设置RPDO3 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo3.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1602,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;			
		
    /*---------------------------------------Slave的RPDO4配置------------------------------------------------------*/
		case 48:
		{//设置RPDO4 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo4.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1603,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;			
				
		/*---------------------------------------事件相关参数------------------------------------------------------------*/
		case 49:
		{//设置Slave心跳发送周期--ms
			UNS16 ProducerHeatbeatTimePeriod = Producer_Heartbeat_Times;
			UNS8  size = sizeof(ProducerHeatbeatTimePeriod);
			res = writeNetworkDictCallBack (d,nodeId,0x1017,0x00,size,0,&ProducerHeatbeatTimePeriod,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 50:
		{//设置Slave同步通讯循环周期(及同步报文的接收间隔时间值)--ms
			UNS32 CommunicationCyclePeriod = SYNC_CommunicationCyclePeriod;       
			UNS8  size = sizeof(CommunicationCyclePeriod);
			res = writeNetworkDictCallBack (d,nodeId,0x1006,0x00,size,0,&CommunicationCyclePeriod,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 51:
		{//设置Slave节点守护时间值--ms
			UNS16 Node_Guardingtime = NMT_GuardTime;
			UNS8  size = sizeof(Node_Guardingtime);
			res = writeNetworkDictCallBack (d,nodeId,0x100C,0x00,size,0,&Node_Guardingtime,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 52:
		{//设置Slave节点守护寿命因子
			UNS8 Node_LifeTimeFactor = NMT_LifeTimeFactor;
			UNS8  size = sizeof(Node_LifeTimeFactor);
			res = writeNetworkDictCallBack (d,nodeId,0x100D,0x00,size,0,&Node_LifeTimeFactor,CheckSDOAndContinue_Write,0);
		}
		break;

		/*-----------------------启动从机-----------------------*/
		case 53:
		{
		    CanMaster.SlaveParaConfigId[id]=nodeId;
				CanMaster.ConfigStep[id] = Config_step_Last_Wheatstone;
//				masterSendNMTstateChange (d, nodeId, NMT_Start_Node);
		}
		break;	

		default:
		break;
	}
	
	if(CanMaster.ConfigStep[id] > Config_step_Last_Wheatstone)
	{
		CanMaster.ConfigStep[id] = Config_step_Last_Wheatstone;
	}    
	
	return res;
}

static UNS8 ConfigureSlaveNode_Keya(CO_Data* d, UNS8 nodeId)
{
	UNS8 res;
	u8 id = nodeId-1;

	switch(CanMaster.ConfigStep[id])
	{
		/*-----------------------读操作-----------------------*/
		case 0:
		{//读取固件版本

		}
//		break;  
		
		/*-----------------------写操作-----------------------*/		
		case 1:
		{//工作模式设置...

		}
//		break;     
		
		case 2:
		{//设置:Profile acceleration:6083h  加速时间斜度：为0rpm加速到3000rpm所需时间

		}
//		break;  
		
		case 3:
		{//设置:Profile deceleration:6084h  减速时间斜度：为3000rpm减速到0rpm所需时间

		}
//		break;   
		
		case 4:
		{

		}
//		break;
		
		case 5:
		{
		
		}
//		break;
		
		case 6:
		{

		}
//		break;
		
		case 7:
		{

		}
//		break;
		
		case 8:
		{

		}
//		break; 
		
		/*-----------------------------------配置PDO参数:通讯参数和映射参数-------------------------------------------------------------------*/
		/*---------------------------------------Slave的TPDO1配置------------------------------------------------------------*/
		case 9:
		{//设置TPDO1 mappingPara_index_0x1A00---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo1.
			UNS8 size = sizeof(subindex_number);
			CanMaster.ConfigStep[id]=9;//前面有空缺的操作时,需要添加该赋值	
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

		case 10:
		{//设置TPDO1 ComunicationPara_index_0x1800,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(20);//定义同步包数量为:x(收到x个同步包后才发送)
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO1;
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1800,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 11:
		{//A驱动器转速
			UNS32 MappIndexData = CanObjectDict_TPDO1_Keya_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 12:
		{//A驱动器电流
			UNS32 MappIndexData = CanObjectDict_TPDO1_Keya_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 13:
		{//A驱动器位置
			UNS32 MappIndexData = CanObjectDict_TPDO1_Keya_Map[2];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x03,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;		
		
		case 14:
		{//设置TPDO1 mappingPara_index_0x1A00---subindex_number
			UNS8 subindex_number = 3;//enable the tpdo1.	
			UNS8 size = sizeof(subindex_number);				
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 15:
		{

		}
//		break; 	

		case 16:
		{

		}
//		break; 		
				
		/*---------------------------------------Slave的TPDO2配置------------------------------------------------------------*/
		case 17:
		{//设置TPDO2 mappingPara_index_0x1A01---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo2.
			UNS8 size = sizeof(subindex_number);       
			CanMaster.ConfigStep[id]=17;//前面有空缺的操作时,需要添加该赋值				
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

		case 18:
		{//设置TPDO2 ComunicationPara_index_0x1801,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(19);//定义同步包数量为:x(收到x个同步包后才发送)		
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO2;
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1801,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 19:
		{//B驱动器转速
			UNS32 MappIndexData = CanObjectDict_TPDO2_Keya_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 20:
		{//B驱动器电流
			UNS32 MappIndexData = CanObjectDict_TPDO2_Keya_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 21:
		{//B驱动器位置
			UNS32 MappIndexData = CanObjectDict_TPDO2_Keya_Map[2];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x03,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;	
		
		case 22:
		{//设置TPDO2 mappingPara_index_0x1A01---subindex_number
			UNS8 subindex_number = 3;//enable the tpdo2.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;   
		
		case 23:
		{
			
		}
//		break;	
				
		/*---------------------------------------Slave的TPDO3配置------------------------------------------------------------*/
		case 24:
		{//设置TPDO3 mappingPara_index_0x1A02---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo3.
			UNS8 size = sizeof(subindex_number);	
			CanMaster.ConfigStep[id]=24;//前面有空缺的操作时,需要添加该赋值				
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;	

		case 25:
		{//设置TPDO3 ComunicationPara_index_0x1802,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(18);//定义同步包数量为:x(收到x个同步包后才发送)		
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO3;
			UNS8 size = sizeof(TransType);		
			res = writeNetworkDictCallBack (d,nodeId,0x1802,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 26:
		{//A驱动器故障
			UNS32 MappIndexData = CanObjectDict_TPDO3_Keya_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);			
		}
		break;
		
		case 27:
		{//B驱动器故障
			UNS32 MappIndexData = CanObjectDict_TPDO3_Keya_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);	
		}
		break;
		
		case 28:
		{//电压
			UNS32 MappIndexData = CanObjectDict_TPDO3_Keya_Map[2];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x03,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);	
		}
		break;		
		
		case 29:
		{//温度
			UNS32 MappIndexData = CanObjectDict_TPDO3_Keya_Map[3];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x04,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);	
		}
		break;			

		case 30:
		{//设置TPDO4 mappingPara_index_0x1A02---subindex_number
			UNS8 subindex_number = 4;//enable the tpdo3.
			UNS8 size = sizeof(subindex_number);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;  

		/*---------------------------------------Slave的TPDO4配置------------------------------------------------------------*/
		case 31:
		{//设置TPDO4 mappingPara_index_0x1A03---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo4.
			UNS8 size = sizeof(subindex_number);       			
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;	

		case 32:
		{//设置TPDO2 ComunicationPara_index_0x1801,发送类型:循环同步方式
			//UNS8 TransType = TRANS_EVERY_N_SYNC(19);//定义同步包数量为:x(收到x个同步包后才发送)		
			UNS8 TransType = TRANS_EVERY_N_SYNC_TPDO4;
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1803,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 33:
		{//A驱动器控制状态
			UNS32 MappIndexData = CanObjectDict_TPDO4_Keya_Map[0];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 34:
		{//B驱动器控制状态
			UNS32 MappIndexData = CanObjectDict_TPDO4_Keya_Map[1];
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;

		case 35:
		{//设置TPDO2 mappingPara_index_0x1A03---subindex_number
			UNS8 subindex_number = 2;//enable the tpdo4.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;  

		/*---------------------------------------Slave的TPDO配置__COB_ID------------------------------------------------------------*/			
		case 36:
		{
			UNS32 COB_ID = 0x181+id;  //TPDO1_COB_ID
			UNS8 size = sizeof(COB_ID);
			CanMaster.ConfigStep[id]=36;//前面有空缺的操作时,需要添加该赋值						
			res = writeNetworkDictCallBack (d,nodeId,0x1800,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break; 
			
		case 37:
		{
			UNS32 COB_ID = 0x281+id;  //TPDO2_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1801,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break; 
			
		case 38:
		{
			UNS32 COB_ID = 0x381+id;  //TPDO3_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1802,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break;    

		case 39:
		{
			UNS32 COB_ID = 0x481+id;  //TPDO4_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1803,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break;  		
			
 	  /*---------------------------------------Slave的RPDO配置__COB_ID------------------------------------------------------------*/     
		case 40:
		{
			UNS32 COB_ID = 0x201+id;  //RPDO1_COB_ID
			UNS8 size = sizeof(COB_ID);
			res = writeNetworkDictCallBack (d,nodeId,0x1400,0x01,size,0,&COB_ID,CheckSDOAndContinue_Write,0);
		}
		break;	
		
		/*---------------------------------------Slave的RPDO1配置------------------------------------------------------------*/
		case 41:
		{//设置RPDO1 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo1.
			UNS8 size = sizeof(subindex_number);					
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

		case 42:
		{//设置RPDO1 ComunicationPara_index_0x1400,发送类型:循环同步方式
			UNS8 TransType = TRANS_EVERY_N_SYNC(1);			
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1400,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;

		case 43:
		{//A驱动器使能
			UNS32 MappIndexData = CanObjectDict_RPDO1_Keya_Map[0];//映射到index:xxxx的subindex:00,对象是：16位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 44:
		{//A驱动器速度命令输入
			UNS32 MappIndexData = CanObjectDict_RPDO1_Keya_Map[1];//映射到index:XXXX的subindex:00,对象是：32位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;

		case 45:
		{//设置RPDO1 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 2;//enable the rpdo1.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;

    /*---------------------------------------Slave的RPDO2配置------------------------------------------------------*/
		case 46:
		{//设置RPDO2 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo2.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1601,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;		

		case 47:
		{//设置RPDO2 ComunicationPara_index_0x1401,发送类型:循环同步方式
			UNS8 TransType = TRANS_EVERY_N_SYNC(1);			
			UNS8 size = sizeof(TransType);
			res = writeNetworkDictCallBack (d,nodeId,0x1401,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;

		case 48:
		{//B驱动器使能
			UNS32 MappIndexData = CanObjectDict_RPDO2_Keya_Map[0];//映射到index:xxxx的subindex:00,对象是：16位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1601,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		
		case 49:
		{//B驱动器速度命令输入
			UNS32 MappIndexData = CanObjectDict_RPDO2_Keya_Map[1];//映射到index:XXXX的subindex:00,对象是：32位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1601,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;

		case 50:
		{//设置RPDO1 mappingPara_index_0x1601---subindex_number
			UNS8 subindex_number = 2;//enable the rpdo2.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1601,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;		

    /*---------------------------------------Slave的RPDO3配置------------------------------------------------------*/
		case 51:
		{//设置RPDO3 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo3.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1602,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;			
		
    /*---------------------------------------Slave的RPDO4配置------------------------------------------------------*/
		case 52:
		{//设置RPDO4 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo4.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1603,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;			
				
		/*---------------------------------------事件相关参数------------------------------------------------------------*/
		case 53:
		{//设置Slave心跳发送周期--ms
			UNS16 ProducerHeatbeatTimePeriod = Producer_Heartbeat_Times;
			UNS8  size = sizeof(ProducerHeatbeatTimePeriod);
			res = writeNetworkDictCallBack (d,nodeId,0x1017,0x00,size,0,&ProducerHeatbeatTimePeriod,CheckSDOAndContinue_Write,0);
		}
		break;

		/*-----------------------启动从机-----------------------*/
		case 54:
		{
				CanMaster.SlaveParaConfigId[id*2]  =(nodeId-1)*2+1;
				CanMaster.ConfigStep[id*2]   = Config_step_Last_Keya;			
			
			  if((id*2+1) < MAX_NodeId_NUM)
				{
						CanMaster.SlaveParaConfigId[id*2+1]=(nodeId-1)*2+2;	
						CanMaster.ConfigStep[id*2+1] = Config_step_Last_Keya;			
		//				masterSendNMTstateChange (d, nodeId, NMT_Start_Node);				
				}
		}
		break;	

		default:
		break;
	}
	
	if(CanMaster.ConfigStep[id] > Config_step_Last_Keya)
	{
		CanMaster.ConfigStep[id] = Config_step_Last_Keya;
	}    
	
	return res;
}


/***************配置从机节点的相关参数...****************/
static UNS8 ConfigureSlaveNode_Copley(CO_Data* d, UNS8 nodeId)
{
	UNS8 res;
	u8 id = nodeId-1;

	switch(CanMaster.ConfigStep[id])
	{
		/*-----------------------读操作-----------------------*/
		case 0:
		{//读取sn		
			res = readNetworkDictCallback (d,nodeId,0x1018,0x04,0x00,CheckSDOAndContinue_Read,0);
		}
		break;
		/*-----------------------写操作-----------------------*/		
		case 1:
		{//工作模式设置...
			
#if (CopleyWorkMode==0)//设置0x2300:为速度loop模式:

			INTEGER16 SetMode = 11;//The velocity loop is driven by the programmed velocity value.
			UNS16 index = 0x2300;

#elif  (CopleyWorkMode==1)//设置0x6060:为速度模式:
		
			UNS8 SetMode = 3;//Profile Velocity mode---0x03	
			UNS16 index = 0x6060;
			
#elif  (CopleyWorkMode==2)//设置:为力矩模式:

			INTEGER16 SetMode = 1;//The current loop is driven by the programmed current value.
			UNS16 index = 0x2300;

#endif

			UNS8 size = sizeof(SetMode);
			res = writeNetworkDictCallBack (d,nodeId,index,0x00,size,0,&SetMode,CheckSDOAndContinue_Write,0);
		}
		break;
		case 2:
		{
		
		}
//		break;
		case 3:
		{

		}
//		break;
		case 4:
		{

		}
//		break;

		/*-----------------------------------配置PDO参数:通讯参数和映射参数-------------------------------------------------------------------*/
		/*---------------------------------------Slave的TPDO1配置------------------------------------------------------------*/
		case 5:
		{//设置TPDO1 mappingPara_index_0x1A00---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo1.
			UNS8 size = sizeof(subindex_number);	
			CanMaster.ConfigStep[id]=5;//前面有空缺的操作时,需要添加该赋值
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		case 6:
		{//设置TPDO1 ComunicationPara_index_0x1800,发送类型:循环同步方式
			UNS8 TransType = TRANS_SYNC_MIN;//定义同步包数量为:2(收到2个同步包后才发送)	
			UNS8 size = sizeof(TransType);		
			res = writeNetworkDictCallBack (d,nodeId,0x1800,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		case 7:
		{//母线电压:0.1V
			UNS32 MappIndexData = CanObjectDict_TPDO1_Copley_Map[0];//映射到index:2201的subindex:00,对象是：16位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		case 8:
		{//电机实际电流:0.01A
			UNS32 MappIndexData = CanObjectDict_TPDO1_Copley_Map[1];//映射到index:221C的subindex:00,对象是：16位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		case 9:
		{//温度值
			UNS32 MappIndexData = CanObjectDict_TPDO1_Copley_Map[2];//映射到index:2202的subindex:00,对象是：16位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x03,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		case 10:
		{//输入pin状态
			UNS32 MappIndexData = CanObjectDict_TPDO1_Copley_Map[3];//映射到index:2190的subindex:00,对象是：16位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x04,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		case 11:
		{//设置TPDO1 mappingPara_index_0x1A00---subindex_number
			UNS8 subindex_number = 4;//enable the tpdo1.
			UNS8 size = sizeof(subindex_number);			
			res = writeNetworkDictCallBack (d,nodeId,0x1A00,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		/*---------------------------------------Slave的TPDO2配置------------------------------------------------------------*/
		case 12:
		{//设置TPDO2 mappingPara_index_0x1A01---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo2.
			UNS8 size = sizeof(subindex_number);			
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		case 13:
		{//设置TPDO2 ComunicationPara_index_0x1801,发送类型:循环同步方式
			UNS8 TransType = TRANS_SYNC_MIN;			
			UNS8 size = sizeof(TransType);		
			res = writeNetworkDictCallBack (d,nodeId,0x1801,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		case 14:
		{//状态字:0x6041
			UNS32 MappIndexData = CanObjectDict_TPDO2_Copley_Map[0];//映射到index:6041的subindex:00,对象是：16位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		case 15:
		{//事件字:0x1002
			UNS32 MappIndexData = CanObjectDict_TPDO2_Copley_Map[1];//映射到index:1002的subindex:00,对象是：32位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		case 16:
		{//工作模式:0x6061
			UNS32 MappIndexData = CanObjectDict_TPDO2_Copley_Map[2];//映射到index:6061的subindex:00,对象是：8位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x03,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		case 17:
		{

		}
//		break;
		case 18:
		{//设置TPDO2 mappingPara_index_0x1A01---subindex_number
			UNS8 subindex_number = 3;//enable the tpdo2.
			UNS8 size = sizeof(subindex_number);	
			CanMaster.ConfigStep[id]=18;//前面有空缺的操作时,需要添加该赋值
			res = writeNetworkDictCallBack (d,nodeId,0x1A01,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		/*---------------------------------------Slave的TPDO3配置------------------------------------------------------------*/
		case 19:
		{//设置TPDO3 mappingPara_index_0x1A02---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo3.
			UNS8 size = sizeof(subindex_number);			
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		case 20:
		{//设置TPDO3 ComunicationPara_index_0x1802,发送类型:循环同步方式
			UNS8 TransType = TRANS_SYNC_MIN;			
			UNS8 size = sizeof(TransType);		
			res = writeNetworkDictCallBack (d,nodeId,0x1802,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		case 21:
		{//电机的实际转速:0x6069
			UNS32 MappIndexData = CanObjectDict_TPDO3_Copley_Map[0];//映射到index:6069的subindex:00,对象是：32位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);			
		}
		break;
		case 22:
		{//电机的故障锁存:0x2183
			UNS32 MappIndexData = CanObjectDict_TPDO3_Copley_Map[1];//映射到index:2183的subindex:00,对象是：32位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);			
		}
		break; 
		case 23:
		{//设置TPDO3 mappingPara_index_0x1A02---subindex_number
			UNS8 subindex_number = 2;//enable the tpdo3.
			UNS8 size = sizeof(subindex_number);			
			res = writeNetworkDictCallBack (d,nodeId,0x1A02,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;   
        /*---------------------------------------Slave的TPDO4配置------------------------------------------------------------*/        
		case 24:
		{//设置TPDO4 mappingPara_index_0x1A03---subindex_number
			UNS8 subindex_number = 0;//disable the tpdo4.
			UNS8 size = sizeof(subindex_number);			
			res = writeNetworkDictCallBack (d,nodeId,0x1A03,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;   
		case 25:
		{

		}
//		break;        
		case 26:
		{

		}
//		break;              
		case 27:
		{

		}
//		break;                      
		case 28:
		{

		}
//		break;  
		case 29:
		{

		}
//		break;     
		case 30:
		{

		}
//		break;               
		/*---------------------------------------Slave的RPDO1配置------------------------------------------------------------*/
		case 31:
		{//设置RPDO1 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo1.
			UNS8 size = sizeof(subindex_number);
      CanMaster.ConfigStep[id]=31;//前面有空缺的操作时,需要添加该赋值
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		case 32:
		{//设置RPDO1 ComunicationPara_index_0x1400,发送类型:循环同步方式
			UNS8 TransType = TRANS_SYNC_MIN;			
			UNS8 size = sizeof(TransType);		
			res = writeNetworkDictCallBack (d,nodeId,0x1400,0x02,size,0,&TransType,CheckSDOAndContinue_Write,0);
		}
		break;
		case 33://控制输入  0x6040 or 0x2191
		{
			UNS32 MappIndexData = CanObjectDict_RPDO1_Copley_Map[0];//映射到index:xxxx的subindex:00,对象是：16位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x01,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		case 34:
		{//速度命令输入  0x60FF or 0x2134 or 0x2340
			UNS32 MappIndexData = CanObjectDict_RPDO1_Copley_Map[1];//映射到index:XXXX的subindex:00,对象是：32位
			UNS8 size = sizeof(MappIndexData);
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x02,size,0,&MappIndexData,CheckSDOAndContinue_Write,0);
		}
		break;
		case 35:
		{//设置RPDO1 mappingPara_index_0x1600---subindex_number
			UNS8 subindex_number = 2;//enable the rpdo1.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1600,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
        /*---------------------------------------Slave的RPDO2配置------------------------------------------------------------*/
		case 36:
		{//设置RPDO2 mappingPara_index_0x1601---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo2.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1601,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
        /*---------------------------------------Slave的RPDO3配置------------------------------------------------------------*/
		case 37:
		{//设置RPDO3 mappingPara_index_0x1602---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo3.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1602,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
        /*---------------------------------------Slave的RPDO4配置------------------------------------------------------------*/
		case 38:
		{//设置RPDO4 mappingPara_index_0x1603---subindex_number
			UNS8 subindex_number = 0;//disable the rpdo4.
			UNS8 size = sizeof(subindex_number);		
			res = writeNetworkDictCallBack (d,nodeId,0x1603,0x00,size,0,&subindex_number,CheckSDOAndContinue_Write,0);
		}
		break;
		/*---------------------------------------事件相关参数------------------------------------------------------------*/
		case 39:
		{//设置Slave心跳发送周期--ms
			UNS16 ProducerHeatbeatTimePeriod = Producer_Heartbeat_Times;
			UNS8  size = sizeof(ProducerHeatbeatTimePeriod);
			CanMaster.ConfigStep[id]=39;//前面有空缺的操作时,需要添加该赋值
			res = writeNetworkDictCallBack (d,nodeId,0x1017,0x00,size,0,&ProducerHeatbeatTimePeriod,CheckSDOAndContinue_Write,0);
		}
		break;
		case 40:
		{//设置Slave同步通讯循环周期(及同步报文的接收间隔时间值)--ms
			UNS32 CommunicationCyclePeriod = 0;//-----A value of zero in this object disables SYNC message production.
			UNS8  size = sizeof(CommunicationCyclePeriod);
			res = writeNetworkDictCallBack (d,nodeId,0x1006,0x00,size,0,&CommunicationCyclePeriod,CheckSDOAndContinue_Write,0);
		}
		break;
		case 41:
		{//设置Slave节点守护时间值--ms
			UNS16 Node_Guardingtime = NMT_GuardTime;
			UNS8  size = sizeof(Node_Guardingtime);
			res = writeNetworkDictCallBack (d,nodeId,0x100C,0x00,size,0,&Node_Guardingtime,CheckSDOAndContinue_Write,0);
		}
		break;
		case 42:
		{//设置Slave节点守护寿命因子
			UNS8 Node_LifeTimeFactor = NMT_LifeTimeFactor;
			UNS8  size = sizeof(Node_LifeTimeFactor);
			res = writeNetworkDictCallBack (d,nodeId,0x100D,0x00,size,0,&Node_LifeTimeFactor,CheckSDOAndContinue_Write,0);
		}
		break;
		case 43:
		{//
			UNS16 Node_GuardingErrorAction = 1;
			UNS8  size = sizeof(Node_GuardingErrorAction);
			res = writeNetworkDictCallBack(d, nodeId, 0x21B2, 0x00, size, 0, &Node_GuardingErrorAction, CheckSDOAndContinue_Write, 0);
		}
		break;
		case 44:
		{//

		}
//		break;
		/*-----------------------启动从机-----------------------*/
		case 45:
		{
			CanMaster.SlaveParaConfigId[id]=nodeId;
			CanMaster.ConfigStep[id] = Config_step_Last_Copley;
//			masterSendNMTstateChange (d, nodeId, NMT_Start_Node);
		}
		break;
		default:
		break;
	} 
	
	if(CanMaster.ConfigStep[id]>Config_step_Last_Copley)
	{
		CanMaster.ConfigStep[id]=Config_step_Last_Copley;
	}
	
	return res;
}

void CanObjectDict_initialisation(CO_Data* d)
{
	eprintf(0, "CanObjectDict_initialisation\n");

}

void CanObjectDict_preOperational(CO_Data* d)
{
	eprintf("CanObjectDict_preOperational\n");

}

void CanObjectDict_operational(CO_Data* d)
{
	eprintf("CanObjectDict_operational\n");
	
}

void CanObjectDict_stopped(CO_Data* d)
{
	eprintf("CanObjectDict_stopped\n");
	
}

/**********************************************************************************************************
在处理 proceedSYNC() 时回Calls
消息transmit PDO同步处理;
***********************************************************************************************************/
void CanObjectDict_post_TPDO(CO_Data* d)
{
	CanFestival_led ++;
}

/**********************************************************************************************************
科亚驱动器速度线性化
科亚驱动器电机上位机中设置速度为MaxSpeed，将MaxSpeed映射为10000，将-MaxSpeed映射为-10000
***********************************************************************************************************/
void Keya_SpeedLinearization(u8 Channel,s16 SetSpeed)
{
    f32 CalcSpeedPtt = 0,    //计算的线性万分比 Per ten thousand
	      SetSpeedBuffer = 0;
	
	  u16 MaxSpeed = 30000;
	
	  SetSpeedBuffer = (-1)*SetSpeed;

	  CalcSpeedPtt = ((f32)(SetSpeedBuffer / MaxSpeed))*10000;
		
		//限幅
		if(CalcSpeedPtt > 10000)
			CalcSpeedPtt = 10000;
		else if(CalcSpeedPtt < -10000)
			CalcSpeedPtt = -10000;
		
    if(Channel%2 == 0)  //奇数序号
		{
		    CanObjectDict_Tx_Map20[Channel/2] = CalcSpeedPtt;
		}
		else
		{
		    CanObjectDict_Tx_Map21[Channel/2] = CalcSpeedPtt;		
		}
}

/**********************************************************************************************************
科亚驱动器使能
***********************************************************************************************************/
void Keya_DriverEnableOrDisable(u8 Channel,u16 Cmd)
{
	 u32 EnableCmd = 0;
	
   //科亚驱动器报文为双驱类型（单驱也是）
	 if(Channel%2 == 0)  //奇数序号
	 {
			 if(Cmd == 0x0F)    //使能
			     EnableCmd = 0x030D2001; 
       else               //失能
			     EnableCmd = 0x030C2001;   

       CanObjectDict_Tx_Map18[Channel/2] = EnableCmd;			 
	 }					
	 else                //偶数序号
	 {
			 if(Cmd == 0x0F)    //使能
			     EnableCmd = 0x030D2002; 
       else               //失能
			     EnableCmd = 0x030C2002; 	

	     CanObjectDict_Tx_Map19[Channel/2] = EnableCmd;			 
	 }				 
}

/**********************************************************************************************************
在处理 proceedSYNC()时Calls
消息同步处理;周期性调用:间隔时间:SYNC_CommunicationCyclePeriod
***********************************************************************************************************/
void CanObjectDict_post_sync(CO_Data* d)
{
	UNS8 i = 0;

	f32 Velocity_rpm[MAX_NodeId_NUM];
	//BYTE4_UNION	*pBU4 = 0;

	for(i=0;i<MAX_NodeId_NUM;i++)
	{
		if(CanMaster.SlaveParaConfigId[i] == i+1)
		{
			if((g_ucDriverMode[i]==DRIVER_MODE_VELOCITY) || (g_ucDriverMode[i]==DRIVER_MODE_ImmediateVelocity))   //速度模式 || 立即速度模式	
			{
				if(CanMaster.Para.SVOType == Can_SVOType_Kinco)
				{
						//rpm = RPM*512*编码器分辨率/1875  
						//对于回转电机，编码器分辨率为65536
						//对于行走电机，编码器分辨率为2500*4				

						CanObjectDict_Tx_Map10[i] = SVO[i].CMDCtrl;//更新:控制字数据
						Velocity_rpm[i] = SVO[i].CMDSpeed / 10;//更新:速度命令值 0.1rpm
						//4代 步科 和 5代步科的编码器设计兼容
						if(SVO[i].SvoType == SvoType_Kinco_5)
						{
							CanObjectDict_Tx_Map11[i] = Velocity_rpm[i]*17895.697f ;//将速度值rpm转换为计数值
						}
						else
						{
							CanObjectDict_Tx_Map11[i] = Velocity_rpm[i]*2730.66666f ;//将速度值rpm转换为计数值
						}
						
				}
				else if(CanMaster.Para.SVOType == Can_SVOType_Delta)
				{
						CanObjectDict_Tx_Map10[i] = SVO[i].CMDCtrl;//更新:控制字数据
						Velocity_rpm[i] = SVO[i].CMDSpeed;//更新:速度命令值 0.1rpm
						CanObjectDict_Tx_Map11[i] = Velocity_rpm[i];//更新:速度命令值 0.1rpm
					
						//速度命令为0时，限制力矩
						if(CanObjectDict_Tx_Map11[i] == 0)
						{
							CanObjectDict_Tx_Map12[i] = g_Para.Walk.TorqueLimit_Stop;
						}
						else
						{
							CanObjectDict_Tx_Map12[i] = SVO[i].CMDLimitTorque;
							//CanObjectDict_Tx_Map12[i] = g_Para.Walk.TorqueLimit_Max;//测试
						}					
				}	
				else if(CanMaster.Para.SVOType == Can_SVOType_Windcon)
				{
						CanObjectDict_Tx_Map10[i] = SVO[i].CMDCtrl;//更新:控制字数据
						Velocity_rpm[i] = SVO[i].CMDSpeed / 10;//更新:速度命令值 0.1rpm
						CanObjectDict_Tx_Map11[i] = Velocity_rpm[i];//更新:速度命令值 0.1rpm			
				}
				else if(CanMaster.Para.SVOType == Can_SVOType_Wheatstone)
				{
						CanObjectDict_Tx_Map10[i] = SVO[i].CMDCtrl;//更新:控制字数据
						Velocity_rpm[i] = SVO[i].CMDSpeed;//更新:速度命令值 0.1rpm
						CanObjectDict_Tx_Map11[i] = Velocity_rpm[i];//更新:速度命令值 0.1rpm				
				}	
				else if(CanMaster.Para.SVOType == Can_SVOType_Keya)
				{
						Keya_DriverEnableOrDisable(i,SVO[i].CMDCtrl);//更新:控制字数据
						Keya_SpeedLinearization(i,SVO[i].CMDSpeed);//更新:速度命令值 0.1rpm			
				}					
				else if(CanMaster.Para.SVOType == Can_SVOType_Copley)
				{
						CanObjectDict_Tx_Map10[i] = SVO[i].CMDCtrl;//更新:控制字数据
						Velocity_rpm[i] = SVO[i].CMDSpeed / 10;//更新:速度命令值 0.1rrpm
						CanObjectDict_Tx_Map11[i] = Velocity_rpm[i] * 1666.6666f;//将速度值rpm转换为计数值10000/6  10000=2500*4(2500线编码器)					
				}						
			}
			else if(g_ucDriverMode[i]==DRIVER_MODE_POSATION)   //位置模式
			{	
				if(CanMaster.PostrigerCnt[i]==0)
				{
					CanObjectDict_Tx_Map10[i] = (0x30 | SVO[i].CMDCtrl);//更新:控制字数据
					CanObjectDict_Tx_Map18[i] = SVO[i].CMDPosition;//更新:电流命令值
					CanMaster.PostrigerCnt[i] = 0x01;
				}
				else
				{
					CanObjectDict_Tx_Map10[i] = (0x20 | SVO[i].CMDCtrl);//更新:控制字数据	
					CanMaster.PostrigerCnt[i] = 0x00;		
				}			
			}				
		}	
	}
}

/*急停消息回调处理*/
void CanObjectDict_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg)
{

}

/*收到Slave产生的boot-up报文*/
void CanObjectDict_post_SlaveBootup(CO_Data* d, UNS8 nodeId)
{
	if(d->nodeState == Pre_operational)// || d->nodeState == Operational)
	{
		if(CanMaster.Para.SVOType == Can_SVOType_Kinco)
		{
		    ConfigureSlaveNode_Kinco(d, nodeId);	
		}
		else if(CanMaster.Para.SVOType == Can_SVOType_Delta)
		{
		    ConfigureSlaveNode_Delta(d, nodeId);	
		}
		else if(CanMaster.Para.SVOType == Can_SVOType_Windcon)
		{
		    ConfigureSlaveNode_Windcon(d, nodeId);	
		}		
		else if(CanMaster.Para.SVOType == Can_SVOType_Wheatstone)
		{
		    ConfigureSlaveNode_Wheatstone(d, nodeId);	
		}			
		else if(CanMaster.Para.SVOType == Can_SVOType_Keya)
		{
		    ConfigureSlaveNode_Keya(d, nodeId);	
		}			
		else if(CanMaster.Para.SVOType == Can_SVOType_Copley)
		{
		    ConfigureSlaveNode_Copley(d, nodeId);	
		}				
	}
}

/*从机有状态改变,产生的回调*/
void CanObjectDict_post_SlaveStateChange(CO_Data* d, UNS8 nodeId, e_nodeState newNodeState)
{
	

//	if((d->nodeState == Pre_operational || d->nodeState == Operational))
//	{
//		SVO[usid].SlaveStatus = newNodeState;
//		
//		if(SVO[usid].SlaveStatus == Operational)
//		{
//		  SVO[usid].SlaveHeartbeatError = Operational;		
//		}
//	}
#if (SVO_Type == 4)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器	 	
	UNS8 usid_A = (nodeId - 1)*2;
	SVO[usid_A].SlaveStatus = newNodeState;
	//A组
    if(d->nodeState == Operational)
	{	
		if(SVO[usid_A].SlaveStatus == Operational)
		{
            SVO[usid_A].SlaveHeartbeatError = Operational;		
		}
        else if(SVO[usid_A].SlaveStatus == Pre_operational)
        {
            SVO[usid_A].SlaveHeartbeatError = Disconnected;	
        }
	}
	//B组
	{
		UNS8 usid_B = usid_A+1;
		if(usid_B < MAX_NodeId_NUM)
		{
			SVO[usid_B].SlaveStatus = newNodeState;
			if(d->nodeState == Operational)
			{	
				if(SVO[usid_B].SlaveStatus == Operational)
				{
					SVO[usid_B].SlaveHeartbeatError = Operational;		
				}
				else if(SVO[usid_B].SlaveStatus == Pre_operational)
				{
					SVO[usid_B].SlaveHeartbeatError = Disconnected;	
				}
			}
		}
	}
#else
	UNS8 usid = nodeId - 1;
	SVO[usid].SlaveStatus = newNodeState;
    
    if(d->nodeState == Operational)
	{	
		if(SVO[usid].SlaveStatus == Operational)
		{
            SVO[usid].SlaveHeartbeatError = Operational;		
		}
        else if(SVO[usid].SlaveStatus == Pre_operational)
        {
            SVO[usid].SlaveHeartbeatError = Disconnected;	
        }
	}
#endif
    
}

/*在规定的时间内Slave Heartbeat超时*/
void CanObjectDict_heartbeatError(CO_Data* d, UNS8 nodeId)
{
//	UNS8 usid = nodeId - 1;

//	if(d->nodeState == Operational && NMT_GuardTime == 0)
//	//注意：步科驱动器和台达驱动器不支持heartbeat下从站对主站的监控，推荐使用node guard
//	{
////		SVO[usid].SlaveStatus = Disconnected;	
////		SVO[usid].CommErrCnt ++;	
//		
//		  SVO[usid].SlaveHeartbeatError = Disconnected;
//	}	
}

/*在规定的时间内Slave Node Guard超时*/
void CanObjectDict_nodeguardError(CO_Data* d, UNS8 nodeId)
{
	
#if (SVO_Type == 4)  //0：步科驱动器   1：台达驱动器		2：风得控驱动器		3：惠斯通驱动器		4：科亚驱动器	 	
		UNS8 usid_A = (nodeId - 1)*2;
		if(d->nodeState == Operational)
		{
			  SVO[usid_A].SlaveHeartbeatError = Disconnected;
		}
		//B组
		{
			UNS8 usid_B = usid_A+1;
			if(usid_B < MAX_NodeId_NUM)
			{
				SVO[usid_A].SlaveHeartbeatError = Disconnected;
			}
		}

#else
		UNS8 usid = nodeId - 1;
		if(d->nodeState == Operational)
		{
			  SVO[usid].SlaveHeartbeatError = Disconnected;
		}
#endif
}

/*---------------------------------------------自定义的映射回调函数-------------------------------------------------------------------*/
UNS32 Callback_Rx_Map1(CO_Data* d, const indextable * indextable, UNS8 bSubindex)
{	
	BYTE2_UNION	BU2, *pBU2;	
	BYTE4_UNION	BU4, *pBU4,*pBU4_1;	
	UNS8 id = d->SlaveNodeid - 1,tmp[8] = {0,0,0,0,0,0,0,0};

	pBU2=&BU2;
	pBU4=&BU4;
	
	//if(!g_DHR_LinkCanOpen)			return 0xFF;

	//copley驱动器BusVoltage单位为v，只需要u16即可存放
	//kinco驱动器BusVoltage单位为mv，需要u32存放	
	
	//如果需要使用copley驱动器，下面应更改为
	//		if(indextable->index == 0x6069
	//  	|| indextable->index == 0x606C	
	//	)
	if(indextable)
	{
		if(CanMaster.Para.SVOType == Can_SVOType_Keya)
		{
		    //A驱动器转速
				if(indextable->index == 0x2012)       
				{
					pBU2 = (BYTE2_UNION *)&(SVO[id*2].Speed);
				}		
				
			  //A驱动器电流
				if(indextable->index == 0x2010)       
				{
					pBU2 = (BYTE2_UNION *)&(SVO[id*2].Current);
				}		
				
			  //A驱动器位置
				if(indextable->index == 0x2016)  
				{
					pBU4 = (BYTE4_UNION *)&(SVO[id*2].Position);
					pBU4_1 = (BYTE4_UNION *)indextable->pSubindex[bSubindex].pObject;
					pBU4->B4_U32 = pBU4_1->B4_U32;				
				}		

        //A驱动器故障				
				if(indextable->index == 0x2011)       
				{
					pBU2 = (BYTE2_UNION *)&(SVO[id*2].FaultCode);
				}	
				
			  //A驱动器控制状态
				if(indextable->index == 0x2003)  
				{
					pBU4 = (BYTE4_UNION *)&(SVO[id*2].EventWord);
					pBU4_1 = (BYTE4_UNION *)indextable->pSubindex[bSubindex].pObject;
					pBU4->B4_U32 = pBU4_1->B4_U32;				
				}					
				
		    //B驱动器转速
				if(indextable->index == 0x2022)       
				{
					pBU2 = (BYTE2_UNION *)&(SVO[id*2+1].Speed);
				}		
				
			  //B驱动器电流
				if(indextable->index == 0x2020)       
				{
					pBU2 = (BYTE2_UNION *)&(SVO[id*2+1].Current);
				}		
				
			  //B驱动器位置
				if(indextable->index == 0x2026)  
				{
					pBU4 = (BYTE4_UNION *)&(SVO[id*2+1].Position);
					pBU4_1 = (BYTE4_UNION *)indextable->pSubindex[bSubindex].pObject;
					pBU4->B4_U32 = pBU4_1->B4_U32;				
				}	
				
        //B驱动器故障				
				if(indextable->index == 0x2021)       
				{
					pBU2 = (BYTE2_UNION *)&(SVO[id*2+1].FaultCode);
				}	

			  //B驱动器控制状态
				if(indextable->index == 0x2004)  
				{
					pBU4 = (BYTE4_UNION *)&(SVO[id*2+1].EventWord);
					pBU4_1 = (BYTE4_UNION *)indextable->pSubindex[bSubindex].pObject;
					pBU4->B4_U32 = pBU4_1->B4_U32;				
				}		

        //驱动器电压				
				if(indextable->index == 0x2013)       
				{
					pBU2 = (BYTE2_UNION *)&(SVO[id*2].Voltage);
					
					SVO[id*2+1].Voltage = SVO[id*2].Voltage;
				}			

        //驱动器温度			
				if(indextable->index == 0x203C)       
				{
					pBU2 = (BYTE2_UNION *)&(SVO[id*2].Temperature);
					
					SVO[id*2+1].Temperature = SVO[id*2].Temperature;
				}						
				
				memcpy(tmp, indextable->pSubindex[bSubindex].pObject, indextable->pSubindex[bSubindex].size); 

				if(indextable->index != 0x2016
				|| indextable->index != 0x2026	
				|| indextable->index != 0x2003	
				|| indextable->index != 0x2004					
				)
				{
						memcpy(pBU2->B2_U8, tmp, 2);	
				}		
				else
				{
						memcpy(pBU4->B4_U8, tmp, 4);			
				}				
		}
		else
		{
			if(indextable->index == 0x6069
			|| indextable->index == 0x606C
			|| indextable->index == 0x6079		
			|| indextable->index == 0x200B	
			|| indextable->index == 0x200C			
			)		
			{
					if(indextable->index == 0x6069)
					{//电机实际速度(ACTUAL VELOCITY)
						//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_ActiveSpeed + id];
						pBU2 = (BYTE2_UNION *)&(SVO[id].Speed);

						pBU4 = (BYTE4_UNION *)indextable->pSubindex[bSubindex].pObject;
						pBU2->B2_S16 = pBU4->B4_S32 * 0.006;   //rpm = (count * 60) / (2500 * 4)---单位:0.1rpm
					}	
					else if(indextable->index == 0x606C)
					{//电机实际速度(ACTUAL VELOCITY)
						//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_ActiveSpeed + id];
						pBU2 = (BYTE2_UNION *)&(SVO[id].Speed);
						//rpm = 512*编码器分辨率/1875  
						//对于回转电机，编码器分辨率为65536
							//对于行走电机，编码器分辨率为2500*4						
						pBU4 = (BYTE4_UNION *)indextable->pSubindex[bSubindex].pObject;

						if(CanMaster.Para.SVOType == Can_SVOType_Kinco)
						{
							//4代 步科 和 5代步科的编码器设计兼容
							if(SVO[id].SvoType == SvoType_Kinco_5)
							{
								pBU2->B2_S16 = pBU4->B4_S32 / 1789.5697;  //HMI反馈速度为0.1rpm，故2730.6变为273.06
							}
							else
							{
								pBU2->B2_S16 = pBU4->B4_S32 / 273.06;  //HMI反馈速度为0.1rpm，故2730.6变为273.06
							}
						}
						else if(CanMaster.Para.SVOType == Can_SVOType_Delta 
								 || CanMaster.Para.SVOType == Can_SVOType_Windcon
								 || CanMaster.Para.SVOType == Can_SVOType_Wheatstone)
						{
								pBU2->B2_S16 = pBU4->B4_S32;  //HMI反馈速度为0.1rpm
						}
					}		
					else if(indextable->index == 0x6079)
					{//总线电压
						//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_BusVoltage + id];
						pBU2 = (BYTE2_UNION *)&(SVO[id].Voltage);
						pBU4 = (BYTE4_UNION *)indextable->pSubindex[bSubindex].pObject;
						pBU2->B2_U16 = pBU4->B4_U32 / 100;   
					}	
					else if(indextable->index == 0x200B)//CanObjectDict_TPDO4_Delta_Map
					{//P0_11:驱动器错误码
						pBU4 = (BYTE4_UNION *)&(SVO[id].EventWord);
						pBU4_1 = (BYTE4_UNION *)indextable->pSubindex[bSubindex].pObject;
						pBU4->B4_U32 = pBU4_1->B4_U32;
					}			
					else if(indextable->index == 0x200C)//CanObjectDict_TPDO4_Delta_Map
					{//P0_12:负载保护计数
						pBU4 = (BYTE4_UNION *)&(SVO[id].LoadCnt);
						pBU4_1 = (BYTE4_UNION *)indextable->pSubindex[bSubindex].pObject;
						pBU4->B4_U32 = pBU4_1->B4_U32;					
					}				
			}
			else
			{
				if(indextable->index == 0x6079)       
				{//copy总线电压数据
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_BusVoltage + id];
					pBU2 = (BYTE2_UNION *)&(SVO[id].Voltage);
				}
				 else if(indextable->index == 0x6078)  
				{//copy实际电机电流
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_AMC + id];
					pBU2 = (BYTE2_UNION *)&(SVO[id].Current);
				}			
				else if(indextable->index == 0x2201)
				{//copy总线电压数据
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_BusVoltage + id];
					pBU2 = (BYTE2_UNION *)&(SVO[id].Voltage);
				}
				else if(indextable->index == 0x221C)
				{//copy实际电机电流
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_AMC + id];
					pBU2 = (BYTE2_UNION *)&(SVO[id].Current);
				}
				else if(indextable->index == 0x2202)
				{//copy温度值
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_TEMP + id];
					pBU2 = (BYTE2_UNION *)&(SVO[id].Temperature);
				}
				else if(indextable->index == 0x2190)
				{//输入pin状态
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_IOstatus + id];
					pBU2 = (BYTE2_UNION *)&(SVO[id].StatusIO);
				}
				else if(indextable->index == 0x6041)
				{//状态字
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_StatusWord + id];
					pBU2 = (BYTE2_UNION *)&(SVO[id].StatusWord);
				}
				else if(indextable->index == 0x1002)
				{//事件字
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_EventWord + (2*id)];
					pBU2 = (BYTE2_UNION *)&(SVO[id].EventWord);
				}
				else if(indextable->index == 0x2183)
				{//错误锁存
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_FaultLatch + (2*id)];
					pBU2 = (BYTE2_UNION *)&(SVO[id].FaultCode);
				}
				else if(indextable->index == 0x6061 || indextable->index == 0x2300)
				{//模式值
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_OPERATION_MODE + id];
					pBU2 = (BYTE2_UNION *)&(SVO[id].WorkMode);
				}
				else if(indextable->index == 0x2601)
				{//错误锁存
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_FaultLatch + (2*id)];
					pBU2 = (BYTE2_UNION *)&(SVO[id].FaultCode);
				}	
				else if(indextable->index == 0x2009)//CanObjectDict_TPDO1_Delta_Map
				{
					if(CanMaster.Para.SVOType == Can_SVOType_Delta)
					{//P0_09:母线电压
						pBU2 = (BYTE2_UNION *)&(SVO[id].Voltage);
					}
					else if(CanMaster.Para.SVOType == Can_SVOType_Wheatstone)
					{
						if(bSubindex == 0x04)
							pBU2 = (BYTE2_UNION *)&(SVO[id].Current);
						else if(bSubindex == 0x05)
							pBU2 = (BYTE2_UNION *)&(SVO[id].Voltage);	
					}					
				}
				else if(indextable->index == 0x200A)//CanObjectDict_TPDO1_Delta_Map
				{//P0_10:IGBT温度
					pBU2 = (BYTE2_UNION *)&(SVO[id].Temperature);
				}			
				else if(indextable->index == 0x6078)//CanObjectDict_TPDO2_Delta_Map
				{//6078:回授电流
					pBU2 = (BYTE2_UNION *)&(SVO[id].Current);
				}
				else if(indextable->index == 0x603F)//CanObjectDict_TPDO3_Delta_Map
				{//603F:错误代码
					pBU2 = (BYTE2_UNION *)&(SVO[id].FaultCode);		
				}
				else if(indextable->index == 0x2003)//CanObjectDict_TPDO1_Map
				{//2003:母线电压
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_BusVoltage + id];  
					pBU2 = (BYTE2_UNION *)&(SVO[id].Voltage);
				}			
				else if(indextable->index == 0x22A2)
				{//22A2:温度值
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_TEMP + id];
					pBU2 = (BYTE2_UNION *)&(SVO[id].Temperature);
				}		
				else if(indextable->index == 0x2000)
				{//2000:风得控驱动器故障信号
					pBU2 = (BYTE2_UNION *)&(SVO[id].FaultCode);	
				}					
				else 
				{
				
				}

				memcpy(tmp, indextable->pSubindex[bSubindex].pObject, indextable->pSubindex[bSubindex].size);            
							
				if(indextable->index != 0x200B)
				{
						memcpy(pBU2->B2_U8, tmp, 2);	
				}		
				else
				{
						memcpy(pBU4->B4_U8, tmp, 4);			
				}
			}	
	  }		
	}
	
	return OD_SUCCESSFUL;
}

UNS32 Callback_Tx_Map2(CO_Data* d, const indextable * indextable, UNS8 bSubindex)
{
	return OD_SUCCESSFUL;
}

UNS32 Callback_Rx_Map2(CO_Data* d, const indextable * indextable, UNS8 bSubindex)
{	
		BYTE2_UNION	BU2, *pBU2;	
//		BYTE4_UNION	BU4, *pBU4;	
		UNS8 id = d->SlaveNodeid - 1;

		pBU2=&BU2;
//		pBU4=&BU4;

		if(indextable)
		{
				//cmlin add
				if(indextable->index == 0x6063 || indextable->index == 0x6064)
				{
					//电机实际位置
					//pBU2 = (BYTE2_UNION *)&g_DHR_LinkCanOpen[Para_Index_StartAddr_ActivePosition + 2*id];
					pBU2 = (BYTE2_UNION *)&SVO[id].Position;
	//				pBU4 = (BYTE4_UNION *)indextable->pSubindex[bSubindex].pObject;
					memcpy(pBU2->B2_U8, indextable->pSubindex[bSubindex].pObject, indextable->pSubindex[bSubindex].size);		
				}
		}
		return OD_SUCCESSFUL;
}

/***************************  INIT  *****************************************/
void InitNodes(CO_Data* d, UNS32 id)
{
//	setNodeId(d, id);
	setNodeId(d, 0x00);
	setState(d, Initialisation);//主机状态初始化
}

/***************************  EXIT  *****************************************/
void CanOpen_Exit(CO_Data* d, UNS32 id)
{
	masterSendNMTstateChange(d, id, NMT_Reset_Node);    

	setState(d, Stopped);//Stop master
}

void canfestival_timer_start(uint8_t pi, FunctionalState NewState)
{
    CanFestivalTimer_Config(pi, NewState);  /* 配置GD32H7 TIMER2 */
}

void can_od_init(CO_Data* d)
{
	u8 id,ch,j;
	u16 i,index=0,heartbeat;
	
	/*ConsumerHeartbeatEntries 初始化,设置检测从机返回timeout值!!!*/	
	heartbeat = Producer_Heartbeat_Times * Producer_Heartbeat_TimesFactor;//设置heart消费时间为生产时间的Producer_Heartbeat_TimesFactor倍
	for(id=1; id<=MAX_NodeId_NUM; id++)
	{
		index = id-1;
		CanObjectDict_obj1016[index] = (((u32)(id) << 16) | heartbeat);//心跳消费时间设置
	}
	
	/*初始化 Clientx SDO Parameter*/
	for(id=1,i=d->firstIndex->SDO_CLT;id<=MAX_NodeId_NUM && i<=d->lastIndex->SDO_CLT;id++,i++)
	{
		index = id-1;
		for(j=0;j<d->objdict[i].bSubCount;j++)
		{
			switch(j)
			{
				case 0:	
					Obj_Clentx_SDO[index].highestSubIndex = 0x03;
					d->objdict[i].pSubindex[0].pObject = (void*)&Obj_Clentx_SDO[index].highestSubIndex;
				break;
				case 1:	
					Obj_Clentx_SDO[index].COB_ID_Client_to_Server_Transmit_SDO = 0x600 + id;
					d->objdict[i].pSubindex[1].pObject = (void*)&Obj_Clentx_SDO[index].COB_ID_Client_to_Server_Transmit_SDO;// 0x600 + id
				break;
				case 2:	
					Obj_Clentx_SDO[index].COB_ID_Server_to_Client_Receive_SDO = 0x580 + id;
					d->objdict[i].pSubindex[2].pObject = (void*)&Obj_Clentx_SDO[index].COB_ID_Server_to_Client_Receive_SDO;// 0x580 + id
				break;
				case 3:	
					Obj_Clentx_SDO[index].Node_ID_of_the_SDO_Server = id;
					d->objdict[i].pSubindex[3].pObject = (void*)&Obj_Clentx_SDO[index].Node_ID_of_the_SDO_Server;		
				break;
				default:	
				break;
			}
		}
	}
	
	/*初始化从机的TPDO通讯参数*/
	for(ch=0,i=d->firstIndex->PDO_RCV;ch<USE_TPDO_CH_NUM && i<=d->lastIndex->PDO_RCV;ch++)
	{
		for(id=1;id<=MAX_NodeId_NUM;id++,i++)
		{
			index = id-1;
			for(j=0;j<d->objdict[i].bSubCount;j++)
			{
				switch(j)
				{
					case 0:	
						Obj_Clentx_TPDO[ch][index].highestSubIndex = 0x05;
						d->objdict[i].pSubindex[0].pObject = (void*)&Obj_Clentx_TPDO[ch][index].highestSubIndex;
					break;
					case 1:	                       
						if(d->objdict[i].index >= 0x1400 && d->objdict[i].index <= 0x141F)
						{
							Obj_Clentx_TPDO[ch][index].COB_ID = 0x180 + id;// 0x180 + id
						}
						else if(d->objdict[i].index >= 0x1420 && d->objdict[i].index <= 0x143F)
						{
							Obj_Clentx_TPDO[ch][index].COB_ID = 0x280 + id;// 0x280 + id
						}
						else if(d->objdict[i].index >= 0x1440 && d->objdict[i].index <= 0x145F) 
						{
							Obj_Clentx_TPDO[ch][index].COB_ID = 0x380 + id;// 0x380 + id
						}
						else if(d->objdict[i].index >= 0x1460 && d->objdict[i].index <= 0x147F)
						{
							Obj_Clentx_TPDO[ch][index].COB_ID = 0x480 + id;// 0x480 + id
						}
						d->objdict[i].pSubindex[1].pObject = (void*)&Obj_Clentx_TPDO[ch][index].COB_ID;                        
					break;
					case 2:	
						Obj_Clentx_TPDO[ch][index].Transmission_type = 0;
						d->objdict[i].pSubindex[2].pObject = (void*)&Obj_Clentx_TPDO[ch][index].Transmission_type;
					break;
					case 3:	
						Obj_Clentx_TPDO[ch][index].Inhibit_times = 0;
						d->objdict[i].pSubindex[3].pObject = (void*)&Obj_Clentx_TPDO[ch][index].Inhibit_times;
					break;
					case 4:	
						Obj_Clentx_TPDO[ch][index].Event_timer = 0;
						d->objdict[i].pSubindex[4].pObject = (void*)&Obj_Clentx_TPDO[ch][index].Event_timer;
					break;
					case 5:	
						Obj_Clentx_TPDO[ch][index].SYNC_startvalue = 0;
						d->objdict[i].pSubindex[5].pObject = (void*)&Obj_Clentx_TPDO[ch][index].SYNC_startvalue;
					break;
					default:	
					break;
				}
			}
		}
        
        i += (MAX_NodeId_Support - MAX_NodeId_NUM);
	}
	
	/*初始化主机对象字典*/
	for(i=d->firstIndex->PDO_TRS;i<=d->lastIndex->PDO_TRS;i++)
	{
			for(j=0;j<d->objdict[i].bSubCount;j++)
			{
					d->objdict[i].pSubindex[j].pObject = (void*)&InitDataBuffer;	
			}
	}

	/*初始化从机的RPDO通讯参数*/
	for(ch=0,i=d->firstIndex->PDO_TRS;ch<USE_RPDO_CH_NUM && i<=d->lastIndex->PDO_TRS;ch++)
	{
		for(id=1;id<=MAX_NodeId_NUM;id++,i++)
		{
			index = id-1;
			for(j=0;j<d->objdict[i].bSubCount;j++)
			{
				switch(j)
				{
					case 0:	
						Obj_Clentx_RPDO[ch][index].highestSubIndex = 0x05;
						d->objdict[i].pSubindex[0].pObject = (void*)&Obj_Clentx_RPDO[ch][index].highestSubIndex;
					break;
					case 1:	
						if(d->objdict[i].index >= 0x1800 && d->objdict[i].index <= 0x181F)
						{
							Obj_Clentx_RPDO[ch][index].COB_ID = 0x200 + id;// 0x200 + id
						}
						else if(d->objdict[i].index >= 0x1820 && d->objdict[i].index <= 0x183F)
						{
							Obj_Clentx_RPDO[ch][index].COB_ID = 0x300 + id;// 0x300 + id
						}     
						else if(d->objdict[i].index >= 0x1840 && d->objdict[i].index <= 0x185F)
						{
							Obj_Clentx_RPDO[ch][index].COB_ID = 0x400 + id;// 0x400 + id
						}   
						else if(d->objdict[i].index >= 0x1860 && d->objdict[i].index <= 0x187F)
						{
							Obj_Clentx_RPDO[ch][index].COB_ID = 0x500 + id;// 0x500 + id
						}                         
						d->objdict[i].pSubindex[1].pObject = (void*)&Obj_Clentx_RPDO[ch][index].COB_ID;
					break;
					case 2:	
						Obj_Clentx_RPDO[ch][index].Transmission_type = TRANS_SYNC_MIN;
						d->objdict[i].pSubindex[2].pObject = (void*)&Obj_Clentx_RPDO[ch][index].Transmission_type;
					break;
					case 3:	
						Obj_Clentx_RPDO[ch][index].Inhibit_times = 0;
						d->objdict[i].pSubindex[3].pObject = (void*)&Obj_Clentx_RPDO[ch][index].Inhibit_times;
					break;
					case 4:	
						Obj_Clentx_RPDO[ch][index].Event_timer = 0;
						d->objdict[i].pSubindex[4].pObject = (void*)&Obj_Clentx_RPDO[ch][index].Event_timer;
					break;
					case 5:	
						Obj_Clentx_RPDO[ch][index].SYNC_startvalue = 0;
						d->objdict[i].pSubindex[5].pObject = (void*)&Obj_Clentx_RPDO[ch][index].SYNC_startvalue;
					break;
					default:	
					break;
				}
			}
		}
        
        i += (MAX_NodeId_Support - MAX_NodeId_NUM);
	}
	
	/****************************** 初始化PDO接收回调 *******************************/
	RegisterSetODentryCallBack(d, 0x1002, 0, &Callback_Rx_Map1);	
	RegisterSetODentryCallBack(d, 0x2201, 0, &Callback_Rx_Map1);
	RegisterSetODentryCallBack(d, 0x221C, 0, &Callback_Rx_Map1);
	RegisterSetODentryCallBack(d, 0x2202, 0, &Callback_Rx_Map1);
	RegisterSetODentryCallBack(d, 0x2190, 0, &Callback_Rx_Map1);
	RegisterSetODentryCallBack(d, 0x6041, 0, &Callback_Rx_Map1);
	RegisterSetODentryCallBack(d, 0x6061, 0, &Callback_Rx_Map1);
	RegisterSetODentryCallBack(d, 0x2300, 0, &Callback_Rx_Map1);
	RegisterSetODentryCallBack(d, 0x6069, 0, &Callback_Rx_Map1);
	RegisterSetODentryCallBack(d, 0x2183, 0, &Callback_Rx_Map1);    
	RegisterSetODentryCallBack(d, 0x2009, 0, &Callback_Rx_Map1);
	RegisterSetODentryCallBack(d, 0x200A, 0, &Callback_Rx_Map1);
	RegisterSetODentryCallBack(d, 0x200B, 0, &Callback_Rx_Map1);	
	RegisterSetODentryCallBack(d, 0x200C, 0, &Callback_Rx_Map1);
	RegisterSetODentryCallBack(d, 0x603F, 0, &Callback_Rx_Map1);	
	RegisterSetODentryCallBack(d, 0x6079, 0, &Callback_Rx_Map1);	
	RegisterSetODentryCallBack(d, 0x6078, 0, &Callback_Rx_Map1);	
	RegisterSetODentryCallBack(d, 0x606C, 0, &Callback_Rx_Map1);	
	RegisterSetODentryCallBack(d, 0x2601, 0, &Callback_Rx_Map1);	
	RegisterSetODentryCallBack(d, 0x2602, 0, &Callback_Rx_Map1);	
	RegisterSetODentryCallBack(d, 0x2003, 0, &Callback_Rx_Map1);	
	RegisterSetODentryCallBack(d, 0x22A2, 0, &Callback_Rx_Map1);		
	RegisterSetODentryCallBack(d, 0x2000, 0, &Callback_Rx_Map1);	
	
	RegisterSetODentryCallBack(d, 0x6063, 0, &Callback_Rx_Map2);     //motor active pos	
	RegisterSetODentryCallBack(d, 0x6064, 0, &Callback_Rx_Map2);	
	
	RegisterSetODentryCallBack(d, 0x6040, 0, &Callback_Tx_Map2);	
	RegisterSetODentryCallBack(d, 0x60FF, 0, &Callback_Tx_Map2);
	RegisterSetODentryCallBack(d, 0x210C, 0, &Callback_Tx_Map2);			

  if(CanMaster.Para.SVOType == Can_SVOType_Keya)
	{
		  RegisterSetODentryCallBack(d, 0x2012, 0, &Callback_Rx_Map1);	
		  RegisterSetODentryCallBack(d, 0x2010, 0, &Callback_Rx_Map1);	
		  RegisterSetODentryCallBack(d, 0x2016, 0, &Callback_Rx_Map1);	
		  RegisterSetODentryCallBack(d, 0x2022, 0, &Callback_Rx_Map1);	
		  RegisterSetODentryCallBack(d, 0x2020, 0, &Callback_Rx_Map1);	
		  RegisterSetODentryCallBack(d, 0x2026, 0, &Callback_Rx_Map1);	
		  RegisterSetODentryCallBack(d, 0x2011, 0, &Callback_Rx_Map1);	
		  RegisterSetODentryCallBack(d, 0x2021, 0, &Callback_Rx_Map1);	
		  RegisterSetODentryCallBack(d, 0x2013, 0, &Callback_Rx_Map1);	
		  RegisterSetODentryCallBack(d, 0x203C, 0, &Callback_Rx_Map1);	
		  RegisterSetODentryCallBack(d, 0x2003, 0, &Callback_Rx_Map1);	
		  RegisterSetODentryCallBack(d, 0x2004, 0, &Callback_Rx_Map1);			
		
	    RegisterSetODentryCallBack(d, 0x607A, 0, &Callback_Tx_Map2);	
	    RegisterSetODentryCallBack(d, 0x6081, 0, &Callback_Tx_Map2);		
	    RegisterSetODentryCallBack(d, 0x6083, 0, &Callback_Tx_Map2);	
	    RegisterSetODentryCallBack(d, 0x6084, 0, &Callback_Tx_Map2);			
	}	
	else
	{
	    RegisterSetODentryCallBack(d, 0x607A, 0, &Callback_Rx_Map2);	
	}
		
	d->heartbeatError = CanObjectDict_heartbeatError;
	d->initialisation = CanObjectDict_initialisation;
	d->preOperational = CanObjectDict_preOperational;
	d->operational    = CanObjectDict_operational;
	d->stopped        = CanObjectDict_stopped;
	d->post_sync      = CanObjectDict_post_sync;
	d->post_TPDO      = CanObjectDict_post_TPDO;
	d->post_emcy      = CanObjectDict_post_emcy;
	d->post_SlaveBootup      = CanObjectDict_post_SlaveBootup;
	d->post_SlaveStateChange = CanObjectDict_post_SlaveStateChange;
	d->nodeguardError = CanObjectDict_nodeguardError;
}

//============================================================================
// 名称：can_master_init
// 功能：can_master主机相关初始化
// 参数：无
// 返回：无
// 说明：无
//============================================================================
void can_master_init(CO_Data* d, FunctionalState NewState)
{
    if (CanMaster.Para.Baud >= CAN_BAUD_5K && CanMaster.Para.Baud <= CAN_BAUD_1M)
    {
        GD32_CAN_Init(CanMaster.Para.Baud, 1U);
        can_od_init(d);
    }
    CanFestivalTimer_Enable(NewState);
}

const u32 HeartMsgTimeoutJfc = Producer_Heartbeat_Times * 0.5;//设定为总时间的0.2倍

//============================================================================
// 名称：SlaveHeartMessageProcess
// 功能：从机心跳消息处理
// 说明：周期性调用
//============================================================================
u8 SlaveHeartMessageProcess(CO_Data* d, Message *pMsg)
{
	u8 NodeId;
	e_nodeState NodeState;
        
	if(d->nodeState != Operational)
		return 1;

	NodeId=pMsg->cob_id & 0x7F;
	NodeState=(e_nodeState)(pMsg->data[0] & 0x7F);
	if(NodeState==Operational && NodeId<=MAX_NodeId_NUM && NodeId>=1)
	{
		switch (pMsg->cob_id >> 7)
        {
			case NODE_GUARD://心跳报文...
			{
				CanMaster.HeartJfcCnt = HeartMsgTimeoutJfc;//置心跳包计时初始值
				CanMaster.HeartMsgId[NodeId-1]=NodeId;
			}
			break;
			case SYNC:
			{
			
			}
			break;
			default:
			break;
        }
    } 
	
	return 0;
}

//============================================================================
// 名称：SlaveHeartMessageProcess_Keya
// 功能：从机心跳消息处理
// 说明：周期性调用
//============================================================================
u8 SlaveHeartMessageProcess_Keya(CO_Data* d, Message *pMsg)
{
	u8 NodeId_A,NodeId_B;
	e_nodeState NodeState;
	
	static u16 SlaverHeartbeatCnt[MAX_NodeId_NUM] = {0};
	static u32 SlaverHeartbeat_delay_cnt = 0;
	
	if(d->nodeState != Operational)
		return 1;

	NodeId_A = ((pMsg->cob_id & 0x7F)-1)*2;
	NodeId_B = NodeId_A+1;
	NodeState=(e_nodeState)(pMsg->data[0] & 0x7F);
	//防止数据越界
	if(NodeId_A >= MAX_NodeId_NUM)
		return 1;
	if(NodeState==Operational)
	{
		//A电机
		SlaverHeartbeatCnt[NodeId_A] = 1000;
		//B电机
		if(NodeId_B < MAX_NodeId_NUM)
		{
			SlaverHeartbeatCnt[NodeId_B] = 1000;
		}
	} 

	if(Delay_Fuction(g_Sys_1ms_Counter, SlaverHeartbeat_delay_cnt, 1))
	{
		SlaverHeartbeat_delay_cnt = g_Sys_1ms_Counter;
		{
			u8 i = 0;
			for(i=0;i<MAX_NodeId_NUM;i++)
			{
				SlaverHeartbeatCnt[i]--;
				if(SlaverHeartbeatCnt[i] <= 1)
				{
					SlaverHeartbeatCnt[i] = 1;
				}
				if(SlaverHeartbeatCnt[i] == 1)
				{
					SVO[i].SlaveHeartbeatError = 1;
				}
			}
		}
	}

	return 0;
}

/*********************************************************************************************************
*	函 数 名: Tsk_CanOpen_TxMsgPro
*	功能说明: canopen接收数据包处理
*********************************************************************************************************/
void Tsk_CanOpen_TxMsgPro(CO_Data* d)
{
    GD32_CAN_TX_FIFO_t *pFifo = &GD32_CAN.tx_fifo;
    if (pFifo->read_adr != pFifo->write_adr)
    {
        GD32_CAN_SendMsg(&pFifo->msg[pFifo->read_adr]);
        if (++pFifo->read_adr >= CAN_TX_BUF_LENGTH_MAX) {
            pFifo->read_adr = 0U;
        }
    }
}

/*********************************************************************************************************
*	函 数 名: Tsk_CanOpen_RxMsgPro
*	功能说明: canopen接收数据包处理
*********************************************************************************************************/
void Tsk_CanOpen_RxMsgPro(CO_Data* d)
{
    Message Msg, *pMsg;
    GD32_CAN_RX_FIFO_t *pFifo = &GD32_CAN.rx_fifo;

    if (pFifo->read_adr == pFifo->write_adr) {
        return;
    }

    pMsg = &Msg;
    pMsg->len    = pFifo->msg[pFifo->read_adr].dlc;
    pMsg->cob_id = (UNS16)pFifo->msg[pFifo->read_adr].id;
    pMsg->rtr    = pFifo->msg[pFifo->read_adr].rtr;
    memcpy(pMsg->data, pFifo->msg[pFifo->read_adr].data, pMsg->len);

    if (++pFifo->read_adr >= CAN_RX_BUF_LENGTH_MAX) {
        pFifo->read_adr = 0U;
    }

    if (pMsg->len > 0U)
    {
        CanFestivalTimer_Ctrl(DISABLE);
        if (CanMaster.Para.SVOType == Can_SVOType_Keya) {
            SlaveHeartMessageProcess_Keya(d, pMsg);
        }
        canDispatch(d, pMsg);
        CanFestivalTimer_Ctrl(ENABLE);
    }
}


u32 canopen_delay_cnt;//canopen延时计数

//canopen从站配置 初始化任务
bool LinkCO_ParaIni(LinkCanOpen *pCm, CO_Data* d)
{
    GD32_CAN.rx_fifo.read_adr  = 0U;
    GD32_CAN.rx_fifo.write_adr = 0U;
    GD32_CAN.tx_fifo.read_adr  = 0U;
    GD32_CAN.tx_fifo.write_adr = 0U;
    memset(&CanMaster.ConfigStep, 0, sizeof(CanMaster.ConfigStep));
    memset(&CanMaster.SlaveParaConfigId, 0, sizeof(CanMaster.SlaveParaConfigId));
    NMT_Reset_NodeIDCnt = 0;
    pCm->CanopenStep = Step_DrivePowerDown;
    return 1;
}

//canopen从站配置 伺服下电
bool LinkCO_DrivePowerDown(LinkCanOpen *pCm, CO_Data* d)
{
    can_master_init(d, ENABLE);
    setState(d, Stopped);
    CanMaster.MasterStatu = Stopped;
    pCm->CanopenStep = Step_DrivePowerUp;
    canopen_delay_cnt = g_Sys_1ms_Counter;
    return 1;
}

//canopen从站配置 伺服上电
bool LinkCO_DrivePowerUp(LinkCanOpen *pCm, CO_Data* d)
{
    if (Delay_Fuction(g_Sys_1ms_Counter, canopen_delay_cnt, 500))
    {
        pCm->CanopenStep = Step_Reset_Node;
        canopen_delay_cnt = g_Sys_1ms_Counter;
    }
    return 1;
}

//canopen从站配置 复位节点
bool LinkCO_ResetNode(LinkCanOpen *pCm, CO_Data* d)
{
    if (Delay_Fuction(g_Sys_1ms_Counter, canopen_delay_cnt, 3000))
    {
        masterSendNMTstateChange(d, 0x00, NMT_Stop_Node);
        masterSendNMTstateChange(d, 0x00, NMT_Reset_Node);
        pCm->CanopenStep = Step_Reset_Comunication;
        canopen_delay_cnt = g_Sys_1ms_Counter;
    }
    return 1;
}

//canopen从站配置 复位通讯
bool LinkCO_ResetComunication(LinkCanOpen *pCm, CO_Data* d)
{
//	if(Delay_Fuction(g_Sys_1ms_Counter, canopen_delay_cnt, 500))
//	{
//		setState(d, Initialisation);
//		CanMaster.MasterStatu = Initialisation;
//		masterSendNMTstateChange(d, 0x00, NMT_Reset_Comunication);//复位node通讯层
//		pCm->CanopenStep=Step_WaitBootUpOk;
//		canopen_delay_cnt=g_Sys_1ms_Counter;
//	}
      //实际测试发现，上电无法检测所有驱动器上线，故修改此处，对单个驱动器进行复位通讯层处理
    if(Delay_Fuction(g_Sys_1ms_Counter, canopen_delay_cnt, 500))   //250
    {
//		SetAlarm(d, 0, &InitNodes, 0, 0);//立即在 canfestival_timer 中调度执行 InitNodes()
        setState(d, Initialisation);
        CanMaster.MasterStatu = Initialisation;

        masterSendNMTstateChange(d, NMT_Reset_NodeIDCnt+1, NMT_Reset_Comunication);//复位node通讯层	
        canopen_delay_cnt=g_Sys_1ms_Counter;	
        NMT_Reset_NodeIDCnt++;						
        
        if(NMT_Reset_NodeIDCnt==MAX_NodeId_NUM)
        {
                pCm->CanopenStep=Step_WaitBootUpOk;
                canopen_delay_cnt=g_Sys_1ms_Counter;							
        }
    }
	return 1;
}

//canopen从站配置 复位通讯
bool LinkCO_WaitBootUpOk(LinkCanOpen *pCm, CO_Data* d)
{
	if(Delay_Fuction(g_Sys_1ms_Counter, canopen_delay_cnt, 500))
	{//4秒内，固定时间
		u8 i,id;
		for(i=0;i<MAX_NodeId_NUM;i++)
		{
			id=i+1;
			
			if(CanMaster.Para.SVOType == Can_SVOType_Kinco 
			&& CanMaster.SlaveParaConfigId[i]==id 
			&& CanMaster.ConfigStep[i]>=Config_step_Last_Kinco)
			{
				  //CanMaster.SlaveParaConfigId[i]=CanMaster.ConfigStep[i]=0;
				  CanMaster.SlaveOnLineNum++;	
			}
			else if(CanMaster.Para.SVOType == Can_SVOType_Delta 
			     && CanMaster.SlaveParaConfigId[i]==id 
			     && CanMaster.ConfigStep[i]>=Config_step_Last_Delta)
			{
				  //CanMaster.SlaveParaConfigId[i]=CanMaster.ConfigStep[i]=0;
				  CanMaster.SlaveOnLineNum++;	
			}	
			else if(CanMaster.Para.SVOType == Can_SVOType_Windcon 
			     && CanMaster.SlaveParaConfigId[i]==id 
			     && CanMaster.ConfigStep[i]>=Config_step_Last_Windcon)				
			{
				  //CanMaster.SlaveParaConfigId[i]=CanMaster.ConfigStep[i]=0;
				  CanMaster.SlaveOnLineNum++;
			}	
			else if(CanMaster.Para.SVOType == Can_SVOType_Wheatstone 
			    && CanMaster.SlaveParaConfigId[i]==id 
			    && CanMaster.ConfigStep[i]>=Config_step_Last_Wheatstone)				
			{
				  //CanMaster.SlaveParaConfigId[i]=CanMaster.ConfigStep[i]=0;
				  CanMaster.SlaveOnLineNum++;
			}		
			else if(CanMaster.Para.SVOType == Can_SVOType_Keya 
			    && CanMaster.SlaveParaConfigId[i]==id 
			    && CanMaster.ConfigStep[i]>=Config_step_Last_Keya)				
			{
				  //CanMaster.SlaveParaConfigId[i]=CanMaster.ConfigStep[i]=0;
				  CanMaster.SlaveOnLineNum++;
			}				
			else if(CanMaster.Para.SVOType == Can_SVOType_Copley 
			    && CanMaster.SlaveParaConfigId[i]==id 
			    && CanMaster.ConfigStep[i]>=Config_step_Last_Copley)				
			{
				  //CanMaster.SlaveParaConfigId[i]=CanMaster.ConfigStep[i]=0;
				  CanMaster.SlaveOnLineNum++;
			}			
		}
		
		if(CanMaster.SlaveOnLineNum >= CanMaster.Para.CheckQuantity)
		{
			masterSendNMTstateChange (d, 0x00, NMT_Start_Node);		
			setState(d, Operational);
			CanMaster.MasterStatu = Operational;
			pCm->CanopenStep = Step_Active;	
			canopen_delay_cnt=g_Sys_1ms_Counter;			
		}
		else
		{
			//CanMaster.RepeatConnectCnt++;
			if(CanMaster.SlaveOnLineNum==0)
				pCm->Err.CanNoSlv = 1;
			else
				pCm->Err.CanLessSlv = 1;
				
			pCm->CanopenStep = Step_CanOpenFault;
		}
	}
	return 1;
}

//canopen循环任务
void tsk_canopen_master(LinkCanOpen *pCm, CO_Data* d)
{
	//u8 i=0;
	//static u32 canopen_delay_cnt;

	if(0) /* GD32H7: no STM32_CANi; handle CAN errors separately if needed */
	{//在非静止canopen_master的情形下,出现主机链路层异常 或 断线  !!!
		/* p_stCAN CAN error flag removed for GD32H7 */
		pCm->CanopenStep = Step_ParaIni;
	}

	switch (pCm->CanopenStep)
	{
		case Step_ParaIni://初始化OD参数....
		{
			//等待启动信号
			if(g_PowerOn.Ctrl_CanOpenStart)
				LinkCO_ParaIni(pCm, d);
		}
		break;
			
		case Step_DrivePowerDown://驱动器断电 and 设置主机状态stop
		{
			LinkCO_DrivePowerDown(pCm, d);
		}
		break;
		
		case Step_DrivePowerUp://驱动器上电
		{
			LinkCO_DrivePowerUp(pCm, d);
		}
		break;
				
		case Step_Reset_Node://复位node应用层(相关应用参数的初始值)和停止节点
		{
			LinkCO_ResetNode(pCm, d);
		}
		break;
		
		case Step_Reset_Comunication://复位node通讯层,此时,可以处理boot-up报文
		{
			LinkCO_ResetComunication(pCm, d);
		}
		break;
		
		case Step_WaitBootUpOk://等待接收、处理从机的boot-up报文,判断参数是否初始化ok
		{
			LinkCO_WaitBootUpOk(pCm, d);
		}
		break;

		case Step_Active://相关命令任务
		{

		}
		break;
		
		case Step_CanOpenFault://停止canopen_master
		{

		}
		break;
		
		default:
		break;
	}
}
//***********************************END OF FILE***********************************
