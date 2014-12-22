#ifndef TFC_PWM_MOTOR_H_
#define TFC_PWM_MOTOR_H_

#include "support/Types.h"

void InitMotorControl();
void TFC_SetMotorPWM(float MotorA ,float MotorB);
float TFC_GetMotorPWM(uint8_t channel);

#endif /* TFC_PWM_MOTOR_H_ */
