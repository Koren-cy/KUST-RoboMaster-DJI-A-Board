/* 包含头文件 ----------------------------------------------------------------*/
#include "../Inc/user_coord.h"
#include <math.h>

/* 函数体 --------------------------------------------------------------------*/
/* 笛卡尔坐标运算函数 ----------------------------------------------------------*/
/**
 * @brief  计算两点间欧氏距离
 * @param  p1: 点1
 * @param  p2: 点2
 * @retval 欧氏距离
 */
float Distance_Cartesian(const CartesianCoord_Point* p1, const CartesianCoord_Point* p2) {
    const float dx = p2->x - p1->x;
    const float dy = p2->y - p1->y;
    const float dz = p2->z - p1->z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

/**
 * @brief  计算向量模长
 * @param  point: 向量
 * @retval 模长
 */
float Magnitude_Cartesian(const CartesianCoord_Point* point) {
    const float x2 = point->x * point->x;
    const float y2 = point->y * point->y;
    const float z2 = point->z * point->z;
    return sqrtf(x2 + y2 + z2);
}

/**
 * @brief  向量归一化
 * @param  point: 输入向量
 * @param  result: 单位向量
 */
void Normalize_Cartesian(const CartesianCoord_Point* point, CartesianCoord_Point* result) {
    const float mag_sq = point->x * point->x + point->y * point->y + point->z * point->z;

    if (mag_sq < PRECISION_SQ) {
        result->x = 0.0f;
        result->y = 0.0f;
        result->z = 0.0f;
    } else {
        const float inv_mag = FastInvSqrt(mag_sq);
        result->x = point->x * inv_mag;
        result->y = point->y * inv_mag;
        result->z = point->z * inv_mag;
    }
}

/**
 * @brief  计算两向量夹角
 * @param  p1: 向量1
 * @param  p2: 向量2
 * @retval 夹角(度), 范围[0°, 180°]
 */
float AngleDifference_Cartesian(const CartesianCoord_Point* p1, const CartesianCoord_Point* p2) {
    const float mag1_sq = p1->x * p1->x + p1->y * p1->y + p1->z * p1->z;
    const float mag2_sq = p2->x * p2->x + p2->y * p2->y + p2->z * p2->z;
    
    if (mag1_sq < PRECISION_SQ || mag2_sq < PRECISION_SQ) {
        return 0.0f;
    }
    
    const float dot = DotProduct_Cartesian(p1, p2);
    const float inv_mag_product = FastInvSqrt(mag1_sq * mag2_sq);
    float cos_angle = dot * inv_mag_product;
    
    // 限制在[-1, 1]范围内
    if (cos_angle > 1.0f) cos_angle = 1.0f;
    if (cos_angle < -1.0f) cos_angle = -1.0f;
    
    return Math_Rad2Deg(acosf(cos_angle));
}

/**
 * @brief  向量叉积
 * @param  p1: 向量1
 * @param  p2: 向量2
 * @param  result: 结果向量(p1 × p2)
 */
void CrossProduct_Cartesian(const CartesianCoord_Point* p1, const CartesianCoord_Point* p2, CartesianCoord_Point* result) {
    result->x = p1->y * p2->z - p1->z * p2->y;
    result->y = p1->z * p2->x - p1->x * p2->z;
    result->z = p1->x * p2->y - p1->y * p2->x;
}

/**
 * @brief  向量投影
 * @param  point: 待投影向量
 * @param  onto: 目标向量
 * @param  result: 投影结果
 */
void Project_Cartesian(const CartesianCoord_Point* point, const CartesianCoord_Point* onto, CartesianCoord_Point* result) {
    const float dot = DotProduct_Cartesian(point, onto);
    const float onto_mag_sq = DotProduct_Cartesian(onto, onto);
    
    if (onto_mag_sq < PRECISION) {
        result->x = 0.0f;
        result->y = 0.0f;
        result->z = 0.0f;
    } else {
        const float scale = dot / onto_mag_sq;
        Scale_Cartesian(onto, scale, result);
    }
}

/**
 * @brief  笛卡尔坐标线性插值
 * @param  p1: 起始点
 * @param  p2: 目标点
 * @param  t: 插值参数[0, 1]
 * @param  result: 插值结果
 */
void Lerp_Cartesian(const CartesianCoord_Point* p1, const CartesianCoord_Point* p2, float t, CartesianCoord_Point* result) {
    result->x = p1->x + t * (p2->x - p1->x);
    result->y = p1->y + t * (p2->y - p1->y);
    result->z = p1->z + t * (p2->z - p1->z);
}

/* 极坐标运算函数 ------------------------------------------------------------*/

/**
 * @brief  极坐标归一化
 * @param  point: 输入极坐标
 * @param  result: 归一化极坐标
 */
void Normalize_Polar(const PolarCoord_Point* point, PolarCoord_Point* result) {
    result->radius = fabsf(point->radius);
    result->yaw = Math_WrapAngleDeg(point->yaw);
    result->pitch = Math_WrapAngleDeg(point->pitch);
}

/**
 * @brief  极坐标加法
 * @param  p1: 极坐标1
 * @param  p2: 极坐标2
 * @param  result: 结果极坐标
 */
void Add_Polar(const PolarCoord_Point* p1, const PolarCoord_Point* p2, PolarCoord_Point* result) {
    CartesianCoord_Point c1, c2, c_result;
    
    PolarToCartesian(p1, &c1);
    PolarToCartesian(p2, &c2);
    Add_Cartesian(&c1, &c2, &c_result);
    CartesianToPolar(&c_result, result);
}

/**
 * @brief  极坐标减法
 * @param  p1: 极坐标1
 * @param  p2: 极坐标2
 * @param  result: 结果极坐标
 */
void Subtract_Polar(const PolarCoord_Point* p1, const PolarCoord_Point* p2, PolarCoord_Point* result) {
    CartesianCoord_Point c1, c2, c_result;
    
    PolarToCartesian(p1, &c1);
    PolarToCartesian(p2, &c2);
    Subtract_Cartesian(&c1, &c2, &c_result);
    CartesianToPolar(&c_result, result);
}

/**
 * @brief  极坐标半径缩放
 * @param  point: 输入极坐标
 * @param  scale: 缩放系数
 * @param  result: 结果极坐标
 */
void Scale_Polar(const PolarCoord_Point* point, float scale, PolarCoord_Point* result) {
    result->radius = point->radius * scale;
    result->yaw = point->yaw;
    result->pitch = point->pitch;
}

/**
 * @brief  计算两极坐标点间距离
 * @param  p1: 极坐标1
 * @param  p2: 极坐标2
 * @retval 欧氏距离
 */
float Distance_Polar(const PolarCoord_Point* p1, const PolarCoord_Point* p2) {
    CartesianCoord_Point c1, c2;
    
    PolarToCartesian(p1, &c1);
    PolarToCartesian(p2, &c2);
    
    return Distance_Cartesian(&c1, &c2);
}

/**
 * @brief  计算综合角度差
 * @param  p1: 极坐标1
 * @param  p2: 极坐标2
 * @retval 综合角度差(度)
 */
float AbsAngleDifference_Polar(const PolarCoord_Point* p1, const PolarCoord_Point* p2) {
    const float yaw_diff = Math_WrapAngleDeg(p2->yaw - p1->yaw);
    const float pitch_diff = Math_WrapAngleDeg(p2->pitch - p1->pitch);
    
    return sqrtf(yaw_diff * yaw_diff + pitch_diff * pitch_diff);
}

/**
 * @brief  计算偏航角差
 * @param  p1: 极坐标1
 * @param  p2: 极坐标2
 * @retval 偏航角差(度), 范围[-180°, 180°]
 */
float YawDifference_Polar(const PolarCoord_Point* p1, const PolarCoord_Point* p2) {
    return Math_WrapAngleDeg(p2->yaw - p1->yaw);
}

/**
 * @brief  计算俯仰角差
 * @param  p1: 极坐标1
 * @param  p2: 极坐标2
 * @retval 俯仰角差(度), 范围[-180°, 180°]
 */
float PitchDifference_Polar(const PolarCoord_Point* p1, const PolarCoord_Point* p2) {
    return Math_WrapAngleDeg(p2->pitch - p1->pitch);
}

/**
 * @brief  极坐标线性插值
 * @param  p1: 起始点
 * @param  p2: 目标点
 * @param  t: 插值参数[0, 1]
 * @param  result: 插值结果
 */
void Lerp_Polar(const PolarCoord_Point* p1, const PolarCoord_Point* p2, float t, PolarCoord_Point* result) {
    result->radius = p1->radius + t * (p2->radius - p1->radius);
    
    // 角度插值考虑最短路径
    const float yaw_diff = Math_WrapAngleDeg(p2->yaw - p1->yaw);
    const float pitch_diff = Math_WrapAngleDeg(p2->pitch - p1->pitch);
    
    result->yaw = Math_WrapAngleDeg(p1->yaw + t * yaw_diff);
    result->pitch = Math_WrapAngleDeg(p1->pitch + t * pitch_diff);
}

/* 坐标系转换函数 ------------------------------------------------------------*/

/**
 * @brief  笛卡尔坐标转极坐标
 * @param  cartesian_point: 笛卡尔坐标
 * @param  result: 极坐标(角度单位:度)
 */
void CartesianToPolar(const CartesianCoord_Point* cartesian_point, PolarCoord_Point* result) {
    const float x = cartesian_point->x;
    const float y = cartesian_point->y;
    const float z = cartesian_point->z;
    
    const float x_sq = x * x;
    const float y_sq = y * y;
    
    result->radius = sqrtf(x_sq + y_sq + z * z);
    result->yaw = Math_Rad2Deg(atan2f(y, x));
    
    const float horizontal_dist = sqrtf(x_sq + y_sq);
    result->pitch = Math_Rad2Deg(atan2f(z, horizontal_dist));
}

/**
 * @brief  极坐标转笛卡尔坐标
 * @param  polar_point: 极坐标(角度单位:度)
 * @param  result: 笛卡尔坐标
 */
void PolarToCartesian(const PolarCoord_Point* polar_point, CartesianCoord_Point* result) {
    const float yaw_rad = Math_Deg2Rad(polar_point->yaw);
    const float pitch_rad = Math_Deg2Rad(polar_point->pitch);
    const float cos_pitch = cosf(pitch_rad);
    
    result->x = polar_point->radius * cos_pitch * cosf(yaw_rad);
    result->y = polar_point->radius * cos_pitch * sinf(yaw_rad);
    result->z = polar_point->radius * sinf(pitch_rad);
}

/* 坐标旋转函数 --------------------------------------------------------------*/

/**
 * @brief  绕X轴旋转
 * @param  point: 输入坐标
 * @param  angle_deg: 旋转角度(度)
 * @param  result: 旋转后坐标
 */
void RotateX_Cartesian(const CartesianCoord_Point* point, float angle_deg, CartesianCoord_Point* result) {
    const float angle_rad = Math_Deg2Rad(angle_deg);
    const float cos_a = cosf(angle_rad);
    const float sin_a = sinf(angle_rad);

    const float y = point->y;
    const float z = point->z;

    result->x = point->x;
    result->y = y * cos_a - z * sin_a;
    result->z = y * sin_a + z * cos_a;
}

/**
 * @brief  绕Y轴旋转
 * @param  point: 输入坐标
 * @param  angle_deg: 旋转角度(度)
 * @param  result: 旋转后坐标
 */
void RotateY_Cartesian(const CartesianCoord_Point* point, float angle_deg, CartesianCoord_Point* result) {
    const float angle_rad = Math_Deg2Rad(angle_deg);
    const float cos_a = cosf(angle_rad);
    const float sin_a = sinf(angle_rad);
    
    const float x = point->x;
    const float z = point->z;
    
    result->x = x * cos_a + z * sin_a;
    result->y = point->y;
    result->z = -x * sin_a + z * cos_a;
}

/**
 * @brief  绕Z轴旋转
 * @param  point: 输入坐标
 * @param  angle_deg: 旋转角度(度)
 * @param  result: 旋转后坐标
 */
void RotateZ_Cartesian(const CartesianCoord_Point* point, float angle_deg, CartesianCoord_Point* result) {
    const float angle_rad = Math_Deg2Rad(angle_deg);
    const float cos_a = cosf(angle_rad);
    const float sin_a = sinf(angle_rad);
    
    const float x = point->x;
    const float y = point->y;
    
    result->x = x * cos_a - y * sin_a;
    result->y = x * sin_a + y * cos_a;
    result->z = point->z;
}

/**
 * @brief  使用旋转矩阵旋转坐标
 * @param  point: 输入坐标
 * @param  matrix: 旋转矩阵
 * @param  result: 旋转后坐标
 */
void RotateByMatrix_Cartesian(const CartesianCoord_Point* point, const RotationMatrix* matrix, 
                              CartesianCoord_Point* result) {
    result->x = matrix->m[0][0] * point->x + matrix->m[0][1] * point->y + matrix->m[0][2] * point->z;
    result->y = matrix->m[1][0] * point->x + matrix->m[1][1] * point->y + matrix->m[1][2] * point->z;
    result->z = matrix->m[2][0] * point->x + matrix->m[2][1] * point->y + matrix->m[2][2] * point->z;
}

/**
 * @brief  使用四元数旋转坐标
 * @param  point: 输入坐标
 * @param  quat: 四元数
 * @param  result: 旋转后坐标
 */
void RotateByQuaternion_Cartesian(const CartesianCoord_Point* point, const Quaternion* quat, 
                                  CartesianCoord_Point* result) {
    const float qx = quat->x;
    const float qy = quat->y;
    const float qz = quat->z;
    const float qw = quat->w;
    
    const float px = point->x;
    const float py = point->y;
    const float pz = point->z;
    
    const float tx = 2.0f * (qy * pz - qz * py);
    const float ty = 2.0f * (qz * px - qx * pz);
    const float tz = 2.0f * (qx * py - qy * px);
    
    result->x = px + qw * tx + (qy * tz - qz * ty);
    result->y = py + qw * ty + (qz * tx - qx * tz);
    result->z = pz + qw * tz + (qx * ty - qy * tx);
}

/* 四元数运算函数 ------------------------------------------------------------*/

/**
 * @brief  初始化为单位四元数
 * @param  quat: 四元数
 */
void Quaternion_Identity(Quaternion* quat) {
    quat->w = 1.0f;
    quat->x = 0.0f;
    quat->y = 0.0f;
    quat->z = 0.0f;
}

/**
 * @brief  从轴角创建四元数
 * @param  axis: 旋转轴
 * @param  angle_deg: 旋转角度(度)
 * @param  quat: 结果四元数
 */
void Quaternion_FromAxisAngle(const CartesianCoord_Point* axis, float angle_deg, Quaternion* quat) {
    const float angle_rad = Math_Deg2Rad(angle_deg);
    const float half_angle = angle_rad * 0.5f;
    const float sin_half = sinf(half_angle);
    
    CartesianCoord_Point normalized_axis;
    Normalize_Cartesian(axis, &normalized_axis);
    
    quat->w = cosf(half_angle);
    quat->x = normalized_axis.x * sin_half;
    quat->y = normalized_axis.y * sin_half;
    quat->z = normalized_axis.z * sin_half;
}

/**
 * @brief  从欧拉角创建四元数
 * @param  euler: 欧拉角(度)
 * @param  quat: 结果四元数
 */
void Quaternion_FromEuler(const EulerAngles* euler, Quaternion* quat) {
    const float roll_rad = Math_Deg2Rad(euler->roll) * 0.5f;
    const float pitch_rad = Math_Deg2Rad(euler->pitch) * 0.5f;
    const float yaw_rad = Math_Deg2Rad(euler->yaw) * 0.5f;
    
    const float cr = cosf(roll_rad);
    const float sr = sinf(roll_rad);
    const float cp = cosf(pitch_rad);
    const float sp = sinf(pitch_rad);
    const float cy = cosf(yaw_rad);
    const float sy = sinf(yaw_rad);
    
    quat->w = cr * cp * cy + sr * sp * sy;
    quat->x = sr * cp * cy - cr * sp * sy;
    quat->y = cr * sp * cy + sr * cp * sy;
    quat->z = cr * cp * sy - sr * sp * cy;
}

/**
 * @brief  四元数转欧拉角
 * @param  quat: 四元数
 * @param  euler: 结果欧拉角(度)
 */
void Quaternion_ToEuler(const Quaternion* quat, EulerAngles* euler) {
    const float sinr_cosp = 2.0f * (quat->w * quat->x + quat->y * quat->z);
    const float cosr_cosp = 1.0f - 2.0f * (quat->x * quat->x + quat->y * quat->y);
    euler->roll = Math_Rad2Deg(atan2f(sinr_cosp, cosr_cosp));
    
    const float sinp = 2.0f * (quat->w * quat->y - quat->z * quat->x);
    if (fabsf(sinp) >= 1.0f) {
        euler->pitch = copysignf(90.0f, sinp);
    } else {
        euler->pitch = Math_Rad2Deg(asinf(sinp));
    }
    
    const float siny_cosp = 2.0f * (quat->w * quat->z + quat->x * quat->y);
    const float cosy_cosp = 1.0f - 2.0f * (quat->y * quat->y + quat->z * quat->z);
    euler->yaw = Math_Rad2Deg(atan2f(siny_cosp, cosy_cosp));
}

/**
 * @brief  四元数乘法
 * @param  q1: 四元数1
 * @param  q2: 四元数2
 * @param  result: 结果四元数(q1 * q2)
 */
void Quaternion_Multiply(const Quaternion* q1, const Quaternion* q2, Quaternion* result) {
    result->w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z;
    result->x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y;
    result->y = q1->w * q2->y - q1->x * q2->z + q1->y * q2->w + q1->z * q2->x;
    result->z = q1->w * q2->z + q1->x * q2->y - q1->y * q2->x + q1->z * q2->w;
}

/**
 * @brief  四元数共轭
 * @param  quat: 输入四元数
 * @param  result: 结果四元数
 */
void Quaternion_Conjugate(const Quaternion* quat, Quaternion* result) {
    result->w = quat->w;
    result->x = -quat->x;
    result->y = -quat->y;
    result->z = -quat->z;
}

/**
 * @brief  计算四元数模长
 * @param  quat: 四元数
 * @retval 模长
 */
float Quaternion_Magnitude(const Quaternion* quat) {
    return sqrtf(quat->w * quat->w + quat->x * quat->x + quat->y * quat->y + quat->z * quat->z);
}

/**
 * @brief  四元数归一化
 * @param  quat: 输入四元数
 * @param  result: 单位四元数
 */
void Quaternion_Normalize(const Quaternion* quat, Quaternion* result) {
    const float mag = Quaternion_Magnitude(quat);
    if (mag < PRECISION) {
        Quaternion_Identity(result);
    } else {
        const float inv_mag = 1.0f / mag;
        result->w = quat->w * inv_mag;
        result->x = quat->x * inv_mag;
        result->y = quat->y * inv_mag;
        result->z = quat->z * inv_mag;
    }
}

/**
 * @brief  四元数球面线性插值
 * @param  q1: 起始四元数
 * @param  q2: 目标四元数
 * @param  t: 插值参数[0, 1]
 * @param  result: 结果四元数
 */
void Quaternion_Slerp(const Quaternion* q1, const Quaternion* q2, float t, Quaternion* result) {
    if (t <= 0.0f) {
        *result = *q1;
        return;
    }
    if (t >= 1.0f) {
        *result = *q2;
        return;
    }
    
    float dot = q1->w * q2->w + q1->x * q2->x + q1->y * q2->y + q1->z * q2->z;
    
    Quaternion q2_copy = *q2;
    if (dot < 0.0f) {
        q2_copy.w = -q2_copy.w;
        q2_copy.x = -q2_copy.x;
        q2_copy.y = -q2_copy.y;
        q2_copy.z = -q2_copy.z;
        dot = -dot;
    }
    
    if (dot > 0.9995f) {
        result->w = q1->w + t * (q2_copy.w - q1->w);
        result->x = q1->x + t * (q2_copy.x - q1->x);
        result->y = q1->y + t * (q2_copy.y - q1->y);
        result->z = q1->z + t * (q2_copy.z - q1->z);
        Quaternion_Normalize(result, result);
        return;
    }
    
    const float theta = acosf(dot);
    const float sin_theta = sinf(theta);
    const float w1 = sinf((1.0f - t) * theta) / sin_theta;
    const float w2 = sinf(t * theta) / sin_theta;
    
    result->w = q1->w * w1 + q2_copy.w * w2;
    result->x = q1->x * w1 + q2_copy.x * w2;
    result->y = q1->y * w1 + q2_copy.y * w2;
    result->z = q1->z * w1 + q2_copy.z * w2;
}

/* 旋转矩阵运算函数 ----------------------------------------------------------*/

/**
 * @brief  初始化为单位矩阵
 * @param  matrix: 旋转矩阵
 */
void RotationMatrix_Identity(RotationMatrix* matrix) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix->m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

/**
 * @brief  从欧拉角创建旋转矩阵
 * @param  euler: 欧拉角(度)
 * @param  matrix: 结果旋转矩阵
 */
void RotationMatrix_FromEuler(const EulerAngles* euler, RotationMatrix* matrix) {
    const float roll_rad = Math_Deg2Rad(euler->roll);
    const float pitch_rad = Math_Deg2Rad(euler->pitch);
    const float yaw_rad = Math_Deg2Rad(euler->yaw);
    
    const float cr = cosf(roll_rad);
    const float sr = sinf(roll_rad);
    const float cp = cosf(pitch_rad);
    const float sp = sinf(pitch_rad);
    const float cy = cosf(yaw_rad);
    const float sy = sinf(yaw_rad);
    
    matrix->m[0][0] = cy * cp;
    matrix->m[0][1] = cy * sp * sr - sy * cr;
    matrix->m[0][2] = cy * sp * cr + sy * sr;
    
    matrix->m[1][0] = sy * cp;
    matrix->m[1][1] = sy * sp * sr + cy * cr;
    matrix->m[1][2] = sy * sp * cr - cy * sr;
    
    matrix->m[2][0] = -sp;
    matrix->m[2][1] = cp * sr;
    matrix->m[2][2] = cp * cr;
}

/**
 * @brief  从四元数创建旋转矩阵
 * @param  quat: 四元数
 * @param  matrix: 结果旋转矩阵
 */
void RotationMatrix_FromQuaternion(const Quaternion* quat, RotationMatrix* matrix) {
    const float qw = quat->w;
    const float qx = quat->x;
    const float qy = quat->y;
    const float qz = quat->z;

    const float qx2 = qx * qx;
    const float qy2 = qy * qy;
    const float qz2 = qz * qz;
    
    matrix->m[0][0] = 1.0f - 2.0f * (qy2 + qz2);
    matrix->m[0][1] = 2.0f * (qx * qy - qw * qz);
    matrix->m[0][2] = 2.0f * (qx * qz + qw * qy);
    
    matrix->m[1][0] = 2.0f * (qx * qy + qw * qz);
    matrix->m[1][1] = 1.0f - 2.0f * (qx2 + qz2);
    matrix->m[1][2] = 2.0f * (qy * qz - qw * qx);
    
    matrix->m[2][0] = 2.0f * (qx * qz - qw * qy);
    matrix->m[2][1] = 2.0f * (qy * qz + qw * qx);
    matrix->m[2][2] = 1.0f - 2.0f * (qx2 + qy2);
}

/**
 * @brief  旋转矩阵转四元数
 * @param  matrix: 旋转矩阵
 * @param  quat: 结果四元数
 */
void RotationMatrix_ToQuaternion(const RotationMatrix* matrix, Quaternion* quat) {
    const float trace = matrix->m[0][0] + matrix->m[1][1] + matrix->m[2][2];
    
    if (trace > 0.0f) {
        const float s = sqrtf(trace + 1.0f) * 2.0f;
        quat->w = 0.25f * s;
        quat->x = (matrix->m[2][1] - matrix->m[1][2]) / s;
        quat->y = (matrix->m[0][2] - matrix->m[2][0]) / s;
        quat->z = (matrix->m[1][0] - matrix->m[0][1]) / s;
    } else if ((matrix->m[0][0] > matrix->m[1][1]) && (matrix->m[0][0] > matrix->m[2][2])) {
        const float s = sqrtf(1.0f + matrix->m[0][0] - matrix->m[1][1] - matrix->m[2][2]) * 2.0f;
        quat->w = (matrix->m[2][1] - matrix->m[1][2]) / s;
        quat->x = 0.25f * s;
        quat->y = (matrix->m[0][1] + matrix->m[1][0]) / s;
        quat->z = (matrix->m[0][2] + matrix->m[2][0]) / s;
    } else if (matrix->m[1][1] > matrix->m[2][2]) {
        const float s = sqrtf(1.0f + matrix->m[1][1] - matrix->m[0][0] - matrix->m[2][2]) * 2.0f;
        quat->w = (matrix->m[0][2] - matrix->m[2][0]) / s;
        quat->x = (matrix->m[0][1] + matrix->m[1][0]) / s;
        quat->y = 0.25f * s;
        quat->z = (matrix->m[1][2] + matrix->m[2][1]) / s;
    } else {
        const float s = sqrtf(1.0f + matrix->m[2][2] - matrix->m[0][0] - matrix->m[1][1]) * 2.0f;
        quat->w = (matrix->m[1][0] - matrix->m[0][1]) / s;
        quat->x = (matrix->m[0][2] + matrix->m[2][0]) / s;
        quat->y = (matrix->m[1][2] + matrix->m[2][1]) / s;
        quat->z = 0.25f * s;
    }
}

/**
 * @brief  旋转矩阵乘法
 * @param  m1: 矩阵1
 * @param  m2: 矩阵2
 * @param  result: 结果矩阵(m1 * m2)
 */
void RotationMatrix_Multiply(const RotationMatrix* m1, const RotationMatrix* m2, RotationMatrix* result) {
    RotationMatrix temp;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            temp.m[i][j] = 0.0f;
            for (int k = 0; k < 3; k++) {
                temp.m[i][j] += m1->m[i][k] * m2->m[k][j];
            }
        }
    }
    *result = temp;
}

