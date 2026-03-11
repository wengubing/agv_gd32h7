/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	app_AGVAutoexcitationNode.c
	* Version    :	v1.0
	* Author     :	Master
	* Date       : 2025-02-08
	* Description:
	******************************************************************************
**/

#include "app_AGVAutoexcitationNode.h"
#include "STM32f4xx.h"
#include <stdlib.h>
#include "string.h"

#define  APP_AGV_AUTOEXCITATION_NODE_GLOBALS



//从tag解析出路径编号
uint8_t TagToIndex(AutoExcitation_NodeList_Type* NodeList,
						uint16_t Tag,uint8_t *index)
{
	uint8_t MaxNum = NodeList->AE_Node_num;
	uint8_t i = 0;
	uint16_t*	p_TagList = NodeList->TagList;
	uint8_t	found_flag = 0;
	for(i=0; (i < MaxNum && found_flag == 0);i++)
	{
		if(p_TagList[i] == Tag)
		{
			found_flag = 1;
		}
	}
	if(found_flag == 1)
	{
		*index = i-1;
		return 0;
	}
	else
	{
		return 1;
	}
	
}


// 创建新节点的路径信息函数；
//parentTag 在 Tag 的 angle方向上；每个节点只有一个父节点和至多3个子节点
uint8_t AGVAutoexcitationCreateNodePath(AutoExcitation_NodeList_Type* NodeList, 
						uint16_t Tag, uint16_t parentTag, float angle)
{
	uint8_t res = 0;
	uint8_t index = 0;
	AutoExcitation_Node_Type* newNode	= NULL;
	AutoExcitation_Node_Type* parent	= NULL;
	do{

		if(	   angle != 0.0f && angle != 90.0f 
			&& angle != 180.0f && angle != 270.0f)		{res = 1;break;}

		if(TagToIndex(NodeList,Tag,&index) != 0)		{res = 2;break;}
		newNode	= &(NodeList->AE_Node[index]);

		if(TagToIndex(NodeList,parentTag,&index) != 0)	{res = 3;break;}
		parent	= &(NodeList->AE_Node[index]);

		newNode->Path.parent 				= parent;
		if(parent->Path.child1 == NULL)
		{
			parent->Path.child1 = newNode;
		}
		else if(parent->Path.child2 == NULL)
		{
			parent->Path.child2 = newNode;
		}
		else if(parent->Path.child3 == NULL)
		{
			parent->Path.child3 = newNode;
		}
		else
		{
			res = 4;break;
		}
		newNode->Path.angle = angle; // 设置方向角度
		
	}while(0);
    return res;
}
//	创建新节点的点位信息函数
uint8_t AGVAutoexcitationCreateNodePoint(AutoExcitation_NodeList_Type *NodeList,
						uint16_t Tag, int32_t pointX, int32_t pointY, float pointZ,
						uint8_t Navigate,uint8_t AccuratePosition)
{
	uint8_t initNum = 0;
	uint8_t res = 0;
	do {
		if (NodeList->AE_Node_num >= MAX_NODE_NUM)		{ res = 1; break; }
		initNum = NodeList->AE_Node_num;
		//if (initNum > NodeList->AE_Node_num) 			{ res = 2; break; }
		if (   pointZ != 0.0f && pointZ != 90.0f 
			&& pointZ != 180.0f && pointZ != 270.0f)	{ res = 3; break; }

		NodeList->AE_Node[initNum].index 					= initNum;
		NodeList->AE_Node[initNum].Point.Tag				= Tag;
		NodeList->AE_Node[initNum].Point.X 					= pointX;
		NodeList->AE_Node[initNum].Point.Y 					= pointY;
		NodeList->AE_Node[initNum].Point.Z 					= pointZ;
		NodeList->AE_Node[initNum].Point.Navigate			= Navigate;
		NodeList->AE_Node[initNum].Point.AccuratePosition	= AccuratePosition;
		NodeList->TagList[initNum]							= Tag;
		NodeList->AE_Node[initNum].Path.parent				= NULL;
		NodeList->AE_Node[initNum].Path.child1				= NULL;
		NodeList->AE_Node[initNum].Path.child2				= NULL;
		NodeList->AE_Node[initNum].Path.child3				= NULL;
		NodeList->AE_Node[initNum].Path.angle				= 0.0f;
		NodeList->AE_Node_num++;

	} while (0);
	return res;
}
	
