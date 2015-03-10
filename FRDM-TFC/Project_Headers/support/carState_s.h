/*
 * carState_s.h
 *
 *  Created on: Feb 18, 2014
 *      Author: Matt
 */

#ifndef CARSTATE_S_H_
#define CARSTATE_S_H_

#include <stdint.h>

typedef enum {FORCED_DISABLED, RUNNING} motorState_t;
typedef enum {SINGLE_SPEED_SINGLE_UART, DUAL_SPEED_NO_UART, UNDEFINED} UARTSpeedState_t;
typedef enum {LINE_FOUND, LINE_TEMPORARILY_LOST, STOPLINE_DETECTED, LINE_LOST, LINE_NOT_FOUND} lineDetectionState_t;
typedef enum {LINESCAN_IMAGE_READY, NO_NEW_LINESCAN_IMAGE} lineScanState_t;

typedef struct {
	motorState_t motorState;
	UARTSpeedState_t UARTSpeedState;
	lineDetectionState_t lineDetectionState;
	lineScanState_t lineScanState;	
	int8_t lineCenter;
	float servoPosition;
}carState_s;

extern carState_s carState;

#endif /* CARSTATE_S_H_ */
