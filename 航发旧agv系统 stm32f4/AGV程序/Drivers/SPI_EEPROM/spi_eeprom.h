#ifndef _SPI_EEPROM_H_	
#define _SPI_EEPROM_H_


#ifdef		EEPROM_GLOBALS
	#define EEPROM_EXT
#else
	#define	EEPROM_EXT  extern
#endif

	
//用户包含的头文件，可以修改
#include "includes.h"//常用库及ST库配置
	#include "..\..\Drivers\inc\drv_ee_para.h"
#include "..\..\Drivers\inc\drv_spi.h"
#include "..\..\Drivers\ModbusRtuPoll\ModbusRtuPoll.h"
	

	
/*
 存储器示意图:
	page_num	  R/W_addr
 ___________  0 	(页起始地址)
|						|	
|____0页____| 256	(页结束地址)
|						|	
|____1页____| 512
|						|	
|____2页____| 768
|						|	
|____3页____| 1024
|						|	
|____4页____| 1280
|						|	
|____x页____| ...
|						|	
|____511页__| 130816
|						|	
|___________| 131072

1.本EEPROM驱动用于存储相关重要的掉电保存数据;
2.实际使用时候,存储的有效数据最大只会用到1/2的存储量;
3.EE整列数据最小修改单位为页区域修改,不可以用字节方式修改;
4.当被写入的目标数据不足刚好整数倍页的时候,程序自动用0xFF自动填充未被使用的剩余页区域;
	
*/
	
	
	
	
/* Private define ------------------------------------------------------------*/
/*eeprom信息参数描述定义*/
#define EE_MemoryArray							1024			//存储容量,unit:Kbit
	
#define EE_MemorySize								((EE_MemoryArray*1024)/8)			//存储容量:131072===0x20000,unit:Byte
	
#define	EE_PageBufferSize						256				//EEPROM页面缓冲区打下,unit:Byte
	
#define	EE_PageSize									256				//EEPROM页大小,unit:Byte
#define	EE_PageNum									(EE_MemorySize/EE_PageSize)				//EEPROM页数量,e.g.:512
#define	EE_PageEndAddr							(EE_MemorySize-EE_PageSize)				//EEPROM末页地址
	
#define	EE_SectorSize								32				//EEPROM扇区大小,unit:KByte
#define	EE_SectorNum								(EE_MemorySize/1024/EE_SectorSize)		//EEPROM扇区数量,e.g.:4

//#define	EE_BlockSize								32				//EEPROM块大小,unit:KByte
//#define	EE_BlockNum									1					//EEPROM块数量


#define	EE_TWC											5					//EEPROM内部最大写周期时间值(字节或页模式和页擦除),unit:ms
#define	EE_TCE											15				//芯片擦除周期时间
#define	EE_TSE											15				//扇区擦除周期时间
#define	EE_TWCE											1000000		//WriteCycleEndurance耐擦写周期
#define EE_1ms_cnt									g_Sys_1ms_Counter
#define	EE_25LC1024_ID							0x29

/*eeprom指令集INSTRUCTION*/
#define	EE_I_READ 		0x03	//0000 0011 //从所选地址开始从存储器阵列读数据
#define	EE_I_WRITE 		0x02	//0000 0010 //从所选地址开始向存储器阵列写数据
#define	EE_I_WREN 		0x06	//0000 0110 //置1 写使能锁存器（使能写操作）
#define	EE_I_WRDI 		0x04	//0000 0100 //复位写使能锁存器（禁止写操作）
#define	EE_I_RDSR			0x05	//0000 0101 //读STATUS 寄存器
#define	EE_I_WRSR			0x01	//0000 0001 //写STATUS 寄存器
//以下指令仅限于:25XX512 和25XX10244 系列
#define	EE_I_PE 			0x42	//0100 0010 //页擦除——擦除存储器阵列的一页
#define	EE_I_SE 			0xD8	//1101 1000 //扇区擦除——擦除存储器阵列的一个扇区
#define	EE_I_CE				0xC7	//1100 0111 //芯片擦除——擦除存储器阵列的所有扇区
#define	EE_I_RDID 		0xAB	//1010 1011 //退出深度掉电,并读电子签名
#define	EE_I_DPD 			0xB9	//1011 1001 //深度掉电模式







/* Private typedef -----------------------------------------------------------*/

//EEPROM 中的写模式定义
typedef	enum
{
	Byte_Mode = 0,	//字节模式
	Page_Mode,			//页写模式(推荐方式,能得到最大的耐擦写能力)
	Block_Mode,			//块写模式
}EEPROM_WRITE_MODE;

//STATUS位信息定义
typedef enum
{
	WIP_R = 0,		//写进行位
	WEL_R,				//写使能锁存位
	BPx_WR,				//非易失位,快保护位
	WPEN_WR = 7,	//非易失位,写保护使能位
}STATUS_BIT;

//阵列保护级别定义
typedef enum
{
	BP_NONE=0x00,	//受保护:无;			未受保护:0/1/2/3
	BP_1p4=0x04,	//受保护:3;				未受保护:0/1/2
	BP_1p2=0x08,	//受保护:2/3;			未受保护:0/1
	BP_1p1=0x0C,	//受保护:0/1/2/3;	未受保护:无
}BP_LEVEL;


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

EEPROM_EXT	u32 EE_IN_AddrMax;
	
	
	
	
	
/* Function -------------------------------------------------------------------*/

EEPROM_EXT	u8 EEPROM_ReadMuliByte(u8 *buffer, u32 StartAddr, u32 Len);
EEPROM_EXT	u8 EEPROM_WriteMuliByte(u8 *buffer, u32 StartAddr, u32 Len);
EEPROM_EXT	u16 EEPROM_RDID(void);
EEPROM_EXT	u8 EEPROM_ChipEarse(void);
	
#endif

/************************************END OF FILE*****************************************/
