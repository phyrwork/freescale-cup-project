/* Collector.c
 * =====================================================
 * ARM_SysTick timer driven data collection service.
 * 
 * Written for use with TFTP for FRDM-TFC.
 * 
 * Author: Connor Newton
 *   Date: November 24, 2014
 * -----------------------------------------------------
 * Any data to be collected with Collector should be
 * shared via its modules' header file.
 *
 * Collector only facilitates the collection of data
 * at specified intervals from other modules - the
 * actual data handling is performed by the TFTP
 * attribute's endpoint function.
 */

#include "telemetry/Collector.h"

//import endpoints
#include "telemetry/tftp/tftp.h"

//import timer
#include "support/ARM_SysTick.h"
#include "config.h"
#define TICKER TFC_Ticker[UPTIME_TICKER]

//import data
#include "sensors/camera/LineScanCamera.h"
#include "sensors/camera/LineDetection.h"
#include "sensors/motor/current.h"
#include "sensors/cadence.h"
#include "sensors/wheel/speed.h"
#include "control/motor/pwm.h"
#include "control/motor/torque.h"
#include "io/ADC.h"

//service defines
#define CLTR_IDLE    0
#define CLTR_WAITING 1

//service config
#include <math.h>
uint32_t const dtlim = ceil( (float)COLLECTOR_TLIM_SECONDS * (float)SYSTICK_FREQUENCY );
CltrItem_s items[NUM_COLLECTOR_ITEMS] =
{
	//[0]
	{ //linescan0
		.data = &LineScanImage0,
		.deref = 1,
		.endpoint = &TFTP_LINESCAN0_ENDPOINT,
		.fauto = 10
	},
	//[1]
	{ //TrackPosition
		.data = &trackPosition,
		.deref = 0,
		.endpoint = &TFTP_TRACK_POSITION_ENDPOINT,
		.fauto = 20
	},
	//[2]
	{ //PositioningState
		.data = &positioningState,
		.deref = 0,
		.endpoint = &TFTP_POSITIONING_STATE_ENDPOINT,
		.fauto = 20
	},
	//[3]
	{ //i_rl
		.data = &MotorCurrent[REAR_LEFT].value,
		.deref = 0,
		.endpoint = &TFTP_I_RL_ENDPOINT,
		.fauto = 50
	},
	//[4]
	{ //i_rr
		.data = &MotorCurrent[REAR_RIGHT].value,
		.deref = 0,
		.endpoint = &TFTP_I_RR_ENDPOINT,
		.fauto = 50
	},
	//[5]
	{ //D_rl
		.data = &MotorPWM[REAR_LEFT].value,
		.deref = 0,
		.endpoint = &TFTP_D_RL_ENDPOINT,
		.fauto = 20
	},
	//[6]
	{ //D_rr
		.data = &MotorPWM[REAR_RIGHT].value,
		.deref = 0,
		.endpoint = &TFTP_D_RR_ENDPOINT,
		.fauto = 20
	},
	//[7]
	{ //w_rl
		.data = &WheelSpeeds[1].value,
		.deref = 0,
		.endpoint = &TFTP_W_RL_ENDPOINT,
		.fauto = 20
	},
	//[8]
	{ //w_rr
		.data = &WheelSpeeds[0].value,
		.deref = 0,
		.endpoint = &TFTP_W_RR_ENDPOINT,
		.fauto = 20
	}
};

void Collector_Init()
{
	//calculate timing information
	for (uint32_t i = 0; i < NUM_COLLECTOR_ITEMS; ++i )
	{
		CltrItem_s *item = &items[i];

		if (item->fauto > 0) item->pauto = SYSTICK_FREQUENCY / item->fauto; //auto-scheduling period
		else item->pauto = 0;

		if (item->flim > 0)  item->plim  = SYSTICK_FREQUENCY / item->flim;  //event period limit
		else item->plim = 0;
		
		//ensure other members initialized correctly
		item->counter = 0;
		item->request = 0;
		item->pending = 0;
	}
}

void CollectorRequest(uint8_t index)
{
	CltrItem_s *item = &items[index];

	item->request = CLTR_WAITING; //set request flag
}

void CollectorUpdate()
{
	static uint32_t tref = 0;      //reference ticker value
	       uint32_t t = TICKER;    //'freeze' timer
	       uint32_t dt = t - tref; //get interval since last call
	                tref = TICKER; //store time reference

	for(uint32_t i = 0; i < NUM_COLLECTOR_ITEMS; ++i)
	{
		CltrItem_s *item = &items[i];

		item->counter += dt; //increment item counter
		
		//schedule collection events
		if (item->pending) continue;
		if (item->counter >= item->plim ||  //if time elapsed greater than rate-limit period
			item->plim == 0)                //or if no rate-limit is set
		{
			//automatic scheduling
			if (item->pauto != 0 &&           //if automatic scheduling period is set
			    item->counter >= item->pauto) //and time elapsed since last event is sufficient
			{
				item->counter = 0;            //restart item timer
				item->pending = CLTR_WAITING; //schedule collection
				continue;
			}

			//request-based scheduling
			if (item->request == CLTR_WAITING) //if collection requested
			{
				item->request = CLTR_IDLE;    //clear request

				item->counter = 0;            //restart item timer
				item->pending = CLTR_WAITING; //schedule collection
				continue;
			}
		}
	}
}

void CollectorProcess()
{
	static uint32_t inum = 0;      //last item serviced (support halt and resume)
	       uint32_t tref = TICKER; //'freeze' the timer

	//process only while still in valid time slice
	for(uint32_t n = 0; n < NUM_COLLECTOR_ITEMS && TICKER - tref < dtlim; ++n, inum = ++inum >= NUM_COLLECTOR_ITEMS ? 0 : inum)
	{
		CltrItem_s *item = &items[inum];

		if (item->pending == CLTR_IDLE) continue; //if collection not scheduled, skip
		item->pending = CLTR_IDLE;                //clear flag

		void *ptr = item->data;  //base address
		uint8_t r = item->deref; //number of times to derefence to reach address of actual data

		while(r--) ptr = (void*)(*((int*) ptr)); //dereference pointer (can't assign 'void', so cast to int)
		(*(item->endpoint))(ptr);                //push data onto endpoint
	}
}
