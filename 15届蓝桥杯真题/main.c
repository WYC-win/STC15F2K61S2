#include <STC15F2K60S2.H>
#include "onewire.h"
#include "iic.h"
#include "ds1302.h"
#include "latch.h"
#include "init.h"
#include "digitaltube.h"
#include "kbd.h"
#define page_freq 0
#define page_para 1
#define page_time 3
#define page_back 4

#define page_para_p1 0
#define page_para_p2 1

#define page_back_freq 0
#define page_back_time 0





unsigned char Y4C=0x80;
unsigned char Y5C=0xA0;
unsigned char Y6C=0xC0;
unsigned char Y7C=0xD0;

unsigned char page_count=page_freq;
unsigned char page_para_count=page_para_p1;
unsigned char page_back_count=page_back_freq;

int freq=0;
int freq_real=0;
unsigned int freq_ex=2000;
int freq_adjust=0;

unsigned char freq_max=0;


void page_freq_display()
{
	freq=freq_real+freq_adjust;
	digitaltube_fixed(1,15);
	if(freq>=0)
	{
		if(freq/10000!=0)
		{
			digitaltube_fixed(4,freq/10000);
			digitaltube_fixed(5,freq%10000/1000);
			digitaltube_fixed(6,freq%1000/100);
			digitaltube_fixed(7,freq%100/10);
			digitaltube_fixed(8,freq%10);
		}
		else if(freq/1000!=0)
		{
			digitaltube_fixed(5,freq/1000);
			digitaltube_fixed(6,freq%1000/100);
			digitaltube_fixed(7,freq%100/10);
			digitaltube_fixed(8,freq%10);
		}
		else if(freq/100!=0)
		{
			digitaltube_fixed(6,freq/100);
			digitaltube_fixed(7,freq%100/10);
			digitaltube_fixed(8,freq%10);
		}
		else if(freq/100!=0)
		{
			digitaltube_fixed(7,freq/10);
			digitaltube_fixed(8,freq%10);
		}
		else if(freq/10!=0)
		{
			digitaltube_fixed(8,freq/10);
		}
	}
	else
	{
		digital_tube(7,0xC7);//L
		digital_tube(8,0xC7);
	}
	
}




void main()
{
	init();
	while(1)
	{
		page_freq_display();
	}
}
