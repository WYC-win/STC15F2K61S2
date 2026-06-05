#include <STC15F2K60S2.H>
#include <stdio.h>
#include "latch.h"
#include "init.h"
#include "digitaltube.h"
#include "kbd.h"
#include "delay.h"
#include "ds18b20.h"
#include "ds1302.h"
#include "superwave.h"
#include "uart.h"
unsigned char uart_receive[20];
unsigned char uart_receive_index=0;
unsigned char t=5;
unsigned char uart_send[20];

void Uart1_Isr(void) interrupt 4
{
	if (RI==1)				//检测串口1接收中断
	{
		uart_receive[uart_receive_index]=SBUF;
//		uart_receive_index++;
		RI=0;
	}
}

void main()
{
	
	init();
	Uart1_Init();
	uart_receive[0]=0;
	while(1)
	{
		digitaltube_fixed(1,uart_receive[0]);
//		sprintf(uart_send,"T = %d",(unsigned int)t);
//		uart_send_string(uart_send);
//		delay(1000);
	}
}