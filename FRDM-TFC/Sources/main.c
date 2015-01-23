#include "main.h"
#include "sensors/cadence.h"
#include "sensors/wheel/speed.h"
#include "control/wheel/speed.h"

///////////////////////////////////////
// Main Routine Task Request Handler //
///////////////////////////////////////

#define SIZEOF_FLAGS ( ((REQUESTS_MAX_TASKS - 1) / 32) + 1 )
uint32_t flags[SIZEOF_FLAGS];
//#define SIZEOF_FLAGS ( (sizeof flags) / (sizeof (uint32_t)) )

#define TR_INDEX (index >> 5)   //divide by 32 to get index in task requests array
#define TR_BIT   (index & 0x1F) //isolate bit number

uint32_t AnyTaskRequest()
{
	for (uint32_t i = 0; i < SIZEOF_FLAGS; ++i) //for each set of 32 flags
	if (flags[i]) return 1;                     //if any flags set return true
	
	return 0; //if execution reaches here no flags set, return false
}

uint32_t PollTaskRequest(uint32_t index)
{
	return (flags[TR_INDEX] & 1 << TR_BIT); //isolate task request bit
}

void SetTaskRequest(uint32_t index)
{
	flags[TR_INDEX] |= 1 << TR_BIT; //set task request bit
}

void ClearTaskRequest(uint32_t index)
{
	flags[TR_INDEX] &= ~(1 << TR_BIT); //clear task request bit
}

typedef struct {
	uint32_t index;     //index of task request bit
	float    frequency; //frequency, in seconds
	uint32_t period;    //period, in ticks
	uint32_t counter;   //time elapsed since last run, in ticks
} PeriodicTask_s;

#define SET_BY_INITIALIZATION 0
PeriodicTask_s tasks[] =
{
	/* [0] = */ {
        /* index = */     CONTROL_REQUEST_INDEX,
        /* frequency = */ CONTROL_REQUEST_FREQUENCY,
        /* period = */    SET_BY_INITIALIZATION,
        /* counter = */   SET_BY_INITIALIZATION
    },
    /* [0] = */ {
        /* index = */     TELEMETRY_REQUEST_INDEX,
        /* frequency = */ TELEMETRY_REQUEST_FREQUENCY,
        /* period = */    SET_BY_INITIALIZATION,
        /* counter = */   SET_BY_INITIALIZATION
    }
};
#define SIZEOF_TASKS ( (sizeof tasks) / (sizeof (PeriodicTask_s)) )

void TaskRequest_Init()
{ 
    //initialize schedule objects
    for (uint32_t i = 0; i < SIZEOF_TASKS; ++i )
    {
        PeriodicTask_s *item = &tasks[i];

        if (item->frequency != 0)
        {
            // Set sampling period if frequency > 0
            item->period = SYSTICK_FREQUENCY/item->frequency;
            item->counter = 0;
        }
    }
}

//#include "support/ARM_SysTick.h"
#define REQUESTS_TICKER TFC_Ticker[TASK_REQUEST_TICKER]

