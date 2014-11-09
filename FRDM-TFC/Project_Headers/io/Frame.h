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

/*	Message length defines for buffer sizing.
 */
#define FR_CTRL_SIZE		2
#define FR_CHK_SIZE			2
#define FR_MAX_UNENC_SIZE	(SERIAL_MAX_MSG_SIZE + FR_CTRL_SIZE + FR_CHK_SIZE)
#define FR_MAX_ENC_SIZE		(uint32_t)(1 + (FR_MAX_UNENC_SIZE * 1.04f))

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
