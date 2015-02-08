#ifndef VEHICLE_SLIP_SENSOR_H
#define VEHICLE_SLIP_SENSOR_H

#include "sensors/vehicle/speed.h"
#include "sensors/wheel/speed.h"

typedef struct {
	float value;
	VehicleSpeedSensor_s *front; //pointer to vehicle speed sensor
	WheelSpeedSensor_s   *rl;    //pointer to rear left wheel speed sensor
	WheelSpeedSensor_s   *rr;    //pointer to rear right wheel speed sensor
} VehicleSlipSensor_s;

extern VehicleSlipSensor_s VehicleSlipSensor;

void InitVehicleSlipSensor();
void UpdateVehicleSlip();

#endif
