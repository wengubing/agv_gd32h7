/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_AGVAutorunAccuratePosition.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: AGV自动运行的框架代码
  *******************************************************************************/

#ifndef _APP_AGVAUTORUNACCURATEPOSITION_H_
#define _APP_AGVAUTORUNACCURATEPOSITION_H_

#ifdef  APP_AGVAUTORUNACCURATEPOSITION_GLOBALS
 #define APP_AGVAUTORUNACCURATEPOSITION_EXT
#else
 #define APP_AGVAUTORUNACCURATEPOSITION_EXT  extern
#endif 

/*自定义引用*/

uint8_t AGV_AccuratePositionStaCheck(void);
void 	AGV_TskFunctionStaAccuratePosition(void);

#endif

/************************************END OF FILE************************************/
