/* endpoint.h
 * =====================================================
 * Endpoint templates for TFTP attributes.
 * -----------------------------------------------------
 * Author: Connor Newton
 *   Date: November 24, 2014
 */

#ifndef TFTP_ENDPOINT
#define TFTP_ENDPOINT

/* Endpoint symbol constructor */
#define TFTP_ENDPOINT_SYM(ID) ID ## _ENDPOINT

/* Endpoint method constructor */
#define TFTP_ENDPOINT_PUSH(SYM, CODE, TYPE, SIZE)				\
																\
int8_t SYM (void* data) {										\
	return Tftp_Push( CODE , data , (sizeof ( TYPE )) * SIZE );	\
}																\

#endif //TFTP_ENDPOINT



