/**********************************************************************************************************
*
*	模块名称 : EEPROM底层驱动
*	文件名称 : spi_eerpom.c
*	版    本 : V1.0
*	说    明 : 实现EEPROM的批量读、写、EARSE等操作

2015.06.02:新建.By dengyang.



**********************************************************************************************************/


/* Includes ------------------------------------------------------------------*/

#define		EEPROM_GLOBALS

/* Includes ------------------------------------------------------------------*/
#include "..\..\Drivers\spi_eeprom\spi_eeprom.h"



/* Private typedef -----------------------------------------------------------*/

//突发写多个字节数据Arraylist
typedef struct
{
	u32 PageStartAddr;//页写入地址
	u16	PageLenSize;//页写入长度
}BurstArrayList;



/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
u32 ee_timesout_cnt;



/* Private functions ---------------------------------------------------------*/

/**********************************************************************************************************
*	函 数 名: EE_SET_WEL_Status
*	功能说明: WEL写使能锁存位状态设置
*	形    参: 0:禁止;1:使能;
*	返 回 值: NONE
备注:
1.要在EE内完成任何写操作,必须先置1此锁存器.
2.
**********************************************************************************************************/
void EE_SET_WEL_Status(u8 sel)
{
	u8 Temp;
	
	if(sel)
	{//使能写入EE阵列,WREN指令将写使能锁存器1
		Temp=EE_I_WREN;
	}
	else
	{//禁止写入EE阵列,WRDI指令置1(将锁存器复位)
		Temp=EE_I_WRDI;
	}
	
	GPIO_CTRL(EE_GPIO_CS, RESET);//拉低,启动写周期
	SpiWriteSingleDS(SPIB, Temp);
	GPIO_CTRL(EE_GPIO_CS, SET);	//拉高,(如果sel==1,将写使能锁存器置1)
}

/**********************************************************************************************************
*	函 数 名: EE_HardProtectSel
*	功能说明: 使能或禁止EE的硬件保护功能
*	形    参: 0:禁止;1:使能;
*	返 回 值: NONE
备注:
当EE被硬件写保护时,仅禁止对STATUS寄存器中的非易失位进行写操作.
**********************************************************************************************************/
void EE_HardProtectSel(u8 sel)
{
	if(sel)
	{//使能
		GPIO_CTRL(EE_GPIO_WP, RESET);//WP拉低

	}
	else
	{//禁止
		GPIO_CTRL(EE_GPIO_WP, SET);//WP拉高,解除硬件写保护
	}
}

/**********************************************************************************************************
*	函 数 名: EEPROM_RDSR
*	功能说明: 获取EE的STATUS寄存器信息
*	形    参: NONE
*	返 回 值: res
//备注:可随时读取状态,即使写周期也不例外.
**********************************************************************************************************/
u8 EEPROM_RDSR(void)
{
	u8 STATUS=0x00;
	
	GPIO_CTRL(EE_GPIO_CS, RESET);//拉低,启动写周期
	SpiWriteSingleDS(SPIB, EE_I_RDSR);
	STATUS = SpiReadSingleDS(SPIB);	
	GPIO_CTRL(EE_GPIO_CS, SET);	//拉高,终止RDSR指令序列	

	return STATUS;
}

/**********************************************************************************************************
*	函 数 名: EEPROM_ReadStatusInfo
*	功能说明: 获取指定相应位的状态
*	形    参: NONE
*	返 回 值: res
**********************************************************************************************************/
u8 EEPROM_ReadStatusInfo(STATUS_BIT cfgbit)
{
	u8 STATUS=0x00,res=0x00;

	STATUS = EEPROM_RDSR();	

	if(cfgbit==WIP_R)
	{
		if(Get_bit(STATUS, WIP_R))
			res=1;//正在进行写操作
		else
			res=0;//未进行写操作
	}
	else if(cfgbit==WEL_R)
	{
		if(Get_bit(STATUS, WIP_R))
			res=1;//允许写入EE阵列
		else
			res=0;//禁止写入EE阵列
	}
	else if(cfgbit==BPx_WR)
	{
		res = (STATUS>>BPx_WR) & 0x03;//获取BP0、BP1组合状态值
	}
	else if(cfgbit==WPEN_WR)
	{
		if(Get_bit(STATUS, WPEN_WR))
			res=1;//EE写使能
		else
			res=0;//EE写禁止
	}

	return res;
}

