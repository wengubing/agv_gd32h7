/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	ObjUpdate_VirtualMagnetic.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-09-27         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
/*自定义引用*/

/*define-----------------------------------------------------------------------*/

#ifndef __OBJUPDATE_VirtualMagnetic_H_
#define __OBJUPDATE_VirtualMagnetic_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ObjUpdate_VirtualMagnetic_GLOBALS
	#define ObjUpdate_VirtualMagnetic_EXT
#else
	#define ObjUpdate_VirtualMagnetic_EXT  extern
#endif

ObjUpdate_VirtualMagnetic_EXT	void ObjUpdate_VirtualMagnetic(void);
#endif


 

