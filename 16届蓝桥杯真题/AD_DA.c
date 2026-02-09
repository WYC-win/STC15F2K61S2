#include <STC15F2K60S2.H>
#include "iic.h"
//#define ADCW 0x90
//#define ADCR 0x91


void DA_write(unsigned char dat)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40);
	I2CWaitAck();
	I2CSendByte(dat);
	I2CWaitAck();
	I2CStop();
}
unsigned char AD_read(unsigned char adr)
{
	unsigned char AIN;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(adr);
	I2CWaitAck();
	I2CStop();
	
//	I2CStart();
//	I2CSendByte(0x91);      // 读地址
//	I2CWaitAck();
//	AIN = I2CReceiveByte(); // 读取（但不使用这个值）
//	I2CSendAck(1);          // 发送NACK
//	I2CStop();
	
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	AIN=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();

	return AIN;
}


