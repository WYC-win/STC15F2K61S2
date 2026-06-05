#include <STC15F2K60S2.H>
#include <stdio.h>
#include "sys.h"
#include "ds1302.h"
#include "onewire.h"
#include "iic.h"

#define page_data 0
#define page_para 1
#define page_standard 2


#define page_data_time 0
#define page_data_liquid 1
#define page_data_volume 2
#define page_data_weight 3

#define page_para_H1 1
#define page_para_H2 2
#define page_para_F 3
#define page_para_S 4
#define page_para_r 5
#define page_para_L 6
#define page_para_W 7
#define page_para_H 8

#define page_standard_0 0
#define page_standard_5 1
#define page_standard_10 2
#define page_standard_15 3

#define container_ball 0
#define container_cylinder 1 
#define container_box 2

unsigned char key_number=0;
unsigned char page_count=0;
unsigned char page_data_count=0;
unsigned char page_para_count=1;
unsigned char page_standard_count=0;
unsigned char freq_measure_timecount=0;

unsigned char hour_now=0;
unsigned char min_now=0;
unsigned char sec_now=0;

int temperature_now=0;
float liquid_high_now=0;
unsigned char superwave_time_count=0;

unsigned char container_type=2;
float xdata r,L,W,H_container,H1,H2;
unsigned  int F;
float h;

unsigned char room_remain_dis;
float volume_now;

float xdata w_standard_0,w_standard_5,w_standard_10,w_standard_15,w_standard_20;
float xdata w_standard_0_last,w_standard_5_last,w_standard_10_last,w_standard_15_last,w_standard_20_last;
float v_now;

float weight_now;

unsigned int freq_now=0;

unsigned char xdata uart_receive[64];
unsigned char uart_receive_index=0;

void Timer0_Init(void);
void Timer0_Isr(void);
unsigned char time_shift(unsigned char addr);
void page_data_dis();
void page_para_dis();
void page_standard_dis();
void para_init();
float volume_get(unsigned char type);
unsigned char room_remain(unsigned char type);
void weight_deal();
void freq_get();
void Timer1_Init(void);

void main()
{
	init();
	para_init();
	Timer1_Init();
	ds1302_init();
	convert_T();
	delay(100);
	Timer0_Init();
	while(1)
	{
		freq_get();
//		digitaltube_fix(1,freq_now/10000,0);
//		digitaltube_fix(2,freq_now/1000%10,0);
//		digitaltube_fix(3,freq_now/100%10,0);
//		digitaltube_fix(4,freq_now/10%10,0);
//		digitaltube_fix(5,freq_now%10,0);
		if(superwave_time_count>=5)
		{
			liquid_high_now=get_distance()/100;
			h=liquid_high_now;
			superwave_time_count=0;
		}
		v_now=AD_read(0x03);
		weight_deal();
		volume_now=volume_get(container_type);
		room_remain_dis=room_remain(container_type);
		hour_now=time_shift(0x85);
		min_now=time_shift(0x83);
		sec_now=time_shift(0x81);
		temperature_now=read_T();
		if(page_count==page_data)
		{
			page_data_dis();
		}
		else if(page_count==page_para)
		{
			page_para_dis();
		}
		else
		{
			page_standard_dis();
		}

	}
}
void Timer1_Isr(void) interrupt 3
{
	freq_measure_timecount++;
	superwave_time_count++;
	key_number=key_back();
	
	
	
	if(page_count==0)
	{
		if(key_number==4)
		{
			page_count++;
			page_data_count=0;
			page_para_count=1;
			page_standard_count=0;
			if(page_count>2)page_count=0;
		}
		if(key_number==5)
		{
			page_data_count++;
			if(page_data_count>3)page_data_count=0;
		}
	}
	
	else if(page_count==1)
	{
		if(key_number==4)
		{
			page_count++;
			page_data_count=0;
			page_para_count=1;
			page_standard_count=0;
			if(page_count>2)page_count=0;
		}
		if(key_number==5)
		{
			page_para_count++;
			if(page_para_count>8)page_para_count=1;
		}
	}
	
	else if(page_count==2)
	{
		w_standard_0_last=w_standard_0;
		w_standard_5_last=w_standard_5;
		w_standard_10_last=w_standard_10;
		w_standard_15_last=w_standard_15;
		w_standard_20_last=w_standard_20;
		if(key_number==5)
		{
			page_standard_count++;
			if(page_standard_count>4)page_standard_count=0;
		}
		if(key_number==8)
		{
			switch(page_standard_count)
			{
				case 0:w_standard_0+=0.1;break;
				case 1:w_standard_5+=0.1;break;
				case 2:w_standard_10+=0.1;break;
				case 3:w_standard_15+=0.1;break;
				case 4:w_standard_20+=0.1;break;
			}
		}
		if(key_number==9)
		{
			switch(page_standard_count)
			{
				case 0:w_standard_0-=0.1;break;
				case 1:w_standard_5-=0.1;break;
				case 2:w_standard_10-=0.1;break;
				case 3:w_standard_15-=0.1;break;
				case 4:w_standard_20-=0.1;break;
			}
		}
		if(key_number==4)
		{
			if(w_standard_0<w_standard_5 && w_standard_5<w_standard_10 && w_standard_10<w_standard_15 && w_standard_15<w_standard_20)
			{
				
			}
			else
			{
				w_standard_0=w_standard_0_last;
				w_standard_5=w_standard_5_last;
				w_standard_10=w_standard_10_last;
				w_standard_15=w_standard_15_last;
				w_standard_20=w_standard_20_last;
			}
			page_count++;
			page_data_count=0;
			page_para_count=1;
			page_standard_count=0;
			if(page_count>2)page_count=0;
		}
	}
	
}