//解析的得到路径信息
uint8_t GetPathData(AutoExcitation_NodeList_Type* NodeList,uint16_t StartPoint,uint16_t EndPoint,uint16_t* p_Dhr)
{
	AutoExcitation_Node_Type*	p_AE_Node = NodeList->AE_Node;
	AutoExcitation_Node_Type	Temp_Node_Head[MAX_NODE_NUM];
	AutoExcitation_Node_Type	Temp_Node_Tail[MAX_NODE_NUM];
	AutoExcitation_Node_Type	Temp_Node[MAX_NODE_NUM];
	
	uint8_t res = 0;
	
	uint8_t RootFlag = 0;
	uint8_t Head_index = 0;
	uint8_t Tail_index = 0;
	uint8_t Sum = 0;

	uint8_t i = 0;
	uint8_t index = 0;
	uint8_t NodeSize = sizeof(AutoExcitation_Node_Type);
	do
	{
		memset(&(Temp_Node_Head[0]),0,sizeof(Temp_Node_Head));
		if(TagToIndex(NodeList,StartPoint,&index) != 0)				{res = 1;break;}
		memcpy(&(Temp_Node_Head[0]),(p_AE_Node+index),NodeSize);
		for(Head_index=1; (Head_index < MAX_NODE_NUM && RootFlag == 0);Head_index++)
		{
			if(Temp_Node_Head[Head_index-1].Path.parent != NULL)
			{
				memcpy(&(Temp_Node_Head[Head_index]),(Temp_Node_Head[Head_index-1].Path.parent),NodeSize);
			}
			else
			{
				RootFlag = 1;
			}
		}
		Head_index = Head_index-1;

		memset(&(Temp_Node_Tail[0]),0,sizeof(Temp_Node_Tail));
		RootFlag = 0;
		if(TagToIndex(NodeList,EndPoint,&index) != 0)				{res = 2;break;}
		p_AE_Node = NodeList->AE_Node;
		memcpy(&(Temp_Node_Tail[0]),(p_AE_Node+index),NodeSize);
		for(Tail_index=1; (Tail_index < MAX_NODE_NUM && RootFlag == 0);Tail_index++)
		{
			if(Temp_Node_Tail[Tail_index-1].Path.parent != NULL)
			{
				memcpy(&(Temp_Node_Tail[Tail_index]),(Temp_Node_Tail[Tail_index-1].Path.parent),NodeSize);
			}
			else
			{
				RootFlag = 1;
			}
		}
		Tail_index = Tail_index-1;
		{
			 // 查找最小公共祖先
			int16_t common_head_index = -1;
			int16_t common_tail_index = -1;
			uint8_t i = 0;
			uint8_t j = 0;
			uint8_t found = 0;
			// 双重循环查找相同Tag的节点
			for (i = 0; i <= Head_index && !found; i++) {
				for (j = 0; j <= Tail_index && !found; j++) {
					if (Temp_Node_Head[i].Point.Tag == Temp_Node_Tail[j].Point.Tag) {
						common_head_index = i;
						common_tail_index = j;
						found = 1;
						break;
					}
				}
			}
			if(found != 0)
			{
				Sum = common_head_index + common_tail_index + 1;
				// 合并第一部分：从头节点数组到公共祖先（正序）
				for (i = 0; i <= common_head_index; i++) {
					Temp_Node[i] = Temp_Node_Head[i];
				}

				// 合并第二部分：从公共祖先到尾节点数组的第0个元素（倒序）
				for (i = 0; i <= common_tail_index; i++) {
					uint8_t src_index = common_tail_index - i;
					uint8_t dst_index = common_head_index + i;
					
					Temp_Node[dst_index] = Temp_Node_Tail[src_index];

					//航向反向
					if(src_index >= 1)
					{
						float  Temp_angle = Temp_Node_Tail[src_index-1].Path.angle;
						if	   (Temp_angle == 000.0f)	{Temp_Node[dst_index].Path.angle = 180.0f;}
						else if(Temp_angle == 090.0f)	{Temp_Node[dst_index].Path.angle = 270.0f;}
						else if(Temp_angle == 180.0f)	{Temp_Node[dst_index].Path.angle = 000.0f;}
						else if(Temp_angle == 270.0f)	{Temp_Node[dst_index].Path.angle = 090.0f;}
						else							{Temp_Node[dst_index].Path.angle = 000.0f;}
					}
				}
				
			}
		}
		i=0;
	}while(0);
	memset(p_Dhr+121,0,30*MAX_NODE_NUM);
	
	for(i=0;i < Sum;i++)
	{
		//填充序列号
		p_Dhr[121+1 +30*i] = i+1;
		//填充Tag
		p_Dhr[121+2 +30*i] = Temp_Node[i].Point.Tag;
		//填充点位坐标、朝向、航向
		*((uint32_t*)&p_Dhr[121+3 +30*i]) = Temp_Node[i].Point.X;
		*((uint32_t*)&p_Dhr[121+5 +30*i]) = Temp_Node[i].Point.Y;
		p_Dhr[121+7 +30*i]	= Temp_Node[i].Point.Z*100;
		//填充高度结果
		if(Temp_Node[i].Point.High == 0)
		{
			p_Dhr[121+8 +30*i] = 65535;
		}
		else
		{
			p_Dhr[121+8 +30*i] = Temp_Node[i].Point.High;
		}
		p_Dhr[121+10+30*i] = Temp_Node[i].Path.angle*100;
		//填充运行速度为100 
		p_Dhr[121+11+30*i] = 0X6400;
		//填充十字路口标记
		p_Dhr[121+14+30*i] = 0X4000;

		//填充导航标记
		p_Dhr[121+24+30*i] = Temp_Node[i].Point.Navigate;
		//填充精定位标记
		p_Dhr[121+22+30*i] = Temp_Node[i].Point.AccuratePosition;

	}
	/*填充多少个点位*/
	p_Dhr[121]=Sum;

	/*填充任务开始*/
	p_Dhr[113]=1;
	
	p_Dhr[119]=1;
	return res;
}




