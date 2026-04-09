#include <STC15F2K60S2.H>
#include "iic.h"

float AD_read(unsigned char addr)
{
	unsigned char rubbish,temp;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CStop();
	
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	rubbish=I2CReceiveByte();
	I2CSendAck(0);
    temp=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	return (float)temp/51.0;
}

void DA_out(unsigned char out_vol)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40);
	I2CWaitAck();
	I2CSendByte(out_vol);
	I2CWaitAck();
	I2CStop();
}