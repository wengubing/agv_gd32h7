/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : common_def.h
* Version    : V1.0		
* Author     : WenXiaoBing			
* Date       : 2023.12.29         
* Description: 常见数据类型重定义、常用物理常量宏定义
*******************************************************************************/

/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

/*常用数据类型重定义*/
typedef unsigned char										u8;
typedef signed char 										s8;
typedef unsigned short										u16;
typedef signed short 										s16;
typedef unsigned int										u32;
typedef signed int 											s32;
typedef float 												f32;
typedef float 												float32_t;
#ifndef u64								        			
typedef unsigned __int64									u64;/* 无符号64位整型变量                 */ 
#endif		                		            			                                 
#ifndef s64													  
typedef signed   __int64									s64;/* 无符号64位整型变量                 */
#endif

//浮点数比较 极小值
#define EPS 1e-9

/*位操作方法定义*/
#ifndef M_TST_BIT
#define M_TST_BIT(a,b)		((a) &   ((u64)0x01 << (b)))	/* 位判断,a的b位*/
#endif      		
#ifndef M_SET_BIT         		
#define M_SET_BIT(a,b)		((a) |   ((u64)0x01 << (b)))	/* 置高位,a的b位置1*/
#endif          		
#ifndef M_CLR_BIT         		
#define M_CLR_BIT(a,b)		((a) & (~((u64)0x01 << (b))))	 /* 清零位,a的b位清0*/ 
#endif
#ifndef M_K_CLR_BIT		
#define M_K_CLR_BIT(a,b)	(a	&= (~((u64)0x01 << b)))		 /* 数据a的b位清0,对a数据进行操作*/
#endif
#ifndef M_K_SET_BIT	
#define M_K_SET_BIT(a,b)	(a	|= ((u64)0x01 << b))		/* 数据位置1,对a数据进行操作*/
#endif
#ifndef M_GET_BIT
#define M_GET_BIT(a,b)		((a) &   ((u64)0x01 << (b)))	/* 位判断,a的b位*/
#endif

/*与平台无关的解耦的定义 便于引用移植*/
#ifndef PGVMAXNUM
#define PGVMAXNUM 1    										//PGV个数
#endif

#ifndef K_RadToAngle
#define K_RadToAngle 57.295779513f    						//180°/ 3.14   PI
#endif

#ifndef K_AngleToRad
#define K_AngleToRad 0.0174532925f    					    //3.14 / 180°
#endif

#ifndef smlt_mbslv1
#define smlt_mbslv1 useUART1								//
#endif

/*常见物理量定义*/
#ifndef PI
#define PI													(3.14159265358979f)
#endif

//常用宏定义方法
#define MIN(a, b) ((a) < (b) ? (a) : (b))					//取最小值

//公共类型
//unit cmd sel&st
typedef enum
{
	AsbModel_Nnull = 0,										//
	AsbModel_A = 1,											//single a
    AsbModel_Master = 1,									//single a
	AsbModel_B = 2,											//
    AsbModel_Slave = 2,										//
	AsbModel_Asb = 3,										//
	AsbModel_End = 65535,									//枚举型结束
}AsbModel_TypeDef;

#endif

/************************************END OF FILE************************************/

