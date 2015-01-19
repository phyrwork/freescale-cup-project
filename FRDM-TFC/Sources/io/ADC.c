#include "io/ADC.h"
#include "devices/MKL25Z4.h"
#include "devices/arm_cm0.h"
#include "devices/CrystalClock.h"
#include "sensors/camera/LineScanCamera.h"
#include "support/rbuf_voidptr.h"
#include "config.h"
#include "main.h"

#define TFC_MOTOR_CURRENT_0_CHANNEL 7 //7a
#define TFC_MOTOR_CURRENT_1_CHANNEL 3
#define TFC_BAT_SENSE_CHANNEL		4
#define TFC_LINESCAN0_ADC_CHANNEL	6 //6b
#define TFC_LINESCAN1_ADC_CHANNEL	7 //7b

#define ADC_MAX_CODE    (4095) //12-bit


// Bit shifting of bitfiled is already taken into account so 
// bitfiled values are always represented as relative to their position.
  
/************************* #Defines ******************************************/


#define A                 0x0
#define B                 0x1

// Conversion Complete (COCO) mask
#define COCO_COMPLETE     ADC_SC1_COCO_MASK
#define COCO_NOT          0x00

// ADC interrupts: enabled, or disabled.
#define AIEN_ON           ADC_SC1_AIEN_MASK
#define AIEN_OFF          0x00

// Differential or Single ended ADC input
#define DIFF_SINGLE       0x00
#define DIFF_DIFFERENTIAL ADC_SC1_DIFF_MASK

//// ADCCFG1

// Power setting of ADC
#define ADLPC_LOW         ADC_CFG1_ADLPC_MASK
#define ADLPC_NORMAL      0x00

// Clock divisor
#define ADIV_1            0x00
#define ADIV_2            0x01
#define ADIV_4            0x02
#define ADIV_8            0x03

// Long samle time, or Short sample time
#define ADLSMP_LONG       ADC_CFG1_ADLSMP_MASK
#define ADLSMP_SHORT      0x00

// How many bits for the conversion?  8, 12, 10, or 16 (single ended).
#define MODE_8            0x00
#define MODE_12           0x01
#define MODE_10           0x02
#define MODE_16           0x03



// ADC Input Clock Source choice? Bus clock, Bus clock/2, "altclk", or the 
//                                ADC's own asynchronous clock for less noise
#define ADICLK_BUS        0x00
#define ADICLK_BUS_2      0x01
#define ADICLK_ALTCLK     0x02
#define ADICLK_ADACK      0x03

//// ADCCFG2

// Select between B or A channels
#define MUXSEL_ADCB       ADC_CFG2_MUXSEL_MASK
#define MUXSEL_ADCA       0x00

// Ansync clock output enable: enable, or disable the output of it
#define ADACKEN_ENABLED   ADC_CFG2_ADACKEN_MASK
#define ADACKEN_DISABLED  0x00

// High speed or low speed conversion mode
#define ADHSC_HISPEED     ADC_CFG2_ADHSC_MASK
#define ADHSC_NORMAL      0x00

// Long Sample Time selector: 20, 12, 6, or 2 extra clocks for a longer sample time
#define ADLSTS_20          0x00
#define ADLSTS_12          0x01
#define ADLSTS_6           0x02
#define ADLSTS_2           0x03

////ADCSC2

// Read-only status bit indicating conversion status
#define ADACT_ACTIVE       ADC_SC2_ADACT_MASK
#define ADACT_INACTIVE     0x00

// Trigger for starting conversion: Hardware trigger, or software trigger.
// For using PDB, the Hardware trigger option is selected.
#define ADTRG_HW           ADC_SC2_ADTRG_MASK
#define ADTRG_SW           0x00

// ADC Compare Function Enable: Disabled, or Enabled.
#define ACFE_DISABLED      0x00
#define ACFE_ENABLED       ADC_SC2_ACFE_MASK

