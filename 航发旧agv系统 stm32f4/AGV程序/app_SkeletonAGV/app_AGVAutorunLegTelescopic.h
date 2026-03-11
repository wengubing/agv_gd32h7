/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_AGVAutorunLegTelescopic.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: AGV自动运行的框架代码
  *******************************************************************************/

#ifndef _APP_AGVAUTORUNLEGTELESCOPIC_H_
#define _APP_AGVAUTORUNLEGTELESCOPIC_H_

#ifdef  APP_AGVAUTORUNLEGTELESCOPIC_GLOBALS
 #define APP_AGVAUTORUNLEGTELESCOPIC_EXT
#else
 #define APP_AGVAUTORUNLEGTELESCOPIC_EXT  extern
#endif 

/*自定义引用*/

uint8_t AGV_LegTelescopiStaCheck(void);
void 	AGV_TskFunctionStaLegTelescopic(void);

#endif

/************************************END OF FILE************************************/
