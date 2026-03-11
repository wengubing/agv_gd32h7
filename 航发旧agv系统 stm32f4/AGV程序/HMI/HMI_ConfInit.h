/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	HMI_ConfInit.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-10-27         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/

/*define-----------------------------------------------------------------------*/
#ifndef __HMI_ConfInit_H_
#define __HMI_ConfInit_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  HMI_ConfInit_GLOBALS
	#define HMI_ConfInit_GLOBALS_EXT
#else
	#define HMI_ConfInit_GLOBALS_EXT  extern
#endif

HMI_ConfInit_GLOBALS_EXT void ObjInit_HMI(void);

#endif




 

