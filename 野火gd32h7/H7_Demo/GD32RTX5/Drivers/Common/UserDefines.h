
#ifndef __USERDEFINES_H
#define	__USERDEFINES_H

/****************************Copyright (c)**********************************************                                       
文件名称：UserDefines.h
描		述：用户公共宏定义
作		者：邓 洋
版		本：V1.0.0
日		期：2014年04月25日
修改记录：
v1.0.0  ->  新建.

******************************************************************************************/



//typedef enum {FALSE = 0, TRUE = !FALSE} bool, MyBoolT;	// 布尔逻辑枚举
 
#define		Get_bit(data, n)		((data & (1 << (n & 0x3f))) == (1 << (n & 0x3f)))		//取第n的状态位[SET:1;RESET:0]---不对数据data操作,n:0...63
#define		Set_bit(data, n)		((data & (1 << (n & 0x3f))) |= (1 << (n & 0x3f)))		//置第n的状态:1---n:0...63
#define		Clr_bit(data, n)		((data & (1 << (n & 0x3f))) &= ~(1 << (n & 0x3f)))	//清第n的状态:0---n:0...63




/* 默认变量定义-----------------------------------------------------------------------*/
#ifndef NULL									/*                                    */
#define NULL  ((void *)0)                       /* 空指针                             */
#endif	                                        /*                                    */
// #ifndef FALSE                                   /*                                    */
// #define FALSE 0                                 /* 逻辑假                             */
// #endif                                          /*                                    */
// #ifndef TRUE                                    /*                                    */
// #define TRUE  !FALSE                            /* 逻辑真                             */
// #endif	                                        /*                                    */
#ifndef false                                   /*                                    */
#define false 0                                 /* 逻辑假                             */
#endif	                                        /*                                    */
#ifndef true                                    /*                                    */
#define true  !false                            /* 逻辑真                             */
#endif	                                        /*                                    */
#ifndef OFF                                     /*                                    */
#define OFF   0                                 /* 关                                 */
#endif	                                        /*                                    */
#ifndef ON                                      /*                                    */
#define ON    !OFF                              /* 开                                 */
#endif	                                        /*                                    */
#ifndef LOW                                     /*                                    */
#define LOW   0                                 /* 低                                 */
#endif	                                        /*                                    */
#ifndef HIGH                                    /*                                    */
#define HIGH  !LOW                              /* 高                                 */
#endif	                                        /*                                    */

/* 变量类型声明-----------------------------------------------------------------------*/
#ifndef uint8									/*                                    */
typedef unsigned char				uint8;      /* 无符号8位整型变量                  */
#endif                  			            /*                                    */                                     
#ifndef int8									/*                                    */                                     
typedef signed   char				int8;       /* 有符号8位整型变量                  */   
#endif                  			            /*                                    */                                     
#ifndef uint16									/*                                    */                                     
typedef unsigned short				uint16;     /* 无符号16位整型变量                 */ 
#endif                  			            /*                                    */                                     
#ifndef int16									/*                                    */                                     
typedef signed   short				int16;      /* 有符号16位整型变量                 */ 
#endif                  			            /*                                    */                                     
#ifndef uint32									/*                                    */                                     
typedef unsigned int				uint32;     /* 无符号32位整型变量                 */ 
#endif                  			            /*                                    */                                     
#ifndef int32									/*                                    */                                     
typedef signed   int				int32;      /* 有符号32位整型变量                 */  
#endif                  			            /*                                    */                                    
#ifndef int64									/*                                    */   
typedef signed   __int64			int64;		/* 有符号64位整型变量                 */  
#endif		                		            /*                                    */                                  
#ifndef uint64									/*                                    */  
typedef unsigned __int64			uint64;		/* 无符号64位整型变量                 */  
#endif		                		            /*                                    */                                   
#ifndef f32										/*                                    */                                     
typedef float						f32;        /* 单精度浮点数（32位长度）           */  
#endif                  			            /*                                    */                                     
#ifndef f64										/*                                    */                                     
typedef double						f64;        /* 双精度浮点数（64位长度）           */ 
#endif		            			            /*                                    */                                    
#ifndef fp32									/*                                    */                                     
typedef float						fp32;       /* 单精度浮点数（32位长度）           */  
#endif                  			            /*                                    */                                     
#ifndef fp64									/*                                    */                                     
typedef double						fp64;       /* 双精度浮点数（64位长度）           */ 
#endif		            			            /*                                    */                                  
#ifndef u64								        /*                                    */  
typedef unsigned __int64			u64;		/* 无符号64位整型变量                 */ 
#endif		                		            /*                                    */                                   
#ifndef s64										/*                                    */    
typedef signed   __int64			s64;		/* 无符号64位整型变量                 */
#endif		                		            /*                                    */                                     
#ifndef vfp32									/*                                    */                                     
typedef volatile float				vfp32;      /* 单精度浮点数（32位长度）           */  
#endif                      		            /*                                    */                                    
#ifndef vfp64									/*                                    */                                     
typedef volatile double				vfp64;      /* 双精度浮点数（64位长度）           */  
#endif		                                    /*                                    */                                    
#ifndef vf32									/*                                    */                                     
typedef volatile float				vf32;       /* 单精度浮点数（32位长度）           */  
#endif                      		            /*                                    */                                    
#ifndef vf64									/*                                    */                                     
typedef volatile double				vf64;       /* 双精度浮点数（64位长度）           */  
#endif		                                    /*                                    */                                
#ifndef vu64							        /*                                    */  
typedef volatile unsigned __int64	vu64;		/* 有符号64位整型变量                 */ 
#endif		                                    /*                                    */                                   
#ifndef vs64							        /*                                    */    
typedef volatile signed __int64		vs64;		/* 无符号64位整型变量                 */
#endif		                                    /*                                    */ 