/**
 * @brief  旋转矩阵转置
 * @param  matrix: 输入矩阵
 * @param  result: 转置矩阵
 */
void RotationMatrix_Transpose(const RotationMatrix* matrix, RotationMatrix* result) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result->m[i][j] = matrix->m[j][i];
        }
    }
}

/* 欧拉角运算函数 ------------------------------------------------------------*/

/**
 * @brief  从四元数获取欧拉角
 * @param  quat: 四元数
 * @param  euler: 结果欧拉角(度)
 */
void EulerAngles_FromQuaternion(const Quaternion* quat, EulerAngles* euler) {
    Quaternion_ToEuler(quat, euler);
}

/**
 * @brief  从旋转矩阵获取欧拉角
 * @param  matrix: 旋转矩阵
 * @param  euler: 结果欧拉角(度)
 */
void EulerAngles_FromRotationMatrix(const RotationMatrix* matrix, EulerAngles* euler) {
    euler->pitch = Math_Rad2Deg(asinf(-matrix->m[2][0]));
    
    if (fabsf(matrix->m[2][0]) < 0.99999f) {
        euler->yaw = Math_Rad2Deg(atan2f(matrix->m[1][0], matrix->m[0][0]));
        euler->roll = Math_Rad2Deg(atan2f(matrix->m[2][1], matrix->m[2][2]));
    } else {
        euler->yaw = Math_Rad2Deg(atan2f(-matrix->m[0][1], matrix->m[1][1]));
        euler->roll = 0.0f;
    }
}

