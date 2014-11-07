/*
 * AutoExposure.h
 *
 *  Created on: Dec 10, 2013
 *      Author: admin
 */

#ifndef AUTOEXPOSURE_H_
#define AUTOEXPOSURE_H_

#include "support/Types.h"

uint32_t getTotalIntensity(volatile uint16_t* LineScanImage);
int32_t calculateNewExposure(int32_t totalIntensity, int32_t targetTotalIntensity);

#endif /* AUTOEXPOSURE_H_ */
