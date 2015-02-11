#include "main.h"
#include "sensors/cadence.h"
#include "sensors/wheel/speed.h"
#include "control/wheel/speed.h"
#include "control/vehicle/speed.h"

///////////////////////////////////////
// Main Routine Task Request Handler //
///////////////////////////////////////

#define SIZEOF_FLAGS ( ((NUM_TASK_ITEMS - 1) / 32) + 1 )
uint32_t pflags[SIZEOF_FLAGS];
//#define SIZEOF_FLAGS ( (sizeof flags) / (sizeof (uint32_t)) )

#define TR_INDEX (index >> 5)   //divide by 32 to get index in task requests array
#define TR_BIT   (index & 0x1F) //isolate bit number

uint32_t AnyTaskPending()
{
	for (uint32_t i = 0; i < SIZEOF_FLAGS; ++i) //for each set of 32 flags
	if (pflags[i]) return 1;                     //if any flags set return true
	
	return 0; //if execution reaches here no flags set, return false
}

uint32_t PollTaskPending(uint32_t index)
{
	return (pflags[TR_INDEX] & 1 << TR_BIT); //isolate task request bit
}

void SetTaskPending(uint32_t index)
{
	pflags[TR_INDEX] |= 1 << TR_BIT; //set task request bit
}

void ClearTaskPending(uint32_t index)
{
	pflags[TR_INDEX] &= ~(1 << TR_BIT); //clear task request bit
}

typedef struct {
	float const fauto;   //when set > 0 data will be pushed to endpoint at this target fauto
	uint32_t    pauto;   //'fauto' expressed in timer ticks
	float const flim;    //used to limit how often a 'request' flag can be serviced
	uint32_t    plim;    //'flim' expressed in timer ticks
	uint32_t    counter; //time elapsed (in ticks) since last service
	uint8_t     request; //task request flag
} MainTask_s;

#define TASK_IDLE 0
#define TASK_WAITING 1

MainTask_s tasks[NUM_TASK_ITEMS] =
{
	//[0]
	{ //control
        .fauto = 100
    },
    //[1]
    { //telemetry
        .fauto = 100
    },
    //[2]
	{ //positioning
	},
	//[3]
	{ //steering
		.fauto = 50
	}
};

void SetTaskRequest(uint32_t index) { tasks[index].request = TASK_WAITING; }
void ClearTaskRequest(uint32_t index) { tasks[index].request = TASK_IDLE; }

void TaskRequest_Init()
{ 
    //calculate timing information
    for (uint32_t i = 0; i < NUM_TASK_ITEMS; ++i )
    {
        MainTask_s *item = &tasks[i];

        if (item->fauto > 0) item->pauto = SYSTICK_FREQUENCY / item->fauto; //auto-scheduling period
        else item->pauto = 0;

		if (item->flim > 0)  item->plim  = SYSTICK_FREQUENCY / item->flim;  //request period limit
		else item->plim = 0;
		
		//ensure other members initialized correctly
		item->counter = 0;
    }
}

//#include "support/ARM_SysTick.h"
#define TICKER TFC_Ticker[UPTIME_TICKER]

void UpdateTaskRequests()
{
	static uint32_t tref = 0;      //reference ticker value
		   uint32_t t = TICKER;    //'freeze' timer
		   uint32_t dt = t - tref; //get interval since last call
		            tref = TICKER; //store time reference

	for(uint32_t i = 0; i < NUM_TASK_ITEMS; ++i)
	{
		MainTask_s *item = &tasks[i];

		item->counter += dt; //increment item counter
		
		//schedule collection events
		if (item->counter >= item->plim ||  //if time elapsed greater than rate-limit period
			item->plim == 0)                //or if no rate-limit is set
		{
			//automatic scheduling
			if (item->pauto != 0 &&           //if automatic scheduling period is set
				item->counter >= item->pauto) //and time elapsed since last event is sufficient
			{
				item->counter = 0;            //restart item timer
				SetTaskPending(i);
				continue;
			}

			//request-based scheduling
			if (item->request == TASK_WAITING) //if collection requested
			{
				item->request = TASK_IDLE;    //clear request

				item->counter = 0;            //restart item timer
				SetTaskPending(i);
				continue;
			}
		}
	}
}

/////////////
// MAIN!!! //
/////////////

#define TARGET_TOTAL_INTENSITY 300000//300000
#define CHANNEL_0 0
#define CHANNEL_1 1

void TFC_Init(carState_s* carState)
{
	TFC_InitClock();
	TFC_InitSysTick();
	TFC_InitGPIO();
	TFC_InitServos();
	TFC_InitLineScanCamera();
	InitCurrentSensors(); //Must be initialized before ADC or illegal memory access will occur
	TaskRequest_Init();
	TFC_InitADCs();
	UART0_Init();
	DMA0_Init();
	InitMotorPWMControl();
	CadenceSensors_Init();
	InitWheelSpeedSensors();
	InitWheelSpeedControl();
	InitVehicleSpeedSensor();
	InitVehicleSpeedControl();
	InitWheelSlipSensors();
	InitMotorTorqueControl();
	TFC_HBRIDGE_DISABLE;
	TFC_SetMotorPWM(0, 0);
	preloadProbabilityTables(); //Prevents probability tables for stop line evaluation from being created too late
	Collector_Init(); // Initialise telemetry
}

