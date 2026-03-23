#include <STC15F2K60S2.H>

void  latch(unsigned char dat,unsigned char sel)
{
	P0=dat;
	P2 = (P2 & 0x1F) | sel;
	P2&=0x1F;
}