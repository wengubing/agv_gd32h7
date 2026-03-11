/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	app_SlamTrackLine.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-09-29
	* Description:
	******************************************************************************
**/

#include "app_SlamTrackLine.h"

#define  app_SlamTrackLinee_GLOBALS

/***************************************************************************************
** 函数名称: Remote_SlamTrackLine_init
** 功能描述: Slam遥控巡线的配置初始化
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void Remote_SlamTrackLine_init(SlamTrackLine_TypeDef *p)
{
	//巡线约束
	{
		//蒙皮
		p->TrackLine[0].org_X = 28433;
		p->TrackLine[0].org_Y = 10523;
		p->TrackLine[0].des_X = 37179;
		p->TrackLine[0].des_Y = 10575;

		//手工
		p->TrackLine[1].org_X = 28424;//不准确
		p->TrackLine[1].org_Y = 15726;
		p->TrackLine[1].des_X = 37200;
		p->TrackLine[1].des_Y = 15778;
		
		//卡板
		p->TrackLine[2].org_X = 28644;
		p->TrackLine[2].org_Y = 19468;
		p->TrackLine[2].des_X = 37225;
		p->TrackLine[2].des_Y = 19468;
		
		//缓存7
		p->TrackLine[3].org_X = 18602;
		p->TrackLine[3].org_Y = 22428;
		p->TrackLine[3].des_X = 24040;
		p->TrackLine[3].des_Y = 22446;
		
		//自动涂胶
		p->TrackLine[4].org_X = 3724;
		p->TrackLine[4].org_Y = 15285;
		p->TrackLine[4].des_X = 12277;
		p->TrackLine[4].des_Y = 15281;
		
		//缓存6
		p->TrackLine[5].org_X = 5537;
		p->TrackLine[5].org_Y = 22447;
		p->TrackLine[5].des_X = 14073;
		p->TrackLine[5].des_Y = 22459;
		
		//缓存5
		p->TrackLine[6].org_X = 5304;
		p->TrackLine[6].org_Y = 25018;
		p->TrackLine[6].des_X = 10755;
		p->TrackLine[6].des_Y = 25027;
		
		//缓存4
		p->TrackLine[7].org_X = 5509;// 缓存4 不准确
		p->TrackLine[7].org_Y = 27600;
		p->TrackLine[7].des_X = 14039;
		p->TrackLine[7].des_Y = 27619;
		
		//长桁
		p->TrackLine[8].org_X = -10336;//长桁自动定位区 
		p->TrackLine[8].org_Y =  10624;
		p->TrackLine[8].des_X =  1100;
		p->TrackLine[8].des_Y =  10624;
		
		//预链接
		p->TrackLine[9].org_X = -10480;//机器人预连接区  待确定
		p->TrackLine[9].org_Y =  15230;
		p->TrackLine[9].des_X = -1736;
		p->TrackLine[9].des_Y =  15274;
		
		//缓存3
		p->TrackLine[10].org_X = -15197;
		p->TrackLine[10].org_Y =  22408;
		p->TrackLine[10].des_X = -9756;
		p->TrackLine[10].des_Y =  22431;
		
		//缓存2
		p->TrackLine[11].org_X = -15215;
		p->TrackLine[11].org_Y =  24956;
		p->TrackLine[11].des_X = -9781;
		p->TrackLine[11].des_Y =  24959;
		
		//缓存1
		p->TrackLine[12].org_X = -15223;
		p->TrackLine[12].org_Y =  27570;
		p->TrackLine[12].des_X = -9774;
		p->TrackLine[12].des_Y =  27576;
		
		//补铆
		p->TrackLine[13].org_X = -35795;
		p->TrackLine[13].org_Y =  6508;
		p->TrackLine[13].des_X = -26982;
		p->TrackLine[13].des_Y =  6543;
		
		//自动钻铆
		p->TrackLine[14].org_X = -35630;
		p->TrackLine[14].org_Y =  12059;
		p->TrackLine[14].des_X = -26893;
		p->TrackLine[14].des_Y =  12105;
		
		//黑户
		p->TrackLine[15].org_X = -35656;
		p->TrackLine[15].org_Y =  19650;
		p->TrackLine[15].des_X = -26849;
		p->TrackLine[15].des_Y =  19650;
	}
		

	//区域约束，从巡线约束中自动计算得到
	{
		//蒙皮
		p->WorkstationArea[0].UpperLeft_X 		= p->TrackLine[0].org_X- 16000;
		p->WorkstationArea[0].UpperLeft_Y 		= p->TrackLine[0].org_Y+ 1000;
		p->WorkstationArea[0].BottomRight_X 	= p->TrackLine[0].des_X+ 2000;
		p->WorkstationArea[0].BottomRight_Y 	= p->TrackLine[0].des_Y- 1000;

		//手工
		p->WorkstationArea[1].UpperLeft_X 		= p->TrackLine[1].org_X- 16000;
		p->WorkstationArea[1].UpperLeft_Y 		= p->TrackLine[1].org_Y+ 1000;
		p->WorkstationArea[1].BottomRight_X 	= p->TrackLine[1].des_X+ 2000;
		p->WorkstationArea[1].BottomRight_Y 	= p->TrackLine[1].des_Y- 1000;
		
		//卡板
		p->WorkstationArea[2].UpperLeft_X 		= p->TrackLine[2].org_X- 8000;//暂时随便定的
		p->WorkstationArea[2].UpperLeft_Y 		= p->TrackLine[2].org_Y+ 1000;
		p->WorkstationArea[2].BottomRight_X		= p->TrackLine[2].des_X+ 2000;
		p->WorkstationArea[2].BottomRight_Y 	= p->TrackLine[2].des_Y- 1000;
		
		//缓存7
		p->WorkstationArea[3].UpperLeft_X 		= p->TrackLine[3].org_X- 1000;
		p->WorkstationArea[3].UpperLeft_Y 		= p->TrackLine[3].org_Y+ 1000;
		p->WorkstationArea[3].BottomRight_X 	= p->TrackLine[3].des_X+ 1000;
		p->WorkstationArea[3].BottomRight_Y 	= p->TrackLine[3].des_Y- 1000;
		
		//自动涂胶
		p->WorkstationArea[4].UpperLeft_X		= p->TrackLine[4].org_X- 2000;
		p->WorkstationArea[4].UpperLeft_Y		= p->TrackLine[4].org_Y+ 1000;
		p->WorkstationArea[4].BottomRight_X 	= p->TrackLine[4].des_X+ 8000;//暂时随便定的
		p->WorkstationArea[4].BottomRight_Y 	= p->TrackLine[4].des_Y- 1000;
		
		//缓存6
		p->WorkstationArea[5].UpperLeft_X 		= p->TrackLine[5].org_X- 7646;//确保缓存6和缓存3可以连起来
		p->WorkstationArea[5].UpperLeft_Y 		= p->TrackLine[5].org_Y+ 1000;
		p->WorkstationArea[5].BottomRight_X 	= p->TrackLine[5].des_X+ 2000;
		p->WorkstationArea[5].BottomRight_Y 	= p->TrackLine[5].des_Y- 1000;
		
		//缓存5
		p->WorkstationArea[6].UpperLeft_X 		= p->TrackLine[6].org_X- 7524;//确保缓存2和缓存5可以连起来
		p->WorkstationArea[6].UpperLeft_Y 		= p->TrackLine[6].org_Y+ 1000;
		p->WorkstationArea[6].BottomRight_X 	= p->TrackLine[6].des_X+ 2000;
		p->WorkstationArea[6].BottomRight_Y 	= p->TrackLine[6].des_Y- 1000;

		//缓存4
		p->WorkstationArea[7].UpperLeft_X 		= p->TrackLine[7].org_X- 7642;//确保缓存1和缓存4可以连起来
		p->WorkstationArea[7].UpperLeft_Y 		= p->TrackLine[7].org_Y+ 1000;
		p->WorkstationArea[7].BottomRight_X 	= p->TrackLine[7].des_X+ 2000;
		p->WorkstationArea[7].BottomRight_Y 	= p->TrackLine[7].des_Y- 1000;
		
		//长桁
		p->WorkstationArea[8].UpperLeft_X 		= p->TrackLine[8].org_X- 16000;
		p->WorkstationArea[8].UpperLeft_Y 		= p->TrackLine[8].org_Y+ 500;	//确保和自动钻铆没有干涉
		p->WorkstationArea[8].BottomRight_X 	= p->TrackLine[8].des_X+ 5000;
		p->WorkstationArea[8].BottomRight_Y 	= p->TrackLine[8].des_Y- 1000;
		
		//预链接
		p->WorkstationArea[9].UpperLeft_X 		= p->TrackLine[9].org_X- 16000;
		p->WorkstationArea[9].UpperLeft_Y 		= p->TrackLine[9].org_Y+ 1000;
		p->WorkstationArea[9].BottomRight_X 	= p->TrackLine[9].des_X+ 1000;
		p->WorkstationArea[9].BottomRight_Y 	= p->TrackLine[9].des_Y- 1000;
		
		//缓存3
		p->WorkstationArea[10].UpperLeft_X 		= p->TrackLine[10].org_X- 2000;
		p->WorkstationArea[10].UpperLeft_Y 		= p->TrackLine[10].org_Y+ 1000;
		p->WorkstationArea[10].BottomRight_X 	= p->TrackLine[10].des_X+ 7646;//确保缓存6和缓存3可以连起来
		p->WorkstationArea[10].BottomRight_Y 	= p->TrackLine[10].des_Y- 1000;
		
		//缓存2
		p->WorkstationArea[11].UpperLeft_X		= p->TrackLine[11].org_X- 2000;
		p->WorkstationArea[11].UpperLeft_Y		= p->TrackLine[11].org_Y+ 1000;
		p->WorkstationArea[11].BottomRight_X 	= p->TrackLine[11].des_X+ 7524;//确保缓存2和缓存5可以连起来
		p->WorkstationArea[11].BottomRight_Y 	= p->TrackLine[11].des_Y- 1000;
		
		//缓存1
		p->WorkstationArea[12].UpperLeft_X 		= p->TrackLine[12].org_X- 2000;
		p->WorkstationArea[12].UpperLeft_Y		= p->TrackLine[12].org_Y+ 1000;
		p->WorkstationArea[12].BottomRight_X	= p->TrackLine[12].des_X+ 7642;//确保缓存1和缓存4可以连起来
		p->WorkstationArea[12].BottomRight_Y	= p->TrackLine[12].des_Y- 1000;
		
		//补铆
		p->WorkstationArea[13].UpperLeft_X		= p->TrackLine[13].org_X- 2000;
		p->WorkstationArea[13].UpperLeft_Y		= p->TrackLine[13].org_Y+ 1000;
		p->WorkstationArea[13].BottomRight_X	= p->TrackLine[13].des_X+ 16000;
		p->WorkstationArea[13].BottomRight_Y	= p->TrackLine[13].des_Y- 1000;
		
		//自动钻铆
		p->WorkstationArea[14].UpperLeft_X 		= p->TrackLine[14].org_X- 2000;
		p->WorkstationArea[14].UpperLeft_Y 		= p->TrackLine[14].org_Y+ 1000;
		p->WorkstationArea[14].BottomRight_X 	= p->TrackLine[14].des_X+ 16000;
		p->WorkstationArea[14].BottomRight_Y 	= p->TrackLine[14].des_Y- 500;	//确保和长桁没有干涉
		
		//黑户
		p->WorkstationArea[15].UpperLeft_X 		= p->TrackLine[15].org_X- 2000;
		p->WorkstationArea[15].UpperLeft_Y 		= p->TrackLine[15].org_Y+ 1000;
		p->WorkstationArea[15].BottomRight_X 	= p->TrackLine[15].des_X+ 16000;
		p->WorkstationArea[15].BottomRight_Y 	= p->TrackLine[15].des_Y- 1000;
	}
}

/***************************************************************************************
** 函数名称: Remote_SlamTrackLine
** 功能描述: Slam遥控巡线
** 参    数: 
** 返 回 值: None
****************************************************************************************/
void app_Remote_SlamTrackLine(SlamTrackLine_TypeDef *p)
{
	static uint8_t Flag = 0;
	uint8_t i = MaxSlamTrackLine;

	//初始化
	if(Flag == 0)
	{
		Flag = 1;
		//Remote_SlamTrackLine_init(p);
	}
	else
	{
		p->SlamTrackLine_STA = 0;
		//通过输入坐标匹配在那个区域 并输出
		for(i=0;i<MaxSlamTrackLine;i++)
		{
			if(   p->Rtx > p->WorkstationArea[i].UpperLeft_X   && p->Rtx < p->WorkstationArea[i].BottomRight_X
				&&p->Rty > p->WorkstationArea[i].BottomRight_Y && p->Rty < p->WorkstationArea[i].UpperLeft_Y   )
			{
				p->WorkstationArea_Index = i;
				i = 100;
				p->SlamTrackLine_STA = 1;
			}
		}
	}

	/*整合输出*/
	{
		TrackLine_TypeDef *p_Out = &(p->Remote_SlamTrackLine);
		if(p->SlamTrackLine_STA == 1)
		{
			uint8_t WorkstationArea_Index = p->WorkstationArea_Index;
			p_Out->org_X = p->TrackLine[WorkstationArea_Index].org_X;
			p_Out->org_Y = p->TrackLine[WorkstationArea_Index].org_Y;
			p_Out->des_X = p->TrackLine[WorkstationArea_Index].des_X;
			p_Out->des_Y = p->TrackLine[WorkstationArea_Index].des_Y;
		}
		//算法出错的情况
		else
		{
			p->WorkstationArea_Index = 0;
			p_Out->org_X = 0;
			p_Out->org_Y = 0;
			p_Out->des_X = 0;
			p_Out->des_Y = 0;
		}
	}
	
}



/************************************END OF FILE************************************/
