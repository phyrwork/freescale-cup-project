
#ifndef TFC_CONFIG_H_
#define TFC_CONFIG_H_

#define RELEASE					"1.0"

#define PCB_REV					A

/* Ticker configuration and index definitions */
//#define SYSTICK_FREQUENCY /* To do: Move #define here and remove hard coded dependencies on this value */
#define NUM_TFC_TICKERS 7
#define UPTIME_TICKER 4
#define COLLECTOR_TICKER 5
#define TRIGGER_TICKER 6

/* main() triggers configuration */
#define MAIN_TRIGGER_POLLLING_INTERVAL 50

#define CONTROL_TRIGGER_FREQUENCY 100
#define TELEMETRY_TRIGGER_FREQUENCY 100

/* Profiler configuration */
#define NUM_PROFILER_ENTRIES 1
#define CONTROL_PROFILER 0

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
#define RB_RX_SIZE 256
#define RB_TX_SIZE 2048

/* TFTP configuration */
#define TFTP_TIMESTAMP_TOLERANCE 0.05

#endif /* TFC_CONFIG_H_ */
