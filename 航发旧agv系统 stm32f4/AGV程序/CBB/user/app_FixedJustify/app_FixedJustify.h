/**
  ************************************* Copyright ******************************   
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  * 
  * FileName   :	app_FixedJustify.h   
  * Version    :	v2.1		
  * Author     :	Master	
  * Date       :	2025年6月13日  
  * ModificationDate : 2025年10月22日  
  * Description:    
  ******************************************************************************
 */


/*include----------------------------------------------------------------------*/
/*自定义引用*/
#include "stdint.h"
/*define-----------------------------------------------------------------------*/

#ifndef _APP_FIXEDJUSTIFY_H_
#define _APP_FIXEDJUSTIFY_H_

//控制对象实例化只被指定文件编译时实例化一次
#ifdef  APP_FixedJustify_GLOBALS
	#define APP_FixedJustify_EXT
#else
	#define APP_FixedJustify_EXT  extern
#endif

#define  ADJUST_STEP_COUNT     6	// 步骤总数

//纠偏模式枚举	
typedef enum
{
	FixedJustify_Still				= 0,
	FixedJustify_Mode_Rotation 		= 1,
	FixedJustify_Mode_PosFlexible 	= 2,
	FixedJustify_Mode_InfFlexible 	= 3,
}FixedJustify_Mode_EnumDef;

// 调整步骤类型定义
typedef enum 
{
    ADJUST_STEP_Z = 0,    // 纠Z轴
    ADJUST_STEP_X,        // 纠X轴  
    ADJUST_STEP_Y,        // 纠Y轴
	ADJUST_STEP_X_DIFF,   // 整车差速纠X轴  
} AdjustStepType;

// X轴调整子状态定义
typedef enum
{
    X_SUBSTATE_IDLE = 0,      // 空闲状态
    X_SUBSTATE_PHASE1,        // 阶段1：旋转对准
    X_SUBSTATE_PHASE2,        // 阶段2：前后移动  
    X_SUBSTATE_PHASE3,        // 阶段3：旋转回正
    X_SUBSTATE_COMPLETED,     // 完成
}XAdjustSubState;

// X轴调整上下文
typedef struct
{
    XAdjustSubState state;        // 当前子状态
    float initial_angle;          // 初始角度
    float initial_x;              // 初始X位置
    float target_rotation_angle;  // 目标旋转角度
} XAdjustContext;

//初始化参数
typedef struct
{
	//到位精度(死区值）
	int32_t x_offset_max;	//单位 1mm
	int32_t y_offset_max;	//单位 1mm
	int16_t z_offset_max;	//单位 0.1°
	
	/*纠偏速度 区分正负速度*/
	float max_x_Speed;		// X方向最大纠偏速度
	float min_x_Speed;		// X方向最小纠偏速度
	
	float max_y_Speed;		// Y方向最大纠偏速度
	float min_y_Speed;		// Y方向最小纠偏速度
	
	float max_z_Speed;		// Z方向最大纠偏速度
	float min_z_Speed;		// Z方向最小纠偏速度
	
    float decel_zone;       // 开始减速区域 (mm)
    float stop_threshold;   // 停止阈值 (mm)
    float kp_near;          // 近距离比例系数
    float kp_far;           // 远距离比例系数
    float kp_transition;    // 过渡区起始偏差 (mm)
	
    //调整顺序配置
    AdjustStepType adjust_order[ADJUST_STEP_COUNT];	//调整顺序列表
	
    uint8_t step_count;								//总调整步骤

}FixedJustify_Para_TypeDef;


//输入
typedef struct
{
	FixedJustify_Mode_EnumDef	ActualMode;
	int32_t 					x;	//单位 1mm
	int32_t 					y;	//单位 1mm
	int16_t 					z;	//单位 0.1°
	
	int16_t 					rotation_angle;  // 实时旋转角度
	uint8_t						Sta;//识别状态 0-传感器未识别到目标 1-传感器识别正常
	
}FixedJustify_Input_TypeDef;

//输入
typedef struct
{
	uint8_t FixedJustifyStep;
	uint8_t current_adjust_index;  // 当前调整步骤索引

    // 差速AGV特有临时变量，X轴子状态机上下文
    XAdjustContext x_context;
}FixedJustify_Temp_TypeDef;


//输出
typedef struct
{
	FixedJustify_Mode_EnumDef	TargetMode;
	float 						Vx;
	float 						Vy;
	float 						Vz;
	uint8_t						FinishFlag; 		//0-纠偏中  1-完成纠偏
	uint8_t						InExecutionFlag;	//纠偏执行中
	uint8_t						BeginFlag; 			//1-开始接管遥控器 0-接管结束
}FixedJustify_Output_TypeDef;

typedef struct
{
	/*初始化参数*/
	FixedJustify_Para_TypeDef	Para;
	/*输入*/
	FixedJustify_Input_TypeDef	Input;
	
	FixedJustify_Temp_TypeDef	Temp;
	/*输出*/
	FixedJustify_Output_TypeDef	Output;
}FixedJustify_TypeDef;

	
APP_FixedJustify_EXT	uint8_t app_FixedJustify_main(FixedJustify_TypeDef *p);
#endif


 

