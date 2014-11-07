/*
 * main.h
 *
 *  Created on: Feb 17, 2014
 *      Author: Matt
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "derivative.h" /* include peripheral declarations */
#include "TFC\TFC.h"
#include "AutoExposure.h"
#include "LineDetection.h"
#include "SpeedControl.h"
#include "SpeedSensor.h"
#include "SteeringControl.h"
#include "TargetSpeedControl.h"
#include "ActiveDifferential.h"
//#include "i2c.h"
#include "MMA8451Q.h"
#include "typedefs_structs\carState_s.h"


void TFC_Init(carState_s* carState);
void TFC_Task();
int main(void);
void heartbeat();
void evaluateUARTorSpeed(carState_s* carState);
void evaluateMotorState(carState_s* carState);
void rawFocussingMode();
void derivativeFocussingMode();
void speedTestMode(carState_s* carState);
void servoAlignment();;
void lineFollowingMode(carState_s* carState);
float targetSpeedAverage(float targetSpeed);


#endif /* MAIN_H_ */
