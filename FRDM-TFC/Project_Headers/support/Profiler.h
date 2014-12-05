/* Profiler.h
 * ===========================================
 * System time profiling helper for FRDM-TFC
 * -------------------------------------------
 * Enables a user to send entry/event labelled
 * timestamps at specific times for a clearer
 * idea of what is happening at various
 * instants.
 */

#ifndef FRDM_PROFILER
#define FRDM_PROFILER

#include <stdint.h>

/* Profiler_Stop send/discard argument definitions */
#define PROFILER_DISCARD 0
#define PROFILER_SEND 1

/* Profiler_Event event definitions */
#define PROFILER_START      10
#define PROFILER_STOP       20
#define PROFILER_CHECKPOINT 30
#define PROFILER_PERIOD     40

/* Internal data type */
typedef struct {
	uint8_t  active; /* Is profiled action currently being timed? */
	uint32_t start;  /* Uptime ticker value at action call */
	uint32_t stop;   /* Uptime ticker value at action complete */
} ProfilerEntry;

/* Tftp frame format */
typedef struct {
	unsigned int entry : 8;
	unsigned int event : 8;
	uint32_t     data;
} ProfilerFrame;

void Profiler_Start(uint32_t entry, uint8_t send);
void Profiler_Checkpoint(uint32_t entry);
void Profiler_Stop(uint32_t entry, uint8_t send);
void Profiler_Send(uint32_t entry, uint8_t event);

#endif //FRDM_PROFILER
