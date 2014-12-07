/* HC05.h
 * =====================
 * Author: Connor Newton
 */

#ifndef TFC_HC05_
#define TFC_HC05_

/*
typedef struct {
	uint32_t baud;
	uint8_t  stop;
	uint8_t  parity;
} HC05_Config;
*/
	
void HC05_Init();
int8_t HC05_Config(/*HC05_Config* config*/);

#endif //TFC_HC05_