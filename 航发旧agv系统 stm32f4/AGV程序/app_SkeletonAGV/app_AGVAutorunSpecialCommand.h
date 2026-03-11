/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_AGVAutorunSpecialCommand.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: AGV自动运行的框架代码
  *******************************************************************************/

#ifndef _APP_AGVAUTORUNSPECIALCOMMAND_H_
#define _APP_AGVAUTORUNSPECIALCOMMAND_H_

#ifdef  APP_AGVAUTORUNSPECIALCOMMAND_GLOBALS
 #define APP_AGVAUTORUNSPECIALCOMMAND_EXT
#else
 #define APP_AGVAUTORUNSPECIALCOMMAND_EXT  extern
#endif 

/*自定义引用*/

#include "drv_path.h"
uint8_t AGV_SpecialCommandStaCheck(void);
void 	AGV_TskFunctionStaSpecialCommand(void);

#endif

/************************************END OF FILE************************************/
