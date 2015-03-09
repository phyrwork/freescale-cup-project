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
#include "sensors/wheel/speed.h"
#include "sensors/vehicle/speed.h"
#include "support/PID.h"

/* Motor properties */
#define TORQUE_STALL 0.00784532f
#define TORQUE_CUTOFF 0.0078f
#define TORQUE_MAX 0.0075f //Nm
#define TORQUE_KT 0.002064557f //Torque constant

#define TORQUE_SPEED_M -0.00018714f
#define TORQUE_SPEED_C 0.0060841f

/* Torque structs */
typedef struct {
	float value;
	float cmd; //debug
	MotorPWM_s *pwm;
	MotorCurrent_s *current;
	WheelSpeedSensor_s *speed;
	//VehicleSpeedSensor_s *speed;
	PID_s *PID;
} MotorTorque_s;
extern MotorTorque_s MotorTorque[NUM_MOTORS];

void InitMotorTorqueControl();
void UpdateMotorTorque(MotorTorque_s *torque);
void SetMotorTorque(MotorTorque_s *torque, float command);

#endif /* MOTOR_TORQUE_H_ */
