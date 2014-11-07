/*
 * MMA8451Q.c
 *
 *  Created on: Feb 28, 2014
 *      Author: Matt
 */
 
#include "MMA8451Q.h"


#define MMA8451_ADDRESS 0x1d



//MMA8451_CTRL_REG_1
#define MMA8451_ACTIVE_BIT_MASK 	0x01
#define MMA8451_F_READ_BIT_MASK 	0x02
#define MMA8451_50HZ_ODR_BIT_MASK	0x20
#define MMA8451_LNOISE_BIT_MASK		0x04

//MMA8451_CTRL_REG_2
#define MMA8641_HIGH_RES 0x02

#define MMA8451_OUT_X_MSB 	0x01
#define MMA8451_OUT_X_LSB 	0x02
#define MMA8451_OUT_Y_MSB 	0x03
#define MMA8451_OUT_Y_LSB 	0x04
#define MMA8451_OUT_Z_MSB 	0x05
#define MMA8451_OUT_Z_LSB 	0x06
#define WHO_AM_I			0x0D
#define MMA8451_CTRL_REG_1 	0x2A
#define MMA8451_CTRL_REG_2	0x2B
#define XYZ_DATA_CFG		0x0E

static uint8_t buf[16];
static float xBias, yBias, zBias;

void Init_MMA8451Q()
{
	I2CWriteRegister(0x1d, MMA8451_CTRL_REG_2, 0x00 | MMA8641_HIGH_RES);
	I2CWriteRegister(0x1d, MMA8451_CTRL_REG_1, 0x00 | MMA8451_50HZ_ODR_BIT_MASK | MMA8451_ACTIVE_BIT_MASK | MMA8451_LNOISE_BIT_MASK);
	TERMINAL_PRINTF("MMA8451 address: %x", getID());
	zeroAccelerometers();
}

void zeroAccelerometers()
{
	float xAccSum = 0;
	float yAccSum = 0;
	float zAccSum = 0;
	xBias = 0;
	yBias = 0;
	zBias = 0;
	for (int i = 0; i < 100; i++)
	{
		while(TFC_Ticker[0] <= 20){}		
		TFC_Ticker[0] = 0;
		xAccSum += getXAcc();
		yAccSum += getYAcc();
		zAccSum += getZAcc();
	}	
	xBias = xAccSum / 100.0f;
	yBias = yAccSum / 100.0f;
	zBias = zAccSum / 100.0f;
}

float getXAcc()
{
	buf[0] = I2CReadRegister(MMA8451_ADDRESS, MMA8451_OUT_X_MSB);
	buf[1] = I2CReadRegister(MMA8451_ADDRESS, MMA8451_OUT_X_LSB);
	return ((((((int8_t)buf[0])*256 + (int8_t)buf[1]) / 4) / 4096.0f) * 9.81f) - xBias;
}

float getYAcc()
{
	buf[0] = I2CReadRegister(MMA8451_ADDRESS, MMA8451_OUT_Y_MSB);
	buf[1] = I2CReadRegister(MMA8451_ADDRESS, MMA8451_OUT_Y_LSB);
	return ((((((int8_t)buf[0])*256 + (int8_t)buf[1]) / 4) / 4096.0f) * 9.81f) - yBias;
}

float getZAcc()
{
	buf[0] = I2CReadRegister(MMA8451_ADDRESS, MMA8451_OUT_Z_MSB);
	buf[1] = I2CReadRegister(MMA8451_ADDRESS, MMA8451_OUT_Z_LSB);
	return ((((((int8_t)buf[0])*256 + (int8_t)buf[1]) / 4) / 4096.0f) * 9.81f) - zBias;
}

int16_t getRawXAcc()
{
	buf[0] = I2CReadRegister(MMA8451_ADDRESS, MMA8451_OUT_X_MSB);
	buf[1] = I2CReadRegister(MMA8451_ADDRESS, MMA8451_OUT_X_LSB);
	return((int8_t)buf[0]*256 + (int8_t)buf[1]) / 4;
}

int16_t getRawYAcc()
{
	buf[0] = I2CReadRegister(MMA8451_ADDRESS, MMA8451_OUT_Y_MSB);
	buf[1] = I2CReadRegister(MMA8451_ADDRESS, MMA8451_OUT_Y_LSB);
	return((int8_t)buf[0]*256 + (int8_t)buf[1]) / 4;
}

int16_t getRawZAcc()
{
	buf[0] = I2CReadRegister(MMA8451_ADDRESS, MMA8451_OUT_Z_MSB);
	buf[1] = I2CReadRegister(MMA8451_ADDRESS, MMA8451_OUT_Z_LSB);
	return((int8_t)buf[0]*256 + (int8_t)buf[1]) / 4;
}

uint8_t getID()
{
	return I2CReadRegister(MMA8451_ADDRESS, WHO_AM_I);
}
