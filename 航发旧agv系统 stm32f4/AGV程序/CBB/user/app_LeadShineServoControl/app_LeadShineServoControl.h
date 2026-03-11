#ifndef __APP_LEADSHINESERVOCONTROL_H__
#define __APP_LEADSHINESERVOCONTROL_H__



#ifdef  	APP_LEADSHINESERVOCONTROL_GLOBALS
 #define 	APP_LEADSHINESERVOCONTROL_EXT
#else
 #define 	APP_LEADSHINESERVOCONTROL_EXT  extern
#endif

#define Servo_delay_fuction(A, B, C)		((s32)(A - B) > (s32)(C))
//延时等待操作A:系统计数器,B：需要延时等待的时间计数器,C:等待的时间值
#define   Servo_Max_Num      4      //伺服的数量 

//伺服状态回调
typedef struct
{	
unsigned short int 	Servo_AlarmStatus;  	//报警状态（需以16进制查看）
unsigned short int 	Servo_IOInputStatus;  //IO输入状态（Bit2:使能状态  Bit4:限位开关状态 ）
               int  Servo_ActivePostion;  //位置反馈状态----32位---脉冲信号
}ServoStatus_Feedback_Typedef;
//伺服控制指令
typedef struct
{	
unsigned short int 	EnableCmd;  	                  //伺服使能
unsigned short int 	RunStatusCmd;                   //伺服运行指令      
unsigned short int 	RunModeCmd;  	                  //伺服运行模式指令
unsigned short int 	SoftResetCmd;                   //软件复位指令
unsigned short int 	SpeedCtrCmd;  	                //运行速度指令
unsigned short int 	RevCmd;                         //保留指令，需要4字节对齐
               int  PostionCtrCmd[Servo_Max_Num];   //位置控制指令----- 脉冲信号
}ServoCtrlCmd_Typedef;

typedef enum
{
  RunStep_Reset=0,                  //复位模式
	RunStep_ResetOk=1,               //复位完成，进入回零模式
  RunStep_WaitInitOk=2,            //等待回零操作
	RunStep_ClearPostionOperation=3, //回零成功后进行位置清楚指令
  RunStep_SetRunMode,              //设置运行模式
	RunStep_NormaLRun,               //正在运行
	RunStep_AlarmStaus,	             //报警状态
  RunStep_StopRun,                 //停止状态            
}LeadShineServo_RunStatus;         //伺服运行状态
//雷赛伺服控制以及状态反馈结构体
typedef struct
{
unsigned short int    *PostionOffset[Servo_Max_Num],    //伺服标定高度---mm
                      *RunSpeed_Low,                    //伺服低速运行速度
								      *RunSpeed_High,                   //伺服高速运行速度
                      *LiftHeightLowValue,              //伺服升降限制低点
                      *LiftHeightMaxValue,              //伺服升降限制高点
                      *SoftReset_flag,                  //伺服软件复位标志
                      *OffCtrlCheck,                    //伺服控制开关标志  0：开启  1：关闭控制
                      *ForceLiftHeight;                 //伺服强制升降高度命令---一般用于调试
int  Servo_RecordPostion[Servo_Max_Num];                //伺服记录位置---用于记录开机后位置
ServoCtrlCmd_Typedef  Servo_CtrlCmd;                    //伺服控制指令以及位置等参数
unsigned short int    HeightCtrCmd;                     //升降控制高度---mm
ServoStatus_Typedef   Servo_FeedbackData[Servo_Max_Num];//伺服状态回调

unsigned  char        Current_RunStep;                  //伺服的当前运行状态
unsigned  char        ServoAlarm_Flag;                  //伺服报警标志
unsigned  char        RunArrive_Flag;                   //伺服控制到位标志
}LeadShineServoControl_TypeDef;



























#endif