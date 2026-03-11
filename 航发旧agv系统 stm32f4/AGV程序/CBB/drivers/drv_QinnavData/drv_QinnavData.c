/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_QinnavData.c
** 创 建 人: 文小兵  
** 描    述: Qinnav差分GPS定位通信解析与坐标转换软件模块 源文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年2月22日
**--------------------------------------------------------------------------------------
***************************************************************************************/  
/*声明对象在此文件编译时实例化
#define  WIRELESS_HANDLE_GLOBALS*/

/*待处理
* 过程重构、函数抽象ok
* 校验、字符串处理等方法抽象整理到公共方法中
* 校验方法 长度输入处理ok
* 坐标转换 推算验证
* ascii消息为空判断处理ok
* 输入消息长度异常处理ok，strtok返回上次处理的指针位置后面去掉分割符之后的地址
*/

/*包含对象头文件*/
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>  
#include <ctype.h>  
#include "drv_QinnavData.h"

/*将角度转换为弧度*/
double DegreesToRadians(double degrees) {
	return degrees * 3.14159 / 180.0;
}

/*坐标系综合变换
* 将点A的坐标和朝向，变换到以点B为原点和以点B的朝向为正向的坐标
*/
Point2D TransformPoint(Point2D pointA, Point2D pointB)
{
	Point2D newCoordinate;
    double rotationAngle = -pointB.z;//OA向量的旋转方向和坐标轴的旋转方向相反
    // 进行平移变换
    double translatedX = pointA.x - pointB.x;
    double translatedY = pointA.y - pointB.y;

    // 进行旋转变换
    double angleInRadians = DegreesToRadians(rotationAngle);
    double rotatedX = translatedX * cos(angleInRadians) - translatedY * sin(angleInRadians);
    double rotatedY = translatedX * sin(angleInRadians) + translatedY * cos(angleInRadians);

    // 更新点A的坐标
    newCoordinate.x=rotatedX;
	newCoordinate.y=rotatedY;
	newCoordinate.z=pointA.z - pointB.z;
    return newCoordinate;
}

/*坐标系旋转变换
点A的坐标轴旋转指定角度（正为逆时针旋转）后，输出新的点的坐标，朝向不变
*/
Point2D TransformPoint_Rotate(Point2D pointA, float rotationAngle)
{
	Point2D newCoordinate;

    // 进行旋转变换
    double angleInRadians = DegreesToRadians(rotationAngle);
    newCoordinate.x = pointA.x * cos(angleInRadians) + pointA.y * sin(angleInRadians);
    newCoordinate.y = - pointA.x * sin(angleInRadians) + pointA.y * cos(angleInRadians);
	newCoordinate.z=pointA.z;
    return newCoordinate;
}

/*函数计算BCC异或校验*/
unsigned char CalculateBCC(const char* data, size_t length)
{
    unsigned char bcc = (unsigned char)data[0];
	size_t i=0;

    for (i = 1; i < length-1; i++)/*这里运算的次数 是 长度-1*/
	{
        bcc =bcc ^ (unsigned char)data[i]; // 对每个字节进行异或操作  
    }
    return bcc;
}

/*将ASCII表达的HEX（不区分大小写），转换成真实的HEX*/
unsigned int hexStringToInt(const char *hexStr,size_t len)
{  
    unsigned int result = 0;
	char i=0,c=0;
    for (i = 0; i < len; i++)
	{  
        c = hexStr[i];  
        if (isdigit(c))
		{  
            result = result * 16 + (c - '0');  
        }
		else if (isxdigit(c))
		{ // isxdigit() 检查字符是否是十六进制数字（0-9, a-f, A-F）  
            result = result * 16 + (toupper(c) - '7'); // 转换为大写并减去'0'以得到数值  
        }
		else
		{  
            //fprintf(stderr, "Invalid hexadecimal character: %c\n", c);  
            //exit(EXIT_FAILURE);  
        }  
    }  
    return result;  
}  

/*圆周角度单圈化处理*/
float MultiturnAngle2Single(const float in_Ang, const float AngMini, const float AngMax)
{
    float out_Ang = in_Ang;
    if (in_Ang < AngMini)
        out_Ang = in_Ang + 360;
    else if (in_Ang >= AngMax)
        out_Ang = in_Ang - 360;
    return out_Ang;
}

/*分割字符串消息*/
size_t  QinnavData_StrSplit(QinnavData_TypeDef* p)
{
    size_t res = 1,i=0;
    unsigned char* p2str = strtok(p->p_InputData, ",");

    //p->MessageNumber = 1;
    strcpy(&(p->p_DataASCII[0 * QinnavData_ASCII_Max]), p2str);
    for (i = 1; i < p->MessageNumber_Max; i++)
    {
        
        strcpy(&(p->p_DataASCII[i * QinnavData_ASCII_Max]), strtok(NULL, ","));/*strtok为状态函数*/
        if (*p2str == NULL )//|| *p2str == ',')
            break;
        else
            res++;
            //p->MessageNumber++;
    }
    return res;
}

