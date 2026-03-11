/********************************** Copyright *********************************** 
  *
  *                           成都航发机器人有限公司
  *                            All Rights Reserved
  *
  * FileName   : app_AGVKalmanFilter.c
  * Version    : V1.0
  * Author     : Master
  * Date       : 2024.xx.xx
  * Description: AGV自动运行的框架代码的初始化
  *******************************************************************************/
#define  APP_AGVKALMANFILTER_GLOBALS
#include <stm32f4xx.h>

// 定义卡尔曼滤波器结构体
typedef struct {
    float state_estimate;         // 状态估计值
    float covariance_estimate;    // 误差协方差估计值
    float process_variance;       // 过程噪声方差
    float measurement_variance;   // 测量噪声方差
} KalmanFilter1D;

// 初始化卡尔曼滤波器
void kalman_filter_init(KalmanFilter1D* kf, float initial_state, float initial_covariance, float process_variance, float measurement_variance) {
    kf->state_estimate = initial_state;
    kf->covariance_estimate = initial_covariance;
    kf->process_variance = process_variance;
    kf->measurement_variance = measurement_variance;
}

// 预测步骤
void kalman_filter_predict(KalmanFilter1D* kf, float (*process_model)(float)) {
    kf->state_estimate = process_model(kf->state_estimate);
    kf->covariance_estimate += kf->process_variance;
}

// 更新步骤
void kalman_filter_update(KalmanFilter1D* kf, float measurement) {
    // 计算卡尔曼增益
    float kalman_gain = kf->covariance_estimate / (kf->covariance_estimate + kf->measurement_variance);
    // 更新状态估计
    kf->state_estimate += kalman_gain * (measurement - kf->state_estimate);
    // 更新误差协方差
    kf->covariance_estimate = (1 - kalman_gain) * kf->covariance_estimate;
}

// 简单的运动模型，假设目标位置在下一时刻保持不变
float process_model(float x) {
    return x;
}
// 初始化卡尔曼滤波器
KalmanFilter1D kf = {0,1.0f,1.0f,4.0f};
float measurements[5] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
float Estimated[5];
float Covariance[5];
int Drv_MergeToPGV_KalmanFilter(void) {
   int16_t i = 0;
	// 模拟测量数据
    
    int num_measurements = sizeof(measurements) / sizeof(measurements[0]);

    // 使用卡尔曼滤波器处理测量数据
    for (i = 0; i < num_measurements; i++) {
        float z = measurements[i];
        kalman_filter_predict(&kf, process_model);
        kalman_filter_update(&kf, z);
		Estimated[i] = kf.state_estimate;
		Covariance[i] = kf.covariance_estimate;
        //printf("Measurement: %.2f, Estimated State: %.2f, Covariance: %.2f\n", z, kf.state_estimate, kf.covariance_estimate);
    }

    return 0;
}


void app_AGVKalmanFilter_Pross(void)
{
	{
//		static float x_Estimated =0.0f;
//		static float x_Covariance =0.0f;
//		static uint8_t MASTER_kalman = 0;
//		float z = 0.0f;
//		if(MASTER_kalman == 0)
//		{
//			//z = g_AGVHostCtrl->NavigationData.NavLidar_x * 1.0f;
//		}
//		else if(MASTER_kalman == 1)
//		{
//			//z = g_AGVHostCtrl->NavigationData.NavLidar_x * 1.0f+g_AGVHostCtrl->Sys1msCounter%10;
//		}
//		else if(MASTER_kalman == 2)
//		{
//			z = 0;
//		}
//		
//        kalman_filter_predict(&kf, process_model);
//        kalman_filter_update(&kf, z);
//		x_Estimated = kf.state_estimate;
//		x_Covariance = kf.covariance_estimate;
	}
}
/************************************END OF FILE************************************/

