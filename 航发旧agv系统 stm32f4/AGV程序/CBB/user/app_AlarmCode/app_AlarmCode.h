/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: app_AlarmCode.h
** 创 建 人: 何斌初版 文小兵392项目重构
** 描    述: 报警枚举
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.1.0
** 创建日期: 2024年3月
**--------------------------------------------------------------------------------------
***************************************************************************************/

//保证以下所有内容只被同一源文件引用一次
#ifndef __app_AlarmCode_h__
#define __app_AlarmCode_h__

//确保仅在该文件编译时实例化一次对象
#ifdef  __app_AlarmCode_h___GLOBALS
	#define __app_AlarmCode_h___EXT
#else
	#define __app_AlarmCode_h___EXT  extern
#endif

//自定义引用	
#include <stdbool.h>

/*2020711@wxb,
原CBB存在以下问题：
1.位的数字位置写在注释中，有重复，难以插入、难以修改维护
2.触摸屏文件与代码定义不一致，维护起来也复杂不易操作
3.与调度系统报警定义不一致，不易同步维护

数据位枚举定义优化未位域定义，一张表定义所有报警，有以下优点：
1.

*/

//不受控停车、提醒，已集中定义在EmgJudge.h文件中
//受控停车整理如下：
typedef struct __attribute__((packed))
{
	//急停
	unsigned long long Remote : 1;								//遥控器急停
	unsigned long long Dsp : 1;									//调度急停
	unsigned long long HMI_EST : 1;								//HMI急停
	unsigned long long Little : 1;								//小急停

	//超时
	unsigned long long DSP : 1;									//调度系统通信超时
	unsigned long long HMI : 1;									//HMI通信超时
	unsigned long long RemoteReceiverA : 1;						//遥控接收器A通信超时
	unsigned long long RemoteReceiverB : 1;						//遥控接收器B通信超时

	unsigned long long RemoteWired : 1;							//遥控有线通信超时
	unsigned long long BMS : 1;									//BMS通信超时
	unsigned long long Voice : 1;								//语音播报通信超时
	unsigned long long AD : 1;									//AD通信超时

	unsigned long long MagF : 1;								//前磁导航通信超时
	unsigned long long MagB : 1;								//后磁导航通信超时
	unsigned long long MagL : 1;								//左磁导航通信超时
	unsigned long long MagR : 1;								//右磁导航通信超时

	unsigned long long RFID : 1;								//RFID通信超时
	unsigned long long PGVUpward : 1;							//向上pgv通信超时
	unsigned long long PGVDownward : 1;							//向下pgv通信超时
	unsigned long long SerialExp : 1;							//串口拓展板通信超时

	unsigned long long CommConv : 1;							//通信转换板通信超时
	unsigned long long WheelAngle : 1;							//轮系角度通信超时
	unsigned long long WheelHeight : 1;							//轮系高度通信超时

	//平台升降
	unsigned long long TooHigh : 1;								//平台过高
	unsigned long long TooLow : 1;								//平台过低
	unsigned long long DiffLimit : 1;							//高度差过大
	unsigned long long DiffLimitRear : 1;						//后差过大
	unsigned long long ExecTimeout : 1;							//执行超时

	//悬挂
	unsigned long long ASB_TooHigh : 1;							//悬挂过高
	unsigned long long ASB_TooLow : 1;							//悬挂过低
	unsigned long long ASB_DiffLimit : 1;						//悬挂差过大
	unsigned long long ASB_DiffLimitRear : 1;					//后差过大
	unsigned long long ASB_ExecTimeout : 1;						//执行超时

	//特殊上装-辊道、浮动等
	unsigned long long Special_TooHigh : 1;							//上平台高限位
	unsigned long long Special_TooLow : 1;							//上平台低限位
	unsigned long long Special_DiffLimit : 1;						//上平台高差大
	unsigned long long Special_ExecTimeout : 1;						//上平台执行超时

	//常见功能
	unsigned long long CanBusErr : 1;							//CAN总线异常 启动时
	unsigned long long CanNoSlv : 1;							//没有can从站上线
	unsigned long long CanLessSlv : 1;							//can从站数量不足
	unsigned long long ModeSwitchTimeOut : 1;					//模式切换超时

	unsigned long long RemoteWirelessSignalErr : 1;				//遥控器无线信号异常
	unsigned long long CrashBarF : 1;							//防撞触边触发 前
	unsigned long long CrashBarB : 1;							//防撞触边触发 后
	unsigned long long CrashBarL : 1;							//防撞触边触发 左

	unsigned long long CrashBarR : 1;							//防撞触边触发 右
	unsigned long long wheelAngleLimited : 1;					//轮系角度限制
	unsigned long long LowBatteryStop : 1;						//低电量停止
	unsigned long long MagOutLine : 1;							//磁导航离线
	unsigned long long BMSErrStop : 1;							//BMS故障停止

	//其它
	unsigned long long CodeErr : 1;								//代码异常
	unsigned long long WheelAngCalErr : 1;						//轮系角度解算错误
	unsigned long long CtrlSysVoltageErr : 1;					//控制系统电压异常
	unsigned long long Expand : 1;								//扩展异常

	unsigned long long PullApart : 1;							///438项目两车距离较远 联动通信线可能被扯断
	unsigned long long Thermodetector : 1;						//温度通信超时
	unsigned long long AsbTimeOut : 1;							//联动通讯超时
	unsigned long long AsbErr : 1;								//联动异常

	unsigned long long Asb_LoseControl : 1;						//对方车产生非受控异常
	unsigned long long Asb_ControlStop : 1;						//对方车产生受控停车
	unsigned long long Asb_MoveLimited : 1;						//对方车产生运动限制
	unsigned long long Asb_Warn : 1;							//对方车提示类信息

	unsigned long long ProjectExtend : 1;						//项目扩展异常
	
//	unsigned int ColourOutLine : 1;							/*色带离线*/
//	unsigned int SlamLostLocation : 1;						/*slam定位丢失*/
//	unsigned int SlamLostCom : 1;							/*slam通讯丢失*/
	//unsigned long long LenTest : 40;							//长度测试 超长编译无提示！但通过代码运行依然可以检出异常。
	
}ACC_ControlledStop_BitDef;

//对象外部声明

//方法外部声明

#endif

//***********************************END OF FILE***********************************


