#ifndef WHEEL_SPEED_CONTROL_H_
#define WHEEL_SPEED_CONTROL_H_

#include "sensors/cadence.h"
#include "control/motor/pwm.h"
#include "sensors/wheel/speed.h"
#include "support/PID.h"

typedef struct {
	float cmd;
	WheelSpeedSensor_s *sensor;
	MotorPWM_s *pwm;
	PID_s *pid;
} WheelSpeedControl_s;

void InitWheelSpeedControl();
void SetWheelSpeed(WheelSpeedControl_s *speed, float command);

extern WheelSpeedControl_s WheelSpeedControls[NUM_MOTORS];

#endif /* WHEEL_SPEED_CONTROL_H_ */
