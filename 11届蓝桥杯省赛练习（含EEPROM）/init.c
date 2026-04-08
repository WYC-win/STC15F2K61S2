#include "latch.h"
#include <STC15F2K60S2.H>

void init()
{
	P0=0xFF;
	latch(4);
	
	P0=0x00;
	latch(5);
}