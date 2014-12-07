#include "io/UART.h"
#include "devices/MKL25Z4.h"
#include "devices/arm_cm0.h"
#include "devices/CrystalClock.h"
#include "support/ARM_SysTick.h"
#include "config.h"
#include "support/rbuf.h"
#include "io/Frame.h"
#include "io/DMA.h"
#include "io/GPIO.h"

/* RingBuffer storage and structures */
uint8_t RxBufferData[RB_RX_SIZE];
uint8_t TxBufferData[RB_TX_SIZE];
rbuf_s  RxBuffer;
rbuf_s  TxBuffer;


////////////////////////
// UART0 User Methods //
////////////////////////

/* Encapsulate message and add to transmit buffer */
int8_t UART0_Send(uint8_t * msg, uint16_t size) {
	
	/* Encapsulate message */
	uint8_t buffer[FR_MAX_ENC_SIZE];
	size = SerialEncode(msg, size, buffer);
	
	/* Push message onto transmit buffer */
	uint32_t write = rbuf_write(&TxBuffer, buffer, size);
	if (write != size) return -1; //Not all data was copied to buffer
	
	/* Enable UART transmission if not already enabled */
    #ifdef SERIAL_TX_IRQ_ENABLED
		//UART0_ArmIRQ();
    #endif
    #ifdef SERIAL_TX_DMA_ENABLED
		/* Cause a compile error */
		DMA_IS_UNSUPPORTED_IN_RBUF
	
		/* If DMA0 is not busy (i.e. there is no ongoing transfer */
		if( !(DMA_DSR_BCR0 & DMA_DSR_BCR_BSY_MASK) )
		{
			/* And only arm DMA if sufficient data ready for transmission */
			if ( rbUsed(&TxBuffer) > SERIAL_TX_DMA_THRESHOLD ) UART0_ArmDMA();
		}
	#endif
		
	return 0;
}

/* Add message to transmit buffer without encapsualtion */
int8_t UART0_SendRaw(uint8_t * msg, uint16_t size) {
	/* Write to buffer without encapsulation */
    uint32_t write = rbuf_write(&TxBuffer, msg, size);
	if (write != size) return -1; //Not all data was copied to buffer
    
    /* Enable UART transmission if not already enabled */
    #ifdef SERIAL_TX_IRQ_ENABLED
		//UART0_ArmIRQ();
    #endif
    #ifdef SERIAL_TX_DMA_ENABLED
		/* Cause a compile error */
		DMA_IS_UNSUPPORTED_IN_RBUF
	
		/* If DMA0 is not busy (i.e. there is no ongoing transfer */
		if( !(DMA_DSR_BCR0 & DMA_DSR_BCR_BSY_MASK) )
		{
			/* And only arm DMA if sufficient data ready for transmission */
			if ( rbUsed(&TxBuffer) > SERIAL_TX_DMA_THRESHOLD ) UART0_ArmDMA();
		}
	#endif

    return 0;
}


//////////////////////////
// UART0 Driver Methods //
//////////////////////////

inline void UART0_ArmIRQ()
{
	/* If data in transmitter buffer */ 
	if(rbuf_used(&TxBuffer) && (UART0_S1 & UART_S1_TDRE_MASK))
		UART0_C2 |= UART_C2_TIE_MASK; //Enable Transmitter Interrupts
}

inline void UART0_DisarmIRQ() { UART0_C2 &= ~UART_C2_TIE_MASK;  }

void UART0_ArmDMA()
{
	/* Get pointer and transfer length from buffer */
	//Vector8u contig = rbPopDma(&TxBuffer);
	
	/* Configure DMATCD */
	//DMA_SAR0 = (uint32_t) contig.ptr;
	DMA_DAR0 = (uint32_t) &UART0_D; //Set destination address
	//DMA_DSR_BCR0 |= ((uint32_t) 0x00FFFFFF) & (uint32_t) contig.size;
	DMA_DCR0 =  (DMA_DCR_EINT_MASK)    //Enable DMA interrupts
			 |  (DMA_DCR_ERQ_MASK)     //Enable peripheral requests (this enabled by UART0_RearmDma)
			 |  (DMA_DCR_CS_MASK)      //Enable cycle stealing - one transfer per request
			 &  (~DMA_DCR_AA_MASK)     //Disable auto-align - shouldn't make a difference when transfer is 1B>1B
			 &  (~DMA_DCR_EADREQ_MASK) //Disable asychronous requests
			 |  (DMA_DCR_SINC_MASK)    //SAR increments after each copy/write
			 &  (~DMA_DCR_SSIZE_MASK) 
			 |  (DMA_DCR_SSIZE(0b01))  //Set source data size to 8-bit
			 &  (~DMA_DCR_DINC_MASK)   //No change to DAR after each copy/write
			 &  (~DMA_DCR_DSIZE_MASK)
			 |  (DMA_DCR_DSIZE(0b01))  //Set destination data size to 8-bit
			 &  (~DMA_DCR_SMOD_MASK)
			 &  (~DMA_DCR_DMOD_MASK) 
			 |  (DMA_DCR_D_REQ_MASK)   //Clear ERQ bit when byte count register reaches zero
			 &  (~DMA_DCR_LINKCC_MASK) //No channel linking
	 	 	 |  (DMA_DCR_ERQ_MASK);    //Enable peripheral requests
}

