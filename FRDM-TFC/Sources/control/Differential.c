
#include "control/Differential.h"

#define KP 0.001f

float getActiveDifferentialModifier(carState_s* carState, int8_t channel)
{
	if(channel == 0)
	{
		return (1.0f + ((float)carState->lineCenter * KP));
	}
	else
	{
		return (1.0f - ((float)carState->lineCenter * KP));
	}
}
