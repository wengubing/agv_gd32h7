/**********************************************************************************************************
*
*	模块名称 : EEPROM参数获取应用层
*	文件名称 : drv_ee_para.c
*	版    本 : V1.0
*	说    明 : 实现EEPROM的数据存储管理操作

2015.06.008:新建.By dengyang.



**********************************************************************************************************/



#define		EEPARA_GLOBALS


#include "includes.h"
#include "drv_ee_para.h"


PARA_Type	g_CurrentParaSources = NONE_PARA;

u32 EE_powerDownCheckCnt;


//要保存在板上的参数结构体
typedef union 
{
    u16		List[EEPARA_U16NUM];
    u8		Array[EEPARA_U16NUM*2];
	EEP_ParaInf_TypeDef ParaInf;
}BoardParameter_Type;


//============================================================================
// 名称： para_load_default
// 功能： 加载默认参数
// 参数： 无
// 返回： 无
// 说明： 无
//============================================================================
void para_load_default(void)
{
		//u32 temp=0;
		//temp = ((2016 << SN_YEAR_MASK) + (6 << SN_MONTH_MASK) + (3 << SN_DATE_MASK) + (01 << SN_ID_MASK));
		//g_pBoardpara[Para_Index_ProductSerial] = temp;
		//g_pBoardpara[Para_Index_ProductSerial+1] = temp>>16;
}

//============================================================================
// 名称： para_loading
// 功能： 加载保存EEPROM中的参数
// 参数： paratype:参数类型输入
// 返回： 可以组合的下面结果
//               0：成功
//               1：板参数错误
//               2：备份板参数错误
// 说明： 无
//============================================================================
u8 para_loading(PARA_Type paratype)
{
	if(paratype==USER_PARA || paratype==FACT_PARA)
	{
		u16 Crc16Value;
		u32 StartAddress, len;	
		BoardParameter_Type l_Boardpara;// 读出的参数，不字节赋值，防止破坏原有数据	
		
		len=CountOf(l_Boardpara.Array);//字节数	
		
		if(paratype==USER_PARA)
		{
			StartAddress=PARA_ADDRESS_USER;
		}
		else if(paratype==FACT_PARA)
		{
			StartAddress=PARA_ADDRESS_FACT;
		}
		
		// 读参数
		EEPROM_ReadMuliByte(l_Boardpara.Array,  StartAddress, len);

		// 检查参数
		Crc16Value = modbus_crc( l_Boardpara.Array, len - 2 );
		//if ( Crc16Value != l_Boardpara.List[Para_Index_CRCSum] )
		if ( Crc16Value != l_Boardpara.ParaInf.CRCSum )
		{
				return ( EE_ErrorParameter );
		}
		
		// 参数赋值
		memcpy(g_pBoardpara, l_Boardpara.Array, len);
	}
	else if(paratype==DEFAULT_PARA)
	{
		para_load_default();
	}

	return ( EE_OK );
}

