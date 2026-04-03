#include <STC15F2K60S2.H>
sbit row4=P3^3;sbit row3=P3^2;sbit row2=P3^1;sbit row1=P3^0;
sbit col4=P3^4;sbit col3=P3^5;sbit col2=P4^2;sbit col1=P4^4;
unsigned char key_back()
{
	unsigned char key_number=0;
	static unsigned char last_key_number=0;
	static unsigned char stable_count=0;
	static unsigned char reported=0;
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	row4=0;
	if(col4==0)key_number=16;
	else if(col3==0)key_number=12;
	else if(col2==0)key_number=8;
	else if(col1==0)key_number=4;
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	row3=0;
	if(col4==0)key_number=17;
	else if(col3==0)key_number=13;
	else if(col2==0)key_number=9;
	else if(col1==0)key_number=5;
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	row2=0;
	if(col4==0)key_number=18;
	else if(col3==0)key_number=14;
	else if(col2==0)key_number=10;
	else if(col1==0)key_number=6;
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	row1=0;
	if(col4==0)key_number=19;
	else if(col3==0)key_number=15;
	else if(col2==0)key_number=11;
	else if(col1==0)key_number=7;
	
	if(key_number==last_key_number)
	{
		if(stable_count<3)stable_count++;
	}
	else
	{
		last_key_number=key_number;
		stable_count=0;
	}
	if(stable_count==3)
	{
		if(key_number!=0 && reported==0)
		{
			reported=1;
			return key_number;
		}
		if(key_number==0)
		{
			reported=0;
			return 0;
		}
	}
	return 0;
}
unsigned char key_double_back(unsigned char key_id)
{
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	row4=0;
	if(col4==0 && key_id==16)return 1;
	else if(col3==0 && key_id==12)return 1;
	else if(col2==0 && key_id==8)return 1;
	else if(col1==0 && key_id==4)return 1;
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	row3=0;
	if(col4==0 && key_id==17)return 1;
	else if(col3==0 && key_id==13)return 1;
	else if(col2==0 && key_id==9)return 1;
	else if(col1==0 && key_id==5)return 1;
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	row2=0;
	if(col4==0 && key_id==18)return 1;
	else if(col3==0 && key_id==14)return 1;
	else if(col2==0 && key_id==10)return 1;
	else if(col1==0 && key_id==6)return 1;
	
	row4=1;row3=1;row2=1;row1=1;
	col4=1;col3=1;col2=1;col1=1;
	row1=0;
	if(col4==0 && key_id==19)return 1;
	else if(col3==0 && key_id==15)return 1;
	else if(col2==0 && key_id==11)return 1;
	else if(col1==0 && key_id==7)return 1;
	
	return 0;
}