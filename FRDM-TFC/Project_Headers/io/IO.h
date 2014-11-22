/* IO.h
 * =================================
 * IO assistant for Kinetis KL25
 * ---------------------------------
 * Assists easy peripheral setup.
 */

#ifndef KL25_IO
#define KL25_IO

#include <stdint.h>
#include "devices/derivative.h"

typedef enum { A = 0, B = 1, C = 2, D = 3, E = 4 } IO_PTx;
typedef enum { MUX_ALT0 = 0, MUX_ALT1 = 1, MUX_ALT2 = 2, MUX_ALT3 = 3,
	           MUX_ALT4 = 4, MUX_ALT5 = 5, MUX_ALT6 = 6, MUX_ALT7 = 7 } IO_Mux;

typedef struct {
	IO_PTx  port; /* Port name */
	uint8_t pin;  /* # occupied pin on port */
} IO_Pin;

typedef struct {
	IO_Pin io;
	IO_Mux mux;
} IO_Config;

IO_Pin IO_InitPin(IO_Config* config);
void   IO_EnableClock(IO_Pin* io);
void   IO_SetMux(IO_Pin* io, IO_Mux mux);

#endif

