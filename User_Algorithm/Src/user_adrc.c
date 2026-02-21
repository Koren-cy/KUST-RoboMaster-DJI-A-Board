/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_adrc.h"
#include <math.h>

/* 私有函数声明 --------------------------------------------------------------*/
static void TD_Calculate(TD_Controller *td, float target, float dt);
static void ESO_Calculate(ESO_Controller *eso, float feedback, float u, float b0, float dt);
static float NLSEF_Calculate(NLSEF_Controller *nlsef, float e1, float e2);
static float fhan(float x1, float x2, float r, float h);
static float fal(float e, float alpha, float delta);

/* 函数体 --------------------------------------------------------------------*/

/**
 * @brief 初始化 ADRC 控制器
 * @param adrc          ADRC 控制器结构体指针
 * @param r             跟踪微分器快速跟踪因子
 * @param h             跟踪微分器滤波因子
 * @param beta1_eso     ESO 观测器增益1
 * @param beta2_eso     ESO 观测器增益2
 * @param beta3_eso     ESO 观测器增益3
 * @param delta_eso     ESO 线性段宽度
 * @param beta1_nlsef   NLSEF 非线性增益1
 * @param beta2_nlsef   NLSEF 非线性增益2
 * @param alpha1        NLSEF 非线性因子1
 * @param alpha2        NLSEF 非线性因子2
 * @param delta_nlsef   NLSEF 线性段宽度
 * @param b0            控制增益补偿
 * @param max_out       输出限幅
 * @param dt            采样时间
 */
void ADRC_Init(ADRC_Controller *adrc,
               const float r, const float h,
               const float beta1_eso, const float beta2_eso, const float beta3_eso, const float delta_eso,
               const float beta1_nlsef, const float beta2_nlsef, const float alpha1, const float alpha2, const float delta_nlsef,
               const float b0, const float max_out, const float dt) {
    // 初始化跟踪微分器
    adrc->td.r = r;
    adrc->td.h = h;
    adrc->td.x1 = 0.0f;
    adrc->td.x2 = 0.0f;
    
    // 初始化扩张状态观测器
    adrc->eso.beta1 = beta1_eso;
    adrc->eso.beta2 = beta2_eso;
    adrc->eso.beta3 = beta3_eso;
    adrc->eso.delta = delta_eso;
    adrc->eso.z1 = 0.0f;
    adrc->eso.z2 = 0.0f;
    adrc->eso.z3 = 0.0f;
    
    // 初始化非线性状态误差反馈控制律
    adrc->nlsef.beta1 = beta1_nlsef;
    adrc->nlsef.beta2 = beta2_nlsef;
    adrc->nlsef.alpha1 = alpha1;
    adrc->nlsef.alpha2 = alpha2;
    adrc->nlsef.delta = delta_nlsef;
    
    // 初始化控制器参数
    adrc->b0 = b0;
    adrc->max_out = max_out;
    adrc->dt = dt;
    
    adrc->set = 0.0f;
    adrc->fdb = 0.0f;
    adrc->out = 0.0f;
    adrc->u0 = 0.0f;
}

/**
 * @brief 设定 ADRC 目标值
 * @param adrc      ADRC 控制器结构体指针
 * @param target    目标值
 */
void ADRC_SetTarget(ADRC_Controller *adrc, const float target) {
    adrc->set = target;
}

/**
 * @brief 计算 ADRC 控制输出
 * @param adrc      ADRC 控制器结构体指针
 * @param feedback  反馈值
 * @return ADRC 控制输出值
 */
float ADRC_Calculate(ADRC_Controller *adrc, const float feedback) {
    adrc->fdb = feedback;
    
    // 跟踪微分器
    TD_Calculate(&adrc->td, adrc->set, adrc->dt);
    
    // 扩张状态观测器
    ESO_Calculate(&adrc->eso, feedback, adrc->out, adrc->b0, adrc->dt);
    
    // 非线性状态误差反馈控制律
    float e1 = adrc->td.x1 - adrc->eso.z1;
    float e2 = adrc->td.x2 - adrc->eso.z2;
    adrc->u0 = NLSEF_Calculate(&adrc->nlsef, e1, e2);
    
    // 扰动补偿
    adrc->out = (adrc->u0 - adrc->eso.z3) / adrc->b0;
    
    // 输出限幅
    if (adrc->out > adrc->max_out) {
        adrc->out = adrc->max_out;
    } else if (adrc->out < -adrc->max_out) {
        adrc->out = -adrc->max_out;
    }
    
    return adrc->out;
}

/**
 * @brief 跟踪微分器计算
 * @param td      跟踪微分器结构体指针
 * @param target  目标值
 * @param dt      采样时间
 */
static void TD_Calculate(TD_Controller *td, const float target, const float dt) {
    const float fh = fhan(td->x1 - target, td->x2, td->r, td->h);
    
    td->x1 += dt * td->x2;
    td->x2 += dt * fh;
}

/**
 * @brief 扩张状态观测器计算
 * @param eso       扩张状态观测器结构体指针
 * @param feedback  反馈值
 * @param u         控制输入
 * @param b0        控制增益
 * @param dt        采样时间
 */
static void ESO_Calculate(ESO_Controller *eso, const float feedback, const float u, const float b0, const float dt) {
    const float e = eso->z1 - feedback;

    const float fe = fal(e, 0.5f, eso->delta);
    const float fe1 = fal(e, 0.25f, eso->delta);
    
    eso->z1 += dt * (eso->z2 - eso->beta1 * e);
    eso->z2 += dt * (eso->z3 - eso->beta2 * fe + b0 * u);
    eso->z3 += dt * (-eso->beta3 * fe1);
}

/**
 * @brief 非线性状态误差反馈控制律计算
 * @param nlsef  NLSEF 结构体指针
 * @param e1     误差1
 * @param e2     误差2
 * @return 控制量
 */
static float NLSEF_Calculate(NLSEF_Controller *nlsef, const float e1, const float e2) {
    const float u0 = nlsef->beta1 * fal(e1, nlsef->alpha1, nlsef->delta) +
               nlsef->beta2 * fal(e2, nlsef->alpha2, nlsef->delta);
    return u0;
}

/**
 * @brief 最速控制综合函数
 * @param x1  跟踪误差
 * @param x2  跟踪误差微分
 * @param r   快速跟踪因子
 * @param h   滤波因子
 * @return 控制量
 */
static float fhan(const float x1, const float x2, const float r, const float h) {
    const float d = r * h * h;
    const float a0 = h * x2;
    const float y = x1 + a0;
    const float a1 = sqrtf(d * (d + 8.0f * fabsf(y)));
    const float a2 = a0 + (y >= 0 ? 1.0f : -1.0f) * (a1 - d) / 2.0f;
    const float sy = (fabsf(y + d) >= fabsf(y - d)) ? 1.0f : -1.0f;
    const float a = (fabsf(a2) > d) ? sy : a2 / d;
    const float fhan_value = -r * (a0 / h - a * (fabsf(a) <= d ? 1.0f : sy));
    
    return fhan_value;
}

/**
 * @brief 非线性函数
 * @param e      误差
 * @param alpha  非线性因子
 * @param delta  线性段宽度
 * @return 函数值
 */
static float fal(const float e, const float alpha, const float delta) {
    const float abs_e = fabsf(e);
    
    if (abs_e <= delta) {
        return e / powf(delta, 1.0f - alpha);
    } else {
        return powf(abs_e, alpha) * (e >= 0 ? 1.0f : -1.0f);
    }
}
