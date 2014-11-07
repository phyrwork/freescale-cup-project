/*
 * MMA8451Q.h
 *
 *  Created on: Feb 28, 2014
 *      Author: Matt
 */

#ifndef MMA8451Q_H_
#define MMA8451Q_H_

#include "I2C.h"
#include "TFC\TFC.h"

void Init_MMA8451Q();
void zeroAccelerometers();
float getXAcc();
float getYAcc();
float getZAcc();
int16_t getRawXAcc();
int16_t getRawYAcc();
int16_t getRawZAcc();
uint8_t getID();


#endif /* MMA8451Q_H_ */