//#ifndef INT32U									/*                                    */
//typedef unsigned int				INT32U;      /* 无符号8位整型变量                  */
//#endif     
//#ifndef INT16U									/*                                    */
//typedef unsigned short				INT16U;      /* 无符号8位整型变量                  */
//#endif 
//#ifndef INT8U									/*                                    */
//typedef unsigned char				INT8U;      /* 无符号8位整型变量                  */
//#endif 


/* -----------------------------------------------------------------------------------*/ 
#ifndef GET_PORT					 					/* 直接端口操作定义           */
#define GET_PORT(port)	   (port->IDR)					/* 获取GPIO 引脚              */
#define GET_PIN(port,pin)  (port->IDR   & (pin))		/* 获取GPIO 引脚              */
#define SET_PIN(port,pin)	port->BSRRL  = (pin)			/* 置高GPIO 引脚              */
#define CLR_PIN(port,pin)	port->BSRRH   = (pin)			/* 置低GPIO 引脚              */
#define GET_PINA(pin)	   (GPIOA->IDR  & (pin))		/* 获取GPIOA引脚              */
#define SET_PINA(pin)		GPIOA->BSRRL = (pin)			/* 置高GPIOA引脚              */
#define CLR_PINA(pin)		GPIOA->BSRRH  = (pin)			/* 置低GPIOA引脚              */																  
#define GET_PINB(pin)	   (GPIOB->IDR  & (pin))		/* 获取GPIOA引脚              */
#define SET_PINB(pin)		GPIOB->BSRRL = (pin)			/* 置高GPIOB引脚              */
#define CLR_PINB(pin)		GPIOB->BSRRH  = (pin)			/* 置低GPIOB引脚              */																 
#define GET_PINC(pin)	   (GPIOC->IDR  & (pin))		/* 获取GPIOA引脚              */
#define SET_PINC(pin)		GPIOC->BSRRL = (pin)			/* 置高GPIOC引脚              */
#define CLR_PINC(pin)		GPIOC->BSRRH  = (pin)			/* 置低GPIOC引脚              */																
#define GET_PIND(pin)	   (GPIOD->IDR  & (pin))		/* 获取GPIOA引脚              */
#define SET_PIND(pin)		GPIOD->BSRRL = (pin)			/* 置高GPIOD引脚              */
#define CLR_PIND(pin)		GPIOD->BSRRH  = (pin)			/* 置低GPIOD引脚              */																 
#define GET_PINE(pin)	   (GPIOE->IDR  & (pin))		/* 获取GPIOA引脚              */
#define SET_PINE(pin)		GPIOE->BSRRL = (pin)			/* 置高GPIOE引脚              */
#define CLR_PINE(pin)		GPIOE->BSRRH  = (pin)			/* 置低GPIOE引脚              */
#define GET_PINF(pin)	   (GPIOF->IDR  & (pin))		/* 获取GPIOA引脚              */
#define SET_PINF(pin)		GPIOF->BSRRL = (pin)			/* 置高GPIOF引脚              */
#define CLR_PINF(pin)		GPIOF->BSRRH  = (pin)			/* 置低GPIOF引脚              */															  
#define GET_PING(pin)	   (GPIOG->IDR  & (pin))		/* 获取GPIOA引脚              */
#define SET_PING(pin)		GPIOG->BSRRL = (pin)			/* 置高GPIOG引脚              */
#define CLR_PING(pin)		GPIOG->BSRRH  = (pin)			/* 置低GPIOG引脚              */ 
#endif	 		 										/*                            */

