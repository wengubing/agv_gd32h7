/**********************************************************************************************************
*
*	模块名称 : 模拟量底层驱动
*	文件名称 : drv_analog.c
*	版    本 : V1.0
*	说    明 : 通过SPI接口控制ADC、DAC实现多路模拟量输入和输出检测及控制

**********************************************************************************************************/

#define		ANALOG_GLOBALS

/* Includes ------------------------------------------------------------------*/
#include "includes.h"

#include "..\..\Drivers\inc\drv_analog.h"
#include "..\..\Drivers\inc\drv_timer.h"
#include "..\..\Drivers\inc\drv_spi.h"
#include "..\..\Drivers\ModbusRtuPoll\ModbusRtuPoll.h"
#include "..\..\Drivers\inc\drv_ee_para.h"
#include "ADDA.h"
#include "ACC_Project.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

#define		Analog_Delay_Times			0
#define		SPI_Analog_WORK_TYPE		0	//0:模拟SPI；1:硬件SPI；


/*--------------------------------------------------------DAC-TLV5614相关宏定义--------------------------------------------------------*/
#define	TLV5614_A									0x0000		// A
#define	TLV5614_B									0x4000		// B
#define	TLV5614_C									0x8000		// C
#define	TLV5614_D									0xC000		// D
#define	TLV5614_PWR_PD						0x2000		// 掉电状态
#define	TLV5614_PWR_NO						0x0000		// 正常状态
#define	TLV5614_SPD_FAST					0x1000		// 快速转换模式
#define	TLV5614_SPD_SLOW					0x0000		// 低速转换模式


/*--------------------------------------------------------ADC-TLV2544相关宏定义--------------------------------------------------------*/
//配置寄存器位CFR(命令集),高4位(bit15-12)
#define	TLV2544_CMD_SelectAIN0													0x0000	//选择模拟通道0
#define	TLV2544_CMD_SelectAIN1													0x2000 //选择模拟通道1
#define	TLV2544_CMD_SelectAIN2													0x4000	//选择模拟通道2
#define	TLV2544_CMD_SelectAIN3													0x6000	//选择模拟通道3
#define	TLV2544_CMD_SelectAIN4													0x8000 //SW电源跌落
#define	TLV2544_CMD_ReadCFG															0x9000 //读CFR导数据SDO(11-0)
#define	TLV2544_CMD_SetCFG															0xA000 //将低12位数据写入CFR
#define	TLV2544_CMD_Test1																0xB000		
#define	TLV2544_CMD_Test2																0xC000
#define	TLV2544_CMD_Test3																0xD000
#define	TLV2544_CMD_ReadFIFO														0xE000 //将FIFO内容送SDO
#define	TLV2544_CMD_Reserved														0xF000 //保留

//配置寄存器位CFR(数据域),低12位(bit11-0)
//bit11(基准源选择配置位)
#define	TLV2544_CFG_REFExternal													0x0000	//外部基准源
#define	TLV2544_CFG_REFInternal													0x0800	//内部基准源
//bit9(采样周期配置位)
#define	TLV2544_CFG_SampleShort 												0x0000	//短采样周期/12SCLKS
#define	TLV2544_CFG_SampleLong 													0x0200	//长采样周期/24SCLKS
//bit8-7(转换时钟源选择配置位)
#define	TLV2544_CFG_ConClockSourceInternalOSC 					0x0000	//转换时钟=内部OSC
#define	TLV2544_CFG_ConClockSourceInternalSCLK_1				0x0080	//转换时钟=SCLK/1
#define	TLV2544_CFG_ConClockSourceInternalSCLK_2				0x0180	//转换时钟=SCLK/2
#define	TLV2544_CFG_ConClockSourceInternalSCLK_4				0x0100	//转换时钟=SCLK/4
//bit6-5(转换模式选择)
#define	TLV2544_CFG_SingleShotMode_00										0x0000	//单次模式00
#define	TLV2544_CFG_RepeatMode_01												0x0020	//重复模式01
#define	TLV2544_CFG_SweepMode_10												0x0040	//扫描模式10
#define	TLV2544_CFG_RepeatSweepMode_11									0x0060	//重复扫描模式11
//bit4-3(扫描自动序列选择)---仅在扫描模式和重复扫描模式有效
#define	TLV2544_CFG_SweepAutoSequence_00								0x0000	//N/A
#define	TLV2544_CFG_SweepAutoSequence_01								0x0008	//0-1-2-3-0-1-2-3
#define	TLV2544_CFG_SweepAutoSequence_10								0x0010	//0-0-1-1-2-2-3-3
#define	TLV2544_CFG_SweepAutoSequence_11								0x0018	//0-1-0-1-0-1-0-1
//bit2(pin function select)
#define	TLV2544_CFG_INT 																0x0000	//EOC/INT作为:INT
#define	TLV2544_CFG_EOC				 													0x0004	//EOC/INT作为:EOC
//bit1-0(SDO输出的FIFO阀值设置)
#define	TLV2544_CFG_SSL_00															0x0000 //FULL
#define	TLV2544_CFG_SSL_01															0x0001 //3/4
#define	TLV2544_CFG_SSL_10															0x0002 //1/2
#define	TLV2544_CFG_SSL_11															0x0003 //1/4


