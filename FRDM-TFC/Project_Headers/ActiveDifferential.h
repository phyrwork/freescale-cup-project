/*
 * ActiveDifferential.h
 *
 *  Created on: Feb 21, 2014
 *      Author: Matt
 */

#ifndef ACTIVEDIFFERENTIAL_H_
#define ACTIVEDIFFERENTIAL_H_

#include "typedefs_structs\carState_s.h"
#include "TFC\TFC.h"

float getActiveDifferentialModifier(carState_s* carState, int8_t channel);

#endif /* ACTIVEDIFFERENTIAL_H_ */
