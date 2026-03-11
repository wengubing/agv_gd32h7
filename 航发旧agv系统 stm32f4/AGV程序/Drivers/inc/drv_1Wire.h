
#ifndef _DRV_1WIRE_H
#define _DRV_1WIRE_H


#ifdef  DRV_1WIRE_EXT_GLOBALS
	#define DRV_1WIRE_EXT
#else
	#define DRV_1WIRE_EXT  extern
#endif





#include "..\..\Drivers\inc\drv_timer.h"




/* Private define ------------------------------------------------------------*/
#define			Bus_1wire_Slave						0					//0:单从机,1:多从机

//gpio define
#define OneWireBus_DQ_GPIO_PORT					GPIOF
#define OneWireBus_DQ_GPIO_PIN					GPIO_Pin_9
#define OneWireBus_DQ_GPIO_CLK					RCC_AHB1Periph_GPIOF
#define	OneWireBus_DQ_GPIO							OneWireBus_DQ_GPIO_PORT, OneWireBus_DQ_GPIO_PIN

				

				

#define		DQ_Write_1()     		(OneWireBus_DQ_GPIO_PORT->BSRRL=OneWireBus_DQ_GPIO_PIN)//写1
#define		DQ_Write_0()     		(OneWireBus_DQ_GPIO_PORT->BSRRH=OneWireBus_DQ_GPIO_PIN)//写0
#define		DQ_ReadBit					((OneWireBus_DQ_GPIO_PORT->IDR & OneWireBus_DQ_GPIO_PIN)			\
																														== OneWireBus_DQ_GPIO_PIN)//读DQ上的值

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
	Mode_9bit=0,
	Mode_10bit,
	Mode_11bit,
	Mode_12bit,
}DS18b20_conv_mode;



/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

typedef struct		Bus_1wire_un	                                            
{
	u8		DS18b20_ROM_DAT[7];//rom data
				//DS18b20_ROM_DAT[0]---------Fmily code
				//DS18b20_ROM_DAT[1-6]-------SN
	u8		DS18b20_REG_DAT[2];//reg data
				//DS18b20_REG_DAT[0-1]---------Temperature LSB、MSB
}Bus_1wire_PAR;



DRV_1WIRE_EXT		Bus_1wire_PAR			DS18B20_PAR;





/* Exported functions ------------------------------------------------------- */

DRV_1WIRE_EXT	u8 Config_ds18b20(DS18b20_conv_mode mode);
DRV_1WIRE_EXT	u8 Tsk_Read_ds18b20_MREG(u32 delaycnt);
DRV_1WIRE_EXT	u8 Read_ds18b20_ROM(void);


#endif

/***************************************************************************************
**  End Of File                                                     
***************************************************************************************/