//ADC采样方式---0:正常采样(软件启动AD变换),1:扩展采样(硬件启动AD变换---需CSTART引脚配合)
#define	ADC_Sample_Type																	0	
#define	ADC_Sample_Mode																	0	//0:单次模式;1:重复模式;2:扫描模式;3:重复扫描模式

#if	  (ADC_Sample_Type==0)		//正常采样方式

#if	  (ADC_Sample_Mode==0)
	#define	TLV2544_CFR		TLV2544_CFG_REFExternal	\
											| TLV2544_CFG_SampleLong	\
											| TLV2544_CFG_ConClockSourceInternalOSC	\
											| TLV2544_CFG_SingleShotMode_00	\
											| TLV2544_CFG_SweepAutoSequence_00	\
											| TLV2544_CFG_EOC	\
											| TLV2544_CFG_SSL_00
#elif	(ADC_Sample_Mode==3)
	#define	TLV2544_CFR		TLV2544_CFG_REFExternal	\
											| TLV2544_CFG_SampleShort	\
											| TLV2544_CFG_ConClockSourceInternalOSC	\
											| TLV2544_CFG_RepeatSweepMode_11	\
											| TLV2544_CFG_SweepAutoSequence_01	\
											| TLV2544_CFG_INT	\
											| TLV2544_CFG_SSL_10
#endif

#elif (ADC_Sample_Type!=0)		//扩展采样方式

	#define	TLV2544_CFR		TLV2544_CFG_REFExternal	\
											| TLV2544_CFG_SampleShort	\
											| TLV2544_CFG_ConClockSourceInternalOSC	\
											| TLV2544_CFG_RepeatSweepMode_11	\
											| TLV2544_CFG_SweepAutoSequence_01	\
											| TLV2544_CFG_INT	\
											| TLV2544_CFG_SSL_10

#endif
			
#define	Q12bit										0x0FFE			//2n-1
#define	AD_REF										5.0
#define	AD_Channel_num						5

/* Private variables ---------------------------------------------------------*/

u16 TLV2544_CMD[AD_Channel_num]={
																	TLV2544_CMD_SelectAIN0,	//1
																	TLV2544_CMD_SelectAIN1,	//2
																	TLV2544_CMD_SelectAIN2,	//3
																	TLV2544_CMD_Test3,			//4
																	TLV2544_CMD_SelectAIN3  //0
																},//ADC采集顺序定义(0->4)
TLV2544_AD_DATA[AD_Channel_num];
																

u8 ADC_poll_chx;

u16 Voutx[8];//临时存储记录最后更改的DA输出电压值
														
f32 K_Voltage_to_High[4] = {0}, K_High_Low = 0;
u16 V_High_Low[4] = {0};																

#define Kp_Voltage_to_High 	0.29296875f	 // 1200/4096;
																
																
#if	(SPI_Analog_WORK_TYPE==0)

