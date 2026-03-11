/****************************Copyright (c)**********************************************
**					╭--------------------------╮
**					︱	成都航发机器人有限公司	︱
**					︱	http://www.hangfa.com	︱
**					╰--------------------------╯
**------------- 文件信息 ---------------------------------------------------------------
** 文 件 名: app_Assembly.h
** 创 建 人: 文小兵
** 描    述: 联动控制模块
**------------- 版本信息 ---------------------------------------------------------------
** 版    本: V1.1.0
** 创建日期: 2024年11月5日
**--------------------------------------------------------------------------------------
***************************************************************************************/  

//保证以下所有内容只被同一源文件引用一次
#ifndef __Assembly_h__
#define __Assembly_h__

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  Assembly_GLOBALS
	#define Assembly_EXT
#else
	#define Assembly_EXT  extern
#endif

//包含对象头文件
//#include "AgvChassisController.h"
#include <stdbool.h>
#include "common_def.h"	

//宏定义
#define ASB_CAR_MAX_NUM (AsbCarStrNo_MaxNum-1)				//最大联动车数量

//车辆数据中数组中的下标索引
typedef enum
{
	AsbCarStrNo_Nnull = 0,									//无效
	AsbCarStrNo_1,											//车1
	AsbCarStrNo_2,											//车2
    AsbCarStrNo_MaxNum,						                //最大联动车数量
    AsbCarStrNo_End = 65535,						        //U16
}AsbCarStrNo_TypeDef;

typedef enum
{
	AsbRole_Nnull = 0,										//
	AsbRole_Master,											//主车
	AsbRole_Slave,											//从车
	AsbRole_End = 65535,									//枚举型结束
}AsbRole_TypeDef;

//遥控器单联动模式
typedef enum
{
	AsbHcmMode_Nnull = 0,									//无效
	AsbHcmMode_StrNo1,										//1单动
	AsbHcmMode_StrNo2,										//2单动
	AsbHcmMode_Asb,											//联动
	AsbHcmMode_End = 65535,									//枚举型结束
}AsbSelCmd_TypeDef;


//单动/联动模式定义
typedef enum
{
	AsbAsbMode_Nnull = 0,									//
	AsbAsbMode_OtherCar,									//联动遥控器未选择自己
	AsbAsbMode_Single,										//单动
	//AsbAsbMode_AsbSwitching,								//模式切换中
	AsbAsbMode_AsbReadyOne,									//预备（自己已就绪，等其它车就绪）
	AsbAsbMode_Asb,											//联动
	AsbAsbMode_AsbExit,										//联动退出
	AsbAsbMode_End = 65535,									//枚举型结束
}AsbAsbMode_TypeDef;

//联动车半长对象 从前左开始顺时针排列 0~65535mm 
typedef struct
{
	u16 FL;													//前左
	u16 FR;													//前右
	u16 RF;													//右前
	u16 RB;													//右后
	u16 BR;													//后右
	u16 BL;													//后左
	u16 LB;													//左后
	u16 LF;													//左前

	u16 Front;												//
	u16 Back;												//
	u16 Left;												//
	u16 Right;
}HalfCar_TypeDef;

//巡线传感器输入
typedef struct
{
	//前后左右目标轨迹线检测传感器数据，单位0.1mm，面向传感器，目标轨迹线偏右为正
	s16 Front;												//
	s16 Back;												//
	s16 Left;												//
	s16 Right;												//
}AsbTrack_TypeDef;

//联动故障
typedef struct
{
  	u32 CarPanelSelErr:1;									//车端单联动切换异常
	u32 CarPanelSelErr_Othre : 1;							//对方车未选择联动
    u32 CantWired:1;										//联动不能有线
    u32 HgDiffOverlimit:1;									//高度差过大

    //进联动时
	u32 LandscapeOverlimit_Enter:1;							//光斑偏移过大
	u32 D_LandR_Overlimit_Enter:1;							//左右差过大
    u32 LaserRangingToofar_Enter:1;							//联动测距过大
    u32 LaserRangingToonear_Enter:1;						//联动测距过小

    //联动过程中
  	u32 LandscapeLaserOverlimit:1;							//光斑偏移过大
    u32 D_LandR_OverlimitMax:1;								//联动时 左右差过大
	u32 LaserRangingChange:1;								//测距变化过大
	u32 MoveSynErr:1;										//动作同步异常
	
	u32 PointErr:1;											//存在空指针
	u32 WheelRMxaErr:1;										//单车最大轮系运动半径过大
	u32 WheelVMxaErr:1;										//
	u32 AsbEnterErr : 1;									//进入联动条件不满足
	u32 CheckErr : 1;										//校验异常
	
	u32 NC :16;
}AsbErr_TypeDef;


