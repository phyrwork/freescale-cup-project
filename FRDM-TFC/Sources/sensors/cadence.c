/*
 * sensors.c
 *
 *  Created: Nov 6, 2014
 *  Updated: Jan 7, 2015
 *  Author:  Connor Newton
 */

#include "sensors/cadence.h"
#include "devices/arm_cm0.h"

/* Sensor configurations and data */
CadenceSensor_s sensors[] = {
	/* [0] = */  {
		/* TPM = */       TPM2_BASE_PTR,
		/* channel = */   0,
		/* overflows = */ 0,
		/* period = */    0,
		/* events = */    0,
		/* epr = */       2
	},
	/* [1] = */ {
		/* TPM = */       TPM2_BASE_PTR,
		/* channel = */   1,
		/* overflows = */ 0,
		/* period = */    0,
		/* events = */    0,
		/* epr = */       2
	}
};
CadenceSensor_s *CadenceSensors = sensors; //"rename" for other files

/* Sensor count macro */
#define NUM_SENSORS (sizeof(sensors) / sizeof(CadenceSensor_s))

void CadenceSensors_Init()
{
	disable_irq(INT_TPM2 - 16);
	
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;

	GPIOA_PDDR &= 0xFFFFFFF9; //Configure PTA1 and PTA2 as inputs

	PORTA_PCR1 &= 0xFFFFF8FF;
	PORTA_PCR1 |= 0x00000300;
	PORTA_PCR2 &= 0xFFFFF8FF;
	PORTA_PCR2 |= 0x00000300;

	SIM_SCGC6 |= SIM_SCGC6_TPM2_MASK; //Enable clock gate to TPM2 module

	TPM2_SC |= 0x004F; //LPTPM counter increments on every LPTPM counter clock, prescaler of 128, interrupt on overflow
	TPM2_MOD = 0xFFFF;
	TPM2_C0SC = 0x0000 | 0b1001100; //Channel interrupts enabled, input capture mode, both edges
	TPM2_C1SC = 0x0000 | 0b1001100; //Channel interrupts enabled, input capture mode, both edges

	/* Pause timer when debug paused */
	TPM2_CONF |= 0x00000000;

	/* Clear event flags */
	TPM2_C0SC |= 0x0080;
	TPM2_C1SC |= 0x0080;

	/* Configure and enable TPM2 interrupts */
	set_irq_priority(INT_TPM2, 3); //Set to lowest priority
	enable_irq(INT_TPM2);
}

void InitSensor(CadenceSensor_s* sensor)
{
	/* Init individual sensors in here! */
}

void SensorEventHandler(CadenceSensor_s* sensor)
{
	/* If sensor X event flag is set */
	if (sensor->TPM->CONTROLS[sensor->channel].CnSC & TPM_CnSC_CHF_MASK)
	{
		/* Clear event flag */
		sensor->TPM->CONTROLS[sensor->channel].CnSC |= TPM_CnSC_CHF_MASK;
		sensor->event = 1; //signal to other modules new data

		/* Calculate period since last event */
		sensor->period = (uint32_t)sensor->TPM->CONTROLS[sensor->channel].CnV + ((uint32_t)0xFFFF - sensor->period);

		/* Account for LPTPM overflows */
		if (sensor->overflows) {
			sensor->period += ((uint32_t)0xFFFF * (sensor->overflows - 1)); //Add period equal to complete LPTPM period * # overflows.
			sensor->period *= sensor->epr; //Adjust period to represent complete revolution
			sensor->overflows = 0; //Reset overflow counter.
		}
	}
}

void FTM2_IRQHandler()
{
	uint8_t br = 0;
	
	for(uint8_t i = 0; i < NUM_SENSORS; ++i)
	{
		/* Select Hall sensor */
		CadenceSensor_s *sensor = &sensors[i];

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
