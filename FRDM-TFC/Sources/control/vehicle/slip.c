#include "control/vehicle/slip.h"
#include "config.h"

#define VSLIP_KP 0
#define VSLIP_KI 0
#define VSLIP_KD 0

VehicleSlipControl_s VehicleSlipControl;
static PID_s pid;

void InitVehicleSlipControl()
{
	VehicleSlipControl_s * const slip = &VehicleSlipControl;
	
	slip->sensor = &VehicleSlipSensor;
	slip->control.left = &MotorTorque[REAR_LEFT];
	slip->control.right = &MotorTorque[REAR_RIGHT];
	
	//init pid
	slip->pid = &pid;
	slip->pid->Kp = VSLIP_KP;
	slip->pid->Ki = VSLIP_KI;
	slip->pid->Kd = VSLIP_KD;
	slip->pid->time = 0;
	slip->pid->value = 0;
	slip->pid->value_max = 0.007;
	slip->pid->value_min = -0.007;
}

void SetVehicleSlip(float command)
{
	VehicleSlipControl_s * const slip = &VehicleSlipControl;
	
	slip->cmd = command;

	UpdateVehicleSlip();
	
	UpdatePID(slip->pid, command, slip->sensor->value);
	SetMotorTorque(slip->control.left, slip->pid->value);
	SetMotorTorque(slip->control.right, slip->pid->value);
}
