#include <STC15F2K60S2.H>
#include <intrins.h> 
sbit onewire_DQ=P1^4;

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
	Delayus(10);
	onewire_DQ=Bit;
	Delayus(50);
	onewire_DQ=1;
}

unsigned char onewire_receivebit(void)
{
	unsigned char Bit;
	onewire_DQ=0;
	Delayus(5);
	onewire_DQ=1;
	Delayus(5);
	Bit=onewire_DQ;
	Delayus(50);
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