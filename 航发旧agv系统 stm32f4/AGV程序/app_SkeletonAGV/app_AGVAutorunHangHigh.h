/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_AGVAutorunHangHigh.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: AGV自动运行的框架代码
  *******************************************************************************/

#ifndef _APP_AGVAUTORUNHANGHIGH_H_
#define _APP_AGVAUTORUNHANGHIGH_H_

#ifdef  APP_AGVAUTORUNHANGHIGH_GLOBALS
 #define APP_AGVAUTORUNHANGHIGH_EXT
#else
 #define APP_AGVAUTORUNHANGHIGH_EXT  extern
#endif 

/*自定义引用*/

#include "drv_path.h"
uint8_t AGV_HangStaCheck(void);
void 	AGV_TskFunctionStaHang(void);

#endif

/************************************END OF FILE************************************/
