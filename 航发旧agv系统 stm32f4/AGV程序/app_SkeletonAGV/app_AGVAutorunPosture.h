/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_AGVAutorunPosture.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: AGV自动运行的框架代码
  *******************************************************************************/

#ifndef _APP_AGVAUTORUNPOSTURE_H_
#define _APP_AGVAUTORUNPOSTURE_H_

#ifdef  APP_AGVAUTORUNPOSTURE_GLOBALS
 #define APP_AGVAUTORUNPOSTURE_EXT
#else
 #define APP_AGVAUTORUNPOSTURE_EXT  extern
#endif 

/*自定义引用*/

#include "drv_path.h"
uint8_t AGV_PostureStaCheck(AGVFunctionSta_TypeDef AGV_FuncSta_Next);
void 	AGV_TskFunctionStaPosture(void);

#endif

/************************************END OF FILE************************************/
