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
#include "AD.h"
#define page_environment 0
#define page_sport 1
#define page_para 2
#define page_summary 3

#define page_para_temprature 0
#define page_para_distance 1

unsigned char page_count=0;
unsigned char page_para_count=0;

unsigned char para_temprature=30;
unsigned char para_distance=30;

unsigned char key_number;
unsigned char light_level;
unsigned int super_distance;
unsigned char temperature;
unsigned char sport_state;
unsigned int last_super_distance=0;
unsigned int distance_change=0;


unsigned int summary=0;

bit hightemp_flag=0;
bit close_flag=0;
bit relay_report=0;

volatile unsigned int key_double_ms=0;
volatile unsigned char getdistance_ms=0;
volatile unsigned char gettemprature_ms=0;
volatile unsigned char getlightlevel_ms=0;
volatile unsigned int judge_sportstate_ms=0;
volatile unsigned char LED_out_ms=0;

void Timer_Init(void)		//10毫秒@12.000MHz
{
	AUXR = 0x00 ;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0xF0;				//设置定时初始值
	TH0 = 0xD8;				//设置定时初始值
	
//	TL1 = 0x18;				//设置定时初始值
//	TH1 = 0xFC;				//设置定时初始值
//	TF1 = 0;				//清除TF1标志
//	TR1 = 1;				//定时器1开始计时
//	ET1 = 1;				//使能定时器1中断

	
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
	EA=1;
}


void page_environment_display()
{
	digitaltube_fixed(1,12);
	digitaltube(7,0xc8);
	digitaltube_fixed(2,temperature/10);
	digitaltube_fixed(3,temperature%10);
	digitaltube_fixed(8,light_level);
}

void page_sport_display()
{
	digitaltube(1,0xc7);
	digitaltube_fixed(6,super_distance/100);
	digitaltube_fixed(7,super_distance%100/10);
	digitaltube_fixed(8,super_distance%10);
	digitaltube_fixed(2,sport_state);
}

void page_para_display()
{
	digitaltube(1,0x8C);
	switch(page_para_count)
	{
		case 0:	digitaltube_fixed(2,12);
				digitaltube_fixed(7,para_temprature/10);
				digitaltube_fixed(8,para_temprature%10);
				break;
		case 1:	digitaltube(2,0xc7);
				digitaltube_fixed(7,para_distance/10);
				digitaltube_fixed(8,para_distance%10);
				break;
	}
}
void page_summary_display()
{
	digitaltube(1,0xc8);
	digitaltube_fixed(2,12);
	if(summary/1000!=0)
	{
		digitaltube_fixed(5,summary/1000);
		digitaltube_fixed(6,summary%1000/100);
		digitaltube_fixed(7,summary%100/10);
		digitaltube_fixed(8,summary%10);
	}
	if(summary/100!=0)
	{
		digitaltube_fixed(6,summary/100);
		digitaltube_fixed(7,summary%100/10);
		digitaltube_fixed(8,summary%10);
	}
	if(summary/10!=0)
	{
		digitaltube_fixed(7,summary/10);
		digitaltube_fixed(8,summary%10);
	}
	if(summary/10==0)
	{
		digitaltube_fixed(8,summary);
	}
}

void relay_out()
{
	if(hightemp_flag==1 && close_flag==1)
	{
		P0=0x10;
		if(relay_report==0)
		{
			summary++;
			relay_report=1;
		}
	}
	else
	{
		P0=0x00;
		relay_report=0;
	}
	latch(5);
	
}
void LED_out()
{
	P0=0xFF;
	if(close_flag==1)
	{
		if(light_level==1)
		{
			P0&=0xFE;
		}
		if(light_level==2)
		{
			P0&=0xFC;
		}
		if(light_level==3)
		{
			P0&=0xF8;
		}
		if(light_level==4)
		{
			P0&=0xF0;
		}
	}
	else
	{
		P0&=0xFF;
	}
	if(sport_state==1)
	{
		P0&=0xFF;
	}
	if(sport_state==2)
	{
		P0&=0x7F;
	}
	if(sport_state==3)
	{
		if(LED_out_ms<10)P0&=0x7F;
	}
	latch(4);
}
unsigned char judge_sport_state()
{
	if(last_super_distance>=super_distance)
	{
		distance_change=last_super_distance-super_distance;
	}
	else
	{
		distance_change=super_distance-last_super_distance;
	}
	if(distance_change>=10) return 3;
	if(distance_change>=5) return 2;
	else return 1;
}
void Timer0_Isr(void) interrupt 1
{
	LED_out_ms++;
	if(LED_out_ms==20)LED_out_ms=0;
	judge_sportstate_ms++;
	getdistance_ms++;
	gettemprature_ms++;
	getlightlevel_ms++;
	if(page_count==3&&key_double_back(8)&&key_double_back(9))
	{
			key_double_ms++;
			if(key_number==4)key_double_ms=0;
			if(key_double_ms>200)
			{
				summary=0;
				key_double_ms=0;
			}
	}
	key_number=key_back();
	if(key_number==4)
	{
		page_count++;
		page_para_count=0;
		if(page_count>3)page_count=0;
	}
	if(page_count==2)
	{
		if(key_number==5)
		{
			page_para_count++;
			if(page_para_count>1)
			{
				page_para_count=0;
			}
		}
		if(page_para_count==0)
		{
			if(key_number==8)
			{
				if(para_temprature<=79)
					para_temprature++;
				else
					para_temprature=80;
			}
			if(key_number==9)
			{
				if(para_temprature>=21)
					para_temprature--;
				else
					para_temprature=20;
			}	
		}
		if(page_para_count==1)
		{
			if(key_number==8)
			{
				if(para_distance<=75)
					para_distance+=5;
				else
					para_distance=80;
			}
			if(key_number==9)
			{
				if(para_distance>=25)
					para_distance-=5;
				else
					para_distance=20;
			}	
		}
	}
}


void main()
{
	init();
	ds18b20_convertT();
	delay(750);
	super_distance=get_distance();
	last_super_distance=super_distance;
	temperature=ds18b20_readT();
	light_level=AD_read(0x01);
	Timer_Init();
	while(1)
	{
		if(super_distance<para_distance)
		{
			close_flag=1;
		}
		else close_flag=0;
		if(temperature>para_temprature)
		{
			hightemp_flag=1;
		}
		else hightemp_flag=0;
		if(getdistance_ms>100)
		{
			last_super_distance=super_distance;
			super_distance=get_distance();
			if(judge_sportstate_ms>300)
			{
				sport_state=judge_sport_state();
				judge_sportstate_ms=0;
			}
			getdistance_ms=0;
		}
		if(gettemprature_ms>10)
		{
			ds18b20_convertT();
			temperature=ds18b20_readT();
			gettemprature_ms=0;
		}
		if(getlightlevel_ms>10)
		{
			light_level=AD_read(0x01);
			getlightlevel_ms=0;
		}
		LED_out();
		relay_out();
		if(page_count==page_environment)
		{
			page_environment_display();
		}
		else if(page_count==page_sport)
		{
			page_sport_display();
		}
		else if(page_count==page_para)
		{
			page_para_display();
		}
		else
		{
			page_summary_display();
		}
	}
}
