/*
 * SpeedSensor.h
 *
 *  Created on: Feb 4, 2014
 *      Author: Matt
 */

#ifndef SPEEDSENSOR_H_
#define SPEEDSENSOR_H_

#include "support/Types.h"

typedef enum {NEW_MEASUREMENT_AVAILABLE, NO_NEW_MEASUREMENT_AVAILABLE} isNewMeasurementAvailable_t;

struct speed_s{
	uint32_t halfPeriodTimes[2];
	uint16_t previousCount;
	uint8_t state;
	uint32_t period;
	uint8_t overflowed;
	isNewMeasurementAvailable_t measurementAvailable;
	float speed;
};

void TFC_InitSpeedSensor();
volatile isNewMeasurementAvailable_t* isANewmeasurementAvailable(uint8_t channel);
void FTM2_IRQHandler();
float getSpeed(uint8_t channel);

#endif /* SPEEDSENSOR_H_ */