#ifndef M_TST_BIT
#define M_TST_BIT(a,b)		((a) &   ((u64)0x01 << (b)))		/* 位判断,a的b位*/
#endif      		
#ifndef M_SET_BIT         		
#define M_SET_BIT(a,b)		((a) |   ((u64)0x01 << (b)))		/* 置高位,a的b位置1*/
#endif          		
#ifndef M_CLR_BIT         		
#define M_CLR_BIT(a,b)		((a) & (~((u64)0x01 << (b))))	  /* 清零位,a的b位清0*/ 
#endif
#ifndef M_K_CLR_BIT		
#define M_K_CLR_BIT(a,b)	(a	&= (~((u64)0x01 << b)))		  /* 数据a的b位清0,对a数据进行操作*/
#endif
#ifndef M_K_SET_BIT	
#define M_K_SET_BIT(a,b)	(a	|= ((u64)0x01 << b))				/* 数据位置1,对a数据进行操作*/
#endif
#ifndef M_GET_BIT
#define M_GET_BIT(a,b)		((a) &   ((u64)0x01 << (b)))		/* 位判断,a的b位*/
#endif

/* -----------------------------------------------------------------------------------*/
#define Delay_Fuction(A, B, C)		((s32)(A - B) > (s32)(C))
//延时等待操作A:系统计数器,B：需要延时等待的时间计数器,C:等待的时间值
#define	Delay_Assign(B, A)				(B=A)

#define byteof(x)     (sizeof(x))	
//计算数组的字节数


//GPIO状态
enum{
			pin_0 = 1,
			pin_1
		};

/*****************************************************************************
		STM32F4_GPIO配置寄存器相关
*****************************************************************************/
/*	GPIO寄存器偏移地址定义	*/
#define GPIO_IDR	0x10
#define GPIO_ODR	0x14
#define	GPIO_BSRR	0x18
#define GPIO_LCKR	0x1C

/*	GPIO_ODR/IDR(A\B\C\D\E\G\H\I)基地址	*/
#define GPIOA_ODR	(GPIOA_BASE+GPIO_ODR)
#define GPIOA_IDR	(GPIOA_BASE+GPIO_IDR)
#define GPIOB_ODR	(GPIOB_BASE+GPIO_ODR)
#define GPIOB_IDR	(GPIOB_BASE+GPIO_IDR)
#define GPIOC_ODR	(GPIOC_BASE+GPIO_ODR)
#define GPIOC_IDR	(GPIOC_BASE+GPIO_IDR)
#define GPIOD_ODR	(GPIOD_BASE+GPIO_ODR)
#define GPIOD_IDR	(GPIOD_BASE+GPIO_IDR)
#define GPIOE_ODR	(GPIOE_BASE+GPIO_ODR)
#define GPIOE_IDR	(GPIOE_BASE+GPIO_IDR)
#define GPIOF_ODR	(GPIOF_BASE+GPIO_ODR)
#define GPIOF_IDR	(GPIOF_BASE+GPIO_IDR)
#define GPIOG_ODR	(GPIOG_BASE+GPIO_ODR)
#define GPIOG_IDR	(GPIOG_BASE+GPIO_IDR)
#define GPIOH_ODR	(GPIOH_BASE+GPIO_ODR)
#define GPIOH_IDR	(GPIOH_BASE+GPIO_IDR)
#define GPIOI_ODR	(GPIOI_BASE+GPIO_ODR)
#define GPIOI_IDR	(GPIOI_BASE+GPIO_IDR)

