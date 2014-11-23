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

/* Import link layer service and add #defines
 * for more portable use */
#include "io/UART.h"
#define SEND(str, len) UART0_Send((str), (len)

/* Import data storage service and add
 * #defines for more portable use */
 //To do: #
#define STORE

/* #include SysTick module */
#include "support/ARM_SysTick.h"
#define TICKER (TFC_Ticker[TFTP_TICKER])
#define ms float(1000/SYSTICK_FREQUENCY)

/* Storage for constructing frames */
uint8_t buffer[SERIAL_MAX_MSG_SIZE];

/* Get constituent bytes of a variable */
inline void cast_uint8(uint8_t* out, void* var, size_t size) {
	
	/* Cast var as uint8_t and copy bytes to *out */
	for (;size > 0; size--; ++out)
		*out = ((uint8_t *)var)[size - 1];
	return;
}


/* Return 4 byte (single) time stamp to *time */
inline void getTimeStamp(uint8_t * time) {
	
	float t = TICKER * ms;
	cast_uint8(time, t, sizeof t);
	return;
}

/* Generate a TFTP frame containing a single code/value
 * pair and push directly to the link layer service. */
int8_t TftpSend(uint8_t key, void* value, size_t size) {

	uint8_t w = 0; //Write index.

	/* Generate time stamp to nearest ms */
	getTimeStamp(buffer);
	w = 4;

	/* Add key and value to frame */
	buffer[w] = key;
	cast_uint8(++w, value, size);
	w += size;

	/* Push value to link layer service */
	SEND(buffer, w);

	return 0;
}








