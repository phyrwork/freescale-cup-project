/*
 * PID.h
 *
 *  Created on: Dec 22, 2014
 *      Author: Connor
 */

#ifndef PID_H_
#define PID_H_

typedef struct {
	float Kp;
	float Ki;
	float Kd;
	float time;
	float value;
	float value_max;
	float value_min;
	float error;
	float integral;
} PID_s;

#endif /* PID_H_ */
