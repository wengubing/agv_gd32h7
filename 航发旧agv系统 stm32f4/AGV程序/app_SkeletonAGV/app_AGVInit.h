/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   : app_AGVInit.h   
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.XX.XX
  * Description: AGV自动运行的框架代码的初始化
  *******************************************************************************/

#ifndef _APP_AGVINIT_H_
#define _APP_AGVINIT_H_

#ifdef  APP_AGVINIT_GLOBALS
 #define APP_AGVINIT_EXT
#else
 #define APP_AGVINIT_EXT  extern
#endif 

/*自定义引用*/
void app_SkeletonAGV_FunInit(void);
void app_SkeletonAGV_ParaIndexInit(void);
 
#endif