/***********************************************************
** 函数名称:	AGVAutoexcitationNodeMain
** 功能描述: 
** 参    数:	None
** 返 回 值:	None
***********************************************************/
//自激AGV的主函数调用
void AGVAutoexcitationNodeMain (AutoExcitation_TypeDef *p) 
{

	AutoExcitation_Temp_TypeDef		*p_Temp = &(p->AE_Temp);

	switch(p->AE_STM)
	{
		default:
		case AutoExcitation_STM_Init:
		{
			if(	  p->AE_InitFlag == 0XAA 
				&&p->AE_Para.Path_MaxNum != 0
				//&&p->AE_Para.Path_MaxNum <= MAX_Path_NUM
			)
			{
				p->AE_STM = AutoExcitation_STM_WaitInput;
			}else{}
		}break;
		case AutoExcitation_STM_WaitInput:
		{
			if(p->AE_CMD != 0X00)
			{
				p_Temp->TempMs = p->AE_Ms;
				p->AE_STM = AutoExcitation_STM_WaitStable;
			}else{}
		}break;
		case AutoExcitation_STM_WaitStable:
		{
			if(p->AE_CMD != AutoExcitation_CMD_Normal)
			{
				if(p->AE_Ms >= p_Temp->TempMs + 3000)
				{
					p_Temp->Temp_CMD = p->AE_CMD;	//
					p_Temp->Temp_In  = p->AE_In;	//结构体赋值，缓存输入指令
					p->AE_STM = AutoExcitation_STM_CheckPathNum;
					//输入置0
					p->AE_CMD = AutoExcitation_CMD_Normal;
					//故障置0
					p->AE_Err.Err.All = 0;
				}else{}
			}
			else
			{
				p->AE_Err.Err.Bit.CMD_Shake = 1;
				p->AE_STM = AutoExcitation_STM_WaitInput;
			}
		}break;
		case AutoExcitation_STM_CheckPathNum:
		{
			//暂时没进行路径选择是否正常的判定，直接赋值
			p_Temp->p_NodeList = &(p->AE_Para.AE_NodeList[p_Temp->Temp_In.SelectionPath_Num]);
			p->AE_STM = AutoExcitation_STM_CheckEndTag;
		}break;
		case AutoExcitation_STM_CheckEndTag:
		{
			AutoExcitation_NodeList_Type 	*p_NodeList = &(p->AE_Para.AE_NodeList[p_Temp->Temp_In.SelectionPath_Num]);
			AutoExcitation_CtrlIn_TypeDef 	*p_Temp_In 	= &(p_Temp->Temp_In);
			uint8_t 						Node_Num 	= 0;
			uint8_t 						Max_NUM 	= p_NodeList->AE_Node_num;
			uint16_t						Node_Tag 	= 0 ;

			for(Node_Num = 0;Node_Num < Max_NUM;Node_Num++)
			{
				Node_Tag = p_NodeList->AE_Node[Node_Num].Point.Tag;
				//识别的Tag和路径点位中的tag匹配
				if(p_Temp_In->EndTag == Node_Tag)
				{
					p_Temp->p_PathEndPoint = &(p_NodeList->AE_Node[Node_Num].Point);
					//提前结束循环
					Node_Num = Max_NUM + 1;
				}
			}
			//基于路径查询结果 确定自动识别起点成功执行下一步 还是异常处理
			{
				if(Node_Num == Max_NUM + 1)
				{
					p->AE_STM = AutoExcitation_STM_GetStartTag;
				}
				else
				{
					p->AE_Err.Err.Bit.EndErr = 1;
					p->AE_STM = AutoExcitation_STM_WaitInput;
				}
			}			
			
		}break;
		case AutoExcitation_STM_GetStartTag:
		{
			AutoExcitation_NodeList_Type 	*p_NodeList = &(p->AE_Para.AE_NodeList[p_Temp->Temp_In.SelectionPath_Num]);
			AutoExcitation_CtrlIn_TypeDef 	*p_Temp_In 	= &(p_Temp->Temp_In);
			uint8_t 						Node_Num 	= 0;
			uint8_t 						Max_NUM 	= p_NodeList->AE_Node_num;
			uint16_t						Node_Tag 	= 0 ;
			if(p_Temp->Temp_CMD == AutoExcitation_CMD_Tag_Launch)
			{
				
				for(Node_Num = 0;Node_Num < Max_NUM;Node_Num++)
				{
					Node_Tag = p_NodeList->AE_Node[Node_Num].Point.Tag;
					//识别的Tag和路径点位中的tag匹配
					if(p_Temp_In->RtTag == Node_Tag)
					{
						p_Temp->p_PathStartPoint = &(p_NodeList->AE_Node[Node_Num].Point);
						p_Temp->Path_StartTag	= Node_Tag;
						//提前结束循环
						Node_Num = Max_NUM + 1;
					}
				}
			}
			else if(p_Temp->Temp_CMD == AutoExcitation_CMD_Coord_Launch)
			{
				int32_t 	Node_X 		= 0;
				int32_t 	Node_Y 		= 0;
				for(Node_Num = 0;Node_Num < Max_NUM;Node_Num++)
				{
					Node_X 		= p_NodeList->AE_Node[Node_Num].Point.X ;
					Node_Y 		= p_NodeList->AE_Node[Node_Num].Point.Y ;
					Node_Tag 	= p_NodeList->AE_Node[Node_Num].Point.Tag;
					//实时坐标在 目标点的1m半径的圆范围内
					if( ( p_Temp_In->Rtx > Node_X-1000 && p_Temp_In->Rtx < Node_X+1000
						&&p_Temp_In->Rty > Node_Y-1000 && p_Temp_In->Rty < Node_Y+1000))
					{
						p_Temp->p_PathStartPoint = &(p_NodeList->AE_Node[Node_Num].Point);
						p_Temp->Path_StartTag	= Node_Tag;
						//提前结束循环
						Node_Num = Max_NUM + 1;
					}
					
				}
			}
			else{}

			//基于自动识别起点的结果 确定生成路径还是异常处理
			{
				if(Node_Num == Max_NUM + 1)
				{
					p->AE_STM = AutoExcitation_STM_CmdParser;
				}
				else
				{
					p->AE_Err.Err.Bit.StartErr = 1;
					p->AE_STM = AutoExcitation_STM_WaitInput;
				}
			}			
			
		}break;
		case AutoExcitation_STM_CmdParser:
		{
			//根据缓存结果，计算得到路径列表的DHR，填充到初始化的DHR中
			GetPathData(p_Temp->p_NodeList,p_Temp->Path_StartTag,p_Temp->Path_EndTag,p->AE_Para.p_DHR);

			p->AE_STM = AutoExcitation_STM_Accomplish;
			
		}break;
		case AutoExcitation_STM_Accomplish:
		{
			//处理上下文，善后处理。暂时没有相关代码
			p->AE_STM = AutoExcitation_STM_WaitInput;
		}break;
	}

}

/************************************END OF FILE************************************/
