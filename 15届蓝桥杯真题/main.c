#include <STC15F2K60S2.H>
#include "onewire.h"
#include "iic.h"
#include "ds1302.h"
#include "latch.h"
#include "init.h"
#include "digitaltube.h"
unsigned char Y4C=0x80;
unsigned char Y5C=0xA0;
unsigned char Y6C=0xC0;
unsigned char Y7C=0xD0;

void main()
{
	init();
	while(1)
	{
		latch(0x00,Y4C);
	}
}
