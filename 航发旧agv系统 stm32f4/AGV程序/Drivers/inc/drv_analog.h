

#ifndef __DRVANALOG_H
#define __DRVANALOG_H




#ifdef		ANALOG_GLOBALS
	#define ANALOG_EXT
#else
	#define ANALOG_EXT  extern
#endif

/* Private define ------------------------------------------------------------*/

//相关GPIO定义
#define DAC_LDAC_GPIO_PORT			GPIOB
#define DAC_LDAC_GPIO_PIN				GPIO_Pin_0
#define DAC_LDAC_GPIO_CLK				RCC_AHB1Periph_GPIOB
#define	DAC_LDAC_GPIO						DAC_LDAC_GPIO_PORT, DAC_LDAC_GPIO_PIN
	
#define DAC_FS_GPIO_PORT				GPIOF
#define DAC_FS_GPIO_PIN					GPIO_Pin_5
#define DAC_FS_GPIO_CLK					RCC_AHB1Periph_GPIOF
#define	DAC_FS_GPIO							DAC_FS_GPIO_PORT, DAC_FS_GPIO_PIN
	
#define DAC1_CS_GPIO_PORT				GPIOB
#define DAC1_CS_GPIO_PIN				GPIO_Pin_1
#define DAC1_CS_GPIO_CLK				RCC_AHB1Periph_GPIOB
#define	DAC1_CS_GPIO						DAC1_CS_GPIO_PORT, DAC1_CS_GPIO_PIN

#define DAC2_CS_GPIO_PORT				GPIOC
#define DAC2_CS_GPIO_PIN				GPIO_Pin_5
#define DAC2_CS_GPIO_CLK				RCC_AHB1Periph_GPIOC
#define	DAC2_CS_GPIO						DAC2_CS_GPIO_PORT, DAC2_CS_GPIO_PIN

#define ADC1_CS_GPIO_PORT				GPIOC
#define ADC1_CS_GPIO_PIN				GPIO_Pin_4
#define ADC1_CS_GPIO_CLK				RCC_AHB1Periph_GPIOC
#define	ADC1_CS_GPIO						ADC1_CS_GPIO_PORT, ADC1_CS_GPIO_PIN

#define ADC1_CSTART_GPIO_PORT		GPIOF
#define ADC1_CSTART_GPIO_PIN		GPIO_Pin_4
#define ADC1_CSTART_GPIO_CLK		RCC_AHB1Periph_GPIOF
#define	ADC1_CSTART_GPIO				ADC1_CSTART_GPIO_PORT, ADC1_CSTART_GPIO_PIN

#define ADC1_EOCINT_GPIO_PORT						GPIOF
#define ADC1_EOCINT_GPIO_PIN						GPIO_Pin_8
#define ADC1_EOCINT_GPIO_CLK						RCC_AHB1Periph_GPIOF
#define	ADC1_EOCINT_GPIO								ADC1_EOCINT_GPIO_PORT, ADC1_EOCINT_GPIO_PIN
#define	GPIO_SOURCE_ADC1_EOCINT					GPIO_PinSource8
#define	EXTI_PortSource_ADC1_EOCINT			EXTI_PortSourceGPIOF
#define	EXTI_PinSource_ADC1_EOCINT			EXTI_PinSource8


#define	DA_REF										2.6f

#define	DA_Vmax										5.2f

//#define	AD_K_RATIO							7.8f  //((10k+68.1k)/10k)---24V
#define	AD_K_RATIO							22.0f  //((3.24k+68.1k)/3.24k)---72V
//#define	AD_K_RATIO							53.0f  //((2k+68.1k)/2k)---144V
//#define	AD_K_RATIO								69.1f  //((1k+68.1k)/1k)---144V

#define	K_Voltage_to_Angle				0.072f		//360 / 5000 ,unit:1°/mv;

#define	PwrIN_OffsetVoltage_data		600		//unit:mV

#define	CurentSensorFullRange			30000	//±值,mA

#define	CurentSensorMidVoltage		2700	//mV

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
//ADC转换CS引脚触发模式步骤枚举
typedef enum
{
	CmdConfigStartSample=0,
	SampleForCH0,
	SampleForCH1,
	SampleForCH2,
	SampleForCH3,
	WaitEocFlag,
	ReadFifo,
}ADC_CONV_TRIG_BY_CS;



ANALOG_EXT u8 ADC_EOC_Flag;
ANALOG_EXT u32 ADC_CHK_1ms_cnt;

ANALOG_EXT u8 	TLV2544_ConfigIni(void);
ANALOG_EXT u16	TLV2544_RWWord(u16 dat);

ANALOG_EXT void Analog_InitHard(void);

ANALOG_EXT void Tsk_Analog(void);

ANALOG_EXT  s16 Voltage_to_High(u16 vol, u8 i);




#endif


