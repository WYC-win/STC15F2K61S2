#include <STC15F2K60S2.H>
#include "one_wire.h"
#include "delay.h"
#include "digital_tube.h"
#include "init.h"
#include "DS18B20.h"
#include "uart.h"

int T,bai,shi,ge,i;

void main()
{
	init();
	DS18B20_convertT();
	delay(1000);
	while(1)
	{
		DS18B20_convertT();
		delay(1000);
		T=DS18B20_readT();
		bai=T/100;
		shi=T%100/10;
		ge=T%100%10;
		for(i=0;i<200;i++)
		{
			digital_tube(1,bai);
			digital_tube(2,shi);
			digital_tube(3,ge);
		}
	}
		


}
