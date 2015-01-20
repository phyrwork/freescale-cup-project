/*
 * SpeedControl.c
 *
 *  Created on: Feb 3, 2014
 *      Author: Matt
 */

#include "control/motor/speed.h"
#include "support/ARM_SysTick.h"
#include "config.h"

/*
typedef struct {
	float value;
	CadenceSensor_s *sensor;
	float ratio;
	MotorPWM_s *pwm;
	PID_s *pid;
} MotorSpeed_s;
*/

#define MSPEED_KP 0.005f
#define MSPEED_KI 0.007f
#define MSPEED_KD 0.0f

static PID_s pid[NUM_MOTORS];
MotorSpeed_s MotorSpeeds[NUM_MOTORS];

void InitMotorSpeedControl()
{
	MotorSpeeds[REAR_LEFT].value = 0.0f;
	MotorSpeeds[REAR_LEFT].sensor = &CadenceSensors[1];
	MotorSpeeds[REAR_LEFT].ratio = 1.0f;
	MotorSpeeds[REAR_LEFT].pwm = &MotorPWM[REAR_LEFT];
	MotorSpeeds[REAR_LEFT].pid = &pid[REAR_LEFT];
	
	MotorSpeeds[REAR_RIGHT].value = 0.0f;
	MotorSpeeds[REAR_RIGHT].sensor = &CadenceSensors[0];
	MotorSpeeds[REAR_RIGHT].ratio = 1.0f;
	MotorSpeeds[REAR_RIGHT].pwm = &MotorPWM[REAR_RIGHT];
	MotorSpeeds[REAR_RIGHT].pid = &pid[REAR_RIGHT];
	
	for (uint8_t i = 0; i < NUM_MOTORS; ++i)
	{
		MotorSpeed_s *speed = &MotorSpeeds[i];
		
		/* Initialise PID controllers */
		speed->pid = &pid[i];
		speed->pid->Kp = MSPEED_KP;
		speed->pid->Ki = MSPEED_KI;
		speed->pid->Kd = MSPEED_KD;
		speed->pid->time = 0;
		speed->pid->value = 0;
		speed->pid->value_max = 1;
		speed->pid->value_min = 0;
	}
}

void UpdateMotorSpeed(MotorSpeed_s *speed)
{
	//if (speed->sensor->flag == 0) return; //no new measurement available
	//else speed->sensor->flag = 0;         //clear flag
	
	uint32_t period = speed->sensor->period * speed->ratio; //revolution period in TPM ticks	
	speed->value = period ? ( (float) speed->sensor->TPM->MOD / (float) period ) : 0; //revolution frequency in seconds
}

void SetMotorSpeed(MotorSpeed_s *speed, float command)
{
	speed->cmd = command; //Store command for telemetry purposes.
	UpdateMotorSpeed(speed);
	UpdatePID(speed->pid, command, speed->value);
	SetMotorPWM(speed->pwm, speed->pid->value);
}
