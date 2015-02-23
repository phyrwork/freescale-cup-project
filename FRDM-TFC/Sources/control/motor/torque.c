/*
 * torque.c
 *
 *  Created on: Dec 22, 2014
 *      Author: Connor
 */

#include "control/motor/torque.h"

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
		torque->speed = &WheelSpeedSensor[i];
		//torque->speed = &VehicleSpeedSensor;
		
		/* Initialise PID controller */
		torque->PID = &PID[i];
		torque->PID->Kp = TORQUE_KP;
		torque->PID->Ki = TORQUE_KI;
		torque->PID->Kd = TORQUE_KD;
		torque->PID->time = 0;
		torque->PID->value = 0;
		torque->PID->value_max = 1;
		torque->PID->value_min = -1;
		torque->PID->antiwindup = ANTI_WINDUP_CLAMP;
	}
}

void UpdateMotorTorque(MotorTorque_s *torque)
{
	UpdateCurrentValue(torque->current); //Update current value
	torque->value = torque->pwm->value >= 0 ? //scale by torque constant
			torque->current->value * TORQUE_KT : torque->current->value * -TORQUE_KT; //positive or negative torque?
}

void SetMotorTorque(MotorTorque_s *torque, float command)
{
	torque->cmd = command; //Store command for telemetry purposes.
	UpdateMotorTorque(torque);
	
	//limit command torque to max torque
	command = TORQUE_MAX;
	
	//recalculate saturation
	float tspd = torque->speed->value > 31 ? 31 : torque->speed->value;
	float tsat = (TORQUE_SPEED_M * tspd) + TORQUE_SPEED_C;
	//command = command > tsat ? tsat : (command < -tsat ? -tsat : command);
	torque->PID->in_max = tsat;
	torque->PID->in_min = -tsat;
	
	UpdatePID(torque->PID, command, torque->value);
	SetMotorPWM(torque->pwm, torque->PID->value);
}
