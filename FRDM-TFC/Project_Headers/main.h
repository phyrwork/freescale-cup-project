/*
 * main.h
 *
 *  Created on: Feb 17, 2014
 *      Author: Matt
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "devices/MKL25Z4.h" /* include peripheral declarations */
#include "devices/arm_cm0.h"
#include "devices/CrystalClock.h"
#include "devices/TFC_SHIELD.h"
#include "devices/MMA8451Q.h"
#include "devices/HC05.h"
//#include "io/I2C.h"
#include "io/UART.h"
#include "io/DMA.h"
#include "io/ADC.h"
#include "sensors/camera/LineScanCamera.h"
#include "sensors/camera/AutoExposure.h"
#include "sensors/camera/LineDetection.h"
#include "sensors/wheel/slip.h"
#include "sensors/motor/current.h"
#include "control/steering/Steering.h"
#include "control/motor/pwm.h"
#include "control/motor/torque.h"
#include "control/steering/Servo.h"
#include "control/TargetSpeed.h"
#include "control/Differential.h"
#include "support/ARM_SysTick.h"
#include "support/carState_s.h"
#include "telemetry/Collector.h"
#include "support/Profiler.h"
#include "config.h"

uint32_t AnyTaskRequest();
uint32_t PollTaskRequest(uint32_t index);
void SetTaskRequest(uint32_t index);
void ClearTaskRequest(uint32_t index);
void TaskRequest_Init();
void UpdateTaskRequests();

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
