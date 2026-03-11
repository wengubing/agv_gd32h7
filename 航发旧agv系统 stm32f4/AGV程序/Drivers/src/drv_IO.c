/**********************************************************************************************************
*
*	模块名称 : 输入/输出驱动
*	文件名称 : drv_IO.c
*	版    本 : V1.0.3
*	说    明 : 相关的使用到的IO控制信号GPIO外设初始化及GPIO状态控制

**********************************************************************************************************/


#define		IO_GLOBALS


/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "drv_IO.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
GPIO_TypeDef    *YO_PORT[YOMAXCNT]	= {
																				GPIO_PORT_YOUT0,
																				GPIO_PORT_YOUT1,
																				GPIO_PORT_YOUT2,
																				GPIO_PORT_YOUT3,
																				GPIO_PORT_YOUT4,
																				GPIO_PORT_YOUT5,
																				GPIO_PORT_YOUT6,
																				GPIO_PORT_YOUT7,
																				GPIO_PORT_YOUT8,
																				GPIO_PORT_YOUT9,
																				GPIO_PORT_YOUT10,
																				GPIO_PORT_YOUT11,
																				GPIO_PORT_YOUT12,
																				GPIO_PORT_YOUT13,
																				GPIO_PORT_YOUT14,
																				GPIO_PORT_YOUT15,
																				GPIO_PORT_YOUT16,
																				GPIO_PORT_YOUT17,
																				GPIO_PORT_YOUT18,
																				GPIO_PORT_YOUT19,
																				GPIO_PORT_YOUT20,
																				GPIO_PORT_YOUT21,
																				GPIO_PORT_YOUT22,
																				GPIO_PORT_YOUT23,
																				GPIO_PORT_YOUT24,
																				GPIO_PORT_YOUT25,
																				GPIO_PORT_YOUT26,
																				GPIO_PORT_YOUT27,
																				GPIO_PORT_YOUT28,
																				GPIO_PORT_YOUT29,
																				GPIO_PORT_YOUT30,
																				GPIO_PORT_YOUT31
};

const u32    YO_PIN[YOMAXCNT] = {
																				GPIO_PIN_YOUT0,
																				GPIO_PIN_YOUT1,
																				GPIO_PIN_YOUT2,
																				GPIO_PIN_YOUT3,
																				GPIO_PIN_YOUT4,
																				GPIO_PIN_YOUT5,
																				GPIO_PIN_YOUT6,
																				GPIO_PIN_YOUT7,
																				GPIO_PIN_YOUT8,
																				GPIO_PIN_YOUT9,
																				GPIO_PIN_YOUT10,
																				GPIO_PIN_YOUT11,
																				GPIO_PIN_YOUT12,
																				GPIO_PIN_YOUT13,
																				GPIO_PIN_YOUT14,
																				GPIO_PIN_YOUT15,
																				GPIO_PIN_YOUT16,
																				GPIO_PIN_YOUT17,
																				GPIO_PIN_YOUT18,
																				GPIO_PIN_YOUT19,
																				GPIO_PIN_YOUT20,
																				GPIO_PIN_YOUT21,
																				GPIO_PIN_YOUT22,
																				GPIO_PIN_YOUT23,
																				GPIO_PIN_YOUT24,
																				GPIO_PIN_YOUT25,
																				GPIO_PIN_YOUT26,
																				GPIO_PIN_YOUT27,
																				GPIO_PIN_YOUT28,
																				GPIO_PIN_YOUT29,
																				GPIO_PIN_YOUT30,
																				GPIO_PIN_YOUT31
};

