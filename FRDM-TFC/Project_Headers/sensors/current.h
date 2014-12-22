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
	uint16_t data[CURRENT_FILTER_BUFFER_SIZE];
	rbuf_uint16_s buffer;
	int16_t value;
} MotorCurrent_s;
extern MotorCurrent_s MotorCurrent[NUM_MOTORS];

/* Methods */
void    InitCurrentSensors();
int16_t GetCurrentValue(MotorCurrent_s *current);

#endif //CURRENT_SENSOR_H
