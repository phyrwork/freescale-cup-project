#include "sensors/camera/LineScanCamera.h" 
#include "devices/MKL25Z4.h"
#include "devices/CrystalClock.h"

//import timer
#include "support/ARM_SysTick.h"
#include "config.h"
#define TICKER TFC_Ticker[UPTIME_TICKER]

LineScanControl_s const shield = {
	.clk = {
		.port = PTE_BASE_PTR,
		.pin = 1
	},
	.si = {
		.port = PTD_BASE_PTR,
		.pin = 7
	}
};
LineScanControl_s const breakout = {
	.clk = {
		.port = PTC_BASE_PTR,
		.pin = 10
	},
	.si = {
		.port = PTC_BASE_PTR,
		.pin = 11
	}
};

LineScan_s linescan[2] = {
	//[0]
	{ //positioning
		.image =  &linescan[0].data[0][0],
		.buffer.data = &linescan[0].data[1][0],
		.signal = &shield,
		.exposure = {
				.time = TFC_DEFAULT_LINESCAN_EXPOSURE_TIME
		}
	},
	//[1]
	{ //lookahead
		.image =  &linescan[1].data[0][0],
		.buffer.data = &linescan[1].data[1][0],
		.signal = &breakout,
		.exposure = {
				.time = TFC_DEFAULT_LINESCAN_EXPOSURE_TIME
		}
	}
};

void TFC_InitLineScanCamera()
{
	//enable clocks to GPIO ports
	SIM_SCGC5 |=   SIM_SCGC5_PORTE_MASK
	             | SIM_SCGC5_PORTD_MASK
	             | SIM_SCGC5_PORTC_MASK;

	//mux pins to GPIO ports
	PORTE_PCR1  = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; //linescan0 clk
	PORTD_PCR7  = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; //linescan0 si
	PORTC_PCR10 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; //linescan1 clk
	PORTC_PCR11 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; //linescan1 si
	
	PORTD_PCR5 = PORT_PCR_MUX(0); //make sure linescan0? signal goes to an analog input
	PORTD_PCR6 = PORT_PCR_MUX(0); //make sure linescan1? signal goes to an analog input
		
	//make sure the clk and si pins are outputs
    GPIOD_PDDR |= (1<<7);
    GPIOE_PDDR |= (1<<1);
    GPIOC_PDDR |= (1<<10);
    GPIOC_PDDR |= (1<<11);
            
	LINESCAN_SIGNAL_LOW(shield.clk);
	LINESCAN_SIGNAL_LOW(shield.si);
	LINESCAN_SIGNAL_LOW(breakout.clk);
	LINESCAN_SIGNAL_LOW(breakout.si);
	
	PIT_LDVAL0 = TFC_DEFAULT_LINESCAN_EXPOSURE_TIME;
	PIT_TCTRL0 = PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK; //Enable PIT channel 0; enable interrupts.
}

int8_t LinescanProcess (LineScan_s *linescan, uint16_t data)
{
    if (linescan->buffer.pos < 128) //image capture sequence ongoing...
    {
        linescan->buffer.data[linescan->buffer.pos++] = data; //store pixel
        
        LINESCAN_SIGNAL_LOW(linescan->signal->clk);               
        for(uint8_t j = 0; j < 10; ++j) {}
        LINESCAN_SIGNAL_HIGH(linescan->signal->clk);

    	//if new exposure has just started
    	//if (linescan->buffer.pos == 19) linescan->exposure.start = TICKER;
    }
    else //image capture sequence complete
    {
        LINESCAN_SIGNAL_HIGH(linescan->signal->clk);                                  
        for(uint8_t j = 0; j < 10; ++j) {}
        LINESCAN_SIGNAL_LOW(linescan->signal->clk); //disconnect final pixel from output
        
        //swap the image buffer
        if(linescan->buffer.data == &linescan->data[0][0])
        {

            linescan->buffer.data = &linescan->data[1][0];
            linescan->image =       &linescan->data[0][0];
        }
        else
        {
            linescan->buffer.data = &linescan->data[0][0];
            linescan->image =       &linescan->data[1][0];
        }
        
        linescan->buffer.pos = 0;
    }
    return 0;
}