//联动传感器报警参数
typedef struct
{
	u16 DisEnterMax;										//进入联动 最大距离 mm
	u16 DisEnterMin;										//进入联动 最小距离 mm
	u16 DiffLR_EnterMax;									//进入联动 左右差的最大值 mm 兼坐标横向进入值
	u16 LaserSpot_EnterMax;									//进入联动 光斑传感器最大偏移 mm
	u16 Ang_EnterMax;										//进入联动 最大角度 0.01°

    u16 DiffLR_Max;											//联动中 左右差的最大值 mm
	u16 DisChangeMax;										//联动中 最大距离变化
	u16 DisChangeMax_Inf;									//联动中 横向最大距离变化
	u16 AngChangeMax;										//联动中 角度最大距离变化0.01°
	u16 LaserSpotMax;										//联动中 光斑传感器最大偏移 mm
}AsbSensorAlarmPara_TypeDef;

//联动阵型
typedef enum
{
	AsbSiteNo_FB = 0,								   		//一前一后，方向为一顺，联动前进方向与单动一致
	AsbSiteNo_FB1,											//两车一前一后，方向为头对头，联动前进方向以被选则为后车的车的前进方向
	AsbSiteNo_LR,											//两车一左一右，方向为一顺，联动前进方向与单动一致
	AsbSiteNo_End = 65535,								   	//U16
}AsbSiteNo_TypeDef;

//联动阵型位置
//为简化统一，不同阵型中的各个位置，编号设计为：从左往右、从前往后，Z字形从1开始进行编号
typedef enum
{
	AsbSiteSeat_NULL = 0,									//无效
	AsbSiteSeat_1,								   			//1
	AsbSiteSeat_2,								   			//
	AsbSiteSeat_End = 65535,								//U16
}AsbSiteSeat_TypeDef;

//联动传感器
typedef struct
{
	f32 Distance_FL;										//左/前（已安装所在车辆自身为参考）激光测距 单位mm 
	f32 Distance_BR;										//右/后（已安装所在车辆自身为参考）激光测距 单位mm
	f32 LaserSpot;											//光斑偏移 面向光来的方向，光照在传感器右侧为正 单位mm 
}AsbSensor_TypeDef;

//强制参数
typedef struct
{
	u16 ForceAsb_OverJdg : 1;								//强制联动 忽略联动条件
	u16 ForceAsb_OverOtherCar : 1;							//强制联动 忽略其它车的联动状态
	u16 ForceModeEnter : 1;									//强制进入模式
	u16 ForceOverErr : 1;									//强制忽略联动报警
	u16 ForceOverCheck : 1;									//强制忽略参数校验
	u16 ForceUseOwnCmd : 1;									//强制使用自己的命令 而非汇总的命令
	u16 Input2CarsOff : 1;									//强制Input2Cars不调用
	u16 CommParseOff : 1;									//强制CommParse不调用
	u16 OutputOff : 1;										//强制Output不调用
	u16 CommUpdateOff : 1;									//强制CommUpdate不调用
}AsbForceBit_TypeDef;

//联动中主车的联动传感器抽象输入
//以主车联动侧中心为为坐标原点，
//从车联动侧中心在主车上述坐标系中的位置，
//联动从车方向为Pos轴正方向，右手边为Inf轴正方向，Pos轴正方向为Ang轴0度方向，逆时针为正方向
typedef struct
{
	f32 Pos;												//坐标 单位mm 
	f32 PosDiff;											//联动距离的变化 单位mm 注意不要输入，只是方便调试观察 内部会更新
	f32 Inf;												//坐标 单位mm 
	f32 Ang;												//坐标 单位1°
}AsbPlace_TypeDef;

