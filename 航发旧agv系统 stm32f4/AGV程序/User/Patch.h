/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: Patch.h
** 创 建 人: 文小兵
** 描    述: 外部方法补丁
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0
** 创建日期: 2024年9月20日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __Patch_h__
#define __Patch_h__

//确保仅在该文件编译时实例化一次对象
#ifdef Patch_GLOBALS
#define Patch_EXT
#else
#define Patch_EXT  extern
#endif

//自定义引用	
#include <stdbool.h>
#include "common_def.h"
#include "AgvChassisController.h"

//动态加速度控制对象定义
typedef struct
{
    //初始化参数
    f32 AccMax;                                             //最大加速度
    f32 AccMin;                                             //最小加速度
    char Para_WalkGear;                                     //行走挡位大于等于该挡位时限制加速度
    f32 Para_Weight;                                        //负载重量大于等于该重量时限制加速度

    //限制选择开关
    struct
    {
        u16 Rely_SOC : 1;							        //根据SOC控制加速度
        u16 Rely_Height : 1;								//根据高度控制加速度
        u16 Rely_Power : 1;									//根据功率控制加速度
        u16 Rely_WalkGear : 1;							    //根据行走挡位控制加速度
        u16 Rely_Weight : 1;							    //根据负载重量限制控制加速度
        u16 Rely_WalkGearAndWeight : 1;						//挡位和重量同时满足时限制

        u16 Weight2Mode : 1;							    //重量仅在升降模式切出时刷新

    }ParaBit;

    //实时输入
    f32 Input_Acc;										    //输入加速度
    f32 Input_Vx;										    //输入速度
    f32 Input_Vy;										    //
    f32 Input_Vz;										    //
    f32 Input_SOC;									        //输入SOC
    f32 Input_Height;									    //输入高度
    f32 Input_Weight;									    //输入负载重量
    f32 Input_Power;									    //输入功率
    f32 Input_WalkGear;								        //输入行走挡位
    ACCMode_TypeDef Input_Mode;							    //输入模式

    //过程数据
    //升降模式状态机
    enum
    {
        LiftMode_Init = 0,								    //初始化
        LiftMode_WaitEnter,								    //等待进入升降
        LiftMode_WaitQuit,								    //等待退出升降 退出时更新重量，并跳到init
        LiftMode_End = 65535, 							    //u16
    }LiftModeSTM;

    //实时输出
    f32 Weight;									            //

    //实时输出
    f32 Output_Acc;									        //输出加速度
    f32 Output_Vx;									        //输出速度
    f32 Output_Vy;									        //
    f32 Output_Vz;									        //

    //实时状态
    struct
    {
        u16 Acc : 1;							            //触发加速度限制
        u16 V : 1;							                //触发速度限制
    }State_Limit;
}WalkLimit_TypeDef;

//动态挡位限制对象 单个
typedef struct
{
    //初始化参数
    char Para_LimitGear;                                    //限制档位
    u16 Para_RelyMax;                                       //限制触发
    u16 Para_RelyMin;                                       //限制恢复

    //实时输入
    u16 Input_Rely;									        //限制触发量
    char Input_Gear;								        //挡位

    //过程数据
    u16 Rely;

    //实时输出
    u16 Output_Gear;								        //挡位
    //实时状态
    struct
    {
        u16 Limited_Rely : 1;							    //触发参数是限制状态
        u16 Limited : 1;							        //触发限制
    }Output_State;
}GearLimit_Single_TypeDef;

//舵轮车手动操作控制单个模组对象
typedef struct
{
    //para
    bool Para_En;                                           //1开启，0关闭，在项目配置中设置
    
    //input
    bool En;                                                //1使能，0关闭，实时控制
    u16 Select;                                             //选择1~n号模组
    u16 SpeedMax;                                           //最大速度 0.1rpm

    //internal
    s16 Speed;
    u32 MsRecord;                                           //时间戳
    enum
    {
        MR_ManualCtrl_Stm_Init = 0,                         //初始化
        MR_ManualCtrl_Stm_Enter,                            //进入操作：打开抱闸、打开全部行走电机使能
        MR_ManualCtrl_Stm_EnterWait,                        //等待进入操作完成
        MR_ManualCtrl_Stm_Run,                              //运行
        MR_ManualCtrl_Stm_Exit,                             //退出操作 暂时什么也不做
        MR_ManualCtrl_Stm_ExitWait,                         //等待退出操作完成，之后返回init状态
        MR_ManualCtrl_Stm_End = 65535,                      //u16
    }Stm;

    //output

}MR_ManualCtrl_TypeDef;

//斜行和转弯命令二选一控制对象
typedef struct
{
    //para
    bool Enable;                                            //使能二选一控制
    bool ForceEnable;                                       //强制使能
    bool Prees_AngOrR;                                      //0默认单轴 1按下单轴
    u16 MaxSliAngPos;                                       //最大斜行角度 单轴
    u16 MinRotDisPos;                                       //最小转弯距离 单轴
	/*避免与系统参数中正常复合运动参数冲突 所以注释
    u16 MaxSliAngPos_Free;                                  //最大斜行角度 复合运动
    u16 MinRotDisPos_Free;                                  //最小转弯距离 复合运动*/

    //input

    //internal
    bool AngAndR_Free;                                      //复合运动有效状态
    bool AngAndR_Free_Last;                                 //last,用于切换时复位状态机

    enum STM_TypeDef
    {
        AngAndRSel_Init = 0,                                //初始化
        AngAndRSel_Ang,                                     //
        AngAndRSel_R,                                       //
        AngAndRSel_AngR,                                    //正常复合运动
        AngAndRSel_End = 65535,                             //u16
    }STM;
    char AngAxis;                                           //斜行轴命令下标
    s16 AngCmd;                                            //

    //output

}Patch_AngAndR_CmdSel_TypeDef;

//电机速度平滑参数
typedef struct
{
    //初始化参数
    bool En;                                                //使能电机速度平滑
    f32 Acc;                                                //加速增量
    f32 Dec;                                                //减速增量

    //
    DataSmoothing_TypeDef SM[ACC_WALKMOTOR_MAX];			//平滑处理变量
    bool RunFlg;                                            //调用标记，用于在跳过了调用时判断，对时间戳进行复位
}WalkMotorSmooth_TypeDef;

//对象外部声明

//方法外部声明
Patch_EXT bool WalkLimit_Run(WalkLimit_TypeDef* p);					//
Patch_EXT bool GearLimit_Single_Run(GearLimit_Single_TypeDef* p);
Patch_EXT bool MR_ManualCtrl_Run(MR_ManualCtrl_TypeDef* p2MRMC);
Patch_EXT bool MR_ManualCtrl_EnterJdg(MR_ManualCtrl_TypeDef* p2MRMC);
bool Patch_AngAndR_CmdSel(void);
#endif
//***********************************END OF FILE***********************************


