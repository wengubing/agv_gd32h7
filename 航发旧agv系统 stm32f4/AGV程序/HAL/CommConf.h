/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: CommConf.h
** 创 建 人: 文小兵  
** 描    述: 通信接口配置
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月10日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __CommConf_h__
#define __CommConf_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  CommConf_GLOBALS
	#define CommConf_EXT
	#define CommConf_INIT ={0}
#else
	#define CommConf_EXT  extern
	#define CommConf_INIT
#endif

//自定义引用	

#include "Stm32f4_Usart_Define.h"
#include "..\..\Drivers\ModbusRtuPoll\drv_485Usart.h"
#include "CommConf.h"
#include "MB_LinkApp.h"
#include "app_SkeletonAGV.h"

//宏定义
#define SLVS_MAXNUM											Master_SLV_MAX_NUM//最大总线从机数量
#define SLVS_MAXNUM_NOTFIND									(SLVS_MAXNUM+1)//未找到从机
#define CCF_MAXNUM											USARTMAXCNT//最大通信接口数量

	
#define TimeOutMsMax										60000//超时毫秒计数最大值

//协议角色定义
typedef enum
{
	CCFMBMaster=0,											//主站
	CCFMBSlave,												//从站
}CCFRole_EnumDef;

//协议格式定义
typedef enum
{
	CCFASCII		=0,	//ASCII格式
	CCFRTU			=1,	//RTU格式
	CCFTCP			=2,	//TCP格式
	CCFCustomUART	=3,	//自定义串口协议
}CCF_Format_EnumDef;

//通信配置对象编号定义
typedef enum
{
	CCFCN13=useUART4,//485
	CCFCN14=useUART6,//485
	CCFCN15=useUART3,//232
	CCFCN16=useUART5,//复用232
	CCFCN21=useUART1,//485
	CCFCN22=useUART2,//485
	CCFCN_Disable=10,//禁用
}CCFID_EnumDef;

//通信接口 通用参数 对象定义
typedef struct
{
	CCFRole_EnumDef Role;									//协议角色
	USART_BAUD Baud;										//波特率值
	unsigned char Pi;										//中断优先级编号
	CCF_Format_EnumDef Format;								//协议格式
}CCFParaPublic_StDef;

//通信接口 主站参数 对象定义
typedef struct
{
	unsigned short Timeout;									//超时时间 ms
	unsigned short SendDelay;								//发送间隔 ms
	unsigned short Retry;									//重发次数
}CCFParaMaster_StDef;

//通信接口 从站参数 对象定义
typedef struct
{
	u16* p2SlaveData;										//数据指针
	unsigned short SlaveDataLen;							//数据长度
}CCFParaSlave_StDef;

//具体做主站参数 定义
typedef struct
{
	CCFParaPublic_StDef PubPara;							//通信接口通用参数
	CCFParaMaster_StDef MstPara;							//主站参数
	u8 SlaveId;												//从站Id
	MB_REG_INDEX MBConf;									//主站数据池参数
	
	CCFID_EnumDef CommId;									//通信接口编号
	unsigned short CommOkCnt;								//通信成功次数
	unsigned short TimeOutMs;								//超时计数
}CCFEQP_Master_StDef;

//具体做从站参数 定义
typedef struct
{
	CCFParaPublic_StDef PubPara;							//通信接口通用参数
	CCFParaSlave_StDef SlvPara;								//从站参数
	
	CCFID_EnumDef CommId;									//通信接口编号
	unsigned short CommOkCnt;								//通信成功次数
	unsigned short TimeOutMs;								//超时计数
}CCFEQP_Slave_StDef;

//通信遍历配置结果 定义
typedef enum
{
	CCFInitRes_Init = 0,									//初始化
	CCFInitRes_Slv,											//从站
	CCFInitRes_Mst,											//主站
}CCFInitRes_EnumDef;

//通信接口遍历记录 对象定义
typedef struct
{
	CCFInitRes_EnumDef InitRes;								//初始化结果
	unsigned char SlvNum;									//从站数量
	MB_REG_INDEX SlvPara[SLVS_MAXNUM];						//从站参数数组
	unsigned char ID[SLVS_MAXNUM];							//从站ID数组

	CCFEQP_Master_StDef* p2Mst[SLVS_MAXNUM];				//从机对象指针队列
	CCFEQP_Slave_StDef* p2Slv;								//从站对象指针
}CCFInitRecord_StDef;

//通信接口 对象定义
typedef struct
{
	CCFParaPublic_StDef PubPara;							//通信接口通用参数
	CCFParaMaster_StDef MstPara;							//主站参数
	CCFParaSlave_StDef SlvPara;								//从站参数
	CCFID_EnumDef UartID;									//硬件绑定编号

	CCFInitRecord_StDef InitRecord;							//遍历初始化记录
	
	bool RxOk;												//接收一帧数据完成标志
}CCF_StDef;

//快速索引表 定义
typedef struct
{
	MB_QUICK_INDEX QuickIndex[LINK_QUE_NUM];
}MB_QUICK_INDEXs;

//接口组初始化记录 对象定义
typedef struct
{
	unsigned char SlvNum;									//从站接口数量
	CCFID_EnumDef SlvCCFID[CCF_MAXNUM];						//从站接口号队列
	unsigned char MstNum;									//主站接口数量
	CCFID_EnumDef MstCCFID[CCF_MAXNUM];						//主站接口队列
	MB_QUICK_INDEXs QuickIndexs[CCF_MAXNUM];				//快速索引表
	MB_REG_INDEX_TABLE MstCCF[CCF_MAXNUM];					//从机参数表队列
}CCFsInitRecord_StDef;

//通信接口组 对象定义
typedef struct
{	//注意顺序不能随意修改，要与CCF_ID_EnumDef定义匹配！
	CCF_StDef CCFCN21;
	CCF_StDef CCFCN22;
	CCF_StDef CCFCN15;
	CCF_StDef CCFCN13;
	CCF_StDef CCFCN16;
	CCF_StDef CCFCN14;
	CCFsInitRecord_StDef Record;							//初始化记录
	unsigned short Number;									//外设数量
}CCFs_StDef;


//对象外部声明
CommConf_EXT CCFs_StDef						CCFs;							//物理接口通信对象

//方法外部声明
CommConf_EXT bool CommConf_Init(void);						//通信接口配置初始化
CommConf_EXT bool EQPMstInit(
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
	u8 rw_size,
	u16* pHR2,
	u16	addr2,
	u8 rw_size2,
	LINK_QUE_TYPE linktpye,

	CCFID_EnumDef CommId									//通信接口编号
);															//主站初始化

bool EQPSlvInit(
	CCFEQP_Slave_StDef* p,
	USART_BAUD Baud,										//波特率值
	unsigned char Pi,										//中断优先级编号
	CCF_Format_EnumDef Format,								//协议格式
	u16* p2SlaveData,										//数据指针
	unsigned short SlaveDataLen,							//数据长度
	CCFID_EnumDef CommId									//通信接口编号
);															//从站初始化

#endif
	
CommConf_EXT bool ObjNumberCal(void);						//计算外设总数

//************************************END OF FILE***********************************


