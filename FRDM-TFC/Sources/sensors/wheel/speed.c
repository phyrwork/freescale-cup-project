#include "sensors/wheel/speed.h"

void UpdateWheelSpeed(WheelSpeed_s *speed)
{
	if (speed->sensor->event == 0) return; //no new measurement available
	else speed->sensor->event = 0;         //clear flag

	uint32_t period = speed->sensor->period * speed->ratio; //revolution period in TPM ticks
	speed->value = (float) speed->sensor->TPM->MOD / (float) period; //revolution frequency in seconds
}