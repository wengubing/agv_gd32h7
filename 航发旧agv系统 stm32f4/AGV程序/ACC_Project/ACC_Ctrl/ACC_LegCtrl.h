/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ACC_LegCtrl.h   
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

#ifndef __ACC__LEGCTRL_H_
#define __ACC__LEGCTRL_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ACCLEGCTRL_GLOBALS
	#define ACCLEGCTRL_GLOBALS_EXT
#else
	#define ACCLEGCTRL_GLOBALS_EXT  extern
#endif

ACCLEGCTRL_GLOBALS_EXT	bool Leg_Ctrl(void);
#endif


 

