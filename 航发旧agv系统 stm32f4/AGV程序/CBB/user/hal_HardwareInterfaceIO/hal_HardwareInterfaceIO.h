/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : hal_HardwareInterfaceIO.h   
  * Version    : V1.0		
  * Author     : Master			
  * Date       : 2024.02.07
  * Description: 硬件抽象层IO接口模块
  *******************************************************************************/

#ifndef _HAL_HARDWAREINTERFACE_H_
#define _HAL_HARDWAREINTERFACE_H_

#ifdef  HAL_HARDWARE_INTER_FACE_IO_GLOBALS
 #define HAL_HARDWARE_INTER_FACE_IO
#else
 #define HAL_HARDWARE_INTER_FACE_IO  extern
#endif

#include "hal_STM32GPIO.h" 
 
static const uint8_t g_HalGpioNumMax = CnEnd;


///*Gpio对象 模式*/
//typedef enum
//{
//	HalNULL		= 0;		
//	HalInPut	= 1,	/*输入管脚*/
//	HalOutPut 	= 2,	/*输出管脚*/
//}_HalGpioMold_TypeDef;

/*Gpio对象 输出电平*/
typedef enum
{
	LowLevel	= 0,		/*低电平*/
	UpLevel		= 1,		/*高电平*/
	LevelErr	= 2,
}_HalGpioLevel_TypeDef;

/*硬件抽象层IO 对像定义*/
typedef struct {
	_HalGpioLevel_TypeDef OutValue;	/*输出电平*/
	_HalGpioLevel_TypeDef InValue;	/*输入电平*/
}_HalGpioValue_TypeDef;

