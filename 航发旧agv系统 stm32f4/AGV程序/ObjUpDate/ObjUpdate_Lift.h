/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ObjUpdate_Lift.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-02-07         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
/*自定义引用*/

/*define-----------------------------------------------------------------------*/

#ifndef __OBJUPDATE_Lift_H_
#define __OBJUPDATE_Lift_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  OBJUPDATE_Lift_GLOBALS
	#define OBJUPDATE_Lift_GLOBALS_EXT
#else
	#define OBJUPDATE_Lift_GLOBALS_EXT  extern
#endif

OBJUPDATE_Lift_GLOBALS_EXT	void ObjUpdate_Lift(void);
#endif


 

