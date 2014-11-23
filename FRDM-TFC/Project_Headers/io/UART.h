#ifndef TFC_UART_H_
#define TFC_UART_H_

#include "io/RingBuffer.h"

void     UART0_Init();
void     UART0_Process();
int8_t   UART0_Send(uint8_t * msg, uint16_t size);
uint16_t UART0_Receive(uint8_t * msg);

extern RingBuffer RxBuffer;
extern RingBuffer TxBuffer;

#endif /* TFC_UART_H_ */