//单车联动时的圆心坐标偏移向量坐标
//以单车时的坐标系为基准坐标系
//z轴坐标含义为目标坐标系的Y轴正方向与基准坐标系的Y轴正方向的夹角，目标坐标轴逆时针为正方向，范围[-180,180]度
//(z暂不参与轮系坐标的变换 计算比较复杂)
typedef struct
{
	f32 X;													//X坐标 单位mm 
	f32 Y;													//Y坐标 单位mm 
	//f32 Z;													//Z坐标 单位1° 暂不考虑两车不平行的情况
}AsbCenter_TypeDef;

//三轴摇杆对象定义
typedef struct
{
	float x;
	float y;
	float z;
}AsbJoystick_TypeDef;

//工作模式定义
typedef enum 
{
	AsbMode_Null = 0,										//初始化
	AsbMode_PosStraight,        							//正向直线行走  
	AsbMode_PosFlexible,       	 							//正向自由行走
	AsbMode_PosTrace,										//正向巡线
	AsbMode_InfStraight,        							//横向直线行走  
	AsbMode_InfFlexible,       	 							//横向自由行走
	AsbMode_InfTrace,										//横向巡线
	AsbMode_Rotation,        								//自转
	AsbMode_Still,           								//静止
	AsbMode_Lift,       									//升降
	AsbMode_Absorption,       								//悬挂减震
	AsbMode_Push,											//推送
	AsbMode_Arm,											//运动臂
	AsbMode_Clip,											//夹持
	AsbMode_Leg, 											//支腿
	AsbMode_Roller,											//滚筒
	AsbMode_Num, 											//总数量
	AsbMode_End=65535, 										//u16
}AsbMode_TypeDef;

//自定义 从车向主车传递的数据对象
typedef struct
{
	s16 S16Data[4];											//
}AsbDataS2M_TypeDef;

//自定义 主车向从车广播的数据对象
typedef struct
{
	//自定义数据
	//s16 S16Data[4];											

	/*避障切区选择*/
	u16 ObsAreaSel;	

	//遥控器命令广播
	//Vxyz 
	s16 Vx;
	s16 Vy;
	s16 Vz;
	AsbMode_TypeDef Mode;									//模式
	/*自动下 路径执行序列号*/
	u16 CurrentPointNum;	

	//实时联动距离
	s16 Dis;

	//阵型编号
	AsbSiteNo_TypeDef SiteNo;								//

	/*避障使能开关*/
	struct
	{
		u32 RadarObsEnF : 1;								//激光雷达避障使能 前
		u32 RadarObsEnB : 1;								//激光雷达避障使能 后
		u32 RadarObsEnL : 1;								//激光雷达避障使能 左
		u32 RadarObsEnR : 1;								//激光雷达避障使能 右
		
		u32 WalkGear : 3;									//行走挡位1~6 二进制001~110
		u32 LiftGear : 3;									//升降挡位1~6 二进制001~110
		
		u32 LiftUp : 1;										//上升命令
		u32 LiftDown : 1;									//下降命令

		u32 LinkCom_ChannelSwitch : 1;						//广播从车，让双车通讯接管摇杆

		u32 UnionVal : 3;									//遥控器单联动模式命令

		u32 Horn : 1;										//喇叭按下
	}Bit;
	
}AsbDataM2S_TypeDef;

//向不同从车单独传递的通信内容 暂时没有实现
typedef struct
{
	/*避障切区选择*/
	u16 ObsAreaSel;											

	/*避障使能开关
	struct
	{
		u16 RadarObsEnF : 1;								//激光雷达避障使能 前
		u16 RadarObsEnB : 1;								//激光雷达避障使能 后
		u16 RadarObsEnL : 1;								//激光雷达避障使能 左
		u16 RadarObsEnR : 1;								//激光雷达避障使能 右
	}Bit;*/
	
}AsbDataM2S_Single_TypeDef;

