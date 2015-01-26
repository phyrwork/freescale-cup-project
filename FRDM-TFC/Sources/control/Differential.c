
#include "control/Differential.h"

#define DIFF_KP 0.001f

float getSpeedDiffModifier(int8_t center)
{
	return (float)center * DIFF_KP;
}
