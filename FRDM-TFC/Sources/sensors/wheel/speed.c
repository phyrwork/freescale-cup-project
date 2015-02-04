#include "sensors/wheel/speed.h"
#include "sensors/cadence.h"
#include "support/ARM_SysTick.h"
#include "config.h"

//time-out config
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

uint32_t FilterWheelPeriod(WheelSpeedSensor_s *speed)
{
	float sum = 0;
	uint32_t elem = speed->sensor->buffer.pos;
	
	float const coeffs[WSPEED_FILTER_ORDER] = WSPEED_FILTER_COEFFS;
	
	//sum coefficients
	for (uint8_t s = 0; s < WSPEED_FILTER_ORDER; ++s)
	{
		sum += (speed->sensor->buffer.data[elem] * coeffs[s]);
		if (++elem == CADENCE_BUFFER_SIZE) elem = 0; //wrap around in buffer if at end
	}
	
	return sum;
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

	float period = FilterWheelPeriod(speed) * speed->ratio; //revolution period in TPM ticks
	speed->value = period ? ( (float) speed->sensor->TPM->MOD / (float) period ) : 0; //revolution frequency in seconds
}
