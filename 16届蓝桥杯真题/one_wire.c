#include <STC15F2K60S2.H>

sbit one_wire_DQ=P1^4;
void Delay70us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 207;
	while (--i);
}

void Delay500us(void)	//@12.000MHz
{
	unsigned char data i, j;

	i = 6;
	j = 211;
	do
	{
		while (--j);
	} while (--i);
}


unsigned char one_wire_init(void)
{
	unsigned char ackbit;
	one_wire_DQ=1;
	one_wire_DQ=0;
	Delay500us();
	one_wire_DQ=1;
	Delay70us();
	ackbit=one_wire_DQ;
	Delay500us();
	return ackbit;
}