// Compare Function Greater Than Enable: Greater, or Less.
#define ACFGT_GREATER      ADC_SC2_ACFGT_MASK
#define ACFGT_LESS         0x00

// Compare Function Range Enable: Enabled or Disabled.
#define ACREN_ENABLED      ADC_SC2_ACREN_MASK
#define ACREN_DISABLED     0x00

// DMA enable: enabled or disabled.
#define DMAEN_ENABLED      ADC_SC2_DMAEN_MASK
#define DMAEN_DISABLED     0x00

// Voltage Reference selection for the ADC conversions
// (***not*** the PGA which uses VREFO only).
// VREFH and VREFL (0) , or VREFO (1).

#define REFSEL_EXT         0x00
#define REFSEL_ALT         0x01
#define REFSEL_RES         0x02     /* reserved */
#define REFSEL_RES_EXT     0x03     /* reserved but defaults to Vref */

////ADCSC3

// Calibration begin or off
#define CAL_BEGIN          ADC_SC3_CAL_MASK
#define CAL_OFF            0x00

// Status indicating Calibration failed, or normal success
#define CALF_FAIL          ADC_SC3_CALF_MASK
#define CALF_NORMAL        0x00

// ADC to continously convert, or do a sinle conversion
#define ADCO_CONTINUOUS    ADC_SC3_ADCO_MASK
#define ADCO_SINGLE        0x00

// Averaging enabled in the ADC, or not.
#define AVGE_ENABLED       ADC_SC3_AVGE_MASK
#define AVGE_DISABLED      0x00

// How many to average prior to "interrupting" the MCU?  4, 8, 16, or 32
#define AVGS_4             0x00
#define AVGS_8             0x01
#define AVGS_16            0x02
#define AVGS_32            0x03

////PGA

// PGA enabled or not?
#define PGAEN_ENABLED      ADC_PGA_PGAEN_MASK
#define PGAEN_DISABLED     0x00 

// Chopper stabilization of the amplifier, or not.
#define PGACHP_CHOP        ADC_PGA_PGACHP_MASK 
#define PGACHP_NOCHOP      0x00

// PGA in low power mode, or normal mode.
#define PGALP_LOW          ADC_PGA_PGALP_MASK
#define PGALP_NORMAL       0x00

// Gain of PGA.  Selectable from 1 to 64.
#define PGAG_1             0x00
#define PGAG_2             0x01
#define PGAG_4             0x02
#define PGAG_8             0x03
#define PGAG_16            0x04
#define PGAG_32            0x05
#define PGAG_64            0x06

/////////// The above values fit into the structure below to select ADC/PGA
/////////// configuration desired:

typedef struct adc_cfg {
  uint8_t  CONFIG1; 
  uint8_t  CONFIG2; 
  uint16_t COMPARE1; 
  uint16_t COMPARE2; 
  uint8_t  STATUS2;
  uint8_t  STATUS3; 
  uint8_t  STATUS1A; 
  uint8_t  STATUS1B;
  uint32_t PGA;
  } *tADC_ConfigPtr, tADC_Config ;  
  

#define CAL_BLK_NUMREC 18 

typedef struct adc_cal {
 
uint16_t  OFS;
uint16_t  PG;
uint16_t  MG;
uint8_t   CLPD;
uint8_t   CLPS;
uint16_t  CLP4;
uint16_t  CLP3;
uint8_t   CLP2;
uint8_t   CLP1;
uint8_t   CLP0;
uint8_t   dummy;
uint8_t   CLMD;
uint8_t   CLMS;
uint16_t  CLM4;
uint16_t  CLM3;
uint8_t   CLM2;
uint8_t   CLM1;
uint8_t   CLM0;
} tADC_Cal_Blk ;  


// function prototypes:

uint8_t ADC_Cal(ADC_MemMapPtr);

void ADC_Config_Alt(ADC_MemMapPtr, tADC_ConfigPtr);

