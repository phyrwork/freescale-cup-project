#include "support/carState_s.h"

#ifndef TFC_ADC_H_
#define TFC_ADC_H_

#define NULL_CALLBACK 0
typedef int8_t (*AdcCallback_f)(void);

typedef enum { MUX_A, MUX_B } AdcMux_e;

typedef struct {
	const uint8_t channel;
	const AdcMux_e mux;
	uint16_t *data;
	const AdcCallback_f callback;
} AdcConfig_s;


void TFC_InitADCs();

int8_t Sampler_Push(AdcConfig_s *config);

void PIT0_IRQ();
void ADC0_IRQ();

float TFC_ReadPot(uint8_t Channel);
float TFC_ReadMotorCurrent(uint8_t channel);
float TFC_ReadBatteryVoltage();

/* Shared data for use by Collector */
extern volatile uint16_t PotADC_Value[2];
extern volatile uint16_t MotorCurrentADC_Value[2];
extern volatile uint16_t BatSenseADC_Value;

#endif /* __ADC16_H__ */

