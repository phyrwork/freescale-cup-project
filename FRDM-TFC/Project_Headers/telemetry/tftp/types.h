/* types.h
 * =====================================================
 * TFTP attribute type definitions.
 * -----------------------------------------------------
 * Author: Connor Newton
 *   Date: November 24, 2014
 */

#ifndef TFTP_TYPES
#define TFTP_TYPES

///////////////////////////////////////////////////
// IMPORTANT: Don't wrap types in parentheses as //
//            it will cause compilation to fail  //
///////////////////////////////////////////////////

/* Linescan camera attribute */
#define TFTP_LINESCAN_TYPE  uint16_t
#define TFTP_LINESCAN_SIZE  128

/* Angular velocity attribute */
#define TFTP_ANGULAR_VELOCITY_TYPE  float
#define TFTP_ANGULAR_VELOCITY_SIZE  1

/* Motor current attribute */
#define TFTP_MOTOR_DUTY_TYPE  float
#define TFTP_MOTOR_DUTY_SIZE  1

/* Motor current attribute */
#define TFTP_MOTOR_CURRENT_TYPE  float
#define TFTP_MOTOR_CURRENT_SIZE  1

/* Motor torque attribute */
#define TFTP_MOTOR_TORQUE_TYPE float
#define TFTP_MOTOR_TORQUE_SIZE 1

/* Slip attributes */
#define TFTP_SLIP_TYPE float
#define TFTP_SLIP_SIZE 1

/* Tracking state attributes */
#include "sensors/camera/LineDetection.h"
#define TFTP_POSITIONING_STATE_TYPE PositioningState
#define TFTP_POSITIONING_STATE_SIZE 1

/* Line attributes */
//#include "sensors/camera/LineDetection.h"
#define TFTP_LINE_TYPE Line_s
#define TFTP_LINE_SIZE 1

/* Track position attributes */
#define TFTP_TRACK_POSITION_TYPE int8_t
#define TFTP_TRACK_POSITION_SIZE 1

/* Profiler attribute */
#include "support/Profiler.h"
#define TFTP_PROFILER_TYPE ProfilerFrame
#define TFTP_PROFILER_SIZE 1

/* PID attributes */
#include "support/PID.h"
#define TFTP_PID_TYPE PID_s
#define TFTP_PID_SIZE 1

#endif //TFTP_TYPES


