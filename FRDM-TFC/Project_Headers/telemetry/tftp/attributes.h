/* attributes.h
 * =====================================================
 * TFTP attribute and endpoint definitions.
 * -----------------------------------------------------
 * Author: Connor Newton
 *   Date: November 24, 2014
 */

#include "telemetry/tftp/types.h"
#include "telemetry/tftp/endpoint.h"

/* Linescan attributes */
#define TFTP_LINESCAN0
#define TFTP_LINESCAN0_ID linescan0
#define TFTP_LINESCAN0_CODE 0x01
#define TFTP_LINESCAN0_ENDPOINT \
 		TFTP_ENDPOINT_PUSH(TFTP_LINESCAN0_ID, TFTP_LINESCAN0_CODE, TFTP_LINESCAN_TYPE, TFTP_LINESCAN_SIZE)

#define TFTP_LINESCAN1
#define TFTP_LINESCAN1_ID linescan1
#define TFTP_LINESCAN1_CODE 0x02
#define TFTP_LINESCAN1_ENDPOINT \
 		TFTP_ENDPOINT_PUSH(TFTP_LINESCAN0_ID, TFTP_LINESCAN0_CODE, TFTP_LINESCAN_TYPE, TFTP_LINESCAN_SIZE)

/* Angular velocity attributes */
#define TFTP_W_FL
#define TFTP_W_FL_ID w_fl
#define TFTP_W_FL_CODE 0x03
#define TFTP_W_FL_ENDPOINT \
 		TFTP_ENDPOINT_PUSH(TFTP_W_FL_ID, TFTP_W_FL_CODE, TFTP_ANGULAR_VELOCITY_TYPE, TFTP_ANGULAR_VELOCITY_SIZE)

#define TFTP_W_FR
#define TFTP_W_FR_ID w_fr
#define TFTP_W_FR_CODE 0x04
#define TFTP_W_FR_ENDPOINT \
 		TFTP_ENDPOINT_PUSH(TFTP_W_FR_ID, TFTP_W_FR_CODE, TFTP_ANGULAR_VELOCITY_TYPE, TFTP_ANGULAR_VELOCITY_SIZE)
		
#define TFTP_W_RL
#define TFTP_W_RL_ID w_rl
#define TFTP_W_RL_CODE 0x05
#define TFTP_W_RL_ENDPOINT \
 		TFTP_ENDPOINT_PUSH(TFTP_W_RL_ID, TFTP_W_RL_CODE, TFTP_ANGULAR_VELOCITY_TYPE, TFTP_ANGULAR_VELOCITY_SIZE)

#define TFTP_W_RR
#define TFTP_W_RR_ID w_rr
#define TFTP_W_RR_CODE 0x06
#define TFTP_W_RR_ENDPOINT \
 		TFTP_ENDPOINT_PUSH(TFTP_W_RR_ID, TFTP_W_RR_CODE, TFTP_ANGULAR_VELOCITY_TYPE, TFTP_ANGULAR_VELOCITY_SIZE)

/* Motor current attributes */
#define TFTP_I_RL_CODE
#define TFTP_I_RL_ID I_rl
#define TFTP_I_RL_CODE 0x07;
#define TFTP_I_RL_ENDPOINT \
 		TFTP_ENDPOINT_PUSH(TFTP_I_RL_ID, TFTP_I_RL_CODE, TFTP_MOTOR_CURRENT_TYPE, TFTP_MOTOR_CURRENT_SIZE)

#define TFTP_I_RR_CODE
#define TFTP_I_RR_ID I_rr
#define TFTP_I_RR_CODE 0x08;
#define TFTP_I_RR_ENDPOINT \
 		TFTP_ENDPOINT_PUSH(TFTP_I_RR_ID, TFTP_I_RR_CODE, TFTP_MOTOR_CURRENT_TYPE, TFTP_MOTOR_CURRENT_SIZE)

/* Motor torque attributes */
#define TFTP_T_RL
#define TFTP_T_RL_ID T_rl
#define TFTP_T_RL_CODE      0x09;
#define TFTP_T_RL_ENDPOINT \
 		TFTP_ENDPOINT_PUSH(TFTP_T_RL_ID, TFTP_T_RL_CODE, TFTP_MOTOR_TORQUE_TYPE, TFTP_MOTOR_TORQUE_SIZE)

#define TFTP_T_RR
#define TFTP_T_RR_ID T_rr
#define TFTP_T_RR_CODE      0x0A;
#define TFTP_T_RR_ENDPOINT \
 		TFTP_ENDPOINT_PUSH(TFTP_T_RR_ID, TFTP_T_RR_CODE, TFTP_MOTOR_TORQUE_TYPE, TFTP_MOTOR_TORQUE_SIZE)

/* Slip attributes */
#define TFTP_S_RL
#define TFTP_S_RL_ID s_rl
#define TFTP_S_RL_CODE      0x0B;
#define TFTP_S_RL_ENDPOINT \
 		TFTP_ENDPOINT_PUSH(TFTP_S_RL_ID, TFTP_S_RL_CODE, TFTP_SLIP_TYPE, TFTP_SLIP_SIZE)

#define TFTP_S_RL
#define TFTP_S_RL_ID s_rr
#define TFTP_S_RL_CODE      0x0C;
#define TFTP_S_RL_ENDPOINT \
 		TFTP_ENDPOINT_PUSH(TFTP_S_RL_ID, TFTP_S_RL_CODE, TFTP_SLIP_TYPE, TFTP_SLIP_SIZE)