//控制命令
typedef struct
{
	AsbMode_TypeDef Mode;									//模式
	AsbJoystick_TypeDef Vxyz;								//摇杆命令
	char WalkGear;											//行走挡位Z[1,6]
	char LiftGear;											//升降挡位Z[1,6]
	//位域
	struct
	{
		u16 LiftUp : 1;										//上升命令
		u16 LiftDown : 1;									//下降命令
	}Bit;												
}AsbCmd_TypeDef;

/*避障状态 单个位*/
typedef struct
{
	unsigned int FBar :1;									//防撞触边
	unsigned int FInner :1;									//激光内层
	unsigned int FMiddle : 1;
	unsigned int FOuter : 1;

	unsigned int BBar : 1;
	unsigned int BInner : 1;
	unsigned int BMiddle : 1;
	unsigned int BOuter : 1;

	unsigned int LBar : 1;
	unsigned int LInner : 1;
	unsigned int LMiddle : 1;
	unsigned int LOuter : 1;

	unsigned int RBar : 1;
	unsigned int RInner : 1;
	unsigned int RMiddle : 1;
	unsigned int ROuter :1;
}AsbObsBits1_TypeDef;

//单侧避障雷达状态
typedef enum
{
	AsbObs_Nomal = 0,										/*雷达无障碍物及故障*/
	AsbObs_Warning ,										/*雷达最外层触发*/
	AsbObs_Caution ,										/*雷达中间层触发*/
	AsbObs_Emergency ,										/*雷达最内层触发*/
	AsbObs_Error ,											/*雷达故障*/
}AsbLaser_TypeDef;

/*避障状态 触边+雷达状态*/
typedef struct
{
	unsigned short FBar :1;									//前防撞触边
	AsbLaser_TypeDef FLaser :3;								//前方雷达状态
	unsigned short BBar : 1;								//后方
	AsbLaser_TypeDef BLaser : 3;							//
	unsigned short LBar : 1;								//左侧
	AsbLaser_TypeDef LLaser : 3;							//
	unsigned short RBar : 1;								//右侧
	AsbLaser_TypeDef RLaser : 3;							//
}AsbObsBits_TypeDef;

//故障检出结果定义
typedef enum 
{
    Asb_FaultsType_Null=0,									//初始化
	Asb_FaultsType_Normal,									//正常
	Asb_FaultsType_Warn,									//提示
	Asb_FaultsType_MoveLimited,								//限制运动
	Asb_FaultsType_ControlledStop,							//受控停车
	Asb_FaultsType_LoseControlStop,							//非受控停车
}Asb_FaultsType_TypeDef;

//异常结果定义
typedef struct
{
    Asb_FaultsType_TypeDef Faults;						    //故障类型
    
    struct
    {
        u16 UpLimited : 1;						            //限制上升
		u16 DownLimited : 1;						        //限制下降
		u16 MoveLimited : 1;						        //限制行走
    }Bit;
}AsbEMGFaults_TypeDef;

//单联动切换状态机
typedef struct
{
    AsbAsbMode_TypeDef Cmd;									//单联动控制命令输入
	AsbAsbMode_TypeDef STM;									//单联动控制状态机
    f32 Dis;                                                //实时联动测距 mm
	f32 Dis_Enter;											//联动距离 锁定
}AsbSwitch_TypeDef;

//联动模式切换状态机
typedef enum
{
	AsbModeChg_Init = 0,									//初始化
	AsbModeChg_OneReady = 1,								//一个车就绪 自己
	AsbModeChg_TwoReady = 2,								//两个车就绪 其它车
	AsbModeChg_Exit = 3,									//退出就绪
	AsbModeChg_End = 65535,									//枚举型结束
}AsbModeChg_TypeDef;

//联动 模式切换状态机
typedef struct
{
    AsbModeChg_TypeDef Cmd;									//单联动控制命令输入
	AsbModeChg_TypeDef STM;							        //联动 模式切换过程 状态机
}AsbModeSwitch_TypeDef;

//Car状态位
typedef struct
{
	u16 Moving : 1;											//运动中
	u16 UpLimited : 1;										//上降限位触发
	u16 DownLimited : 1;									//下降限位触发
}AsbCarStateBit_TypeDef;

