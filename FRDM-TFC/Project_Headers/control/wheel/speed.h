#ifndef MOTOR_SPEED_CONTROL_H_
#define MOTOR_SPEED_CONTROL_H_

#include "sensors/cadence.h"
#include "control/motor/pwm.h"
#include "support/PID.h"

typedef struct {
	float cmd;
	float value;
	WheelSpeedSensor_s *sensor;
	MotorPWM_s *pwm;
	PID_s *pid;
} WheelSpeedControl_s;

void InitWheelSpeedControl();
void SetWheelSpeed(WheelSpeedControl_s *speed, float command);

extern WheelSpeedControl_s WheelSpeedControls[NUM_MOTORS];

#endif /* SPEEDCONTROL_H_ */
