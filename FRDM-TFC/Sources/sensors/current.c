/* CurrentSensor.c
 * ==========================================
 * Current signal storage and filtering.
 */

#include "sensors/current.h"
#include "config.h"

/* Import filter co-effs */
const float current_filter_coeffs[] = CURRENT_FILTER_COEFFS;

/* Current signal structs */
MotorCurrent_s I_rl;
MotorCurrent_s I_rr;

void InitCurrentSensors()
{
	/* Initialise signal buffers */
	rbuf_uint16_init(&I_rl.buffer, I_rl.data, CURRENT_FILTER_BUFFER_SIZE);
	rbuf_uint16_init(&I_rr.buffer, I_rr.data, CURRENT_FILTER_BUFFER_SIZE);
	rbuf_uint16_set_mode(&I_rl.buffer, RBUF_UINT16_MODE_OVERWRITE); //Set overwrite mode so that the buffer is never 'full'.
	rbuf_uint16_set_mode(&I_rr.buffer, RBUF_UINT16_MODE_OVERWRITE);
}

/* Convolve filter with most recent samples to determine current value */
#define UINT16_MAX 0xFFFF
int16_t GetCurrentValue(MotorCurrent_s *current)
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
	
	/* Return value */
	if (sum > UINT16_MAX) return UINT16_MAX;
	else if (sum < 0) return 0;
	else return (uint16_t) sum;
}

