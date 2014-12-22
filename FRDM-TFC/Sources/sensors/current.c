/* CurrentSensor.c
 * ==========================================
 * Current signal storage and filtering.
 */

#include "sensors/current.h"
#include "config.h"

/* Import filter co-effs */
const float current_filter_coeffs[] = CURRENT_FILTER_COEFFS;

/* Current signal structs */
MotorCurrent_s MotorCurrent[NUM_MOTORS];

void InitCurrentSensors()
{
	/* Initialise signal buffers */
	for (uint8_t i = 0; i < NUM_MOTORS; ++i)
	{
		MotorCurrent_s *current = &MotorCurrent[i];
		
		rbuf_uint16_init(&current->buffer, current->data, CURRENT_FILTER_BUFFER_SIZE);
		rbuf_uint16_set_mode(&current->buffer, RBUF_UINT16_MODE_OVERWRITE); //Set overwrite mode so that the buffer is never 'full'.
	}
}

/* Convolve filter with most recent samples to determine current value */
#define UINT16_MAX 0xFFFF
float UpdateCurrentValue(MotorCurrent_s *current)
{
	float sum = 0;
	uint32_t elem = current->buffer.rfx; //Find first element position
	
	for (uint8_t s = 0; s < CURRENT_FILTER_ORDER; ++s)
	{
		/* Isolate next sample */
		if (++elem == current->buffer.size) elem = 0; //Wrap around in buffer if at end
		uint16_t sample = current->buffer.buf[s];
		
		/* Accumulate value */
		sum += sample * current_filter_coeffs[s];
	}
	
	/* Convert from 12-bit to actual figure */
	current->value *= 3.3f * 1.2615f; //1.2615f is empirically measured
	return current->value;
}

