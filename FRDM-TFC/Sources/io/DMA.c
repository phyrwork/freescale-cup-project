/* DMA.c
 * ==========================================
 * Direct Memory Access configuration and use
 */

#include "io/DMA.h"
#include "devices/arm_cm0.h"
#include "io/UART.h"
#include "io/RingBuffer.h"
#include "config.h"

#include <stdint.h>

void DMAMUX0_Init()
{
	/* Initialise DMAMUX0 to route UART0 to DMA0 and configure
	   to enable peripheral request triggering */
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK; //Enable clock gate to DMAMUX

	/* Set DMA0 source to UART0 Transmit (2) n.b. Receive is source 2 */
	DMAMUX0_CHCFG0 &= DMAMUX_CHCFG_SOURCE_MASK;
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_SOURCE(3);

	/* Enable channel triggering (n.b. Ch 0/1 only) */
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_TRIG_MASK;

	/* Enable the DMA channel */
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK;
}

void DMA0_Init()
{
	/* Initialise DMA0 for use with
	   telemetry on UART0 */
	
	DMAMUX0_Init(); //Configure DMAMUX
	SIM_SCGC7 |= SIM_SCGC7_DMA_MASK; //Enable clock gate to DMA module
	
	/* Enable DMA0 interrupts */
	enable_irq(INT_DMA0 - 16);
}

void DMA0_IRQHandler()
{
	/* If interrupt generated due to transfer completion */
	if(DMA_DSR_BCR0 & DMA_DSR_BCR_DONE_MASK)
	{
		/* Write to DONE - this resets all flags in DSR */
		DMA_DSR_BCR0 |= DMA_DSR_BCR_DONE_MASK;

		/* To do: Check amount of data available in TxBuffer */
		if ( rbUsed(&TxBuffer) > SERIAL_TX_DMA_THRESHOLD ) UART0_ArmDMA();
	}
}