int main(void)
{
	//initialise car state
	carState.motorState = FORCED_DISABLED;
	carState.UARTSpeedState = UNDEFINED;
	carState.lineDetectionState = LINE_LOST;
	carState.lineScanState = NO_NEW_LINESCAN_IMAGE;
	
	//initialise modules
	TFC_Init(&carState);
	
	while ( !PollTaskPending(POSITIONING_REQUEST_INDEX) ){};
	//InitTracking(LineScanImage0, 350);
	TFC_SetLED(0);

	while (1)
	{	
		UpdateTaskRequests();
		if ( AnyTaskPending() )
		{
			//disable UART0
			#ifdef SERIAL_TX_IRQ_ENABLED
				UART0_DisarmIRQ();
			#endif

			//Positioning update tasks
			if ( PollTaskPending(POSITIONING_REQUEST_INDEX) )
			{    ClearTaskPending(POSITIONING_REQUEST_INDEX);
			
				uint32_t totalIntensity = 0;
				int16_t dy[128];

				//update position
				diff(LineScanImage0, dy, 128);

				if (/*findStop(dy) == STOP_LINE_FOUND*/0)
				{
					carState.lineDetectionState = STOPLINE_DETECTED;
					SetTaskPending(CONTROL_REQUEST_INDEX);
				}
				else if (carState.lineDetectionState != STOPLINE_DETECTED)
				{
					findPosition(dy, &carState);
				}

				//adjust camera exposure
				totalIntensity = getTotalIntensity(LineScanImage0);
				TFC_SetLineScanExposureTime(calculateNewExposure(totalIntensity, TARGET_TOTAL_INTENSITY));
			}
			
			//main control
			if ( PollTaskPending(CONTROL_REQUEST_INDEX) )
			{    ClearTaskPending(CONTROL_REQUEST_INDEX);
			
				//enable/disable H-bridge
				evaluateMotorState(&carState);
				
				//UpdateWheelSlip(&WheelSlipSensors[REAR_LEFT]);
				//UpdateWheelSlip(&WheelSlipSensors[REAR_RIGHT]);
				UpdateWheelSpeed(&WheelSpeedSensors[FRONT_LEFT]);
				UpdateWheelSpeed(&WheelSpeedSensors[FRONT_RIGHT]);
				UpdateMotorTorque(&MotorTorque[REAR_LEFT]);
				UpdateMotorTorque(&MotorTorque[REAR_RIGHT]);
	
				if (carState.lineDetectionState == LINE_FOUND || carState.lineDetectionState == LINE_TEMPORARILY_LOST)
				{
					float tspeed = GetVehicleSpeed((float)carState.lineCenter);
					SetVehicleSpeed(tspeed); 
					UpdateWheelSlip(&WheelSlipSensors[REAR_LEFT]);
					UpdateWheelSlip(&WheelSlipSensors[REAR_RIGHT]);
				}
				else if (carState.lineDetectionState == LINE_LOST)
				{
					//WheelSpeedControls[REAR_LEFT].pid->value_min = 0; //unidirectional speed sensors suck
					//WheelSpeedControls[REAR_RIGHT].pid->value_min = 0;
					SetVehicleSpeed(0);
					TFC_HBRIDGE_DISABLE;
				}
				else if (carState.lineDetectionState == STOPLINE_DETECTED)
				{
					//WheelSpeedControls[REAR_LEFT].pid->value_min = 0;
					//WheelSpeedControls[REAR_RIGHT].pid->value_min = 0;
					SetVehicleSpeed(0);
					TFC_HBRIDGE_DISABLE;
				}
			}
			
			//update steering
			if ( PollTaskPending(STEERING_REQUEST_INDEX) )
			{    ClearTaskPending(STEERING_REQUEST_INDEX);
				carState.servoPosition = getDesiredServoValue(carState.lineCenter, 0);
				CollectorRequest(SERVO_POSITION_COLLECTOR_INDEX);
				TFC_SetServo(0, carState.servoPosition);
			}
			
			if ( PollTaskPending(TELEMETRY_REQUEST_INDEX) )
			{    ClearTaskPending(TELEMETRY_REQUEST_INDEX);

				/* Run data collection routine */
				CollectorUpdate();
				CollectorProcess();
			}
		}
		else
		{
			#ifdef SERIAL_TX_IRQ_ENABLED
				UART0_ArmIRQ();
			#endif
		}
	}
	return 0;
}

void evaluateMotorState(carState_s* carState)
{
	if ((TFC_GetDIP_Switch() & 0x01) == 1)
	{
		TFC_HBRIDGE_ENABLE;
		carState->motorState = RUNNING;
	}
	else if ((TFC_GetDIP_Switch() & 0x01) == 0)
	{
		TFC_HBRIDGE_DISABLE;
		TFC_SetMotorPWM(0, 0);
	}
}