//Collect状态位
typedef struct
{
	u16 LiftUp : 1;											//上升命令 有一个车升即为1
	u16 LiftDown : 1;							            //下降命令 有一个车降即为1

	u16 Moving : 1;											//运动中 
	u16 UpLimited : 1;										//上降限位触发 有一个触发为1
	u16 DownLimited : 1;									//降限位触发 有一个触发为1
}AsbCollectStateBit_TypeDef;

//联动通信状态位
typedef struct
{
	u16 LiftUp : 1;											//上升命令
	u16 LiftDown : 1;							            //下降命令
	
	u16 Moving : 1;											//运动中
	u16 UpLimited : 1;										//上降限位触发
	u16 DownLimited : 1;									//降限位触发
	u16 ErrUpLimited : 1;									//限制上升
	u16 ErrDownLimited : 1;									//限制下降
	u16 ErrMoveLimited : 1;									//限制行走
	u16 WalkGear : 3;										//行走挡位1~6 二进制001~110
	u16 LiftGear : 3;										//升降挡位1~6 二进制001~110
}AsbCommBit_TypeDef;

//纠正量
typedef struct
{
	f32 Vx;													//Vx纠正量
	f32 Vy;													//Vy纠正量
	f32 Vz;													//Vz纠正量
	f32 Ax;													//Ax纠正量
	f32 Ay;													//Ay纠正量
	f32 Az;													//Az纠正量
	f32 UpDown;												//升降纠正量
	bool Down_Wait;											//下降等待标志
}AsbAdj_TypeDef;

//联动本地参数校验对象
typedef struct
{
	//平滑参数
	f32 VxAcc;												//Vx加速增量
	f32 VyAcc;												//Vy加速增量
	f32 VzAcc;												//Vz加速增量
	f32 VxDec;												//Vy减速增量
	f32 VyDec;												//Vy减速增量
	f32 VzDec;												//Vz减速增量
	f32 AxAcc;												//Ax加速增量
	f32 AyAcc;												//Ay加速增量
	f32 AzAcc;												//Az加速增量
	f32 AxDec;												//Ax减速增量
	f32 AyDec;												//Ay减速增量
	f32 AzDec;												//Az减速增量

	//行走解算参数
	f32 MaxSliAngPos,MaxSliAngInf;							//斜行最大控制映射角度 °
	f32 MaxRotDisPos,MinRotDisPos;							//正向圆弧运动控制映射半径 最大 最小mm
	
	//遥控器选择编号
	u16 RmcId;												//遥控器选择编号

	//避障使能
	struct
	{
		u16 LaserF : 1;										//前激光避障
		u16 LaserB : 1;										//后激光避障
		u16 LaserL : 1;										//左激光避障
		u16 LaserR : 1;										//右激光避障
		u16 BarF : 1;										//前触边
		u16 BarB : 1;										//后触边
		u16 BarL : 1;										//左触边
		u16 BarR : 1;										//右触边
	}ObsEn;

	//位状态
	struct
	{
		u32 AsbSel : 1;										//面板选择联动
	}Bit;
	
}AsbLocalParaCheck_TypeDef;

//联动时底盘参数
typedef struct
{
	AsbJoystick_TypeDef VxyzMax;							//联动时 最大速度命令
	AsbJoystick_TypeDef AxyzMax;							//联动时 最大复合运动命令
}AsbAccPara_TypeDef;

