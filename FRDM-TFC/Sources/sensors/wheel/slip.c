#include "sensors/wheel/slip.h"
#include "sensors/wheel/speed.h"
#include "config.h"

/*
typedef struct {
	float value;
	WheelSpeedSensor_s  *front;
	WheelSpeedSensor_s  *rear;
} WheelSlipSensor_s
*/

#define WHEEL_SLIP_PAIRS NUM_MOTORS
WheelSlipSensor_s WheelSlipSensors[WHEEL_SLIP_PAIRS];

void InitWheelSlipSensors()
{
	//rear left
	WheelSlipSensors[REAR_LEFT].value = 0;
	//WheelSlipSensors[REAR_LEFT].front = &WheelSpeedSensors[FRONT_LEFT];
	WheelSlipSensors[REAR_LEFT].rear  = &WheelSpeedSensors[REAR_LEFT];

	//rear right
	WheelSlipSensors[REAR_RIGHT].value = 0;
	//WheelSlipSensors[REAR_RIGHT].front = &WheelSpeedSensors[FRONT_RIGHT];
	WheelSlipSensors[REAR_RIGHT].rear  = &WheelSpeedSensors[REAR_RIGHT];
}

void UpdateWheelSlip(WheelSlipSensor_s *sensor)
{
	UpdateWheelSpeed(sensor->front);
	UpdateWheelSpeed(sensor->rear);

	//avoid division by zero
	if (sensor->front->value) //if non-zero divisor
	{
		//calculate slip;
		sensor->value = ( sensor->rear->value / sensor->front->value ) - 1;
	}
	else sensor->value = 0; //otherwise assume zero-slip
}