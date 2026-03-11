/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ACC_PushPullCtrl.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-02-07         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
#include <stdbool.h>


/*define-----------------------------------------------------------------------*/
#ifndef __ACC__PUSHPULLCTRL_H_
#define __ACC__PUSHPULLCTRL_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ACCPUSHPILLCTR_GLOBALS
	#define ACCPUSHPILLCTR_GLOBALS_EXT
#else
	#define ACCPUSHPILLCTR_GLOBALS_EXT  extern
#endif

ACCPUSHPILLCTR_GLOBALS_EXT bool Push_Pull_Ctrl(void);

#endif




 

