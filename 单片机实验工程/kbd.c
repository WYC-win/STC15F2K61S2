#include <STC15F2K60S2.H>
sbit row4=P3^3;sbit row3=P3^2;sbit row2=P3^1;sbit row1=P3^0;
sbit col4=P3^4;sbit col3=P3^5;sbit col2=P4^2;sbit col1=P4^4;

unsigned char key_back()
{
	unsigned char key_number=0;
	static bit reported=0;
	static unsigned char last_keynumber=0;
	static unsigned char stable_count=0;
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	col4=0;
	if(row4==0)key_number=16;
	else if(row3==0)key_number=17;
	else if(row2==0)key_number=18;
	else if(row1==0)key_number=19;
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	col3=0;
	if(row4==0)key_number=12;
	else if(row3==0)key_number=13;
	else if(row2==0)key_number=14;
	else if(row1==0)key_number=15;
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	col2=0;
	if(row4==0)key_number=8;
	else if(row3==0)key_number=9;
	else if(row2==0)key_number=10;
	else if(row1==0)key_number=11;
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	col1=0;
	if(row4==0)key_number=4;
	else if(row3==0)key_number=5;
	else if(row2==0)key_number=6;
	else if(row1==0)key_number=7;
	
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