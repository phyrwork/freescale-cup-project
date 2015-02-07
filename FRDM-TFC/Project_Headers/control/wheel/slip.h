#ifndef WHEEL_SLIP_CONTROL_H
#define WHEEL_SLIP_CONTROL_H

#include "sensors/wheel/slip.h"
#include "support/PID.h"

typedef struct {
	float                cmd;
	WheelSlipSensor_s   *sensor;
	WheelSpeedControl_s *control;
} WheelSlipControl_s;

#endif