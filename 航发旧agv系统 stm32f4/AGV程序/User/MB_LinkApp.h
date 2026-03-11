/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: MB_LinkApp.h
** 创 建 人: 文小兵  
** 描    述: modbus应用层数据命令队列管理
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年5月13日 重构
**--------------------------------------------------------------------------------------
***************************************************************************************/

#ifndef _MB_LINKAPP_H_
#define _MB_LINKAPP_H_


#ifdef		MBLINK_GLOBALS
	#define MBLINK_EXT
#else
	#define MBLINK_EXT  extern
#endif

//自定义引用
#include "..\..\Drivers\ModbusRtuPoll\ModbusRtuPoll.h"

#define		 Master_SLV_MAX_NUM								5//最大从机数量
#define		 NML_QUE_SIZE									16//定义一般队列fifo大小
#define		 INJ_QUE_SIZE									16//定义一般队列fifo大小
#define		 LINK_ERR_CNT									3//定义通讯错误次数

//注入队列命令定义
typedef enum
{
	SET_SERVO_NULL=0,
	SET_SERVO_SPX, //设置速度
	
}INJ_CMD_TYPE;

//link que 形式
typedef enum
{
	ONCE_QUE=0,												//单次队列
	NORMAL_QUE,												//一般循环队列
	INJ_QUE,												//注入队列
	LINK_QUE_NUM,
	NULL_QUE,												//无执行队列
}LINK_QUE_TYPE;

//link error定义
typedef enum
{
	LINK_CNT_ERROR=0,										//通讯错误次数异常
	
}LINK_ERROR_TYPE;

//注入队列数据
typedef struct
{
	u16 *pData_new, 
	    *pData_old;
	u8  size;
}LINK_INJ_DATA;

//操作寄存器结构体
typedef struct
{
    MD_FUNC func;											//功能码
	u8 sel;													//选择

	//读配置
	u16* pHR;												//数据存放地址
	u16 addr;												//寄存器地址
	u8 rw_size;												//读写长度
	
	//写配置
    u16 *pHR2;
    u16 addr2;
	u8 rw_size2;	

	LINK_QUE_TYPE linktpye; //注入类型
}MB_REG_INDEX;

typedef struct 
{
	s8 start_index;
	s8 end_index;
}MB_QUICK_INDEX;

typedef struct
{
    MB_REG_INDEX *pRegindex;
    u8 Count; 
    U8 mb_num;//当前主机编号
}MB_REG_INDEX_TABLE;

//master队列应用管理结构体
typedef struct
{
	LINK_QUE link_que_set[NML_QUE_SIZE];
	LINK_COMM_PARA link_comm_para_set;						//主机通讯参数
	LINK_ERROR_TYPE link_error_type;						//link错误类型
	MB_REG_INDEX_TABLE  *pMbTable;
	MB_QUICK_INDEX *pQuickIndex;
	LINK_INJ_DATA *pInjDataBuf;
	u16 wr_que_addr,										//当前注入队列地址
		rd_que_addr,										//当前读出队列地址
		que_len;											//当前注入/循环队列实际大小
	bool app_run_sta;										//0:查询队列请求;1:有添加link队列存在
	u8 link_slv_addr_index,									//当前连接从机的地址索引
		SlvAddrList[Master_SLV_MAX_NUM],					//从机地址清单
		Slv_num,											//从机数量
		mb_status,											//当前主机状态
		once_flag;
	LINK_QUE_TYPE linkquetype_step;
	u16 scomm_err_cnt;
}QUE_MANGER_APP;

//回调函数指针
typedef void (*mb_timout_process_t)(QUE_MANGER_APP* pp, u8 id);
typedef void (*mb_ScommOK_process_t)(QUE_MANGER_APP* pp, u8 id);
typedef void (*mb_ScommErr_process_t)(QUE_MANGER_APP* pp, u8 id);

//mb主机管理结构体
typedef struct
{
	u8  mb_step,
		mb_turn,											//主机轮询有效标志
	    mb_nb,												//当前主机运行数量
		mb_run_id,
	    mb_flag;
	u32 mb_PowerUp_cnt;
	QUE_MANGER_APP link_que[USARTMAXCNT];
	mb_timout_process_t t_mb_timout_process;
	mb_ScommOK_process_t t_mb_ScommOK_process;
	mb_ScommErr_process_t t_mb_ScommErr_process;		
}MB_MANGER_APP;

MBLINK_EXT MB_MANGER_APP mb_manger_app;
MBLINK_EXT char MBLink_MASTER_NUM;							//主站接口数量

//外部方法
MBLINK_EXT	void MasterTest(ST485_USART *p);
MBLINK_EXT	void Tsk_MB(MB_MANGER_APP *pa);

#endif

//***********************************END OF FILE***********************************

