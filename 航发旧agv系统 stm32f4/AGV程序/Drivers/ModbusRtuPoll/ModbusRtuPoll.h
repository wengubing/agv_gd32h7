#ifndef _BSP_MODBUS_H_ 
#define _BSP_MODBUS_H_

/****************************Copyright (c)**********************************************                                       
文件名称：ModbusRtuPoll.c
描  述：modbus主/从协议文件
作  者：邓 洋
版  本：V1.0.0
日  期：2015年04月25日
修改记录：
v1.0.0  ->  新建之前的协议和link注入主机发送接收处理文件合并；


使用说明：
1.包含路径设置:#include "..\..\Drivers\ModbusRtuPoll\ModbusRtuPoll.h"(存放在此路径即可)
2.


***************************************************************************************/  


#ifdef  MB_GLOBALS
 #define MB_EXT
#else
 #define MB_EXT  extern
#endif

 
 
/***************************************************用户需要(非必须)修改部分1***************************************************/
 
//用户包含的头文件，可以修改
#include "includes.h"//常用库及ST库配置
#include "..\..\Drivers\ModbusRtuPoll\drv_485Usart.h"//st_usart驱动

#include "..\..\Drivers\inc\drv_timer.h"//定时器配置
#include "..\..\Drivers\inc\drv_led.h"//led配置
////#include "DHR_Defines.h"
 
 
#define   Dylinkcomm_1ms_cnt      g_Sys_1ms_Counter  //只需要将后面的宏定义用外部的标准1ms计数器值代替即可

#define   Master_HardInterface     RS485_USARTx[R485_MASTER_ID];//定义作为主机的USARTx通道编号

#define   SLV_RECIVE_LED        b_led2
#define   LINK_COMM_LED         b_led3 //link成功20次闪烁翻转







/***************************************************用户不需要(必须)修改部分***************************************************/

/* Private define ------------------------------------------------------------*/
 


/*  modbus通讯波特率计数值 */
/**-------------------------------------
1 起始位 
8 数据位,首先发送最低有效位
0 位作为奇偶校验
1 停止位 
T=3.5*( 1+数据位+奇偶校验+停止位) / 波特率
e.g.:t = 3.5*（1+8+1+1）/波特率
------------------------------------->
115200bps ---0.35ms - 350us   50x7
57600bps ---0.70ms - 700us   50x14
38400bps ---1ms  - 1000us  50x20
19200bps ---2ms  - 2000us  50x40       
9600bps  ---4ms  - 4000us  50x80
4800bps  ---8ms  - 8000us  50x160
2400bps  ---16ms  - 16000us  50x320
1200bps  ---32ms  - 32000us  50x640
--------------------------------------**/
#define   T35_RB115200_cnt       7
#define   T35_RB57600_cnt        14
#define   T35_RB38400_cnt        20
#define   T35_RB19200_cnt        40
#define   T35_RB9600_cnt        80
#define   T35_RB4800_cnt        160
#define   T35_RB2400_cnt        320
#define   T35_RB1200_cnt        640

#define   OP_REG            		1 //寄存器操作
#define   OP_COIL            		0 //线圈操作

#define   MODBUS_OT_RO         1
#define   MODBUS_OT_RW         0

//USART收发缓冲区大小
#define   MB_TRBUFFER_LENGTH		256 //缓冲区深度

#define   Const_Coil_ON         0xFF00
#define   Const_Coil_OFF        0x0000

#define		Slv_Num_Max						12

/* Private typedef -----------------------------------------------------------*/


//Usartx收/发定义
typedef enum{
   send_dat = 1,
   recv_dat
  }RS485USART_TxRx;

//Usartx工作模式定义
typedef enum
{
 RS485_Master,
 RS485_Slave,
}RS485USART_WORKMODE;


//主机link队列状态枚举
typedef enum
{
 STA_NULL = 0,
 STA_TX,
 STA_RX,
}Master_STA;

//MODBUS 常量结构体
typedef struct
{
 unsigned char   slv_adr1;  //从机地址
 unsigned short  hr_nb1;   //HR的数量
}ST_MODBUS_SLV_CONST;

//MODBUS 从模式写入命令记录结构体
typedef struct
{
 u8 func;
 u16 wr_adr;
 u16 wr_n;
}MODBUS_SLV_WR;
       