void ADC_Read_Cal(ADC_MemMapPtr, tADC_Cal_Blk *);

#define ADC_STATE_INIT							0
#define ADC_STATE_CAPTURE_POT_0			        1
#define ADC_STATE_CAPTURE_POT_1			        2
#define ADC_MOTOR_CURRENT_0						3
#define ADC_MOTOR_CURRENT_1						4
#define ADC_STATE_CAPTURE_BATTERY_LEVEL			5
#define ADC_STATE_CAPTURE_LINE_SCAN		        6

static carState_s* carState;

void InitADC0();

/******************************************************************************
Function 1. Name	AUTO CAL ROUTINE   

Parameters		ADC module pointer points to adc0 or adc1 register map 
                         base address.
Returns			Zero indicates success.
Notes         		Calibrates the ADC16. Required to meet specifications 
                        after reset and before a conversion is initiated.
******************************************************************************/
unsigned char ADC_Cal(ADC_MemMapPtr adcmap)
{

  unsigned short cal_var;
  
  ADC_SC2_REG(adcmap) &=  ~ADC_SC2_ADTRG_MASK ; // Enable Software Conversion Trigger for Calibration Process    - ADC0_SC2 = ADC0_SC2 | ADC_SC2_ADTRGW(0);   
  ADC_SC3_REG(adcmap) &= ( ~ADC_SC3_ADCO_MASK & ~ADC_SC3_AVGS_MASK ); // set single conversion, clear avgs bitfield for next writing
  ADC_SC3_REG(adcmap) |= ( ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(AVGS_32) );  // Turn averaging ON and set at max value ( 32 )
  
  
  ADC_SC3_REG(adcmap) |= ADC_SC3_CAL_MASK ;      // Start CAL
  while ( (ADC_SC1_REG(adcmap,A) & ADC_SC1_COCO_MASK ) == COCO_NOT ); // Wait calibration end
  	
  if ((ADC_SC3_REG(adcmap)& ADC_SC3_CALF_MASK) == CALF_FAIL )
  {  
   return(1);    // Check for Calibration fail error and return 
  }
  // Calculate plus-side calibration
  cal_var = 0x00;
  
  cal_var =  ADC_CLP0_REG(adcmap); 
  cal_var += ADC_CLP1_REG(adcmap);
  cal_var += ADC_CLP2_REG(adcmap);
  cal_var += ADC_CLP3_REG(adcmap);
  cal_var += ADC_CLP4_REG(adcmap);
  cal_var += ADC_CLPS_REG(adcmap);

  cal_var = cal_var/2;
  cal_var |= 0x8000; // Set MSB

  ADC_PG_REG(adcmap) = ADC_PG_PG(cal_var);
 

  // Calculate minus-side calibration
  cal_var = 0x00;

  cal_var =  ADC_CLM0_REG(adcmap); 
  cal_var += ADC_CLM1_REG(adcmap);
  cal_var += ADC_CLM2_REG(adcmap);
  cal_var += ADC_CLM3_REG(adcmap);
  cal_var += ADC_CLM4_REG(adcmap);
  cal_var += ADC_CLMS_REG(adcmap);

  cal_var = cal_var/2;

  cal_var |= 0x8000; // Set MSB

  ADC_MG_REG(adcmap) = ADC_MG_MG(cal_var); 
  
  ADC_SC3_REG(adcmap) &= ~ADC_SC3_CAL_MASK ; /* Clear CAL bit */

  return(0);
}




/******************************************************************************
Function 2 Name 	ADC_Config_Alt 
Parameters		the register values to be set in the adc in a structure
Returns			NONE
Notes         		Configures ADC0 or ADC1 depending on adcmap
                        Prior to calling this function populate the structure
                        elements with the desired ADC configuration.
******************************************************************************/


