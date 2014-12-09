
#ifndef TFC_CONFIG_H_
#define TFC_CONFIG_H_

#define RELEASE					"1.0"

#define PCB_REV					A

/* Ticker configuration and index definitions */
//#define SYSTICK_FREQUENCY /* To do: Move #define here and remove hard coded dependencies on this value */
#define NUM_TFC_TICKERS 8
#define UPTIME_TICKER 4
#define COLLECTOR_TICKER 5
#define MAIN_TRIGGER_TICKER 6
#define ADC_SAMPLE_TICKER 7

/* Trigger configuration */
#define MAIN_TRIGGER_POLLLING_INTERVAL 50
#define CONTROL_TRIGGER_FREQUENCY 100
#define TELEMETRY_TRIGGER_FREQUENCY 100

/* ADC sampling configuration - n.b. these are target sampling frequencies only */
#define POT_0_SAMPLE_FREQUENCY 10
#define POT_1_SAMPLE_FREQUENCY 10
#define MOTOR_CURRENT_0_SAMPLE_FREQUENCY 50
#define MOTOR_CURRENT_1_SAMPLE_FREQUENCY 50
#define BATTERY_SAMPLE_FREQUENCY 1
#define LINESCAN_0_SAMPLE_FREQUENCY 50

/* Profiler configuration */
#define NUM_PROFILER_ENTRIES 1
#define CONTROL_PROFILER 0

//This is what the exposure time for the Linescan will be set to out of bootup
#define TFC_DEFAULT_LINESCAN_EXPOSURE_TIME_uS	10000 //50000

#define TFC_MOTOR_SWITCHING_FREQUENCY	((float)(4000.0))

#define SDA_SERIAL_BAUD		460800	
#define SDA_SERIAL_OUTGOING_QUEUE_SIZE	2048
#define SDA_SERIAL_INCOMING_QUEUE_SIZE	128

/* Terminal defines from depreciated Terminal.h */
#define TERMINAL_OUT_QUEUE			
#define TERMINAL_IN_QUEUE			
#define TERMINAL_PRINTF(...)   		
#define TERMINAL_PUTC(c)        	
#define TERMINAL_READABLE       	
#define TERMINAL_GETC           	

/* SERIAL IO */
#define SERIAL_MAX_MSG_SIZE 384
//#define SERIAL_TX_DMA_ENABLED
#define SERIAL_TX_IRQ_ENABLED
#define SERIAL_TX_DMA_THRESHOLD 64
#define RB_RX_SIZE 256
#define RB_TX_SIZE 2048
#define RB_MAX_DMA_SIZE 256

/* TFTP configuration */
#define TFTP_TIMESTAMP_TOLERANCE 0.05

#endif /* TFC_CONFIG_H_ */
