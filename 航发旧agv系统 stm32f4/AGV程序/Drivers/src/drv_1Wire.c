
#define			DRV_1WIRE_EXT_GLOBALS


/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "..\..\Drivers\inc\drv_1Wire.h"


/* Private typedef -----------------------------------------------------------*/

//config conversion mode enum.



/* Private define ------------------------------------------------------------*/

//ROM COMMANDS
#define		ROM_Search_Cmd   				0xF0
#define		ROM_Read_Cmd     				0x33			//读取ROM
#define		ROM_Match_Cmd    				0x55
#define		ROM_Skip_Cmd     				0xCC
#define		ROM_AlarmSearch_Cmd 		0xEC
//DS18b20 FUNCTION COMMANDS
#define		Convert_T          			0x44
#define		Write_Scratchpad				0x4E
#define		Read_Scratchpad					0xBE
#define		Copy_Scratchpad					0x48
#define		Recall_EEPROM						0xB8
#define		Read_PowerSupply				0xB4

/* Private variables ---------------------------------------------------------*/

GPIO_InitTypeDef GPIO_DQ;

u16 Con_T_Times[4] = {95, 188, 375, 750};//不同的分辨率的转换时间定义,unit:ms
u8  ConfigMode;
u32 g_ds18b20_counter;

/*******************************************************************************
函数名称：Crc8_Check
功    能：8-bit CRC check, X8+X5+X4+1, to check the data get from DS18B20
参    数：
					Crc_Org: Oringal crc value
					* Value: the data need do a crc check
					Length: The length of data
返回值  ：crc result
*********************************************************************************/
u8 Crc8_Check(u8 *Value, u8 Length)
{
	u8 i;
	u8 Crc_Org = 0;
	
	while (Length--!=0)
	{
		for (i = 1; i!= 0; i*=2)
		{
			if((Crc_Org & 0x01)!= 0)// LSB of crcorg
			{
				Crc_Org >>= 1;
				Crc_Org ^= 0x8C;
			}
			else
			{
				Crc_Org >>= 1;
			}
			if ( ((* Value) & i) != 0)
			{
				Crc_Org ^= 0x8C;
			}
     }
			Value ++;
	}
	return (Crc_Org);
}

/******************************************
函数名称：GPIO_DQ_Out_Mode
功    能：设置DQ引脚为开漏输出模式
参    数：无
返回值  ：无
*******************************************/
void GPIO_DQ_Out_Mode(void)
{
		GPIO_DQ.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_DQ.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(OneWireBus_DQ_GPIO_PORT, &GPIO_DQ);
}

/******************************************
函数名称：GPIO_DQ_Input_Mode
功    能：设置DQ引脚为浮空输入模式
参    数：无
返回值  ：无
*******************************************/
void GPIO_DQ_Input_Mode(void)
{
		GPIO_DQ.GPIO_Mode = GPIO_Mode_IN;
		GPIO_DQ.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(OneWireBus_DQ_GPIO_PORT, &GPIO_DQ);
}

/******************************************
函数名称：Write_OneByte_To1wire
功    能：写一个字节到1wire
参    数：无
返回值  ：无
*******************************************/
void Write_OneByte_To1wire(u8 data)
{
    u8 i ;
    GPIO_DQ_Out_Mode() ;
    for(i=0 ;i<8 ;i++)
    {
        if(data&0x01)    //低位在前
        {
            //写1
            DQ_Write_0(); //写时间空隙总是从总线的低电平开始
            Delay_1us(5);  //拉低5us
            DQ_Write_1();
            Delay_1us(80); //整个写1时隙不低于60us
        }
        else
        {
            //写0
            DQ_Write_0();
            Delay_1us(80); //保持在60us到120us之间
            DQ_Write_1();
            Delay_1us(5);
        }
        data >>= 1 ;
    }
}

/******************************************
函数名称：Read_OneByte_From1wire
功    能：从1wire读一个字节
参    数：无
返回值  ：读出的数据
*******************************************/
u8 Read_OneByte_From1wire(void)
{
    u8 i ,data = 0x00;
    
    for(i=0 ;i<8 ;i++)
    {
        GPIO_DQ_Out_Mode();
        data >>= 1 ;		//开始取最低位
        DQ_Write_0();		//拉低总线,产生读信号
        Delay_1us(1);
        DQ_Write_1();		//释放总线,准备读数据
        Delay_1us(1);			
        GPIO_DQ_Input_Mode();
        Delay_1us(1);
        if(DQ_ReadBit)
        {
					data |= 0x80;
        }
        Delay_1us(40);   //等待这一位数据完成传输
    }
    GPIO_DQ_Out_Mode();
		
    return data ;
}

/******************************************
函数名称：Bus_1wire_Init
功    能：初始化1wire
参    数：无
返回值  ：无
*******************************************/
u8 Bus_1wire_Init(void)
{
 	u8 res = 1;//初始化结果,默认失败
	u32 cnt=0;
	
 	do
 	{
 		/* 发送复位pulse */
    GPIO_DQ_Out_Mode() ;	//设置端口为输出模式
    DQ_Write_0();					//将DQ拉低
    Delay_1us(480); 			//至少保持480us
    DQ_Write_1();  				//释放总线(加速上升沿速度)
    Delay_1us(60);//60
 		/* 接收应答pulse */       
    GPIO_DQ_Input_Mode();		//设置端口为输入模式
 		if(!DQ_ReadBit)//判断是否有应答ack
 		{//有ack信号
 			res = 0;				//置初始化成功
 		}
		else
		{
			if(++cnt>1000)
				return 1;
		}
 	}while(res);//若初始化失败,则继续初始化
 	Delay_1us(120);//等待1wire将总线拉低60~240us,然后总线由上拉电阻拉高(释放总线)
 	GPIO_DQ_Out_Mode();//接收完成,主机重新控制总线
	
	return res;
}

