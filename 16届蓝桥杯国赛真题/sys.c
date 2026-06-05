#include <STC15F2K60S2.H>

sbit row1=P3^0;sbit row2=P3^1;sbit row3=P3^2;sbit row4=P3^3;
sbit col1=P4^4;sbit col2=P4^2;sbit col3=P3^5;
sbit RX=P1^1;sbit TX=P1^0;

code unsigned char digitaltube_seg[16]={0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E};
void latch(unsigned char HC38);
void delay(unsigned int xms);
void init();


void latch(unsigned char HC38)
{
	switch(HC38)
	{
		case 4:P2=(P2&0X1F)|0x80;break;
		case 5:P2=(P2&0X1F)|0xA0;break;
		case 6:P2=(P2&0X1F)|0xC0;break;
		case 7:P2=(P2&0X1F)|0xE0;break;
	}
	P2&=0x1F;
}

void delay(unsigned int xms)	//@12.000MHz
{
	unsigned char data i, j;
	while(xms--)
	{
		i = 12;
		j = 169;
		do
		{
			while (--j);
		} while (--i);
	}
	
}

void init()
{
	P0=0xFF;
	latch(4);
	
	P0=0x00;
	latch(5);
}

void digitaltube_fix(unsigned char tube_position,unsigned char tube_number,bit dp)
{
	switch(tube_position)
	{
		case 1:P0=0x01;break;
		case 2:P0=0x02;break;
		case 3:P0=0x04;break;
		case 4:P0=0x08;break;
		case 5:P0=0x10;break;
		case 6:P0=0x20;break;
		case 7:P0=0x40;break;
		case 8:P0=0x80;break;
		default:P0=0x00;break;
	}
	latch(6);
	if(dp)
	{
		P0=digitaltube_seg[tube_number]&0x7F;
	}
	else
	{
		P0=digitaltube_seg[tube_number];
	}
	latch(7);
	
	delay(2);
	
	P0=0xFF;
	latch(7);
}

void digitaltube(unsigned char tube_position,unsigned char tube_number,bit dp)
{
	switch(tube_position)
	{
		case 1:P0=0x01;break;
		case 2:P0=0x02;break;
		case 3:P0=0x04;break;
		case 4:P0=0x08;break;
		case 5:P0=0x10;break;
		case 6:P0=0x20;break;
		case 7:P0=0x40;break;
		case 8:P0=0x80;break;
		default:P0=0x00;break;
	}
	latch(6);
	if(dp)
	{
		P0=tube_number&0x7F;
	}
	else
	{
		P0=tube_number;
	}
	latch(7);
	
	delay(2);
	
	P0=0xFF;
	latch(7);
}

unsigned char key_back()
{
	unsigned char key_number=0;
	static unsigned char last_keynumber=0;
	static unsigned char stable_count=0;
	static bit reported=0;
	row1=1;row2=1;row3=1;row4=1;
	col1=1;col2=1;col3=1;
	row1=0;
	if(col1==0)key_number=7;
	else if(col2==0)key_number=11;
	else if(col3==0)key_number=15;
	
	row1=1;row2=1;row3=1;row4=1;
	col1=1;col2=1;col3=1;
	row2=0;
	if(col1==0)key_number=6;
	else if(col2==0)key_number=10;
	else if(col3==0)key_number=14;
	
	row1=1;row2=1;row3=1;row4=1;
	col1=1;col2=1;col3=1;
	row3=0;
	if(col1==0)key_number=5;
	else if(col2==0)key_number=9;
	else if(col3==0)key_number=13;
	
	row1=1;row2=1;row3=1;row4=1;
	col1=1;col2=1;col3=1;
	row4=0;
	if(col1==0)key_number=4;
	else if(col2==0)key_number=8;
	else if(col3==0)key_number=12;
	
	if(key_number==last_keynumber)
	{
		if(stable_count<3)
			{
				stable_count++;
			}
	}
	else
	{
		stable_count=0;
		last_keynumber=key_number;
	}
	
	if(stable_count==3)
	{
		if(reported==0 && key_number!=0)
		{
			reported=1;
			return key_number;
		}
		if(key_number==0)
		{
			reported=0;
		}
	}
	return 0;
}

void Delay12us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}


float get_distance()
{
	unsigned int time;
	unsigned i;
	CMOD=0x01;
	CCON=0x00;
	CH=0x00;CL=0x00;
	CR=1;
	for(i=0;i<5;i++)
	{
		TX=1;
		Delay12us();
		TX=0;
		Delay12us();
	}
	
	while(RX==1 && CH<0x40);
	CR=0;
	if(CH>=0x40)return 0;
	else
	{
		time=((int)(CH<<8)|CL);
		return time*0.017;
	}
}

void uart_send_byte(unsigned char dat)
{
	SBUF=dat;
	while(TI==0);
	TI=0;
}

void uart_send_string(unsigned char *dat)
{
	while(*dat!='\0')
		uart_send_byte(*dat++);
}