/**
 * @brief  欧拉角归一化
 * @param  euler: 输入欧拉角
 * @param  result: 归一化欧拉角
 */
void EulerAngles_Normalize(const EulerAngles* euler, EulerAngles* result) {
    result->roll = Math_WrapAngleDeg(euler->roll);
    result->pitch = Math_WrapAngleDeg(euler->pitch);
    result->yaw = Math_WrapAngleDeg(euler->yaw);
}

/* 坐标系变换函数 ------------------------------------------------------------*/

/**
 * @brief  世界坐标系转局部坐标系
 * @param  point: 世界坐标系中的点
 * @param  origin: 局部坐标系原点
 * @param  rotation: 局部坐标系旋转矩阵
 * @param  result: 局部坐标系中的点
 */
void TransformPoint_Cartesian(const CartesianCoord_Point* point, const CartesianCoord_Point* origin, 
                              const RotationMatrix* rotation, CartesianCoord_Point* result) {
    CartesianCoord_Point translated;
    Subtract_Cartesian(point, origin, &translated);
    RotateByMatrix_Cartesian(&translated, rotation, result);
}

/**
 * @brief  局部坐标系转世界坐标系
 * @param  point: 局部坐标系中的点
 * @param  origin: 局部坐标系原点
 * @param  rotation: 局部坐标系旋转矩阵
 * @param  result: 世界坐标系中的点
 */
void InverseTransformPoint_Cartesian(const CartesianCoord_Point* point, const CartesianCoord_Point* origin, 
                                     const RotationMatrix* rotation, CartesianCoord_Point* result) {
    RotationMatrix rotation_transpose;
    RotationMatrix_Transpose(rotation, &rotation_transpose);
    
    CartesianCoord_Point rotated;
    RotateByMatrix_Cartesian(point, &rotation_transpose, &rotated);
    
    Add_Cartesian(&rotated, origin, result);
}
