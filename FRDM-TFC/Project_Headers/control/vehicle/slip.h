#ifndef VEHICLE_SLIP_CONTROL_H
#define VEHICLE_SLIP_CONTROL_H

#include "sensors/vehicle/slip.h"
#include "control/motor/torque.h"
#include "support/PID.h"

typedef struct {
	float                cmd;
	VehicleSlipSensor_s  *sensor;
	PID_s                *pid;
	struct {
		MotorTorque_s    *left;
		MotorTorque_s    *right;
	} control;
} VehicleSlipControl_s;

VehicleSlipControl_s VehicleSlipControl;

#endif
