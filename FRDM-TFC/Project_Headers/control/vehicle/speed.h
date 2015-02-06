#ifndef VEHICLE_SPEED_CONTROL_H_
#define VEHICLE_SPEED_CONTROL_H_

#include "control/wheel/speed.h"

typedef struct {
	float cmd;
	WheelSpeedControl_s *left;
	WheelSpeedControl_s *right;
} VehicleSpeedControl_s;

extern VehicleSpeedControl_s VehicleSpeedControl;

void InitVehicleSpeedControl();
void SetVehicleSpeed(float command);
float GetVehicleSpeed(float modifier);

#endif /* VEHICLE_SPEED_CONTROL_H_ */
