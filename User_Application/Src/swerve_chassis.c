/* 包含头文件 ----------------------------------------------------------------*/
#include "../../User_Application/swerve_chassis.h"
#include "../../User_Algorithm/user_coord.h"

/* 私有宏定义 ----------------------------------------------------------------*/
#define RPM_TO_RAD(RPM) ((RPM) * TWO_PI / 60.0f)
#define RAD_TO_RPM(RAD) ((RAD) * 60.0f / TWO_PI)

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 初始化舵轮底盘
* @param chassis 底盘状态结构体指针
* @param wheelbase_radius 轮子到底盘中心的距离 (m)
* @param wheel_radius 轮子的半径 (m)
* @param ratio 减速比
* @param fl_wheel 左前轮
* @param fr_wheel 右前轮
* @param rl_wheel 左后轮
* @param rr_wheel 右后轮
* @param fl_steer 前左轮转向电机
* @param fr_steer 前右轮转向电机
* @param rl_steer 前左轮转向电机
* @param rr_steer 后右轮转向电机
* @param fl_steer_offset 左前轮转向角度零点偏移 (deg)
* @param fr_steer_offset 右前轮转向角度零点偏移 (deg)
* @param rl_steer_offset 左前轮转向角度零点偏移 (deg)
* @param rr_steer_offset 左后轮转向角度零点偏移 (deg)
*/
void SwerveChassis_Init(SwerveChassisState* chassis, const float wheelbase_radius, const float wheel_radius, const float ratio,
    DJI_MOTOR_DRIVES* fl_wheel,  DJI_MOTOR_DRIVES* fr_wheel,  DJI_MOTOR_DRIVES* rl_wheel,  DJI_MOTOR_DRIVES* rr_wheel,
    DJI_MOTOR_DRIVES* fl_steer,  DJI_MOTOR_DRIVES* fr_steer,  DJI_MOTOR_DRIVES* rl_steer,  DJI_MOTOR_DRIVES* rr_steer,
    const float fl_steer_offset, const float fr_steer_offset, const float rl_steer_offset, const float rr_steer_offset,
    const int8_t fl_reverse,     const int8_t fr_reverse,     const int8_t rl_reverse,     const int8_t rr_reverse) {

    chassis->vx_target = 0.0f;
    chassis->vy_target = 0.0f;
    chassis->omega_target = 0.0f;
    chassis->wheelbase_radius = wheelbase_radius;
    chassis->wheel_radius = wheel_radius;
    chassis->ratio = ratio;

    chassis->wheel_fl.wheel_motor = fl_wheel;
    chassis->wheel_fr.wheel_motor = fr_wheel;
    chassis->wheel_rl.wheel_motor = rl_wheel;
    chassis->wheel_rr.wheel_motor = rr_wheel;

    chassis->wheel_fl.steer_motor = fl_steer;
    chassis->wheel_fr.steer_motor = fr_steer;
    chassis->wheel_rl.steer_motor = rl_steer;
    chassis->wheel_rr.steer_motor = rr_steer;

    chassis->wheel_fl.steer_angle_offset = fl_steer_offset;
    chassis->wheel_fr.steer_angle_offset = fr_steer_offset;
    chassis->wheel_rl.steer_angle_offset = rl_steer_offset;
    chassis->wheel_rr.steer_angle_offset = rr_steer_offset;

    chassis->wheel_fl.reverse = fl_reverse;
    chassis->wheel_fr.reverse = fr_reverse;
    chassis->wheel_rl.reverse = rl_reverse;
    chassis->wheel_rr.reverse = rr_reverse;
}


