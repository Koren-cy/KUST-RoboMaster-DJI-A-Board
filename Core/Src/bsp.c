/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp.h"
#include <string.h>
#include <stdio.h>
#include "../../User_Algorithm/user_coord.h"


/* 主循环注册表 --------------------------------------------------------------*/
void (*loop_event[MAX_LOOP_EVENT])(void) = {0};
uint8_t loop_event_num = 0;

void LOOP_EVENT_Handle(void) {
    for (uint8_t event_index = 0 ; event_index < loop_event_num ; event_index++) {
        loop_event[event_index]();
    }
}

/* JScope ------------------------------------------------------------------*/
JScope_Transmit_t jscope_transmit = {0};
uint8_t JScope_RTT_UpBuffer[BUFFER_SIZE_UP] = {0};

/* 接口定义 --------------------------------------------------------------------*/

// 调试串口
UART_DRIVES user_debug_uart = {0};

// 状态灯
LED_DRIVES user_red_led = {0};
LED_DRIVES user_green_led = {0};

// can 总线
CAN_DRIVES user_can_1 = {0};
CAN_DRIVES user_can_2 = {0};
CAN_REMOTE_CONTROL_COMMAND can_remote_control_command = {0};
CAN_CHASSIS_MOTION_PROTOCOL can_chassis_motion_command = {0};
CAN_GYROSCOPE_PROTOCOL can_gyroscope_data = {0};
void user_can_2_callback(void * user_can) {
    const CAN_DRIVES *can = (CAN_DRIVES*)user_can;
    const uint8_t *receive_data = can->rx_msg.Data;

    CartesianCoord_Point move_vector = {0};
    CartesianCoord_Point input_vector = {0, 0, 0};

    switch (can->rx_msg.StdId) {
        case CAN_REMOTE_CONTROL_ID:
            if (0) break;
            // 兼容无避障模式
            can_remote_control_command.v_y             = (int16_t) (receive_data[0] << 0 | receive_data[1] << 8);
            can_remote_control_command.v_x             = (int16_t) (receive_data[2] << 0 | receive_data[3] << 8);
            can_remote_control_command.ω_theta_chassis = (int16_t) (receive_data[4] << 0 | receive_data[5] << 8);
            can_remote_control_command.d_theta_turret  = (int16_t) (receive_data[6] << 0 | receive_data[7] << 8);

            SwerveChassis_InverseKinematics(&user_swerve_chassis);
            orientation_angle = orientation_angle - user_swerve_chassis.omega_current * 0.12975f - (float) can_remote_control_command.d_theta_turret * 0.0001f;

            input_vector.x = (float) can_remote_control_command.v_x * 0.006f;
            input_vector.y = (float) can_remote_control_command.v_y * 0.006f;

            RotateZ_Cartesian(&input_vector, -orientation_angle, &move_vector);
            SwerveChassis_Kinematics(&user_swerve_chassis, move_vector.x, move_vector.y, (float) can_remote_control_command.ω_theta_chassis * 0.005f);
            SwerveChassis_Set_Motor_Target(&user_swerve_chassis);

            DJI_Motor_Set_State(&YAW_GM6020, orientation_angle);
            DJI_Motor_Execute(&user_can_1);
            break;
        case CAN_CHASSIS_MOTION_ID:
            can_chassis_motion_command.v_y             = (int16_t) (receive_data[0] << 0 | receive_data[1] << 8);
            can_chassis_motion_command.v_x             = (int16_t) (receive_data[2] << 0 | receive_data[3] << 8);
            can_chassis_motion_command.ω_theta_chassis = (int16_t) (receive_data[4] << 0 | receive_data[5] << 8);
            can_chassis_motion_command.d_theta_turret  = (int16_t) (receive_data[6] << 0 | receive_data[7] << 8);

            SwerveChassis_InverseKinematics(&user_swerve_chassis);
            orientation_angle = orientation_angle - user_swerve_chassis.omega_current * 0.12975f - (float) can_chassis_motion_command.d_theta_turret;

            input_vector.x = (float) can_chassis_motion_command.v_x / 1000.0f;
            input_vector.y = (float) can_chassis_motion_command.v_y / 1000.0f;

            RotateZ_Cartesian(&input_vector, -orientation_angle, &move_vector);
            SwerveChassis_Kinematics(&user_swerve_chassis, move_vector.x, move_vector.y, (float) can_chassis_motion_command.ω_theta_chassis * 3.0f / 660.0f / TWO_PI);
            SwerveChassis_Set_Motor_Target(&user_swerve_chassis);

            DJI_Motor_Set_State(&YAW_GM6020, orientation_angle);
            DJI_Motor_Execute(&user_can_1);
            break;
        case CAN_GYROSCOPE_ID:

            break;
        default: ;
    }

}

// 蜂鸣器
BUZZER_DRIVES user_buzzer_1 = {0};

// 启动音乐
STARTUP_MUSIC_DRIVES user_startup_music = {0};
SysTick_Task user_startup_music_task = {0};

// PID 控制器
PID_Controller FR_GM6020_PID = {0};
PID_Controller FL_GM6020_PID = {0};
PID_Controller RR_GM6020_PID = {0};
PID_Controller RL_GM6020_PID = {0};
PID_Controller FR_M3508_PID = {0};
PID_Controller FL_M3508_PID = {0};
PID_Controller RR_M3508_PID = {0};
PID_Controller RL_M3508_PID = {0};

// LADRC 控制器
LADRC_Controller YAW_GM6020_LADRC = {0};

// 大疆电机
DJI_MOTOR_DRIVES FR_GM6020 = {0};
DJI_MOTOR_DRIVES FL_GM6020 = {0};
DJI_MOTOR_DRIVES RR_GM6020 = {0};
DJI_MOTOR_DRIVES RL_GM6020 = {0};

DJI_MOTOR_DRIVES FR_M3508 = {0};
DJI_MOTOR_DRIVES FL_M3508 = {0};
DJI_MOTOR_DRIVES RR_M3508 = {0};
DJI_MOTOR_DRIVES RL_M3508 = {0};

DJI_MOTOR_DRIVES YAW_GM6020 = {0};

// 舵轮底盘
SwerveChassisState user_swerve_chassis = {0};

// 云台指向
float orientation_angle = 0.0f;