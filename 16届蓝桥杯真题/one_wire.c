#include <STC15F2K60S2.H>
#include <intrins.h> 
sbit onewire_DQ=P1^4;

void Delay10us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 27;
	while (--i);
}

void Delay50us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 147;
	while (--i);
}

void Delay5us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 12;
	while (--i);
}



void Delayus(unsigned int xus)	//@12.000MHz
{
	unsigned char tem=xus/5;
	unsigned char data i;
	while(tem--)
	{
		_nop_();
		_nop_();
		i = 12;
		while (--i);
	}

}

unsigned char onewire_init(void)
{
	unsigned char ackbit;
	onewire_DQ=1;
	onewire_DQ=0;
	Delayus(500);
	onewire_DQ=1;
	Delayus(70);
	ackbit=onewire_DQ;
	Delayus(500);
	return ackbit;
}

void onewire_sendbit(unsigned char Bit)
{
	onewire_DQ=0;
	Delay10us();
	onewire_DQ=Bit;
	Delay50us();
	onewire_DQ=1;
}

unsigned char onewire_receivebit(void)
{
	unsigned char Bit;
	onewire_DQ=0;
	Delay5us();
	onewire_DQ=1;
	Delay5us();
	Bit=onewire_DQ;
	Delay50us();
	return Bit;
}

void onewire_sendbyte(unsigned char byte)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		onewire_sendbit(byte&(0x01<<i));
	}
}

unsigned char onewire_receivebyte(void)
{

	unsigned char i;
	unsigned char byte=0x00;
	for(i=0;i<8;i++)
	{
		if(onewire_receivebit()){byte|=(0x01<<i);}
	}
	return byte;
}