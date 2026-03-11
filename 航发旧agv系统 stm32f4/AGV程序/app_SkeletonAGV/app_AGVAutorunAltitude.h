/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_AGVAutorunAltitude.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: AGV自动运行的框架代码
  *******************************************************************************/

#ifndef _APP_AGVAUTORUNALTITUDE_H_
#define _APP_AGVAUTORUNALTITUDE_H_

#ifdef  APP_AGVAUTORUNALTITUDE_GLOBALS
 #define APP_AGVAUTORUNALTITUDE_EXT
#else
 #define APP_AGVAUTORUNALTITUDE_EXT  extern
#endif 

/*自定义引用*/

#include "drv_path.h"
uint8_t AGV_AltitudeStaCheck(AGVFunctionSta_TypeDef AGV_FuncSta_Next);
void 	AGV_TskFunctionStaAltitude(void);

#endif

/************************************END OF FILE************************************/
