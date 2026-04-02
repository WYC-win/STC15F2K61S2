#include <STC15F2K60S2.H>
#include "latch.h"
#include "init.h"
#include "digitaltube.h"
#include "kbd.h"
#include "ds1302.h"
#include "iic.h"
#include "onewire.h"
#include "delay.h"
#include "ds18b20.h"
#include "superwave.h"
unsigned int dis,i;

void main()
{
	init();
	
	while(1)
	{
		dis=get_distance();
		for(i=0;i<200;i++)
		{
			digitaltube_fixed(1,dis/100);
			digitaltube_fixed(2,dis%100/10);
			digitaltube_fixed(3,dis%10);
		}
		
	}
}
