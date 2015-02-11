/*
 * attributes.c
 *
 *  Created on: Dec 4, 2014
 *      Author: Connor
 */

#include "telemetry/tftp/attributes.h"

/* Linescan attributes */
TFTP_ENDPOINT_PUSH(TFTP_LINESCAN0_ENDPOINT, TFTP_LINESCAN0_CODE, TFTP_LINESCAN_TYPE, TFTP_LINESCAN_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_LINESCAN1_ENDPOINT, TFTP_LINESCAN1_CODE, TFTP_LINESCAN_TYPE, TFTP_LINESCAN_SIZE);

/* Angular velocity attributes */
TFTP_ENDPOINT_PUSH(TFTP_W_FL_ENDPOINT, TFTP_W_FL_CODE, TFTP_ANGULAR_VELOCITY_TYPE, TFTP_ANGULAR_VELOCITY_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_W_FR_ENDPOINT, TFTP_W_FR_CODE, TFTP_ANGULAR_VELOCITY_TYPE, TFTP_ANGULAR_VELOCITY_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_W_RL_ENDPOINT, TFTP_W_RL_CODE, TFTP_ANGULAR_VELOCITY_TYPE, TFTP_ANGULAR_VELOCITY_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_W_RR_ENDPOINT, TFTP_W_RR_CODE, TFTP_ANGULAR_VELOCITY_TYPE, TFTP_ANGULAR_VELOCITY_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_WREF_RL_ENDPOINT, TFTP_WREF_RL_CODE, TFTP_ANGULAR_VELOCITY_TYPE, TFTP_ANGULAR_VELOCITY_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_WREF_RR_ENDPOINT, TFTP_WREF_RR_CODE, TFTP_ANGULAR_VELOCITY_TYPE, TFTP_ANGULAR_VELOCITY_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_W_V_ENDPOINT, TFTP_W_V_CODE, TFTP_ANGULAR_VELOCITY_TYPE, TFTP_ANGULAR_VELOCITY_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_WREF_V_ENDPOINT, TFTP_WREF_V_CODE, TFTP_ANGULAR_VELOCITY_TYPE, TFTP_ANGULAR_VELOCITY_SIZE);

/* Motor duty attributes */
TFTP_ENDPOINT_PUSH(TFTP_D_RL_ENDPOINT, TFTP_D_RL_CODE, TFTP_MOTOR_DUTY_TYPE, TFTP_MOTOR_DUTY_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_D_RR_ENDPOINT, TFTP_D_RR_CODE, TFTP_MOTOR_DUTY_TYPE, TFTP_MOTOR_DUTY_SIZE);

/* Motor current attributes */
TFTP_ENDPOINT_PUSH(TFTP_I_RL_ENDPOINT, TFTP_I_RL_CODE, TFTP_MOTOR_CURRENT_TYPE, TFTP_MOTOR_CURRENT_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_I_RR_ENDPOINT, TFTP_I_RR_CODE, TFTP_MOTOR_CURRENT_TYPE, TFTP_MOTOR_CURRENT_SIZE);

/* Motor torque attributes */
TFTP_ENDPOINT_PUSH(TFTP_T_RL_ENDPOINT, TFTP_T_RL_CODE, TFTP_MOTOR_TORQUE_TYPE, TFTP_MOTOR_TORQUE_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_T_RR_ENDPOINT, TFTP_T_RR_CODE, TFTP_MOTOR_TORQUE_TYPE, TFTP_MOTOR_TORQUE_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_TREF_RL_ENDPOINT, TFTP_TREF_RL_CODE, TFTP_MOTOR_TORQUE_TYPE, TFTP_MOTOR_TORQUE_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_TREF_RR_ENDPOINT, TFTP_TREF_RR_CODE, TFTP_MOTOR_TORQUE_TYPE, TFTP_MOTOR_TORQUE_SIZE);

/* Slip attributes */
TFTP_ENDPOINT_PUSH(TFTP_S_RL_ENDPOINT, TFTP_S_RL_CODE, TFTP_SLIP_TYPE, TFTP_SLIP_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_S_RR_ENDPOINT, TFTP_S_RR_CODE, TFTP_SLIP_TYPE, TFTP_SLIP_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_SREF_RL_ENDPOINT, TFTP_SREF_RL_CODE, TFTP_SLIP_TYPE, TFTP_SLIP_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_SREF_RR_ENDPOINT, TFTP_SREF_RR_CODE, TFTP_SLIP_TYPE, TFTP_SLIP_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_S_V_ENDPOINT, TFTP_S_V_CODE, TFTP_SLIP_TYPE, TFTP_SLIP_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_SREF_V_ENDPOINT, TFTP_SREF_V_CODE, TFTP_SLIP_TYPE, TFTP_SLIP_SIZE);

/* Positioning state attributes */
TFTP_ENDPOINT_PUSH(TFTP_POSITIONING_STATE_ENDPOINT, TFTP_POSITIONING_STATE_CODE, TFTP_POSITIONING_STATE_TYPE, TFTP_POSITIONING_STATE_SIZE);

/* Line attributes */
TFTP_ENDPOINT_PUSH(TFTP_TARGET_LINE_ENDPOINT, TFTP_TARGET_LINE_CODE, TFTP_LINE_TYPE, TFTP_LINE_SIZE);

/* Track position attributes */
TFTP_ENDPOINT_PUSH(TFTP_TRACK_POSITION_ENDPOINT, TFTP_TRACK_POSITION_CODE, TFTP_TRACK_POSITION_TYPE, TFTP_TRACK_POSITION_SIZE);

/* Profiler attributes */
TFTP_ENDPOINT_PUSH(TFTP_PROFILER_ENDPOINT, TFTP_PROFILER_CODE, TFTP_PROFILER_TYPE, TFTP_PROFILER_SIZE);

/* PID attributes */
TFTP_ENDPOINT_PUSH(TFTP_PID_T_RL_ENDPOINT, TFTP_PID_T_RL_CODE, TFTP_PID_TYPE, TFTP_PID_SIZE);
TFTP_ENDPOINT_PUSH(TFTP_PID_T_RR_ENDPOINT, TFTP_PID_T_RR_CODE, TFTP_PID_TYPE, TFTP_PID_SIZE);

/* Servo position attributes */
TFTP_ENDPOINT_PUSH(TFTP_SERVO_POSITION_ENDPOINT, TFTP_SERVO_POSITION_CODE, TFTP_SERVO_POSITION_TYPE, TFTP_SERVO_POSITION_SIZE);
