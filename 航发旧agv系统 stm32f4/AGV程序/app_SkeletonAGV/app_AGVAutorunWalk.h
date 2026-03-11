/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_AGVAutorunWalk.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: AGV自动运行的框架代码
  *******************************************************************************/

#ifndef _APP_AGVAUTORUNWALK_H_
#define _APP_AGVAUTORUNWALK_H_

#ifdef  APP_AGVAUTORUNWALK_GLOBALS
 #define APP_AGVAUTORUNWALK_EXT
#else
 #define APP_AGVAUTORUNWALK_EXT  extern
#endif 

/*自定义引用*/

#include "drv_path.h"
uint8_t AGV_WalkStaCheck(PathTypeDef *p_NextPath , PathTypeDef *p_LasetPath);
void AGV_TskFunctionStaWalk(void);

#endif

/************************************END OF FILE************************************/
