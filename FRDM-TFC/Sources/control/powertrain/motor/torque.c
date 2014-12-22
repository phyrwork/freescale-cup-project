/*
 * torque.c
 *
 *  Created on: Dec 22, 2014
 *      Author: Connor
 */

#include "control/powertrain/motor/torque.h"

/* Define torque control structs */
static PID_s PID[NUM_MOTORS];
MotorTorque_s MotorTorque[NUM_MOTORS];

void InitMotorTorqueControl()
{
	/* Initialise torque control structs */
	for (uint8_t i = 0; i < NUM_MOTORS; ++i)
	{
		MotorTorque_s *torque = &MotorTorque[i];
		
		/* Assign PWM controller and current sensor */
		torque->value = 0;
		torque->pwm = &MotorPWM[i];
		torque->current = &MotorCurrent[i];
		
		/* Initialise PID controller */
		torque->PID = &PID[i];
		torque->PID->Kp = TORQUE_KP;
		torque->PID->Ki = TORQUE_KI;
		torque->PID->Kd = TORQUE_KD;
		torque->PID->value_max = TORQUE_MAX;
		torque->PID->value_min = -TORQUE_MAX;
	}
}

void UpdateMotorTorque(MotorTorque_s *torque)
{
	UpdateCurrentValue(&torque->current); //Update current value
	torque->value = torque->current->value * TORQUE_KT; //Scale by torque constant
}

void SetMotorTorque(MotorTorque_s *torque, float command)
{
	UpdateMotorTorque(torque);
	UpdatePID(&torque->PID, command, torque->value);
}
