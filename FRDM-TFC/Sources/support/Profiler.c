/* Profiler.c
 * ===========================================
 * System time profiling helper for FRDM-TFC
 * -------------------------------------------
 * Enables a user to send entry/event labelled
 * timestamps at specific times for a clearer
 * idea of what is happening at various
 * instants.
 */

#include "support/Profiler.h"
#include "config.h"
#include "support/tools.h"
#include "telemetry/tftp/tftp.h"

ProfilerEntry ProfilerEntries[NUM_PROFILER_ENTRIES];

/* Shorthand #defines */
#define ACTIVE ProfilerEntries[entry].active
#define START  ProfilerEntries[entry].start
#define STOP   ProfilerEntries[entry].stop

void Profiler_Start(uint32_t entry, uint8_t send)
{
	/* Get time reference */
	ACTIVE = 1;
	START = (uint32_t) getTime();

	/* Push to Tftp if requested */
	if (send == PROFILER_SEND) Profiler_Send(entry, PROFILER_START);
}

void Profiler_Checkpoint(uint32_t entry)
{
	Profiler_Send(entry, PROFILER_CHECKPOINT);
}

void Profiler_Stop(uint32_t entry, uint8_t send)
{
	/* Get time reference */
	STOP = (uint32_t) getTime();
	ACTIVE = 0;

	/* Push to Tftp if requested */
	if (send == PROFILER_SEND)
	{
		Profiler_Send(entry, PROFILER_STOP);
		Profiler_Send(entry, PROFILER_PERIOD);
	}
}

void Profiler_Send(uint32_t entry, uint8_t event)
{
	/* Construct frame */
	ProfilerFrame frame;
	frame.entry = entry & 0x000000FF; //8:0 passthrough
	frame.event = event;
	switch(event) {
		/* Populate frame.data based on event */
		case PROFILER_START:
			frame.data = START;
			break;

		case PROFILER_STOP:
			frame.data = STOP;
			break;

		case PROFILER_CHECKPOINT:
			frame.data = (uint32_t) getTime();
			break;

		case PROFILER_PERIOD:
			frame.data = STOP - START;
			break;
	}

	/* Push frame onto endpoint */
	TFTP_PROFILER_ENDPOINT(&frame);
}
