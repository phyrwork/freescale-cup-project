/* DMA.h
 * ==========================================
 * Direct Memory Access configuration and use
 */

#ifndef FRDM_DMA_H
#define FRDM_DMA_H

#include "devices/MKL25Z4.h"

/* DMA Transfer Control Descriptor */
typedef struct {
	uint32_t SAR;
	uint32_t DAR;
	union {
		uint32_t DSR_BCR;
		struct {
			uint8_t RESERVED_0[3];
			uint8_t DSR;
		} DMA_DSR_ACCESS8BIT;
	};
	uint32_t DCR;
} DMATCD;

DMAMUX_MemMapPtr DMAMUX[] = DMAMUX_BASE_PTRS;
DMATCD*          DMA[]    = {
	(DMATCD*)&(DMA_BASE_PTR->DMA[0]),
	(DMATCD*)&(DMA_BASE_PTR->DMA[1]),
	(DMATCD*)&(DMA_BASE_PTR->DMA[2]),
	(DMATCD*)&(DMA_BASE_PTR->DMA[3])
};

#endif //FRDM_DMA_H
