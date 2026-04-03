#include <STC15F2K60S2.H>
#include "iic.h"
unsigned char AD_read(unsigned char AD_chanel)
{
	unsigned char light_temp;
	float light_level;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(AD_chanel);
	I2CWaitAck();
	I2CStop();
	
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	light_temp=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	light_level=light_temp/51.0;
	if(light_level>=3)return 1;
	if(light_level>=2)return 2;
	if(light_level>=0.5)return 3;
	else return 4;
	
	
	
}