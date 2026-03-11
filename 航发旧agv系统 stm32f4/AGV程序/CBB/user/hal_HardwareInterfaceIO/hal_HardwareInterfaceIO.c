/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : hal_HardwareInterfaceIO.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.02.07
  * Description: 硬件抽象层IO接口模块
  *					1、32个单片机直接GPIO输出管脚， Y0 -Y31，其中Y0-Y3 用作PWM输出
  *					1、40个595芯片拓展GPIO输出管脚，Y32-Y71
  *******************************************************************************/
#define  HAL_HARDWARE_INTER_FACE_IO_GLOBALS

#include "hal_HardwareInterfaceIO.h"
#include "string.h"

/*硬件抽象层IO 运行过程的中间变量对像定义*/
typedef struct {
		
	uint16_t 				HC595SendBuff[EXT595_NUM];			/*HC595即将发送的数据内容*/
	uint8_t 				HalGpioNum;							/*GPIO对象列表数量，每次成功调用HAL_IOAdd函数自加1*/
	_HalGpio_TypeDef		*p_HalGpioList[g_HalGpioNumMax];	/*GPIO对象列表，自动将全局变量的值，放置此列表元素指向的位置*/
	_HalGpioValue_TypeDef	*p_Temp;
	
}_HalGpioTemp_TypeDef;



/*本模全局变量声明*/
_HalGpioCN_TypeDef 		g_HalGPIO;		/*用于存储所有GPIO的状态信息，对外开放*/
_HalGpioTemp_TypeDef	g_HalGpioTemp;	/*用于存储所有运行过程的中间变量,外部不可访问*/


/*内部调用函数声明*/
void HAL_IO_Ini_HC595(void);

//ADR,代表变量中的bit(0-15)
/* -----------------------------------------------------------------------------------*/ 
#define HAL_IO_SET_COIL(ADR) (g_HalGpioTemp.HC595SendBuff[((ADR)/16)] |=  (0x0001 << (ADR&0x0F))) /* 置位COIL   */
#define HAL_IO_CLR_COIL(ADR) (g_HalGpioTemp.HC595SendBuff[((ADR)/16)] &= ~(0x0001 << (ADR&0x0F))) /* 清零COIL   */

/**************************************************************************
* 函数名:	HAL_IOAdd()
* 功  能:	硬件抽象层IO 添加GPIO服务对象
* 参  数:	无
* 返回值:	无
**************************************************************************/
uint8_t HAL_IOAdd(_HalGpio_TypeDef *p_Data)
{
	uint8_t res = 0;
	do
	{
		/*链表越界*/
		if(g_HalGpioTemp.HalGpioNum >= g_HalGpioNumMax)
		{
			res = 1;
			break;
		}
		/*链表内容添加*/
		g_HalGpioTemp.p_HalGpioList[g_HalGpioTemp.HalGpioNum+1] = p_Data;
		/*链表地址递增*/
		g_HalGpioTemp.HalGpioNum++;
	}while(0);

	return res;
}

