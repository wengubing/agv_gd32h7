/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ACC_LiftCtrl.h   
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

#ifndef __ACC__LIFTCTRL_H_
#define __ACC__LIFTCTRL_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ACCLIFTCTRL_GLOBALS
	#define ACCLIFTCTRL_GLOBALS_EXT
#else
	#define ACCLIFTCTRL_GLOBALS_EXT  extern
#endif

ACCLIFTCTRL_GLOBALS_EXT bool LiftHighSet(void);
ACCLIFTCTRL_GLOBALS_EXT bool LiftLowSet(void);
ACCLIFTCTRL_GLOBALS_EXT bool Lift_Ctrl(void);

#endif


 

