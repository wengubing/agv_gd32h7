/********************************** Copyright ***********************************
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_VoicePlayer.h
  * Version    : V1.0
  * Author     : CDHF
  * Date       : 2024.5.13
  * Description: 实现对语音播放器播放语音内容的控制
  *******************************************************************************/
//保证以下所有内容只被同一源文件引用一次
#ifndef __app_Voiceplayer_h__
#define __app_Voiceplayer_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  app_Voiceplayer_GLOBALS
	#define app_Voiceplayer_EXT
#else
	#define app_Voiceplayer_EXT  extern
#endif

//头文件
#include <stdbool.h>
#include "..\..\..\CBB\user\basecalfunc\common_def.h"

//语音播报序列号                                    
typedef enum
{
	//启动状态
	Voice_Null=0,											//0无效	
	Voice_InitErr,      									//1启动失败
	Voice_InitOk,          									//2启动完成
	Voice_DspConnected,          							//3调度连接成功
	
	//临时放这里 文件夹名格式AW104！
	//Voice_WalkPos=5,										//5正向运行中
	//Voice_WalkInf=6,										//6横向运行中
	//Voice_WalkRot=7,										//7原地自转中

	//模式状态
	Voice_EnterAuto=11,          							//11进入自动模式
	Voice_EnterManual,          							//12进入遥控模式

	//运动状态
	Voice_ModeChange=21,          							//21模式切换中
	Voice_ModeChangeOk,          							//22模式切换完成
	Voice_Still,											//23静止中
	Voice_Moving,											//24运动中
	Voice_Walking,											//25行走中
	Voice_WalkPos,											//26正向运行中
	Voice_WalkInf,											//27横向运行中
	Voice_WalkRot,											//28原地自转中
	Voice_Up,												//29平台上升中
	Voice_Down,												//30平台下降中

	//任务状态
	Voice_Trace=51,											//51巡线运动中
	Voice_AutoMoving,										//52自动运行中
	Voice_TaskPause,										//53任务暂停中
	Voice_TaskStart,										//54任务开始
	Voice_TaskFinish,										//55任务完成
	Voice_Arrive,											//56到达指定位置

	//故障状态
	Voice_Stop_Obs=71,										//71避障停车
	Voice_Stop_Crash,										//72碰撞停车

	Voice_Stop_Limit=81,									//81动作限制
	Voice_Stop_LimitMove,									//82限制行走
	Voice_Stop_LimitUp,										//83限制上升
	Voice_Stop_LimitDown,									//84限制下降
	
	Voice_EStop_Car=91,										//91车体急停
	Voice_EStop_Manual,										//92遥控器急停
	Voice_EStop_DSP,										//93调度系统急停
	Voice_EStop_PDA,										//94PDA急停

	Voice_Err_Sys=101,										//101系统异常
	Voice_Err_Unit,											//102联动异常
	Voice_Err_Auto,											//103自动异常
	Voice_LowPower,											//104电量低
}Voice_List;

//语音播放状态以及输出语音号
typedef struct
{
	//输入状态
	u32 MsInput;											//当前时间
	u16 MsTriggerHold;										//触发状态保持时间
	u16 MsPlay;												//语音播放时间
	u16 MsPlayOff;											//语音播放间隔时间

	u16 Cur_mode;											//当前控制模式
	u16 Target_mode;										//目标控制模式
	
	//位域状态输入
	struct
	{
		//急停
		u16 EStop_Car : 1;             						//车体急停触发标志
		u16 EStop_Manual : 1;          						//遥控器急停触发标志
		u16 EStop_DSP : 1;             						//调度系统急停触发标志
		u16 EStop_PDA : 1;             						//PDA急停触发标志

		//避障报警
		u16 Stop_Obs : 1;              						//避障停车触发标志
		u16 Stop_Crash : 1;            						//碰撞停车触发标志

		//其他报警
		u16 Err_Sys : 1;                					//报警触发标志
		u16 Err_Unit : 1;             						//联动类报警触发标志
		u16 Err_Auto : 1;             						//自动类报警触发标志
		u16 Stop_Limit : 1;         						//限制动作
		u16 LowPower : 1;            						//小车电量低

		//到位等触发提醒
		u16 InitOk : 1;          							//小车初初始化完成标志
		u16 ModeChangeOk : 1;             					//模式切换完成标志
		u16 Arrive : 1;          							//小车到达指定位置标志
		u16 Still : 1;             							//静止中标志
		u16 TaskPause : 1;               					//任务暂停标志
		u16 TaskStart : 1;               					//任务开始标志
		u16 DspConnected : 1;               				//调度连接成功标志

		//动作提醒
		u16 ModeChange : 1;             					//模式切换中标志
		u16 Moving : 1;             						//运动中标志
		u16 Walking : 1;             						//行走中标志
		u16 WalkPos : 1;             						//正向运行中标志
		u16 WalkInf : 1;             						//横向运行中标志
		u16 WalkRot : 1;             						//原地自转中标志
		u16 Lifting : 1;             						//平台升降中标志
		u16 Up : 1;             							//平台上升中标志
		u16 Down : 1;             							//平台下降中标志
	}BitInput;

	//过程状态
	Voice_List Voice_ID_Last;								//上次语音播放的序列号
	u32 MsRecord_IDUpdate;									//上次语音播放ID更新的时间
	u32 MsRecord_PlayCtrlChange;							//上次语音播放控制变化的时间

	//输出状态
	Voice_List Voice_ID;									//实时播放编号生成结果
	Voice_List Voice_ID_TrigSyn;							//触发同步结果
	Voice_List Voice_ID_Cmd;								//控制命令
	u16 Music_Volume;										//语音播报的声音可设为最大28 （0-28)					
}VoicePlayer_TypeDef;

Voice_List app_VoicePlayer_Ctrl(VoicePlayer_TypeDef *p,u32 Ms);

#endif

//***********************************END OF FILE***********************************
