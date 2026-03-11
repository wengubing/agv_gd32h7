/********************************** Copyright ***********************************
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_VoicePlayer.c
  * Version    : V1.0
  * Author     : CDHF
  * Date       : 2024.5.13
  * Description: 实现对语音播放器播放语音内容的控制
  *******************************************************************************/

//头文件
#include "app_VoicePlayer.h"

//宏定义

//播放编号解析更新
Voice_List app_VoicePlayer_IDUpdate(VoicePlayer_TypeDef *p,Voice_List ID)
{
    p->Voice_ID = ID;
    if(p->Voice_ID != p->Voice_ID_Last)
    {
        p->Voice_ID_Last = p->Voice_ID;
        p->MsRecord_IDUpdate = p->MsInput;

        //更新播放指令编号
        p->Voice_ID_TrigSyn = p->Voice_ID;
        p->Voice_ID_Cmd = p->Voice_ID_TrigSyn;//播放编号发生变化时，更新播放指令编号,立即开始循环播放控制
    }

    return p->Voice_ID;
}

//播放编号解析
Voice_List app_VoicePlayer_Status2ID(VoicePlayer_TypeDef *p)
{
    //急停
    if(p->BitInput.EStop_Car)
        app_VoicePlayer_IDUpdate(p,Voice_EStop_Car);
    else if(p->BitInput.EStop_Manual)
        app_VoicePlayer_IDUpdate(p,Voice_EStop_Manual);
    else if(p->BitInput.EStop_DSP)
        app_VoicePlayer_IDUpdate(p,Voice_EStop_DSP);
    else if(p->BitInput.EStop_PDA)
        app_VoicePlayer_IDUpdate(p,Voice_EStop_PDA);
    //避障报警
    else if(p->BitInput.Stop_Crash)
        app_VoicePlayer_IDUpdate(p,Voice_Stop_Crash);
    else if(p->BitInput.Stop_Obs)
        app_VoicePlayer_IDUpdate(p,Voice_Stop_Obs);
    //其他报警
    else if(p->BitInput.Err_Sys)
        app_VoicePlayer_IDUpdate(p,Voice_Err_Sys);
    else if(p->BitInput.Err_Unit)
        app_VoicePlayer_IDUpdate(p,Voice_Err_Unit);
    else if(p->BitInput.Err_Auto)
        app_VoicePlayer_IDUpdate(p,Voice_Err_Auto);
    else if(p->BitInput.Stop_Limit)
        app_VoicePlayer_IDUpdate(p,Voice_Stop_Limit);
    else if(p->BitInput.LowPower)
        app_VoicePlayer_IDUpdate(p,Voice_LowPower);
    //到位等触发提醒
    else if(p->BitInput.InitOk)
    {
        app_VoicePlayer_IDUpdate(p,Voice_InitOk);
        if(p->MsInput - p->MsRecord_IDUpdate > p->MsTriggerHold)
            p->BitInput.InitOk = 0;
    }
    else if(p->BitInput.ModeChangeOk)
    {
        app_VoicePlayer_IDUpdate(p,Voice_ModeChangeOk);
        if(p->MsInput - p->MsRecord_IDUpdate > p->MsTriggerHold)
            p->BitInput.ModeChangeOk = 0;
    }
    else if(p->BitInput.Arrive)
    {
        app_VoicePlayer_IDUpdate(p,Voice_Arrive);
        if(p->MsInput - p->MsRecord_IDUpdate > p->MsTriggerHold)
            p->BitInput.Arrive = 0;
    }
    else if(0)//p->BitInput.Still)
    {
        app_VoicePlayer_IDUpdate(p,Voice_Still);
        if(p->MsInput - p->MsRecord_IDUpdate > p->MsTriggerHold)
            p->BitInput.Still = 0;
    }
    else if(p->BitInput.TaskPause)
    {
        app_VoicePlayer_IDUpdate(p,Voice_TaskPause);
        if(p->MsInput - p->MsRecord_IDUpdate > p->MsTriggerHold)
            p->BitInput.TaskPause = 0;
    }
    else if(p->BitInput.TaskStart)
    {
        app_VoicePlayer_IDUpdate(p,Voice_TaskStart);
        if(p->MsInput - p->MsRecord_IDUpdate > p->MsTriggerHold)
            p->BitInput.TaskStart = 0;
    }
    //动作提醒
    else if(p->BitInput.ModeChange)
        app_VoicePlayer_IDUpdate(p,Voice_ModeChange);
	else if(p->BitInput.WalkPos)
        app_VoicePlayer_IDUpdate(p,Voice_WalkPos);
    else if(p->BitInput.WalkInf)
        app_VoicePlayer_IDUpdate(p,Voice_WalkInf);
    else if(p->BitInput.WalkRot)
        app_VoicePlayer_IDUpdate(p,Voice_WalkRot);
    else if(p->BitInput.Walking)
        app_VoicePlayer_IDUpdate(p,Voice_Walking);
   
    else if(p->BitInput.Up)
        app_VoicePlayer_IDUpdate(p,Voice_Up);
    else if(p->BitInput.Down)
        app_VoicePlayer_IDUpdate(p,Voice_Down);
    //无触发
    else
		app_VoicePlayer_IDUpdate(p,Voice_Null);

    return p->Voice_ID;
}

//播放命令控制
Voice_List app_VoicePlayer_Ctrl(VoicePlayer_TypeDef *p,u32 Ms)
{
    p->MsInput = Ms;

    /*从输入状态实时接线播放编号*/
    app_VoicePlayer_Status2ID(p);

    //外部更改指令编号，同步回各个源
    if(p->Voice_ID_TrigSyn != p->Voice_ID)
    {
        p->Voice_ID = p->Voice_ID_TrigSyn;
    }
    
    //播放编号不为空时，周期切换播放指令
    if(p->Voice_ID_TrigSyn != Voice_Null)
    {
        if(p->MsInput - p->MsRecord_PlayCtrlChange >= p->MsPlay && p->Voice_ID_Cmd != Voice_Null)
        {
            p->Voice_ID_Cmd = Voice_Null;
            p->MsRecord_PlayCtrlChange = p->MsInput;
        }
        else if(p->MsInput - p->MsRecord_PlayCtrlChange >= p->MsPlayOff && p->Voice_ID_Cmd == Voice_Null)
        {
            p->MsRecord_PlayCtrlChange = p->MsInput;
            p->Voice_ID_Cmd = p->Voice_ID_TrigSyn;
        }
    }
    else
        p->Voice_ID_Cmd = Voice_Null;

    return p->Voice_ID_Cmd;
}


//***********************************END OF FILE***********************************