/*MOTC-3主板GPIO硬件对象穷举*/
typedef struct {
	/*全部的输入IO*/
		/*普通输入IO，无源输入，共计16个*/
		_HalGpioValue_TypeDef CN56;
		_HalGpioValue_TypeDef CN72;
		_HalGpioValue_TypeDef CN57;
		_HalGpioValue_TypeDef CN73;

		_HalGpioValue_TypeDef CN58;
		_HalGpioValue_TypeDef CN74;
		_HalGpioValue_TypeDef CN61;
		_HalGpioValue_TypeDef CN77;

		_HalGpioValue_TypeDef CN62;
		_HalGpioValue_TypeDef CN78;
		_HalGpioValue_TypeDef CN63;
		_HalGpioValue_TypeDef CN79;

		_HalGpioValue_TypeDef CN64;
		_HalGpioValue_TypeDef CN80;
		_HalGpioValue_TypeDef CN65;
		_HalGpioValue_TypeDef CN81;

		/*特殊输入IO，有源输入，共计4个*/
		_HalGpioValue_TypeDef CN66;
		_HalGpioValue_TypeDef CN67;
		_HalGpioValue_TypeDef CN68;
		_HalGpioValue_TypeDef CN69;

		/*4PIN端子；1脚电源 2脚GND 3、4脚 2路 输入IO*/
		_HalGpioValue_TypeDef CN60_3;
		_HalGpioValue_TypeDef CN60_4;
		_HalGpioValue_TypeDef CN75_3;
		_HalGpioValue_TypeDef CN75_4;
		_HalGpioValue_TypeDef CN59_3;
		_HalGpioValue_TypeDef CN59_4;
		_HalGpioValue_TypeDef CN76_3;
		_HalGpioValue_TypeDef CN76_4;

		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/

		_HalGpioValue_TypeDef CN40_9;
		_HalGpioValue_TypeDef CN40_10;
		_HalGpioValue_TypeDef CN40_11;
		_HalGpioValue_TypeDef CN40_12;

		_HalGpioValue_TypeDef CN24_9;
		_HalGpioValue_TypeDef CN24_10;
		_HalGpioValue_TypeDef CN24_11;
		_HalGpioValue_TypeDef CN24_12;

		/*15PIN端子；6路 输出IO、2路 输入IO*/

		_HalGpioValue_TypeDef CN70_10;
		_HalGpioValue_TypeDef CN70_11;

		_HalGpioValue_TypeDef CN82_10;
		_HalGpioValue_TypeDef CN82_11;

		_HalGpioValue_TypeDef CN83_10;
		_HalGpioValue_TypeDef CN83_11;

		_HalGpioValue_TypeDef CN84_10;
		_HalGpioValue_TypeDef CN84_11;
	
	/*单片机GPIO直接输出*/
		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/

		_HalGpioValue_TypeDef CN40_5;
		_HalGpioValue_TypeDef CN40_6;
		_HalGpioValue_TypeDef CN40_7;

		_HalGpioValue_TypeDef CN24_3;
		_HalGpioValue_TypeDef CN24_4;
		_HalGpioValue_TypeDef CN24_5;
		_HalGpioValue_TypeDef CN24_6;
		_HalGpioValue_TypeDef CN24_7;

		/*15PIN端子；6路 输出IO、2路 输入IO*/

		_HalGpioValue_TypeDef CN70_5;
		_HalGpioValue_TypeDef CN70_6;
		_HalGpioValue_TypeDef CN70_7;
		_HalGpioValue_TypeDef CN70_8;
		_HalGpioValue_TypeDef CN70_9;

		_HalGpioValue_TypeDef CN82_5;
		_HalGpioValue_TypeDef CN82_6;
		_HalGpioValue_TypeDef CN82_7;
		_HalGpioValue_TypeDef CN82_8;
		_HalGpioValue_TypeDef CN82_9;

		_HalGpioValue_TypeDef CN83_5;
		_HalGpioValue_TypeDef CN83_6;
		_HalGpioValue_TypeDef CN83_7;
		_HalGpioValue_TypeDef CN83_8;
		_HalGpioValue_TypeDef CN83_9;

		_HalGpioValue_TypeDef CN84_5;
		_HalGpioValue_TypeDef CN84_6;
		_HalGpioValue_TypeDef CN84_7;
		_HalGpioValue_TypeDef CN84_8;
		_HalGpioValue_TypeDef CN84_9;

		/*15PIN端子；6路 输出IO、2路 输入IO*/
		_HalGpioValue_TypeDef CN70_3;

		_HalGpioValue_TypeDef CN82_3;

		_HalGpioValue_TypeDef CN83_3;

		_HalGpioValue_TypeDef CN84_3;

	/*HC595扩展IO的GPIO，共计68个*/
		/*13PIN端子；5路 输出IO、4路 输入IO；一般用作避障雷达*/
		_HalGpioValue_TypeDef CN40_4;	//Y32
		_HalGpioValue_TypeDef CN40_3;	//Y33

		/*4PIN端子；3路 输出IO；一般用作三色灯*/
		_HalGpioValue_TypeDef CN31_2;	//Y34
		_HalGpioValue_TypeDef CN31_3;	//Y35

		
		/*普通输出IO，有源输出，共计28个*/
		_HalGpioValue_TypeDef CN26;	//Y36
		_HalGpioValue_TypeDef CN42;	//Y37
		_HalGpioValue_TypeDef CN27;	//Y38
		_HalGpioValue_TypeDef CN43;	//Y39

		_HalGpioValue_TypeDef CN28;	//Y40
		_HalGpioValue_TypeDef CN44;	//Y41
		_HalGpioValue_TypeDef CN32;	//Y42
		_HalGpioValue_TypeDef CN47;	//Y43

		_HalGpioValue_TypeDef CN25;	//Y44
		_HalGpioValue_TypeDef CN41;	//Y45
		_HalGpioValue_TypeDef CN33;	//Y46
		_HalGpioValue_TypeDef CN48;	//Y47

		_HalGpioValue_TypeDef CN34;	//Y48
		_HalGpioValue_TypeDef CN49;	//Y49
		_HalGpioValue_TypeDef CN35;	//Y50
		_HalGpioValue_TypeDef CN50;	//Y51
		
		_HalGpioValue_TypeDef CN36;	//Y52
		_HalGpioValue_TypeDef CN51;	//Y53
		_HalGpioValue_TypeDef CN37;	//Y54
		_HalGpioValue_TypeDef CN52;	//Y55

		_HalGpioValue_TypeDef CN38;	//Y56
		_HalGpioValue_TypeDef CN53;	//Y57
		_HalGpioValue_TypeDef CN87;	//Y58
		_HalGpioValue_TypeDef CN89;	//Y59

		/*4PIN端子；3路 输出IO；一般用作三色灯*/
		_HalGpioValue_TypeDef CN46_2;	//Y60
		_HalGpioValue_TypeDef CN46_3;	//Y61
		_HalGpioValue_TypeDef CN46_4;	//Y62

		_HalGpioValue_TypeDef CN31_4;	//Y63

		/*普通输出IO，有源输出，共计28个*/
		_HalGpioValue_TypeDef CN45;	//Y64
		_HalGpioValue_TypeDef CN29;	//Y65
		_HalGpioValue_TypeDef CN90;	//Y66
		_HalGpioValue_TypeDef CN88;	//Y67

		/*7PIN端子；4路 输出IO*/
		_HalGpioValue_TypeDef CN30_3;	//Y68
		_HalGpioValue_TypeDef CN30_4;	//Y69
		_HalGpioValue_TypeDef CN30_5;	//Y70
		_HalGpioValue_TypeDef CN30_6;	//Y71

}
_HalGpioCN_TypeDef;

/*硬件抽象层IO 对像定义*/
typedef struct {

	/*初始化输入*/
	_HalGpioNum_TypeDef		Map;		/*管脚连接在开发板的位置*/

	/*实时调用输入*/
	_HalGpioLevel_TypeDef	OutValue;	/*输出电平*/

	/*查询与控制输出*/
	_HalGpioLevel_TypeDef	InValue;	/*输入电平*/

}_HalGpio_TypeDef;


/*外部方法声明*/
HAL_HARDWARE_INTER_FACE_IO _HalGpioCN_TypeDef g_HalGPIO;
HAL_HARDWARE_INTER_FACE_IO void HAL_IOInit(void);
HAL_HARDWARE_INTER_FACE_IO void HAL_IOTaskProcess(void);	/*不支持重入*/
HAL_HARDWARE_INTER_FACE_IO uint8_t HAL_IOAdd(_HalGpio_TypeDef *p_Data);	
/***********************************************************************************/
///*初始化函数中添加如下代码:*/
//{

//}
///*主循环中添加如下代码:*/
//{
//	/*数据输入管道*/
//	{

//	}
//	/*算法调用*/
//	{
//		HAL_IOTaskProcess(&GPIO);
//	}
//	/*数据输出管道*/
//	{

//	}
//}
/***********************************************************************************/

#endif

/************************************END OF FILE************************************/
