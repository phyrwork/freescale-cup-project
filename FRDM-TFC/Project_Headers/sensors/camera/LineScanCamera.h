#ifndef TFC_LINESCANCAMERA_H_
#define TFC_LINESCANCAMERA_H_

#include "support/Types.h"

void TFC_InitLineScanCamera();
void TFC_LineScanImageCamera();

void TFC_SetLineScanExposureTime(uint32_t  TimeIn_uS);
uint32_t TFC_getLineScanExposureTime();

#define TAOS_CLK_HIGH  GPIOE_PSOR = (1<<1)  
#define TAOS_CLK_LOW   GPIOE_PCOR = (1<<1)
#define TAOS_CLK_VAL  (GPIOE_PDOR & (1<<1))
#define TAOS_SI_HIGH   GPIOD_PSOR = (1<<7)
#define TAOS_SI_LOW    GPIOD_PCOR =	(1<<7)

typedef struct
{
	uint8_t  flag;
	uint16_t data[2][128];
	uint16_t *image;
	struct {
		uint16_t *data;
		uint8_t pos;
	} buffer;
} LineScan_s;

extern LineScan_s linescan0;
extern LineScan_s linescan1;

#endif /* TFC_LINESCANCAMERA_H_ */
