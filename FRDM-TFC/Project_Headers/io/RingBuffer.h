/*  RingBuffer.c
 *  ========================================================
 *  Specialist ring buffer structure and methods for use
 *  with zero-delimited messsages and DMA.
 *  --------------------------------------------------------
 *  Author: Connor Newton
 *  Completed: 7 Nov, 2014
 *  Revised: n/a
 */
#ifndef TFTP_RING_BUFFER_
#define TFTP_RING_BUFFER_

#include <stdint.h>

typedef struct {
  uint8_t* elems;  /* Pointer to data storage array */
  uint16_t size;   /* Size of data storage array */
  uint16_t head;   /* Next buffer index to be read from */
  uint16_t dhead;  /* New head after DMA complete */
  uint16_t tail;   /* Last buffer index to be written to */
  uint16_t read;   /* Number of bytes read */
  uint16_t write;  /* Number of bytes written */
  uint16_t frame;  /* Number of zero-delimited frames in buffer */
  uint16_t last;   /* Last zero written to the buffer */
} RingBuffer;

typedef struct {
  uint8_t *     ptr;
  uint16_t      size;
} Vector8u;

void rbInit(RingBuffer * rb, uint8_t * ptr, uint16_t size);
uint16_t rbAvailable(RingBuffer * rb);
uint16_t rbUsed(RingBuffer * rb);
uint16_t rbContig(RingBuffer * rb);
int8_t rbPush(RingBuffer * rb, uint8_t datum);
int8_t rbPushFrame(RingBuffer * rb, uint8_t * array, uint16_t size);
int8_t rbPop(RingBuffer * rb, uint8_t * datum);
uint16_t rbPopFrame(RingBuffer * rb, uint8_t * array);
Vector8u rbPopDma(RingBuffer * rb);
uint16_t rbFrames(RingBuffer * rb);

#endif /*TFTP_RING_BUFFER_ */
