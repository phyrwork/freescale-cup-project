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

/* Control properties */
#define TORQUE_KP 40.0f
#define TORQUE_KI 40.0f
#define TORQUE_KD 0.0f

#define TORQUE_PID_GAIN_C 1.0f
#define TORQUE_PID_GAIN_M 0.1f

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
		torque->speed = &WheelSpeedSensors[i];
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
		torque->PID->value_max = MAX_DUTY;
		torque->PID->value_min = -MAX_DUTY;
	}
}

void UpdateMotorTorque(MotorTorque_s *torque)
{
	UpdateCurrentValue(torque->current); //Update current value
	torque->value = torque->current->value * TORQUE_KT;
}

void SetMotorTorque(MotorTorque_s *torque, float command)
{
	torque->cmd = command; //Store command for telemetry purposes.
	UpdateMotorTorque(torque);
	
	//limit command torque to max torque
	command = command > TORQUE_MAX ? TORQUE_MAX : (command < -TORQUE_MAX ? -TORQUE_MAX : command);
	
	//recalculate saturation
	float tspd = torque->speed->value > 31 ? 31 : torque->speed->value;
	float tsat = (TORQUE_SPEED_M * tspd) + TORQUE_SPEED_C;
	//command = command > tsat ? tsat : (command < -tsat ? -tsat : command);
	
	UpdatePID(torque->PID, command, torque->value);
	SetMotorPWM(torque->pwm, torque->PID->value);
}
