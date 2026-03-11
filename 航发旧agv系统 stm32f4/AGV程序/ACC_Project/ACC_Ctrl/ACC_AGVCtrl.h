/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ACC_AGVCtrl.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-02-07         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
/*自定义引用*/
#include <stdbool.h>

/*define-----------------------------------------------------------------------*/

#ifndef __ACC__AGVCTRL_H_
#define __ACC__AGVCTRL_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ACC_AGVCTRL_GLOBALS
	#define ACC_AGVCTRL_GLOBALS_EXT
#else
	#define ACC_AGVCTRL_GLOBALS_EXT  extern
#endif

ACC_AGVCTRL_GLOBALS_EXT	bool ACC_AGV_Ctrl(void);
#endif


 

