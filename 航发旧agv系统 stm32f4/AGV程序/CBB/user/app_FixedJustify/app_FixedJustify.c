/**
	************************************* Copyright ****************************** 
	*
	*                           成都航发机器人有限公司
	*                            All Rights Reserved
	*
	*
	* FileName   :	app_FixedJustify.c
	* Version    :	v2.1
	* Author     :	Master
	* Date       : 2025年6月13日
	* ModificationDate : 2025年10月22日  
	* Description:
	******************************************************************************
**/
//声明对象在此文件编译时实例化
#define  APP_FixedJustify_GLOBALS

#include "includes.h"
#include "app_FixedJustify.h"

/***********************************************************
** 函数名称:	app_FixedJustify_Reset
** 功能描述: 重置对正状态机
** 参    数:p - 对正结构体指针
***********************************************************/
void app_FixedJustify_Reset(FixedJustify_TypeDef *p)
{
    p->Temp.FixedJustifyStep = 0;
    p->Temp.current_adjust_index = 0;
    p->Output.FinishFlag = 0;
    p->Output.Vx = 0;
    p->Output.Vy = 0;
    p->Output.Vz = 0;
}

/***********************************************************
** 函数名称:	init_x_adjust_context
** 功能描述: 初始化X轴调整上下文
** 参    数: ctx - X轴调整上下文指针
***********************************************************/
void init_x_adjust_context(XAdjustContext *ctx)
{
    ctx->state = X_SUBSTATE_IDLE;
    ctx->initial_angle = 0.0f;
    ctx->initial_x = 0.0f;
    ctx->target_rotation_angle = 0.0f;
}

/***********************************************************
** 函数名称: reset_adjust_substate_machine
** 功能描述: 重置对正状态机
** 参    数: p - 对正结构体指针
***********************************************************/
void reset_adjust_substate_machine(FixedJustify_TypeDef *p)
{
	app_FixedJustify_Reset(p);
    init_x_adjust_context(&(p->Temp.x_context));
}

/***********************************************************
** 函数名称:	normalize_angle
** 功能描述: 角度归一化到-180~180度
** 参    数: angle - 输入角度
** 返 回 值:	归一化后的角度
***********************************************************/
float normalize_angle(float angle)
{
    while(angle > 180.0f) angle -= 360.0f;
    while(angle < -180.0f) angle += 360.0f;
    return angle;
}

/***********************************************************
** 函数名称:	adjust_arrival_speed(float target, float current)
** 功能描述: 到位控制函数
** 参    数: 实际值，目标值,死区值,最小速度，最大速度
** 返 回 值:	 纠偏速度
***********************************************************/
float adjust_arrival_speed(FixedJustify_TypeDef *p,float target, float current,float DeadZoneValue,float min_speed,float max_speed)
{
	FixedJustify_Para_TypeDef	*p_Para	= &(p->Para);
	float direction;			//速度方向
    float final_speed;			//最终速度
	float stop_threshold;		//死区值
	float error_x,abs_error;	//差值
	float target_speed = 0.0f;	//自适应速度初始化
	
	float decay_factor;			//衰减后的速度
	
	float previous_speed;		//上一次速度
	
	stop_threshold = DeadZoneValue;
    // 1. 计算位置偏差
    error_x = target - current;
    abs_error = fabs(error_x);
    
    // 2. 判断是否到达目标位置
    if (abs_error <= stop_threshold) 
	{
        // 执行到位停止动作
        final_speed = 0;
        return final_speed;
    }
    //记录上一次速度
	previous_speed = final_speed;
    // 3. 自适应速度计算
    
    // 非线性自适应控制算法
    if (abs_error > p_Para->kp_transition) 
	{
        // 远距离区域 - 使用大比例系数
        target_speed = p_Para->kp_far * abs_error;
    }else
	{
        // 近距离区域 - 使用小比例系数
        target_speed = p_Para->kp_near * abs_error;
        
        // 在减速区添加指数衰减因子
        if (abs_error < p_Para->decel_zone) 
		{
            decay_factor = powf((abs_error * p_Para->kp_near), 2.0f);
            target_speed *= decay_factor;
        }
		//速度出现突变,继续使用上一次记录值,最低速度的3备认为突变
		if(target_speed > min_speed *3)
			target_speed = previous_speed;
		
    }
    
    // 4. 确保最小速度
    if (target_speed < min_speed) 
	{
        target_speed = min_speed;
    }
    
    // 5. 速度限幅
    if (target_speed > max_speed) 
	{
        target_speed = max_speed;
    }
    
    // 6. 方向判断
    direction = (error_x > 0) ? -1.0f : 1.0f;
	// 7. 最终速度输出
    final_speed = direction * target_speed;
    
	return final_speed;
}