void UART0_IRQHandler()
{
	uint8_t temp;

	/////////////
	// RECEIVE //
	/////////////
	
	/* If receive register full flag is set */
	if(UART0_S1 & UART_S1_RDRF_MASK)
	{
		/* Push data from UART onto receive buffer */
		rbuf_write(&RxBuffer, &UART0_D, 1);
	}


	//////////////
	// TRANSMIT //
	//////////////

	/* If transmitter data register empty flag set */
	if(UART0_S1 & UART_S1_TDRE_MASK)
	{
		/* If there is data in transmitter buffer */
		if(rbuf_used(&TxBuffer))
		{
			/* Pop value from transmitter buffer */
			rbuf_read(&TxBuffer, &temp, 1);
			UART0_D = temp; //Write TX data to UART data register.
		}
		/* Otherwise... */
		else
		{
			//if there is nothing left in the queue then disable interrupts
			UART0_C2 &= ~UART_C2_TIE_MASK; //Disable the  Interrupts
		}
	}
}


//////////////////////////////////
// UART0 Initialization Methods //
//////////////////////////////////

void UART0_Init()
{
	/* Enable clock gates to UART0 peripheral and IO pins */
    SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	/* Initialise ring buffers */
	rbuf_init(&RxBuffer, RxBufferData, sizeof RxBufferData);
	rbuf_init(&TxBuffer, TxBufferData, sizeof TxBufferData);
	
	PORTA_PCR1 = PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;   
	PORTA_PCR2 = PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;  
	
	//Select PLL/2 Clock
	SIM_SOPT2 &= ~(3<<26);
	SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); 
	SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
	
	//We have to feed this function the clock in KHz!
    UART0_ConfigureDataRate(CORE_CLOCK/2/1000, USB_SERIAL_BAUD);
	#ifdef BLUETOOTH_ENABLED
    	UART0_ConfigureDataRate(CORE_CLOCK/2/1000, BLUETOOTH_SERIAL_BAUD_DEFAULT);
	#endif //BLUETOOTH_ENABLED   
    
    //Enable transmitter DMA requests
    //UART0_C4 |= UART_C4_TDMAS_MASK;
    UART0_C5 |= UART0_C5_TDMAE_MASK;
     
	//Enable receive interrupts
    UART0_C2 |= UART_C2_RIE_MASK;
    enable_irq(INT_UART0-16);
}

void UART0_ConfigureDataRate (int sysclk, int baud)
{
    uint8_t i;
    uint32_t calculated_baud = 0;
    uint32_t baud_diff = 0;
    uint32_t osr_val = 0;
    uint32_t sbr_val, uart0clk;
    uint32_t baud_rate;
    uint32_t reg_temp = 0;
    uint32_t temp = 0;
    
    // Disable UART0 before changing registers
    UART0_C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);
  
    // Verify that a valid clock value has been passed to the function 
    if ((sysclk > 50000) || (sysclk < 32))
    {
        sysclk = 0;
        reg_temp = SIM_SOPT2;
        reg_temp &= ~SIM_SOPT2_UART0SRC_MASK;
        reg_temp |= SIM_SOPT2_UART0SRC(0);
        SIM_SOPT2 = reg_temp;
			
			  // Enter inifinite loop because the 
			  // the desired system clock value is 
			  // invalid!!
			  while(1)
				{}
    }
   
    
    // Initialize baud rate
    baud_rate = baud;
    
    // Change units to Hz
    uart0clk = sysclk * 1000;
    // Calculate the first baud rate using the lowest OSR value possible.  
    i = 4;
    sbr_val = (uint32_t)(uart0clk/(baud_rate * i));
    calculated_baud = (uart0clk / (i * sbr_val));
        
    if (calculated_baud > baud_rate)
        baud_diff = calculated_baud - baud_rate;
    else
        baud_diff = baud_rate - calculated_baud;
    
    osr_val = i;
        
    // Select the best OSR value
    for (i = 5; i <= 32; i++)
    {
        sbr_val = (uint32_t)(uart0clk/(baud_rate * i));
        calculated_baud = (uart0clk / (i * sbr_val));
        
        if (calculated_baud > baud_rate)
            temp = calculated_baud - baud_rate;
        else
            temp = baud_rate - calculated_baud;
        
        if (temp <= baud_diff)
        {
            baud_diff = temp;
            osr_val = i; 
        }
    }
    
    if (baud_diff < ((baud_rate / 100) * 3))
    {
        // If the OSR is between 4x and 8x then both
        // edge sampling MUST be turned on.  
        if ((osr_val >3) && (osr_val < 9))
            UART0_C5|= UART0_C5_BOTHEDGE_MASK;
        
        // Setup OSR value 
        reg_temp = UART0_C4;
        reg_temp &= ~UART0_C4_OSR_MASK;
        reg_temp |= UART0_C4_OSR(osr_val-1);
    
        // Write reg_temp to C4 register
        UART0_C4 = reg_temp;
        
        reg_temp = (reg_temp & UART0_C4_OSR_MASK) + 1;
        sbr_val = (uint32_t)((uart0clk)/(baud_rate * (reg_temp)));
        
         /* Save off the current value of the uartx_BDH except for the SBR field */
        reg_temp = UART0_BDH & ~(UART0_BDH_SBR(0x1F));
   
        UART0_BDH = reg_temp |  UART0_BDH_SBR(((sbr_val & 0x1F00) >> 8));
        UART0_BDL = (uint8_t)(sbr_val & UART0_BDL_SBR_MASK);
        
        /* Enable receiver and transmitter */
        UART0_C2 |= (UART0_C2_TE_MASK
                    | UART0_C2_RE_MASK );
    }
    else
		{
        // Unacceptable baud rate difference
        // More than 3% difference!!
        // Enter infinite loop!
        //while(1)
			//	{}
		}					
}
