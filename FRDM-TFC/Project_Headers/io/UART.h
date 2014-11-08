#ifndef TFC_UART_H_
#define TFC_UART_H_

#include "io/RingBuffer.h"

void TFC_InitUARTs();
void TFC_UART_Process();

extern RingBuffer RxBuffer;
extern RingBuffer TxBuffer;

#endif /* TFC_UART_H_ */