void ADC_Config_Alt(ADC_MemMapPtr adcmap, tADC_ConfigPtr ADC_CfgPtr)
{
 ADC_CFG1_REG(adcmap) = ADC_CfgPtr->CONFIG1;
 ADC_CFG2_REG(adcmap) = ADC_CfgPtr->CONFIG2;
 ADC_CV1_REG(adcmap)  = ADC_CfgPtr->COMPARE1; 
 ADC_CV2_REG(adcmap)  = ADC_CfgPtr->COMPARE2;
 ADC_SC2_REG(adcmap)  = ADC_CfgPtr->STATUS2;
 ADC_SC3_REG(adcmap)  = ADC_CfgPtr->STATUS3;
 //ADC_PGA_REG(adcmap)  = ADC_CfgPtr->PGA;
 ADC_SC1_REG(adcmap,A)= ADC_CfgPtr->STATUS1A;       
 ADC_SC1_REG(adcmap,B)= ADC_CfgPtr->STATUS1B;
}


void ADC_Read_Cal(ADC_MemMapPtr adcmap, tADC_Cal_Blk *blk)
{
  blk->OFS  = ADC_OFS_REG(adcmap);
  blk->PG   = ADC_PG_REG(adcmap); 
  blk->MG   = ADC_MG_REG(adcmap); 
  blk->CLPD = ADC_CLPD_REG(adcmap); 
  blk->CLPS = ADC_CLPS_REG(adcmap); 
  blk->CLP4 = ADC_CLP4_REG(adcmap);
  blk->CLP3 = ADC_CLP3_REG(adcmap); 
  blk->CLP2 = ADC_CLP2_REG(adcmap); 
  blk->CLP1 = ADC_CLP1_REG(adcmap);
  blk->CLP0 = ADC_CLP0_REG(adcmap);
  blk->CLMD = ADC_CLMD_REG(adcmap); 
  blk->CLMS = ADC_CLMS_REG(adcmap); 
  blk->CLM4 = ADC_CLM4_REG(adcmap);
  blk->CLM3 = ADC_CLM3_REG(adcmap); 
  blk->CLM2 = ADC_CLM2_REG(adcmap); 
  blk->CLM1 = ADC_CLM1_REG(adcmap);
  blk->CLM0 = ADC_CLM0_REG(adcmap);
  
}



void InitADC0()
{
	tADC_Config Master_Adc0_Config;
	
	
    SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK);
    
    //Lets calibrate the ADC. 1st setup how the channel will be used.

    disable_irq(INT_ADC0-16);   
     
    Master_Adc0_Config.CONFIG1 = ADLPC_NORMAL 			//No low power mode
								| ADC_CFG1_ADIV(ADIV_1) //divide clock input by 4
								| ADLSMP_LONG 			//long sample time
								| ADC_CFG1_MODE(MODE_12)//single ended 8-bit conversion
								| ADC_CFG1_ADICLK(ADICLK_BUS);
    
    Master_Adc0_Config.CONFIG2 = MUXSEL_ADCA // select the A side of the ADC channel.
								| ADACKEN_DISABLED
								| ADHSC_HISPEED
								| ADC_CFG2_ADLSTS(ADLSTS_2);//Extra long sample Time (20 extra clocks)
    
    
    Master_Adc0_Config.COMPARE1 = 00000; // Comparators don't matter for calibration
    Master_Adc0_Config.COMPARE1 = 0xFFFF;
    
    Master_Adc0_Config.STATUS2  = ADTRG_HW //hardware triggers for calibration
                               | ACFE_DISABLED //disable comparator
                               | ACFGT_GREATER
                               | ACREN_ENABLED
                               | DMAEN_DISABLED //Disable DMA
                               | ADC_SC2_REFSEL(REFSEL_EXT); //External Reference
        
    Master_Adc0_Config.STATUS3 = CAL_OFF  
								| ADCO_SINGLE
    							| AVGE_ENABLED
								| ADC_SC3_AVGS(AVGS_4);
								
    Master_Adc0_Config.PGA =     0; // Disable the PGA
   
   
    // Configure ADC as it will be used, but because ADC_SC1_ADCH is 31,
    // the ADC will be inactive.  Channel 31 is just disable function.
    // There really is no channel 31.
    
    Master_Adc0_Config.STATUS1A = AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(31);

    
    ADC_Config_Alt(ADC0_BASE_PTR, &Master_Adc0_Config);  // config ADC
 
    // Calibrate the ADC in the configuration in which it will be used:
     ADC_Cal(ADC0_BASE_PTR);                    // do the calibration
     
     
     Master_Adc0_Config.STATUS2  = ACFE_DISABLED //disable comparator
                                | ACFGT_GREATER
                                | ACREN_ENABLED
                                | DMAEN_DISABLED //Disable DMA
                                | ADC_SC2_REFSEL(REFSEL_EXT); //External Reference
    
     Master_Adc0_Config.STATUS3 = CAL_OFF  
     							| ADCO_SINGLE;
     	 	 	 	 	 	 	
     	 	 	 	 	 	 	
     			
     ADC_Config_Alt(ADC0_BASE_PTR, &Master_Adc0_Config);
}


