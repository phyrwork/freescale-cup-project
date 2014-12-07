#ifndef TFC_UART_H_
#define TFC_UART_H_

#include <stdint.h>
#include "support/rbuf.h"

void     UART0_Init();
void     UART0_ConfigureDataRate(int sysclk, int baud);
void     UART0_ArmIRQ();
void     UART0_DisarmIRQ();
void     UART0_ArmDMA();
int8_t   UART0_Send(uint8_t * msg, uint16_t size);
int8_t   UART0_SendRaw(uint8_t * msg, uint16_t size);

extern rbuf_s RxBuffer;
extern rbuf_s TxBuffer;

#endif /* TFC_UART_H_ */
