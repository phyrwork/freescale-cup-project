#include "sensors/wheel/speed.h"
#include "sensors/cadence.h"
#include "support/ARM_SysTick.h"

//time-out config
#include "config.h"
#define TICKER TFC_Ticker[UPTIME_TICKER]
#define SENSOR_TIMEOUT_TICKS (  SYSTICK_FREQUENCY * 0.01f )

/* Sensor configurations and data */
WheelSpeedSensor_s WheelSpeedSensors[NUM_CADENCE_SENSORS];

void InitWheelSpeedSensors()
{
	//rear left
	WheelSpeedSensors[REAR_LEFT].value = 0;
	WheelSpeedSensors[REAR_LEFT].sensor = &CadenceSensors[1];
	WheelSpeedSensors[REAR_LEFT].ratio = 8;
	
	//rear right
	WheelSpeedSensors[REAR_RIGHT].value = 0;
	WheelSpeedSensors[REAR_RIGHT].sensor = &CadenceSensors[0];
	WheelSpeedSensors[REAR_RIGHT].ratio = 8;
}

void UpdateWheelSpeed(WheelSpeedSensor_s *speed)
{
	uint32_t ref = TICKER; //'freeze' timer 
	
	//if no new measurement available
	if (speed->sensor->flag == 0)
	{
		 //if sensor not triggered in a while
		if (ref - speed->timeout > SENSOR_TIMEOUT_TICKS) speed->value = 0; //assume car is stationary
		return;
	}
	
	speed->sensor->flag = 0; //clear flag
	speed->timeout = ref;    //take time reference

	uint32_t period = speed->sensor->period * speed->ratio; //revolution period in TPM ticks	
	speed->value = period ? ( (float) speed->sensor->TPM->MOD / (float) period ) : 0; //revolution frequency in seconds
}
