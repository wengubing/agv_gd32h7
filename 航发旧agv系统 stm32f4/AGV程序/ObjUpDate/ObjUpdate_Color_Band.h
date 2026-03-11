/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ObjUpdate_Color_Band.c
	* Version    :	v1.0
	* Author     :	MT
	* Date       : 2025-06-17
	* Description:
	******************************************************************************
**/


/*include----------------------------------------------------------------------*/
/*自定义引用*/

/*define-----------------------------------------------------------------------*/

#ifndef __OBJUPDATE_COLOR_BAND_H
#define __OBJUPDATE_COLOR_BAND_H

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  OBJUPDATE_COLOR_BAND_GLOBALS
	#define OBJUPDATE_COLOR_BAND_GLOBALS_EXT
#else
	#define OBJUPDATE_COLOR_BAND_GLOBALS_EXT  extern
#endif

OBJUPDATE_COLOR_BAND_GLOBALS_EXT	void ObjUpdate_Color_Band(void);
#endif


 