/*****************************************************************************
用于硬件直接访问的专用宏
*****************************************************************************/
#define HWREG(x)		(*((volatile unsigned long *)(x)))
#define HWREGH(x)		(*((volatile unsigned short *)(x)))
#define HWREGB(x)		(*((volatile unsigned char *)(x)))
#define HWREGBITW(x, b)                                                       \
        HWREG(((unsigned long)(x) & 0xF0000000) | 0x02000000 |                \
              (((unsigned long)(x) & 0x000FFFFF) << 5) | ((b) << 2))
								
#define HWREGBITH(x, b)                                                       \
        HWREGH(((unsigned long)(x) & 0xF0000000) | 0x02000000 |               \
               (((unsigned long)(x) & 0x000FFFFF) << 5) | ((b) << 2))
								 
#define HWREGBITB(x, b)                                                       \
        HWREGB(((unsigned long)(x) & 0xF0000000) | 0x02000000 |               \
               (((unsigned long)(x) & 0x000FFFFF) << 5) | ((b) << 2))

#define REG_BIT(x,b)\
							(((unsigned long)(x) & 0xF0000000) | 0x02000000 |                \
							(((unsigned long)(x) & 0x000FFFFF) << 5) | ((b) << 2))



//-------------定义多种数据类型的联合，以支持不同数据类型-----------------
//8字节数据类型
typedef union
{
	unsigned    char        B8_U8[8];
	signed      char        B8_S8[8];
	unsigned    short       B8_U16[4];
	signed      short       B8_S16[4];
	unsigned    int         B8_U32[2];
	signed      int         B8_S32[2];
	unsigned    long long   B8_U64;
	signed      long long   B8_S64;
	float										B8_F32[2];
	double								  B8_F64;
}BYTE8_UNION;

//4字节数据类型
typedef union
{
	unsigned    char        B4_U8[4];
	signed      char        B4_S8[4];
	unsigned    short       B4_U16[2];
	signed      short       B4_S16[2];
	unsigned    int         B4_U32;
	signed      int         B4_S32;
	float										B4_F32;
} BYTE4_UNION;


//2字节数据类型,数据类型转换时用
typedef union
{
	unsigned    char        B2_U8[2];
	signed      char        B2_S8[2];
	unsigned    short       B2_U16;
	signed      short       B2_S16;
} BYTE2_UNION;

//#define		CortexM4

//cortex-m3位映射功能
#ifdef CortexM3
	#define B_GPIOA_BASE	0x42210000				// GPIOA的位基地址
	#define B_GPIOB_BASE	0x42218000				// GPIOB的位基地址
	#define B_GPIOC_BASE	0x42220000      	// GPIOC的位基地址
	#define B_GPIOD_BASE	0x42228000      	// GPIOD的位基地址
	#define B_GPIOE_BASE	0x42230000      	// GPIOE的位基地址
	#define B_GPIOF_BASE	0x42238000      	// GPIOF的位基地址
	#define B_GPIOG_BASE	0x42240000      	// GPIOG的位基地址	
	#define B_GPIOG_MAX		0x42248000      	// GPIO的位最大地址
#endif

#ifdef CortexM4
	#define B_GPIOA_BASE	0x42210000				// GPIOA的位基地址
	#define B_GPIOB_BASE	0x42218000				// GPIOB的位基地址
	#define B_GPIOC_BASE	0x42220000      	// GPIOC的位基地址
	#define B_GPIOD_BASE	0x42228000      	// GPIOD的位基地址
	#define B_GPIOE_BASE	0x42230000      	// GPIOE的位基地址
	#define B_GPIOF_BASE	0x42238000      	// GPIOF的位基地址
	#define B_GPIOG_BASE	0x42240000      	// GPIOG的位基地址	
	#define B_GPIOG_MAX		0x42248000      	// GPIO的位最大地址	
#endif