/**
* @brief 舵轮底盘运动学正解
* @note 根据底盘速度指令计算各轮子的速度和转向角度
* @param chassis 底盘状态结构体指针
* @param vx 速度 (m/s)
* @param vy 速度 (m/s)
* @param omega 角速度 (rad/s)
*/
void SwerveChassis_Kinematics(SwerveChassisState* chassis, const float vx, const float vy, const float omega) {
    const float r = chassis->wheelbase_radius;
    chassis->vx_target = vx;
    chassis->vy_target = vy;
    chassis->omega_target = omega;
    
    // 轮子数组
    SwerveWheel* wheels[4] = {
        &chassis->wheel_fl,
        &chassis->wheel_fr,
        &chassis->wheel_rl,
        &chassis->wheel_rr
    };
    
    // 各轮子在底盘坐标系中的位置
    const CartesianCoord_Point positions[4] = {
        {-r * 0.707107f, r  * 0.707107f, 0.0f},
        {-r * 0.707107f, -r * 0.707107f, 0.0f},
        {-r * 0.707107f, -r * 0.707107f, 0.0f},
        {r  * 0.707107f, -r * 0.707107f, 0.0f}
    };
    
    // 平移速度向量
    const CartesianCoord_Point v_trans = {vx, vy, 0.0f};
    // 角速度向量
    const CartesianCoord_Point omega_vec = {0.0f, 0.0f, omega};
    
    // 对每个轮子进行运动学计算
    for (int i = 0; i < 4; i++) {
        // 计算由于旋转产生的速度分量
        CartesianCoord_Point v_rot;
        CrossProduct_Cartesian(&omega_vec, &positions[i], &v_rot);
        
        // 计算轮子的总速度
        CartesianCoord_Point v_wheel;
        Add_Cartesian(&v_trans, &v_rot, &v_wheel);
        
        // 将速度向量转换为极坐标，获取速度大小和方向角
        PolarCoord_Point polar;
        CartesianToPolar(&v_wheel, &polar);
        
        // 设置轮子的目标速度和转向角度
        wheels[i]->drive_speed_target = polar.radius;
        wheels[i]->steer_angle_target = polar.yaw;
    }
}

/**
* @brief 劣弧优化 - 选择最短转向路径
* @note 如果转向角度差大于90度，反转驱动方向并调整转向角度
* @param wheel 舵轮状态结构体指针
*/
static void OptimizeSteerAngle(SwerveWheel* wheel) {
    // 计算当前角度与目标角度的差值
    const float angle_diff = Math_WrapAngleDeg(wheel->steer_angle_target - wheel->steer_angle_current);

    // 如果角度差的绝对值大于90度，使用劣弧优化
    if (fabsf(angle_diff) > 90.0f) {
        wheel->drive_speed_target = -wheel->drive_speed_target;
        wheel->steer_angle_target = Math_WrapAngleDeg(wheel->steer_angle_target + 180.0f);
    }
}


/**
* @brief 设置底盘各电机目标值
* @note 应用劣弧优化、零点偏移和单位换算，设置电机目标值
* @param chassis 底盘状态结构体指针
*/
void SwerveChassis_Set_Motor_Target(SwerveChassisState* chassis) {
    SwerveWheel* wheels[4] = {
        &chassis->wheel_fl,
        &chassis->wheel_fr,
        &chassis->wheel_rl,
        &chassis->wheel_rr
    };
    
    // 对每个轮子进行处理
    for (int i = 0; i < 4; i++) {
        SwerveWheel* wheel = wheels[i];
        
        // 读取当前转向角度（从电机反馈获取，需要减去零点偏移）
        wheel->steer_angle_current = (float)wheel->steer_motor->rotor_angle * 360.0f / 8192.0f - wheel->steer_angle_offset;
        wheel->steer_angle_current = Math_WrapAngleDeg(wheel->steer_angle_current);

        // 计算加零点偏移后转向电机的目标角度
        wheel->steer_angle_target = wheel->steer_angle_target + wheel->steer_angle_offset;

        // 劣弧优化
        OptimizeSteerAngle(wheel);

        // 设置转向电机目标
        // 角度控制模式：将角度转换为编码器值 [0, 8192)
        const float target_encoder = (wheel->steer_angle_target + 180.0f) * 8192.0f / 360.0f;
        DJI_Motor_Set_Target(wheel->steer_motor, target_encoder);

        // 计算驱动轮的目标转速
        const float wheel_angular_velocity = wheel->drive_speed_target / chassis->wheel_radius;
        const float motor_rpm = RAD_TO_RPM(wheel_angular_velocity);
        
        // 读取当前驱动速度
        wheel->drive_speed_current = RPM_TO_RAD(wheel->wheel_motor->rotor_speed) * chassis->wheel_radius / chassis->ratio;
        
        // 设置驱动电机目标
        DJI_Motor_Set_Target(wheel->wheel_motor, motor_rpm * (float)wheel->reverse * chassis->ratio);
    }
}


