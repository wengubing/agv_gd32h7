/****************************Copyright (c)**********************************************                                       
**                  ╭--------------------------╮
**                  ︱   成都航发液压工程有限公司 ︱
**                  ︱ http://www.hangfa.com ︱                     
**                  ╰--------------------------╯
文件名称：DRV_SDIO.c
描  述：SDIO外设初始化数
作  者：邓 洋
版  本：V1.0.0
日  期：2015年06月09日
修改记录：

***************************************************************************************/  


#ifndef __DRV_SDIO_H
#define __DRV_SDIO_H

#include "..\..\Drivers\SDCard\SDCard.h"


#ifdef  SDIO_GLOBALS
 #define SDIO_EXT
#else
 #define SDIO_EXT  extern
#endif
 
 
 
 
 

SDIO_EXT u8 SD_plug_Status_Check(void);
SDIO_EXT u8 MMC_disk_initialize(void);





#endif

