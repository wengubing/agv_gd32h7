/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ACC_WalkMotorResolving.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-02-07         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
#include <stdbool.h>

/*define-----------------------------------------------------------------------*/


#ifndef __ACC__WALK_MOTOR_RESOLVING_H_
#define __ACC__WALK_MOTOR_RESOLVING_H_

#ifdef  ACC_WALK_MOTOR_RESOLVING_GLOBALS
	#define ACC_WALK_MOTOR_RESOLVING_GLOBALS_EXT
#else
	#define  ACC_WALK_MOTOR_RESOLVING_GLOBALS_EXT extern
#endif

ACC_WALK_MOTOR_RESOLVING_GLOBALS_EXT	bool WheelAngleZeroSet(void);
	
	
	
#endif


 

