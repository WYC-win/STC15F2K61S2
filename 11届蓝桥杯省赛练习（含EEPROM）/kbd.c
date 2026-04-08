#include <STC15F2K60S2.H>
sbit row4=P3^3;sbit row3=P3^2;sbit row2=P3^1;sbit row1=P3^0;
sbit col4=P3^4;sbit col3=P3^5;sbit col2=P4^2;sbit col1=P4^4;
unsigned char kbd()
{
	unsigned char keynumber=0;
	static unsigned char last_keynumber=0;
	static unsigned char reported=0;
	static unsigned char stable_count=0;
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	row4=0;
	if(col1==0)keynumber=4;
	if(col2==0)keynumber=8;
	if(col3==0)keynumber=12;
	if(col4==0)keynumber=16;
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	row3=0;
	if(col1==0)keynumber=5;
	if(col2==0)keynumber=9;
	if(col3==0)keynumber=13;
	if(col4==0)keynumber=17;
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	row2=0;
	if(col1==0)keynumber=6;
	if(col2==0)keynumber=10;
	if(col3==0)keynumber=14;
	if(col4==0)keynumber=18;
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	row1=0;
	if(col1==0)keynumber=7;
	if(col2==0)keynumber=11;
	if(col3==0)keynumber=15;
	if(col4==0)keynumber=19;
	
	if(last_keynumber==keynumber)
	{
		if(stable_count<3)stable_count++;
	}
	else
	{
		last_keynumber=keynumber;
		stable_count=0;
	}
	
	if(stable_count==3)
	{
		if(keynumber!=0 && reported==0)
		{
			reported=1;
			return keynumber;
		}
		if(keynumber==0)
		{
			reported=0;
			return 0;
		}
	}
	return 0;

}
	