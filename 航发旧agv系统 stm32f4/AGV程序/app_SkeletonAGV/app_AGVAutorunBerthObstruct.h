/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_AGVAutorunBerthObstruct.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: AGV自动运行的框架代码
  *******************************************************************************/

#ifndef _APP_AGVAUTORUNBERTHOBSTRUCT_H_
#define _APP_AGVAUTORUNBERTHOBSTRUCT_H_

#ifdef  APP_AGVAUTORUNBERTHOBSTRUCT_GLOBALS
 #define APP_AGVAUTORUNBERTHOBSTRUCT_EXT
#else
 #define APP_AGVAUTORUNBERTHOBSTRUCT_EXT  extern
#endif 

/*自定义引用*/

#include "drv_path.h"
uint8_t AGV_BerthObstructStaCheck(void);
void 	AGV_TskFunctionStaBerthObstruct(void);

#endif

/************************************END OF FILE************************************/
