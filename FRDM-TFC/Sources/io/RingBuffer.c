/*  RingBuffer.c
 *  =========================================================
 *  Specialist ring buffer structure and methods for use
 *  with zero-delimited messsages and DMA.
 *  ---------------------------------------------------------
 *  Author: Connor Newton
 *  Completed: 7 Nov, 2014
 *  Revised: 8 Nov, 2014 - various bugfixes; modified rbPop
 *           to return status code and write data to provided
 *           pointer
 */

#include "io/RingBuffer.h"
#include "config.h"

inline void rbInit(RingBuffer * rb, uint8_t * ptr, uint16_t size);
inline uint16_t rbAvailable(RingBuffer * rb);
inline uint16_t rbUsed(RingBuffer * rb);
inline uint16_t rbContig(RingBuffer * rb);
int8_t rbPush(RingBuffer * rb, uint8_t datum);
int8_t rbPushFrame(RingBuffer * rb, uint8_t * array, uint16_t size);
int8_t rbPop(RingBuffer * rb, uint8_t * datum);
uint16_t rbPopFrame(RingBuffer * rb, uint8_t * array);
Vector8u rbPopDma(RingBuffer * rb);
inline uint16_t rbFrames(RingBuffer * rb);

/*  rbInit()
 *  ========================================================
 *  Initialise RingBuffer structure.
 *  --------------------------------------------------------
 *  RingBuffer * rb       - pointer to RingBuffer structure.
 *  uint8_t * storagePtr  - pointer to buffer storage byte
 *                          array.
 *  uint16_t size         - size of buffer storage byte
 *                          array.
 *  void return           - nothing.
 */
inline void rbInit(RingBuffer * rb, uint8_t * ptr, uint16_t size) {
  rb->size  = size;
  rb->head  = 0;
  rb->dhead = 0;
  rb->tail  = 0;
  rb->elems = ptr;
}

/*  rbAvailable()
 *  ==================================================
 *  Query free slots in buffer.
 *  --------------------------------------------------
 *  RingBuffer * rb - pointer to RingBuffer structure.
 *  uint16_t return - number of empty elements.
 */
inline uint16_t rbAvailable(RingBuffer * rb) { return rb->size - rbUsed(rb); }

/*  rbUsed()
 *  ==================================================
 *  Query used slots in buffer.
 *  --------------------------------------------------
 *  RingBuffer * rb - pointer to RingBuffer structure.
 *  uint16_t return - number of used elements.
 */
inline uint16_t rbUsed(RingBuffer * rb) { return rb->write - rb->read; }

/*  rbContig()
 *  ==================================================
 *  Query next contiguous used space in buffer.
 *  --------------------------------------------------
 *  RingBuffer * rb - pointer to RingBuffer structure.
 *  uint16_t return - size of next contiguous space.
 */
inline uint16_t rbContig(RingBuffer * rb) {
  /* If head > tail, contig from head to end */
  if ( !(rb->head > rb->tail) ) return rb->size - rb->head;
  /* Else contig from head to tail */
  else return rb->tail - rb->head;
}

/*  rbPush()
 *  ==================================================
 *  Push byte value onto buffer.
 *  --------------------------------------------------
 *  RingBuffer * rb - pointer to RingBuffer structure.
 *  uint8_t datum   - byte value to push onto buffer.
 *  uint8_t return  - storage success true/false.
 */
int8_t rbPush(RingBuffer * rb, uint8_t datum) {

  /* Check if buffer space available */
  if ( rbAvailable(rb) ) {

    /* Count frame boundaries */
    if (datum == 0) {
      if (rb->last == 0) return 1; //Discard superfluous zeros.
      rb->frame++;
    }

    /* Push datum onto buffer */
    rb->elems[rb->tail++] = rb->last = datum;

    /* Check if tail pointer has reached limit of buffer */
    if (rb->tail == rb->size) rb->tail = 0; //Wrap tail pointer around if appropriate.

    /* Increment write count -- do this last so that
       data cannot be read prematurely by an ISR     */
    rb->write++;

    return 0;
  } else return -1; //Return false.
}

/*  rbPushFrame()
 *  ==================================================
 *  Push frame onto buffer.
 *  --------------------------------------------------
 *  RingBuffer * rb - pointer to RingBuffer structure.
 *  uint8_t * array - pointer to frame data.
 *  uint16_t size   - size of frame in bytes.
 *  uint8_t return  - storage success true/false.
 */
