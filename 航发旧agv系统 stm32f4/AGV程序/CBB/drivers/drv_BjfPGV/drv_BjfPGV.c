/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: drv_BjfPGV.h
** 创 建 人: Master 
** 描    述: 
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2025年3月29日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define DRV_BJFPGV_GLOBALS
#include "drv_BjfPGV.h"
#include "string.h"


/***********************************************************
** 函数名称:	BjfPGV_DataAnalysis
** 功能描述: 
** 参    数:	None
** 返 回 值:		0-数据核验通过  其他- 不通过，详见注释
***********************************************************/
uint8_t BjfPGV_DataAnalysis(uint8_t* p_buff,uint16_t buff_num)
{
	uint8_t 	res = 0x00;
	do{
		/*检验数据长度*/
	//	if(buff_num != 21)			{res = 1;p_buff[0]=0;break;}

	}while(0);

	return res;
}

/***********************************************************
** 函数名称:	BjfPGV_DateTransform
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//
void BjfPGV_DateTransform(EQP_BJF_PGVObj_StDef *Data)
{
	EQP_BJF_PGVOffsetObj_StDef *p_Offset = &(Data->Offset);

	float	Anglevalue = Data->Anglevalue;

		if(Anglevalue >= 0.0f && Anglevalue <= 45.0f )
		{
			p_Offset->X 			= (+1)*Data->Ypositon;
			p_Offset->Y 			= (+1)*Data->Xpositon;
			p_Offset->Z_Deviation 	= (+1)*(Anglevalue - 0);
		}
		else if(Anglevalue > 45.0f && Anglevalue <= 135.0f )
		{
			p_Offset->X 			= (+1)*Data->Xpositon;
			p_Offset->Y 			= (+1)*Data->Ypositon;
			p_Offset->Z_Deviation 	= (+1)*(Anglevalue - 90.0f);	
		
		}	
		else if(Anglevalue > 135.0f && Anglevalue <= 225.0f )
		{
			p_Offset->X 			= (+1)*Data->Ypositon;
			p_Offset->Y 			= (-1)*Data->Xpositon;
			p_Offset->Z_Deviation 	= (+1)*(Anglevalue - 180.0f);			
		}	
		else if(Anglevalue > 225.0f && Anglevalue <= 315.0f )
		{
			p_Offset->X 			= (+1)*Data->Xpositon;
			p_Offset->Y 			= (-1)*Data->Ypositon;
			p_Offset->Z_Deviation 	= (+1)*(Anglevalue - 270.0f);	
		}
		else if(Anglevalue > 315.0f && Anglevalue <= 360.0f )
		{
			p_Offset->X 			= (+1)*Data->Ypositon;
			p_Offset->Y 			= (+1)*Data->Xpositon;
			p_Offset->Z_Deviation 	= (+1)*(Anglevalue - 360.0f);	
		}
			 
}
/***********************************************************
** 函数名称:	BjfPGV_UpDate
** 功能描述: 
** 参    数:	None
** 返 回 值:		

*   直行向前，所有方位基于PGV行驶方向,PGV处于二维码Mark点，前后为X轴，左右为Y轴
*
*   PGV在Mark点右边时，Y值为负，Byte6的高四位为7，运算时Y =(0x4000-(Byte6*0x80+Byte7))
*   PGV在Mark点左边时，Y值为正，运算时Y =(Byte6*0x80+Byte7)
*
*   PGV在Mark点上时，顺时针，角度为0---1800，Angle =(Byte10*0x80+Byte11)
*   PGV在Mark点上时，逆时针，角度为3590---1800，Angle =(Byte10*0x80+Byte11)
*
*   PGV在Mark点前面时，X值为正，运算时X =(Byte2*0x80*0x4000+Byte3*0x4000+Byte4*0x80+Byte5)
*   PGV在Mark点后面时，X值为负，Byte2 =0x07，运算时X =(0xFFFFFF-(Byte2*0x80*0x4000+Byte3*0x4000+Byte4*0x80+Byte5))
*
*   当前Mark点的编码为Code =(Byte14*0x80*0x4000+Byte15*0x4000+Byte16*0x80+Byte17)
*   MARK点最大数值为99999999，0000 0101 1111 0101 1110 0000 1111 1111
*	MARK点在实地环境的坐标值为(Xpos, Ypos)
*   Xpos = (Code>>14)&0x01FFF;
*   Ypos = Code&0x00003FFFF;

*	函 数 名: DealNavMatrixData
*	功能说明: 解码二维码扫描信息，计算出码值、角度、偏移量信息
*	形    参:  	无
*	返 回 值: 	无
*   程序说明:所有参数都是基于控制命令为Input_Selection_Dir_Right = 1 ，Input_Selection_Dir_Left = 1，
*   直行向前，所有方位基于PGV行驶方向,PGV处于二维码Mark点，上下为Y轴，左右为X轴
*
*   PGV在Mark点下边时，Y值为负，Byte6的高四位为7，运算时Y =(0x4000-(Byte6*0x80+Byte7))
*   PGV在Mark点上边时，Y值为正，运算时Y =(Byte6*0x80+Byte7)
*
*   PGV在Mark点上时，上方为270°，右方为0°，下方位90°，左方为180°，Angle =(Byte10*0x80+Byte11)
*
*   PGV在Mark点右边时，X值为正，运算时X =(Byte2*0x80*0x4000+Byte3*0x4000+Byte4*0x80+Byte5)
*   PGV在Mark点左边时，X值为负，Byte2 =0x07，运算时X =(0xFFFFFF-(Byte2*0x80*0x4000+Byte3*0x4000+Byte4*0x80+Byte5))
*
*   当前Mark点的编码为Code =(Byte14*0x80*0x4000+Byte15*0x4000+Byte16*0x80+Byte17)
*   MARK点最大数值为99999999，0000 0101 1111 0101 1110 0000 1111 1111
*
*   PGV上位机设置格式如下：
*   X分辨率 1mmm
*   Y分辨率 1mm
*   角度分辨率0.1°  0 --- 3600
*

***********************************************************/
void BjfPGV_UpDate(uint8_t *BJG_PGV_RD,EQP_BJF_PGVObj_StDef *Data)
{
	static uint8_t temp_buff[21];

	memcpy(temp_buff,BJG_PGV_RD,sizeof(temp_buff));

	Data->State = BJF_PGV_NotData;
	if(temp_buff[0] == 0xC8)
	{
		return;
	}
	//识别状态更新
	if((temp_buff[0]&0x02) == 0x02)
	{
		
	}
	else
	{
		Data->State |= BJF_PGV_FindCODE;
	}
	if((temp_buff[1]&0x04) == 0x04)
	{
		
	}
	else
	{
		Data->State |= BJF_PGV_FindLine;
	}

	//获取原始控制码数据
//	if(0X00 != (temp_buff[14]*0x80*0x4000 + temp_buff[15]*0x4000 + temp_buff[16]*0x80 + temp_buff[17]))
//	{
//		Data->State |= BJF_PGV_MarkCode;
//	}
	//没事任何识别结果，对结构体清零
	if(Data->State == BJF_PGV_NotData)
	{
		Data->Xpositon		=	0;
		Data->Ypositon		=	0;
		Data->Anglevalue	=	0;
		Data->ControlCode	=	0;
	}
	else
	{
		//计算X位置
		if(temp_buff[2] == 0x07)	 //PGV位于Mark点中心坐标的左边，X轴为负值
		{
			//Data->Xpositon=temp_buff[2]*0x80*0x4000+temp_buff[3]*0x4000+temp_buff[4]*0x80+temp_buff[5];
			Data->Xpositon=(0xFFFFFF-(temp_buff[2]*0x80*0x4000+temp_buff[3]*0x4000+temp_buff[4]*0x80+temp_buff[5]))*(-1);
		}
		else		             //PGV位于Mark点中心坐标的右边，X轴为正值
		{
			Data->Xpositon=temp_buff[2]*0x80*0x4000+temp_buff[3]*0x4000+temp_buff[4]*0x80+temp_buff[5];
		}

		//计算Y位置
		if(temp_buff[6]&0x70) 		 //PGV位于Mark点中心坐标的下边，Y轴为负值
		{
			Data->Ypositon=temp_buff[6]*0x80+temp_buff[7];
			Data->Ypositon=(0x4000-Data->Ypositon)*(-1);
		}
		else		             //PGV位于Mark点中心坐标的上边，Y轴为正值
		{
			Data->Ypositon=temp_buff[6]*0x80+temp_buff[7];
		}
		
		//计算角度值
		Data->Zpositon=((float)(temp_buff[10]*0x80+temp_buff[11]))/10;  

		if(Data->Zpositon == 0)
		{
			Data->Anglevalue = 0;
		}
		else
		{
			Data->Anglevalue = (-Data->Zpositon)-90.0f;
			if(Data->Anglevalue >= 360.0f)
			{
				Data->Anglevalue = Data->Anglevalue - 360.0f;
			}
			else if(Data->Anglevalue <= -360.0f)
			{
				Data->Anglevalue = (Data->Anglevalue + 720.0f);
			}
			else if(Data->Anglevalue <= 0.0f)
			{
				Data->Anglevalue = (Data->Anglevalue + 360.0f);
			}
		}
		
		//1.5  --->   1.2    --->    2    --->    1.6   --->1.5
		//最终确定为1.5
		Data->ControlCode = temp_buff[14]*0x80*0x4000 + temp_buff[15]*0x4000 + temp_buff[16]*0x80 + temp_buff[17];
		
		//将世界坐标系转换成车体坐标系
		BjfPGV_DateTransform(Data);
	}
}



//***********************************END OF FILE***********************************