GPIO_TypeDef    *XIN_PORT[XINMAXCNT]	= {
																				GPIO_PORT_XIN8,
																				GPIO_PORT_XIN9,
																				GPIO_PORT_XIN10,
																				GPIO_PORT_XIN11,
																				GPIO_PORT_XIN12,
																				GPIO_PORT_XIN13,
																				GPIO_PORT_XIN14,
																				GPIO_PORT_XIN15,
																				GPIO_PORT_XIN16,
																				GPIO_PORT_XIN17,
																				GPIO_PORT_XIN18,
																				GPIO_PORT_XIN19,
																				GPIO_PORT_XIN20,
																				GPIO_PORT_XIN21,
																				GPIO_PORT_XIN22,
																				GPIO_PORT_XIN23,
																				GPIO_PORT_XIN24,
																				GPIO_PORT_XIN25,
																				GPIO_PORT_XIN26,
																				GPIO_PORT_XIN27,
																				GPIO_PORT_XIN28,
																				GPIO_PORT_XIN29,
																				GPIO_PORT_XIN30,
																				GPIO_PORT_XIN31,
																				GPIO_PORT_XIN32,
																				GPIO_PORT_XIN33,
																				GPIO_PORT_XIN34,
																				GPIO_PORT_XIN35,
																				GPIO_PORT_XIN36,
																				GPIO_PORT_XIN37,
																				GPIO_PORT_XIN38,
																				GPIO_PORT_XIN39,
																				GPIO_PORT_XIN40,
																				GPIO_PORT_XIN41,
																				GPIO_PORT_XIN42,
																				GPIO_PORT_XIN43,
																				GPIO_PORT_XIN44,
																				GPIO_PORT_XIN45,
																				GPIO_PORT_XIN46,
																				GPIO_PORT_XIN47,
																				GPIO_PORT_XIN48,
																				GPIO_PORT_XIN49,
																				GPIO_PORT_XIN50,
																				GPIO_PORT_XIN51
};

const u32    XIN_PIN[XINMAXCNT] = {
																				GPIO_PIN_XIN8,
																				GPIO_PIN_XIN9,
																				GPIO_PIN_XIN10,
																				GPIO_PIN_XIN11,
																				GPIO_PIN_XIN12,
																				GPIO_PIN_XIN13,
																				GPIO_PIN_XIN14,
																				GPIO_PIN_XIN15,
																				GPIO_PIN_XIN16,
																				GPIO_PIN_XIN17,
																				GPIO_PIN_XIN18,
																				GPIO_PIN_XIN19,
																				GPIO_PIN_XIN20,
																				GPIO_PIN_XIN21,
																				GPIO_PIN_XIN22,
																				GPIO_PIN_XIN23,
																				GPIO_PIN_XIN24,
																				GPIO_PIN_XIN25,
																				GPIO_PIN_XIN26,
																				GPIO_PIN_XIN27,
																				GPIO_PIN_XIN28,
																				GPIO_PIN_XIN29,
																				GPIO_PIN_XIN30,
																				GPIO_PIN_XIN31,
																				GPIO_PIN_XIN32,
																				GPIO_PIN_XIN33,
																				GPIO_PIN_XIN34,
																				GPIO_PIN_XIN35,
																				GPIO_PIN_XIN36,
																				GPIO_PIN_XIN37,
																				GPIO_PIN_XIN38,
																				GPIO_PIN_XIN39,
																				GPIO_PIN_XIN40,
																				GPIO_PIN_XIN41,
																				GPIO_PIN_XIN42,
																				GPIO_PIN_XIN43,
																				GPIO_PIN_XIN44,
																				GPIO_PIN_XIN45,
																				GPIO_PIN_XIN46,
																				GPIO_PIN_XIN47,
																				GPIO_PIN_XIN48,
																				GPIO_PIN_XIN49,
																				GPIO_PIN_XIN50,
																				GPIO_PIN_XIN51
};


u16 YoLsTemp[YOUT_DHR_NUM_LS];

u16 YoHsTemp[YOUT_DHR_NUM_HS];



/**********************************************************************************************************
*	函 数 名: YOUTx_Turn
*	功能说明: 选中GPIO输出翻转
*	形    参: YOx
*	返 回 值: 无
**********************************************************************************************************/
void YOUTx_Turn(YOUT_ID YOx)
{
	YO_PORT[YOx]->ODR^=YO_PIN[YOx];
}

/**********************************************************************************************************
*	函 数 名: YOUT_0
*	功能说明: 选中GPIO输出"0"
*	形    参: YOx
*	返 回 值: 无
**********************************************************************************************************/
void YOUTx_0(YOUT_ID YOx)
{
	YO_PORT[YOx]->BSRRH=YO_PIN[YOx];
}

/**********************************************************************************************************
*	函 数 名: YOUT_1
*	功能说明: 选中GPIO输出"1"
*	形    参: YOx
*	返 回 值: 无
**********************************************************************************************************/
void YOUTx_1(YOUT_ID YOx)
{
	YO_PORT[YOx]->BSRRL=YO_PIN[YOx];
}

