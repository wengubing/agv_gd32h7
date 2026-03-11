

#ifndef __DRV_ADC_H
#define __DRV_ADC_H





#ifdef		DRV_ADC_GLOBALS
	#define DRV_ADC_EXT
#else
	#define DRV_ADC_EXT  extern
#endif




/* Private define ------------------------------------------------------------*/


//外设基地址定义
#define		ADC1_DR_BaseAddr		((u32)0x4001204C)




#define		ADC_COV_CHX					1		//ADC转换的通道数






/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


//ADC数据结构体
typedef struct
{
	u32	ADC_Cov_data[ADC_COV_CHX];
	
}ADC_Typedef;




DRV_ADC_EXT		ADC_Typedef		ST_ADC;





DRV_ADC_EXT u8 ADC_InitHard(ADC_TypeDef* ADCx, u32 *pMemoryBaseAddr, u8 pi);






#endif


