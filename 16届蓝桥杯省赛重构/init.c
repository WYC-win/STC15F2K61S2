#include <STC15F2K60S2.H>
#include "latch.h"

void init()
{
	P0=0x00;
	latch(5);
	
	P0=0xFF;
	latch(4);

}