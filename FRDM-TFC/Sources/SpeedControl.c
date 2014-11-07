/*
 * SpeedControl.c
 *
 *  Created on: Feb 3, 2014
 *      Author: Matt
 */

#include <SpeedControl.h>

#define KP 0.3f
#define KI 0.5f
#define KD 0.0f
#define BREAK_KP 0.005f
#define MIN_BREAK_SPEED 10.0f
#define MAX_PWM 1.0f
#define MIN_PWM -0.6f
#define MIN_SPEED 4.0f //Minimum to maintain control / avoid stalling

#define INCLUDE_INTEGRAL 1
#define NO_INTEGRAL 0

float getMinPWM(float measurement)
{
	measurement -= MIN_BREAK_SPEED;
	if(measurement < 0.0f)
		measurement = 0.0f;
	float minPWM = - (measurement * BREAK_KP);
	if(minPWM < MIN_PWM)
		minPWM = MIN_PWM;
	
	return minPWM;
}

float getSpeedEstimate(float measurement, volatile isNewMeasurementAvailable_t* isNewMeasurementAvailable, float acceleration, float dt)
{
	static float speed = 0;
	if (*isNewMeasurementAvailable == NEW_MEASUREMENT_AVAILABLE)
	{
		*isNewMeasurementAvailable = NO_NEW_MEASUREMENT_AVAILABLE;
		speed = measurement;
	}
	else
	{
//		speed += acceleration * dt * (1 / ACCEL_SCALER);
	}
	return speed;
}

float getDesiredMotorPWM(float setpoint, float measurement, volatile isNewMeasurementAvailable_t* isNewMeasurementAvailable, uint8_t channel)
{
	static struct persistantPIDVariables_s PIDVariables[] =
	{
	{ 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0 } };
//	static float lastSetpoint = 0;

//	float dSetpoint = setpoint - lastSetpoint;
//	
//	if(dSetpoint > MAX_DSETPOINT)
//		dSetpoint = MAX_DSETPOINT;
//	else if(dSetpoint < -MAX_DSETPOINT)
//		dSetpoint = -MAX_DSETPOINT;
//	
//	setpoint = lastSetpoint + dSetpoint;

	if (*isNewMeasurementAvailable == NEW_MEASUREMENT_AVAILABLE)
	{
		*isNewMeasurementAvailable = NO_NEW_MEASUREMENT_AVAILABLE;

		float dt = (TFC_Ticker[2] / 10000.0f);
		TFC_Ticker[2] = 0;
		if(dt > 0.1f)
		{
			dt = 0.1f;
		}

		
		float minPWM = -0.8f;//getMinPWM(measurement);

		if (PIDVariables[channel].PWM < MAX_PWM && PIDVariables[channel].PWM > minPWM)
		{
			PID(setpoint, measurement, &PIDVariables[channel], INCLUDE_INTEGRAL, dt);
		}
		else
		{
			PID(setpoint, measurement, &PIDVariables[channel], NO_INTEGRAL, dt);
		}

		
		if (PIDVariables[channel].PWM < minPWM)
			PIDVariables[channel].PWM = minPWM;
		else if (PIDVariables[channel].PWM > MAX_PWM)
			PIDVariables[channel].PWM = MAX_PWM;
	}
	return PIDVariables[channel].PWM;
}

void PID(float setpoint, float measurement, struct persistantPIDVariables_s* PIDVariables, uint8_t incrementIntegral, float dt)
{
	PIDVariables->previousError = PIDVariables->error;
	PIDVariables->previousPWM = PIDVariables->PWM;
	PIDVariables->error = setpoint - measurement;
	PIDVariables->errorSum += PIDVariables->error * incrementIntegral * dt;
	PIDVariables->dError = (PIDVariables->error - PIDVariables->previousError) / dt;
	PIDVariables->PWM = (PIDVariables->error * KP) + (PIDVariables->errorSum * KI); // + (PIDVariables->dError * KD);
//	float dPWM = (PIDVariables->PWM - PIDVariables->previousPWM); // / dt;
//
//	if (dPWM > MAX_DPWM)
//	{
//		dPWM = MAX_DPWM;
//	}
//	else if (dPWM < -MAX_DPWM)
//	{
//		dPWM = -MAX_DPWM;
//	}
//
//	PIDVariables->PWM = PIDVariables->previousPWM + dPWM;
}