/***********************************************************
** 函数名称:	adjust_rotation_angle
** 功能描述: 调整Z轴
** 参    数:p - 对正结构体指针
** 返 回 值:	0-调整完成  1-调整中
***********************************************************/
uint8_t adjust_rotation_angle(FixedJustify_TypeDef *p)
{
    FixedJustify_Output_TypeDef *p_Out = &(p->Output);
    FixedJustify_Input_TypeDef *p_In = &(p->Input);
    FixedJustify_Para_TypeDef *p_Para = &(p->Para);
    
    p_Out->TargetMode = FixedJustify_Mode_Rotation;
    if(p_In->ActualMode == FixedJustify_Mode_Rotation)
	{

        p_Out->Vz = adjust_arrival_speed(p,  p_In->z, 0, 
                                        p_Para->z_offset_max, 
                                        p_Para->min_z_Speed, 
                                        p_Para->max_z_Speed);
        
        if(fabs(p_In->z) <= p_Para->z_offset_max)
		{
            return 0; // 调整完成
        }
    }
    return 1; // 调整中
}


/***********************************************************
** 函数名称:	adjust_lateral_direct
** 功能描述: 调整X轴（全向车）
** 参    数:p - 对正结构体指针
** 返 回 值:	0-调整完成  1-调整中
***********************************************************/
uint8_t adjust_lateral_direct(FixedJustify_TypeDef *p)
{
    FixedJustify_Output_TypeDef *p_Out = &(p->Output);
    FixedJustify_Input_TypeDef *p_In = &(p->Input);
    FixedJustify_Para_TypeDef *p_Para = &(p->Para);
    
    p_Out->TargetMode = FixedJustify_Mode_InfFlexible;
	
    if(p_In->ActualMode == FixedJustify_Mode_InfFlexible) 
	{
        p_Out->Vx = adjust_arrival_speed(p, p_In->x, 0, 
                                        p_Para->x_offset_max, 
                                        p_Para->min_x_Speed, 
                                        p_Para->max_x_Speed);
        
        if(fabs(p_In->x) <= p_Para->x_offset_max)
		{
            return 0; // 调整完成
        }
    }
    return 1; // 调整中
}

/***********************************************************
** 函数名称:	adjust_longitudinal
** 功能描述: 调整Y轴
** 参    数:p - 对正结构体指针
** 返 回 值:	0-调整完成  1-调整中
***********************************************************/
uint8_t adjust_longitudinal(FixedJustify_TypeDef *p)
{
    FixedJustify_Output_TypeDef *p_Out = &(p->Output);
    FixedJustify_Input_TypeDef *p_In = &(p->Input);
    FixedJustify_Para_TypeDef *p_Para = &(p->Para);
    
    p_Out->TargetMode = FixedJustify_Mode_PosFlexible;
	
    if(p_In->ActualMode == FixedJustify_Mode_PosFlexible)
	{
        p_Out->Vy = adjust_arrival_speed(p, p_In->y, 0, 
                                        p_Para->y_offset_max, 
                                        p_Para->min_y_Speed, 
                                        p_Para->max_y_Speed);
        
        if(fabs(p_In->y) <= p_Para->y_offset_max)
		{
            return 0; // 调整完成
        }
    }
    return 1; // 调整中
}

