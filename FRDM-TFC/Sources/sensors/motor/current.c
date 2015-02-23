/* CurrentSensor.c
 * ==========================================
 * Current signal storage and filtering.
 */

#include "sensors/motor/current.h"
#include "config.h"

/* Current signal structs */
MotorCurrent_s MotorCurrent[NUM_MOTORS];

void InitCurrentSensors()
{
	//uhm
}

/* Convolve filter with most recent samples to determine current value */
#define UINT16_MAX 0xFFFF
float UpdateCurrentValue(MotorCurrent_s *current)
{
	float sum = 0;
	uint32_t elem = current->buffer.pos;
	
	float const coeffs[CURRENT_FILTER_ORDER] = CURRENT_FILTER_COEFFS;
	
	//sum coefficients
	for (uint8_t s = 0; s < CURRENT_FILTER_ORDER; ++s)
	{
		sum += (current->buffer.data[elem] * coeffs[s]);
		if (++elem == CURRENT_FILTER_BUFFER_SIZE) elem = 0; //wrap around in buffer if at end
	}
	
	/* Convert from 12-bit to actual figure */
	current->value = sum * 0.002111355311355; //3.3 (V) / 4095 (12-bit) * ('14 - 2.62)('15 - 1.2615) (empirically measured)
	return current->value;
}

