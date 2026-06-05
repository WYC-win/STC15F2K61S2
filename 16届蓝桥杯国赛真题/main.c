#include <STC15F2K60S2.H>
#include <stdio.h>
#include "sys.h"
unsigned char key=0;
unsigned char t=1;
void Timer0_Init(void);
void Timer0_Isr(void);
void main()
{
	init();
	Timer0_Init();
	while(1)
	{
		digitaltube_fix(t,1,0);
	}
}

void Timer0_Isr(void) interrupt 1
{
	key=key_back();
	if(key==4)t++;
	if(key==5)t--;
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
	EA = 1;
}