/***********************************************************
** 函数名称:	adjust_lateral_phase1_rotation
** 功能描述: 横向调整阶段1：旋转对准（差速AGV）原理：旋转指定角度，使车头朝向需要横向移动的方向
** 参    数:p - 对正结构体指针
** 返 回 值:	0-调整完成  1-调整中
***********************************************************/
uint8_t x_adjust_phase1_rotation(FixedJustify_TypeDef *p)
{
	FixedJustify_Output_TypeDef *p_Out = &(p->Output);
    FixedJustify_Input_TypeDef *p_In = &(p->Input);
    FixedJustify_Para_TypeDef *p_Para = &(p->Para);
    XAdjustContext *x_ctx = &(p->Temp.x_context);
    
    p_Out->TargetMode = FixedJustify_Mode_Rotation;
	
    if(p_In->ActualMode == FixedJustify_Mode_Rotation) 
	{

        p_Out->Vz = adjust_arrival_speed(p, p_In->rotation_angle, x_ctx->target_rotation_angle, 
                                        p_Para->z_offset_max, 
                                        p_Para->min_z_Speed, 
                                        p_Para->max_z_Speed);
        
 
		if(!p_Out->Vz) 
		{
            return 2; // 阶段1完成
        }
        

    }
    
    return 1; // 阶段1进行中行中
}

/***********************************************************
** 函数名称:	adjust_lateral_phase2_movement
** 功能描述: 横向调整阶段2：前后移动（差速AGV）原理：旋转后的方向上前后移动，实现横向位移
** 参    数:p - 对正结构体指针
** 返 回 值:	0-调整完成  1-调整中
***********************************************************/
uint8_t x_adjust_phase2_movement(FixedJustify_TypeDef *p)
{
    FixedJustify_Output_TypeDef *p_Out = &(p->Output);
    FixedJustify_Input_TypeDef *p_In = &(p->Input);
    FixedJustify_Para_TypeDef *p_Para = &(p->Para);

    
    p_Out->TargetMode = FixedJustify_Mode_PosFlexible;
	
    if(p_In->ActualMode == FixedJustify_Mode_PosFlexible)
	{

		p_Out->Vy = adjust_arrival_speed(p, p_In->x, 0, 
                                         p_Para->x_offset_max, 
                                         p_Para->min_x_Speed, 
                                         p_Para->max_x_Speed);
                        
		 if(!p_Out->Vy) 
		 {
			 return 2; // 阶段2完成
		 }
            
    }
    
    return 1; // 阶段2进行中
}

/***********************************************************
** 函数名称:	adjust_lateral_phase3_restore
** 功能描述: 横向调整阶段3：旋转回正（差速AGV）原理：旋转回原始角度，保持车头方向
** 参    数:p - 对正结构体指针
** 返 回 值:	0-调整完成  1-调整中
***********************************************************/
uint8_t x_adjust_phase3_restore(FixedJustify_TypeDef *p)
{
    FixedJustify_Output_TypeDef *p_Out = &(p->Output);
    FixedJustify_Input_TypeDef *p_In = &(p->Input);
    FixedJustify_Para_TypeDef *p_Para = &(p->Para);
    XAdjustContext *x_ctx = &(p->Temp.x_context);
    
    p_Out->TargetMode = FixedJustify_Mode_Rotation;
	
    if(p_In->ActualMode == FixedJustify_Mode_Rotation)
	{
        // 计算角度误差（相对于初始角度）
        float angle_error = p_In->rotation_angle - x_ctx->initial_angle;
        
        p_Out->Vz = adjust_arrival_speed(p, angle_error, 0, 
                                        p_Para->z_offset_max,  
                                        p_Para->min_z_Speed, 
                                        p_Para->max_z_Speed);
        
        
        // 检查是否回到原始角度
        if(fabs(angle_error) <= p_Para->z_offset_max)
		{
            return 2; // 阶段3完成
        }
    }
    
    return 1; // 阶段3进行中
}