void Sampler_Init();
void TFC_InitADCs(carState_s* carStateInputPointer)
{
	InitADC0();

    carState = carStateInputPointer;

  //The pump will be primed with the PIT interrupt.  upon timeout/interrupt it will set the SI signal high
	//for the camera and then start the conversions for the pots.
	
  /* (PIT - Periodic Interrupt Timer)
   * Configure PIT0
   */
	SIM_SCGC6 |= SIM_SCGC6_PIT_MASK; //Enable clock to the PIT module.
	PIT_TCTRL0 = PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK; //Enable PIT channel 0; enable interrupts.
	PIT_MCR |= PIT_MCR_FRZ_MASK; //Pause PIT0 when in debug mode.
	PIT_MCR &= ~PIT_MCR_MDIS_MASK; //Enable PIT module (Reset = 1 - disabled).

  Sampler_Init();

  TFC_SetLineScanExposureTime(TFC_DEFAULT_LINESCAN_EXPOSURE_TIME_uS);
	
  /* Configure and enable interrupts */
	set_irq_priority (INT_PIT - 16, 0);
	set_irq_priority (INT_ADC0 - 16, 1);
	enable_irq(INT_PIT-16);
	enable_irq(INT_ADC0-16);
}

///////////////////////////
// ADC CONVERISON DRIVER //
///////////////////////////

#include "support/ARM_SysTick.h"
#define ADC_TICKER TFC_Ticker[ADC_SAMPLE_TICKER]
#define UPTIME TFC_Ticker[UPTIME_TICKER]

typedef int8_t (*AdcPrime_f)(void);
typedef int8_t (*AdcComplete_f)(void);

typedef struct {
    AdcPrime_f    primer;
    AdcComplete_f completer;
} AdcConfig_s;


//////////////////////////////////
// Linescan-related definitions //
// and ADC configuration        //
//////////////////////////////////

#define LINESCAN_WAITING 0
#define LINESCAN_HOLD 1
#define LINESCAN_SCANNING 2
#define LINESCAN_DONE 3

typedef struct {
    //uint16_t __data[2][128];
    //uint16_t  *data;
    uint8_t      pixel;
    AdcConfig_s *AdcConfig;
} LinescanImage;

LinescanImage  linescan0;
AdcConfig_s    AdcConfigLinescan0;
rbuf_voidptr_s queue;

int8_t AdcPrimeLinescan0 ()
{
    ADC0_CFG2 |= ADC_CFG2_MUXSEL_MASK; //mux B
    ADC0_SC1A  = TFC_LINESCAN0_ADC_CHANNEL | ADC_SC1_AIEN_MASK; //initiate conversion
    return 0;
}

