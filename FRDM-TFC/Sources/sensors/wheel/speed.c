#include "sensors/wheel/speed.h"
#include "sensors/cadence.h"

/* Sensor configurations and data */
WheelSpeedSensor_s WheelSpeedSensors[NUM_CADENCE_SENSORS];

void InitWheelSpeedSensors()
{
	WheelSpeedSensors[0].value = 0;
	WheelSpeedSensors[0].sensor = &CadenceSensors[0];
	WheelSpeedSensors[0].ratio = 8;
	
	WheelSpeedSensors[1].value = 0;
	WheelSpeedSensors[1].sensor = &CadenceSensors[1];
	WheelSpeedSensors[1].ratio = 8;
}

void UpdateWheelSpeed(WheelSpeedSensor_s *speed)
{
	//if (speed->sensor->flag == 0) return; //no new measurement available
	//else speed->sensor->flag = 0;         //clear flag

	uint32_t period = speed->sensor->period * speed->ratio; //revolution period in TPM ticks	
	speed->value = period ? ( (float) speed->sensor->TPM->MOD / (float) period ) : 0; //revolution frequency in seconds
}
