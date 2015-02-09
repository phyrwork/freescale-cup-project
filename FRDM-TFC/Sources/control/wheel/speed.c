/*
 * wheel/speed.c
 *
 *  Created on: Jan 21, 2015
 *      Author: Connor
 */

#include "control/wheel/speed.h"
#include "sensors/vehicle/speed.h"
#include "support/ARM_SysTick.h"
#include "config.h"

#define MSPEED_KP 0.14f
#define MSPEED_KI 0.140f
#define MSPEED_KD 0.0f

static PID_s pid[NUM_MOTORS];
WheelSpeedControl_s WheelSpeedControls[NUM_MOTORS];

void InitWheelSpeedControl()
{
	//rear left
	WheelSpeedControls[REAR_LEFT].sensor = &WheelSpeedSensors[REAR_LEFT];
	WheelSpeedControls[REAR_LEFT].pwm = &MotorPWM[REAR_LEFT];
	WheelSpeedControls[REAR_LEFT].pid = &pid[REAR_LEFT];
	
	//rear right
	WheelSpeedControls[REAR_RIGHT].sensor = &WheelSpeedSensors[REAR_RIGHT];
	WheelSpeedControls[REAR_RIGHT].pwm = &MotorPWM[REAR_RIGHT];
	WheelSpeedControls[REAR_RIGHT].pid = &pid[REAR_RIGHT];
	
	for (uint8_t i = 0; i < NUM_MOTORS; ++i)
	{
		WheelSpeedControl_s *speed = &WheelSpeedControls[i];
		
		/* Initialise PID controllers */
		speed->pid = &pid[i];
		speed->pid->Kp = MSPEED_KP;
		speed->pid->Ki = MSPEED_KI;
		speed->pid->Kd = MSPEED_KD;
		speed->pid->time = 0;
		speed->pid->value = 0;
		speed->pid->value_max = 1;
		speed->pid->value_min = -1;
	}
}

void SetWheelSpeed(WheelSpeedControl_s *speed, float command)
{
	speed->cmd = command; //Store command for telemetry purposes.
	UpdateWheelSpeed(speed->sensor);
	
	if (command == 0 || VehicleSpeedSensor.value < 5) speed->pid->value_min = 0; //try and prevent max throttle 'stops'
	else speed->pid->value_min = -1;
	
	if (speed->sensor->value == 0 && command == 0 ) //if vehicle is stationary or nearly stationary
	{
		*(speed->pwm->tpm->fwdCnVReg) = (uint16_t) (TPM0_MOD * 0xFF);  //complementary 50% PWM to prevent car rolling
		*(speed->pwm->tpm->bwdCnVReg) = (uint16_t) ~(TPM0_MOD * 0xFF);
		speed->pwm->value = 0;
	}
	else {
		UpdatePID(speed->pid, command, speed->sensor->value);
		SetMotorPWM(speed->pwm, speed->pid->value);
	}
}
