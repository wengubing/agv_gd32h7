/**********************************************************************************************************
*
*	模块名称 : ADC转换底层驱动
*	文件名称 : drv_ADC.c
*	版    本 : V1.0
*	说    明 : 

**********************************************************************************************************/


#define		DRV_ADC_GLOBALS


/* Includes ------------------------------------------------------------------*/
#include "includes.h"

#include "..\..\Drivers\inc\drv_ADC.h"

/* Private typedef -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
DMA_Stream_TypeDef *DMA2_Streamx;
u32								 ADCx_DR_BaseAddr;




/**********************************************************************************************************
*	函 数 名: ADC_InitHard
*	功能说明: 
*	形    参:  无
*	返 回 值: 无
**********************************************************************************************************/
u8 ADC_InitHard(ADC_TypeDef* ADCx, u32 *pMemoryBaseAddr, u8 pi)
{
	u8 res=0;
	u32 DMA_Channel_chx;
	ADC_InitTypeDef 				ADC_IS;
	ADC_CommonInitTypeDef 	ADC_CS;
	NVIC_InitTypeDef				NVIC_IS;
	DMA_InitTypeDef 	DMA_IS;

	if(ADCx == ADC1)
	{
		DMA_Channel_chx = DMA_Channel_0;
		DMA2_Streamx = DMA2_Stream0;
		ADCx_DR_BaseAddr = ADC1_DR_BaseAddr;
	}
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//开ADCx时钟
	
	ADC_DeInit();
	
	ADC_IS.ADC_Resolution = ADC_Resolution_12b; //精度:12位								   
	ADC_IS.ADC_ScanConvMode = DISABLE;//扫描转换模式失能
	ADC_IS.ADC_ContinuousConvMode = ENABLE;//连续转换使能
	ADC_IS.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //不用外部触发,软件触发转换
	ADC_IS.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_IS.ADC_DataAlign = ADC_DataAlign_Right;	//数据右对齐,低字节对齐
	ADC_IS.ADC_NbrOfConversion = 1;//规定了顺序进行规则转换的ADC通道的数目
	ADC_Init(ADCx, &ADC_IS);						
	
	ADC_CS.ADC_Mode = ADC_Mode_Independent; //独立模式
	ADC_CS.ADC_Prescaler = ADC_Prescaler_Div4;	//分频为4
	ADC_CS.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //失能DMA_MODE
	ADC_CS.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;//两次采样间隔20个周期
	ADC_CommonInit(&ADC_CS);
	
	ADC_RegularChannelConfig(ADCx, ADC_Channel_16, 1, ADC_SampleTime_480Cycles);//规则通道配置，1表示规则组采样顺序
	
	ADC_TempSensorVrefintCmd(ENABLE);//使能温度传感器的基准电源 
	
	ADC_Cmd(ADCx, ENABLE);//使能ADCx
	
	ADC_DMACmd(ADCx, ENABLE);//使能ADCx的DMA
	
	ADC_DMARequestAfterLastTransferCmd(ADCx, ENABLE); //单通道模式下上次转换完成后DMA请求允许，也就是持续DMA

	NVIC_IS.NVIC_IRQChannel = ADC_IRQn; 
	NVIC_IS.NVIC_IRQChannelPreemptionPriority = pi;//(0-15)
	NVIC_IS.NVIC_IRQChannelSubPriority = 0;
	NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_IS);  

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//开启DMA时钟
	
	DMA_DeInit(DMA2_Streamx);
	DMA_StructInit( &DMA_IS);
	DMA_IS.DMA_Channel = DMA_Channel_chx;//通道指定
	DMA_IS.DMA_PeripheralBaseAddr = ADCx_DR_BaseAddr;//数据传输的外设首地址，详解见上
	DMA_IS.DMA_Memory0BaseAddr = (u32)pMemoryBaseAddr;//自己定义待发送数组的首地址，要强制转换为32位
	DMA_IS.DMA_DIR = DMA_DIR_PeripheralToMemory;	  //数据传输方向选择为内存->外设
	DMA_IS.DMA_BufferSize = 1;//传输数据大小为16，单位由以下确定，大小要配合定义的数组类型和外设数据类型
	DMA_IS.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器自动增加禁止，因为这里只用到了DR数据寄存器
	DMA_IS.DMA_MemoryInc = DMA_MemoryInc_Disable;//内存地址自增不允许，因为只存取一个数
	DMA_IS.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //外设的数据大小
	DMA_IS.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	//这里也选Byte
	DMA_IS.DMA_Mode = DMA_Mode_Circular;//DMA传输模式为Normal，如果为Circular,将会循环传输
	DMA_IS.DMA_Priority = DMA_Priority_High;//优先级
	
	DMA_Init(DMA2_Streamx, &DMA_IS);
	DMA_Cmd(DMA2_Streamx, ENABLE);//使能DMA2_Streamx通道

	NVIC_IS.NVIC_IRQChannel = DMA2_Stream0_IRQn; 
	NVIC_IS.NVIC_IRQChannelPreemptionPriority = pi;//(0-15)
	NVIC_IS.NVIC_IRQChannelSubPriority = 0;
	NVIC_IS.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_IS);  
	
	/* DMA中断使能 */
	DMA_ITConfig(DMA2_Streamx, DMA_IT_TC, ENABLE);

	ADC_SoftwareStartConv(ADCx);//开始转换
	
	return res;
}

//DMA中断
void DMA2_Stream0_IRQHandler(void)
{
  if (DMA_GetITStatus(DMA2_Streamx, DMA_IT_TCIF0) != RESET)
	{
		DMA_ClearITPendingBit(DMA2_Streamx, DMA_IT_TCIF0);
		//更新用户程序数据

	}
}

//ADC看门狗中断服务程序,ADC输入超过界限产生中断
void ADC_IRQHandler(void)
{
  if (ADC_GetITStatus(ADC3, ADC_IT_AWD) == SET)
	{
		 ADC_ClearITPendingBit(ADC3, ADC_IT_AWD);
		 ADC_Cmd(ADC3, DISABLE);

	}
}

/***************************** hangfa (END OF FILE) *********************************/