//============================================================================
// 名称： para_save
// 功能： 将参数保存到EEPROM指定区域
// 参数： paratype:参数类型输入
// 返回： 可以组合的下面结果
//               0：成功
//							 1：失败
// 说明： 无
//============================================================================
u8 para_save(PARA_Type paratype)
{
	//u8 buffer[5];
	u16 Crc16Value1, Crc16Value2;
	u32 StartAddress, len;
	//u32 temp;
	BoardParameter_Type *pBoardpara;
	
	len=CountOf(pBoardpara->Array);//字节数
	
	pBoardpara = (BoardParameter_Type *)g_pBoardpara;//获取DHR上中EERPOM区域的起始地址

	if(paratype==USER_PARA)
	{
		StartAddress=PARA_ADDRESS_USER;
	}
	else if(paratype==FACT_PARA)
	{
		StartAddress=PARA_ADDRESS_FACT;
	}
	else if(paratype==DEFAULT_PARA)
	{
		StartAddress=PARA_ADDRESS_DEFAULT;
	}
	
	//存储次数加1
	//pBoardpara->List[Para_Index_EE_Write_CNT]	+= 1;
	pBoardpara->ParaInf.WriteCnt++;

////
	/*获取当前时间值,写入记录当前存储时间
	buffer[0] = DHR[R_STM32_RTC_DATA] % 20;//15
	buffer[1] = DHR[R_STM32_RTC_DATA+1];
	buffer[2] = DHR[R_STM32_RTC_DATA+2];
	buffer[3] = DHR[R_STM32_RTC_DATA+4];
	buffer[4] = DHR[R_STM32_RTC_DATA+5];*/
	//temp = buffer[0]*100000000+buffer[1]*1000000+buffer[2]*10000+buffer[3]*100+buffer[4];
	//temp=1506100850;	
	//pBoardpara->List[Para_Index_EE_Write_TIMES]	= temp;
	//pBoardpara->List[Para_Index_EE_Write_TIMES+1] = temp>>16;

	// 计算crc1
	//pBoardpara->List[Para_Index_CRCSum] = Crc16Value1 = modbus_crc( pBoardpara->Array, len - 2 );
	pBoardpara->ParaInf.CRCSum=Crc16Value1 = modbus_crc( pBoardpara->Array, len - 2 );
	
	//写入EEPROM
	if(EEPROM_WriteMuliByte(pBoardpara->Array, StartAddress, len)!=0)
	{
			return EE_ErrorProgramBlock;
	}
	else
	{//写入过程无异常...
		BoardParameter_Type l_Boardpara;
		
		//给EEPROM断电操作
		EE_powerDownCheckCnt = EE_1ms_cnt;
		for(;;)
		{//阻塞延迟!!!
			if(Delay_Fuction(EE_1ms_cnt, EE_powerDownCheckCnt, EEPROM_POWER_WODWN_TIMES))
				break;
		}
		//给EEPROM上电操作

		// 立即读取刚才写入EEPROM数据
		EEPROM_ReadMuliByte(l_Boardpara.Array,  StartAddress, len);

		// 计算crc2
		Crc16Value2 = modbus_crc( l_Boardpara.Array, len - 2 );
		
		if(Crc16Value1 != Crc16Value2)
		{
				return EE_ErrorProgramBlock;
		}
	}
	
	return EE_OK;
}

//u32 buff1[60];
//u32 buff2[60];
/**********************************************************************************************************
*	函 数 名: GET_EE_INFO
*	功能说明: 获取EEPROM信息
*	形    参: NONE
*	返 回 值: NONE
**********************************************************************************************************/
u8  GET_EE_INFO(void)
{
	EE_IN_AddrMax = EE_MemorySize - 1; //最大寻址地址
	if(EEPROM_RDID() != EE_25LC1024_ID)
		return 1;

	return 0;
}

/**********************************************************************************************************
*	函 数 名: TSK_EE_PARA
*	功能说明: EEPROM参数存储及相关管理任务
*	形    参: cmd:命令输入
*	返 回 值: res
1.此任务函数不需要循环调用;
**********************************************************************************************************/
u8 TSK_EE_PARA(EE_CMD cmd)
{
	switch (cmd)
	{
		case EE_CMD_INI:
		{
			//g_pBoardpara = &DHR[R_EEPROM_PARA_START_ADDR];//获取参数区域的全局指针地址
			if(GET_EE_INFO())
				return 1;
		}
		break;
		case EE_CMD_PWR_UP_GET_DATA:
		{
			if ( EE_OK == para_loading(USER_PARA) )// 读用户参数,不正确则调用出厂参数
			{
					g_CurrentParaSources = USER_PARA;
			}
			else
			{
					if ( EE_OK == para_loading(FACT_PARA) )// 读用出厂参数,不正确则调用默认参数
					{
							g_CurrentParaSources = FACT_PARA;
					}
					else
					{
							para_loading(DEFAULT_PARA);
							g_CurrentParaSources = DEFAULT_PARA;
					}
			}

		}
		break;
		case EE_CMD_SAVE_USER_PARA:
		{
			para_save(USER_PARA);
		}
		break;
		case EE_CMD_SAVE_FACT_PARA:
		{
			para_save(FACT_PARA);
		}
		break;
		case EE_CMD_SAVE_DEFAULT_PARA:
		{
			para_load_default();
			para_save(DEFAULT_PARA);
		}
		break;
		case EE_CMD_LOADING_DEFAULT_PARA:
		{
			para_load_default();
		}
		break;
		default:
		break;
	}
		
	return 0;
}


/************************************END OF FILE*****************************************/
