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
 * Features:
 *  - Data compression by grouping code/value
 *    pairs with similar time stamps within
 *    a definable time tolerance (e.g. 1ms)
 *       Added: November 24, 2014
 */

#include "telemetry/tftp/core.h"
#include "config.h"

/* Import link layer service and add #defines
 * for more portable use */
#include "io/UART.h"
#define SEND(str, len) UART0_Send((str), (len))

/* Import data storage service and add
 * #defines for more portable use */
 //To do: #
#define STORE

/* Import cast_uint8 and time methods */
#include "support/tools.h"

//
// To do: Write Tftp_Init to send frame containing enivronment configuration data
//        for historicak purposes and so that Tftp can self-configure various things.
//
//        The motivation is to be able to return times as 32-bit integer values instead
//        of doing many costly time calculations (divisions!) and let Tftp client
//        calculate times itself.
//
// void Tftp_Init() {}

/* Generate a TFTP frame containing a single code/value
 * pair and push directly to the link layer service. */
int8_t Tftp_Send(uint8_t code, void* value, uint16_t size)
{
	uint8_t buffer[SERIAL_MAX_MSG_SIZE];
	uint16_t w = 0; //Write index.

	/* Generate time stamp to nearest ms */
	getTimestamp(buffer);
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
	static uint16_t w = 0;
	
	/* Time data */
	static float frameTime = 0;
	       float  thisTime = getTime();

	if ( /* Check time difference; if too large */
		 thisTime - frameTime > TFTP_TIMESTAMP_TOLERANCE ||
		 /* Check there is enough space left in the buffer */
		 (sizeof buffer) - w < (size + sizeof code)
	   ) {
		
		/* Send previous frame */
		if (w > 0) { //Don't send a blank frame
			uint8_t error;
			if ( (error = SEND(buffer, w)) )
				return error;
		}

		/* Start a new one */
		frameTime = thisTime;  // Save new timestamp...
		getTimestamp(buffer); // ...write to frame.
		w = 4;                 // Set write index.
	}

	/* Copy sample to frame */
	buffer[w] = code;
	cast_uint8(buffer + ++w, value, size); 
	w += size;

	return 0;
}