//功能码枚举
typedef enum
{
	MD_FUNC_NULL	= 0x00,
	MD_RD_COIL		= 0x01, //读线圈
	MD_RD_DISCRETE	= 0x02, //读离散输入状态
	MD_RD_HR		= 0x03, //读保持寄存器
	MD_RD_IR        = 0x04,	//读输入寄存器

	MD_FR_SCOIL		= 0x05, //强制单个线圈
	MD_FR_SHR		= 0x06, //强制单个寄存器
	MD_FR_MCOIL		= 0x0F, //强制多个线圈-15
	MD_FR_MHR		= 0x10, //强制多个寄存器 -16
	MD_FR_MHR_RDHR	= 0x17, //读写多个寄存器 -23
}MD_FUNC;

//modbus数据包格式检查判断内容
typedef enum
{
	MODBUS_FORMAT_OK=0,    //格式OK
	MODBUS_ERR_TIMEOUT,      //超时
	MODBUS_ERR_BYTELESS,     //字节数少
	MODBUS_ERR_CRC,      //CRC错
	MODBUS_ERR_SLV,     //从机地址错
	MODBUS_ERR_FUNC,//从机命令错
	MODBUS_ERR_BYTE,      //接收字节数错
	MODBUS_ERR_BYTECOUNT,      //ByteCount错
	MODBUS_ERR_ADR,      //地址错
	MODBUS_ERR_DAT,      //数据错
	MODBUS_ERR_N,     //字节数错 
}MODBUS_MASTER_ERR_INFO;

//modbus从机返回值宏定义
typedef enum
{
	MODBUS_SLV_NULL=0, //无操作
	MODBUS_SLV_OK, //操作正确
	MODBUS_SLV_FORMAT_OK, //格式正确,但不是自己的数据包
	MODBUS_SLV_FORMAT_ERR, //格式错误
	MODBUS_SLV_FUNC_ERR, //命令错误 
}MODBUS_SLV_INFO;

//主模式状态
typedef enum 
{
 MD_MST_STA_NULL = 0,
 MD_MST_STA_OK,    //发送并接收完成
 MD_MST_STA_READY,   //上一帧完成,可以启动发送
 MD_MST_STA_INPROCESS, //正在进程中
 MD_MST_STA_SEND_ERR, //发送错误
 MD_MST_STA_NO_REC,  //未接收到数据
 MD_MST_STA_REC_ERR,  //接收错误
 MD_MST_STA_TIMEOUT,  //7 接收超时
 MD_MST_STA_RETRYED,  //达到重发指定次数
 MD_MST_STA_RETx,  //在预定的重复发送次数内,重新启动发送下一帧
 MD_MST_STA_INVALID_FUNC, //无效指令码(此时,立即去执行下一条指令)
}MD_MST_STA;

//USART初始化结构体
typedef struct
{
 USART_ID    usartx;//usart当前的ID编号
 RS485USART_WORKMODE  workmode;//工作方式选择,见相关枚举
 unsigned long  baud;//当前设置的波特率值
 unsigned char  baud_set; //波特率设置编号
 unsigned short comm_cfg_par;//通讯配置参数,见宏定义
 unsigned char  pi;//中断优先级编号
 unsigned long   T35_val;//串口接收间隔计时数值
 unsigned long  T35_IT;//不同波特率数据包的最小间隔时间值
}RS485USART_Ini;

//Usart数据结构体(modbus协议和硬件对应的接口)
typedef struct
{
 unsigned long  base;    //指向通讯端口的寄存器基址
 unsigned long  RS485;   //RS485位地址
 unsigned long	Slave_timeout;
	
 unsigned char  sta;    //通讯状态
 unsigned char  adr;    //从机地址,用于提高多机通讯效率

 signed int   jfc;    //3.5t计数设置计数器

 RS485USART_Ini Ini_par; //初始化参数
 
 unsigned short rx_nb;  //接收尾指针(通讯读取的数据数量)
 unsigned short tx_snb;  //发送首指针
 unsigned short tx_nb;  //发送尾指针
 unsigned char  tr_buf[MB_TRBUFFER_LENGTH]; //通讯缓冲区

}ST485_USART;

//主模式命令结构体
typedef struct
{
 MD_MST_STA    mb_sta;    //主机当前状态
 unsigned short  *phr_read, //指向读取下位机对应的HR指针
         *pwr_write; //指向写入下位机对应的HR指针
 unsigned char  slv;    //从机地址
 MD_FUNC     func;  //命令
 unsigned short  da_adr, //操作的数据地址
         dat_num;  //操作的数据个数/数据内容,unit:byte
 unsigned short  wr_adr, //RW命令的写入地址
         wr_n;  //RW命令的字节数
 Master_STA   Master_link_Sta;
}ST_MODBUS_MASTER;


