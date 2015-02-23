#ifndef TFC_PWM_MOTOR_H_
#define TFC_PWM_MOTOR_H_

#include "support/Types.h"
#include "config.h"

#define MAX_DUTY 0.8

typedef struct {
	volatile uint32_t *fwdCnVReg;
	volatile uint32_t *bwdCnVReg;
} MotorTPM_s;

typedef struct {
	float value;
	const MotorTPM_s *tpm;
} MotorPWM_s;

extern MotorPWM_s MotorPWM[NUM_MOTORS];

void InitMotorPWMControl();
void SetMotorPWM(MotorPWM_s *MotorPWM, float value);
float GetMotorPWM(MotorPWM_s *pwm);
void TFC_SetMotorPWM(float MotorA ,float MotorB);
float TFC_GetMotorPWM(uint8_t channel);

#endif /* TFC_PWM_MOTOR_H_ */
