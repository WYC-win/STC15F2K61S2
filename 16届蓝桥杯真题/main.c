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
void Parameter();
int tem,i,distance,counter=0,counter1=0,tem_para=30,dis_para=30;
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
		if(counter==0)
		{
			Parameter();
		}
	}
}

void environment()
{
	
	DS18B20_convertT();
	light_intensity_digtal=AD_read(0x01);
	if(light_intensity_digtal>=153)light_intensity=1;
	else if(light_intensity_digtal>=102&&light_intensity_digtal<153)light_intensity=2;
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
	digital_tube(1,0xC7);//L
	digital_tubefixed(6,distance/100);
	digital_tubefixed(7,distance%100/10);
	digital_tubefixed(8,distance%100%10);
	distance=superwave_getdistance();
}

void Parameter()//参数界面
{
	if(counter1==0)
	{
		digital_tube(1,0x8C);//P
		digital_tubefixed(2,12);
		digital_tubefixed(7,tem_para/10);
		digital_tubefixed(8,tem_para%10);
		if(keynumber()==8)
		{
			tem_para++;
		}
		else if(keynumber()==9)
		{
			tem_para--;
		}
		else if(keynumber()==8 && keynumber()==9)
		{
			tem_para=0;
		}
	}
	if(counter1==1)
	{
		digital_tube(1,0x8C);//P
		digital_tube(2,0xC7);//L
		digital_tubefixed(7,dis_para/10);
		digital_tubefixed(8,dis_para%10);
		if(keynumber()==8)
		{
			dis_para++;
		}
		else if(keynumber()==9)
		{
			dis_para--;
		}
		else if(keynumber()==8 && keynumber()==9)
		{
			dis_para=0;
		}
	}
	if(keynumber()==5)
	{
		counter1++;
		if(counter1==2)counter1=0;
	}
	
}
