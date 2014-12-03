/* tools.c
 * ==========================================
 * Misc. methods for doing useful stuff.
 */

 /* Get constituent bytes of a variable */
void cast_uint8(uint8_t* out, void* var, uint16_t size)
{
	/* Cast var as uint8_t and copy bytes to *out */
	for ( ; size > 0; size--, ++out)
		*out = ((uint8_t *)var)[size - 1];
	return;
}

/* Get current system up-time */
#include "support/ARM_SysTick.h"
#include "config.h"
#define TICKER (TFC_Ticker[UPTIME_TICKER])
#define TIME   ((float) TICKER / (float) SYSTICK_FREQUENCY)

inline float getTime() { return TIME; }

/* Return 4 byte (single) time stamp to *time */
void getTimestamp(uint8_t * time)
{	
	float t = getTime;
	cast_uint8(time, &t, sizeof t);
	return;
}