//联动参数
typedef struct
{
	//角色配置
	AsbCarStrNo_TypeDef Iam;								//编号
	/*备忘
	Iam与遥控器选择解绑、
	其他参与控制的地方，全部转到Role上？
	*/
	
	AsbRole_TypeDef Role;									//角色
	/*备忘
	可通过hmi切换
	当配置为主站是，自动切换modbus协议栈，
	暂不考虑设定从站的modbus通信地址，但是要考虑不同从站时，轮系坐标偏移参数将发生变化
	*/

	//尺寸参数
	HalfCar_TypeDef HalfCar;								//车身半长(目前仅约定联动距离通过联动测距计算 非两车中心距离)
	HalfCar_TypeDef HalfCar_Edge;							//边缘的半长参数 特殊联动中心计算用
	HalfCar_TypeDef HalfCar_Edge_Slave;						//从车边缘的半长参数 特殊联动中心计算用
	HalfCar_TypeDef HalfCar_Edge_Slave_List[8];				//conf设定从车边缘的半长参数数组 特殊联动中心计算用
	//阵型配置
	AsbSiteNo_TypeDef SiteNo;								//阵型编号 可自由设定
	AsbSiteSeat_TypeDef Seat;								//阵型位置 暂不设计为自由设定，
	//作为前后头对头联动中心的轮系
	u8 Asb_Point_Num;	
	//和角色绑定固定关系，简化程序设计，同时在超低车中避免联动距离通过联动通信传递参与解算（实际没有实时更新）

	//调整参数
	f32 K_HgAdj;											//升降高度纠正系数
	f32 K_DisAdj;											//行走距离纠正系数
	f32 K_InfAdj;											//横向距离纠正系数
	f32 K_AngAdj;											//角度纠正系数
	u16 HgDiff_Tg;											//升降调平 高度差触发值 设65535关闭该功能
	u16 HgDiff_Re;											//升降调平 高度差恢复值
	AsbAccPara_TypeDef Acc;									//联动时底盘参数

	u16 WirelessController;									//联动遥控器编号
	u16 SlaveCarType;										//从车车型

	//报警参数
	u16 HgDiffMax;											//
	u16 PullApart;											//联动通信线 报警距离 mm
	AsbSensorAlarmPara_TypeDef SensorAlarm;					//传感器报警参数
	u16 SynTimeout;											//运动同步超时时间 ms

	//全局联动参数 仅主车配置
	AsbSiteSeat_TypeDef Seats[ASB_CAR_MAX_NUM];				//参与联动的从车 阵型位置表

	//功能开关
	struct
	{
		u16 TrackOrg : 1;									//0主车汇总广播 1主车直接广播自己的
		u16 RadarObsEn_Single : 1;							//0主车广播统一的 1主车单独设定不同从车 目前无效
		u16 ObsAreaSel_Single : 1;							//0主车广播统一的 1主车单独设定不同从车 目前无效
	}FuncSwitch;

	u16 ErrRecover;											//单车的故障输入触发后 恢复延迟的时间 ms 避免报警快速恢复没有及时发其它车
	u16 AsbSTM_Delay;										//单联动切换延迟时间 ms

	//强制开关参数
	AsbForceBit_TypeDef Bit;								//强制参数位域
}AsbPara_TypeDef;

//输入
typedef struct
{
	u32 Ms;												    //ms
	//开关 选择
	bool AsbSel_Panel;										//面板选择联动
	AsbSelCmd_TypeDef AsbSel_Cmd;							//遥控器单联动选择命令
	bool Wired;												//有线控制

	//传感器
	AsbTrack_TypeDef Track;									//巡线传感器
	AsbSensor_TypeDef Sensor;								//联动传感器
	AsbPlace_TypeDef Place;									//从车位置
	//u16 ManualInput_Dis;									//手动输入的距离 mm
	//AsbSiteNo_TypeDef ManualInput_SiteNo;					//手动输入 阵型编号

	//命令
	AsbCmd_TypeDef OrgCmd;									//原始控制命令
	AsbCmd_TypeDef FinaleCmd;								//压轴控制命令 未合并其它车的
	AsbCmd_TypeDef ExeCmd;									//实际执行的控制命令

	//状态
	AsbMode_TypeDef ModeReal;								//实际模式
	u16 Hg;													//升降高度
	u32 WheelRMax, WheelVMax;								//单车解算结果
	AsbObsBits_TypeDef Obs;									//避障状态									
	AsbCarStateBit_TypeDef Bit;								//状态位
	
	
	AsbEMGFaults_TypeDef OrgErr;							//自身故障状态
	AsbEMGFaults_TypeDef ExeErr;							//实际执行的故障状态

	//向不同从车单独传递的通信内容 暂时未使用
	AsbDataM2S_Single_TypeDef SingleCustom[ASB_CAR_MAX_NUM];

	//新增从车向主车传递的信息
	AsbDataS2M_TypeDef CustomS;

	//新增主车向从车广播的信息
	AsbDataM2S_TypeDef CustomM;
	
	//检验对象
	AsbLocalParaCheck_TypeDef CheckData;					//本地校验参数
	u16 CheckResult;										//校验结果
}AsbInput_TypeDef;

