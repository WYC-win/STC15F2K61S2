#include <STC15F2K60S2.H>
#define Y4C 0x80
#define Y5C 0xA0
void  latch (unsigned char dat,unsigned char sel)
{
	P0=dat;
	P2 = (P2 & 0x1F) | sel;
	P2&=0x1F;
}