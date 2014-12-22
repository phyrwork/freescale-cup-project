/*
 * torque.h
 *
 *  Created on: Dec 22, 2014
 *      Author: Connor
 */

#ifndef MOTOR_TORQUE_H_
#define MOTOR_TORQUE_H_

#include "control/powertrain/motor/pwm.h"
#include "sensors/current.h"
#include "support/PID.h"

/* Motor properties */
#define TORQUE_MAX 0.005 //Nm
#define TORQUE_KT 1 //Torque constant

/* Control properties */
#define TORQUE_KP 1
#define TORQUE_KI 1
#define TORQUE_KD 1

/* Torque structs */
typedef struct {
	float value;
	MotorPWM_s *pwm;
	MotorCurrent_s *current;
	PID_s *PID;
} MotorTorque_s;
extern MotorTorque_s MotorTorque[NUM_MOTORS];

void InitMotorTorqueControl();
void UpdateMotorTorque(MotorTorque_s *torque);
void SetMotorTorque(MotorTorque_s *torque, float command);

#endif /* MOTOR_TORQUE_H_ */