//--------------------------------------------------------------------------------------
// 端口配置寄存器(GPIOx_CRL) (x=A..E)
//--------------------------------------------------------------------------------------
// CNFy[1:0]：端口x配置位(y = 8… 15)
// 软件通过这些位配置相应的I/O端口，请参考表11端口位配置表。
// 在输入模式(MODE[1:0]=00)：
// 00：模拟输入模式
// 01：浮空输入模式(复位后的状态)
// 10：上拉/下拉输入模式
// 11：保留
// 在输出模式(MODE[1:0]>00)：
// 00：通用推挽输出模式
// 01：通用开漏输出模式
// 10：复用功能推挽输出模式
// 11：复用功能开漏输出模式
//--------------------------------------------------------------------------------------
// MODEy[1:0]：端口x的模式位(y = 8… 15)
// 软件通过这些位配置相应的I/O端口，请参考表11端口位配置表。
// 00：输入模式(复位后的状态)
// 01：输出模式，最大速度10MHz
// 10：输出模式，最大速度2MHz
// 11：输出模式，最大速度50MHz	 
//--------------------------------------------------------------------------------------
typedef struct _GPIO_CRL_REG 
{
	vu32 MODE0[2];		// 
	vu32 CNF0[2];		// 
	vu32 MODE1[2];		// 
	vu32 CNF1[2];		// 
	vu32 MODE2[2];		// 
	vu32 CNF2[2];		// 
	vu32 MODE3[2];		// 
	vu32 CNF3[2];		// 
	vu32 MODE4[2];		// 
	vu32 CNF4[2];		// 
	vu32 MODE5[2];		// 
	vu32 CNF5[2];		// 
	vu32 MODE6[2];		// 
	vu32 CNF6[2];		// 
	vu32 MODE7[2];		// 
	vu32 CNF7[2];		// 
}GPIO_CRL_REG, *pGPIO_CRL_REG;
/*------------------------------------------------------------------------------------*/
typedef struct _GPIO_CRH_REG 
{
	vu32 MODE8[2];		// 
	vu32 CNF8[2];		// 
	vu32 MODE9[2];		// 
	vu32 CNF9[2];		// 
	vu32 MODE10[2];		// 
	vu32 CNF10[2];		// 
	vu32 MODE11[2];		// 
	vu32 CNF11[2];		// 
	vu32 MODE12[2];		// 
	vu32 CNF12[2];		// 
	vu32 MODE13[2];		// 
	vu32 CNF13[2];		// 
	vu32 MODE14[2];		// 
	vu32 CNF14[2];		// 
	vu32 MODE15[2];		// 
	vu32 CNF15[2];		// 
}GPIO_CRH_REG, *pGPIO_CRH_REG;
/*------------------------------------------------------------------------------------*/
typedef struct _GPIO_IDR_REG 
{							 
	vu32 IDR0;			// 0  
	vu32 IDR1;			// 1  
	vu32 IDR2;			// 2  
	vu32 IDR3;			// 3  
	vu32 IDR4;			// 4 
	vu32 IDR5;			// 5  
	vu32 IDR6;			// 6	
	vu32 IDR7;			// 7
	vu32 IDR8;			// 8 
	vu32 IDR9;			// 9  
	vu32 IDR10;			// 10 
	vu32 IDR11;			// 11 
	vu32 IDR12;			// 12 
	vu32 IDR13;			// 13 
	vu32 IDR14;			// 14
	vu32 IDR15;			// 15
	vu32 Reserved[16];	// 31:16
}GPIO_IDR_REG, *pGPIO_IDR_REG;
/*------------------------------------------------------------------------------------*/
typedef struct _GPIO_ODR_REG 
{
	vu32 ODR0;			// 0
	vu32 ODR1;			// 1
	vu32 ODR2;			// 2
	vu32 ODR3;			// 3
	vu32 ODR4;			// 4
	vu32 ODR5;			// 5
	vu32 ODR6;			// 6
	vu32 ODR7;			// 7
	vu32 ODR8;			// 8
	vu32 ODR9;			// 9
	vu32 ODR10;			// 10
	vu32 ODR11;			// 11
	vu32 ODR12;			// 12
	vu32 ODR13;			// 13
	vu32 ODR14;			// 14
	vu32 ODR15;			// 15
	vu32 Reserved[16];	// 31:16
}GPIO_ODR_REG, *pGPIO_ODR_REG;
/*------------------------------------------------------------------------------------*/
// BRy: 清除端口x的位y (y = 0… 15)
// 这些位只能写入并只能以字(16位)的形式操作。
// 0：对对应的ODRy位不产生影响
// 1：清除对应的ODRy位为0
// BSy: 设置端口x的位y (y = 0… 15)
// 这些位只能写入并只能以字(16位)的形式操作。
// 0：对对应的ODRy位不产生影响
// 1：设置对应的ODRy位为1		 
//--------------------------------------------------------------------------------------
typedef struct _GPIO_BSRR_REG 
{
	vu32 BS0;			// 0
	vu32 BS1;			// 1
	vu32 BS2;			// 2
	vu32 BS3;			// 3
	vu32 BS4;			// 4
	vu32 BS5;			// 5
	vu32 BS6;			// 6
	vu32 BS7;			// 7
	vu32 BS8;			// 8
	vu32 BS9;			// 9
	vu32 BS10;			// 10
	vu32 BS11;			// 11
	vu32 BS12;			// 12
	vu32 BS13;			// 13
	vu32 BS14;			// 14
	vu32 BS15;			// 15
	vu32 BR0;			// 16
	vu32 BR1;			// 17
	vu32 BR2;			// 18
	vu32 BR3;			// 19
	vu32 BR4;			// 20
	vu32 BR5;			// 21
	vu32 BR6;			// 22
	vu32 BR7;			// 23
	vu32 BR8;			// 24
	vu32 BR9;			// 25
	vu32 BR10;			// 26
	vu32 BR11;			// 27
	vu32 BR12;			// 28
	vu32 BR13;			// 29
	vu32 BR14;			// 30
	vu32 BR15;			// 31
}GPIO_BSRR_REG, *pGPIO_BSRR_REG;
/*------------------------------------------------------------------------------------*/
// BRy: 清除端口x的位y (y = 0… 15)
// 这些位只能写入并只能以字(16位)的形式操作。
// 0：对对应的ODRy位不产生影响
// 1：清除对应的ODRy位为0		   
//--------------------------------------------------------------------------------------
typedef struct _GPIO_BRR_REG 
{
	vu32 BR0;			// 0
	vu32 BR1;			// 1
	vu32 BR2;			// 2
	vu32 BR3;			// 3
	vu32 BR4;			// 4
	vu32 BR5;			// 5
	vu32 BR6;			// 6
	vu32 BR7;			// 7
	vu32 BR8;			// 8
	vu32 BR9;			// 9
	vu32 BR10;			// 0
	vu32 BR11;			// 11
	vu32 BR12;			// 12
	vu32 BR13;			// 13
	vu32 BR14;			// 14
	vu32 BR15;			// 15
	vu32 Reserved[16];	// 31:16
}GPIO_BRR_REG, *pGPIO_BRR_REG;
/*------------------------------------------------------------------------------------*/
// LCKK：锁键
// 该位可随时读出，它只可通过锁键写入序列修改。
// 0：端口配置锁键位未激活
// 1：端口配置锁键位被激活，下次系统复位前GPIOx_LCKR寄存器被锁住。
// LCKy: 端口x的锁位y (y = 0… 15)
// 这些位可读可写但只能在LCKK位为0时写入。
// 0：不锁定端口的配置
// 1：锁定端口的配置			   
//--------------------------------------------------------------------------------------
typedef struct _GPIO_LCKR_REG 
{
	vu32 LCK0;			// 0
	vu32 LCK1;			// 1
	vu32 LCK2;			// 2
	vu32 LCK3;			// 3
	vu32 LCK4;			// 4
	vu32 LCK5;			// 5
	vu32 LCK6;			// 6
	vu32 LCK7;			// 7
	vu32 LCK8;			// 8
	vu32 LCK9;			// 9
	vu32 LCK10;			// 10
	vu32 LCK11;			// 11
	vu32 LCK12;			// 12
	vu32 LCK13;			// 13
	vu32 LCK14;			// 14
	vu32 LCK15;			// 15
	vu32 LCKK;			// 16
	vu32 Reserved[15];	// 31:17
}GPIO_LCKR_REG, *pGPIO_LCKR_REG;  	

