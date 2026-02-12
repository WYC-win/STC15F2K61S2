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
#include "superwave.h"
void environment();
void sport();
int tem,i,distance;
unsigned char light_intensity_digtal,light_intensity;
void main()
{
	init();
	tem=30;
	distance=30;
	DS18B20_convertT();
	delay(750);
	while(1)
	{
//		environment();
		sport();
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
		digital_tubefixed(1,12);
		digital_tubefixed(2,tem%100/10);
		digital_tubefixed(3,tem%100%10);
		digital_tube(7,0xC8);
		digital_tubefixed(8,light_intensity);
	}
	tem=DS18B20_readT();
}

void sport()
{
	int dis1,dis2;
	
	digital_tubefixed(6,distance/100);
	digital_tubefixed(7,distance%100/10);
	digital_tubefixed(8,distance%100%10);
	distance=superwave_getdistance();
}