int8_t AdcCompleteLinescan0 ()
{
    if (linescan0.pixel < 128)
    {
        //image capture sequence ongoing...

        LineScanImage0WorkingBuffer[linescan0.pixel++] = ADC0_RA; //store pixel

        AdcConfig_s *config = &AdcConfigLinescan0;      //To-do separate push into push and push_array
        rbuf_voidptr_push(&queue, (void**) &config, 1); //Queue conversion
        
        TAOS_CLK_LOW;               
        for(uint8_t j = 0; j < 10; ++j) {}
        TAOS_CLK_HIGH; //shift in next pixel

    }
    else
    {
        //image capture sequence complete

        TAOS_CLK_HIGH;                                  
        for(uint8_t j = 0; j < 10; ++j) {}
        TAOS_CLK_LOW; //disconnect final pixel from output
        
        //swap the image buffer
        if(LineScanWorkingBuffer == 0)
        {
            LineScanWorkingBuffer = 1;
            LineScanImage0WorkingBuffer = &LineScanImage0Buffer[1][0];
            LineScanImage0 = &LineScanImage0Buffer[0][0];
        }
        else
        {
            LineScanWorkingBuffer = 0;
            LineScanImage0WorkingBuffer = &LineScanImage0Buffer[0][0];
            LineScanImage0 = &LineScanImage0Buffer[1][0];
        }

        carState->lineScanState = LINESCAN_IMAGE_READY; //announce image ready
    }
    return 0;
}

AdcConfig_s AdcConfigLinescan0 = { //linescan0 ADC config
    /* primer = */ &AdcPrimeLinescan0,
    /* completer = */ &AdcCompleteLinescan0
};

LinescanImage linescan0 = { //linescan0 struct
	/* pixel = */ 0,
    /* AdcConfig = */ &AdcConfigLinescan0
};


///////////////////////////////////////
// Potentiometer-related definitions //
// and ADC configuration             //
///////////////////////////////////////

#define TFC_POT_0_ADC_CHANNEL 13
#define TFC_POT_1_ADC_CHANNEL 12

volatile uint16_t PotADC_Value[2];

//Pot Reading is Scaled to return a value of -1.0 to 1.0
float TFC_ReadPot(uint8_t Channel)
{
    if(Channel == 0)
        return ((float)PotADC_Value[0]/-((float)ADC_MAX_CODE/2.0))+1.0;
    else
        return ((float)PotADC_Value[1]/-((float)ADC_MAX_CODE/2.0))+1.0;
}

int8_t AdcPrimePotentiometer0 ()
{
    ADC0_CFG2 &= ~ADC_CFG2_MUXSEL_MASK; //mux A
    ADC0_SC1A  =  TFC_POT_0_ADC_CHANNEL | ADC_SC1_AIEN_MASK; //initiate conversion
    return 0;
}

int8_t AdcCompletePotentiometer0 ()
{
    PotADC_Value[0] = ADC0_RA; //store sample
    return 0;
}

int8_t AdcPrimePotentiometer1 ()
{
    ADC0_CFG2 &= ~ADC_CFG2_MUXSEL_MASK; //mux A
    ADC0_SC1A  =  TFC_POT_1_ADC_CHANNEL | ADC_SC1_AIEN_MASK; //initiate conversion
    return 0;
}

int8_t AdcCompletePotentiometer1 ()
{
    PotADC_Value[1] = ADC0_RA; //store sample
    return 0;
}

AdcConfig_s AdcConfigPotentiometer0 = { //potentiometer0 ADC config
    /* primer = */ &AdcPrimePotentiometer0,
    /* completer = */ &AdcCompletePotentiometer0
};

AdcConfig_s AdcConfigPotentiometer1 = { //potentiometer1 ADC config
    /* primer = */ &AdcPrimePotentiometer1,
    /* completer = */ &AdcCompletePotentiometer1
};


///////////////////////////////////////
// Motor current-related definitions //
// and ADC configuration             //
///////////////////////////////////////

