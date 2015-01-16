#include "sensors/wheel/speed.h"
#include "sensors/cadence.h"

/* Sensor configurations and data */
WheelSpeed_s WheelSpeeds[NUM_CADENCE_SENSORS];

void InitWheelSpeedSensors()
{
	WheelSpeeds[0].value = 0;
	WheelSpeeds[0].sensor = &CadenceSensors[0];
	WheelSpeeds[0].ratio = 8;
	
	WheelSpeeds[1].value = 0;
	WheelSpeeds[1].sensor = &CadenceSensors[1];
	WheelSpeeds[1].ratio = 8;
}

void UpdateWheelSpeed(WheelSpeed_s *speed)
{
	if (speed->sensor->flag == 0) return; //no new measurement available
	else speed->sensor->flag = 0;         //clear flag

	uint32_t period = speed->sensor->period * speed->ratio;          //revolution period in TPM ticks
	speed->value = (float) speed->sensor->TPM->MOD / (float) period; //revolution frequency in seconds
}
