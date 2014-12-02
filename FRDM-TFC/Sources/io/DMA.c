/* DMA.c
 * ==========================================
 * Direct Memory Access configuration and use
 */

#include "io/DMA.h"
#include "io/UART.h"

#include <stdint.h>

void DMAMUX0_Init()
{
	/* Initialise DMAMUX0 to route UART0 to DMA0 and configure
	   to enable peripheral request triggering */

	/* Set DMA0 source to UART0 Receive (2) n.b. Transmit is source 3 */
	DMAMUX[0]->CHCFG[0] &= DMAMUX_CHCFG_SOURCE_MASK;
	DMAMUX[0]->CHCFG[0] |= DMAMUX_CHCFG_SOURCE(2);

	/* Enable channel triggering (n.b. Ch 0/1 only) */
	DMAMUX[0]->CHCFG[0] |= DMAMUX_CHCFG_TRIG_MASK;

	/* Enable the DMA channel */
	DMAMUX[0]->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;
}

void DMA0_Init()
{
	/* Initialise DMA0 for use with
	   telemetry on UART0 */
	
	DMA[0]->DAR = (uint32_t) &UART0_D; //Set destination address

	DMA[0]->DCR |= DMA_DCR_EINT_MASK;   //Enable DMA interrupts
  //DMA[0]->DCR |= DMA_DCR_ERQ_MASK;    //Enable peripheral requests (this enabled by UART0_RearmDma)
	DMA[0]->DCR |= DMA_DCR_CS_MASK;     //Enable cycle stealing - one transfer per request
	DMA[0]->DCR &= DMA_DCR_AA_MASK;     //Disable auto-align - shouldn't make a difference when transfer is 1B>1B
	DMA[0]->DCR |= DMA_DCR_EADREQ_MASK; //Disable asychronous requests
	DMA[0]->DCR |= DMA_DCR_SINC_MASK;   //SAR increments after each copy/write
	DMA[0]->DCR &= DMA_DCR_SSIZE_MASK;
	DMA[0]->DCR |= DMA_DCR_SSIZE(0b01); //Set source data size to 8-bit
	DMA[0]->DCR &= DMA_DCR_DINC_MASK;   //No change to DAR after each copy/write
	DMA[0]->DCR &= DMA_DCR_DSIZE_MASK;
	DMA[0]->DCR |= DMA_DCR_DSIZE(0b01); //Set destination data size to 8-bit
	DMA[0]->DCR &= DMA_DCR_SMOD_MASK;
	DMA[0]->DCR &= DMA_DCR_DMOD_MASK;
	DMA[0]->DCR |= DMA_DCR_D_REQ_MASK;  //Clear ERQ bit when byte count register reaches zero
	DMA[0]->DCR &= DMA_DCR_LINKCC_MASK; //No channel linking
}

void DMA0_IRQHandler()
{
	/* If interrupt generated due to transfer completion */
	if(DMA[0]->DSR_BCR & DMA_DSR_BCR_DONE_MASK)
	{
		/* Write to DONE - this resets all flags in DSR */
		DMA[0]->DSR_BCR |= DMA_DSR_BCR_DONE_MASK;

		/* To do: Check amount of data available in TxBuffer */
		if (1)
		{
			/* Rearm DMA */
			//UART0_RearmDMA();
		}	
	}
}
