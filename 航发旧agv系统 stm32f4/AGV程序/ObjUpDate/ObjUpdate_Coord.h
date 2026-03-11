/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ObjUpdate_COORD.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-02-07         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
/*自定义引用*/

/*define-----------------------------------------------------------------------*/

#ifndef __OBJUPDATE_COORD_H_
#define __OBJUPDATE_COORD_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  OBJUPDATE_COORD_GLOBALS
	#define OBJUPDATE_COORD_GLOBALS_EXT
#else
	#define OBJUPDATE_COORD_GLOBALS_EXT  extern
#endif

OBJUPDATE_COORD_GLOBALS_EXT	void ObjUpdate_Coord(void);
#endif


 

