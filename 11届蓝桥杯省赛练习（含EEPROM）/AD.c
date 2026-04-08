#include <STC15F2K60S2.H>
#include "iic.h"

float get_ain3(void)
{
	unsigned char ain3=0,rubbish=0;
	float voltage_ain3=0.0;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x03);
	I2CWaitAck();
	I2CStop();
	
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	rubbish=I2CReceiveByte();
	I2CSendAck(0);
	ain3=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	voltage_ain3=ain3/51.0;
	return voltage_ain3;
}