void UpdateTaskRequests()
{
	uint32_t ticker = REQUESTS_TICKER; //"freeze" timer
	REQUESTS_TICKER = 0;               //reset timer for accurate sampling
	
    for (uint32_t i = 0; i < SIZEOF_TASKS; ++i)
    {
        PeriodicTask_s *item = &tasks[i];

        item->counter += ticker; //update item counter

        if (item->frequency == 0) continue; //skip if frequency = 0
        if (item->counter >= item->period)
        {
            //item counter has exceeded sampling period
            item->counter = 0; //reset the counter

            SetTaskRequest(item->index); //set the task request flag
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
	TFC_InitADCs(carState);
	UART0_Init();
	DMA0_Init();
	//TFC_InitSpeedSensor();
	CadenceSensors_Init();
	InitWheelSpeedSensors();
	InitMotorPWMControl();
	InitWheelSpeedControl();
	InitWheelSlipSensors();
	InitMotorTorqueControl();
	TFC_HBRIDGE_DISABLE;
	TFC_SetMotorPWM(0, 0);
	preloadProbabilityTables(); //Prevents probability tables for stop line evaluation from being created too late
	Collector_Init(); // Initialise telemetry
}

int main(void)
{
	/* Initialise control routine */
	static carState_s carState =
	{ .motorState = FORCED_DISABLED, .UARTSpeedState = UNDEFINED, .lineDetectionState = LINE_LOST, .lineScanState = NO_NEW_LINESCAN_IMAGE };
	TFC_Init(&carState);
	
	while (carState.lineScanState != LINESCAN_IMAGE_READY){};
	//InitTracking(LineScanImage0, 350);
	TFC_SetLED(0);

	while (1)
	{	
		if ( AnyTaskRequest() )
		{
			//disable UART0
			#ifdef SERIAL_TX_IRQ_ENABLED
				UART0_DisarmIRQ();
			#endif
			
			if ( PollTaskRequest(CONTROL_REQUEST_INDEX) )
			{    ClearTaskRequest(CONTROL_REQUEST_INDEX);
			
				/* Update car state before main control routine */
				//evaluateUARTorSpeed(&carState);
				evaluateMotorState(&carState);
	
				/* Perform main control routine */
				//Profiler_Start(CONTROL_PROFILER, PROFILER_SEND);
				switch ((TFC_GetDIP_Switch() >> 1) & 0x03)
				{
					default:
						break;
						
					case 3:
						lineFollowingMode(&carState);
						TFC_SetLED(0);
						break;
				}
			}
			
			if ( PollTaskRequest(TELEMETRY_REQUEST_INDEX) )
			{    ClearTaskRequest(TELEMETRY_REQUEST_INDEX);

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

void lineFollowingMode(carState_s* carState)
{
	static lineScanState_t steeringControlUpdate;
	static uint32_t totalIntensity = 0;
	if (carState->lineScanState == LINESCAN_IMAGE_READY)
	{
		steeringControlUpdate = LINESCAN_IMAGE_READY;
		findPosition(LineScanImage0, carState, 350);// ((uint32_t)350*TARGET_TOTAL_INTENSITY) / totalIntensity);
		totalIntensity = getTotalIntensity(LineScanImage0);
		TFC_SetLineScanExposureTime(calculateNewExposure(totalIntensity, TARGET_TOTAL_INTENSITY));
		carState->lineScanState = NO_NEW_LINESCAN_IMAGE;
	}

	if (TFC_Ticker[0] >= 200)
	{
		TFC_Ticker[0] = 0;
		TFC_SetServo(0, getDesiredServoValue(carState->lineCenter, 0, &steeringControlUpdate));
	}

	if (carState->lineDetectionState == LINE_FOUND || carState->lineDetectionState == LINE_TEMPORARILY_LOST)
	{
		SetWheelSpeed(&WheelSpeedControls[REAR_LEFT], 2);
		SetWheelSpeed(&WheelSpeedControls[REAR_RIGHT], 2);
		//UpdateWheelSlip(&WheelSlipSensors[REAR_LEFT]);
		//UpdateWheelSlip(&WheelSlipSensors[REAR_RIGHT]);
		UpdateMotorTorque(&MotorTorque[REAR_LEFT]);
		UpdateMotorTorque(&MotorTorque[REAR_RIGHT]);
			
		if (carState->lineDetectionState == LINE_TEMPORARILY_LOST)
		{
//			TFC_SetLED(1);
		}
		else
		{
//			TFC_ClearLED(1);
		}
	}
	else if (carState->lineDetectionState == LINE_LOST)
	{
		//TFC_HBRIDGE_DISABLE;
		//TFC_SetMotorPWM(0, 0);
//		TFC_SetLED(2);PWM
	}
	else if (carState->lineDetectionState == STOPLINE_DETECTED)
	{
		//STOP!
	}
}
