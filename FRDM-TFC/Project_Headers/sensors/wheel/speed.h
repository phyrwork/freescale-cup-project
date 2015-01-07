#ifndef WHEEL_SPEED_SENSOR_H
#define WHEEL_SPEED_SENSOR_H

#include "sensors/cadence.h"

typedef struct {
	float value;
	CadenceSensor_s *sensor; //either direct pointer to a hall sensor
	float ratio; //period ratio of wheel:sensor (wheel:motor = 8:1??)
} WheelSpeed_s;

#endif