/*解析状态更新*/
QinnavDataRes_TypeDef QinnavData_GitAnalysisRes(QinnavData_TypeDef* p)
{
	QinnavDataRes_TypeDef res;
    if (p->DataLength < QinnavData_BytesMini)
        p->DataAnalysisRes = QinnavDataRes_ShortOfBytes;
    else if (p->CommaContinuous == QinnavDataRes_CommaContinuous_two)
        p->DataAnalysisRes = QinnavDataRes_CommaContinuous;
    else if (p->CntOfComma != (p->MessageNumber_Max-1))
        p->DataAnalysisRes = QinnavDataRes_CommaNumberErr;
    else if (strcmp(p->MessageASCII.Header,"$GPYBM") != 0)
        p->DataAnalysisRes = QinnavDataRes_HeaderErr;
    //else if (strcmp(p->MessageASCII.CRLF, "\r\n") == 0)
    //else if(p->MessageASCII.CheckCode[3]=='\r' && p->MessageASCII.CheckCode[4] == '\n')
    else if (p->MessageASCII.CheckCode[3] == 0x0d && p->MessageASCII.CheckCode[4] == 0xd6)
        p->DataAnalysisRes = QinnavDataRes_EndErr;
    else if (p->CheckCode != p->CheckCodeCal)
        p->DataAnalysisRes = QinnavDataRes_CheckErr;
    else if (p->MessageNumber != p->MessageNumber_Max)
        p->DataAnalysisRes = QinnavDataRes_ShortOfMessage;
    else
        p->DataAnalysisRes = QinnavDataRes_Norma;
    res = p->DataAnalysisRes;
    return res;
}

/*坐标转换*/
void QinnavData_Transform(QinnavData_TypeDef* p)
{
    Point2D PointA, PointB, PointC;
    /*坐标转换 将坐标转换到车体中心*/
    PointA.x = p->AntennaX;
    PointA.y = p->AntennaY;
    PointA.z = p->AntennaZ;

    /*将主天线为原点的坐标系逆时针旋转，到与基站坐标系平行*/
    PointC = TransformPoint_Rotate(PointA, p->OriginalZ);

    /*主天线的坐标，平移到车体中心去*/
    p->Transform2CarX = p->OriginalX + PointC.x;
    p->Transform2CarY = p->OriginalY + PointC.y;

    /*主副天线的朝向，转到车体长轴的朝向*/
    p->Transform2CarZ = p->OriginalZ + p->AntennaZ;
    p->Transform2CarZ = MultiturnAngle2Single(p->Transform2CarZ, 0, 360);

    /*将坐标变换到 导航地图坐标原点
    PointA.x = p->Transform2CarX;
    PointA.y = p->Transform2CarY;
    PointA.z = p->Transform2CarZ;

    PointB.x = p->PointX;
    PointB.y = p->PointY;
    PointB.z = p->PointZ;
    PointC = TransformPoint(PointA, PointB);*/

    /*将坐标转换到，以新的点为坐标原点的坐标系*/
    PointA.x = p->Transform2CarX - p->PointX;
    PointA.y = p->Transform2CarY - p->PointY;

    /*将转换原点的新坐标，再进行坐标轴的旋转*/
    PointC = TransformPoint_Rotate(PointA, p->PointZ);

    p->ResX = PointC.x;
    p->ResY = PointC.y;

    /*车体的朝向，转换到以新坐标系的0度方向*/
    p->ResZ = p->Transform2CarZ + p->PointZ;
    p->ResZ = MultiturnAngle2Single(p->ResZ, 0, 360);
    //p->ResZ = PointC.z;

    //p->out_Z = p->out_Z - p->in_InitAntennaZ;
    //p->out_Z = MultiturnAngle2Single(p->out_Z/1000.0f,0,360)*1000;
}

/*坐标转换*/
void QinnavData_Transform2(QinnavData_TypeDef* p)
{
	Point2D PointA, PointB, PointC;
    /*坐标转换 将坐标转换到车体中心*/
    PointA.x = p->AntennaX;
    PointA.y = p->AntennaY;
    PointA.z = p->AntennaZ;

    PointC = TransformPoint_Rotate(PointA, p->AntennaZ);

    p->Transform2CarX = p->OriginalX + PointC.x;
    p->Transform2CarY = p->OriginalY + PointC.y;

    p->Transform2CarZ = p->OriginalZ - p->AntennaZ;
    p->Transform2CarZ = MultiturnAngle2Single(p->Transform2CarZ, 0, 360);

    /*将坐标变换到 导航地图坐标原点*/
    PointA.x = p->Transform2CarX;
    PointA.y = p->Transform2CarY;
    PointA.z = p->Transform2CarZ;
	
    PointB.x = p->PointX;
    PointB.y = p->PointY;
    PointB.z = p->PointZ;
    PointC = TransformPoint(PointA, PointB);

    p->ResX = PointC.x;
    p->ResY = PointC.y;
    p->ResZ = PointC.z;

    //p->out_Z = p->out_Z - p->in_InitAntennaZ;
    //p->out_Z = MultiturnAngle2Single(p->out_Z/1000.0f,0,360)*1000;
}