/**********************************************************************************************************
*	函 数 名: TLV5614_Write_SimulateSpi
*	功能说明: TLV5614写半个字
*	形    参:  无
*	返 回 值:  无
**********************************************************************************************************/
void TLV5614_Write_SimulateSpi(UseSPIx ch, u16 dat)
{
	u8 i;

	for(i=0;i<16;i++)
	{
		SPIx_GPIO_PORT_SCK[ch]->BSRRL=SPIx_GPIO_PIN_SCK[ch];//H
		Delay_1us(Analog_Delay_Times);
		if((dat<<i) & 0x8000)//1
			SPIx_GPIO_PORT_MOSI[ch]->BSRRL=SPIx_GPIO_PIN_MOSI[ch];//H
		else//0
			SPIx_GPIO_PORT_MOSI[ch]->BSRRH=SPIx_GPIO_PIN_MOSI[ch];//L
		SPIx_GPIO_PORT_SCK[ch]->BSRRH=SPIx_GPIO_PIN_SCK[ch];//L,下降沿采样
		Delay_1us(Analog_Delay_Times);
	}
	
	SPIx_GPIO_PORT_SCK[ch]->BSRRL=SPIx_GPIO_PIN_SCK[ch];//H
	SPIx_GPIO_PORT_MOSI[ch]->BSRRH=SPIx_GPIO_PIN_MOSI[ch];//H
}

/**********************************************************************************************************
*	函 数 名: TLV2544_Write_SimulateSpi
*	功能说明: TLV2544写半个字
*	形    参:  无
*	返 回 值:  无
**********************************************************************************************************/
void TLV2544_Write_SimulateSpi(UseSPIx ch, u16 dat)
{
	u8 i;
	
	for(i=0;i<16;i++)
	{
		SPIx_GPIO_PORT_SCK[ch]->BSRRH=SPIx_GPIO_PIN_SCK[ch];//L
		Delay_1us(Analog_Delay_Times);
		if((dat<<i) & 0x8000)//1
			SPIx_GPIO_PORT_MOSI[ch]->BSRRL=SPIx_GPIO_PIN_MOSI[ch];//H
		else//0
			SPIx_GPIO_PORT_MOSI[ch]->BSRRH=SPIx_GPIO_PIN_MOSI[ch];//L
		SPIx_GPIO_PORT_SCK[ch]->BSRRL=SPIx_GPIO_PIN_SCK[ch];//H,上升沿采样
		Delay_1us(Analog_Delay_Times);
	}
	
	SPIx_GPIO_PORT_MOSI[ch]->BSRRH=SPIx_GPIO_PIN_MOSI[ch];//H
}

/**********************************************************************************************************
*	函 数 名: TLV2544_Write_SimulateSpi
*	功能说明: TLV2544写半个字
*	形    参:  无
*	返 回 值:  无
**********************************************************************************************************/
void TLV2544_Send_Clk(UseSPIx ch, u16 num)
{
	u8 i;
	
	for(i=0;i<num;i++)
	{
		SPIx_GPIO_PORT_SCK[ch]->BSRRH=SPIx_GPIO_PIN_SCK[ch];//L
		Delay_1us(Analog_Delay_Times);
		SPIx_GPIO_PORT_MOSI[ch]->BSRRH=SPIx_GPIO_PIN_MOSI[ch];//H
		SPIx_GPIO_PORT_SCK[ch]->BSRRL=SPIx_GPIO_PIN_SCK[ch];//H	
		Delay_1us(Analog_Delay_Times);
	}
	
}

