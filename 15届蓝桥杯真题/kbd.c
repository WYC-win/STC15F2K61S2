#include <STC15F2K60S2.H>
sbit row1=P3^0;
sbit row2=P3^1;
sbit row3=P3^2;
sbit row4=P3^3;
sbit col1=P4^4;
sbit col2=P4^2;
sbit col3=P3^5;
sbit col4=P3^4;

unsigned char kbd_number()
{
	unsigned char key_number=0;
	
	row1=1;row2=1;row3=1;row4=1;
	col1=1;col2=1;col3=1;col4=1;
	col1=0;
	if(row1==0)key_number=7;
	else if(row2==0)key_number=6;
	else if(row3==0)key_number=5;
	else if(row4==0)key_number=4;
	
	
	row1=1;row2=1;row3=1;row4=1;
	col1=1;col2=1;col3=1;col4=1;
	col2=0;
	if(row1==0)key_number=11;
	else if(row2==0)key_number=10;
	else if(row3==0)key_number=9;
	else if(row4==0)key_number=8;
	
	row1=1;row2=1;row3=1;row4=1;
	col1=1;col2=1;col3=1;col4=1;
	col3=0;
	if(row1==0)key_number=15;
	else if(row2==0)key_number=14;
	else if(row3==0)key_number=13;
	else if(row4==0)key_number=12;
	
	
	row1=1;row2=1;row3=1;row4=1;
	col1=1;col2=1;col3=1;col4=1;
	col4=0;
	if(row1==0)key_number=19;
	else if(row2==0)key_number=18;
	else if(row3==0)key_number=17;
	else if(row4==0)key_number=16;
	
	return key_number;
}