#include "sensors/motor/current.h"
extern MotorCurrent_s MotorCurrent[NUM_MOTORS];

int8_t AdcPrimeMotorCurrent0 ()
{
    ADC0_CFG2 &= ~ADC_CFG2_MUXSEL_MASK; //mux A
    ADC0_SC1A  =  TFC_MOTOR_CURRENT_0_CHANNEL | ADC_SC1_AIEN_MASK; //initiate conversion
    return 0;
}

int8_t AdcCompleteMotorCurrent0 ()
{
    rbuf_uint16_write(&MotorCurrent[REAR_RIGHT].buffer, (uint16_t*) &ADC0_RA, 1); //push sample onto signal buffer
    return 0;
}

int8_t AdcPrimeMotorCurrent1 ()
{
    ADC0_CFG2 &= ~ADC_CFG2_MUXSEL_MASK; //mux A
    ADC0_SC1A  =  TFC_MOTOR_CURRENT_1_CHANNEL | ADC_SC1_AIEN_MASK; //initiate conversion
    return 0;
}

int8_t AdcCompleteMotorCurrent1 ()
{
    rbuf_uint16_write(&MotorCurrent[REAR_LEFT].buffer, (uint16_t*) &ADC0_RA, 1); //push sample onto signal buffer
    return 0;
}

AdcConfig_s AdcConfigMotorCurrent0 = { //MotorCurrent0 ADC config
    /* primer = */ &AdcPrimeMotorCurrent0,
    /* completer = */ &AdcCompleteMotorCurrent0
};

AdcConfig_s AdcConfigMotorCurrent1 = { //MotorCurrent1 ADC config
    /* primer = */ &AdcPrimeMotorCurrent1,
    /* completer = */ &AdcCompleteMotorCurrent1
};


///////////////////////////////////
// Periodic sampling schedule    //
// definitions and configuration //
///////////////////////////////////

#define SET_BY_INITIALIZATION 0

typedef struct {
    AdcConfig_s *AdcConfig;
    uint32_t    frequency;
    uint32_t    period;
    uint32_t    counter;
} PeriodicSample_s;

PeriodicSample_s schedule[] =
{
    /* [0] = */ {
        /* AdcConfig = */ &AdcConfigPotentiometer0,
        /* frequency = */ POT_0_SAMPLE_FREQUENCY,
        /* period = */    SET_BY_INITIALIZATION,
        /* counter = */   SET_BY_INITIALIZATION
    },
    /* [1] = */ {
        /* AdcConfig = */ &AdcConfigPotentiometer1,
        /* frequency = */ POT_1_SAMPLE_FREQUENCY,
        /* period = */    SET_BY_INITIALIZATION,
        /* counter = */   SET_BY_INITIALIZATION
    },
    /* [2] = */ {
        /* AdcConfig = */ &AdcConfigMotorCurrent0,
        /* frequency = */ MOTOR_CURRENT_0_SAMPLE_FREQUENCY,
        /* period = */    SET_BY_INITIALIZATION,
        /* counter = */   SET_BY_INITIALIZATION
    },
    /* [3] = */ {
        /* AdcConfig = */ &AdcConfigMotorCurrent1,
        /* frequency = */ MOTOR_CURRENT_1_SAMPLE_FREQUENCY,
        /* period = */    SET_BY_INITIALIZATION,
        /* counter = */   SET_BY_INITIALIZATION
    }
};
#define SIZEOF_SCHEDULE ( (sizeof schedule) / (sizeof (PeriodicSample_s)) )


//////////////////////////////
// Periodic sampling system //
// definitions and methods  //
//////////////////////////////

void*        __queueData[SAMPLER_QUEUE_LENGTH]; //buffer storage
rbuf_voidptr_s queue;                           //buffer struct

AdcConfig_s *focus = 0; //pointer to ongoing sample, used for communication between methods

