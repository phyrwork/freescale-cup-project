/*
 * SteeringControl.h
 *
 *  Created on: Nov 14, 2013
 *      Author: Matt
 */

#ifndef STEERINGCONTROL_H_
#define STEERINGCONTROL_H_

#include "support/Types.h"
#include "sensors/camera/LineDetection.h"

float getDesiredServoValue(int8_t position, int8_t setpoint, lineScanState_t* lineScanState);

#endif /* STEERINGCONTROL_H_ */