/**********************************************************************************************************
*	函 数 名: TLV2544_WriteRead_SimulateSpi
*	功能说明: TLV2544写/读半个字
*	形    参:  无
*	返 回 值:  无
//当该端口为高,则状态为"1"
#define		BIT_MISO_status1				(((GPIO_PORT_SPI2_MISO -> IDR) & GPIO_PIN_SPI2_MISO) == GPIO_PIN_SPI2_MISO)	
//当该端口为低,则状态为"0"
#define		BIT_MISO_status0				(((GPIO_PORT_SPI2_MISO -> IDR) & GPIO_PIN_SPI2_MISO) == 0x0000)
**********************************************************************************************************/
u16 TLV2544_WriteRead_SimulateSpi(UseSPIx ch, u16 dat, u8 type)
{
	u8 i;//temp=0;
	u16 data=0;

	for(i=0;i<16;i++)
	{
		if(type==0)
		{//读取CFG
			
			if(i<4)
			{
				SPIx_GPIO_PORT_SCK[ch]->BSRRH=SPIx_GPIO_PIN_SCK[ch];//L
				Delay_1us(Analog_Delay_Times);				
				if((dat<<i) & 0x8000)//1
					SPIx_GPIO_PORT_MOSI[ch]->BSRRL=SPIx_GPIO_PIN_MOSI[ch];//L
				else//0
					SPIx_GPIO_PORT_MOSI[ch]->BSRRH=SPIx_GPIO_PIN_MOSI[ch];//H
				SPIx_GPIO_PORT_SCK[ch]->BSRRL=SPIx_GPIO_PIN_SCK[ch];//H
				Delay_1us(Analog_Delay_Times);
			}
			else
			{
				SPIx_GPIO_PORT_SCK[ch]->BSRRH=SPIx_GPIO_PIN_SCK[ch];//L
				Delay_1us(Analog_Delay_Times);		
				SPIx_GPIO_PORT_SCK[ch]->BSRRL=SPIx_GPIO_PIN_SCK[ch];//H
				Delay_1us(Analog_Delay_Times);
				data |= ((u16)((SPIx_GPIO_PORT_MISO[ch]->IDR & SPIx_GPIO_PIN_MISO[ch]) == SPIx_GPIO_PIN_MISO[ch]) << (15-i));
			}
		}
		else if(type==1)
		{//读取指定通道的数据
			SPIx_GPIO_PORT_SCK[ch]->BSRRH=SPIx_GPIO_PIN_SCK[ch];//L
			Delay_1us(Analog_Delay_Times);	
			if((dat<<i) & 0x8000)//1
				SPIx_GPIO_PORT_MOSI[ch]->BSRRL=SPIx_GPIO_PIN_MOSI[ch];//L
			else//0
				SPIx_GPIO_PORT_MOSI[ch]->BSRRH=SPIx_GPIO_PIN_MOSI[ch];//H
			SPIx_GPIO_PORT_SCK[ch]->BSRRL=SPIx_GPIO_PIN_SCK[ch];//H
			Delay_1us(Analog_Delay_Times);
			data |= ((u16)((SPIx_GPIO_PORT_MISO[ch]->IDR & SPIx_GPIO_PIN_MISO[ch]) == SPIx_GPIO_PIN_MISO[ch]) << (15-i));
		}
	}

	SPIx_GPIO_PORT_MOSI[ch]->BSRRH=SPIx_GPIO_PIN_MOSI[ch];//H
	SPIx_GPIO_PORT_MISO[ch]->BSRRH=SPIx_GPIO_PIN_MISO[ch];//H
	
	return data;
}

#endif

/**********************************************************************************************************
*	函 数 名: TLV2544_WriteHalfWord
*	功能说明: TLV2544写半个字
*	形    参:  无
*	返 回 值:  无
**********************************************************************************************************/
void TLV2544_WriteHalfWord(u16 dat)
{
	GPIO_CTRL(ADC1_CS_GPIO, RESET);
	
#if	(SPI_Analog_WORK_TYPE==0)
	
	TLV2544_Write_SimulateSpi(SPIA, dat);
	
#elif 	(SPI_Analog_WORK_TYPE==1)
	
	SpiWriteSingleDS(SPIA, dat);
	
#endif

	GPIO_CTRL(ADC1_CS_GPIO, SET);	
}

/**********************************************************************************************************
*	函 数 名: TLV2544_RWWord
*	功能说明: TLV2544读/写半个字
*	形    参:  无
*	返 回 值:  无
**********************************************************************************************************/
u16 TLV2544_RWWord(u16 dat)
{
	u16 data=0;
	
	//拉低CS前需要将SCK置低
#if	(SPI_Analog_WORK_TYPE==0)

	SPIx_GPIO_PORT_SCK[SPIA]->BSRRH=SPIx_GPIO_PIN_SCK[SPIA];//L	
	
#elif 	(SPI_Analog_WORK_TYPE==1)

#endif

	GPIO_CTRL(ADC1_CS_GPIO, RESET);
	
#if	(SPI_Analog_WORK_TYPE==0)
	
	data=TLV2544_WriteRead_SimulateSpi(SPIA, dat, 1);
	
#elif 	(SPI_Analog_WORK_TYPE==1)
	
	data=SpiRWSingleDS(SPIA, dat);
	
#endif

	//发送固定个数的采用脉冲
//#if	(SPI_Analog_WORK_TYPE==0)
//	
//	TLV2544_Write_SimulateSpi(SPIA, 0x0000);	
//	//TLV2544_Send_Clk(SPIA, 14);
//	
//#elif 	(SPI_Analog_WORK_TYPE==1)

//	SpiWriteSingleDS(SPIA, 0x0000);
//	
//#endif

	//拉高CS前需要将SCK置低
#if	(SPI_Analog_WORK_TYPE==0)

	SPIx_GPIO_PORT_SCK[SPIA]->BSRRH=SPIx_GPIO_PIN_SCK[SPIA];//L	
	
#elif 	(SPI_Analog_WORK_TYPE==1)

#endif

	Delay_1us(1);
	GPIO_CTRL(ADC1_CS_GPIO, SET);	


	return data;
}

