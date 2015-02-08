#include "sensors/vehicle/slip.h"

VehicleSlipSensor_s VehicleSlipSensor;

void InitVehicleSlipSensor()
{
	VehicleSlipSensor_s * const slip = &VehicleSlipSensor;
	
	slip->front = &VehicleSpeedSensor;
	slip->rl = &WheelSpeedSensors[REAR_LEFT];
	slip->rr = &WheelSpeedSensors[REAR_RIGHT];
}

void UpdateVehicleSlip()
{
	VehicleSlipSensor_s * const slip = &VehicleSlipSensor;
	
	//update sensors
	UpdateVehicleSpeed();
	UpdateWheelSpeed(slip->rl);
	UpdateWheelSpeed(slip->rr);

	//avoid division by zero
	if (slip->front->value != 0.0f) //if non-zero divisor
	{
		//calculate average rear wheel speed
		float speed = (slip->rl->value + slip->rr->value) / 2;
		
		//calculate slip;
		slip->value = ( speed / slip->front->value ) - 1;
	}
	else slip->value = 0; //otherwise assume zero-slip (even though it's Inf)
}
