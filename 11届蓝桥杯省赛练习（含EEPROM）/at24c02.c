#include <STC15F2K60S2.H>
#include "iic.h"
#include "delay.h"
void at24c02_write(unsigned char *dat,unsigned char addr,unsigned char num)
{
	I2CStart();
	I2CSendByte(0xA0);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	while(num--)
	{
		I2CSendByte(*dat++);
		I2CWaitAck();
	}
	I2CStop();
	delay(5);
}
void at24c02_read(unsigned char *dat,unsigned char addr,unsigned char num)
{
	I2CStart();
	I2CSendByte(0xA0);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CStop();
	
	I2CStart();
	I2CSendByte(0xA1);
	I2CWaitAck();
	while(num--)
	{
		*dat++=I2CReceiveByte();
		if(num)I2CSendAck(1);
	}
	I2CStop();
}