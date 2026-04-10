#include <STC15F2K60S2.H>
#include "latch.h"
#include "init.h"
#include "digitaltube.h"
#include "kbd.h"
#include "delay.h"
#include "ds18b20.h"
#include "ds1302.h"
//#include <stdio.h>
void main()
{
	init();

	while(1)
	{
		printf("a");
	}
}