/**********************************************************************************************************
*	函 数 名: TLV2544_ReadCfg
*	功能说明: TLV2544读取配置数据
*	形    参:  无
*	返 回 值:  无
**********************************************************************************************************/
u16 TLV2544_ReadCfg(u16 dat)
{	
	u16 data=0;

	GPIO_CTRL(ADC1_CS_GPIO, RESET);

#if	(SPI_Analog_WORK_TYPE==0)
	
	data=TLV2544_WriteRead_SimulateSpi(SPIA, dat, 0);

#elif 	(SPI_Analog_WORK_TYPE==1)
	
	data=SpiRWSingleDS(SPIA, dat);
	
#endif

	GPIO_CTRL(ADC1_CS_GPIO, SET);	
	
	return data;
}

/**********************************************************************************************************
*	函 数 名: TLV2544_ConfigIni
*	功能说明: TLV2544配置初始化
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
u8 TLV2544_ConfigIni(void)
{
#if	  (ADC_Sample_Mode==0)
	u16 TLV2544ConfigData=0;	
#endif
	
	//配置CFG相关位(配置其工作方式)	
	TLV2544_WriteHalfWord(TLV2544_CMD_SetCFG | TLV2544_CFR);
	
#if	  (ADC_Sample_Mode==0)
	TLV2544ConfigData=TLV2544_ReadCfg(TLV2544_CMD_ReadCFG) ;//& 0x0FFF;	
	if(TLV2544ConfigData!=TLV2544_CFR)
		return 1;
#endif

	TLV2544_RWWord(TLV2544_CMD[0]);
	
	return 0;
}

/**********************************************************************************************************
*	函 数 名: TLV5614_WriteHalfWord
*	功能说明: TLV5614写半个字
*	形    参:  无
*	返 回 值:  无
**********************************************************************************************************/
void TLV5614_WriteHalfWord(u16 dat, u8 sel)
{
	GPIO_TypeDef* CS_GPIOx;
	u16 CS_GPIO_Pin;
	
	if(sel==1)
	{
		CS_GPIOx=DAC1_CS_GPIO_PORT;
		CS_GPIO_Pin=DAC1_CS_GPIO_PIN;
	}
	else if(sel==2)
	{
		CS_GPIOx=DAC2_CS_GPIO_PORT;
		CS_GPIO_Pin=DAC2_CS_GPIO_PIN;
	}

	GPIO_CTRL(CS_GPIOx, CS_GPIO_Pin, RESET);//激活DAC
	Delay_1us(Analog_Delay_Times);	//至少50ns

	/*发出同步脉冲FS信号(下降沿),表示串行数据帧开始传送*/
	GPIO_CTRL(DAC_FS_GPIO, SET);	
	Delay_1us(Analog_Delay_Times);	//至少50ns
	GPIO_CTRL(DAC_FS_GPIO, RESET);//FS拉低
	
#if	(SPI_Analog_WORK_TYPE==0)
	
	TLV5614_Write_SimulateSpi(SPIA, dat);

#elif 	(SPI_Analog_WORK_TYPE==1)
	
	SpiWriteSingleDS(SPIA, dat);
	
#endif
	
	/*LDAC产生一个下降沿,更新输出*/
	GPIO_CTRL(DAC_LDAC_GPIO, RESET);
	Delay_1us(Analog_Delay_Times);
	GPIO_CTRL(DAC_LDAC_GPIO, SET);
	
	GPIO_CTRL(DAC_FS_GPIO, SET);	//FS拉高
	GPIO_CTRL(CS_GPIOx, CS_GPIO_Pin, SET); //CS拉高
}

