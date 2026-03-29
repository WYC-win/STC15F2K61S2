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
#define page_back_time 1

unsigned int t0_count;
unsigned char gate_10ms = 0;

unsigned char page_count=page_freq;
unsigned char page_para_count=page_para_p1;
unsigned char page_back_count=page_back_freq;

long freq=0;
unsigned int freq_real=0;
unsigned int freq_ex=2000;
int freq_adjust=0;

unsigned char hour;
unsigned char min;
unsigned char sec;

unsigned int freq_max=0;

void get_freq_real()
{
	EA=0;
	freq_real=t0_count*10;
	EA=1;
}

void Timer1_Isr(void) interrupt 3
{
	gate_10ms++;

    if(gate_10ms >= 10)
    {
        gate_10ms = 0;

        TR0 = 0;
        t0_count = ((unsigned int)TH0 << 8) | TL0;
        TH0 = 0;
        TL0 = 0;
        TR0 = 1;
    }
}

void Timer_Init(void)		//10毫秒@12.000MHz
{
	AUXR &= 0x00;			//定时器时钟12T模式
	TMOD = 0x04;			//设置定时器模式
	TL1 = 0xF0;				//设置定时初1始值
	TH1 = 0xD8;				//设置定时1初始值
	
	TL0 = 0x00;
	TH0 = 0x00;
	
	TF1 = 0;				//清除TF1标志
	TF0 = 0;
	
	TR1 = 1;				//定时器1开始计时
	TR0 = 1;
	
	ET1 = 1;				//使能定时器1中断
	EA = 1;
}

void read_time()
{
		hour=Read_Ds1302_Byte(0x85);
		min=Read_Ds1302_Byte(0x83);
		sec=Read_Ds1302_Byte(0x81);
		hour=hour/16*10+hour%16;
		min=min/16*10+min%16;
		sec=sec/16*10+sec%16;
}
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
		else if(freq/10!=0)
		{
			digitaltube_fixed(7,freq/10);
			digitaltube_fixed(8,freq%10);
		}
		else 
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

void page_para_display()
{
	digital_tube(1,0x8C);//P
	if(page_para_count==0)
	{
		digitaltube_fixed(2,1);
		digitaltube_fixed(5,freq_ex/1000);
		digitaltube_fixed(6,freq_ex%1000/100);
		digitaltube_fixed(7,freq_ex%100/10);
		digitaltube_fixed(8,freq_ex%10);
	}
	else
	{
		digitaltube_fixed(2,2);
		if(freq_adjust>0)
		{
				digitaltube_fixed(6,freq_adjust/100);
				digitaltube_fixed(7,freq_adjust%100/10);
				digitaltube_fixed(8,freq_adjust%10);
			
		}
		else if(freq_adjust<0)
		{
			int tem_dat;
			tem_dat=0-freq_adjust;
			digital_tube(5,0xBF);//-
			digitaltube_fixed(6,tem_dat/100);
			digitaltube_fixed(7,tem_dat%100/10);
			digitaltube_fixed(8,tem_dat%10);
		}
		else
		{
			digitaltube_fixed(8,freq_adjust);
		}

	}
}


void page_back_display()
{
	digital_tube(1,0x89);//H
	if(page_back_count==0)
	{
		digitaltube_fixed(2,15);
		if(freq_max/10000!=0)
		{
			digitaltube_fixed(4,freq_max/10000);
			digitaltube_fixed(5,freq_max%10000/1000);
			digitaltube_fixed(6,freq_max%1000/100);
			digitaltube_fixed(7,freq_max%100/10);
			digitaltube_fixed(8,freq_max%10);
		}
		else if(freq_max/1000!=0)
		{
			digitaltube_fixed(5,freq_max/1000);
			digitaltube_fixed(6,freq_max%1000/100);
			digitaltube_fixed(7,freq_max%100/10);
			digitaltube_fixed(8,freq_max%10);
		}
		else if(freq_max/100!=0)
		{
			digitaltube_fixed(6,freq_max/100);
			digitaltube_fixed(7,freq_max%100/10);
			digitaltube_fixed(8,freq_max%10);
		}
		else if(freq_max/100!=0)
		{
			digitaltube_fixed(7,freq_max/10);
			digitaltube_fixed(8,freq_max%10);
		}
		else if(freq_max/10!=0)
		{
			digitaltube_fixed(8,freq_max/10);
		}
	}
	else
	{
		digitaltube_fixed(2,10);
	}
}
void page_time_display()
{
	digitaltube_fixed(1,hour/10);
	digitaltube_fixed(2,hour%10);
	digital_tube(3,0xBF);//-
	digitaltube_fixed(4,min/10);
	digitaltube_fixed(5,min%10);
	digital_tube(6,0xBF);
	digitaltube_fixed(7,sec/10);
	digitaltube_fixed(8,sec%10);
}
void main()
{
	init();
	Timer_Init();
	while(1)
	{
		get_freq_real();
		page_freq_display();
	}
}
