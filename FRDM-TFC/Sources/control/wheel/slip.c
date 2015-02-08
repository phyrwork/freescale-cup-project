#include "control/wheel/slip.h"
#include "control/wheel/speed.h"
#include "config.h"

#define SLIP_KP 0
#define SLIP_KI 0
#define SLIP_KD 0

#define NUM_SLIP_CONTROLS 2

WheelSlipControl_s WheelSlipControls[NUM_SLIP_CONTROLS];

void InitWheelSlipControl()
{
	//rear left
	WheelSlipControls[REAR_LEFT].sensor  = &WheelSlipSensors[REAR_LEFT];
	WheelSlipControls[REAR_LEFT].control = &WheelSpeedControls[REAR_LEFT];

	//rear right
	WheelSlipControls[REAR_RIGHT].sensor  = &WheelSlipSensors[REAR_RIGHT];
	WheelSlipControls[REAR_RIGHT].control = &WheelSpeedControls[REAR_RIGHT];
}

void SetWheelSlip(WheelSlipControl_s *slip, float command)
{
	slip->cmd = command;

	UpdateWheelSlip(slip->sensor);
	command = slip->sensor->front->value * (1 + command);

	SetWheelSpeed(slip->control, command);
}
