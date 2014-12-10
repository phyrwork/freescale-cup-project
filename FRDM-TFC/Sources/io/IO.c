/* IO.c
 * =================================
 * IO helper for Kinetis KL25
 * ---------------------------------
 * Assists easy peripheral setup.
 */

#include "io/GPIO.h"

PORT_MemMapPtr PTx[] = PORT_BASE_PTRS;

#define PORT io->port
#define PIN  io->pin

IO_Pin IO_InitPin(IO_Config* config) {
	/* Initialise IO */
	IO_EnableClock(&(config->io));
	IO_SetMux(&(config->io), config->mux);
	return config->io;
}

void IO_EnableClock(IO_Pin* io) {
	/* Enable clock gate to GPIO module if not already enabled */
	switch(PORT) {
		/* One case for each PT */
		case A:
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			break;
		case B:
			SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
			break;
		case C:
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
			break;
		case D:
			SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
			break;
		case E:
			SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
			break;
	}
}

void IO_SetMux(IO_Pin* io, IO_Mux mux) {
/* Clear existing config */
	PTx[PORT]->PCR[PIN] &= PORT_PCR_MUX_MASK;

	/* Write new config */
	PTx[PORT]->PCR[PIN] |= PORT_PCR_MUX(mux);
}
