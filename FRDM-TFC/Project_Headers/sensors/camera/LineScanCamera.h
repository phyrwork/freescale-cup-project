#ifndef TFC_LINESCANCAMERA_H_
#define TFC_LINESCANCAMERA_H_

#include "support/Types.h"
#include "devices/MKL25Z4.h"

#define LINESCAN_SIGNAL_HIGH(signal)   signal.port->PSOR = (1 << signal.pin)
#define LINESCAN_SIGNAL_LOW(signal)    signal.port->PCOR = (1 << signal.pin)
#define LINESCAN_SIGNAL_VAL(signal)  ( signal.port->PDOR & 1 (<< signal.pin) )

typedef struct 
{
	struct {
		GPIO_MemMapPtr const port;
		uint8_t const pin;
	} clk;
	struct {
		GPIO_MemMapPtr const port;
		uint8_t const pin;
	} si;
} LineScanControl_s;

typedef struct
{	
	uint8_t  flag;
	uint16_t data[2][128];
	uint16_t *image;
	struct {
		uint16_t *data;
		uint8_t pos;
	} buffer;
	struct {
		uint32_t time;  //most recent commanded exposure time
		uint32_t start; //time of start of current exposure
	} exposure;
	LineScanControl_s * const signal;
} LineScan_s;

void TFC_InitLineScanCamera();
void TFC_LineScanImageCamera();

int8_t LinescanProcess (LineScan_s *linescan, uint16_t data);

void TFC_SetLineScanExposureTime(uint32_t  TimeIn_uS);
uint32_t TFC_getLineScanExposureTime();

extern LineScan_s linescan[2];

#endif /* TFC_LINESCANCAMERA_H_ */