int8_t rbPushFrame(RingBuffer * rb, uint8_t * array, uint16_t size) {

  uint16_t i = 0;   /* Read position of incoming array */
  uint16_t contig;  /* Size of first contiguous free space in buffer */
  uint16_t rem = 0; /* Size of remaining transfer */

  /* Check if buffer space available */
  if ( size <= rbAvailable(rb) ) {

    /* Update write count early to prevent writes from ISR */
    rb->write += size;

    /* Determine one or two contigous writes */ 
    if ( size > (contig = rb->size - rb->tail) ) rem = size - contig;
    else contig = size; //Write is smaller than first contiguous free space.

    /* First write to buffer */
    while (contig--) rb->elems[rb->tail++] = array[i++];

    /* If first write to limit of buffer */
    if (rb->tail == rb->size) {
      
      rb->tail = 0; //Wrap tail around.

      /* Second write to buffer -- nested because if first write
         not to limit then no need to test for second write      */
      while (rem--) rb->elems[rb->tail++] = array[i++];
    }

    rb->frame++; //Count frame.
    return 0;
  } else return -1;
}

/*  rbPop()
 *  ==================================================
 *  Pop byte value from buffer.
 *
 *  NOTE: This method will not fail if rbUsed() = 0;
 *  HOWEVER it should only be called to pop data for
 *  serial transmission and with COBS extra zeros are
 *  fine.
 *  --------------------------------------------------
 *  RingBuffer * rb - pointer to RingBuffer structure.
 *  uint8_t * array - pointer to frame data.
 *  uint16_t size   - size of frame in bytes.
 *  uint8_t return  - storage success true/false.
 */
int8_t rbPop(RingBuffer * rb, uint8_t * datum) {

	if ( rbUsed(rb) ){
	  /* Pop data from buffer */
	  *datum = rb->elems[rb->head++];
	
	  /* Check if head pointer has reached limit of buffer */
	  if (rb->head == rb->size) rb->head = 0; //Wrap head pointer around if appropriate.
	
	  /* Count frame boundaries */
	  if (*datum == 0) rb->frame--;
	
	  /* Increment read count -- do this last so that
		 data cannot be written prematurely by an ISR */
	  rb->read++;
	
	  return 0;
	} else return -1;
}

/*  rbPopFrame()
 *  ==================================================
 *  Pop frame from buffer.
 *
 *  NOTE: *array should be as large as the maximum
 *  defined frame size to prevent overflows.
 *  --------------------------------------------------
 *  RingBuffer * rb - pointer to RingBuffer structure.
 *  uint8_t * array - pointer to frame data.
 *  uint16_t size   - size of frame in bytes.
 *  uint8_t return  - storage success true/false.
 */
uint16_t rbPopFrame(RingBuffer * rb, uint8_t * array) {

  uint8_t datum = 1; /* Most recently popped value */
  uint16_t size = 0; /* Size of frame popped */

  if ( rbFrames(rb) ) {

    /* Until frame boundary encountered */
    while (datum != 0) { // datum initialized to 1, first loop always occurs

      /* Read data from buffer */
      datum = array[size++] = rb->elems[rb->head++];
      
      /* Wrap head around if appropriate */
      if (rb->head == rb->size) rb->head = 0;
    }

    /* Decrement frames, update read counter */
    rb->frame--;
    rb->read += size;

    return size;
  } else return 0;
}

/*  rbPopDma()
 *  ==================================================
 *  Get pointer to and size of next contiguous used
 *  buffer space for use in a DMA request.
 *
 *  NOTE: This method should be used exclusively to
 *  pop - not in conjunction with rbPop.
 *  NOTE: This method will not fail if rbUsed() = 0
 *  - check 'significant' data available beforehand.
 *  --------------------------------------------------
 *  RingBuffer * rb - pointer to RingBuffer structure.
 *  Vector8u        - struct containing pointer and
 *                    size of next contiguous used
 *                    space.
 */
Vector8u rbPopDma(RingBuffer * rb) {

  Vector8u dma; /* Return struct */
  
  /* Calculate transfer size */
  dma.size = rbContig(rb);
  if (dma.size > RB_MAX_DMA_SIZE) dma.size = RB_MAX_DMA_SIZE;

  /* Calculate head after transfer */
  rb->dhead = rb->head + dma.size;
  if (rb->dhead == rb->size) rb->dhead = 0; //Wrap head around if necessary

  /* Get pointer to data */
  dma.ptr = &(rb->elems[rb->head]);

  return dma;
}

/*  rbFrames()
 *  ==================================================
 *  The definition says it all...
 */
inline uint16_t rbFrames(RingBuffer * rb) { return rb->frame; }