typedef struct _BIT_REG 
{
	vu32 bit0;		//
	vu32 bit1;		//
	vu32 bit2;		//
	vu32 bit3;		//
	vu32 bit4;		//
	vu32 bit5;		//
	vu32 bit6;		//
	vu32 bit7;		//
	vu32 bit8;		//
	vu32 bit9;		//	
	vu32 bit10;		//	
	vu32 bit11;		//	
	vu32 bit12;		//	
	vu32 bit13;		//	
	vu32 bit14;		//	
	vu32 bit15;		//	
	vu32 bit16;		//	
	vu32 bit17;		//	
	vu32 bit18;		//	
	vu32 bit19;		//	
	vu32 bit20;		//	
	vu32 bit21;		//	
	vu32 bit22;		//	
	vu32 bit23;		//	
	vu32 bit24;		//	
	vu32 bit25;		//	
	vu32 bit26;		//	
	vu32 bit27;		//	
	vu32 bit28;		//	
	vu32 bit29;		//	
	vu32 bit30;		//	
	vu32 bit31;		//	
}BIT_REG, *pBIT_REG;

typedef struct _B_GPIO_REG 
{
	GPIO_CRL_REG  	CRL;			// 端口配置低寄存器 (GPIOx_CRL) (x=A..E)
	GPIO_CRH_REG  	CRH;			// 端口配置高寄存器 (GPIOx_CRH) (x=A..E)
	BIT_REG		  		IDR;			// 端口输入数据寄存器 (GPIOx_IDR) (x=A..E)
	BIT_REG		  		ODR;			// 端口输出数据寄存器 (GPIOx_ODR) (x=A..E)
	GPIO_BSRR_REG 	BSRRL;			// 端口位设置/复位寄存器 (GPIOx_BSRR)(x=A..E)
	GPIO_BRR_REG  	BSRRH;			// 端口位复位寄存器 (GPIOx_BRR) (x=A..E)
	GPIO_LCKR_REG 	LCKR;			// 端口配置锁定寄存器 (GPIOx_LCKR)(x=A..E)
}B_GPIO_REG, *pB_GPIO_REG;

