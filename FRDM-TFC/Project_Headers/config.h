
#ifndef TFC_CONFIG_H_
#define TFC_CONFIG_H_

#define RELEASE					"1.0"

#define PCB_REV					A

/* Ticker configuration and index definitions */
//#define SYSTICK_FREQUENCY /* To do: Move #define here and remove hard coded dependencies on this value */
#define NUM_TFC_TICKERS 9
#define UPTIME_TICKER 4
#define MAIN_TRIGGER_TICKER 6
#define ADC_SAMPLE_TICKER 7
#define TASK_REQUEST_TICKER 8

/* Collector config */
#define COLLECTOR_TLIM_SECONDS 0.0005f //2000Hz
#define NUM_COLLECTOR_ITEMS               17
#define LINESCAN0_COLLECTOR_INDEX          0
#define TRACK_POSITION_COLLECTOR_INDEX     1
#define POSITIONING_STATE_COLLECTOR_INDEX  2
#define I_RL_COLLECTOR_INDEX               3
#define I_RR_COLLECTOR_INDEX               4
#define D_RL_COLLECTOR_INDEX               5
#define D_RR_COLLECTOR_INDEX               6
#define W_RL_COLLECTOR_INDEX               7
#define W_RR_COLLECTOR_INDEX               8
#define W_FL_COLLECTOR_INDEX               9
#define W_FR_COLLECTOR_INDEX              10
#define WREF_RL_COLLECTOR_INDEX           11
#define WREF_RR_COLLECTOR_INDEX           12
#define S_RL_COLLECTOR_INDEX              13
#define S_RR_COLLECTOR_INDEX              14
#define TARGET_LINE_COLLECTOR_INDEX       15
#define SERVO_POSITION_COLLECTOR_INDEX    16

/* PIT1 config */
#define PIT1_POLLING_FREQUENCY 2400

/* Trigger configuration */
#define NUM_TASK_ITEMS 4
#define CONTROL_REQUEST_INDEX 0
#define TELEMETRY_REQUEST_INDEX 1
#define POSITIONING_REQUEST_INDEX 2
#define STEERING_REQUEST_INDEX 3

/* ADC sampling configuration - n.b. these are target sampling frequencies only */
#define SAMPLER_QUEUE_LENGTH 16
#define POT_0_SAMPLE_FREQUENCY 0
#define POT_1_SAMPLE_FREQUENCY 0
#define MOTOR_CURRENT_0_SAMPLE_FREQUENCY 600
#define MOTOR_CURRENT_1_SAMPLE_FREQUENCY 600
#define BATTERY_SAMPLE_FREQUENCY 1

/* Powertrain config */
#define NUM_MOTORS  2
#define REAR_LEFT   0
#define REAR_RIGHT  1
#define FRONT_LEFT  2
#define FRONT_RIGHT 3

/* filtering configuration */
#define CURRENT_FILTER_COEFFS {0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258,0.032258}
#define CURRENT_FILTER_ORDER 31
#define WSPEED_FILTER_COEFFS {0.017092,0.020771,0.024819,0.029156,0.033675,0.038242,0.042697,0.04687,0.050586,0.053678,0.056002,0.057445,0.057934,0.057445,0.056002,0.053678,0.050586,0.04687,0.042697,0.038242,0.033675,0.029156,0.024819,0.020771,0.017092}
#define WSPEED_FILTER_ORDER 25
#define CADENCE_BUFFER_SIZE WSPEED_FILTER_ORDER


/* Profiler configuration */
#define NUM_PROFILER_ENTRIES 1
#define CONTROL_PROFILER 0
#define ADC_CURRENT_PROFILER 1

//This is what the exposure time for the Linescan will be set to out of bootup
#define TFC_DEFAULT_LINESCAN_EXPOSURE_TIME_uS	10000 //50000

#define TFC_MOTOR_SWITCHING_FREQUENCY	((float)(4000.0))

/* Terminal defines from depreciated Terminal.h */
#define TERMINAL_OUT_QUEUE			
#define TERMINAL_IN_QUEUE			
#define TERMINAL_PRINTF(...)   		
#define TERMINAL_PUTC(c)        	
#define TERMINAL_READABLE       	
#define TERMINAL_GETC           	

//////////////////////////
// Serial Configuration //
//////////////////////////

/* Media configuration */
#define BLUETOOTH_ENABLED
#define SERIAL_TX_IRQ_ENABLED
//#define SERIAL_TX_DMA_ENABLED

/* DMA configuration */
#define SERIAL_TX_DMA_THRESHOLD 64
#define RB_MAX_DMA_SIZE 256

/* Data rate configuration */
#define USB_SERIAL_BAUD 115200
#define BLUETOOTH_SERIAL_BAUD_DEFAULT 9600
#define BLUETOOTH_CMD_BAUD 38400
#define BLUETOOTH_SERIAL_BAUD 460800

/* Buffer and MTU configuration */
#define SERIAL_MAX_MSG_SIZE 384
#define RB_RX_SIZE 128
#define RB_TX_SIZE 1536

/* TFTP configuration */
#define TFTP_TIMESTAMP_TOLERANCE_SECONDS 0.005

#endif /* TFC_CONFIG_H_ */
