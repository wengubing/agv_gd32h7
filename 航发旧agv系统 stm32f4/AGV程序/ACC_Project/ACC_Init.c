/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: ACC_Init.c
** 创 建 人: 文小兵  
** 描    述: ACC对象实例化、初始化，整理汇总到此文件
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年4月25日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//声明对象在此文件编译时实例化
#define ACC_INIT_GLOBALS

//自定义引用			
#include "includes.h"
#include "CBB_Init.h"
#include "ACC_Init.h"
#include "ACC_Project.h"
#include "EmgJudge.h"

//外部对象声明

//内部方法声明
#include "ACC_LiftCtrl.h"
#include "ACC_LegCtrl.h"
#include "ACC_AbsorptionCtrl.h"
#include "ACC_PushPullCtrl.h"
#include "ACC_RollerCtrl.h"
/**************************************************************************
* 函数名:ACC_ObjInit
* 功  能:模块初始化内容
* 参  数:无
* 返回值:bool
**************************************************************************/
void ACC_ObjInit(ACC_DATA_TypeDef *p,ACC_Para_TypeDef *p2Para)
{
	//输入参数初始化
	p->ChassisType = p2Para->ChassisType;					//底盘类型
	p->JoystickMax = p2Para->Walk.JoystickMax;				//摇杆最大值
	p->ModeSwitchMode = p2Para->Walk.ModeSwitchMode;		//模式切换方式
	
	memcpy(p->SpeedPlan, p2Para->Walk.SpeedPlan, sizeof(p->SpeedPlan));//速度规划
	p->OrgCmd.WalkGear = 3;									//仿真调试时，默认赋值
	p->OrgCmd.LiftGear = 3;
	//...
	
	//故障检出方法指针初始化
	p->ExtFunc.p2EmgJudge 			= EMG_Judge;
	p->ExtFunc.p2WalkFunc			= Walk_MotorResolving;
	p->ExtFunc.p2LiftFunc 			= Lift_Ctrl;
	p->ExtFunc.p2LegFunc 			= Leg_Ctrl;
	p->ExtFunc.p2AbsorptionFunc		= AbsorptionCtrl_Ctrl;
	p->ExtFunc.p2PushPull_Func 		= Push_Pull_Ctrl;
	p->ExtFunc.p2Roller_Func 		= Roller_Ctrl;
	
	

	//启动时科润驱动器刹车生效
	p->IOCtrl.KoRunBrake = 0;//392项目实际为继电器失电 刹车生效

	//联动方向
	//p->OrgCmd.AssemblyInfeed = p2Para->SysFucBit.AssemblyInfeed;
}



//***********************************END OF FILE***********************************

