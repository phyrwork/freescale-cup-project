/* endpoint.h
 * =====================================================
 * Endpoint templates for TFTP attributes.
 * -----------------------------------------------------
 * Author: Connor Newton
 *   Date: November 24, 2014
 */

#define TFTP_ENDPOINT_PUSH(ID, CODE, TYPE, SIZE)				\
																\
int8_t Tftp_ ## ID ## _Push(uint8_t CODE, void* data) {			\   
	/* Push */													\
	return Tftp_Push(CODE, data, (sizeof ( TYPE ) * SIZE ));	\
}																\