/**************************************************************************
* 函数名:	HAL_IOTaskProcess()
* 功  能:	硬件抽象层IO服务主循环函数
* 参  数:	无
* 返回值:	无
**************************************************************************/
void HAL_IOInit(void)
{
	_HalGpioNum_TypeDef CountNum = Cn56;
	GPIO_InitTypeDef GPIO_IS;
	EXTI_InitTypeDef EXTI_IS;
	NVIC_InitTypeDef NVIC_IS;
	/*清零模块内部的变量*/
	memset(&g_HalGPIO,0,sizeof(g_HalGPIO));
	memset(&g_HalGpioTemp,0,sizeof(g_HalGpioTemp));
	/*初始化链表，保证指针不是野指针*/
	for(CountNum=Cn56; CountNum<g_HalGpioNumMax ;CountNum++)
	{
		g_HalGpioTemp.p_HalGpioList[CountNum] = NULL;
	}

	/*配置普通输入管脚-硬件配置*/
	RCC_AHB1PeriphClockCmd(CN_CLK_IN,ENABLE);
	GPIO_IS.GPIO_Mode = GPIO_Mode_IN;		/*设为输入模式*/
	GPIO_IS.GPIO_OType = GPIO_OType_PP;		/*设为推挽类型*/
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_DOWN;		/*下拉电阻使能*/
	GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;	/*IO口最大速度*/
	/*依次初始化，Cn40_5是第一个输出io*/
	for(CountNum=Cn56; CountNum<Cn40_5 ;CountNum++)
	{
		GPIO_IS.GPIO_Pin = CN_PIN[CountNum];
		GPIO_Init(CN_PORT[CountNum], &GPIO_IS);
	}
	
	/*配置CN56（XIN16）、CN72（XIN17）、CN57（XIN18）的EXTI检测中断*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟

	SYSCFG_EXTILineConfig(EXTI_PortSource_CN56, EXTI_PinSource_CN56);	//连接 EXTI Line9到 XIN16 引脚
	SYSCFG_EXTILineConfig(EXTI_PortSource_CN72, EXTI_PinSource_CN72);	//连接 EXTI Line10到 XIN17 引脚
	SYSCFG_EXTILineConfig(EXTI_PortSource_CN57, EXTI_PinSource_CN57);	//连接 EXTI Line11到 XIN18 引脚
	
	EXTI_IS.EXTI_Line = EXTI_Line9 | EXTI_Line10;	//gpio外中断9-10
	EXTI_IS.EXTI_Mode = EXTI_Mode_Interrupt;		//中断事件
	EXTI_IS.EXTI_Trigger = EXTI_Trigger_Rising;		//上升沿触发
	EXTI_IS.EXTI_LineCmd = ENABLE;					//使能相应的中断线
	EXTI_Init(&EXTI_IS);
	
	EXTI_IS.EXTI_Line = EXTI_Line11;				//gpio外中断11
	EXTI_IS.EXTI_Trigger = EXTI_Trigger_Falling;	//下降沿触发
	EXTI_Init(&EXTI_IS);

	NVIC_IS.NVIC_IRQChannelPreemptionPriority=2 ;	//抢占优先级2
	NVIC_IS.NVIC_IRQChannelSubPriority = 2;			//子优先级2
	NVIC_IS.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能	

	NVIC_IS.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_IS);

	NVIC_IS.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Init(&NVIC_IS);

	/*配置普通输出管脚-硬件配置*/
	RCC_AHB1PeriphClockCmd(CN_CLK_OUT,ENABLE);
	GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;			//设为输出口
	GPIO_IS.GPIO_OType = GPIO_OType_PP;			//设为推挽模式
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_DOWN;			//下拉使能
	GPIO_IS.GPIO_Speed = GPIO_Speed_25MHz;		//IO口最大速度
	/*依次初始化，Cn40_5是第一个输出io*/
	for(CountNum=Cn40_5; CountNum<Cn28 ;CountNum++)
	{
		GPIO_IS.GPIO_Pin = CN_PIN[CountNum];
		GPIO_Init(CN_PORT[CountNum], &GPIO_IS);
	}

	/*配置74HC595扩展IO的控制端口*/
	RCC_AHB1PeriphClockCmd(EXTIO_CLK_SI|EXTIO_CLK_RCK|EXTIO_CLK_SCK|EXTIO_CLK_OE, ENABLE);              
	GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;		//设为输出口
	GPIO_IS.GPIO_OType = GPIO_OType_PP;		//设为推挽模式
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;		//上拉使能
	GPIO_IS.GPIO_Speed = GPIO_Speed_100MHz;	//IO口最大速度

	GPIO_IS.GPIO_Pin = EXTIO_PIN_SI;
	GPIO_Init(EXTIO_PORT_SI, &GPIO_IS);

	GPIO_IS.GPIO_Pin = EXTIO_PIN_RCK;
	GPIO_Init(EXTIO_PORT_RCK, &GPIO_IS);

	GPIO_IS.GPIO_Pin = EXTIO_PIN_SCK;
	GPIO_Init(EXTIO_PORT_SCK, &GPIO_IS);
    
	GPIO_IS.GPIO_Pin = EXTIO_PIN_OE;                 
	GPIO_Init(EXTIO_PORT_OE, &GPIO_IS);
	HAL_IO_Ini_HC595();
	
	
}

/**********************************************************************************************************
*	函 数 名: HAL_IO_XIN_CHK
*	功能说明: 
*	形    参: 
*	返 回 值: 1:检测高电平"1"，0:检测到低电平"0";
**********************************************************************************************************/
_HalGpioLevel_TypeDef HAL_IO_XIN_CHK(_HalGpioNum_TypeDef CnNum)
{
	if ((CN_PORT[CnNum]->IDR & CN_PIN[CnNum]) != 0)
		return UpLevel;
	else
		return LowLevel;
}

/**********************************************************************************************************
*	函 数 名: YOUT_0
*	功能说明: 选中GPIO输出"0"
*	形    参: YOx
*	返 回 值: 无
**********************************************************************************************************/
void HAL_IO_YOUTx_0(_HalGpioNum_TypeDef CnNum)
{
	CN_PORT[CnNum]->BSRRH=CN_PIN[CnNum];
}

