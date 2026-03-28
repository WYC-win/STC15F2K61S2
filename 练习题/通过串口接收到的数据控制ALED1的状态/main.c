#include <STC15F2K60S2.H>

void Delay100ms(void)	//@12.000MHz
{
	unsigned char data i, j, k;

	_nop_();
	_nop_();
	i = 5;
	j = 144;
	k = 71;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

unsigned long D;
void UART_SendByte(unsigned char Byte)
{
	SBUF=Byte;
	while(TI==0);
	//Delay(1)
	TI=0;
}

void Uart1_Isr(void) interrupt 4
{
	if (RI==1)				//检测串口1接收中断
	{
//		P0=~SBUF;
//		UART_SendByte(SBUF);
//		D=strtol(SBUF,NULL,16);
		RI = 0;
		
		if (SBUF & 0x01) 
		{
			while(1)
			{	
				P0=0xFE;
				Delay100ms();
				P0=0xFF;
				Delay100ms();
				if(RI==1)break;
			}
				
		}
		else
		{
			while(1)
			{
				P0=0xFF;
				if(RI==1)break;
			}
		}
	}	
}



void Uart1_Init(void)	//9600bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xC7;			//设置定时初始值
	T2H = 0xFE;			//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
	ES = 1;				//使能串口1中断
	EA=1;
}

void main()
{
	Uart1_Init();
	WR=0;
	P25=0;
	P26=0;
	P27=1;
	while(1)
	{
	
	}
}