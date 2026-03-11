/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ObjUpdate_LoadDetection.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-02-07         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
/*自定义引用*/

/*define-----------------------------------------------------------------------*/

#ifndef __OBJUPDATE_LoadDetection_H_
#define __OBJUPDATE_LoadDetection_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  OBJUPDATE_LoadDetection_GLOBALS
	#define OBJUPDATE_LoadDetection_GLOBALS_EXT
#else
	#define OBJUPDATE_LoadDetection_GLOBALS_EXT  extern
#endif

OBJUPDATE_LoadDetection_GLOBALS_EXT	void ObjUpdate_LoadDetection(void);
#endif


 

