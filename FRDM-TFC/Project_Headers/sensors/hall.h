/*
 *	sensors.h
 *
 *  Created: Nov 6, 2014
 *  Updated: Nov 23, 2014
 *  Author:  Connor Newton
 */

#ifndef HALL_SENSORS_H_
#define HALL_SENSORS_H_

typedef struct {
  TPM_MemMapPtr TPM;       //pointer to TPM module struct
  uint8_t       channel;   //channel occupied by sensor
  uint32_t      overflows; //number of times parent timer has overflowed during current period
  uint32_t      ticks;     //most recent event period in ticks
  uint32_t      events;    //events counter
} HallSensor_s;

extern HallSensor_s *HallSensors;

void InitHallSensors();
void FTM2_IRQHandler();

#endif /* HALL_SENSORS_H_ */
