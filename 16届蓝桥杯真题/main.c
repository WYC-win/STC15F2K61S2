#include <STC15F2K60S2.H>
#include "delay.h"
#include "digital_tube.h"
#include "init.h"
#include "DS18B20.h"
#include "uart.h"
#include "kbd.h"
#include "iic.h"
#include "onewire.h"
#include "AD_DA.h"
void environment();
int tem,i;
unsigned char light_intensity_digtal,light_intensity;
void main()
{
	init();
	tem=30;
	DS18B20_convertT();
	delay(750);
	while(1)
	{
		environment();
	}
}

void environment()
{
	DS18B20_convertT();
	light_intensity_digtal=AD_read(0x01);
	if(light_intensity_digtal>=151)light_intensity=1;
	else if(light_intensity_digtal>=102&&light_intensity_digtal<151)light_intensity=2;
	else if(light_intensity_digtal>=25.5&&light_intensity_digtal<102)light_intensity=3;
	else if(light_intensity_digtal<151)light_intensity=4;
	for(i=0;i<200;i++)
	{
		digital_tube(1,12);
		digital_tube(2,tem%100/10);
		digital_tube(3,tem%100%10);
		digital_tube(7,16);
		digital_tube(8,light_intensity);
	}
	tem=DS18B20_readT();
}