/*分隔符计算处理*/
void QinnavData_CommaCal(QinnavData_TypeDef* p)
{
	char i = 0;
    p->CntOfComma = 0;
    p->CommaContinuous = QinnavDataRes_CommaContinuous_Init;
    for (i = 0; i < p->DataLength; i++)
    {
        if (p->p_InputData[i] == NULL)
            break;
        else if (p->p_InputData[i] == ',')
        {
            p->CntOfComma++;
            if (p->CommaContinuous == QinnavDataRes_CommaContinuous_Init)
            {
                p->CommaContinuous = QinnavDataRes_CommaContinuous_one;
            }
            else if (p->CommaContinuous == QinnavDataRes_CommaContinuous_one)
            {
                p->CommaContinuous = QinnavDataRes_CommaContinuous_two;
                break;
            }
        }
        else
            p->CommaContinuous = QinnavDataRes_CommaContinuous_Init;
    }
}

/**************************************************************************
* 函数名: QinnavData_Process()
* 功  能: gps通信解析与坐标转换
* 参  数: 模块对象地址,ASCII消息字节数，系统ms
* 返回值: 滤波值
**************************************************************************/
QinnavDataRes_TypeDef QinnavData_Process(QinnavData_TypeDef* p, u16 len, u32 ms)
{
	QinnavDataRes_TypeDef res;
    
	//unsigned char CheckCodeTemp=0;
	
    p->DataLength = len;
    p->in_Ms = ms;
	
	/*接口中的初始化参数，输入到内部正在用的参数*/
	if(p->ManualInput==0)
	{
		p->PointX=p->in_InitPointX / 10000.0;												
		p->PointY=p->in_InitPointY / 10000.0;
		p->PointZ=p->in_InitPointZ / 1000.0;
		p->AntennaX=p->in_InitAntennaX / 10000.0;
		p->AntennaY=p->in_InitAntennaY / 10000.0;
		p->AntennaZ=p->in_InitAntennaZ / 1000.0;
	}
	
    
    if (p->DataLength >= QinnavData_BytesMini)
    {
        /*$与*号中间的数据进行异或校验,strtok调用前处理！*/
        p->CheckCodeCal = CalculateBCC(&(p->p_InputData[1]), (p->DataLength) - 6);//减掉$*HHRN0

        //分割符个数处理
        QinnavData_CommaCal(p);

        if (p->CntOfComma == (p->MessageNumber_Max-1) && p->CommaContinuous != QinnavDataRes_CommaContinuous_two)
        {
            /*分割字符串消息*/
            p->MessageNumber = QinnavData_StrSplit(p);

            /*ASCII转hex数据*/
			if(p->ManualInput==0)
			{
				p->OriginalY = atof(p->MessageASCII.NorthDistance);
				p->OriginalX = atof(p->MessageASCII.EastDistance);
				p->OriginalZ = atof(p->MessageASCII.Heading);
			}

            /*坐标转换*/
            QinnavData_Transform(p);
			
			/*结果更新到接口*/
			p->out_X = p->ResX * 10000;
			p->out_Y = p->ResY * 10000;
			p->out_Z = p->ResZ * 1000;

            /*取得校验*/
            if (p->MessageASCII.CheckCode[0] == '*')
            {
                //p->CheckCode = p->MessageASCII.CheckCode[1];
                p->CheckCode = hexStringToInt(&(p->MessageASCII.CheckCode[1]), 2);
            }
        }
    }
    
    /*解析状态更新*/
    res = QinnavData_GitAnalysisRes(p);

	return res;
}

/**************************************************************************
* 函数名:QinnavData_InitExample
* 功  能:模块单独测试时的初始化内容，实际外部使用模块时可参考编写
* 参  数:QinnavData_TypeDef *p
* 返回值:无
**************************************************************************/
bool QinnavData_InitExample(QinnavData_TypeDef* p,char *p2str)
{
    p->in_InitPointX = 1;
    p->in_InitPointY = 1;
    p->in_InitPointZ = 0;

    p->in_InitAntennaX = 0;
    p->in_InitAntennaY = 0;
    p->in_InitAntennaZ = 0;

    //p->ManualInput = 1;

    p->OriginalX = 5000;
    p->OriginalY = 2000;
    p->OriginalX = 0;
	
    p->MessageNumber_Max = QinnavData_MessageNumber_Max;
    p->p_DataASCII = (char *)(&(p->MessageASCII));
    p->p_InputData = p2str;
    return 0;
}

/************************************END OF FILE************************************/
