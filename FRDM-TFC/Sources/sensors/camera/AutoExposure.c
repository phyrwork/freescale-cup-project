/* 
 * AutoExposure.c
 *
 *  Created on: Dec 10, 2013
 *      Author: admin
 */

#include "sensors/camera/AutoExposure.h"
#include "sensors/camera/LineScanCamera.h"
#include "support/Types.h"
#include "support/ARM_SysTick.h"

#define Kp 0.01f
#define Ki 0.0f
#define Kd 0.0001f
#define MAX_EXPOSURE 100
#define MIN_EXPOSURE 10

int32_t error = 0;
int32_t previousError = 0;
int32_t dError = 0;
int32_t errorSum = 0;
int32_t newExposure = 10000;

uint32_t getTotalIntensity(volatile uint16_t* LineScanImage)
{
	uint32_t pixelTotal = 0;

	for (uint8_t k = 0; k < 128; k++)
	{
		pixelTotal += LineScanImage[k];
	}

	return pixelTotal;
}


int32_t calculateNewExposure(LineScan_s *linescan, int32_t targetTotalIntensity)
{
	uint32_t totalIntensity = getTotalIntensity(linescan->image);
	
	previousError = error;
	error = targetTotalIntensity - totalIntensity;
	dError = (error - previousError) / ( (float)linescan->exposure.time * (float)SYSTICK_FREQUENCY );
	errorSum += error * ( (float)linescan->exposure.time / (float)SYSTICK_FREQUENCY );

	newExposure += (int32_t) ((float) error * Kp);

	if (newExposure > MAX_EXPOSURE)
	{
		newExposure = MAX_EXPOSURE;
	}
	else if (newExposure < MIN_EXPOSURE)
	{
		newExposure = MIN_EXPOSURE;
	}

	return newExposure;
}
