#ifndef _iic_H_
#define _iic_H_
void I2CStart(void);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CReceiveByte(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(unsigned char ackbit);
static void I2C_Delay(unsigned char n);
#endif