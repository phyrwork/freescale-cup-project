#include "sensors/SpeedSensor.h"
#include "devices/MKL25Z4.h"
#include "devices/TFC_SHIELD.h"
#include "devices/arm_cm0.h"

#define PERIOD 1.0f/187200.0f //Calculated from known input
#define WHEEL_DIAMETER 1.0f //Pulled out my arse
#define RATE_SCALER 50.0f*3750.0f/8.0f
#define MIN_SPEED 5.0f
#define MAX_QUARTER_PERIOD  RATE_SCALER / (4.0f*MIN_SPEED)

static volatile struct speed_s speeds[2];

void TFC_InitSpeedSensor()
{
	disable_irq(INT_TPM2 - 16);

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

	TPM2_CONF |= 0x00000000; //Timer doesn't continue when debug paused

	TPM2_C0SC |= 0x0080; //Clear interrupt flag by writing a 1 to it (weird but the datasheet says so) (other method TPM2C1SC &= 0x7F;), needs to satisfy read->write


	set_irq_priority(INT_TPM2 - 16, 3); //Set to lowest priority
	enable_irq(INT_TPM2 - 16);
}

volatile isNewMeasurementAvailable_t* isANewmeasurementAvailable(uint8_t channel)
{
	//check for timeout, if current value - previous is greater than 5 rev/s
	if (speeds[channel].overflowed > 0)
	{
		if(((uint32_t) TPM2_CNT + ((uint32_t)0xFFFF - speeds[channel].previousCount) + (0xFFFF * ((uint32_t)speeds[channel].overflowed - 1))) > MAX_QUARTER_PERIOD)
		{
			speeds[channel].speed = 0;
			speeds[channel].measurementAvailable = NEW_MEASUREMENT_AVAILABLE;
			TFC_SetLED(1);
		}
	}
	else
	{
		if(TPM2_CNT - speeds[channel].previousCount > MAX_QUARTER_PERIOD)
		{
			speeds[channel].speed = 0;
			speeds[channel].measurementAvailable = NEW_MEASUREMENT_AVAILABLE;
			TFC_SetLED(1);
		}
	}
	TFC_ClearLED(1);
	return &speeds[channel].measurementAvailable;
}

float getSpeed(uint8_t channel)
{
	if (channel > 1)
	{
		return 0; //Invalid access
	}

	speeds[channel].period = 0;
	for (uint8_t i = 0; i < 2; i++)
	{
		if (speeds[channel].halfPeriodTimes[i] != 0.0f)
		{
			speeds[channel].period += speeds[channel].halfPeriodTimes[i];
		}
		else
		{
			return 0.0f;
		}
	}

	speeds[channel].speed = WHEEL_DIAMETER * (RATE_SCALER / (float) speeds[channel].period);
	return speeds[channel].speed;
}

void FTM2_IRQHandler()
{
	if (((TPM2_SC >> 7) & 0x0001) == 0x0001) //If counter overflow interrupt flag is set
	{
		TPM2_SC |= 0x00000080; //Clear counter overflow interrupt flag
		speeds[0].overflowed++;
		speeds[1].overflowed++;
	}

	if (((TPM2_C0SC >> 7) & 0x0001) == 0x0001) //If ch0 interrupt flag is set
	{
		TPM2_C0SC |= 0x0080; //Clear capture interrupt flag by writing a 1 to it, confirmed working!

		if(speeds[0].overflowed > 0)
		{
			speeds[0].halfPeriodTimes[speeds[0].state & 0x01] = (uint32_t)TPM2_C0V + ((uint32_t)0xFFFF - speeds[0].previousCount) + (0xFFFF * ((uint32_t)speeds[0].overflowed - 1)); //Must account for timer overflow
			speeds[0].overflowed = 0;
		}
		else
		{
			speeds[0].halfPeriodTimes[speeds[0].state & 0x01] = TPM2_C0V - speeds[0].previousCount;
		}
		speeds[0].previousCount = TPM2_C0V;

		speeds[0].state++;
		speeds[0].measurementAvailable = NEW_MEASUREMENT_AVAILABLE;
	}

	if (((TPM2_C1SC >> 7) & 0x0001) == 0x0001) //If ch1 interrupt flag is set
	{
		TPM2_C1SC |= 0x0080; //Clear capture interrupt flag by writing a 1 to it, confirmed working!

		if(speeds[1].overflowed > 0)
		{
			speeds[1].halfPeriodTimes[speeds[1].state & 0x01] = (uint32_t)TPM2_C1V + ((uint32_t)0xFFFF - speeds[1].previousCount) + (0xFFFF * ((uint32_t)speeds[1].overflowed - 1)); //Must account for timer overflow
			speeds[1].overflowed = 0;
		}
		else
		{
			speeds[1].halfPeriodTimes[speeds[1].state & 0x01] = TPM2_C1V - speeds[1].previousCount;
		}
		speeds[1].previousCount = TPM2_C1V;

		speeds[1].state++;
		speeds[1].measurementAvailable = NEW_MEASUREMENT_AVAILABLE;
	}
}

