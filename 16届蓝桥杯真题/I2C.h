#ifndef __I2C_H__
#define __I2C_H__
void I2C_start(void);
void I2C_stop(void);
void I2C_sendbyte(unsigned char Byte);
unsigned char I2C_receivebyte(void);
void I2C_sendack(unsigned char ACKbit);
unsigned char I2C_receiveack(void);

#endif