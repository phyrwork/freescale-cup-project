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
	PID->value  = (PID->error * PID->Kp); //sum proportional
	
	PID->integral += PID->error * dt; //accumulate error into integral
	PID->value += (PID->integral * PID->Ki); //sum integral
	
	if (PID->Kd > 0.0)
	{
		float error_diff = (PID->error - prev_error) / dt; //calculate error derivative
		PID->value += (error_diff * PID->Kd); //sum derivative
	}
	
	/* Saturation filter */
	if (PID->value > PID->value_max) PID->value = PID->value_max;
	if (PID->value < PID->value_min) PID->value = PID->value_min;
}
