/* Collector.h
 * =====================================================
 * Author: Connor Newton
 *   Date: November 24, 2014
 */

#ifndef FRDM_COLLECTOR
#define FRDM_COLLECTOR

#include <stdint.h>

/* Define endpoint function pointer */
typedef int8_t (*Endpoint)(void*);

/* Define collector element struct */
typedef struct {
	void*     data;
	uint8_t   deref; /* Number of deferences required to reach data -
	                   facilitates access to data behind pointers */
	Endpoint  endpoint;
	uint16_t  frequency;
	uint16_t  period;
	uint16_t  counter;
} CollectorItem;

/* Endpoint error reporting */
typedef struct {
	uint8_t flag;
	int8_t* error;
} CollectorStatus;

#define COLLECTOR_ENDPOINT_OK    0
#define COLLECTOR_ENDPOINT_ERROR 1

void             Collector_Init();
CollectorStatus* Collector();

#endif //FRDM_COLLECTOR