/**
* @brief 舵轮底盘逆向运动学解算
* @note 根据各轮子的实际速度和转向角度，计算底盘的实际运动状态
* @param chassis 底盘状态结构体指针
*/
void SwerveChassis_InverseKinematics(SwerveChassisState* chassis) {
    const float r = chassis->wheelbase_radius;
    
    // 获取各轮子的实际速度向量
    CartesianCoord_Point v_fl, v_fr, v_rl, v_rr;
    
    // 将极坐标速度转换为笛卡尔坐标
    const PolarCoord_Point polar_fl = {chassis->wheel_fl.drive_speed_current * (float)chassis->wheel_fl.reverse, chassis->wheel_fl.steer_angle_current, 0.0f};
    const PolarCoord_Point polar_fr = {chassis->wheel_fr.drive_speed_current * (float)chassis->wheel_fr.reverse, chassis->wheel_fr.steer_angle_current, 0.0f};
    const PolarCoord_Point polar_rl = {chassis->wheel_rl.drive_speed_current * (float)chassis->wheel_rl.reverse, chassis->wheel_rl.steer_angle_current, 0.0f};
    const PolarCoord_Point polar_rr = {chassis->wheel_rr.drive_speed_current * (float)chassis->wheel_rr.reverse, chassis->wheel_rr.steer_angle_current, 0.0f};
    
    PolarToCartesian(&polar_fl, &v_fl);
    PolarToCartesian(&polar_fr, &v_fr);
    PolarToCartesian(&polar_rl, &v_rl);
    PolarToCartesian(&polar_rr, &v_rr);
    
    // 计算各轮子在底盘坐标系中的位置
    const CartesianCoord_Point pos_fl = {-r * 0.707107f, r  * 0.707107f, 0.0f};
    const CartesianCoord_Point pos_fr = {-r * 0.707107f, -r * 0.707107f, 0.0f};
    const CartesianCoord_Point pos_rl = {-r * 0.707107f, -r * 0.707107f, 0.0f};
    const CartesianCoord_Point pos_rr = {r  * 0.707107f, -r * 0.707107f, 0.0f};

    // 计算平移速度
    CartesianCoord_Point v_sum = {0.0f, 0.0f, 0.0f};
    Add_Cartesian(&v_sum, &v_fl, &v_sum);
    Add_Cartesian(&v_sum, &v_fr, &v_sum);
    Add_Cartesian(&v_sum, &v_rl, &v_sum);
    Add_Cartesian(&v_sum, &v_rr, &v_sum);
    
    chassis->vx_current = v_sum.x / 4.0f;
    chassis->vy_current = v_sum.y / 4.0f;
    
    // 计算旋转角速度
    const CartesianCoord_Point v_trans = {chassis->vx_current, chassis->vy_current, 0.0f};
    
    // 计算每个轮子的旋转分量
    CartesianCoord_Point v_rot_fl, v_rot_fr, v_rot_rl, v_rot_rr;
    Subtract_Cartesian(&v_fl, &v_trans, &v_rot_fl);
    Subtract_Cartesian(&v_fr, &v_trans, &v_rot_fr);
    Subtract_Cartesian(&v_rl, &v_trans, &v_rot_rl);
    Subtract_Cartesian(&v_rr, &v_trans, &v_rot_rr);
    
    // 计算角速度
    const float omega_fl = (v_rot_fl.x * (-pos_fl.y) + v_rot_fl.y * pos_fl.x) / (r * r);
    const float omega_fr = (v_rot_fr.x * (-pos_fr.y) + v_rot_fr.y * pos_fr.x) / (r * r);
    const float omega_rl = (v_rot_rl.x * (-pos_rl.y) + v_rot_rl.y * pos_rl.x) / (r * r);
    const float omega_rr = (v_rot_rr.x * (-pos_rr.y) + v_rot_rr.y * pos_rr.x) / (r * r);
    
    // 取平均值
    chassis->omega_current = (omega_fl + omega_fr + omega_rl + omega_rr) / 4.0f;
}