/**********************************************************************************************************
*	函 数 名: EEPROM_WRSR
*	功能说明: 设置EE的STATUS状态寄存器指令值设置
*	形    参: 
						level:BP保护级别设置值;
						wpen:写保护使能状态值,0:硬件写保护解除;1:硬件写保护使能
*	返 回 值: res
//备注:
1.STATUS值是非易失性的;
2.BP0、BP1两个位可写;
3.在对STATUS中的非易失位写操作时,需线解除EE的硬件写保护功能;
**********************************************************************************************************/
u8 EEPROM_Write_WRSR(BP_LEVEL level, u8 wpen)
{
	u8 res=0, temp=level, STATUS;
	
	if(wpen)
	{
		temp |= 0x80;
	}
	else
	{
		temp &= 0x7F;
	}
	
	GPIO_CTRL(EE_GPIO_CS, RESET);//拉低,启动写周期
	SpiWriteSingleDS(SPIB, EE_I_WRSR);
	SpiWriteSingleDS(SPIB, temp);
	GPIO_CTRL(EE_GPIO_CS, SET);	//拉高,终止RDID指令序列	
	
	STATUS = EEPROM_RDSR();//重新获取status
	
	if(STATUS!=temp)
	{
		res=1;
	}

	return res;
}
/**********************************************************************************************************
*	函 数 名: EEPROM_PageEarse
*	功能说明: EE指定页擦除
*	形    参: PageAddr:页地址
*	返 回 值: 0:成功;1:失败;
备注：
1.如果BP0、BP1保护位不为0,EE的页擦除功能将忽略;
**********************************************************************************************************/
u8 EEPROM_PageEarse(u8 PageAddr)
{
	u8 i,data,res=0;
	u32 timesout=0;
	
	if(PageAddr>EE_PageEndAddr)
		res=1;//页地址异常
	else
	{
		if(EEPROM_ReadStatusInfo(BPx_WR)!=0)
		{//有受保护的扇区块
			/*需先写WRSR,设置EE没有受保护的块,由于,有受保护的块时,使用CE指令会被忽略*/
			EEPROM_Write_WRSR(BP_NONE, 0);
		}

		EE_SET_WEL_Status(1);//发WREN指令

		GPIO_CTRL(EE_GPIO_CS, RESET);	
		for(i=0;i<4;i++)
		{
			if(i==0)
			{
				data = EE_I_PE;	//发1条芯片擦除(CE)命令
			}
			else
			{
				data = PageAddr;
			}
			SpiWriteSingleDS(SPIB, data);	
		}
		GPIO_CTRL(EE_GPIO_CS, SET);	//拉高,启动EE自定时擦除功能
		
		/*通过判断WIP位状态,确认是否擦除完成*/
		while(EEPROM_ReadStatusInfo(WIP_R))
		{
			if(++timesout>=0xFFF)
			{
				res=2;
				break;
			}
		}	
	}

	return res;
}

/**********************************************************************************************************
*	函 数 名: EEPROM_ChipEarse
*	功能说明: EE全片擦除
*	形    参: NONE
*	返 回 值: 0:成功;1:失败;
备注：
1.如果BP0、BP1保护位不为0,EE的芯片的擦除功能将忽略;
2.若要擦除保护的扇区,
**********************************************************************************************************/
u8 EEPROM_ChipEarse(void)
{
	u8 res=0;
	
	if(EEPROM_ReadStatusInfo(BPx_WR)!=0)
	{//有受保护的扇区块
		/*需先写WRSR,设置EE没有受保护的块,由于,有受保护的块时,使用CE指令会被忽略*/
		EEPROM_Write_WRSR(BP_NONE, 0);
	}
	
	GPIO_CTRL(EE_GPIO_CS, RESET);
	SpiWriteSingleDS(SPIB, EE_I_WREN);	//先发1条写使能(WREN)命令
	GPIO_CTRL(EE_GPIO_CS, SET);	//拉高,将写使能锁存WEL寄存器置1
	
	GPIO_CTRL(EE_GPIO_CS, RESET);	
	SpiWriteSingleDS(SPIB, EE_I_CE);	//发1条芯片擦除(CE)命令
	GPIO_CTRL(EE_GPIO_CS, SET);	//拉高,启动EE自定时擦除功能
	
	ee_timesout_cnt = EE_1ms_cnt;

	/*通过判断WIP位状态,确认是否擦除完成*/
	while(EEPROM_ReadStatusInfo(WIP_R))
	{
		if(Delay_Fuction(EE_1ms_cnt, ee_timesout_cnt, EE_TSE))
		{
			res=1;
			break;		
		}
	}
	
	return res;
}

