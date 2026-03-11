/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_QinnavData.h
** 创 建 人: 文小兵  
** 描    述: Qinnav差分GPS定位通信解析与坐标转换软件模块 头文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年2月22日       
**-------------------------------------------------------------------------------------
***************************************************************************************/  
/*保证以下所有内容只被同一源文件引用一次*/
#ifndef __drv_QinnavData_h__
#define __drv_QinnavData_h__

/*宏定义*/
#define QinnavData_ASCII_Max								20/*结构化ASCII消息对象单个最大长度*/
#define QinnavData_MessageNumber_Max						(sizeof(QinnavDataASCII_TypeDef) / QinnavData_ASCII_Max)//最大消息数量
#define QinnavData_BytesMini								20/*最小字节长度*/

/*控制对象实例化只被指定文件编译时实例化一次
#ifdef  WIRELESS_HANDLE_GLOBALS
	#define WIRELESS_HANDLE_EXT
#else
	#define WIRELESS_HANDLE_EXT  extern
#endif*/

/*自定义引用*/																
#include <stdbool.h>
#include <math.h>
#include "..\..\..\CBB\user\basecalfunc\common_def.h"

// 定义结构体表示二维坐标
typedef struct {
	double x;
	double y;
	double z; // 朝向角度，以度为单位
} Point2D;

/*数据解析状态定义*/
typedef enum
{
	QinnavDataRes_Norma=0,									//正常
	QinnavDataRes_ShortOfBytes,								//长度不足
	QinnavDataRes_CommaNumberErr,							//分隔符数量异常
	QinnavDataRes_CommaContinuous,							//分隔符连续
	QinnavDataRes_HeaderErr,								//帧头错误
	QinnavDataRes_EndErr,									//帧尾错误
	QinnavDataRes_CheckErr,									//校验错误
	QinnavDataRes_ShortOfMessage,							//数据段不足
}QinnavDataRes_TypeDef;

/*分割符连续状态*/
typedef enum
{
	QinnavDataRes_CommaContinuous_Init = 0,					//初始化
	QinnavDataRes_CommaContinuous_one,						//连续一个
	QinnavDataRes_CommaContinuous_two,						//连续两个
}QinnavDataRes_CommaContinuous_TypeDef;

/*结构化ASCII消息对象*/
typedef struct
{
	char Header[QinnavData_ASCII_Max];						/*此模块默认解析$GPYBM*/ //No.1
	char SerialNO[QinnavData_ASCII_Max];
	char UTC[QinnavData_ASCII_Max];
	char Lat[QinnavData_ASCII_Max];
	char Lon[QinnavData_ASCII_Max];
	char ElpHeight[QinnavData_ASCII_Max];
	char Heading[QinnavData_ASCII_Max];
	char Pitch[QinnavData_ASCII_Max];
	char Vel_N[QinnavData_ASCII_Max];
	char Vel_E[QinnavData_ASCII_Max];
	char Vel_D[QinnavData_ASCII_Max];
	char Vel_G[QinnavData_ASCII_Max];
	char CoordinateNorthing[QinnavData_ASCII_Max];
	char CoordinateEasting[QinnavData_ASCII_Max];
	char NorthDistance[QinnavData_ASCII_Max];
	char EastDistance[QinnavData_ASCII_Max];
	char PositionIndicator[QinnavData_ASCII_Max];
	char HeadingIndicator[QinnavData_ASCII_Max];
	char SVn[QinnavData_ASCII_Max];
	char DiffAge[QinnavData_ASCII_Max];
	char StationID[QinnavData_ASCII_Max];
	char Baselinelength[QinnavData_ASCII_Max];
	char SolutionSv[QinnavData_ASCII_Max];					//No.23
	//char Rolling[QinnavData_ASCII_Max];						//No.24
	unsigned char CheckCode[QinnavData_ASCII_Max];					//No.24 帧尾没有逗号分离，
	//char CRLF[QinnavData_ASCII_Max];						//No.25
}QinnavDataASCII_TypeDef;

/*数据对象定义*/
typedef struct
{
	/*初始化 单位0.1mm 0.001°*/
	s32 in_InitPointX;										/*运行地图原点相对基站位置的坐标偏移*/
	s32 in_InitPointY;
	s32 in_InitPointZ;										/*运行地图原点0度方向相对GPS定位0度方向角度偏移，slam地图的0度在GPS的0度的逆时针方向上为正*/

	s32 in_InitAntennaX;									/*以主天线为坐标原点，副天线为Y轴建立坐标系*/
	s32 in_InitAntennaY;
	s32 in_InitAntennaZ;									/*GPS移动站两只天线连线与AGV长轴夹角，车的长轴线在右边为正*/
	
	f32 PointX;												
	f32 PointY;
	f32 PointZ;										
	f32 AntennaX;
	f32 AntennaY;
	f32 AntennaZ;									

	/*调用输入*/
	char * p_InputData;										/*实时数据输入*/
	u16 DataLength;
	u32 in_Ms;												/*ms输入*/

	/*内部成员*/
	bool ManualInput;										/*手动输入使能 用于仿真调试*/
	char* p_DataASCII;										/**/
	char* p_Fields[QinnavData_MessageNumber_Max];
	short MessageNumber_Max;
	QinnavDataASCII_TypeDef MessageASCII;					/**/
	short MessageNumber;									/**/
	short CntOfComma;										/*分隔符计数*/
	QinnavDataRes_CommaContinuous_TypeDef CommaContinuous;	/*分隔符连续情况*/
	unsigned char CheckCode, CheckCodeCal;					/*校验传输值、计算值*/

	/*输出*/
	QinnavDataRes_TypeDef DataAnalysisRes;					/*数据解析状态*/

	/*基站坐标系下的移动站（主副天线连线,从主天线指向副天线）坐标（基站坐标为原点）*/
	f32 OriginalX;											/*原始为0.1mm 正北/负南，xy交换为正常数学坐标系*/
	f32 OriginalY;											/*正东/负西*/
	/*主天线在前，主副天线连线，与0度方向的夹角*/
	f32 OriginalZ;											/*0.001°[0,360°),正北方向为0度，顺时针为增*/

	/*转换到车体中心的位置*/
	f32 Transform2CarX, Transform2CarY, Transform2CarZ;

	/*相对AGV导航原点坐标X，Y；Y轴正方向为0度，逆时针转为正，范围[0,360°)*/
	f32 ResX,ResY,ResZ;										/*浮点型结果，便于仿真查看*/
	
	s32 out_X;												/*0.1mm */
	s32 out_Y;
	s32 out_Z;												/*0.001°*/
}QinnavData_TypeDef;

/*函数声明*/
QinnavDataRes_TypeDef QinnavData_Process(QinnavData_TypeDef *p,u16 len,u32 ms);
bool QinnavData_InitExample(QinnavData_TypeDef* p, char* p2str);
Point2D TransformPoint(Point2D pointA, Point2D pointB);
unsigned char CalculateBCC(const char* data, size_t length);

#endif

/************************************END OF FILE************************************/

