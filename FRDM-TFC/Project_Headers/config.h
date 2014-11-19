
#ifndef TFC_CONFIG_H_
#define TFC_CONFIG_H_

#define RELEASE					"1.0"

#define PCB_REV					A

//A TFC Ticker is a variable that will increment every 100uS in the Systick interrupt routine
//you can use it to for general purpose timing, scheduling events, etc.  The TFC_Ticker variable
//is just an array of 32-bit integers, use the Macro below to set how many tickers you need
#define NUM_TFC_TICKERS 5
#define TFTP_TICKER 4

//This is what the exposure time for the Linescan will be set to out of bootup
#define TFC_DEFAULT_LINESCAN_EXPOSURE_TIME_uS	10000 //50000

#define TFC_MOTOR_SWITCHING_FREQUENCY	((float)(4000.0))


#define TERMINAL_USE_SDA_SERIAL
#define SDA_SERIAL_BAUD		115200	
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
#define SERIAL_MAX_MSG_SIZE 512
#define RB_RX_SIZE 2048
#define RB_TX_SIZE 256
#define RB_MAX_DMA_SIZE 256

#endif /* TFC_CONFIG_H_ */
