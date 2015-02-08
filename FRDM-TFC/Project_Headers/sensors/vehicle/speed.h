#ifndef VEHICLE_SPEED_SENSOR_H
#define VEHICLE_SPEED_SENSOR_H

#include "sensors/wheel/speed.h"

typedef struct {
	float value;
	WheelSpeedSensor_s *fl; //pointer to front-left wheel sensor
	WheelSpeedSensor_s *fr; //pointer to front-right wheel sensor
} VehicleSpeedSensor_s;

extern VehicleSpeedSensor_s VehicleSpeedSensor;

void InitVehicleSpeedSensor();
void UpdateVehicleSpeed();

#endif
