/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_LeadShine.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-01-12
	* Description:
	******************************************************************************
**/


/*include----------------------------------------------------------------------*/
/*自定义引用*/

/*define-----------------------------------------------------------------------*/

#ifndef __ObjUpdate_LeadShine_H
#define __ObjUpdate_LeadShine_H

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ObjUpdate_LeadShine_GLOBALS
	#define ObjUpdate_LeadShine_GLOBALS_EXT
#else
	#define ObjUpdate_LeadShine_GLOBALS_EXT  extern
#endif

ObjUpdate_LeadShine_GLOBALS_EXT	void ObjUpdate_LeadShine(void);
#endif


 