/******************************************
函数名称：Write_18B20_EEPROM
功    能：写配18B20置参数
参    数：TH - 报警温度上限
          TL - 报警温度下限
          Register_Con - 控制寄存器的值
返回值  ：读出的数据
*******************************************/
void Write_18B20_EEPROM(u8 TH,u8 TL,u8 Register_Con )
{
    Bus_1wire_Init();
    Write_OneByte_To1wire(ROM_Skip_Cmd);
    Write_OneByte_To1wire(Write_Scratchpad);  
    Write_OneByte_To1wire(TH);									//TH=7F
    Write_OneByte_To1wire(TL);									//TL=FF 最高位符号位
    Write_OneByte_To1wire(Register_Con);				//xx位模式
    
    Delay_1us(700);
    Bus_1wire_Init();
    Write_OneByte_To1wire(ROM_Skip_Cmd);
    Write_OneByte_To1wire(Copy_Scratchpad);			//将寄存器的配置值写入EEPROM
    
    Delay_1us(300);
}

/************************************************************************************************
函数名称：Config_ds18b20
功    能：上电配置18b20
参    数：无
返回值  ：无
*************************************************************************************************/
u8 Config_ds18b20(DS18b20_conv_mode mode)
{
	u8 res=0;
	
	if(mode>Mode_12bit)
		res++;
	else
	{
		RCC_AHB1PeriphClockCmd(OneWireBus_DQ_GPIO_CLK, ENABLE);
		GPIO_DQ.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_DQ.GPIO_OType = GPIO_OType_PP;
		GPIO_DQ.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_DQ.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_DQ.GPIO_Pin = OneWireBus_DQ_GPIO_PIN;
		GPIO_Init(OneWireBus_DQ_GPIO_PORT, &GPIO_DQ);

		Write_18B20_EEPROM(0x7F, 0x99, (6<<mode)+0x1F);
		Bus_1wire_Init();
		Write_OneByte_To1wire(ROM_Skip_Cmd);//跳过读序列号操作		
		Write_OneByte_To1wire(0xB8);//回调设定值	
		
		ConfigMode=mode;
	}

	return res;
}

/************************************************************************************************
函数名称：Task_read_ds18b20_ROM
功    能：读取18b20_ROM任务
参    数：无
返回值  ：无
*************************************************************************************************/
u8 Read_ds18b20_ROM(void)
{
	u8	i,res=0,crc,rom_data[7];
	
#if	(Bus_1wire_Slave==0) 

		Bus_1wire_Init();//1-wire初始化
		Write_OneByte_To1wire(ROM_Read_Cmd);				//读取ROM	
		for(i = 0; i < 8; i ++)
		{
			if(i<=6)
			{
				rom_data[i] = Read_OneByte_From1wire();
			}
			else if(i==7)
			{
				crc = Read_OneByte_From1wire();
			}
		}
		
		/*crc计算*/
		if(crc==Crc8_Check(rom_data, sizeof(rom_data)))
		{
			memcpy(DS18B20_PAR.DS18b20_ROM_DAT,rom_data,sizeof(DS18B20_PAR.DS18b20_ROM_DAT));
		}
		else
		{
			res++;
		}

		
//  		Bus_1wire_Init();														//1-wire初始化	
// 		Write_OneByte_To1wire(ROM_Skip_Cmd);				//跳过读序列号操作	
//  		Write_OneByte_To1wire(Read_PowerSupply);		//读取供电模式状态 
//  		DS18B20_PAR.DS18b20_ROM_DAT[8] = Read_OneByte_From1wire();
		
#endif
		
		return res;
}

/************************************************************************************************
函数名称：Tsk_Read_ds18b20_MREG
功    能：读取REG
参    数：
返回值  ：无
*************************************************************************************************/
u8 Tsk_Read_ds18b20_MREG(u32 delaycnt)
{ 
	u8	i,res=0,crc,Scratchpad[8];
	static u8 step=0;
	
	switch(step)
	{
		case 0:
		{
			if(Delay_Fuction(g_Sys_1ms_Counter, g_ds18b20_counter, delaycnt))
			{
				Bus_1wire_Init();
				Write_OneByte_To1wire(ROM_Skip_Cmd);//跳过读序列号操作	
				Write_OneByte_To1wire(Convert_T);//启动温度转换
				step = 1;
				g_ds18b20_counter = g_Sys_1ms_Counter;
			}
		}
		break;
		case 1: 
		{
			res = 0x00;
			if(Delay_Fuction(g_Sys_1ms_Counter, g_ds18b20_counter, Con_T_Times[ConfigMode]))
			{
				Bus_1wire_Init();
				Write_OneByte_To1wire(ROM_Skip_Cmd);
				Write_OneByte_To1wire(Read_Scratchpad); //读取寄存器内容（可以从寄存器0读到寄存器8,共9个寄存器）

				for(i=0;i<9;i++)
				{
					if(i<=7)
					{
						Scratchpad[i] = Read_OneByte_From1wire();
					}
					else if(i==8)
					{
						crc = Read_OneByte_From1wire();
					}
				}
				
				/*crc计算*/
				if(crc==Crc8_Check(Scratchpad, sizeof(Scratchpad)))
				{
					memcpy(DS18B20_PAR.DS18b20_REG_DAT,Scratchpad,sizeof(DS18B20_PAR.DS18b20_REG_DAT));
					res = 0x10;
				}
				else
				{
					res = 0x01;
				}
				step = 0;
				g_ds18b20_counter = g_Sys_1ms_Counter;
			}
		}
		break;
		default:
		break;
	}

	return res;
}








/***************************************************************************************
**  End Of File                                                     
***************************************************************************************/
