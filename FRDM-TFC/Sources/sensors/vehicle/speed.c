#include "sensors/vehicle/speed.h"

/* Sensor configurations and data */
VehicleSpeedSensor_s VehicleSpeedSensor;

void InitVehicleSpeedSensor()
{
	VehicleSpeedSensor_s * const speed = &VehicleSpeedSensor;
	
	speed->fl = &WheelSpeedSensors[FRONT_LEFT];
	speed->fr = &WheelSpeedSensors[FRONT_RIGHT];
}

void UpdateVehicleSpeed()
{
	VehicleSpeedSensor_s * const speed = &VehicleSpeedSensor;
	
	//for now just average two front wheel speeds
	speed->value = (speed->fl->value + speed->fr->value) / 2;
}
