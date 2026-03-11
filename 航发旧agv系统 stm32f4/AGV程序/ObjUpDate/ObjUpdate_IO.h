/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ObjUpdate_IO.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-02-07         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
/*自定义引用*/

/*define-----------------------------------------------------------------------*/

#ifndef __OBJUPDATE_IO_H_
#define __OBJUPDATE_IO_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  OBJUPDATE_IO_GLOBALS
	#define OBJUPDATE_IO_GLOBALS_EXT
#else
	#define OBJUPDATE_IO_GLOBALS_EXT  extern
#endif

OBJUPDATE_IO_GLOBALS_EXT	void ObjUpdate_IOOutput(void);
OBJUPDATE_IO_GLOBALS_EXT	void ObjUpdate_IOInput(void);
#endif


 