void Sampler_Init()
{ 
    PIT_TCTRL1 = PIT_TCTRL_TEN_MASK | //enable timer
                 PIT_TCTRL_TIE_MASK;  //enable interrupts

    PIT_LDVAL1 = (uint32_t) PERIPHERAL_BUS_CLOCK / PIT1_POLLING_FREQUENCY; //set polling period

    //initialize schedule objects
    for (uint32_t i = 0; i < SIZEOF_SCHEDULE; ++i )
    {
        PeriodicSample_s *item = &schedule[i];

        if (item->frequency != 0)
        {
            // Set sampling period if frequency > 0
            item->period = SYSTICK_FREQUENCY/item->frequency;
            item->counter = 0;
        }
    }

    rbuf_voidptr_init(&queue, __queueData, SAMPLER_QUEUE_LENGTH); //initialize queue
}

void Sampler_Update()
{
	uint32_t ticker = ADC_TICKER; //"freeze" timer
	ADC_TICKER = 0;               //reset timer for accurate sampling
	
    for (uint32_t i = 0; i < SIZEOF_SCHEDULE; ++i)
    {
        PeriodicSample_s *item = &schedule[i];

        item->counter += ticker; //update item counter

        if (item->frequency == 0) continue; //skip if frequency = 0
        if (item->counter >= item->period)
        {
            //item counter has exceeded sampling period

            item->counter = 0; //reset the counter

            rbuf_voidptr_push(&queue, (void**) &item->AdcConfig, 1); //add sample to queue
        }
    }
}

void Sampler_Dispatch()
{	
    if (rbuf_voidptr_used(&queue) == 0) return;        //if queue empty, no samples to dispatch
    else rbuf_voidptr_pop(&queue, (void**) &focus, 1); //otherwise pop a queued sample

    (*(focus->primer))(); //call the specified priming method
}


/////////////////////////////////////////////
// Periodic interrupt timer routine        //
// ======================================= //
// Linescan image capture sequence priming //
// and ADC queue priming managed by this   //
// routine                                 //
/////////////////////////////////////////////

void PIT_IRQHandler()
{
    ///////////////////////////////////////
    // LINESCAN0 CAPTURE SEQUENCE PRIMER //
    ///////////////////////////////////////

    if (PIT_TFLG0) {

        PIT_TFLG0 = PIT_TFLG_TIF_MASK; //clear the IRQ flag

		//Begin image capture sequence
		linescan0.pixel = 0;
		TAOS_SI_HIGH;                      //rising SI holds linescan image at current state
		for(uint8_t j = 0; j < 10; ++j) {}
		TAOS_CLK_HIGH;				       //rising CLK shifts in first pixel
		for(uint8_t j = 0; j < 10; ++j) {}
		TAOS_SI_LOW;                       //SI needs to fall at some point before next capture

		AdcConfig_s *config = &AdcConfigLinescan0;      //To-do separate push into push and push_array
		rbuf_voidptr_push(&queue, (void**) &config, 1); //Queue conversion
    }


    //////////////////////
    // ADC QUEUE PRIMER //
    //////////////////////

    if (PIT_TFLG1) {
        PIT_TFLG1 = PIT_TFLG_TIF_MASK; //clear the IRQ flag

        Sampler_Update(); //poll schedule and add samples to queue
        if (focus == 0) Sampler_Dispatch(); //if no active conversions attempt to re-prime the ADC
        
        UpdateTaskRequests();
    }
}

///////////////////////////////////////////////
// ADC interrupt routine                     //
// ========================================= //
// This routine called when a conversion     //
// is completed by the hardware. Calls       //
// the specified method to handle the sample //
// and attempts to re-arm the ADC.           //
///////////////////////////////////////////////

void ADC0_IRQHandler()
{
    if (focus != 0) (*(focus->completer))();
    else
    {
        uint16_t tmp = ADC0_RA; //clear an orphaned sample?
    }

    focus = 0;          //signal sample complete
    Sampler_Dispatch(); //attempt to re-arm ADC
}
