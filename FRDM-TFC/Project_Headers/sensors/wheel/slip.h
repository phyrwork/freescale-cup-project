#ifndef WHEEL_SLIP_SENSOR_H
#define WHEEL_SLIP_SENSOR_H

#include "sensors/wheel/speed.h"
#include "config.h"

typedef struct {
	float value;
	WheelSpeedSensor_s  *front;
	WheelSpeedSensor_s  *rear;
} WheelSlipSensor_s;

#define WHEEL_SLIP_PAIRS NUM_MOTORS
WheelSlipSensor_s WheelSlipSensors[WHEEL_SLIP_PAIRS];

void InitWheelSlipSensors();
void UpdateWheelSlip(WheelSlipSensor_s *sensor);

#endif