/**********************************************************************************************************
*	函 数 名: TLV5614_UpdateOut
*	功能说明: TLV5614更新输出
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void TLV5614_UpdateOut(u16 code, u8 channel, u16 status)
{
	u8  cs;
	u16 data=0x0000;
	
	if(code>Q12bit || channel>8 || channel==0)
		return;
	
	if(channel>=5)
		cs=2;//使用DAC2
	else
		cs=1;//使用DAC1	

	data = (status | TLV5614_SPD_FAST | code);

	switch (channel)
	{
		case 1:
		case 5:
						data |= TLV5614_A;
			break;
		case 2:
		case 6:
						data |= TLV5614_B;
			break;
		case 3:
		case 7:
						data |= TLV5614_C;
			break;
		case 4:
		case 8:
						data |= TLV5614_D;
			break;
		default:
			break;
	}

	TLV5614_WriteHalfWord(data, cs);
}

/**********************************************************************************************************
*	函 数 名: AD_CalVoltage
*	功能说明: 根据AD计算电压
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
u16 AD_CalVoltage(u16 data,u8 ch)
{
	u16 buffer=0;
	f32	xpt;
	
	xpt=(f32)data*((f32)AD_REF / (f32)Q12bit);
	buffer=xpt*1000;//unit:mv

	return (buffer);
}

/**********************************************************************************************************
*	函 数 名: Voltage_to_Angle
*	功能说明: 根据电压计算角度
*	形    参: v:输入电压:unit:mv
*	返 回 值: X0.1°的值
**********************************************************************************************************/
u16 Voltage_to_Angle(u16 v)
{
	f32 temp = (f32)v * K_Voltage_to_Angle;
	temp += 0.5f;
	
	return (temp);
}

/**********************************************************************************************************
*	函 数 名: Voltage_to_High
*	功能说明: 根据电压计算长度
*	形    参: vol:输入电压:unit:mv
*	返 回 值: 
**********************************************************************************************************/
s16 Voltage_to_High(u16 vol, u8 i)
{
	f32 temp = vol - V_High_Low[i];

	if(temp>0)
	{
		temp = (f32)temp * K_Voltage_to_High[i] + K_High_Low;	
	}
	else
	{
		temp = K_High_Low;	
	}

	return (temp);
}

/**********************************************************************************************************
*	函 数 名: Voltage_to_Current
*	功能说明: 根据电压计算电流
*	形    参: v:输入电压:unit:mv
*	返 回 值: mA直
**********************************************************************************************************/
s32 Voltage_to_Current(u16 v)
{
	f32 i_temp=0;

//	i_temp = (v - CurentSensorMidVoltage) * CurentSensor_delta;
	return (i_temp);
}

/**********************************************************************************************************
*	函 数 名: Voltage_to_Voltage
*	功能说明: 根据电压计算电流
*	形    参: v:输入电压:unit:mv
*	返 回 值: 电压值
**********************************************************************************************************/
s32 Voltage_to_Voltage(u16 v)
{
    //说明
    //隔离电压采集模块范围为0-250
    //输出范围为0-5V，AD值为0-4096
	f32 kp = 0.6105f,      //250/4095 = 0.06105 扩大十倍
        i_temp=0;

    i_temp =  v * kp;   
    
	return (i_temp);
}

/**********************************************************************************************************
*	函 数 名: Voltage_CalDA
*	功能说明: 根据电压计算DA值
*	形    参: 无
*	返 回 值: 无
由此计算：(DaCode/Q12bit)*DA_REF*2=v
DaCode=(v*Q12bit-1)/(2*DA_REF)=(4095*v)/2*DA_REF
**********************************************************************************************************/
u16 Voltage_CalDACode(f32 v)
{
	u16 DaCode=0x0000;
	
	if(v>DA_Vmax && v<0.0f)
		return 0xFFFF;
	
	if(v==0.0f)
		DaCode=0x0000;
	else if(v>=DA_Vmax)
		DaCode=Q12bit;
	else
	{
		DaCode=(v*4095)/(2*DA_REF)+0.5f;	
		if(DaCode>Q12bit)
			DaCode=Q12bit;		
	}

	return DaCode;
}


