#include "main.h"

#define TARGET_TOTAL_INTENSITY 300000//300000
#define CHANNEL_0 0
#define CHANNEL_1 1

void TFC_Init(carState_s* carState)
{
	TFC_InitClock();
	TFC_InitSysTick();
	TFC_InitGPIO();
	TFC_InitServos();
	TFC_InitMotorPWM();
	TFC_InitADCs(carState);
	TFC_InitLineScanCamera();
	TFC_InitUARTs();
	TFC_HBRIDGE_DISABLE;
	TFC_SetMotorPWM(0, 0);
	TFC_InitSpeedSensor();
	preloadProbabilityTables(); //Prevents probability tables for stop line evaluation from being created too late
//	Init_I2C();
//	Init_MMA8451Q();
}

void TFC_Task()
{
#if defined(TERMINAL_USE_SDA_SERIAL)
	TFC_UART_Process();
#endif
}

int main(void)
{
	static carState_s carState =
	{ .motorState = FORCED_DISABLED, .UARTSpeedState = UNDEFINED, .lineDetectionState = LINE_LOST, .lineScanState = NO_NEW_LINESCAN_IMAGE };
	TFC_Init(&carState);

	while (1)
	{
		TFC_Task();
		evaluateUARTorSpeed(&carState);
		evaluateMotorState(&carState);
		
//		TFC_SetMotorPWM(0.3f,0.3f);
//		while(1){}

		switch ((TFC_GetDIP_Switch() >> 1) & 0x03)
		{
		default:
		case 0:
			rawFocussingMode(&carState);
			break;

		case 1:
			servoAlignment();
			//speedTestMode(&carState);
			break;

		case 2:
			derivativeFocussingMode(&carState);
			break;

		case 3:
			lineFollowingMode(&carState);
			break;
		}
	}
	return 0;
}

void heartbeat()
{
	static uint8_t toggle = 0;
	toggle = ~toggle;
	if (toggle == 0)
	{
		TFC_SetLED(0);
	}
	else
	{
		TFC_ClearLED(0);
	}
}

void evaluateUARTorSpeed(carState_s* carState)
{
	if (((((TFC_GetDIP_Switch() >> 3) & 0x01) == 0x00) && ((PORTA_PCR2 >> 8) & 0x00000007) != 0x00000002) || carState->UARTSpeedState == UNDEFINED) //Single UART single sensor
	{
		PORTA_PCR2 &= 0xFFFFF8FF;
		PORTA_PCR2 |= 0x00000200;
		enable_irq(INT_UART0-16);
		carState->UARTSpeedState = SINGLE_SPEED_SINGLE_UART;
		TERMINAL_PRINTF("\nSwitching to single uart single sensor");
	}

	else if (((((TFC_GetDIP_Switch() >> 3) & 0x01) == 0x01) && ((PORTA_PCR2 >> 8) & 0x00000007) != 0x00000003) || carState->UARTSpeedState ==UNDEFINED) //Dual sensor no uart
	{
		TERMINAL_PRINTF("\nSwitching to dual sensor no uart");
		PORTA_PCR2 &= 0xFFFFF8FF;
		PORTA_PCR2 |= 0x00000300;
		disable_irq(INT_UART0-16);
		carState->UARTSpeedState = DUAL_SPEED_NO_UART;
	}
}

void evaluateMotorState(carState_s* carState)
{
	if ((TFC_GetDIP_Switch() & 0x01) == 1)
	{
		TFC_HBRIDGE_ENABLE;
		carState->motorState = RUNNING;
	}
	else if ((TFC_GetDIP_Switch() & 0x01) == 0)
	{
		TFC_HBRIDGE_DISABLE;
		TFC_SetMotorPWM(0, 0);
	}
}

void rawFocussingMode(carState_s* carState)
{
	if (TFC_Ticker[0] >= 200 && carState->lineScanState == LINESCAN_IMAGE_READY)
	{
		TFC_Ticker[0] = 0;
		carState->lineScanState = NO_NEW_LINESCAN_IMAGE;

		TFC_SetServo(0, 0);
		TFC_HBRIDGE_DISABLE;
		TFC_SetMotorPWM(0, 0);

		TFC_SetLineScanExposureTime(calculateNewExposure(getTotalIntensity(LineScanImage0), TARGET_TOTAL_INTENSITY));

		for (uint8_t i = 0; i < 128; i++)
		{
			TERMINAL_PRINTF("%X,", LineScanImage0[i]);
		}
		TERMINAL_PRINTF("\r\n");
	}
}

void speedTestMode(carState_s* carState)
{
	static uint16_t speedChangeDuration = 0;
	if (TFC_Ticker[0] >= 200)
	{
		speedChangeDuration += TFC_Ticker[0];
		TFC_Ticker[0] = 0;
		if (carState->UARTSpeedState == SINGLE_SPEED_SINGLE_UART)
		{
//			float speed = getSpeed(CHANNEL_0);
//			int8_t whole = speed;
//			int8_t frac = abs((speed - (float) whole) * 100.0f);
//			TERMINAL_PRINTF("\n%d.%d", whole, frac);

			if (speedChangeDuration <= 6000)
			{
//				float PWM = getDesiredMotorPWM(14.0f, getSpeed(0), isANewmeasurementAvailable(0), CHANNEL_0);
//				TFC_SetMotorPWM(PWM, 0);
			}
			else if (speedChangeDuration <= 12000)
			{
//				float PWM = getDesiredMotorPWM(20.0f, getSpeed(0), isANewmeasurementAvailable(0), CHANNEL_0);
//				TFC_SetMotorPWM(PWM, 0);
			}
			else
			{
				speedChangeDuration = 0;
			}
		}
		else if (carState->UARTSpeedState == DUAL_SPEED_NO_UART)
		{
			if (speedChangeDuration <= 6000)
			{
//				float PWM0 = getDesiredMotorPWM(7.0f, getSpeed(0), isANewmeasurementAvailable(0), CHANNEL_0);
//				float PWM1 = getDesiredMotorPWM(7.0f, getSpeed(1), isANewmeasurementAvailable(1), CHANNEL_1);
//				TFC_SetMotorPWM(PWM0, PWM1);
			}
			else if (speedChangeDuration <= 12000)
			{
//				float PWM0 = getDesiredMotorPWM(13.0f, getSpeed(0), isANewmeasurementAvailable(0), CHANNEL_0);
//				float PWM1 = getDesiredMotorPWM(13.0f, getSpeed(1), isANewmeasurementAvailable(1), CHANNEL_1);
//				TFC_SetMotorPWM(PWM0, PWM1);
			}
			else
			{
				speedChangeDuration = 0;
			}

		}
	}
}

