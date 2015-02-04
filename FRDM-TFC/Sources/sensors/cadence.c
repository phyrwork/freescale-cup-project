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
#define NUM_SENSORS 4

CadenceSensor_s CadenceSensors[NUM_SENSORS] = {
	//[0]
	{ //rr
		.TPM =     TPM2_BASE_PTR,
		.channel = 0,
		.epr =     6
	},
	//[1]
	{ //rl
		.TPM =     TPM2_BASE_PTR,
		.channel = 1,
		.epr =     6
	},
	//[2]
	{ //fr
		.TPM =     TPM0_BASE_PTR,
		.channel = 4,
		.epr =     36
	},
	//[3]
	{ //fl
		.TPM =     TPM0_BASE_PTR,
		.channel = 5,
		.epr =     36	
	}
};

void CadenceSensors_Init()
{
	disable_irq(INT_TPM0 - 16);
	disable_irq(INT_TPM2 - 16);
	
	//enable clock gates to PTB and PTC
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;

	GPIOB_PDDR &= 0xFFFFFFF3; //configure PTB2 and PTB3 as inputs
	GPIOC_PDDR &= ~0x300;     //configure PTC8 and PTC9 as inputs  

	//mux IO pins to TPM modules
	PORTB_PCR2 &= 0xFFFFF8FF;
	PORTB_PCR2 |= 0x00000300;
	
	PORTB_PCR3 &= 0xFFFFF8FF;
	PORTB_PCR3 |= 0x00000300;
	
	PORTC_PCR8 &= 0xFFFFF8FF;
	PORTC_PCR8 |= 0x00000300;
	
	PORTC_PCR9 &= 0xFFFFF8FF;
	PORTC_PCR9 |= 0x00000300;
	
	//enable clock gates to TPM modules
	SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK;
	SIM_SCGC6 |= SIM_SCGC6_TPM2_MASK;
	
	//pause TPM clocks
	TPM0_SC &= ~TPM_SC_CMOD_MASK;
	TPM2_SC &= ~TPM_SC_CMOD_MASK;

	//configure TPM module control
	TPM2_SC |= 0x004F; //LPTPM counter increments on every LPTPM counter clock, prescaler of 128, interrupt on overflow
	TPM2_MOD = 0xFFFF;
	TPM0_SC |= TPM_SC_TOIE_MASK; //enable TPM0 interrupts
	
	//configure individual TPM channels
	TPM2_C0SC = 0b1001100; //channel interrupts enabled, input capture mode, both edges
	TPM2_C1SC = 0b1001100;
	TPM0_C4SC = 0b1001100;
	TPM0_C5SC = 0b1001100;

	TPM2_CONF |= TPM_CONF_DBGMODE(3);

	//clear event flags
	TPM2_C0SC |= 0x0080;
	TPM2_C1SC |= 0x0080;
	TPM0_C4SC |= 0x0080;
	TPM0_C5SC |= 0x0080;
	
	//resume TPM clocks
	TPM0_SC |= TPM_SC_CMOD(1);
	TPM2_SC |= TPM_SC_CMOD(1);

	//configure and enable interrupts
	set_irq_priority(INT_TPM2 - 16, 3); //set to lowest priority
	set_irq_priority(INT_TPM0 - 16, 3);
	enable_irq(INT_TPM2 - 16);
	enable_irq(INT_TPM0 - 16);
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
			sensor->period += ((uint32_t)sensor->TPM->MOD * (sensor->overflows - 1)); //Add period equal to complete LPTPM period * # overflows.
			sensor->overflows = 0; //Reset overflow counter.
		}
		
		sensor->period <<= sensor->TPM->SC & TPM_SC_PS_MASK; //multiply by the counter prescale factor
		sensor->period *= sensor->epr; //adjust to represent complete revolution
		
		/* Push new period onto buffer */
		if (++sensor->buffer.pos >= CADENCE_BUFFER_SIZE - 1) sensor->buffer.pos = 0; //wrap buffer around
		sensor->buffer.data[sensor->buffer.pos] = sensor->period;
	}
}

void FTM2_IRQHandler()
{	
	for(uint8_t i = 0; i < 2; ++i) //rear sensors
	{
		CadenceSensor_s *sensor = &CadenceSensors[i]; //select sensor
		//if (sensor->TPM != TPM2_BASE_PTR) continue; //skip sensor if wrong module
		
		if (sensor->TPM->SC & TPM_SC_TOF_MASK) sensor->overflows++; //count overflows
		SensorEventHandler(sensor);                                 //update individual sensors
	}
	
	TPM2_SC |= TPM_SC_TOF_MASK; //clear the timer overflow flag if this is what called the interrupt
}

void FTM0_IRQHandler()
{
	for(uint8_t i = 2; i < 4; ++i) //front sensors
	{
		CadenceSensor_s *sensor = &CadenceSensors[i]; //select sensor
		//if (sensor->TPM != TPM0_BASE_PTR) continue; //skip sensor if wrong module
		
		if (sensor->TPM->SC & TPM_SC_TOF_MASK) sensor->overflows++; //count overflows
		SensorEventHandler(sensor);                                 //update individual sensors
	}
	
	TPM0_SC |= TPM_SC_TOF_MASK; //clear the timer overflow flag if this is what called the interrupt
}
