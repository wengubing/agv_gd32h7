/********************************** Copyright *********************************** 
*
*                           成都航发机器人有限公司
*                            All Rights Reserved
* 
* FileName   : app_PairPusher.c  
* Version    : V1.0		
* Author     : app_PairPusher			
* Date       : 2025.5.17         
* Description: 联动推送控制模块
*******************************************************************************/

/*包含以下头文件*/
//#include <string.h>
#include "app_PairPusher.h"

//单动模式操作
bool PpSingleMode(PairPusher_TypeDef *p)
{
	//释放联动通信IO信号
	p->Output.CommIO.Connect = 0;						//握手释放释放
	p->Output.CommIO.Control = 0;						//占位释放
	p->Output.CommIO.Fault = 0;							//报警释放
	p->Output.CommIO.Push = 0;							//推命令释放
	p->Output.CommIO.Pull = 0;							//拉命令释放
	p->Output.CommIO.SpeedGearBit_Low = 0;				//挡位释放
	p->Output.CommIO.SpeedGearBit_High = 0;				//挡位释放

	//响应遥控器的信号
	p->Output.Push = p->Input.Push;						//推命令
	p->Output.Pull = p->Input.Pull;						//拉命令
	p->Output.SpeedGear = p->Input.SpeedGear;			//挡位

	return 1;
}

//模块主方法
bool PairPusher_Main(PairPusher_TypeDef *p)
{	
	u8 temp = 0;
	/*状态机方法 复杂
	if (p->Input.PairSel == 0)
	{
		PpSingleMode(p);//单动时，不响应输入的联动IO信号，直接响应遥控器的信号
	}
	else//联动时
	{
		//判断模式是否就绪
		if (p->Input.CommIO.Connect == 0)
		{
			//模式
		}
		else
		{
			//收到对方控制占位信号，立即停止自己的占位信号和推拉命令，并进入被动模式
			//注意此时如自己有故障，通知对方，
			if (p->Input.CommIO.Control == 1)
			{
				
			}
			else
			{
			}
		}
	}*/

	//预处理
	//推拉输入同事按下无效
	if (p->Input.Push == 1 && p->Input.Pull == 1)
	{
		p->Input.Push = 0;									//推命令释放
		p->Input.Pull = 0;									//拉命令释放
	}
	else {}

	//单个输出信号逐一处理
	//握手信号 仅自己在联动且模式为推送时，才输出握手信号
	if(p->Input.PairSel == 1 && p->Input.ModeReady == 1)
	{
		p->Output.CommIO.Connect = 1;						//握手信号
	}
	else
	{
		p->Output.CommIO.Connect = 0;						//握手释放释放
	}

	//占位信号 仅自己输出了握手信号，且对方也输出了握手信号时，且对方没有占位信号，同时自己推或拉命令有效时，才输出占位信号
	if(p->Input.PairSel == 1 && p->Input.ModeReady == 1 && p->Input.CommIO.Connect == 1 && p->Input.CommIO.Control == 0 && (p->Input.Push || p->Input.Pull))
	{
		p->Output.CommIO.Control = 1;						//占位信号
	}
	else
	{
		p->Output.CommIO.Control = 0;						//占位释放释放
	}

	//故障停止信号 只要自己有故障，就输出故障信号
	if(p->Input.FaultStop == 1)
	{
		p->Output.CommIO.Fault = 1;							//故障信号
	}
	else
	{
		p->Output.CommIO.Fault = 0;							//故障释放释放
	}

	//推拉命令信号 跟随自己的遥控器命令信号（当自己没有故障和限位时，确保被动方直接响应不去判断通信来的信号也可以）
	if (p->Input.FaultStop == 0)
	{
		p->Output.CommIO.Push = p->Input.Push;				//推命令信号

		//拉命令信号需要考虑自己的限位状态
		if(p->Input.Limited == 1)
			p->Output.CommIO.Pull = 0;						//拉命令释放
		else
			p->Output.CommIO.Pull = p->Input.Pull;			//拉命令信号
	}
	else
	{
		p->Output.CommIO.Push = 0;							//推命令释放
		p->Output.CommIO.Pull = 0;							//拉命令释放
	}
	
	//挡位信号 跟随自己的遥控器命令信号
	temp = p->Input.SpeedGear;
	if(temp > 3)
		temp = 3;	//挡位信号限制在1~3
	else {}
	p->Output.CommIO.SpeedGearBit_Low = temp & 0x01;		//挡位信号低位
	p->Output.CommIO.SpeedGearBit_High = (temp >> 1) & 0x01;//挡位信号高位

	//控制输出
	//自己的输出完全不考虑限位信号，因为底盘有限位响应，且底盘必须要有命令才会长鸣提醒
	//联动满足条件且处于被动模式时，才输出对方的推拉命令和挡位信号
	if (p->Input.PairSel == 1 && p->Input.ModeReady == 1 && p->Input.CommIO.Connect == 1 && p->Input.CommIO.Control == 1
		&& p->Input.Pull == 0 && p->Input.Push == 0)//且自己没有推拉命令
	{
		//注意两车的推拉命令是相反的
		p->Output.Push = p->Input.CommIO.Pull;				//对方推 自己拉，此时限位开关在自己这边
		/*对方拉 自己推，此时限位开关在对方那边,但没有关系，对方如果限位了要需要关闭动作命令信号
		if(p->Input.CommIO.Fault == 0)
			p->Output.Pull = p->Input.CommIO.Push;
		else
			p->Output.Pull = 0;
		*/
		//所以直接响应对方的推拉命令
		p->Output.Pull = p->Input.CommIO.Push;

		p->Output.SpeedGear = (p->Input.CommIO.SpeedGearBit_High << 1) | p->Input.CommIO.SpeedGearBit_Low;	//挡位信号
	}
	else if(p->Input.PairSel == 1 && p->Input.ModeReady == 1 && p->Input.CommIO.Connect == 1 && p->Input.CommIO.Control == 0
		&& p->Input.CommIO.Pull == 0 && p->Input.CommIO.Push == 0//对方没有推拉命令
		&& p->Input.CommIO.Fault == 0)//联动，且对方没有占位信号，则响应自己遥控器控制，
		//注意要检查对方车的故障和限位信号(限位信号暂缺！ 对方车自己会停 这边还在推，但货已离开 无危险)
	{
		p->Output.Push = p->Input.Push;						//推命令信号
		p->Output.Pull = p->Input.Pull;						//拉命令信号

		temp = p->Input.SpeedGear;
		if (temp >= 3)
			p->Output.SpeedGear = 3;
		else
			p->Output.SpeedGear = temp;
	}
	else if(p->Input.PairSel == 0)//单动模式
	{
		p->Output.Push = p->Input.Push;						//推命令信号
		p->Output.Pull = p->Input.Pull;						//拉命令信号

		//挡位信号
		//仅在推送模式限制，因为底盘复用的升降挡位，升降要维持6档
		temp = p->Input.SpeedGear;
		if (p->Input.ModeReady == 1)
		{
			if (temp >= 3)
				p->Output.SpeedGear = 3;
			else
				p->Output.SpeedGear = p->Input.SpeedGear;
		}
		else
		{
			p->Output.SpeedGear = p->Input.SpeedGear;
		}
	}
	else//动作条件不满足
	{
		p->Output.Push = 0;									//推命令释放
		p->Output.Pull = 0;									//拉命令释放
		p->Output.SpeedGear = 0;							//挡位信号释放
	}
	return 1;
}

/************************************END OF FILE************************************/