void derivativeFocussingMode(carState_s* carState)
{
	if (TFC_Ticker[0] >= 200 && carState->lineScanState == LINESCAN_IMAGE_READY)
	{
		TFC_Ticker[0] = 0;
		carState->lineScanState = NO_NEW_LINESCAN_IMAGE;

		TFC_SetServo(0, 0);
		TFC_HBRIDGE_DISABLE;
		TFC_SetMotorPWM(0, 0);

		TFC_SetLineScanExposureTime(calculateNewExposure(getTotalIntensity(LineScanImage0), TARGET_TOTAL_INTENSITY));
		int16_t temp[128];
		getFirstDerivative(LineScanImage0, temp, 128);
		//						
		for (uint8_t i = 0; i < 128; i++)
		{
			TERMINAL_PRINTF("%X,", abs(temp[i]));
		}
		TERMINAL_PRINTF("\r\n");
	}
}

void servoAlignment()
{
	if (TFC_Ticker[0] >= 200)
	{
		TFC_Ticker[0] = 0;
		float offset = TFC_ReadPot(0) * 0.1f;
		TFC_SetServo(0, offset);
	}
}

void lineFollowingMode(carState_s* carState)
{
	static lineScanState_t steeringControlUpdate;
	static uint32_t totalIntensity = 0;
	if (carState->lineScanState == LINESCAN_IMAGE_READY)
	{
		steeringControlUpdate = LINESCAN_IMAGE_READY;
		findLine(LineScanImage0, carState, 350);// ((uint32_t)350*TARGET_TOTAL_INTENSITY) / totalIntensity);
	}

	if (TFC_Ticker[0] >= 200)
	{
		TFC_Ticker[0] = 0;
		TFC_SetServo(0, getDesiredServoValue(carState->lineCenter, 0, &steeringControlUpdate));
	}

	if (carState->lineScanState == LINESCAN_IMAGE_READY) //Stopline detection and exposure control are performed after servo has been updated
	{
		carState->lineScanState = NO_NEW_LINESCAN_IMAGE;
		findStop(carState);
		totalIntensity = getTotalIntensity(LineScanImage0);
		TFC_SetLineScanExposureTime(calculateNewExposure(totalIntensity, TARGET_TOTAL_INTENSITY));
	}

	if (carState->lineDetectionState == LINE_FOUND || carState->lineDetectionState == LINE_TEMPORARILY_LOST)
	{
		float targetSpeed = targetSpeedAverage(getDesiredSpeed(carState, 18.0f + (TFC_ReadPot(0) * 5.0f)));
		float activeDifferentialModifier[] =
		{ getActiveDifferentialModifier(carState, CHANNEL_0), getActiveDifferentialModifier(carState, CHANNEL_1) };
		float speedMeasurement[] = { getSpeed(CHANNEL_0), getSpeed(CHANNEL_1) };

		if (carState->UARTSpeedState == SINGLE_SPEED_SINGLE_UART)
		{

		}
		else if (carState->UARTSpeedState == DUAL_SPEED_NO_UART)
		{
			TFC_SetMotorPWM(
					getDesiredMotorPWM(targetSpeed * activeDifferentialModifier[0], speedMeasurement[0], isANewmeasurementAvailable(CHANNEL_0), CHANNEL_0),
					getDesiredMotorPWM(targetSpeed * activeDifferentialModifier[1], speedMeasurement[1], isANewmeasurementAvailable(CHANNEL_1), CHANNEL_1));
		}
//		TFC_ClearLED(2);

		if (carState->lineDetectionState == LINE_TEMPORARILY_LOST)
		{
//			TFC_SetLED(1);
		}
		else
		{
//			TFC_ClearLED(1);
		}
	}
	else if (carState->lineDetectionState == LINE_LOST)
	{
		TFC_HBRIDGE_DISABLE;
		TFC_SetMotorPWM(0, 0);
//		TFC_SetLED(2);
	}
	else if (carState->lineDetectionState == STOPLINE_DETECTED)
	{
		float speedMeasurement[] = { getSpeed(CHANNEL_0), getSpeed(CHANNEL_1) };

		if (speedMeasurement[0] > 2.0f || speedMeasurement[1] > 2.0f)
		{
			TFC_SetMotorPWM(-0.5f, -0.5f);
		}
		else
		{
			while(1){TFC_SetMotorPWM(0, 0);}
		}
	}
}

float targetSpeedAverage(float targetSpeed)
{
	static float previousTargetSpeed = 0;
	previousTargetSpeed = 0.9f*previousTargetSpeed + 0.1*targetSpeed;
	return previousTargetSpeed;
}
