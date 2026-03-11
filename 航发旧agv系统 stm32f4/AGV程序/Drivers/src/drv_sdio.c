/****************************Copyright (c)**********************************************                                       
**                  ╭--------------------------╮
**                  ︱   成都航发液压工程有限公司 ︱
**                  ︱ http://www.hangfa.com ︱                     
**                  ╰--------------------------╯
文件名称：drv_sdio.c
描  述：sdio驱动
作  者：邓 洋
版  本：V1.0.0
日  期：2015年03月18日
修改记录：

***************************************************************************************/  

#define  SDIO_GLOBALS


/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "..\..\Drivers\inc\drv_sdio.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/


//SDIO外设寄存器设置为默认值 
void SDIO_Register_Deinit()
{
 SDIO_DeInit();
 SDIO->POWER=0x00000000;
 SDIO->CLKCR=0x00000000;
 SDIO->ARG=0x00000000;
 SDIO->CMD=0x00000000;
 SDIO->DTIMER=0x00000000;
 SDIO->DLEN=0x00000000;
 SDIO->DCTRL=0x00000000;
 SDIO->ICR=0x00C007FF;
 SDIO->MASK=0x00000000;  
}

/***************************************************************************************
** 函数名称: SD_plug_Status_Check
** 功能描述: SD卡座状态检测
参数:
返回值:0:卡插入;1:无卡
***************************************************************************************/
u8 SD_plug_Status_Check(void)
{
 if(GET_PIN(GPIOD, GPIO_Pin_0))
 {
  SDCardInfo.SDCardPlugStatus=SDCard_PlugOut;
  return SDCard_PlugOut;
 }
 else
 {
  SDCardInfo.SDCardPlugStatus=SDCard_PlugIn;
  return SDCard_PlugIn;
 }
}

/***************************************************************************************
** 函数名称: SDIO_InitHard
** 功能描述: 初始化SDIO外设
参数:
返回值:0:卡插入;1:无卡
***************************************************************************************/
u8 SDIO_InitHard(u8 pi)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
 NVIC_InitTypeDef NVIC_InitStructure;
 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA2, ENABLE);//使能GPIOC,GPIOD DMA2时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);//SDIO时钟使能
 RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, ENABLE);//SDIO复位

  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;  //PC8,9,10,11,12复用功能输出 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);// PC8,9,10,11,12复用功能输出
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);//PD2复用功能输出
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入
  GPIO_Init(GPIOD, &GPIO_InitStructure);// PD0输入
 
  //引脚复用映射设置
 GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_SDIO); //PC8,AF12
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SDIO); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_SDIO); 
 
 RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, DISABLE);//SDIO结束复位
  
  //SDIO外设寄存器设置为默认值  
 SDIO_Register_Deinit();
 
  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级3
 NVIC_InitStructure.NVIC_IRQChannelSubPriority=pi;  //子优先级3
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   //IRQ通道使能
 NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器
 
 return SD_plug_Status_Check();
}

/***************************************************************************************
** 函数名称: MMC_disk_initialize
** 功能描述: MMC_SD存储初始化
参数:
返回值:错误代码;(0,无错误)
***************************************************************************************/
u8 MMC_disk_initialize(void)
{
 u8 status=0;

 if(SDIO_InitHard(1)==SDCard_PlugOut)
 {//无SD卡
  status=1;
 }
 else
 {//有SD卡
   if(SD_Init()!=SD_OK)
    status = 2;
 }
 
 return status;   
}

/************************ (C) COPYRIGHT HANGFA *****END OF FILE****/