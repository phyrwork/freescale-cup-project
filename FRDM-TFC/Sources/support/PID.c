/*
 * PID.c
 *
 *  Created on: Dec 22, 2014
 *      Author: Connor
 */

#include "support/PID.h"

/* Import timer facility */
#include "support/ARM_SysTick.h"
#include "config.h"
#define PID_TICKER TFC_Ticker[UPTIME_TICKER]
#define TIME ( (float)PID_TICKER / (float)SYSTICK_FREQUENCY )
#define MAX_DT 0.01

#define KI_CLAMPED 1
#define KI_ACTIVE  0

void UpdatePID(PID_s *PID, float ref, float actual)
{
	/* Take reference values */
	float prev_error = PID->error;
	float prev_value = PID->value;
	float prev_time  = PID->time;
	
	/* Fetch new time, calculate dt */
	PID->time = TIME;
	float dt = PID->time - prev_time;
	
	/* Update controller */
	PID->error = ref - actual; //calculate error
	
	
	PID->value = (PID->error * PID->Kp); //sum proportional
	
	if (!PID->clamped)  PID->integral += PID->error * dt;        //accumulate error into integral
	if (PID->Ki > 0.0f) PID->value += (PID->integral * PID->Ki); //sum integral	
	
	if (PID->Kd > 0.0f && dt > 0.0f)
	{
		float error_diff = (PID->error - prev_error) / dt; //calculate error derivative
		PID->value += (error_diff * PID->Kd); //sum derivative
	}
	
	
	//operate integral clamp
	if (PID->antiwindup == ANTI_WINDUP_CLAMP)
	{
		int8_t error_sign = PID->error != 0 ? (PID->error > 0 ? 1 : -1) : 0;
		int8_t integral_sign = PID->integral != 0 ? (PID->integral > 0 ? 1 : -1) : 0;
		
		if (!PID->clamped) {
			// if output > saturation limits and integral output and set point have same sign, clamp
			if ( (PID->value > PID->value_max || PID->value < PID->value_min) && integral_sign == error_sign )
				PID->clamped = KI_CLAMPED;
		}
		else
		{
			// if output > saturation limits and integral output and set point have different sign, unclamp				
			if ( (PID->value > PID->value_max || PID->value < PID->value_min) && integral_sign != error_sign )
				PID->clamped = KI_ACTIVE;
		}
	}
	
	/* Saturation filter */
	if (PID->value > PID->value_max) PID->value = PID->value_max;
	if (PID->value < PID->value_min) PID->value = PID->value_min;
}
