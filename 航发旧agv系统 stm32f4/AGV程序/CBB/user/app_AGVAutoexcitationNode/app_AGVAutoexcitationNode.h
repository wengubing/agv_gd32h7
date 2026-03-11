/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	app_AGVAutoexcitationNode.h   
  * Version    :	v1.0		
  * Author     :	Master	
  * Date       :	2025-02-08         
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
#include "STM32f4xx.h"

/*define-----------------------------------------------------------------------*/


#ifndef __APP_AGV_AUTOEXCITATION_NODE_H_
#define __APP_AGV_AUTOEXCITATION_NODE_H_

#ifdef  APP_AGV_AUTOEXCITATION_NODE
	#define APP_AGV_AUTOEXCITATION_NODE_GLOBALS_EXT
#else
	#define  APP_AGV_AUTOEXCITATION_NODE_GLOBALS_EXT extern
#endif

	//最大路径点数量
#define MAX_NODE_NUM 3
#define MAX_Path_NUM 1

typedef struct AutoExcitation_Point {

		uint16_t	Tag;			//本节点Tag
		int32_t		X;
		int32_t		Y;
		float		Z;
		uint16_t	High;
		uint8_t		Navigate;
		uint8_t		AccuratePosition;
} AutoExcitation_Point_Type;
	
typedef struct Autoexcitation_Node {

	uint8_t	index;				//本节点在顺序存储中的位置
	AutoExcitation_Point_Type	Point;

	struct{
		struct Autoexcitation_Node* 	parent; 	// 父节点
		float							angle;		// 父节点在此节点的方向角度
		struct Autoexcitation_Node* 	child1; 	// 子节点1
		struct Autoexcitation_Node* 	child2; 	// 子节点2
		struct Autoexcitation_Node* 	child3; 	// 子节点3
	}Path;
} AutoExcitation_Node_Type;

typedef struct Node {
	
	/*索引表，是个得到数组下标的表格*/
	uint16_t					TagList[MAX_NODE_NUM];
	uint8_t 					AE_Node_num;
	/*真实链表*/
	AutoExcitation_Node_Type	AE_Node[MAX_NODE_NUM];
} AutoExcitation_NodeList_Type;


//状态
typedef enum
{
	AutoExcitation_STM_Init 		= 0,	//初始状态判断
	AutoExcitation_STM_WaitInput	= 1,	//等待命令输入
	AutoExcitation_STM_WaitStable 	= 2,	//等待输入命令稳定
	AutoExcitation_STM_CheckPathNum	= 3,	//检验路径终点 是否在预设路径中
	AutoExcitation_STM_CheckEndTag 	= 4,	//检验路径终点 是否在预设路径中
	AutoExcitation_STM_GetStartTag 	= 5,	//自动解析路径 起始点
	AutoExcitation_STM_CmdParser	= 6,	//命令解析，生成路径中
	AutoExcitation_STM_Accomplish	= 7,	//动作完成

}AutoExcitation_STM_TypeDef;

//控制参数对象定义
typedef struct
{
	uint8_t							Path_MaxNum					; 	//最大路径数量
	uint16_t						*p_DHR						;	//路径列表填充的地址	
	AutoExcitation_NodeList_Type 	AE_NodeList[MAX_Path_NUM]	;	//路径结果
}AutoExcitation_Para_TypeDef;

/*输入结果*/
typedef struct
{
	
	uint8_t		SelectionPath_Num	;	//选择的路径编号
	//起点信息
	int32_t		Rtx					;	//实时坐标 单位mm
	int32_t		Rty					;	//
	uint16_t	RtTag				;	//Tag编号
	//终点信息
	uint16_t	EndTag				;	//终点Tag编号
}AutoExcitation_CtrlIn_TypeDef;
//状态
typedef enum
{
	AutoExcitation_CMD_Normal 		= 0,	//正常、长期状态
	AutoExcitation_CMD_Tag_Launch	= 1,	//标签启动
	AutoExcitation_CMD_Coord_Launch	= 2,	//定位坐标启动

}AutoExcitation_CMD_TypeDef;
/*中间过程*/
typedef struct
{
	uint32_t						TempMs			;	//缓存毫秒计数
	AutoExcitation_CMD_TypeDef		Temp_CMD		;
	AutoExcitation_CtrlIn_TypeDef	Temp_In			;	//输入指令缓存
	
	AutoExcitation_NodeList_Type	*p_NodeList	;
	AutoExcitation_Point_Type		*p_PathStartPoint;
	AutoExcitation_Point_Type		*p_PathEndPoint	;

	uint16_t						Path_StartTag	;	//路径起点 Tag
	uint16_t						Path_EndTag		;	//路径终点 Tag
	uint8_t							Towards			;	//朝向
	
	
}AutoExcitation_Temp_TypeDef;

/*故障输出*/
typedef struct
{
	//通过联合体，可按Bit访问或者字节访问
	union
	{
		struct
		{
			uint8_t							CMD_Shake	:1;	//输入抖动
			uint8_t							EndErr		:1;	//终点异常
			uint8_t							StartErr	:1;	//起点异常
		}Bit;
		uint8_t	All;
	}Err; 
}AutoExcitation_Err_TypeDef;

typedef struct
{
	//初始化参数
	AutoExcitation_Para_TypeDef 		AE_Para		;	
	uint8_t								AE_InitFlag	; 	//初始化标志，需要0XAA 才能进入正常流程
	//传入参数
	uint32_t							AE_Ms		;	//输入毫秒计数
	AutoExcitation_CtrlIn_TypeDef		AE_In		;	//输入内容
	AutoExcitation_CMD_TypeDef			AE_CMD		;	//启动路径生成的指令
	//中间变量
	AutoExcitation_STM_TypeDef			AE_STM		;	//状态机
	AutoExcitation_Temp_TypeDef			AE_Temp		;
	//输出
	AutoExcitation_Err_TypeDef			AE_Err		;	//故障输出

}AutoExcitation_TypeDef;


APP_AGV_AUTOEXCITATION_NODE_GLOBALS_EXT	void AGVAutoexcitationNodeMain (AutoExcitation_TypeDef *p);
APP_AGV_AUTOEXCITATION_NODE_GLOBALS_EXT	uint8_t AGVAutoexcitationCreateNodePath(AutoExcitation_NodeList_Type* NodeList,
						uint16_t Tag, uint16_t parentTag, float angle);
APP_AGV_AUTOEXCITATION_NODE_GLOBALS_EXT	uint8_t AGVAutoexcitationCreateNodePoint(AutoExcitation_NodeList_Type *NodeList,
						uint16_t Tag, int32_t pointX, int32_t pointY, float pointZ,
						uint8_t Navigate,uint8_t AccuratePosition);
#endif


 

