#include <STC15F2K60S2.H>
sbit TX=P1^0;
sbit RX=P1^1;
void Delay12us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}

unsigned int get_distance()
{
	unsigned int time;
	unsigned char i;
	CMOD=0x01;
	CCON=0x00;
	CH=0x00;CL=0x00;
	for(i=0;i<8;i++)
	{
		TX=1;
		Delay12us();
		TX=0;
		Delay12us();
	}
	CR=1;
	while(RX==1 && CH<0x40);
	if(CH>=0x40)return 0;
	else
	{
		CR=0;
		time=(((int)CH<<8)|CL);
		return (int)((time*0.0172)+3.3024);
	}
	
}