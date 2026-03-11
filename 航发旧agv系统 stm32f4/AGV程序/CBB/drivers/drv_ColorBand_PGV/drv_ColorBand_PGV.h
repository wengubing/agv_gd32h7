/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	drv_ColorBand_PGV.c
	* Version    :	v1.0
	* Author     :	MT
	* Date       : 2025-06-17
	* Description:
	******************************************************************************
**/


/*include----------------------------------------------------------------------*/
/*自定义引用*/

/*define-----------------------------------------------------------------------*/

#ifndef DRV_COLORBAND_PGV_H_
#define DRV_COLORBAND_PGV_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  DRV_COLORBAND_PGV_GLOBALS
	#define DRV_COLORBAND_PGV_GLOBALS_EXT
#else
	#define DRV_COLORBAND_PGV_GLOBALS_EXT  extern
#endif
	
/*自定义引用*/																
#include <stm32f4xx.h>

//色带传感器数据内容
typedef struct
{
	uint8_t		Status				;/*读状态 */
	uint8_t		Tag					;/*标签结果*/
	int16_t		X					;/*X 方向定位数据，单位1mm*/
	int16_t		Y					;/*Y 方向定位数据，单位1mm*/
	float		Z					;/*z 方向定位数据，单位1° 0-360*/
}ColorBand_output_StDef;


//色带偏差数据内容
typedef struct
{
	uint8_t Status;      										//传感器识别色带状态
	int16_t Distance;										//色带与传感器中心的距离
	
}ColorBand_Data_StDef;

//色带PGV类型选择
typedef struct
{
	uint8_t		HF_F_B_Sensor			: 1	;	/*连接方式：直连；类型：自制；安装位置：前后*/
	uint8_t		BJF_F_B_Sensor			: 1	;	/*连接方式：直连；类型：倍加福；安装位置：前后*/
	uint8_t		NCS_F_B_Sensor			: 1	;	/*连接方式：从转接板获取；类型：xxx；安装位置：前后*/


	uint8_t		HF_L_R_Sensor			: 1	;	/*连接方式：直连；类型：自制；安装位置：左右*/
	uint8_t		BJF_L_R_Sensor			: 1	;	/*连接方式：直连；类型：倍加福；安装位置：左右*/
	uint8_t		NCS_L_R_Sensor			: 1	;	/*连接方式：从转接板获取；类型：xxx；安装位置：左右*/

}ColorBandType_TypeDef;

//色带PGV类型枚举
typedef enum
{
	HF_F_B_Sensor			= 0x00,	/*连接方式：直连；类型：自制；安装位置：前后*/
	BJF_F_B_Sensor			= 0x01,	/*连接方式：直连；类型：倍加福；安装位置：前后*/
	NCS_F_B_Sensor			= 0x02,	/*连接方式：从转接板获取；类型：xxx；安装位置：前后*/


	HF_L_R_Sensor			= 0x03,	/*连接方式：直连；类型：自制；安装位置：左右*/
	BJF_L_R_Sensor			= 0x04,	/*连接方式：直连；类型：倍加福；安装位置：左右*/
	NCS_L_R_Sensor			= 0x05,	/*连接方式：从转接板获取；类型：xxx；安装位置：左右*/

}ColorBandType_EnumDef;

/*PGV数据*/
typedef struct
{
	/*初始化参数*/
	ColorBandType_TypeDef		ColorConnectionType	;	/*传感器类型，安装方式选择*/	
	ColorBandType_EnumDef		Color_Sensor		;
	float						Color_diff			;	/*传感器数据偏差*/
	/*输入*/
	void *						InPutData			;	/*数据输入接口*/
	/*输出*/
	ColorBand_output_StDef		ColorBand_output	;	/*色带传感器数据输出*/
	ColorBand_Data_StDef		ColorBand_Deviation	;	/*色带传感器归一化偏差数据输出*/
	uint16_t					Sensor_HeartbeatCnt	;	/*色带传感器心跳计数*/
	uint16_t					Sensor_ErrorCode	;	/*色带传感器错误状态*/
}ColorBand_TypeDef;


DRV_COLORBAND_PGV_GLOBALS_EXT	void DataSourceSelection(ColorBand_TypeDef *ColorBandData);

#endif