/**********************************************************************************************************
*	函 数 名: XIN_CHK
*	功能说明: 
*	形    参: 
*	返 回 值: 1:检测高电平"1"，0:检测到低电平"0";
**********************************************************************************************************/
u8 XIN_CHK(XIN_ID XINx)
{
	if ((XIN_PORT[XINx]->IDR & XIN_PIN[XINx]) != 0)
		return 1;
	else
		return 0;
}

/**********************************************************************************************************
*	函 数 名: XIN_CHK
*	功能说明: 初始化GPIO扩展IC
*	形    参: None
*	返 回 值: None
**********************************************************************************************************/
void Ini_HC595(void)
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
void Fuc_Hc595_SendByte(u8 dat)
{
	u8 i;
	
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
void Fuc_Hc595_FreshIO(u8 *dat, u8 len)
{
	u8 i;

	if(!dat && !len)
		return;
	
	RCK_L;//拉低
//	Delay_1us(1);

	for(i=len;i>0;i--)
	{
		//dat1,dat2.....,datn
		Fuc_Hc595_SendByte(*(dat+i-1));//先传送靠后的字节数据:datn
	}
	
	SI_H;
	SCK_H;
	RCK_H;//上升沿,移位寄存器的数据进入数据存储寄存器
}

/**********************************************************************************************************
*	函 数 名: Fuc_All_FreshIO()
*	功能说明: 刷新所有输出IO
*	形    参: None
*	返 回 值: None
**********************************************************************************************************/
void Fuc_All_FreshIO(u16 *pDat, u8 ClrAll)
{
	u8 i,temp=YOUT_DHR_NUM_LS;
	BYTE4_UNION *pB4U;
	
	/*输出特殊命令*/
	if(ClrAll!=0x00)
	{
		if(ClrAll==0xAA)
		{//全关
			for(i=0;i<temp;i++)
			{
				pDat[i]=0x0000;
			}
		}
		else if(ClrAll==0x5A)
		{//全开
			for(i=0;i<temp;i++)
			{
				pDat[i]=0xFFFF;
			}
		}
		
		/*更新DHR的状态*/
		for(i=0;i<YOUT_DHR_NUM_LS;i++)
		{
			IO_DHR[RW_BIT_YOUT_STATUS+i]=pDat[i];
		}
	}
	
	/*先更新前28个mcu_gpio端口输出数据*/
	pB4U=(BYTE4_UNION *)pDat;
	for(i=YO4;i<=YO31;i++)
	{
#if (GPIO_Y10_FUNC == 1)
		if(i==YO10)
			continue;
#endif
		if((pB4U->B4_U32>>i) & 0x0001)
			YOUTx_1((YOUT_ID)i);
		else
			YOUTx_0((YOUT_ID)i);
	}
	
	/*更新HC595扩展的I/O端口*/
	Fuc_Hc595_FreshIO((u8 *)&pDat[2], EXT595_NUM);
}



/**********************************************************************************************************
*	函 数 名: Tsk_GpioOutputControl
*	功能说明: Gpio输出控制
*	形    参: None
*	返 回 值: None
**********************************************************************************************************/
void Tsk_GpioOutputControl(void)
{
	static u8 YoStep=0;
	u16 i,YOUT_LS_CompareDifferentCnt=0,*pu16=0;
	
	switch (YoStep)
	{
		case 0:
			{
				/*判断是否有参数更改*/
				pu16=&IO_DHR[RW_BIT_YOUT_STATUS];
				for(i=0;i<YOUT_DHR_NUM_LS;i++)
				{
					if(pu16[i] != YoLsTemp[i])
					{//有设置参数更改，立即更新输出缓冲区数据
						YoLsTemp[i]=pu16[i];
						YOUT_LS_CompareDifferentCnt++;
					}
				}

				if(YOUT_LS_CompareDifferentCnt>0)
				{
					/*命令触发方式,更新输出逻辑量*/
					Fuc_All_FreshIO(YoLsTemp, 0x00);
				}

			}
			break;
		case 1://全开
			{
					Fuc_All_FreshIO(YoLsTemp, 0x5A);
					YoStep=0;
			}
			break;
		case 2://全关
			{
					Fuc_All_FreshIO(YoLsTemp, 0xAA);
					YoStep=0;
			}
			break;
		default:
			break;
	}
}

/**********************************************************************************************************
*	函 数 名: Tsk_GpioInputScan
*	功能说明: gpio输入状态检测,周期性调用检测
*	形    参: None
*	返 回 值: None
**********************************************************************************************************/
void Tsk_GpioInputScan(void)
{
	u16 i;
	
	for(i=B0;i<XINMAXCNT;i++)
	{
		/*将状态值写入DHR中*/
		if(XIN_CHK((XIN_ID)i)!=0)
			SET_MODBUS_COIL(i);
		else
			CLR_MODBUS_COIL(i);
	}
	
//	Button_Scan();
	
}


/**********************************************************************************************************
*	函 数 名: IO_InitHard
*	功能说明: 配置IO相关GPIO
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void IO_InitHard(void)
{
	GPIO_InitTypeDef GPIO_IS;
	EXTI_InitTypeDef EXTI_IS;
	NVIC_InitTypeDef NVIC_IS;

	u8 config=0;
	
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	 //重映射（第1路）
	
	/*配置input管脚*/
	//配置4路高速输入管脚
	
	//配置普通输入管脚-硬件配置
	RCC_AHB1PeriphClockCmd( GPIO_CLK_XIN8 | 
													GPIO_CLK_XIN9 |
													GPIO_CLK_XIN10 |
													GPIO_CLK_XIN11 |
													GPIO_CLK_XIN12 |
													GPIO_CLK_XIN13 |
													GPIO_CLK_XIN14 |
													GPIO_CLK_XIN15 |
													GPIO_CLK_XIN16 |
													GPIO_CLK_XIN17 |
													GPIO_CLK_XIN18 |
													GPIO_CLK_XIN19 |
													GPIO_CLK_XIN20 |
													GPIO_CLK_XIN21 |
													GPIO_CLK_XIN22 |
													GPIO_CLK_XIN23 |
													GPIO_CLK_XIN24 |
													GPIO_CLK_XIN25 |
													GPIO_CLK_XIN26 |
													GPIO_CLK_XIN27 |
													GPIO_CLK_XIN28 |
													GPIO_CLK_XIN29 |
													GPIO_CLK_XIN30 |
													GPIO_CLK_XIN31 |
													GPIO_CLK_XIN32 |
													GPIO_CLK_XIN33 |
													GPIO_CLK_XIN34 |
													GPIO_CLK_XIN35 |
													GPIO_CLK_XIN36 |
													GPIO_CLK_XIN37 |
													GPIO_CLK_XIN38 |
													GPIO_CLK_XIN39 |
													GPIO_CLK_XIN40 |
													GPIO_CLK_XIN41 |
													GPIO_CLK_XIN42 |
													GPIO_CLK_XIN43 |
													GPIO_CLK_XIN44 |
													GPIO_CLK_XIN45 |
													GPIO_CLK_XIN46 |
													GPIO_CLK_XIN47 |
													GPIO_CLK_XIN48 |
													GPIO_CLK_XIN49 |
													GPIO_CLK_XIN50 |
													GPIO_CLK_XIN51, ENABLE);
	GPIO_IS.GPIO_Mode = GPIO_Mode_IN;				//设为输入模式
	GPIO_IS.GPIO_OType = GPIO_OType_PP;			//设为推挽类型
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_DOWN;			//下拉电阻使能
	GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;	//IO口最大速度

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN8;
	GPIO_Init(GPIO_PORT_XIN8, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN9;
	GPIO_Init(GPIO_PORT_XIN9, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN10;
	GPIO_Init(GPIO_PORT_XIN10, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN11;
	GPIO_Init(GPIO_PORT_XIN11, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN12;
	GPIO_Init(GPIO_PORT_XIN12, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN13;
	GPIO_Init(GPIO_PORT_XIN13, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN14;
	GPIO_Init(GPIO_PORT_XIN14, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN15;
	GPIO_Init(GPIO_PORT_XIN15, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN16;
	GPIO_Init(GPIO_PORT_XIN16, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN17;
	GPIO_Init(GPIO_PORT_XIN17, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN18;
	GPIO_Init(GPIO_PORT_XIN18, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN19;
	GPIO_Init(GPIO_PORT_XIN19, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN20;
	GPIO_Init(GPIO_PORT_XIN20, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN21;
	GPIO_Init(GPIO_PORT_XIN21, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN22;
	GPIO_Init(GPIO_PORT_XIN22, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN23;
	GPIO_Init(GPIO_PORT_XIN23, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN24;
	GPIO_Init(GPIO_PORT_XIN24, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN25;
	GPIO_Init(GPIO_PORT_XIN25, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN26;
	GPIO_Init(GPIO_PORT_XIN26, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN27;
	GPIO_Init(GPIO_PORT_XIN27, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN28;
	GPIO_Init(GPIO_PORT_XIN28, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN29;
	GPIO_Init(GPIO_PORT_XIN29, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN30;
	GPIO_Init(GPIO_PORT_XIN30, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN31;
	GPIO_Init(GPIO_PORT_XIN31, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN32;
	GPIO_Init(GPIO_PORT_XIN32, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN33;
	GPIO_Init(GPIO_PORT_XIN33, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN34;
	GPIO_Init(GPIO_PORT_XIN34, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN35;
	GPIO_Init(GPIO_PORT_XIN35, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN36;
	GPIO_Init(GPIO_PORT_XIN36, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN37;
	GPIO_Init(GPIO_PORT_XIN37, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN38;
	GPIO_Init(GPIO_PORT_XIN38, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN39;
	GPIO_Init(GPIO_PORT_XIN39, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN40;
	GPIO_Init(GPIO_PORT_XIN40, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN41;
	GPIO_Init(GPIO_PORT_XIN41, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN42;
	GPIO_Init(GPIO_PORT_XIN42, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN43;
	GPIO_Init(GPIO_PORT_XIN43, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN44;
	GPIO_Init(GPIO_PORT_XIN44, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN45;
	GPIO_Init(GPIO_PORT_XIN45, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN46;
	GPIO_Init(GPIO_PORT_XIN46, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN47;
	GPIO_Init(GPIO_PORT_XIN47, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN48;
	GPIO_Init(GPIO_PORT_XIN48, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN49;
	GPIO_Init(GPIO_PORT_XIN49, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN50;
	GPIO_Init(GPIO_PORT_XIN50, &GPIO_IS);

	GPIO_IS.GPIO_Pin = GPIO_PIN_XIN51;
	GPIO_Init(GPIO_PORT_XIN51, &GPIO_IS);

	/*配置XIN16、XIN17的EXTI检测中断*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟

	SYSCFG_EXTILineConfig(EXTI_PortSource_XIN16, EXTI_PinSource_XIN16);	//连接 EXTI Line9到 XIN16 引脚
	SYSCFG_EXTILineConfig(EXTI_PortSource_XIN17, EXTI_PinSource_XIN17);	//连接 EXTI Line10到 XIN17 引脚
	SYSCFG_EXTILineConfig(EXTI_PortSource_XIN18, EXTI_PinSource_XIN18);	//连接 EXTI Line11到 XIN18 引脚
	
	EXTI_IS.EXTI_Line = EXTI_Line9 | EXTI_Line10;//gpio外中断9-10
	EXTI_IS.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_IS.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
	EXTI_IS.EXTI_LineCmd = ENABLE;//使能相应的中断线
	EXTI_Init(&EXTI_IS);
	
	EXTI_IS.EXTI_Line = EXTI_Line11;//gpio外中断11
	EXTI_IS.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
	EXTI_Init(&EXTI_IS);

	NVIC_IS.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_IS.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_IS.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能	

	NVIC_IS.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_IS);

	NVIC_IS.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Init(&NVIC_IS);
	
	/*X0-X2、X4-X6高速输入配置(编码器接口)*/
	////Encoder_Init(g_pBoardpara[Para_Index_EncoderALines]);

	/*配置output管脚*/
	RCC_AHB1PeriphClockCmd(
													GPIO_CLK_YOUT4 |
													GPIO_CLK_YOUT5 |
													GPIO_CLK_YOUT6 |
													GPIO_CLK_YOUT7 |
													GPIO_CLK_YOUT8 |
													GPIO_CLK_YOUT9 |
													GPIO_CLK_YOUT10 |
													GPIO_CLK_YOUT11 |
													GPIO_CLK_YOUT12 |
													GPIO_CLK_YOUT13 |
													GPIO_CLK_YOUT14 |
													GPIO_CLK_YOUT15 |
													GPIO_CLK_YOUT16 |
													GPIO_CLK_YOUT17 |
													GPIO_CLK_YOUT18 |
													GPIO_CLK_YOUT19 |
													GPIO_CLK_YOUT20 |
													GPIO_CLK_YOUT21 |
													GPIO_CLK_YOUT22 |
													GPIO_CLK_YOUT23 |
													GPIO_CLK_YOUT24 |
													GPIO_CLK_YOUT25 |
													GPIO_CLK_YOUT26 |
													GPIO_CLK_YOUT27 |
													GPIO_CLK_YOUT28 |
													GPIO_CLK_YOUT29 |
													GPIO_CLK_YOUT30 |
													GPIO_CLK_YOUT31, ENABLE);

	GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;			//设为输出口
	GPIO_IS.GPIO_OType = GPIO_OType_PP;			//设为推挽模式
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_DOWN;			//下拉使能
	GPIO_IS.GPIO_Speed = GPIO_Speed_25MHz;	//IO口最大速度
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT4;
	GPIO_Init(GPIO_PORT_YOUT4, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT5;
	GPIO_Init(GPIO_PORT_YOUT5, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT6;
	GPIO_Init(GPIO_PORT_YOUT6, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT7;
	GPIO_Init(GPIO_PORT_YOUT7, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT8;
	GPIO_Init(GPIO_PORT_YOUT8, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT9;
	GPIO_Init(GPIO_PORT_YOUT9, &GPIO_IS);

#if (!GPIO_Y10_FUNC)

	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT10;
	GPIO_Init(GPIO_PORT_YOUT10, &GPIO_IS);

#endif
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT11;
	GPIO_Init(GPIO_PORT_YOUT11, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT12;
	GPIO_Init(GPIO_PORT_YOUT12, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT13;
	GPIO_Init(GPIO_PORT_YOUT13, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT14;
	GPIO_Init(GPIO_PORT_YOUT14, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT15;
	GPIO_Init(GPIO_PORT_YOUT15, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT16;
	GPIO_Init(GPIO_PORT_YOUT16, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT17;
	GPIO_Init(GPIO_PORT_YOUT17, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT18;
	GPIO_Init(GPIO_PORT_YOUT18, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT19;
	GPIO_Init(GPIO_PORT_YOUT19, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT20;
	GPIO_Init(GPIO_PORT_YOUT20, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT21;
	GPIO_Init(GPIO_PORT_YOUT21, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT22;
	GPIO_Init(GPIO_PORT_YOUT22, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT23;
	GPIO_Init(GPIO_PORT_YOUT23, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT24;
	GPIO_Init(GPIO_PORT_YOUT24, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT25;
	GPIO_Init(GPIO_PORT_YOUT25, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT26;
	GPIO_Init(GPIO_PORT_YOUT26, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT27;
	GPIO_Init(GPIO_PORT_YOUT27, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT28;
	GPIO_Init(GPIO_PORT_YOUT28, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT29;
	GPIO_Init(GPIO_PORT_YOUT29, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT30;
	GPIO_Init(GPIO_PORT_YOUT30, &GPIO_IS);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT31;
	GPIO_Init(GPIO_PORT_YOUT31, &GPIO_IS);

	/*配置74HC595扩展IO的控制端口*/
	RCC_AHB1PeriphClockCmd(EXTIO_CLK_SI|EXTIO_CLK_RCK|EXTIO_CLK_SCK|EXTIO_CLK_OE, ENABLE);              
	GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;//设为输出口
	GPIO_IS.GPIO_OType = GPIO_OType_PP;//设为推挽模式
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_UP;//上拉使能
	GPIO_IS.GPIO_Speed = GPIO_Speed_100MHz;	//IO口最大速度

	GPIO_IS.GPIO_Pin = EXTIO_PIN_SI;
	GPIO_Init(EXTIO_PORT_SI, &GPIO_IS);

	GPIO_IS.GPIO_Pin = EXTIO_PIN_RCK;
	GPIO_Init(EXTIO_PORT_RCK, &GPIO_IS);

	GPIO_IS.GPIO_Pin = EXTIO_PIN_SCK;
	GPIO_Init(EXTIO_PORT_SCK, &GPIO_IS);
    
    GPIO_IS.GPIO_Pin = EXTIO_PIN_OE;                 
	GPIO_Init(EXTIO_PORT_OE, &GPIO_IS);

#if (Motor_CtrlMode==0 || Motor_CtrlMode==1)	//PWM+方向 或 频率+方向

	/*Y0-Y3高速输出配置*/
	RCC_AHB1PeriphClockCmd(GPIO_CLK_YOUT0 | GPIO_CLK_YOUT1 | GPIO_CLK_YOUT2 | GPIO_CLK_YOUT3, ENABLE);
	GPIO_IS.GPIO_Mode = GPIO_Mode_AF;//设为复用
	GPIO_IS.GPIO_OType = GPIO_OType_PP;//设为推挽模式
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_DOWN;//上拉使能
	GPIO_IS.GPIO_Speed = GPIO_Speed_100MHz;	//IO口最大速度

	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT0;
	GPIO_Init(GPIO_PORT_YOUT0, &GPIO_IS);
  GPIO_PinAFConfig(GPIO_PORT_YOUT0,GPIO_SOURCE_YOUT0,GPIO_AF_YOUT0);//复用功能指定...

	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT1;
	GPIO_Init(GPIO_PORT_YOUT1, &GPIO_IS);
  GPIO_PinAFConfig(GPIO_PORT_YOUT1,GPIO_SOURCE_YOUT1,GPIO_AF_YOUT1);

	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT2;
	GPIO_Init(GPIO_PORT_YOUT2, &GPIO_IS);
  GPIO_PinAFConfig(GPIO_PORT_YOUT2,GPIO_SOURCE_YOUT2,GPIO_AF_YOUT2);
	
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT3;
	GPIO_Init(GPIO_PORT_YOUT3, &GPIO_IS);
  GPIO_PinAFConfig(GPIO_PORT_YOUT3,GPIO_SOURCE_YOUT3,GPIO_AF_YOUT3);
	
#if (Motor_CtrlMode==0)	//脉宽控制速度模式

	config = 0;
	
#elif (Motor_CtrlMode==1)	//频率控制位置模式

	config = 1;
	
#endif

	//输出脉冲频率f=1000/toc,unit:Khz---2.5K可以获得较大的ARR
	//脉冲周期t=toc,unit:us  250
	Timer_init(Y0TimerSet, 400,//toc
															-3, &Period_ARR[YO0], &Prescaler_PSC[YO0], ENABLE, Y0Timer_pwm_CHx, config);//不开启nvic
	Timer_init(Y1TimerSet, 400, 
															-3, &Period_ARR[YO1], &Prescaler_PSC[YO1], ENABLE, Y1Timer_pwm_CHx, config);
	Timer_init(Y2TimerSet, 400,  
															-3, &Period_ARR[YO2], &Prescaler_PSC[YO2], ENABLE, Y2Timer_pwm_CHx, config);
	Timer_init(Y3TimerSet, 400,  
															-3, &Period_ARR[YO3], &Prescaler_PSC[YO3], ENABLE, Y3Timer_pwm_CHx, config);
															

#if (GPIO_Y10_FUNC == 1)

	//2015.10.30增加非高速输出端口,-Y10--1.25K
	RCC_AHB1PeriphClockCmd(GPIO_CLK_YOUT10, ENABLE);
	GPIO_IS.GPIO_Mode = GPIO_Mode_AF;//设为复用
	GPIO_IS.GPIO_OType = GPIO_OType_PP;//设为推挽模式
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_DOWN;//上拉使能
	GPIO_IS.GPIO_Speed = GPIO_Speed_2MHz;	//IO口低速
	GPIO_IS.GPIO_Pin = GPIO_PIN_YOUT10;
	GPIO_Init(GPIO_PORT_YOUT10, &GPIO_IS);
  GPIO_PinAFConfig(GPIO_PORT_YOUT10, GPIO_SOURCE_YOUT10, GPIO_AF_YOUT10);//复用功能指定...
	//819
	Timer_init(Y10TimerSet, 800, -3, &Period_ARR[4], &Prescaler_PSC[4], ENABLE, Y10Timer_pwm_CHx, config);//其映射通道为：TIM5_CH1

#endif

#endif


	/*IO输出状态初始化配置*/
	Ini_HC595();
	
	Fuc_All_FreshIO(YoLsTemp, 0xAA);
	
	/*配置EN_232_SEL引脚*/
	RCC_AHB1PeriphClockCmd(RCC_EN_232_SEL, ENABLE);
	GPIO_IS.GPIO_Mode = GPIO_Mode_OUT;			//设为输出口
	GPIO_IS.GPIO_OType = GPIO_OType_PP;			//设为推挽模式
	GPIO_IS.GPIO_PuPd = GPIO_PuPd_NOPULL;		//外部有上拉电阻
	GPIO_IS.GPIO_Speed = GPIO_Speed_50MHz;	//IO口最大速度

	GPIO_IS.GPIO_Pin = GPIO_PIN_EN_232_SEL;
	GPIO_Init(GPIO_PORT_EN_232_SEL, &GPIO_IS);

}
/**********************************************************************************************************
*	函 数 名: SysPowerDownProcess
*	功能说明: 系统关电相关处理
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void SysPowerDownProcess(u8 event)
{
	while(1)
	{
		Fuc_All_FreshIO(YoLsTemp, 0xAA);
	}
}



/**********************************************************************************************************
*	函 数 名: EXTI9_5_IRQHandler
*	功能说明: 外部中断服务程序
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/
void EXTI9_5_IRQHandler(void)
{
	
	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
	{//ADC1_EOCINT上升沿到来(转换完成)!!!
		EXTI_ClearITPendingBit(EXTI_Line8);//清除中断标志位
		ADC_EOC_Flag=1;
	}	
	if(EXTI_GetITStatus(EXTI_Line9) != RESET)
	{	
		//EXTI->IMR&=~(1<<9);//关闭中断
		EXTI_ClearITPendingBit(EXTI_Line9);//清除中断标志位
	}
}

/**********************************************************************************************************
*	函 数 名: EXTI15_10_IRQHandler
*	功能说明: 外部中断服务程序
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/
void EXTI15_10_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line10) != RESET)
	{	
		//EXTI->IMR&=~(1<<10);//关闭中断
		EXTI_ClearITPendingBit(EXTI_Line10);//清除中断标志位
		
	}
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)
	{//检测到主开关关闭!!!
		//EXTI->IMR&=~(1<<11);//关闭中断
		EXTI_ClearITPendingBit(EXTI_Line11);//清除中断标志位
		//SysPowerDownProcess(0);
	}
}

/**********************************************************************************************************
*	函 数 名: Y0Timer_IrqHandler
*	功能说明: Y0输出定时器中断程序
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/
void Y0Timer_IrqHandler(void)
{
	if ( TIM_GetITStatus( Y0Timer_Base, TIM_IT_Update ) != RESET )
	{

	}
}

/**********************************************************************************************************
*	函 数 名: Y0Timer_IrqHandler
*	功能说明: Y0输出定时器中断程序
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/
void Y1Timer_IrqHandler(void)
{
	if ( TIM_GetITStatus( Y1Timer_Base, TIM_IT_Update ) != RESET )
	{

	}
}

/**********************************************************************************************************
*	函 数 名: Y0Timer_IrqHandler
*	功能说明: Y0输出定时器中断程序
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/
void Y2Timer_IrqHandler(void)
{
	if ( TIM_GetITStatus( Y2Timer_Base, TIM_IT_Update ) != RESET )
	{

	}
}

/**********************************************************************************************************
*	函 数 名: Y0Timer_IrqHandler
*	功能说明: Y0输出定时器中断程序
*	形    参：无
*	返 回 值: 无
**********************************************************************************************************/
void Y3Timer_IrqHandler(void)
{
	if ( TIM_GetITStatus( Y3Timer_Base, TIM_IT_Update ) != RESET )
	{

	}
}

/**********************************************************************************************************
*	函 数 名: Y10Timer_IrqHandler
*	功能说明: Y0输出定时器中断程序
*	形    参：无Y10Timer_IrqHandler
*	返 回 值: 无
**********************************************************************************************************/
void Y10Timer_IrqHandler(void)
{
	if ( TIM_GetITStatus( Y10Timer_Base, TIM_IT_Update ) != RESET )
	{

	}
}


/***************************** hangfa (END OF FILE) *********************************/
