#include <STC15F2K60S2.H>
#include "delay.h"
#include "digitaltube.h"
#include "kbd.h"
#include "init.h"
#include "at24c02.h"
#include "AD.h"
#include "latch.h"
#define page_data 0
#define page_para 1
#define page_sumarry 2

unsigned char key_back=0;
unsigned char page_count=0;
unsigned char data_at24c02[3]={0,0,0};

bit at24c02_flag=0;
bit high=0;
bit low=0;
float voltage_data=0;
float voltage_para=3.00;
unsigned long summary_count=0;

unsigned char wrong_key_count=0;
unsigned char get_data_ms=0;
unsigned int L1_ms=0;
void page_data_display()
{
	digitaltube(1,0xC1);
	digitaltube_fixed_dp(6,(int)voltage_data);
	digitaltube_fixed(7,(int)(voltage_data*10)%10);
	digitaltube_fixed(8,(int)(voltage_data*100)%10);
}
void page_para_display()
{
	digitaltube(1,0x8C);
	digitaltube_fixed_dp(6,(int)voltage_para);
	digitaltube_fixed(7,(int)(voltage_para*10)%10);
	digitaltube_fixed(8,(int)(voltage_para*100)%10);
}

void page_summary_display()
{
	digitaltube(1,0xC8);
	if(summary_count/1000000!=0)
	{
		digitaltube_fixed(2,summary_count/1000000);
		digitaltube_fixed(3,summary_count%1000000/100000);
		digitaltube_fixed(4,summary_count%100000/10000);
		digitaltube_fixed(5,summary_count%10000/1000);
		digitaltube_fixed(6,summary_count%1000/100);
		digitaltube_fixed(7,summary_count%100/10);
		digitaltube_fixed(8,summary_count%10);
	}
	else if(summary_count/100000!=0)
	{
		digitaltube_fixed(3,summary_count/100000);
		digitaltube_fixed(4,summary_count%100000/10000);
		digitaltube_fixed(5,summary_count%10000/1000);
		digitaltube_fixed(6,summary_count%1000/100);
		digitaltube_fixed(7,summary_count%100/10);
		digitaltube_fixed(8,summary_count%10);
	}
	else if(summary_count/10000!=0)
	{
		digitaltube_fixed(4,summary_count/10000);
		digitaltube_fixed(5,summary_count%10000/1000);
		digitaltube_fixed(6,summary_count%1000/100);
		digitaltube_fixed(7,summary_count%100/10);
		digitaltube_fixed(8,summary_count%10);
	}
	else if(summary_count/1000!=0)
	{
		digitaltube_fixed(5,summary_count/1000);
		digitaltube_fixed(6,summary_count%1000/100);
		digitaltube_fixed(7,summary_count%100/10);
		digitaltube_fixed(8,summary_count%10);
	}
	else if(summary_count/100!=0)
	{
		digitaltube_fixed(6,summary_count/100);
		digitaltube_fixed(7,summary_count%100/10);
		digitaltube_fixed(8,summary_count%10);
	}
	else if(summary_count/10!=0)
	{
		digitaltube_fixed(7,summary_count/10);
		digitaltube_fixed(8,summary_count%10);
	}
	else
	{
		digitaltube_fixed(8,summary_count);
	}
}
void Timer0_Isr(void) interrupt 1
{
	key_back=kbd();
	if(page_count==1)
	{
		if(key_back==12)
		{
			at24c02_flag=1;
			data_at24c02[0]=(char)(voltage_para*10);
		}
	}
			
	if(voltage_data<voltage_para)
	{
		L1_ms++;
	}
	else
	{
		L1_ms=0;
	}
	
	if(voltage_data>voltage_para)
	{
		high=1;
		low=0;
	}
	if(high==1)
	{
		if(voltage_data<voltage_para)
		{
			low=1;
			summary_count++;
			high=0;
		}
	}
	get_data_ms++;
	if(key_back!=12 && key_back!=13 && key_back!=16 && key_back!=17 && key_back!=0)
	{
		wrong_key_count++;
	}
	if(key_back==12 || key_back==13 || key_back==16 || key_back==17)
	{
		wrong_key_count=0;
	}
	if(key_back==12)
	{
		page_count++;
		if(page_count>2)
		{
			page_count=0;
		}
	}
	if(page_count==2)
	{
		if(key_back==13)
		{
			summary_count=0;
		}
	}
	if(page_count==1)
	{
		if(key_back==16)
		{
			if(voltage_para<5.0)
			{
				voltage_para+=0.5;
			}
			
			else
			{
				voltage_para=0.00;
			}
		}
		else if(key_back==17)
		{
			if(voltage_para>0.0)
			{
				voltage_para-=0.5;
			}
			else
			{
				voltage_para=5.0;
			}
		}
	}
}

void led_out()
{
	P0=0xFF;
	if(L1_ms>500)
	{
		P0&=0xFE;
	}
	else
	{
		P0&=0xFF;
	}
	if(summary_count%2!=0)
	{
		P0&=0xFD;
	}
	else
	{
		P0&=0xFF;
	}
	if(wrong_key_count>=3)
	{
		P0&=0xFB;
	}
	else
	{
		P0&=0xFF;
	}
	latch(4);
}
void Timer0_Init(void)		//10毫秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0xF0;				//设置定时初始值
	TH0 = 0xD8;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
	EA=1;
}


void main()
{
	init();
	Timer0_Init();
	at24c02_read(data_at24c02,0,1);
	voltage_para=(float)data_at24c02[0]*1.0/10;
	while(1)
	{
		led_out();
		if(at24c02_flag==1)
		{
			at24c02_write(data_at24c02,0,1);
			at24c02_flag=0;
		}
		if(get_data_ms>5)
		{
			voltage_data=get_ain3();
			get_data_ms=0;
		}
		if(page_count==page_data)
		{
			page_data_display();
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