//输出
typedef struct
{
	AsbAsbMode_TypeDef AsbMode;								//模式
	AsbErr_TypeDef AsbErr;									//联动故障枚举
	AsbEMGFaults_TypeDef CarErr;							//汇总执行的故障结果

	//合并后的控制命令
	AsbObsBits_TypeDef Obs;									//避障状态
	AsbCollectStateBit_TypeDef Bit;							//状态位
	AsbCmd_TypeDef Cmd;										//控制命令
	bool CmdExe;											//动作执行使能

	AsbAdj_TypeDef Adj;										//纠正量
	AsbTrack_TypeDef Track;									//巡线传感器
	AsbCenter_TypeDef Center;								//单车联动时的圆心坐标偏移向量坐标
	AsbCenter_TypeDef Center_Cal;							//

	//解算缩放参数
	f32 WheelRMax;											//最大轮系运动半径
	f32 WheelVMax;											//最大轮系速度
}AsbOutput_TypeDef;

//单车参数
typedef struct
{
	u32 Ms; 												//ms循环

	//锁定参数输入
	AsbCenter_TypeDef Center;								//单车联动时的圆心坐标偏移向量坐标 
	f32 Dis;												//联动距离 mm 两车之间 注意主车是实时值 从车是锁定值
	f32 Dis_Half;											//两车间距一半 mm 主车从车都是锁定值
	bool InAsb;												//为1表示在联动中
	AsbSelCmd_TypeDef AsbSel_Last;							//遥控器单联动命令 上次值

	//控制输入
	u32 WheelRMax,WheelVMax;								//单车解算结果 最终的最大轮系运动半径 速度
	AsbCmd_TypeDef CmdIn;									//控制命令
	AsbCmd_TypeDef CmdOut;									//控制命令 输出
	AsbTrack_TypeDef Track;									//巡线传感器
	AsbAdj_TypeDef Adj;										//纠正量

	//状态输入
	AsbMode_TypeDef ModeReal;								//实际模式
	u16 Hg;													//升降高度
	AsbObsBits_TypeDef Obs;									//避障状态
	AsbErr_TypeDef AsbErr;									//联动故障枚举
	AsbEMGFaults_TypeDef Err;								//自身故障状态
	AsbCarStateBit_TypeDef Bit;								//状态位
										
	u16 Check;												//其它参数校验码

	//内部状态
	AsbSwitch_TypeDef AsbSwitch;							//联动切换对象
	AsbModeSwitch_TypeDef ModeSwitch;						//联动 模式切换过程 状态机
}AsbCar_TypeDef;

//联动通信从车数据对象 -> Cars对象
typedef struct
{
	u16 Ms;													//ms循环

	//锁定参数
	f32 Dis;												//联动距离 mm 两车之间

	//控制
	f32 WheelRMax;											//单车解算结果 最终的最大轮系运动半径
	f32	WheelVMax;											//速度
	AsbAsbMode_TypeDef AsbSTM;								//单联动控制状态机
	AsbModeChg_TypeDef ModeSTM;								//模式切换状态机
	AsbMode_TypeDef Mode;									//模式命令
	AsbTrack_TypeDef Track;									//巡线传感器
	
	//状态
	AsbMode_TypeDef ModeReal;								//实际模式
	u16 Hg;													//升降高度
	AsbObsBits_TypeDef Obs;									//避障状态
	Asb_FaultsType_TypeDef Err;								//故障等级
	AsbCommBit_TypeDef Bit;									//状态位

	//新增从车向主车传递s16*4的信息
	AsbDataS2M_TypeDef CustomS;

	u16 Check;												//其它参数校验码

}AsbCommSlave_TypeDef;

