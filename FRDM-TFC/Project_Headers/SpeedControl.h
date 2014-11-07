/*
 * SpeedControl.h
 *
 *  Created on: Feb 3, 2014
 *      Author: Matt
 */

#include <TFC/TFC.h>
#include "SpeedSensor.h"

#ifndef SPEEDCONTROL_H_
#define SPEEDCONTROL_H_

struct persistantPIDVariables_s{
	float PWM;
	float previousPWM;
	float error;
	float previousError;
	float dError;
	float errorSum;
};

float getSpeedEstimate(float measurement, volatile isNewMeasurementAvailable_t* isNewMeasurementAvailable, float acceleration, float dt);
float getDesiredMotorPWM(float setpoint, float measurement, volatile isNewMeasurementAvailable_t* isNewMeasurementAvailable, uint8_t channel);
void PID(float setpoint, float measurement, struct persistantPIDVariables_s* PIDVariables, uint8_t incrementIntegral, float dt);


#endif /* SPEEDCONTROL_H_ */
