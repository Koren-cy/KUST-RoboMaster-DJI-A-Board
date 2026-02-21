/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_ladrc.h"

/* 私有函数声明 --------------------------------------------------------------*/
static void LESO_Calculate(LESO_Controller *leso, float feedback, float u, float b0, float dt);
static float LSEF_Calculate(const LSEF_Controller *lsef, float e1, float e2);

/* 函数体 --------------------------------------------------------------------*/

/**
 * @brief 初始化 LADRC 控制器
 * @param ladrc     LADRC 控制器结构体指针
 * @param wc        观测器带宽
 * @param kp        比例增益
 * @param kd        微分增益
 * @param b0        控制增益补偿
 * @param max_out   输出限幅
 * @param dt        采样时间
 */
void LADRC_Init(LADRC_Controller *ladrc,
                const float wc, const float kp, const float kd,
                const float b0, const float max_out, const float dt) {
    // 初始化线性扩张状态观测器
    ladrc->leso.wc = wc;
    ladrc->leso.z1 = 0.0f;
    ladrc->leso.z2 = 0.0f;
    ladrc->leso.z3 = 0.0f;
    
    // 初始化线性状态误差反馈控制律
    ladrc->lsef.kp = kp;
    ladrc->lsef.kd = kd;
    
    // 初始化控制器参数
    ladrc->b0 = b0;
    ladrc->max_out = max_out;
    ladrc->dt = dt;
    
    ladrc->set = 0.0f;
    ladrc->fdb = 0.0f;
    ladrc->out = 0.0f;
    ladrc->u0 = 0.0f;
}

/**
 * @brief 设定 LADRC 目标值
 * @param ladrc     LADRC 控制器结构体指针
 * @param target    目标值
 */
void LADRC_SetTarget(LADRC_Controller *ladrc, const float target) {
    ladrc->set = target;
}

/**
 * @brief 计算 LADRC 控制输出
 * @param ladrc     LADRC 控制器结构体指针
 * @param feedback  反馈值
 * @return LADRC 控制输出值
 */
float LADRC_Calculate(LADRC_Controller *ladrc, const float feedback) {
    ladrc->fdb = feedback;
    
    // 线性扩张状态观测器
    LESO_Calculate(&ladrc->leso, feedback, ladrc->out, ladrc->b0, ladrc->dt);
    
    // 线性状态误差反馈控制律
    const float e1 = ladrc->set - ladrc->leso.z1;
    const float e2 = 0.0f - ladrc->leso.z2;
    ladrc->u0 = LSEF_Calculate(&ladrc->lsef, e1, e2);
    
    // 扰动补偿
    ladrc->out = (ladrc->u0 - ladrc->leso.z3) / ladrc->b0;
    
    // 输出限幅
    if (ladrc->out > ladrc->max_out) {
        ladrc->out = ladrc->max_out;
    } else if (ladrc->out < -ladrc->max_out) {
        ladrc->out = -ladrc->max_out;
    }
    
    return ladrc->out;
}

/**
 * @brief 线性扩张状态观测器计算
 * @param leso      线性扩张状态观测器结构体指针
 * @param feedback  反馈值
 * @param u         控制输入
 * @param b0        控制增益
 * @param dt        采样时间
 */
static void LESO_Calculate(LESO_Controller *leso, const float feedback, const float u, const float b0, const float dt) {
    const float e = leso->z1 - feedback;
    
    // 基于带宽参数化的观测器增益
    const float beta1 = 3.0f * leso->wc;
    const float beta2 = 3.0f * leso->wc * leso->wc;
    const float beta3 = leso->wc * leso->wc * leso->wc;
    
    // 状态更新
    leso->z1 += dt * (leso->z2 - beta1 * e);
    leso->z2 += dt * (leso->z3 - beta2 * e + b0 * u);
    leso->z3 += dt * (-beta3 * e);
}

/**
 * @brief 线性状态误差反馈控制律计算
 * @param lsef  LSEF 结构体指针
 * @param e1    位置误差
 * @param e2    速度误差
 * @return 控制量
 */
static float LSEF_Calculate(const LSEF_Controller *lsef, const float e1, const float e2) {
    const float u0 = lsef->kp * e1 + lsef->kd * e2;
    return u0;
}
