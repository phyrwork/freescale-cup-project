/* Collector.c
 * =====================================================
 * Periodic Interrupt Timer (PIT) driven data collection
 * service.
 * 
 * Written for use with TFTP for FRDM-TFC.
 * 
 * Author: Connor Newton
 *   Date: November 24, 2014
 * -----------------------------------------------------
 * Any data to be collected with Collector should be
 * shared via its modules' header file.
 *
 * Collector only facilitates the collection of data
 * at specified intervals from other modules - the
 * actual data handling is performed by the TFTP
 * attribute's endpoint function.
 */

#include "telemetry/Collector.h"

/* Import Tftp */
#include "telemetry/tftp/tftp.h"

/* Import timer */
#include "support/ARM_SysTick.h"
#include "config.h"
#define TICKER TFC_Ticker[COLLECTOR_TICKER]

/* Include headers to access shared data */
//SOMETHING SOMETHING #INCLUDES

/* Define CollectorItems */
CollectorItem items[] = {
	/* [0] = */ {
		/* data = */      0,
		/* endpoint = */  0,
		/* frequency = */ 0,
		/* period = */    0,
		/* counter = */   0
	}
};
#define NUM_ITEMS ( (sizeof items) / (sizeof (CollectorItem)) )

/* PIT1 (Periodic Interrupt Timer) Interrupt Routine */
void PIT1_IRQHandler()
{
	/* Increment all counters */
	for (uint32_t i = 0; i < NUM_ITEMS; i++)
		items[i].counter++;
}

/* Collector initialization routine */
void Collector_Init()
{

	/* Initialize CollectorItems */
	for (uint32_t i = 0; i < NUM_ITEMS; i++ )
	{
		/* Calculate period in ticks; initialize counter */
		items[i].period = SYSTICK_FREQUENCY/items[i].frequency;
		items[i].counter = 0;
	}

	/* Initialize PIT1 */
	// SOMETHING SOMETHING REGISTERS
}

CollectorStatus* Collector()
{
	/* Endpoint error reporting */
	static int8_t errors[NUM_ITEMS];
	static CollectorStatus status = { COLLECTOR_ENDPOINT_OK, errors };
	
	status.flag = COLLECTOR_ENDPOINT_OK; // Reset errors flag

	/* Handle CollectorItems */
	for (uint32_t i = 0; i < NUM_ITEMS; i++)
	{
		/* If CollectorItem counter has reached 
		   or exceeded period in ticks */
		if (items[i].counter >= items[i].period)
		{
			/* Push data onto endpoint */
			errors[i] = (*(items[i].endpoint))(items[i].data);

			/* Set CollectorStatus error flag if appropriate */
			if (errors[i]) status.flag = COLLECTOR_ENDPOINT_ERROR;
		}
	}
	
	/* Finished; return pointer to endpoint results */
	return &status;
}
