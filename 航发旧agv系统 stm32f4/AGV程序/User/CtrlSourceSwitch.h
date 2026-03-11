/****************************Copyright (c)**********************************************                                       
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱                     
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: CtrlSourceSwitch.h
** 创 建 人: 文小兵  
** 描    述: 控制命令源切换、解析
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.0.0  
** 创建日期: 2024年8月26日
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __CtrlSourceSwitch_h__
#define __CtrlSourceSwitch_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  CtrlSourceSwitch_GLOBALS
    #define CtrlSourceSwitch_EXT
#else
    #define CtrlSourceSwitch_EXT  extern
#endif

//自定义引用	
#include "AgvChassisController.h"
//#include "ACC_Project.h"
#include <stdbool.h>
#include "common_def.h"
	
//控制源定义
typedef enum
{
    ACCCC_Null = 0,											//初始化
    ACCCC_WirelessRemote,									//无线遥控控制
    ACCCC_WiredRemote,										//有线遥控控制
    ACCCC_HandShank,										//特殊有线手柄控制
    ACCCC_HMI,												//HMI控制
    ACCCC_Auto,												//调度自动运行
    ACCCC_End = 65535,										//u16
}ACCCC_TypeDef;

//调度系统端 避障雷达区域模式定义
typedef enum
{
    DspObsArea_Null = 0,							            //无效
    DspObsArea_Noload,							            //1空载
    DspObsArea_Load,							            //2负载
    DspObsArea_Charge,							            //3充电
    DspObsArea_Enter_Noload,							    //4进工位 空载
    DspObsArea_Narrow,							            //5狭窄
    DspObsArea_Enter_Load,							        //6进工位 负载
    DspObsArea_HighSpeed,							        //7高速模式
    DspObsArea_Reserved,							        //8预留
}DspObsArea_TypeDef;

//空气悬挂负载状态定义
typedef enum
{
    NoLoad = 0,													//无负载
    LowLoad = 1,													//轻负载
    HighLoad = 2,												//重负载
}LoadStatus_Typedef;

//控制源切换过程 数据对象定义
typedef struct
{
    //初始化参数
    //休眠最大计数 设0关闭
    u32 WirelessSleepCntMax;
	u32 Para_LiftTwoButtonPress;					        //升降按钮同时按下 判定时间ms

    //实时输入
    u32 in_Ms;												//ms输入
    ACCCC_TypeDef in_Channel;								//控制源选择
    char in_WROverAuto;										//无线遥控覆盖接管自动控制命令
    u16 InputPointID;                    					//HMI输入点ID

    //内部成员
    ACCCC_TypeDef STM_Channel;								//控制源状态机
    u32 MsRecord;											//ms记录
    u32 WirelessSleepCnt;									//遥控器休眠计数
    u32 MsRecord_LiftTwoButtonPress;						//ms记录 升降按钮同时按下

    //遥控器
    //测试记录：设备开机时，遥控器如果是休眠状态，车能读到通信数量10和休眠标志，但是无法读到遥控器的真实状态！
    //设备开机时，遥控器关机，车能读到通信数量0和休眠标志
    bool WirelessRemoteComminitOk;							//无线遥控时 无线连接完成
    ACC_ObsEn_BitDef WhRadarEn;                             //遥控器雷达使能实时结果
    ACC_ObsEn_BitDef WhRadarEnLast;                         //遥控器雷达使能上次结果

    //hmi
    ACC_ObsEn_BitDef HmiRadarEn;                            //Hmi雷达使能实时结果
    ACC_ObsEn_BitDef HmiRadarEnLast;                        //Hmi雷达使能上次结果

    //调度系统
    ACC_ObsEn_BitDef RadarEn;                               //雷达使能 实时结果
    DspObsArea_TypeDef DspObsArea;                          //调度系统端 避障雷达区域模式
    bool DirectionNotInit_Flag;								//方向未初始化标志
    bool NotFindPointSign_Flag;								//未找到点标志

    //强制设置避障区域命令
    AccObsArea_TypeDef ForceObsArea;                        //强制设置避障区域命令
    //雷达使能 上一次结果 
    ACC_ObsEn_BitDef RadarEnLast;                           //雷达使能 上一次结果 

    //平滑前缩放控制对象
	Unifor_TypeDef VxyzZoom;							    //

    //输出
    OrgCtrlCmd_TypeDef CtrlCmd;								//控制命令

    //位域对象
    struct
    {
        //远端控制器上的授权信号 例如遥控器上的钥匙开关，注意不是车体上的
		u32 RemoteKeyAuth 	: 1;								//远端控制器授权
		u32 DspModeSel		: 1;								//遥控器切换调度
    }Bits;

	//空气悬挂负载状态
	LoadStatus_Typedef AirAbsorptionLoad;
}ACCCCSwitch_TypeDef;

//对象外部声明
//ACC_INIT_EXT ACC_DATA_TypeDef AccData;


//方法外部声明
CtrlSourceSwitch_EXT bool CSS_Process(void);                        //控制命令源切换、解析
#endif

//***********************************END OF FILE***********************************


