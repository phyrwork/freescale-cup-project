/*
 *	SerialFrame.h
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

#ifndef SERIALFRAME_H_
#define SERIALFRAME_H_

#include <stdint.h>

/*	SerialEncapsulate()
 *
 *	@return: length of encapsulated message
 *	
 *	@params:
 *		* msg 		- pointer to incoming message buffer
 *		msgLen		- length to read from incoming message buffer.
 *		* rtnMsg	- pointer to write encapsulated message to
 */
uint16_t SerialEncode(uint8_t * msg, uint16_t msgLen, uint8_t * rtnMsg);

/*	SerialDecapsulate()
 *
 *	@return: length of decapsulated message
 *	
 *	@params:
 *		* msg 		- pointer to incoming message buffer
 *		msgLen		- length to read from incoming message buffer.
 *		* rtnMsg	- pointer to write decapsulated message to
 */
uint16_t SerialDecode(uint8_t * msg, uint16_t msgLen, uint8_t * rtnMsg);

#endif /* SERIALFRAME_H_ */
