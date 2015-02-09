/*
 * torque.h
 *
 *  Created on: Dec 22, 2014
 *      Author: Connor
 */

#ifndef MOTOR_TORQUE_H_
#define MOTOR_TORQUE_H_

#include "control/motor/pwm.h"
#include "sensors/motor/current.h"
#include "support/PID.h"

/* Motor properties */
#define TORQUE_MAX 0.0075f //Nm
#define TORQUE_KT 0.0134f //Torque constant

/* Control properties */
#define TORQUE_KP 120.0f
#define TORQUE_KI 75.0f
#define TORQUE_KD 0.5f

/* Torque structs */
typedef struct {
	float value;
	float cmd; //debug
	MotorPWM_s *pwm;
	MotorCurrent_s *current;
	PID_s *PID;
} MotorTorque_s;
extern MotorTorque_s MotorTorque[NUM_MOTORS];

void InitMotorTorqueControl();
void UpdateMotorTorque(MotorTorque_s *torque);
void SetMotorTorque(MotorTorque_s *torque, float command);

#endif /* MOTOR_TORQUE_H_ */
