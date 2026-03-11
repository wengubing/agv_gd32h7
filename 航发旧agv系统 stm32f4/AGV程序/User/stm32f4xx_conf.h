/**
  ******************************************************************************
  * @file    ../Libraries/STM32F4xx_StdPeriph_Driver/stm32f4xx_conf.h  
  * @author  By dengyang
  * @version V1.4.0
  * @date    13-November-2013
  * @brief   Library configuration file.
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_CONF_H
#define __STM32F4xx_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Uncomment the line below to enable peripheral header file inclusion */
#include "stm32f4xx_adc.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_crc.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dbgmcu.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_fsmc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rng.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_sdio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_wwdg.h"
#include "stm32f4xx_dcmi.h"
#include "misc.h" /* 用于NVIC和SysTick的高级函数(与CMSIS相关) */

#if defined (STM32F429_439xx)
#include "stm32f4xx_cryp.h"
#include "stm32f4xx_hash.h"
#include "stm32f4xx_dma2d.h"
#include "stm32f4xx_fmc.h"
#include "stm32f4xx_ltdc.h"
#include "stm32f4xx_sai.h"
#endif   /* #define STM32F429_439xx */  /*!< STM32F429VG, STM32F429VI, STM32F429ZG, STM32F429ZI, STM32F429BG, STM32F429BI,  
                                      STM32F429NG, STM32F439NI, STM32F429IG, STM32F429II, STM32F439VG, STM32F439VI, 
                                      STM32F439ZG, STM32F439ZI, STM32F439BG, STM32F439BI, STM32F439NG, STM32F439NI,
                                      STM32F439IG and STM32F439II Devices */

#if defined (STM32F427_437xx)
#include "stm32f4xx_cryp.h"
#include "stm32f4xx_hash.h"
#include "stm32f4xx_dma2d.h"
#include "stm32f4xx_fmc.h"
#include "stm32f4xx_sai.h"
#endif /* #define STM32F427_437xx */  /*!< STM32F427VG, STM32F427VI, STM32F427ZG, STM32F427ZI, STM32F427IG, STM32F427II,  
                                      STM32F437VG, STM32F437VI, STM32F437ZG, STM32F437ZI, STM32F437IG, STM32F437II Devices */

#if defined (STM32F40_41xxx)
#include "stm32f4xx_cryp.h"
#include "stm32f4xx_hash.h"
#endif     /* #define STM32F40_41xxx */   /*!< STM32F405RG, STM32F405VG, STM32F405ZG, STM32F415RG, STM32F415VG, STM32F415ZG,  
                                      STM32F407VG, STM32F407VE, STM32F407ZG, STM32F407ZE, STM32F407IG, STM32F407IE, 
                                      STM32F417VG, STM32F417VE, STM32F417ZG, STM32F417ZE, STM32F417IG and STM32F417IE Devices */

/*
 用户可以选择是否使能ST固件库的断言供能。使能断言的方法有两种：
 (1) 在C编译器的预定义宏选项中定义USE_FULL_ASSERT。
 (2) 在本文件取消"#define USE_FULL_ASSERT    1"行的注释。
*/
/* 取消下面代码行的注释则固件库代码会展开assert_param宏进行断言 */

//#define   USE_FULL_ASSERT    1

#ifdef   USE_FULL_ASSERT
 /*
  assert_param宏用于函数形参检查。如果expr是false，它将调用assert_failed()函数报告发生错误的源文件
  和行号。如果expr是true，将不执行任何操作。

  assert_failed() 函数在stm32f10x_assert.c文件(这是安富莱建立的文件)
 */
    #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))

    void assert_failed(uint8_t* file, uint32_t line);
#else
    #define assert_param(expr) ((void)0)
#endif




#endif /* __STM32F4xx_CONF_H */
