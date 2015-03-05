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
#include "sensors/vehicle/slip.h"
#include "control/vehicle/slip.h"
#include "sensors/vehicle/speed.h"
#include "control/vehicle/speed.h"
#include "control/wheel/speed.h"
#include "sensors/wheel/slip.h"
#include "control/motor/pwm.h"
#include "control/motor/torque.h"
#include "io/ADC.h"
#include "support/carState_s.h"

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
		.data = &linescan0.image,
		.deref = 1,
		.endpoint = &TFTP_LINESCAN0_ENDPOINT,
		.flim = 4
	},
	//[1]
	{ //linescan0
		.data = &linescan1.image,
		.deref = 1,
		.endpoint = &TFTP_LINESCAN1_ENDPOINT,
		.flim = 4
	},
	//[2]
	{ //TrackPosition
		.data = &trackPosition,
		.endpoint = &TFTP_TRACK_POSITION_ENDPOINT,
		.fauto = 20
	},
	//[3]
	{ //PositioningState
		.data = &positioningState,
		.endpoint = &TFTP_POSITIONING_STATE_ENDPOINT,
		.fauto = 10
	},
	//[4]
	{ //i_rl
		.data = &MotorCurrent[REAR_LEFT].value,
		.endpoint = &TFTP_I_RL_ENDPOINT,
		.fauto = 30
	},
	//[5]
	{ //i_rr
		.data = &MotorCurrent[REAR_RIGHT].value,
		.endpoint = &TFTP_I_RR_ENDPOINT,
		.fauto = 30
	},
	//[6]
	{ //D_rl
		.data = &MotorPWM[REAR_LEFT].value,
		.endpoint = &TFTP_D_RL_ENDPOINT,
		.fauto = 30
	},
	//[7]
	{ //D_rr
		.data = &MotorPWM[REAR_RIGHT].value,
		.endpoint = &TFTP_D_RR_ENDPOINT,
		.fauto = 30
	},
	//[8]
	{ //w_rl
		.data = &WheelSpeedSensors[REAR_LEFT].value,
		.endpoint = &TFTP_W_RL_ENDPOINT,
		.fauto = 20
	},
	//[9]
	{ //w_rr
		.data = &WheelSpeedSensors[REAR_RIGHT].value,
		.endpoint = &TFTP_W_RR_ENDPOINT,
		.fauto = 20
	},
	//[10]
	{ //w_fl
		.data = &WheelSpeedSensors[FRONT_LEFT].value,
		.endpoint = &TFTP_W_FL_ENDPOINT,
		.fauto = 20
	},
	//[11]
	{ //w_fr
		.data = &WheelSpeedSensors[FRONT_RIGHT].value,
		.endpoint = &TFTP_W_FR_ENDPOINT,
		.fauto = 20
	},
	//[12]
	{ //w_rl
		.data = &WheelSpeedControls[REAR_LEFT].cmd,
		.endpoint = &TFTP_WREF_RL_ENDPOINT,
		.fauto = 20
	},
	//[13]
	{ //w_rr
		.data = &WheelSpeedControls[REAR_RIGHT].cmd,
		.endpoint = &TFTP_WREF_RR_ENDPOINT,
		.fauto = 20
	},
	//[14]
	{ //s_rl
		.data = &WheelSlipSensors[REAR_LEFT].value,
		//.data = &s_rl,
		//.deref = 1,
		.endpoint = &TFTP_S_RL_ENDPOINT,
		.fauto = 30
	},
	//[15]
	{ //s_rr
		.data = &WheelSlipSensors[REAR_RIGHT].value,
		//.data = &s_rr,
		//.deref = 1,
		.endpoint = &TFTP_S_RR_ENDPOINT,
		.fauto = 30
	},
	//[16]
	{ //TargetLine
		.data = &TargetLine,
		.endpoint = &TFTP_TARGET_LINE_ENDPOINT,
		//.flim = 10
	},
	//[17]
	{ //ServoPosition
		.data = &carState.servoPosition,
		.endpoint = &TFTP_SERVO_POSITION_ENDPOINT,
		.flim = 100
	},
	//[18]
	{ //T_rl
		.data = &MotorTorque[REAR_LEFT].value,
		.endpoint = &TFTP_T_RL_ENDPOINT,
		.fauto = 30
	},
	//[19]
	{ //T_rr
		.data = &MotorTorque[REAR_RIGHT].value,
		.endpoint = &TFTP_T_RR_ENDPOINT,
		.fauto = 30
	},
	//[20]
	{ //T*_rl
		.data = &MotorTorque[REAR_LEFT].cmd,
		.endpoint = &TFTP_TREF_RL_ENDPOINT,
		.fauto = 30
	},
	//[21]
	{ //T*_rr
		.data = &MotorTorque[REAR_RIGHT].cmd,
		.endpoint = &TFTP_TREF_RR_ENDPOINT,
		.fauto = 30
	},
	//[22]
	{ //S_v
		.data = &VehicleSlipSensor.value,
		.endpoint = &TFTP_S_V_ENDPOINT,
		//.fauto = 20
	},
	//[23]
	{ //S*_v
		.data = &VehicleSlipControl.cmd,
		//.data = &contrib,
		.endpoint = &TFTP_SREF_V_ENDPOINT,
		.fauto = 30
	},
	//[24]
	{ //S_v
		.data = &VehicleSpeedSensor.value,
		.endpoint = &TFTP_W_V_ENDPOINT,
		.fauto = 20
	},
	//[25]
	{ //S*_v
		.data = &VehicleSpeedControl.cmd,
		.endpoint = &TFTP_WREF_V_ENDPOINT,
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
