/*
 * cadence.c
 *
 *  Created: Nov 6, 2014
 *  Updated: Jan 7, 2015
 *  Author:  Connor Newton
 */

#include "sensors/cadence.h"
#include "devices/arm_cm0.h"

/* Sensor configurations and data */
/* Sensor count macro */
//#define NUM_SENSORS (sizeof(CadenceSensors) / sizeof(CadenceSensor_s))
#define NUM_SENSORS NUM_CADENCE_SENSORS

CadenceSensor_s CadenceSensors[NUM_SENSORS] = {
	/* [0] (REAR_RIGHT) = */  {
		/* TPM = */       TPM2_BASE_PTR,
		/* channel = */   0,
		/* cnv = */       0,
		/* overflows = */ 0,
		/* period = */    0,
		/* epr = */       6,
		/* flag = */      0
	},
	/* [1] (REAR_LEFT) = */ {
		/* TPM = */       TPM2_BASE_PTR,
		/* channel = */   1,
		/* cnv = */       0,
		/* overflows = */ 0,
		/* period = */    0,
		/* epr = */       6,
		/* flag = */      0
	}
};

void CadenceSensors_Init()
{
	disable_irq(INT_TPM2 - 16);
	
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;

	GPIOB_PDDR &= 0xFFFFFFF3; //Configure PTB2 and PTB3 as inputs

	PORTB_PCR2 &= 0xFFFFF8FF;
	PORTB_PCR2 |= 0x00000300;
	PORTB_PCR3 &= 0xFFFFF8FF;
	PORTB_PCR3 |= 0x00000300; //Mux

	SIM_SCGC6 |= SIM_SCGC6_TPM2_MASK; //Enable clock gate to TPM2 module

	TPM2_SC |= 0x004F; //LPTPM counter increments on every LPTPM counter clock, prescaler of 128, interrupt on overflow
	TPM2_MOD = 0xFFFF;
	TPM2_C0SC = 0x0000 | 0b1001100; //Channel interrupts enabled, input capture mode, both edges
	TPM2_C1SC = 0x0000 | 0b1001100; //Channel interrupts enabled, input capture mode, both edges

	TPM2_CONF |= TPM_CONF_DBGMODE(3);

	/* Clear event flags */
	TPM2_C0SC |= 0x0080;
	TPM2_C1SC |= 0x0080;

	/* Configure and enable TPM2 interrupts */
	set_irq_priority(INT_TPM2 - 16, 3); //Set to lowest priority
	enable_irq(INT_TPM2 - 16);
}

void SensorEventHandler(CadenceSensor_s* sensor)
{
	/* If sensor X event flag is set */
	if (sensor->TPM->CONTROLS[sensor->channel].CnSC & TPM_CnSC_CHF_MASK)
	{
		/* Calculate period since last event */
		sensor->period = (uint16_t)(sensor->TPM->CONTROLS[sensor->channel].CnV + ((uint16_t)0xFFFF - sensor->cnv));
		sensor->cnv = sensor->TPM->CONTROLS[sensor->channel].CnV;

		/* Clear event flag */
		sensor->TPM->CONTROLS[sensor->channel].CnSC |= TPM_CnSC_CHF_MASK;
		sensor->flag = 1; //signal to other modules new data

		/* Account for LPTPM overflows */
		if (sensor->overflows) {
			sensor->period += ((uint32_t)0xFFFF * (sensor->overflows - 1)); //Add period equal to complete LPTPM period * # overflows.
			sensor->overflows = 0; //Reset overflow counter.
		}

		sensor->period *= sensor->epr; //adjust to represent complete revolution
	}
}

void FTM2_IRQHandler()
{	
	for(uint8_t i = 0; i < NUM_SENSORS; ++i)
	{
		/* Select Hall sensor */
		CadenceSensor_s *sensor = &CadenceSensors[i];

		/* If TPM module doesn't belong to this interrupt routine */
		if (sensor->TPM != TPM2_BASE_PTR) continue;

		/* -only- if timer overflow flag is set */
		if (sensor->TPM->SC & TPM_SC_TOF_MASK)
		{
			/* Clear timer flag if last sensor to be handled */
			if (i == NUM_SENSORS - 1) sensor->TPM->SC |= TPM_SC_TOF_MASK;

			/* Increment sensor overflow counter */
			sensor->overflows++;
		}

		/* Update period of sensor */
		SensorEventHandler(sensor);
	}
}