//modbus通讯协议类型
typedef enum
{
	MODBUS_NULL	=	0,
	MODBUS_RTU	=	1,
	MODBUS_ASCII=	2,
	MODBUS_TCP	=	4,
	//新增自定义协议栈类型
	Custom_UART = 101,
}ProtocolType;

//master通讯连接参数设置
typedef struct
{
	ST485_USART				*pST;		//硬件层指针
	ST_MODBUS_MASTER	mb_master;//mb主机通讯层
	vu32 	MasterSend_dealy_cnt,//master发送延迟计数器
				wait_recive_timeout_cnt,
				comm_timeout,  //通讯超时值(ms)
				wait_timeout_cnt,//等待超时计数器
				comm_Send_delay;//master发送延迟时间(link通讯延迟时间)
	vu16	comm_retry,   //master重发次数
				link_comm_ok_cnt, //通讯ok计数
				link_comm_err_cnt,//通讯错误计数
				master_send_cnt;//1次完整通讯过程中发送计数
				ProtocolType  Protocol_id;//通讯协议代码
	u8    TimeoutInterval_flag;
}LINK_COMM_PARA;

//master数据连接队列结构体
typedef struct
{
 u8  func, //要求的功能码
    slv_addr,//准备的从机地址id
    sel_coil_reg;//区分:线圈[0]和寄存器[1]
 u16  *phr_read,//读取从机操作返回数据存放的指针
    rw_slv_first_hr_addr,//单指令r/w地址及强制多个线圈
    rw_num,//reg:读/写数量,unit:字(word);coil:实际操作的线圈数量;
    *phr_write,//向从机写操作的数据源的指针     
    wr_slv_first_hr_addr,//写从机的起始hr地址(仅RW指令使用)
    write_num;//写数量,寄存器操作时(仅RW指令使用),unit:字(word)
}LINK_QUE;


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
/****************************外部使用变量**************************************/
MB_EXT ST_MODBUS_SLV_CONST    modbus_slv_const;
//MB_EXT ST_MODBUS_MASTER				modbus_master;
MB_EXT ST485_USART        		RS485_USARTx[USARTMAXCNT];//485底层数据端口x

/****************************相关接口函数**************************************/



//协议相关
MB_EXT unsigned short modbus_crc(void *puchMsg, unsigned short usDataLen);
MB_EXT void modbus_init(unsigned short hr_nb2, unsigned char slv_adr2);   
MB_EXT MODBUS_SLV_INFO task_modbus_rtu_slv(ST485_USART *p, unsigned short *phr, u16 option);
MB_EXT MD_MST_STA  link_ptl_modbus_rtu(LINK_COMM_PARA *plinkpara, LINK_QUE *plinkque);
MB_EXT unsigned char link_para_init(LINK_COMM_PARA *pLpara, u32 timeout, u32 send_dlay, u8 retry, ProtocolType prid);
MB_EXT MD_MST_STA master_send_ready(LINK_COMM_PARA *para, ST485_USART *st, ST_MODBUS_MASTER *psm);
MB_EXT void link_TimeoutProcess(LINK_COMM_PARA *plinkpara, ST485_USART *pSTu, ST_MODBUS_MASTER *pSTpm);

//tcp
MB_EXT MODBUS_SLV_INFO task_modbus_tcp_slv(ST485_USART *p, unsigned short *phr, u16 option);



MB_EXT unsigned short short_xch_hl(unsigned short i);
MB_EXT unsigned int uint_xch_hl(unsigned int i);

/***************************************************用户需要(非必须)修改部分2***************************************************/
//与硬件相关(需要user自己定义的函数，再将其包含即可)
MB_EXT u8 Rs485_usart_tm_50us(ST485_USART *su);//3.5T时间判断函数

extern void Rs485_Usart_InitHard(RS485USART_Ini *ui);//usart初始化
extern void Rs485_usart_tx_rx_rde(USART_ID Usartx, unsigned char on_off_sta);//rs485芯片收发状态控制
extern void Rs485_send_msg(ST485_USART *su, unsigned short nb1);//使用usart发送数据包消息




#endif

/************************************END OF FILE*****************************************/
