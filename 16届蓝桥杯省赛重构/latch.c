#include <STC15F2K60S2.H>
//#define Y4C 0x80
//#define Y5C 0xA0
//#define Y6C 0xC0
//#define Y7C 0xE0
void latch(unsigned char HC38)
{
	switch (HC38)
	{
		case 4:P2=(P2&0x1F)|0X80;break;
		case 5:P2=(P2&0x1F)|0xA0;break;
		case 6:P2=(P2&0x1F)|0XC0;break;
		case 7:P2=(P2&0x1F)|0XE0;break;
	}
	P2&=0x1F;
}