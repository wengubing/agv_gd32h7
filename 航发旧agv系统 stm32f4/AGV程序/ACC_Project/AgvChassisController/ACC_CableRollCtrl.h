/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	ACC_CableRollCtrl.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-10-29
	* Description:
	******************************************************************************
**/


/*include----------------------------------------------------------------------*/

/*define-----------------------------------------------------------------------*/
#ifndef __ACC_CableRollCtrl_H_
#define __ACC_CableRollCtrl_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  ACC_CableRollCtrl_GLOBALS
	#define ACC_CableRollCtrl_GLOBALS_EXT
#else
	#define ACC_CableRollCtrl_GLOBALS_EXT  extern
#endif

ACC_CableRollCtrl_GLOBALS_EXT void ACC_CableRoll_Ctrl(void);

#endif




 