/**********************************************************************************************************
*	函 数 名: Analog_InitHard
*	功能说明: 模拟量硬件初始化
*	形    参:  无
*	返 回 值:  无
**********************************************************************************************************/
void Analog_InitHard(void)
{
	u8 i;
  GPIO_InitTypeDef	GPIO_IS;
	EXTI_InitTypeDef	EXTI_IS;
	NVIC_InitTypeDef	NVIC_IS;

	/*SPI外设硬件配置*/
#if	(SPI_Analog_WORK_TYPE==0)
	
	Spi_InitHard(SPIA, PLAN1);
	
#elif 	(SPI_Analog_WORK_TYPE==1)
	
	Spi_InitHard(SPIA, PLAN2);
	
#endif

	
	/*模拟量相关GPIO配置*/
	RCC_AHB1PeriphClockCmd(DAC1_CS_GPIO_CLK | DAC2_CS_GPIO_CLK  | DAC_LDAC_GPIO_CLK | DAC_FS_GPIO_CLK 
												| ADC1_CS_GPIO_CLK | ADC1_CSTART_GPIO_CLK, ENABLE);
	GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_IS.GPIO_OType = GPIO_OType_PP;
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_IS.GPIO_Pin = DAC_FS_GPIO_PIN;//FS
	GPIO_Init(DAC_FS_GPIO_PORT, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = DAC_LDAC_GPIO_PIN;//LDAC
	GPIO_Init(DAC_LDAC_GPIO_PORT, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = DAC1_CS_GPIO_PIN;//DAC1_CS
	GPIO_Init(DAC1_CS_GPIO_PORT, &GPIO_IS);

	GPIO_IS.GPIO_Pin = DAC2_CS_GPIO_PIN;//DAC2_CS
	GPIO_Init(DAC2_CS_GPIO_PORT, &GPIO_IS);

	GPIO_IS.GPIO_Pin = ADC1_CS_GPIO_PIN;//ADC1_CS
	GPIO_Init(ADC1_CS_GPIO_PORT, &GPIO_IS);

	GPIO_IS.GPIO_Pin = ADC1_CSTART_GPIO_PIN;//ADC1_CSTART
	GPIO_Init(ADC1_CSTART_GPIO_PORT, &GPIO_IS);

	/*配置ADC_INT引脚检测中断*/
	RCC_AHB1PeriphClockCmd(ADC1_EOCINT_GPIO_CLK, ENABLE);
	GPIO_IS.GPIO_Mode = GPIO_Mode_IN;
	GPIO_IS.GPIO_OType = GPIO_OType_PP;
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_IS.GPIO_Pin = ADC1_EOCINT_GPIO_PIN;
	GPIO_Init(ADC1_EOCINT_GPIO_PORT, &GPIO_IS);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	SYSCFG_EXTILineConfig(EXTI_PortSource_ADC1_EOCINT, EXTI_PinSource_ADC1_EOCINT);	//连接 EXTI Line8到 XIN16 引脚

	EXTI_IS.EXTI_Line = EXTI_Line8;//gpio外中断8
	EXTI_IS.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_IS.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿
	EXTI_IS.EXTI_LineCmd = ENABLE;//使能相应的中断线
	EXTI_Init(&EXTI_IS);

	NVIC_IS.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_IS.NVIC_IRQChannelSubPriority = 2;//子优先级2
	NVIC_IS.NVIC_IRQChannelCmd = ENABLE;//IRQ通道使能	
	NVIC_IS.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_IS);
	
	/*初始化引脚*/
	GPIO_CTRL(DAC1_CS_GPIO, SET);
	GPIO_CTRL(DAC2_CS_GPIO, SET);
	GPIO_CTRL(ADC1_CS_GPIO, SET);
	GPIO_CTRL(ADC1_CSTART_GPIO, SET);
	GPIO_CTRL(DAC_LDAC_GPIO, SET);
	GPIO_CTRL(DAC_FS_GPIO, SET);
	
	/*ADC初始化配置*/
	TLV2544_ConfigIni();

	/*DAC上电初始化*/
	for(i=1;i<9;i++)
	{
		TLV5614_UpdateOut(0, i, TLV5614_PWR_PD);//更新输出
	}


}

/**********************************************************************************************************
*	函 数 名: Tsk_Analog
*	功能说明: 模拟量任务函数
*	形    参:  无
*	返 回 值:  无
**********************************************************************************************************/
void Tsk_Analog(void)
{
	ADDA_ADCN_StDef* p2ADCN = (ADDA_ADCN_StDef*)&g_ADCNs;
	ADDA_DACN_StDef* p2DACN = (ADDA_DACN_StDef*)&g_DACNs;

	u8 i, IsWantToVout=0,j,
			VoutChannel[8]={0};//记录当前要输出的通道号:1-8
	u16 buffer;

	//--------------------------------------------------------ADC采集处理程序

#if	  (ADC_Sample_Type==0)		//正常采样方式
			
#if	  (ADC_Sample_Mode==0x00)//单次模式!!!

	if(Delay_Fuction(g_Sys_1ms_Counter, ADC_CHK_1ms_cnt, 5) && ADC_EOC_Flag)
	{
		ADC_EOC_Flag=0;
		TLV2544_AD_DATA[ADC_poll_chx]=TLV2544_RWWord(TLV2544_CMD[ADC_poll_chx]);//依次选取相应通道CHx,并读取出上一次选定的通道的值
		if(++ADC_poll_chx > 4)
		{
			ADC_poll_chx=0;		
		}
		ADC_CHK_1ms_cnt=g_Sys_1ms_Counter;//置延迟计数器
	}
	else
	{
		if(Delay_Fuction(g_Sys_1ms_Counter, ADC_CHK_1ms_cnt, 1000))
		{//模拟量部分出现异常...
				ADC_EOC_Flag = 1;
				ADC_CHK_1ms_cnt=g_Sys_1ms_Counter;
		}
	}

		
#elif	(ADC_Sample_Mode==0x03)//重复扫描模式!!!

	if(Delay_Fuction(g_Sys_1ms_Counter, ADC_CHK_1ms_cnt, 2) && ADC_poll_chx<4)
	{
		ADC_CHK_1ms_cnt=g_Sys_1ms_Counter;//置延迟计数器
		for(i=0;i<AD_Channel_num;i++)
		{
			TLV2544_WriteHalfWord(TLV2544_CMD[i]);
		}	
		ADC_poll_chx=4;
	}
	else
	{
		if(ADC_EOC_Flag)
		{
			ADC_EOC_Flag=0;
			for(i=0;i<AD_Channel_num;i++)
			{
				TLV2544_AD_DATA[i]=TLV2544_RWWord(TLV2544_CMD_ReadFIFO)>>4;
			}
			ADC_poll_chx=0;
		}
	}
	
#endif

#elif (ADC_Sample_Type!=0)		//扩展采样方式!!!
		
#endif
	/*采集到的AD值,数据处理*/
	for(i=0;i<AD_Channel_num;i++)
	{
		//强制停止AD原始值更新
		if(g_Para.Debug.ForceADOrgStop==0)
			p2ADCN[i].OrgVal=TLV2544_AD_DATA[i]>>4;	

		if(i==3)//CN5换算
		{
			p2ADCN[i].mV=(p2ADCN[i].OrgVal*AD_K_RATIO - PwrIN_OffsetVoltage_data) / 100;//0.1V
		}
		else
		{
			p2ADCN[i].mV=AD_CalVoltage(p2ADCN[i].OrgVal, i);
		}
	}

	
	//--------------------------------------------------------DAC输出配置程序
	////
	/*判断DA输出数据区是否有更改*/
	for(i=0;i<8;i++)
	{
		if(Voutx[i]!=p2DACN[i].DigVal && g_Para.Debug.ForceDACmdStop==0)//有更改
		{
			Voutx[i]=p2DACN[i].DigVal;
			VoutChannel[i]=i+1;
			IsWantToVout++;
		}
		/*{
			Voutx[i]=p2DACN[i].DigVal;
			VoutChannel[i]=i+1;
			IsWantToVout++;
		}*/
	}

	/*有更改操作,TLV5614更新输出*/
	if(IsWantToVout>0)
	{
		for(i=0;i<8;i++)
		{
			if(VoutChannel[i]>0)
			{
				j=VoutChannel[i];
				buffer=Voltage_CalDACode((f32)Voutx[j-1]/1000);//计算输出code
				if(buffer>Q12bit)
					continue;
				TLV5614_UpdateOut(buffer, j, TLV5614_PWR_NO);//更新输出
			}
		}
	}


}


/**********************************************************************************************************
*	函 数 名: EXTI2_IRQHandler
*	功能说明: 外部中断服务程序
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2) != RESET)
	{	
		EXTI_ClearITPendingBit(EXTI_Line2);

	}
}


/***************************** hangfa (END OF FILE) *********************************/
