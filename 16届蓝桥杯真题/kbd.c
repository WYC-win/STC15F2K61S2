
// Header:矩阵键盘模块驱动程序
// File Name: 
// Author:WYC
// Date:2026 1.29


#include <STC15F2K60S2.H>
#include "delay.h"
unsigned char keynumber(void)
{
	unsigned char keynum=0;
	P44=1;P42=1;P35=1;P34=1;//col 1-4
	P30=1;P31=1;P32=1;P33=1;//row 1-4
	
	P44=0;
	if(P30==0){delay(20);while(P30==0);delay(20);keynum=7;}
	if(P31==0){delay(20);while(P31==0);delay(20);keynum=6;}
	if(P32==0){delay(20);while(P32==0);delay(20);keynum=5;}
	if(P33==0){delay(20);while(P33==0);delay(20);keynum=4;}
	
	P44=1;P42=1;P35=1;P34=1;//col 1-4
	P30=1;P31=1;P32=1;P33=1;//row 1-4
	P42=0;
	if(P30==0){delay(20);while(P30==0);delay(20);keynum=11;}
	if(P31==0){delay(20);while(P31==0);delay(20);keynum=10;}
	if(P32==0){delay(20);while(P32==0);delay(20);keynum=9;}
	if(P33==0){delay(20);while(P33==0);delay(20);keynum=8;}
	
	P44=1;P42=1;P35=1;P34=1;//col 1-4
	P30=1;P31=1;P32=1;P33=1;//row 1-4
	P35=0;
	if(P30==0){delay(20);while(P30==0);delay(20);keynum=15;}
	if(P31==0){delay(20);while(P31==0);delay(20);keynum=14;}
	if(P32==0){delay(20);while(P32==0);delay(20);keynum=13;}
	if(P33==0){delay(20);while(P33==0);delay(20);keynum=12;}
	
	P44=1;P42=1;P35=1;P34=1;//col 1-4
	P30=1;P31=1;P32=1;P33=1;//row 1-4
	P34=0;
	if(P30==0){delay(20);while(P30==0);delay(20);keynum=19;}
	if(P31==0){delay(20);while(P31==0);delay(20);keynum=18;}
	if(P32==0){delay(20);while(P32==0);delay(20);keynum=17;}
	if(P33==0){delay(20);while(P33==0);delay(20);keynum=16;}
	
	return keynum;
}