/***********************************************************
** 函数名称:	adjust_x_axis_substate_machine
** 功能描述: X轴调整子状态机
** 参    数:p - 对正结构体指针
** 返 回 值:	0-完成 1-进行中 2-失败
***********************************************************/
uint8_t adjust_x_axis_substate_machine(FixedJustify_TypeDef *p)
{

    XAdjustContext *x_ctx = &(p->Temp.x_context);
    
    uint8_t result = 1; // 默认进行中
    
    switch(x_ctx->state)
	{
        case X_SUBSTATE_IDLE: // 空闲状态 - 初始化
        {                
            x_ctx->state = X_SUBSTATE_PHASE1;
        }
        break;
        
        case X_SUBSTATE_PHASE1: // 阶段1：旋转对准
        {
            result = x_adjust_phase1_rotation(p);
            if(result == 2)
			{
                // 阶段1完成，进入阶段2
                x_ctx->state = X_SUBSTATE_PHASE2;
            } 
        }
        break;
        
        case X_SUBSTATE_PHASE2: // 阶段2：前后移动
        {
            result = x_adjust_phase2_movement(p);
            if(result == 2)
			{
                // 阶段2完成，进入阶段3
                x_ctx->state = X_SUBSTATE_PHASE3;

            } 
        }
        break;
        
        case X_SUBSTATE_PHASE3: // 阶段3：旋转回正
        {
            result = x_adjust_phase3_restore(p);
            if(result == 2)
			{
                // 阶段3完成，整个X轴调整完成
                x_ctx->state = X_SUBSTATE_COMPLETED;
  
            } 
        }
        break;
        
        case X_SUBSTATE_COMPLETED: // 完成状态
        {
            result = 0; // 返回完成
        }
        break;
		default:break;
        
    }
     
    return result;
}

/***********************************************************
** 函数名称:	app_FixedJustify_main
** 功能描述: 对正主函数
** 参    数:p - 对正结构体指针
** 返 回 值:	0-纠偏结束  1-纠偏中
***********************************************************/
uint8_t app_FixedJustify_main(FixedJustify_TypeDef *p)
{
    uint8_t res = 0xFF;
    FixedJustify_Output_TypeDef *p_Out = &(p->Output);
    FixedJustify_Para_TypeDef *p_Para = &(p->Para);
    FixedJustify_Temp_TypeDef *p_Temp = &(p->Temp);
	
	AdjustStepType current_step;			//创建中间变量记录当前执行步骤

    switch(p_Temp->FixedJustifyStep)
    {
        case 0: // 初始化阶段，速度，相关标志位置零
        {
            res = 1;
            p_Out->Vx = 0;
            p_Out->Vy = 0;
            p_Out->Vz = 0;
            p_Out->TargetMode = FixedJustify_Still;
            p_Temp->current_adjust_index = 0;
            p_Temp->FixedJustifyStep = 1;
      
        }
        break;
        
        case 1: // 执行调整步骤
        {
            res = 1;
            // 检查是否所有步骤已完成
            if(p_Temp->current_adjust_index >= p_Para->step_count) //执行步骤计数大于等于总步骤数，判定为执行完成，跳转状态机
			{
                p_Temp->FixedJustifyStep = 2; // 所有步骤完成
                break;
            }
            
            // 获取当前调整步骤类型
			current_step = p_Para->adjust_order[p_Temp->current_adjust_index];		//索引初始化中的步骤顺序
            
            // 根据步骤类型执行相应调整
            switch(current_step) 
			{
                case ADJUST_STEP_Z: // 纠Z轴
                {
					res = adjust_rotation_angle(p);
                }
                break;
                
                case ADJUST_STEP_X: // 纠X轴 标准差速舵轮
                {
					res = adjust_lateral_direct(p);		
                }
                break;
                
                case ADJUST_STEP_Y: // 纠Y轴
                {
					res = adjust_longitudinal(p);
                }
                break;
				
                case ADJUST_STEP_X_DIFF: // //整车差速调整X轴
                {
					res = adjust_x_axis_substate_machine(p);
                }
                break;
				default:break;
            }

            // 检查步骤完成
            if(res == 0) 
			{ // 步骤完成
                p_Temp->current_adjust_index++;
            } 			

        }
        break;
        
        case 2: // 完成阶段
        {
            res = 0;
			reset_adjust_substate_machine(p);	//对正完成重置所有状态机
            p_Out->FinishFlag = 1;
            // 保持完成状态，直到外部重置
        }
        break;
    }
    
    /*同步定位完成标记到外部*/
//    p_Out->FinishFlag = (res == 0) ? 1 : 0;
    
    return res;
}

/************************************END OF FILE************************************/
