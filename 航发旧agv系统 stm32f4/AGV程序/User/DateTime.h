#ifndef __DATETIME_H__
    #define __DATETIME_H__

#ifdef DATETIME_GLOBALS
    #define DATETIME_EXT
#else
    #define DATETIME_EXT extern
#endif

#include "includes.h"
	
DATETIME_EXT uint8_t AppInfo_GetCompileYear(void);
DATETIME_EXT uint8_t AppInfo_GetCompileMonth(void);
DATETIME_EXT uint8_t AppInfo_GetCompileDay(void);

DATETIME_EXT uint8_t AppInfo_GetCompileHour(void);
DATETIME_EXT uint8_t AppInfo_GetCompileMinute(void);
DATETIME_EXT uint8_t AppInfo_GetCompileSecond(void);

/********************************************************************
 *       ÎÄ¼þ½áÊø
 ********************************************************************/
#endif
