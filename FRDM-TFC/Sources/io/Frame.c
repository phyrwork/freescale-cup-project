/*
 *	SerialFrame.c
 *
 *  Created on: Oct 31, 2014
 *      Author: Connor Newton
 *	
 *	Encapsulates and decapsulates byte sequences
 *	for transmission across a serial
 *	connection.
 *	
 *	Integrity checking with CRC16 and
 *	convenient, unambiguous frame delimiting
 *	with Consistent Overhead Byte Stuffing.
 */

#include "io/Frame.h"

#include <stdint.h>
#include "support/crc16.h"
#include "support/cobs.h"
#include "config.h"

/*	
 *
 *	
 */
#define CONTROLSIZE	2
#define CHECKSIZE	2
#define BUFFERSIZE	SERIAL_MAX_MSG_SIZE + CONTROLSIZE + CHECKSIZE
// MAXMSGLEN defined in Config.h. VALIDITY OF MESSAFE LENGTH SHOULD BE CHECKED ELSEWHERE.

/*	Message Buffers */
uint8_t buffer[BUFFERSIZE + 1];

/*	SerialEncapsulate()
 *
 *	@return: length of encapsulated message
 *	
 *	@params:
 *		* msg 		- pointer to incoming message buffer
 *		msgLen		- length to read from incoming message buffer.
 *		* rtnMsg	- pointer to write encapsulated message to
 */
uint16_t SerialEncode(uint8_t * msg, uint16_t msgLen, uint8_t * rtnMsg) {

	uint16_t r; //read pos.
	uint16_t w; //write pos.
	uint16_t c; //checksum

	/* Populate encapuslation buffer with control data and message */

	buffer[w++] = ((uint8_t *)&msgLen)[0]; //Write byte-wise, MSB then LSB
	buffer[w++] = ((uint8_t *)&msgLen)[1]; //(Casting to uint16_t incompatible with 32-bit alignment)

	for(r = 0; r < msgLen; r++) buffer[w++] = msg[r]; //Copy message to buffer

	c = CRC16(CRC16_FIRSTPASS, buffer, w); //Perform checksum on buffer up to this point
	buffer[w++] = ((uint8_t *)&c)[0]; //and write byte-wise as before.
	buffer[w++] = ((uint8_t *)&c)[1];


	/* Encode with COBS and return */
	return cobs_encode(buffer, w, rtnMsg);
}

/*	SerialDecapsulate()
 *
 *	@return: length of decapsulated message
 *	
 *	@params:
 *		* msg 		- pointer to incoming message buffer
 *		msgLen		- length to read from incoming message buffer.
 *		* rtnMsg	- pointer to write decapsulated message to
 */
uint16_t SerialDecode(uint8_t * msg, uint16_t msgLen, uint8_t * rtnMsg) {
	
	uint16_t r; //read pos.
	uint16_t w; //write pos.
	uint16_t c; //checksum
	uint16_t l; //decapsulated message length

	/* Decode with COBS */
	r = cobs_decode(msg, msgLen, buffer);

	/* Pop checksum and validate message */
	((uint8_t *)&c)[1] = buffer[--r];
	((uint8_t *)&c)[0] = buffer[--r];
	if ( c != CRC16(CRC16_FIRSTPASS, buffer, r) ) return 0; //if checksum fails: message invalid; return false

	r = 0;
	/* Pop length */
	((uint8_t *)&l)[0] = buffer[r++];
	((uint8_t *)&l)[1] = buffer[r++];

	/* Copy message to location provided */
	for(w = 0; w < l; w++) rtnMsg[w] = buffer[r++];

	return l;
}

#undef CONTROLSIZE
#undef CHECKSIZE
#undef BUFFERSIZE
