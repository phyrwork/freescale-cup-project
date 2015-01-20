#ifndef MOTOR_SPEED_CONTROL_H_
#define MOTOR_SPEED_CONTROL_H_

#include "sensors/cadence.h"
#include "control/motor/pwm.h"
#include "support/PID.h"

typedef struct {
	float cmd;
	float value;
	CadenceSensor_s *sensor;
	float ratio;
	MotorPWM_s *pwm;
	PID_s *pid;
} MotorSpeed_s;

void InitMotorSpeedControl();
void UpdateMotorSpeed(MotorSpeed_s *speed);
void SetMotorSpeed(MotorSpeed_s *speed, float command);

extern MotorSpeed_s MotorSpeeds[NUM_MOTORS];

#endif /* SPEEDCONTROL_H_ */
