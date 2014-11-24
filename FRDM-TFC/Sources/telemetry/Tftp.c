/* Tftp.c
 * =========================================
 * Team Freescalextric Telemetry Protocol
 * -----------------------------------------
 * When supported by sufficient link layer
 * or data storage services TFTP facilitates
 * the collection of various data from the
 * FRDM-TFC system.
 *
 *      Author: Connor Newton
 *        Date: November 19, 2014
 *
 * Planned features:
 *  - Data compression by grouping code/value
 *    pairs with similar time stamps within
 *    a definable time tolerance (e.g. 1ms)
 */

#include "config.h"

/* Import link layer service and add #defines
 * for more portable use */
#include "io/UART.h"
#define SEND(str, len) UART0_Send((str), (len))

/* Import data storage service and add
 * #defines for more portable use */
 //To do: #
#define STORE

/* #include SysTick module */
#include "support/ARM_SysTick.h"
#define TICKER (TFC_Ticker[TFTP_TICKER])
#define TIME   ((float) TICKER/ (float) SYSTICK_FREQUENCY)

/* Get constituent bytes of a variable */
void cast_uint8(uint8_t* out, void* var, uint16_t size) {
	
	/* Cast var as uint8_t and copy bytes to *out */
	for ( ; size > 0; size--, ++out)
		*out = ((uint8_t *)var)[size - 1];
	return;
}


/* Return 4 byte (single) time stamp to *time */
void getTimeStamp(uint8_t * time) {
	
	float t = TIME;
	cast_uint8(time, &t, sizeof t);
	return;
}

/* Generate a TFTP frame containing a single code/value
 * pair and push directly to the link layer service. */
int8_t Tftp_Send(uint8_t code, void* value, uint16_t size)
{
	uint8_t buffer[SERIAL_MAX_MSG_SIZE];
	uint8_t w = 0; //Write index.

	/* Generate time stamp to nearest ms */
	getTimeStamp(buffer);
	w = 4;

	/* Add key and value to frame */
	buffer[w] = code;
	cast_uint8(buffer + ++w, value, size);
	w += size;

	/* Push value to link layer service */
	SEND(buffer, w);

	return 0;
}

int8_t Tftp_Push(uint8_t code, void* value, uint16_t size)
{
	/* Static frame storage */
	static uint8_t buffer[SERIAL_MAX_MSG_SIZE];
	static uint8_t w = 0;

	/* Time data */
	static float frameTime = 0;
	       float  thisTime = TIME;

	if ( /* Check time difference; if too large */
		 thisTime - frameTime > TFTP_TIMESTAMP_TOLERANCE ||
		 /* Check there is enough space left in the buffer */
		 (sizeof buffer) - w < size
	   ) {
		
		/* Send previous frame */
		if (w > 0) { //Don't send a blank frame
			uint8_t error;
			if ( (error = SEND(buffer, w)) )
				return error;
		}

		/* Start a new one */
		frameTime = thisTime;  // Save new timestamp...
		getTimeStamp(buffer); // ...write to frame.
		w = 4;                 // Set write index.
	}

	/* Copy sample to frame */
	buffer[w] = code;
	cast_uint8(buffer + ++w, value, size); 
	w += size;

	return 0;
}








