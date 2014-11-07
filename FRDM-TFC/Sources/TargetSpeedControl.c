/*
 * TargetSpeedControl.c
 *
 *  Created on: Feb 20, 2014
 *      Author: Matt
 */
#include "TargetSpeedControl.h"

#define MAX_SPEED 30.0f
#define MIN_SPEED 0.0f//5.0f
#define KP 0.012f//0.0068f
#define ANGLE_LIMIT 0

float getDesiredSpeed(carState_s* carState, float maxSetpoint)
{
	float speed;
	if(abs(carState->lineCenter) > ANGLE_LIMIT)
	{
		speed = (maxSetpoint * (1 - (KP * abs(carState->lineCenter))));
	}
	else
	{
		speed = maxSetpoint;
	}
	
	if(speed > MAX_SPEED)
		speed = MAX_SPEED;
	else if(speed < MIN_SPEED)
		speed = MIN_SPEED;
	return speed;
}
