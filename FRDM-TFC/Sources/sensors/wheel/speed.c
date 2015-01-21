#include "sensors/wheel/speed.h"
#include "sensors/cadence.h"

/* Sensor configurations and data */
WheelSpeedSensor_s WheelSpeedSensors[NUM_CADENCE_SENSORS];

void InitWheelSpeedSensors()
{
	WheelSpeedSensors[REAR_LEFT].value = 0;
	WheelSpeedSensors[REAR_LEFT].sensor = &CadenceSensors[1];
	WheelSpeedSensors[REAR_LEFT].ratio = 8;
	
	WheelSpeedSensors[REAR_RIGHT].value = 0;
	WheelSpeedSensors[REAR_RIGHT].sensor = &CadenceSensors[0];
	WheelSpeedSensors[REAR_RIGHT].ratio = 8;
}

void UpdateWheelSpeed(WheelSpeedSensor_s *speed)
{
	//if (speed->sensor->flag == 0) return; //no new measurement available
	//else speed->sensor->flag = 0;         //clear flag

	uint32_t period = speed->sensor->period * speed->ratio; //revolution period in TPM ticks	
	speed->value = period ? ( (float) speed->sensor->TPM->MOD / (float) period ) : 0; //revolution frequency in seconds
}