/**********************************************************************************************************
*	函 数 名: EEPROM_RDID
*	功能说明: EE读取EEPROM的RDID操作
*	形    参: NONE
*	返 回 值: data: 返回序列数据
**********************************************************************************************************/
u16 EEPROM_RDID(void)
{
	u8 i;
	u16 data;

	GPIO_CTRL(EE_GPIO_CS, RESET);
	for(i=0;i<4;i++)
	{
		if(i==0)
		{
			data = EE_I_RDID;
		}
		else
		{
			data = 0x5F;//A23-A0无效寻址地址,连续发3个0x5F---0x5F5F5F(需大于0x1FFF)
		}
		SpiWriteSingleDS(SPIB, data);	
	}
	data = SpiReadSingleDS(SPIB);	
	GPIO_CTRL(EE_GPIO_CS, SET);	//拉高,终止RDID指令序列
	
	return data;
}
/**********************************************************************************************************
*	函 数 名: EEPROM_PageWrite
*	功能说明: EE页写
*	形    参: 
						buffer:指向的数据区首指针;
						StartAddr:写入1页的起始地址;
						Len:长度(有限制,所有写的byte数据内容必须在1页之内)(0-EE_PageSize)
*	返 回 值: 0:成功;1:失败;
1.写序列包括一个自动自定时的擦除周期,写前无需擦除EE;
2.注意页边界处理;
3.需使用算法阻止试图跨越页边界的页写操作;
4.如跨越边界,会导致数据跳回到当前页的开始处(覆盖之前写入的数据);
5.写操作期间不能读EE阵列(可读取STATUS寄存器);
6.应当在写操作周期结束后,继续啊下一次页写操作(这点非常重要);---最大可以参照:EE_TWC
7.WEIP位只在写操作过程中置1;写操作完成后,WIP/WEL位会被清零;
8.指定的写入地址和长度的数据必须在一页之内;
9.
**********************************************************************************************************/
u8 EEPROM_PageWrite(u8 *buffer, u32 StartAddr, u16 Len)
{
	u8 res=0;
	
	if(buffer==0 || Len==0 || Len>EE_PageSize)
	{
		res=1;
	}
	else
	{
		u8 data;
		u16 i;
		BYTE4_UNION BU4;

		BU4.B4_U32 = StartAddr & EE_IN_AddrMax;
		
		EE_SET_WEL_Status(1);//发WREN指令

		GPIO_CTRL(EE_GPIO_CS, RESET);	//拉低,开始发送
		for(i=0;i<4;i++)
		{
			if(i>=1)
			{
				data = BU4.B4_U8[i-1];//发要写入的24bit地址
			}
			else
			{
				data = EE_I_WRITE;	//先发1条写使能(WRITE)命令
			}
			SpiWriteSingleDS(SPIB, data);
		}
		/*将既定的数据写入页面缓冲*/
		for(i=0;i<Len;i++)
		{
			SpiWriteSingleDS(SPIB, buffer[i]);
		}
		GPIO_CTRL(EE_GPIO_CS, SET);	//拉高,启动内部定时的写周期(将页缓存数据指定的EE阵列)
		
		ee_timesout_cnt = EE_1ms_cnt;
		
		/*通过判断WIP和WEL位状态,确认写操作是否完成*/
		while(EEPROM_ReadStatusInfo(WIP_R)==1)
		{//等待写周期结束
			if(Delay_Fuction(EE_1ms_cnt, ee_timesout_cnt, EE_TSE))
			{
				res=2;
				break;
			}
		}
	}
	
	return res;
}

