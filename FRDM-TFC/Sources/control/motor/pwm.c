#include "control/motor/PWM.h"
#include "devices/MKL25Z4.h"
#include "devices/CrystalClock.h"
#include <math.h>

#define FTM0_MOD_VALUE	(int)((float)(PERIPHERAL_BUS_CLOCK)/TFC_MOTOR_SWITCHING_FREQUENCY)

#define FTM0_CLOCK                                   	      (CORE_CLOCK/2)
#define FTM0_CLK_PRESCALE                                 	   0  // Prescale Selector value - see comments in Status Control (SC) section for more details
#define FTM0_OVERFLOW_FREQUENCY 5000							  //
#define MAX_DUTY 0.4

/* Define motor structs */
const MotorTPM_s MotorTPM[NUM_MOTORS] = {
	/* [0] = */ { &TPM0_C2V, &TPM0_C3V },
	/* [1] = */ { &TPM0_C0V, &TPM0_C1V }
};
MotorPWM_s MotorPWM[NUM_MOTORS];;

/**********************************************************************************************/


void InitMotorPWMControl()
{
	//Mux clock to TPM0
    SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;// We Want MCGPLLCLK/2 
    SIM_SOPT2 &= ~(SIM_SOPT2_TPMSRC_MASK);
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);

    //Enable clock
	SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK;
    
    //Blow away the control registers to ensure that the counter is not running
    TPM0_SC = 0;
    TPM0_CONF = 0;
    
    //Set TPM module to continue in debug mode
    //TPM0_CONF |= 0x00000060;
    
    //While the counter is disabled we can setup the prescaler
    TPM0_SC = TPM_SC_PS(FTM0_CLK_PRESCALE);
    
    //Setup the mod register to get the correct PWM Period
    TPM0_MOD = FTM0_CLOCK/(1<<FTM0_CLK_PRESCALE)/FTM0_OVERFLOW_FREQUENCY;
    
    //Setup Channels 0,1,2,3
    TPM0_C0SC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
    TPM0_C1SC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK; 
    TPM0_C2SC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
    TPM0_C3SC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
    
    //Assign TPM channels to PWM structs */
    MotorPWM[REAR_LEFT].tpm = &MotorTPM[1];
    MotorPWM[REAR_RIGHT].tpm = &MotorTPM[0];
    
    //Initialise motors at zero duty
    for (uint8_t i = 0; i < NUM_MOTORS; ++i)
    	SetMotorPWM(&MotorPWM[i], 0.0);
    
    //Enable the TPM COunter
    TPM0_SC |= TPM_SC_CMOD(1);
    
    //Enable the FTM functions on the the port
    PORTC_PCR1 = PORT_PCR_MUX(4);
    PORTC_PCR2 = PORT_PCR_MUX(4);     
    PORTC_PCR3 = PORT_PCR_MUX(4);  
    PORTC_PCR4 = PORT_PCR_MUX(4);
}

void SetMotorPWM(MotorPWM_s *pwm, float value)
{
	/* Negative values not possible, so...   *
	 * For positive torque: fwd > 0, bwd = 0 *
	 * For negative torque: fwd = 0, bwd > 0 *
	 * ------------------------------------- *
	 * Be sure to limit duty to 1 to prevent *
	 * overflows                             */

	 //Check polarity
	 #define FORWARD 0
	 #define BACKWARD 1
	 uint8_t pol;
	 if (value > 0.0) pol = FORWARD;
	 else pol = BACKWARD;

	 //Check/limit magnitude
	 value = fabsf(value); //return abs(float value)
	 if (value > MAX_DUTY) value = MAX_DUTY; //limit to max duty

	 //Set PWM
	 if (value != 0.0) //Any magnitude
		//+'ve
		if (pol == FORWARD)
		{
			*(pwm->tpm->fwdCnVReg) = (uint16_t) (TPM0_MOD * (float) value);
			*(pwm->tpm->bwdCnVReg) = 0;
			pwm->value = value;
		}
		//-'ve
		else //(if value ==  BACKWARD)
		{
			*(pwm->tpm->fwdCnVReg) = 0;
			*(pwm->tpm->bwdCnVReg) = (uint16_t) (TPM0_MOD * (float) value);
			pwm->value = -value;
		}
	else //No magnitude
	{
		*(pwm->tpm->fwdCnVReg) = 0; //fwd = 0, bwd = 0...
		*(pwm->tpm->bwdCnVReg) = 0; //...motor free runs.
		pwm->value = 0;
	}
}

float GetMotorPWM(MotorPWM_s *pwm) { return pwm->value; }

/* Backwards compatibility code */
void TFC_SetMotorPWM(float duty_A, float duty_B)
{
	//Motor A = REAR_RIGHT
	SetMotorPWM(&MotorPWM[REAR_RIGHT], duty_A);
	//Motor B = REAR_LEFT
	SetMotorPWM(&MotorPWM[REAR_LEFT], duty_B);
}

float TFC_GetMotorPWM(uint8_t channel)
{
	return GetMotorPWM(&MotorPWM[channel]);
}


