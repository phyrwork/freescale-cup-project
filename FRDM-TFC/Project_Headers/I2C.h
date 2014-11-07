#include "TFC\TFC.h"

#define MWSR                   0x00  /* Master write  */
#define MRSW                   0x01  /* Master read */
#define i2c_DisableAck()       I2C0_C1 |= I2C_C1_TXAK_MASK
#define i2c_EnableAck()        I2C0_C1 &= ~I2C_C1_TXAK_MASK
#define i2c_RepeatedStart()    I2C0_C1 |= I2C_C1_RSTA_MASK
#define i2c_EnterRxMode()      I2C0_C1 &= ~I2C_C1_TX_MASK
#define i2c_write_byte(data)   I2C0_D = data

#define i2c_Start()            I2C0_C1 |= I2C_C1_TX_MASK;\
                               I2C0_C1 |= I2C_C1_MST_MASK

#define i2c_Stop()             I2C0_C1 &= ~I2C_C1_MST_MASK;\
                               I2C0_C1 &= ~I2C_C1_TX_MASK

#define i2c_Wait()               while((I2C0_S & I2C_S_IICIF_MASK)==0) {} \
                                  I2C0_S |= I2C_S_IICIF_MASK;

void Init_I2C(void);
void IIC_StartTransmission (uint8_t SlaveID, uint8_t Mode);
void I2CWriteRegister(uint8_t SlaveID, uint8_t u8RegisterAddress, uint8_t u8Data);
uint8_t I2CReadRegister(uint8_t SlaveID, uint8_t u8RegisterAddress);
void I2CReadMultiRegisters(uint8_t SlaveID, uint8_t u8RegisterAddress, uint8_t n, uint8_t * r);