/**********************************************************************************************************
*	函 数 名: YOUT_1
*	功能说明: 选中GPIO输出"1"
*	形    参: YOx
*	返 回 值: 无
**********************************************************************************************************/
void HAL_IO_YOUTx_1(_HalGpioNum_TypeDef CnNum)
{
	CN_PORT[CnNum]->BSRRL=CN_PIN[CnNum];
}
/**********************************************************************************************************
*	函 数 名: XIN_CHK
*	功能说明: 初始化GPIO扩展IC
*	形    参: None
*	返 回 值: None
**********************************************************************************************************/
void HAL_IO_Ini_HC595(void)
{
	SI_L;
	RCK_H;
	SCK_H;
	OE_L;
}

/**********************************************************************************************************
*	函 数 名: Fuc_Hc595_SendByte
*	功能说明: 595传输1字节数据
*	形    参: None
*	返 回 值: None
**********************************************************************************************************/
void HAL_IO_Hc595_SendByte(uint8_t dat)
{
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		SCK_L;
		if(dat&0x80)//MSB在前
			SI_H;
		else		
			SI_L;
//		Delay_1us(1);
		dat=dat<<1;

		SCK_H;//上升沿,数据寄存器的数据移位
//		Delay_1us(1);
	}
}
/**********************************************************************************************************
*	函 数 名: Fuc_Hc595_FreshIO
*	功能说明: 刷新595IO状态
*	形    参: dat:刷新数据起始地址指针,len:有效数据长度;
*	返 回 值: None
**********************************************************************************************************/
void HAL_IO_Hc595_FreshIO(uint8_t *dat, uint8_t len)
{
	uint8_t i;

	if(!dat && !len)
		return;
	
	RCK_L;//拉低
//	Delay_1us(1);

	for(i=len;i>0;i--)
	{
		//dat1,dat2.....,datn
		HAL_IO_Hc595_SendByte(*(dat+i-1));//先传送靠后的字节数据:datn
	}
	
	SI_H;
	SCK_H;
	RCK_H;//上升沿,移位寄存器的数据进入数据存储寄存器
}

/**************************************************************************
* 函数名:	HAL_IOTaskProcess()
* 功  能:	硬件抽象层IO服务主循环函数
* 参  数:	无
* 返回值:	无
**************************************************************************/
void HAL_IOTaskProcess(void)
{
	_HalGpioNum_TypeDef CountNum = Cn56;
	_HalGpioValue_TypeDef *p_HalGpio = &(g_HalGPIO.CN56);			/*过程指针指向全局变量首地址*/
	/*扫描单片机直连 全部的输入GPIO*/
	for(CountNum=Cn56; CountNum<Cn40_5; CountNum++)
	{
		(p_HalGpio+CountNum)->InValue =  HAL_IO_XIN_CHK(CountNum);	/*将状态值写入全局变量中*/
	}

	/*扫描单片机直连 输出GPIO*/
	for(CountNum=Cn40_5; CountNum<Cn28 ;CountNum++)
	{
		(p_HalGpio+CountNum)->OutValue == UpLevel? (CN_PORT[CountNum]->BSRRH=CN_PIN[CountNum]):(CN_PORT[CountNum]->BSRRL=CN_PIN[CountNum]);
//		if((p_HalGpio+CountNum)->OutValue == UpLevel)
//		{
//			HAL_IO_YOUTx_1(CountNum);
//		}
//		else
//		{
//			HAL_IO_YOUTx_0(CountNum);
//		}
	}
	/*扫描HC595扩展的输出GPIO*/
	{
		uint8_t HC595NumTemp = 0;
		for(CountNum=Cn40_4; CountNum<CnEnd ;CountNum++)
		{
			HC595NumTemp = CountNum-Cn40_4;
			(p_HalGpio+CountNum)->OutValue == UpLevel? HAL_IO_SET_COIL(HC595NumTemp):HAL_IO_CLR_COIL(HC595NumTemp);
	//		if((p_HalGpio+CountNum)->OutValue == UpLevel)
	//		{
	//			HAL_IO_SET_COIL(HC595NumTemp);
	//		}
	//		else
	//		{
	//			HAL_IO_CLR_COIL(HC595NumTemp);
	//		}
		}
	}

	/*更新HC595扩展的I/O端口*/
	HAL_IO_Hc595_FreshIO((uint8_t*)&(g_HalGpioTemp.HC595SendBuff[0]), EXT595_NUM);

	/*按照数量遍历GPIO对象列表，构建本次的输出管道和下一次的输入管道*/
	{
		uint8_t i = 0;
		for(i=0;i<g_HalGpioTemp.HalGpioNum;i++)
		{
			g_HalGpioTemp.p_Temp = p_HalGpio+(g_HalGpioTemp.p_HalGpioList[i]->Map);
			g_HalGpioTemp.p_Temp->OutValue = g_HalGpioTemp.p_HalGpioList[i]->OutValue;
			g_HalGpioTemp.p_HalGpioList[i]->InValue = g_HalGpioTemp.p_Temp->InValue;
		}
	}

}

/************************************END OF FILE************************************/