/* -----------------------------------------------------------------------------------*/ 
#define cu8(da)			(*((u8   *)(&(da))))	/* 强制转换成 8U格式                  */
#define cs8(da)			(*((s8   *)(&(da))))	/* 强制转换成 8S格式                  */
#define cu16(da)		(*((u16  *)(&(da))))	/* 强制转换成16U格式                  */
#define cs16(da)		(*((s16  *)(&(da))))	/* 强制转换成16S格式                  */
#define cu32(da)		(*((u32  *)(&(da))))	/* 强制转换成32U格式                  */
#define cs32(da)		(*((s32  *)(&(da))))	/* 强制转换成32S格式                  */
#define cu64(da)		(*((u64  *)(&(da))))	/* 强制转换成64U格式                  */
#define cs64(da)		(*((s64  *)(&(da))))	/* 强制转换成64S格式                  */
#define cf32(da)		(*((fp32 *)(&(da))))	/* 强制转换成单精浮点                 */
#define cf64(da)		(*((fp64 *)(&(da))))	/* 强制转换成双精浮点                 */
/* -----------------------------------------------------------------------------------*/  
#define cu8p(da)		(*((u8   *)(da)))		/* 强制转换成 8U格式指针              */
#define cs8p(da)		(*((s8   *)(da)))		/* 强制转换成 8S格式指针              */
#define cu16p(da)		(*((u16  *)(da)))		/* 强制转换成16U格式指针              */
#define cs16p(da)		(*((s16  *)(da)))		/* 强制转换成16S格式指针              */
#define cu32p(da)		(*((u32  *)(da)))		/* 强制转换成32U格式指针              */
#define cs32p(da)		(*((s32  *)(da)))		/* 强制转换成32S格式指针              */
#define cu64p(da)		(*((u64  *)(da)))		/* 强制转换成64U格式指针              */
#define cs64p(da)		(*((s64  *)(da)))		/* 强制转换成64S格式指针              */
#define cf32p(da)		(*((fp32 *)(da)))		/* 强制转换成单精浮点指针             */
#define cf64p(da)		(*((fp64 *)(da)))		/* 强制转换成双精浮点指针             */ 
/* -----------------------------------------------------------------------------------*/ 
#define cvu8(da)		(*((vu8  *)(&(da))))	/* 强制转换成 8U格式                  */
#define cvs8(da)		(*((vs8  *)(&(da))))	/* 强制转换成 8S格式                  */
#define cvu16(da)		(*((vu16 *)(&(da))))	/* 强制转换成16U格式                  */
#define cvs16(da)		(*((vs16 *)(&(da))))	/* 强制转换成16S格式                  */
#define cvu32(da)		(*((vu32 *)(&(da))))	/* 强制转换成32U格式                  */
#define cvs32(da)		(*((vs32 *)(&(da))))	/* 强制转换成32S格式                  */
#define cvu64(da)		(*((vu64 *)(&(da))))	/* 强制转换成64U格式                  */
#define cvs64(da)		(*((vs64 *)(&(da))))	/* 强制转换成64S格式                  */
#define cvf32(da)		(*((vfp32*)(&(da))))	/* 强制转换成单精浮点                 */
#define cvf64(da)		(*((vfp64*)(&(da))))	/* 强制转换成双精浮点                 */ 
/* -----------------------------------------------------------------------------------*/ 
#define cvu8p(da)		(*((vu8  *)(da)))		/* 强制转换成 8U格式                  */
#define cvs8p(da)		(*((vs8  *)(da)))		/* 强制转换成 8S格式                  */
#define cvu16p(da)		(*((vu16 *)(da)))		/* 强制转换成16U格式                  */
#define cvs16p(da)		(*((vs16 *)(da)))		/* 强制转换成16S格式                  */
#define cvu32p(da)		(*((vu32 *)(da)))		/* 强制转换成32U格式                  */
#define cvs32p(da)		(*((vs32 *)(da)))		/* 强制转换成32S格式                  */
#define cvu64p(da)		(*((vu64 *)(da)))		/* 强制转换成64U格式                  */
#define cvs64p(da)		(*((vs64 *)(da)))		/* 强制转换成64S格式                  */
#define cvf32p(da)		(*((vfp32*)(da)))		/* 强制转换成单精浮点                 */
#define cvf64p(da)		(*((vfp64*)(da)))		/* 强制转换成双精浮点                 */ 
/* -----------------------------------------------------------------------------------*/  	 
#define cb(d,b)			(*((u8 *)(((u32)(&(d))) +(b))))	/* 获取第b 个字节   	      */	 
#define cb0(da)			(*((u8 *)(((u32)(&(da)))    )))	/* 获取第一个字节   	      */	  	 
#define cb1(da)			(*((u8 *)(((u32)(&(da)))+ 1 )))	/* 获取第二个字节   	      */  	 
#define cb2(da)			(*((u8 *)(((u32)(&(da)))+ 2 )))	/* 获取第三个字节   	      */  	 
#define cb3(da)			(*((u8 *)(((u32)(&(da)))+ 3 )))	/* 获取第四个字节   	      */    
#define cw(d,b)			(*((u16*)(((u32)(&(d))) +(b))))	/* 获取第b 个字     	      */	
#define cw0(da)			(*((u16*)(((u32)(&(da)))    )))	/* 获取第一个字     	      */  	 
#define cw1(da)			(*((u16*)(((u32)(&(da)))+ 2 )))	/* 获取第二个字     	      */ 
/* -----------------------------------------------------------------------------------*/ 

