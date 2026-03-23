#include <STC15F2K60S2.H>
#include "latch.h"
#include "delay.h"
void digitaltube_fixed(unsigned char digitaltube_position,unsigned char number)
{
	unsigned char Y4C=0x80;
	unsigned char Y5C=0xA0;
	unsigned char Y6C=0xC0;
	unsigned char Y7C=0xE0;
	unsigned char digital_number[16]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E};
	switch(digitaltube_position)
	{
		case 1: latch(0x01,Y6C);break;
		case 2: latch(0x02,Y6C);break;
		case 3: latch(0x04,Y6C);break;
		case 4: latch(0x08,Y6C);break;
		case 5: latch(0x10,Y6C);break;
		case 6: latch(0x20,Y6C);break;
		case 7: latch(0x40,Y6C);break;
		case 8: latch(0x80,Y6C);break;
	}
	latch(digital_number[number],Y7C);
	delay(2);
	latch(0xFF,Y7C);
}