/**********************************************************************************************************
*	函 数 名: EEPROM_WriteMuliByte
*	功能说明: EE写多个字节操作
*	形    参: 
						buffer:指向写的数据区首指针;
						StartAddr:向EE写的起始物理地址(0-EE_MemorySize);
						Len:长度(0-EE_MemorySize)
*	返 回 值: 0:成功;1:失败;
1.写前无需擦除EE;
2.本函数实现突发写多个字节数据;
3.实现自动换页及边界处理;
4.
**********************************************************************************************************/
u8 EEPROM_WriteMuliByte(u8 *buffer, u32 StartAddr, u32 Len)
{
	u8 res=0;

	if(buffer==0 || Len==0 || StartAddr>=EE_MemorySize || Len>(EE_MemorySize - StartAddr))
	{
		res=1;
	}
	else
	{
		BurstArrayList		WritePageArrayList;	
		u8								Start_flag	= StartAddr%EE_PageSize,//起始地址是否在页边界处(0:是;>=1:否)
											End_flag		= (StartAddr+Len)%EE_PageSize;//结束地址是否在页边界处(0:是;>=1:否)
		u16 							i,
											array_column,//所在矩阵页的列地址
											page_origin_num,//起始物理页编号:0、1、2.....EE_PageNum-1
											page_end_num,//结束物理页编号:0、1、2.....EE_PageNum-1
											page_num;//所占页的总数(包括满页操作和非满页操作)

		/*批量数据,在EE阵列的list映射分解*/
		page_origin_num = StartAddr / EE_PageSize;		
		if((Start_flag==0) && End_flag==0)
		{//写的数据为整数倍页操作
			page_end_num = ((StartAddr + Len) / EE_PageSize) - 1;
		}
		else if(End_flag)
		{//非整数倍页操作
			page_end_num = ((StartAddr + Len) / EE_PageSize);
		}

		page_num = page_end_num - page_origin_num + 1;//计算当前需要操作页的总数	

		if(page_num)
		{
			/*连续页写入操作*/
			for(i=page_origin_num;i<=page_end_num;i++)
			{
				if(page_num==1)
				{//总页数=1页
					WritePageArrayList.PageLenSize=Len;
					WritePageArrayList.PageStartAddr=StartAddr;
				}
				else
				{//总页数＞1页
					if(i<=page_origin_num && Start_flag)
					{//当前所在首页的起地址不在边界
						array_column = StartAddr%EE_PageSize;
						WritePageArrayList.PageLenSize=EE_PageSize-array_column;
						WritePageArrayList.PageStartAddr=StartAddr;
					}
					else if(i>=page_end_num && End_flag)
					{//当前所在未页的始地址不在边界上
						array_column = (StartAddr + Len)%EE_PageSize;
						WritePageArrayList.PageLenSize=array_column;
						WritePageArrayList.PageStartAddr=page_end_num*EE_PageSize;
					}				
					else if(i>=page_origin_num && i<=page_end_num)
					{//当前所在页的起地址在边界处
						WritePageArrayList.PageLenSize=EE_PageSize;
						WritePageArrayList.PageStartAddr=i*EE_PageSize;				
					}				
				}

				EEPROM_PageWrite(buffer, WritePageArrayList.PageStartAddr, WritePageArrayList.PageLenSize);
				buffer += WritePageArrayList.PageLenSize;
			}
		}
		else
		{
			res=2;
		}
	}

	return res;
}

/**********************************************************************************************************
*	函 数 名: EEPROM_ReadMuliByte
*	功能说明: EE读取多个字节操作
*	形    参: 
						buffer:指向读取存放的数据区首指针;
						 :读取的起始地址;
						Len:长度(Len<=EE_MemorySize)
*	返 回 值: 0:成功;1:失败;
**********************************************************************************************************/
u8 EEPROM_ReadMuliByte(u8 *buffer, u32 StartAddr, u32 Len)
{
	u8 res=0, data=0x00;
	u32 i;
	BYTE4_UNION BU4;

	if(buffer==0 || Len==0 || StartAddr>=EE_MemorySize || Len>(EE_MemorySize - StartAddr))
	{
		res=1;
	}
	else
	{
		BU4.B4_U32 = StartAddr & EE_IN_AddrMax;
		GPIO_CTRL(EE_GPIO_CS, RESET);	//选择EE的片选
		for(i=0;i<4;i++)
		{
			if(i>=1)
			{
				data = BU4.B4_U8[i-1];//写入要读取的24bit起始地址
			}
			else
			{
				data = EE_I_READ;	//先发1条写使能(READ)命令
			}
			SpiWriteSingleDS(SPIB, data);
		}
		
		/*连续读取Len个byte的数据*/
		for(i=0;i<Len;i++)
		{
			buffer[i] = SpiReadSingleDS(SPIB);
		}
		GPIO_CTRL(EE_GPIO_CS, SET);	//拉高,将写使能锁存WEL寄存器置1	
	}

	return res;
}


/************************************END OF FILE*****************************************/