//指针对象
typedef struct
{
    AsbCar_TypeDef *p2Self;                                 //指向自己
}AsbP2Obj_TypeDef;

//联动 Collect汇总数据
typedef struct
{
	u16 Ms;													//ms循环
    u32 WheelRMax;										    //最大轮系运动半径
    u32 WheelVMax;										    //最大轮系速度

	//状态
    u16 HgMin;												//最小高度值
    u16 HgMax;												//最大高度值
	f32 Dis;												//联动距离 mm 两车之间 每个车访问不一样？
    AsbObsBits_TypeDef Obs;									//避障状态
	AsbEMGFaults_TypeDef ErrMax;							//最高故障等级
	AsbMode_TypeDef ModeReal;								//实际模式
	AsbCollectStateBit_TypeDef Bit;							//状态位

	//控制
    char WalkGearMin;										//最小行走档位
    char LiftGearMin;										//最小升降档位
	AsbAsbMode_TypeDef AsbSTM;								//单联动控制状态机
	AsbModeChg_TypeDef ModeSTM;								//模式切换状态机
	AsbMode_TypeDef Mode;									//模式 都是同一个模式 或 为无效
	AsbTrack_TypeDef Track;									//巡线传感器

	//
    u32 Ms_MoveTogether;									//两车动作同步时间时 更新
    u32 Ms_MoveTogetherTimeout;								//两车动作同步超时时间 单位ms

	u16 Check;												//其它参数校验码 计算不一致则输出0xFFFF
}AsbCollectObj_TypeDef; 

//联动通信主车数据对象 -> Collec对象
typedef struct
{
	u16 Ms;													//ms循环

	//状态
	f32 Dis;												//联动距离 mm 两车之间
	u32 WheelRMax;										    //最大轮系运动半径
	u32 WheelVMax;										    //最大轮系速度

	AsbMode_TypeDef ModeReal;								//实际模式
	u16 Hg;													//升降高度
	AsbObsBits_TypeDef Obs;									//避障状态
	Asb_FaultsType_TypeDef Err;								//故障等级-去掉对方车故障
	AsbTrack_TypeDef Track;									//巡线传感器
	AsbCommBit_TypeDef Bit;									//状态位

	//控制
	AsbAsbMode_TypeDef AsbSTM;								//单联动控制状态机
	AsbModeChg_TypeDef ModeSTM;								//模式切换状态机
	AsbMode_TypeDef Mode;									//模式命令

	//新增主车向从车广播
	AsbDataM2S_TypeDef CustomM;

	u16 Check;												//其它参数校验码

}AsbCommMaster_TypeDef;

//联动通信数据对象
typedef struct
{
	u16 SlaveDataLength;									//主车访问从车 数据u16长度
	u16 MasterDataLength;									//主车发从车 数据u16长度
	AsbCommSlave_TypeDef SlaveData[ASB_CAR_MAX_NUM];		//从车通信数据 被主车访问
	AsbCommMaster_TypeDef MasterData;						//主车通信数据
	s16 DiffMs;												//时间差 = 当前ms - 从站返回ms
}AsbComm_TypeDef;

//ms记录对象
typedef struct
{
	u32 ErrRecover;
	u32 AsbSTM_Delay;
}AsbMsRecord_TypeDef;

//联动对象
typedef struct
{
	//输入
	AsbPara_TypeDef Para;									//参数对象
	AsbInput_TypeDef Input;									//输入

	//内部状态
	AsbMsRecord_TypeDef MsRecord;							//ms记录对象
	AsbCar_TypeDef Cars[ASB_CAR_MAX_NUM];					//单车输入对象
	AsbCollectObj_TypeDef Collect;							//汇总数据
	AsbP2Obj_TypeDef P2Obj;									//指针对象

	//计算和输出部分
	AsbOutput_TypeDef Output;
	AsbComm_TypeDef Comm;									//联动通信数据对象
}AsbCtrl_TypeDef;

//函数声明
Assembly_EXT bool Asb_Main(AsbCtrl_TypeDef* p);
Assembly_EXT bool Asb_Reset(AsbCtrl_TypeDef* p);
#endif

//***********************************END OF FILE***********************************
