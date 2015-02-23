/* current.h
 * ==========================================
 * Current signal storage and filtering.
 */

#ifndef CURRENT_SENSOR_H
#define CURRENT_SENSOR_H

#include "support/rbuf_uint16.h"
#include "config.h"

/* Define motor current struct */
#define CURRENT_FILTER_BUFFER_SIZE (CURRENT_FILTER_ORDER * 3)
typedef struct {
	struct {
		  int16_t data[CURRENT_FILTER_BUFFER_SIZE];
		  uint16_t pos;
	} buffer;
	float value;
} MotorCurrent_s;
extern MotorCurrent_s MotorCurrent[NUM_MOTORS];

/* Methods */
void  InitCurrentSensors();
float UpdateCurrentValue(MotorCurrent_s *current);

#endif //CURRENT_SENSOR_H
