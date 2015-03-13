#ifndef VEHICLE_SPEED_CONTROL_H_
#define VEHICLE_SPEED_CONTROL_H_

#include "sensors/vehicle/speed.h"
#include "control/wheel/speed.h"
#include "control/vehicle/slip.h"

typedef struct {
	float cmd;
	VehicleSpeedSensor_s *sensor;
	WheelSpeedControl_s  *left;
	WheelSpeedControl_s  *right;
	PID_s                *pid;
	VehicleSlipControl_s *slip;
} VehicleSpeedControl_s;

extern VehicleSpeedControl_s VehicleSpeedControl;

void InitVehicleSpeedControl();
void SetVehicleSpeed(float command);
float GetVehicleSpeed();

#endif /* VEHICLE_SPEED_CONTROL_H_ */