void Timer1_Init(void)		//10毫秒@12.000MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0xF0;				//设置定时初始值
	TH1 = 0xD8;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
	ET1 = 1;				//使能定时器1中断
	EA =1;
}
void Timer0_Init(void)		
{
	AUXR &= 0x7F;			
	TMOD &= 0xF0;	
	TMOD |=	0x05;
	TL0 = 0x00;				
	TH0 = 0x00;								
	TR0 = 1;							
}

void Uart1_Isr(void) interrupt 4
{
	if (RI)				//检测串口1接收中断
	{
		uart_receive[uart_receive_index]=SBUF;
		uart_receive_index++;
		RI = 0;			//清除串口1接收中断请求位
	}
}

void Uart1_Init(void)	//9600bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR &= 0xFB;		//定时器时钟12T模式
	T2L = 0xE6;			//设置定时初始值
	T2H = 0xFF;			//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
	ES = 1;				//使能串口1中断
}

void para_init()
{
	H1=1.0;
	H2=0.1;
	F=2000;
	r=1.0;
	L=1.0;
	W=1.0;
	H_container=1.0;
	w_standard_0=0;
	w_standard_5=1.0;
	w_standard_10=2.0;
	w_standard_15=4.0;
	w_standard_20=5.0;
}
float volume_get(unsigned char type)
{
	float temp;
	if(type==0)
	{
		temp=(3.14*(2*r-h)*(2*r-h)*(r+h))/3;
	}
	 else if(type==1)
	{
		temp=3.14*r*r*(H_container-h);
	}
	else if(type==2)
	{
		temp=L*W*(H_container-h);
	}
	return temp;
}
void weight_deal()
{
	if(v_now<=w_standard_5)
	{
		weight_now=(v_now-w_standard_0)/(w_standard_5-w_standard_0)*5;
	}
	else if(v_now<=w_standard_10)
	{
		weight_now=(v_now-w_standard_5)/(w_standard_10-w_standard_5)*5+5;
	}
	else if(v_now<=w_standard_15)
	{
		weight_now=(v_now-w_standard_10)/(w_standard_15-w_standard_10)*5+10;
	}
	else if(v_now<=w_standard_20)
	{
		weight_now=(v_now-w_standard_15)/(w_standard_20-w_standard_15)*5+15;
	}
}
unsigned char room_remain(unsigned char type)
{
	float temp;
	if(type==0)
	{
		temp=(((4*3.14*r*r*r)/3-(3.14*(2*r-h)*(2*r-h)*(r+h))/3)*100)/((4*3.14*r*r*r)/3);
		return (unsigned char)temp;
	}
	 else if(type==1||type==2)
	{
		temp=h*100/H_container;
		return (unsigned char)temp;
	}
	else return 0;
}

unsigned char time_shift(unsigned char addr)
{
	unsigned char tem;
	tem=Read_Ds1302_Byte(addr);
	return tem/16*10+tem%16;
}
	
