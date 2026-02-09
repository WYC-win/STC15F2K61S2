#include <STC15F2K60S2.H>

sbit I2C_SCL=P2^0;
sbit I2C_SDA=P2^1;
void I2C_start(void)
{
	I2C_SDA=1;
	I2C_SCL=1;
	I2C_SDA=0;
	I2C_SCL=0;
}

void I2C_stop(void)
{
	I2C_SDA=0;
	I2C_SCL=1;
	I2C_SDA=1;
}

void I2C_sendbyte(unsigned char Byte)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		I2C_SDA=Byte&(0x80>>i);
		I2C_SCL=1;
		I2C_SCL=0;
	}
	
}

unsigned char I2C_receivebyte(void)
{
	unsigned char i,Byte=0x00;
	I2C_SDA=1;
	for(i=0;i<8;i++)
	{
		I2C_SCL=1;
		if(I2C_SDA){Byte|=(0x80>>i);}
		I2C_SCL=1;
	}
	return Byte;
}

void I2C_sendack(unsigned char ACKbit)
{
	I2C_SDA=ACKbit;
	I2C_SCL=1;
	I2C_SCL=0;
}

unsigned char I2C_receiveack(void)
{
	unsigned char ACKbit;
	I2C_SDA=1;
	I2C_SCL=0;
	ACKbit=I2C_SDA;
	I2C_SCL=0;
	return ACKbit;
}