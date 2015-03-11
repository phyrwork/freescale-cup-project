/*
 * SteeringControl.h
 *
 *  Created on: Nov 14, 2013
 *      Author: Matt
 */

#ifndef STEERINGCONTROL_H_
#define STEERINGCONTROL_H_

#include "support/Types.h"
#include "sensors/vision/positioning.h"

float getDesiredServoValue(int8_t position, int8_t setpoint);

#endif /* STEERINGCONTROL_H_ */
