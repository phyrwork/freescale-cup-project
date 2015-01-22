/* Collector.h
 * =====================================================
 * Author: Connor Newton
 *   Date: November 24, 2014
 */

#ifndef FRDM_COLLECTOR
#define FRDM_COLLECTOR

#include <stdint.h>

//Define endpoint function pointer
typedef int8_t (*CltrEndpoint_f)(void*);

//Define collector element struct
typedef struct {

	//data config
	void* const    data;     //base address of telemetry data to be passed to endpoint
	uint8_t const  deref;    //number of address 'hops' required to reach actual data (used to access 'moving' data such as linescan)
	CltrEndpoint_f endpoint; //pointer to data endpoint method (usually Tftp)

	//timing config/meta
	float const fauto;       //when set > 0 data will be pushed to endpoint at this target fauto
	uint32_t    pauto;       //'fauto' expressed in timer ticks
	float const flim;        //used to limit how often a 'request' flag can be serviced
	uint32_t    plim;        //'flim' expressed in timer ticks
	uint32_t    counter;     //time elapsed (in ticks) since last service

	//event meta
	uint8_t request;         //set by an external service to request a data collection event
	uint8_t pending;         //set internally to schedule a data collection event

} CltrItem_s;

void Collector_Init();
void CollectorRequest(uint8_t index);
void CollectorUpdate();
void CollectorProcess();

#endif //FRDM_COLLECTOR
