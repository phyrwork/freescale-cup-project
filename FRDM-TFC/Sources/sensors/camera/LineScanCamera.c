#include "sensors/camera/LineScanCamera.h" 
#include "devices/MKL25Z4.h"
#include "devices/CrystalClock.h"

LineScan_s linescan0 = {
	.image =  &linescan0.data[0][0],
	.buffer.data = &linescan0.data[1][0]
};
LineScan_s linescan1 = {
	.image =  &linescan1.data[0][0],
	.buffer.data = &linescan0.data[1][0]
};

void TFC_InitLineScanCamera()
{
	SIM_SCGC5 |=     SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTD_MASK; //Make sure the clock is enabled for PORTE;
	PORTE_PCR1 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;   //Enable GPIO on on the pin for the CLOCK Signal
	PORTD_PCR7 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;   //Enable GPIO on on the pin for SI signal
	
	PORTD_PCR5 = PORT_PCR_MUX(0); //Make sure AO signal goes to an analog input
	PORTD_PCR6 = PORT_PCR_MUX(0); //Make sure AO signal goes to an analog input
		
	//Make sure the Clock and SI pins are outputs
    GPIOD_PDDR |= (1<<7);
    GPIOE_PDDR |= (1<<1);
            
	TAOS_CLK_LOW;
	TAOS_SI_LOW;
}


void TFC_SetLineScanExposureTime(uint32_t  TimeIn_uS)
{
	/* Calculate number of PIT ticks for exposure time */
	float t;

	t = (TimeIn_uS /1000000.0f) * (float)(PERIPHERAL_BUS_CLOCK);
	PIT_LDVAL0 = (uint32_t)t; //Set PIT0 counter register
	
}

uint32_t TFC_getLineScanExposureTime(){
	return (uint32_t)(((float)PIT_LDVAL0 * 1000000.0f) / (float)(PERIPHERAL_BUS_CLOCK));
}
