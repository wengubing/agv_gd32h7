/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ObjUpdate_VirtualOdo.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-09-30
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
/*自定义引用*/

/*define-----------------------------------------------------------------------*/

#ifndef __OBJUPDATE_VirtualOdo_H_
#define __OBJUPDATE_VirtualOdo_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ObjUpdate_VirtualOdo_GLOBALS
	#define ObjUpdate_VirtualOdo_EXT
#else
	#define ObjUpdate_VirtualOdo_EXT  extern
#endif

ObjUpdate_VirtualOdo_EXT	void ObjUpdate_VirtualOdo(void);
#endif


 