void freq_get()
{
	if(freq_measure_timecount>=100)
	{
		TR0=0;
		freq_now=(((int)TH0<<8)|TL0);
		TL0=0x00;TH0=0x00;
		freq_measure_timecount=0;
		TR0=1;
	}
}
void page_data_dis()
{
	if(page_data_count==0)
	{
		digitaltube_fix(1,hour_now/10,0);
		digitaltube_fix(2,hour_now%10,0);
		digitaltube(3,0xBF,0);
		digitaltube_fix(4,min_now/10,0);
		digitaltube_fix(5,min_now%10,0);
		digitaltube(6,0xBF,0);
		digitaltube_fix(7,sec_now/10,0);
		digitaltube_fix(8,sec_now%10,0);
	}
	else if(page_data_count==1)
	{
		if(temperature_now>0)
		{
			digitaltube_fix(3,temperature_now/10,0);
			digitaltube_fix(4,temperature_now%10,0);
			digitaltube(5,0xBF,0);
			digitaltube_fix(6,(int)(liquid_high_now*100)/100,1);
			digitaltube_fix(7,(int)(liquid_high_now*100)/10,0);
			digitaltube_fix(8,(int)(liquid_high_now*100)%10,0);
		}
		else
		{
			digitaltube(2,0xBF,0);
			digitaltube_fix(3,temperature_now/10,0);
			digitaltube_fix(4,temperature_now%10,0);
			digitaltube(5,0xBF,0);
			digitaltube_fix(6,(int)(liquid_high_now*100)/100,1);
			digitaltube_fix(7,(int)(liquid_high_now*100)%100/10,0);
			digitaltube_fix(8,(int)(liquid_high_now*100)%10,0);
		}
		
	}
	else if(page_data_count==2)
	{
		if(room_remain_dis>=100)
		{
			digitaltube_fix(1,room_remain_dis/100,0);
			digitaltube_fix(2,room_remain_dis%100/10,0);
			digitaltube_fix(3,room_remain_dis%10,0);
		}
		else
		{
			digitaltube_fix(2,room_remain_dis/10,0);
			digitaltube_fix(3,room_remain_dis%10,0);
		}
		digitaltube(4,0xBF,0);
//		unsigned char digits[4];
//		digits[0]=(int)(volume_now*10)/1000;
//		digits[1]=(int)(volume_now*10)/100%10;
//		digits[2]=(int)(volume_now*10)/10%10;
//		digits[3]=(int)(volume_now*10)%10;
//		
//		unsigned char start = 0;
//		unsigned char i;
//		while (start < 4 && digits[start] == 0)
//			start++;
//		for (i = start; i < 5; i++)
//			digitaltube_fixed(5 + i, digits[i],0);
		digitaltube_fix(7,(int)(volume_now*10)/10,1);
		digitaltube_fix(8,(int)(volume_now*10)%10,0);

	}
	else
	{
		if(temperature_now>0)
		{
			digitaltube_fix(3,temperature_now/10,0);
			digitaltube_fix(4,temperature_now%10,0);
			digitaltube(5,0xBF,0);
			if(weight_now>10)
			{
				digitaltube_fix(6,(int)(weight_now*10)/100,0);
				digitaltube_fix(7,(int)(weight_now*10)/10%10,1);
				digitaltube_fix(8,(int)(weight_now*10)%10,0);
			}
			else 
			{
				digitaltube_fix(7,(int)(weight_now*10)/10,1);
				digitaltube_fix(8,(int)(weight_now*10)%10,0);
			}
		}
		else
		{
			digitaltube(2,0xBF,0);
			digitaltube_fix(3,temperature_now/10,0);
			digitaltube_fix(4,temperature_now%10,0);
			digitaltube(5,0xBF,0);
			if(weight_now>10)
			{
				digitaltube_fix(6,(int)(weight_now*10)/100,0);
				digitaltube_fix(7,(int)(weight_now*10)/10%10,1);
				digitaltube_fix(8,(int)(weight_now*10)%10,0);
			}
			else 
			{
				digitaltube_fix(7,(int)(weight_now*10)/10,1);
				digitaltube_fix(8,(int)(weight_now*10)%10,0);
			}
		}
	}
}