/*------------------------------------------------------------------------------------*/
/***************************************************************************************
** 功能描述: GPIO相关定义
** 使用方法：															   
**			if(B_GPIOA->IDR.bit0 == 0)
**				B_GPIOA->ODR.bit1 = 1;	
***************************************************************************************/ 
#define B_GPIOA		((pB_GPIO_REG)B_GPIOA_BASE)	
#define B_GPIOB		((pB_GPIO_REG)B_GPIOB_BASE)	
#define B_GPIOC		((pB_GPIO_REG)B_GPIOC_BASE)	
#define B_GPIOD		((pB_GPIO_REG)B_GPIOD_BASE)	
#define B_GPIOE		((pB_GPIO_REG)B_GPIOE_BASE)	
#define B_GPIOF		((pB_GPIO_REG)B_GPIOF_BASE)	
#define B_GPIOG		((pB_GPIO_REG)B_GPIOG_BASE)	
																			





#define		GetPointIndex(n,p)			(*(p+n))
#define 	ClrPointIndex(n,p,m)		(*(p+n)=m)




#define 	Close_TI()		__set_PRIMASK(1) //关闭总中断
#define 	Open_TI()   	__set_PRIMASK(0) //打开总中断


#define		CountOf(a)     (sizeof(a) / sizeof(*(a)))	/* 计算数组中变量的个数,非字节数  */ 











#endif /* __my_def_H */					  
/***************************************************************************************
**  End Of File                                                     
***************************************************************************************/
