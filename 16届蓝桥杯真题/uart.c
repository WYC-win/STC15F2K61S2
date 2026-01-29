
// Header:STC15串口驱动程序
// File Name: 
// Author:WYC
// Date:2026 1.27

#include <STC15F2K60S2.H>

void UART_SendByte(unsigned char Byte)
{
	SBUF=Byte;
	while(TI==0);
	TI=0;
}

//void Uart1_Isr(void) interrupt 4
//{
//	if (TI)				//检测串口1发送中断
//	{
//		TI = 0;			//清除串口1发送中断请求位
//	}
//	if (RI)				//检测串口1接收中断
//	{
//		UART_SendByte(SBUF);
//		RI = 0;			//清除串口1接收中断请求位
//	}
//}

void Uart1_Init(void)	//4800bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x8F;			//设置定时初始值
	TH1 = 0xFD;			//设置定时初始值
	ET1 = 0;			//禁止定时器中断
	TR1 = 1;			//定时器1开始计时
	ES = 1;				//使能串口1中断
	EA=1;
}