void page_para_dis()
{
	digitaltube(1,0x8C,0);
	digitaltube_fix(2,page_para_count,0);
	digitaltube(3,0xBF,0);
	if(page_para_count==1)
	{
		if((unsigned int)(H1*10)>10000)
		{
			digitaltube_fix(4,(unsigned int)(H1*10)/10000,0);
			digitaltube_fix(5,(unsigned int)(H1*10)/1000%10,0);
			digitaltube_fix(6,(unsigned int)(H1*10)/100%10,0);
			digitaltube_fix(7,(unsigned int)(H1*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(H1*10)%10,0);
		}
		else if((unsigned int)(H1*10)>1000)
		{
			digitaltube_fix(5,(unsigned int)(H1*10)/1000,0);
			digitaltube_fix(6,(unsigned int)(H1*10)/100%10,0);
			digitaltube_fix(7,(unsigned int)(H1*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(H1*10)%10,0);
		}
		else if((unsigned int)(H1*10)>100)
		{
			digitaltube_fix(6,(unsigned int)(H1*10)/100,0);
			digitaltube_fix(7,(unsigned int)(H1*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(H1*10)%10,0);
		}
		else
		{
			digitaltube_fix(7,(unsigned int)(H1*10)/10,1);
			digitaltube_fix(8,(unsigned int)(H1*10)%10,0);
		}
	}
	else if(page_para_count==2)
	{
		if((unsigned int)(H2*10)>10000)
		{
			digitaltube_fix(4,(unsigned int)(H2*10)/10000,0);
			digitaltube_fix(5,(unsigned int)(H2*10)/1000%10,0);
			digitaltube_fix(6,(unsigned int)(H2*10)/100%10,0);
			digitaltube_fix(7,(unsigned int)(H2*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(H2*10)%10,0);
		}
		else if((unsigned int)(H2*10)>1000)
		{
			digitaltube_fix(5,(unsigned int)(H2*10)/1000,0);
			digitaltube_fix(6,(unsigned int)(H2*10)/100%10,0);
			digitaltube_fix(7,(unsigned int)(H2*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(H2*10)%10,0);
		}
		else if((unsigned int)(H2*10)>100)
		{
			digitaltube_fix(6,(unsigned int)(H2*10)/100,0);
			digitaltube_fix(7,(unsigned int)(H2*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(H2*10)%10,0);
		}
		else
		{
			digitaltube_fix(7,(unsigned int)(H2*10)/10,1);
			digitaltube_fix(8,(unsigned int)(H2*10)%10,0);
		}
	}
	else if(page_para_count==3)
	{
		if(F>10000)
		{
			digitaltube_fix(4,F/10000,0);
			digitaltube_fix(5,F/1000%10,0);
			digitaltube_fix(6,F/100%10,0);
			digitaltube_fix(7,F/10%10,0);
			digitaltube_fix(8,F%10,0);
		}
		else if(F>1000)
		{
			digitaltube_fix(5,F/1000,0);
			digitaltube_fix(6,F/100%10,0);
			digitaltube_fix(7,F/10%10,0);
			digitaltube_fix(8,F%10,0);
		}
		else if(F>100)
		{
			digitaltube_fix(6,F/100,0);
			digitaltube_fix(7,F/10%10,0);
			digitaltube_fix(8,F%10,0);
		}
		else if(F>10)
		{
			digitaltube_fix(7,F/10,0);
			digitaltube_fix(8,F%10,0);
		}
		else
		{
			digitaltube_fix(8,F%10,0);	
		}
	}
	else if(page_para_count==4)
	{
		digitaltube_fix(8,container_type,0);
	}
	else if(page_para_count==5)
	{
		if((unsigned int)(r*10)>10000)
		{
			digitaltube_fix(4,(unsigned int)(r*10)/10000,0);
			digitaltube_fix(5,(unsigned int)(r*10)/1000%10,0);
			digitaltube_fix(6,(unsigned int)(r*10)/100%10,0);
			digitaltube_fix(7,(unsigned int)(r*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(r*10)%10,0);
		}
		else if((unsigned int)(r*10)>1000)
		{
			digitaltube_fix(5,(unsigned int)(r*10)/1000,0);
			digitaltube_fix(6,(unsigned int)(r*10)/100%10,0);
			digitaltube_fix(7,(unsigned int)(r*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(r*10)%10,0);
		}
		else if((unsigned int)(r*10)>100)
		{
			digitaltube_fix(6,(unsigned int)(r*10)/100,0);
			digitaltube_fix(7,(unsigned int)(r*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(r*10)%10,0);
		}
		else
		{
			digitaltube_fix(7,(unsigned int)(r*10)/10,1);
			digitaltube_fix(8,(unsigned int)(r*10)%10,0);
		}
	}
	else if(page_para_count==6)
	{
		if((unsigned int)(L*10)>10000)
		{
			digitaltube_fix(4,(unsigned int)(L*10)/10000,0);
			digitaltube_fix(5,(unsigned int)(L*10)/1000%10,0);
			digitaltube_fix(6,(unsigned int)(L*10)/100%10,0);
			digitaltube_fix(7,(unsigned int)(L*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(L*10)%10,0);
		}
		else if((unsigned int)(L*10)>1000)
		{
			digitaltube_fix(5,(unsigned int)(L*10)/1000,0);
			digitaltube_fix(6,(unsigned int)(L*10)/100%10,0);
			digitaltube_fix(7,(unsigned int)(L*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(L*10)%10,0);
		}
		else if((unsigned int)(L*10)>100)
		{
			digitaltube_fix(6,(unsigned int)(L*10)/100,0);
			digitaltube_fix(7,(unsigned int)(L*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(L*10)%10,0);
		}
		else
		{
			digitaltube_fix(7,(unsigned int)(L*10)/10,1);
			digitaltube_fix(8,(unsigned int)(L*10)%10,0);
		}
	}
	else if(page_para_count==7)
	{
		if((unsigned int)(W*10)>10000)
		{
			digitaltube_fix(4,(unsigned int)(W*10)/10000,0);
			digitaltube_fix(5,(unsigned int)(W*10)/1000%10,0);
			digitaltube_fix(6,(unsigned int)(W*10)/100%10,0);
			digitaltube_fix(7,(unsigned int)(W*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(W*10)%10,0);
		}
		else if((unsigned int)(W*10)>1000)
		{
			digitaltube_fix(5,(unsigned int)(W*10)/1000,0);
			digitaltube_fix(6,(unsigned int)(W*10)/100%10,0);
			digitaltube_fix(7,(unsigned int)(W*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(W*10)%10,0);
		}
		else if((unsigned int)(W*10)>100)
		{
			digitaltube_fix(6,(unsigned int)(W*10)/100,0);
			digitaltube_fix(7,(unsigned int)(W*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(W*10)%10,0);
		}
		else
		{
			digitaltube_fix(7,(unsigned int)(W*10)/10,1);
			digitaltube_fix(8,(unsigned int)(W*10)%10,0);
		}
	}
	else if(page_para_count==8)
	{
		if((unsigned int)(H_container*10)>10000)
		{
			digitaltube_fix(4,(unsigned int)(H_container*10)/10000,0);
			digitaltube_fix(5,(unsigned int)(H_container*10)/1000%10,0);
			digitaltube_fix(6,(unsigned int)(H_container*10)/100%10,0);
			digitaltube_fix(7,(unsigned int)(H_container*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(H_container*10)%10,0);
		}
		else if((unsigned int)(H_container*10)>1000)
		{
			digitaltube_fix(5,(unsigned int)(H_container*10)/1000,0);
			digitaltube_fix(6,(unsigned int)(H_container*10)/100%10,0);
			digitaltube_fix(7,(unsigned int)(H_container*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(H_container*10)%10,0);
		}
		else if((unsigned int)(H_container*10)>100)
		{
			digitaltube_fix(6,(unsigned int)(H_container*10)/100,0);
			digitaltube_fix(7,(unsigned int)(H_container*10)/10%10,1);
			digitaltube_fix(8,(unsigned int)(H_container*10)%10,0);
		}
		else
		{
			digitaltube_fix(7,(unsigned int)(H_container*10)/10,1);
			digitaltube_fix(8,(unsigned int)(H_container*10)%10,0);
		}
	}
}

void page_standard_dis()
{
	digitaltube_fix(1,14,0);
	if(page_standard_count==0)
	{
		digitaltube_fix(2,0,0);
		digitaltube_fix(3,0,0);
		digitaltube_fix(7,(int)(w_standard_0*10)/10,1);
		digitaltube_fix(8,(int)(w_standard_0*10)%10,0);
	}
	else if(page_standard_count==1)
	{
		digitaltube_fix(2,0,0);
		digitaltube_fix(3,5,0);
		digitaltube_fix(7,(int)(w_standard_5*10)/10,1);
		digitaltube_fix(8,(int)(w_standard_5*10)%10,0);
	}
	else if(page_standard_count==2)
	{
		digitaltube_fix(2,1,0);
		digitaltube_fix(3,0,0);
		digitaltube_fix(7,(int)(w_standard_10*10)/10,1);
		digitaltube_fix(8,(int)(w_standard_10*10)%10,0);
	}
	else if(page_standard_count==3)
	{
		digitaltube_fix(2,1,0);
		digitaltube_fix(3,5,0);
		digitaltube_fix(7,(int)(w_standard_15*10)/10,1);
		digitaltube_fix(8,(int)(w_standard_15*10)%10,0);
	}
	else if(page_standard_count==4)
	{
		digitaltube_fix(2,2,0);
		digitaltube_fix(3,0,0);
		digitaltube_fix(7,(int)(w_standard_20*10)/10,1);
		digitaltube_fix(8,(int)(w_standard_20*10)%10,0);
	}
}