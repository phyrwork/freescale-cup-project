/* tools.c
 * ==========================================
 * Misc. methods for doing useful stuff.
 */

#include "support/tools.h"

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

inline uint32_t getTime() { return TICKER; }

/* Return 4 byte (single) time stamp to *time */
void getTimestamp(void* time)
{	
	uint32_t t = getTime();
	cast_uint8((uint8_t*)time, &t, sizeof t);
	return;
}
