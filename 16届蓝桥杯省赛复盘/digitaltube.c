#include <STC15F2K60S2.H>
#include "latch.h"
#include "delay.h"
code unsigned char number[16]={0xC0 ,0xF9 ,0xA4 ,0xB0 ,0x99 ,0x92 ,0x82 ,0xF8 ,0x80 ,0x90 ,0x88 ,0x83 ,0xC6 ,0xA1 ,0x86 ,0x8E};
void digitaltube_fixed(unsigned char tube_position,unsigned char tube_number)
{
	switch(tube_position)
	{
		case 1:P0=0x01;break;
		case 2:P0=0x02;break;
		case 3:P0=0x04;break;
		case 4:P0=0x08;break;
		case 5:P0=0x10;break;
		case 6:P0=0x20;break;
		case 7:P0=0x40;break;
		case 8:P0=0x80;break;
	}
	latch(6);
	P0=number[tube_number];
	latch(7);
	
	delay(1);

	P0=0XFF;
	latch(7);

}

void digitaltube(unsigned char tube_position,unsigned char tube_number)
{
	switch(tube_position)
	{
		case 1:P0=0x01;break;
		case 2:P0=0x02;break;
		case 3:P0=0x04;break;
		case 4:P0=0x08;break;
		case 5:P0=0x10;break;
		case 6:P0=0x20;break;
		case 7:P0=0x40;break;
		case 8:P0=0x80;break;
	}
	latch(6);
	P0=tube_number;
	latch(7);
	
	delay(1);

	P0=0XFF;
	latch(7);

}