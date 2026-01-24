#include <STC15F2K60S2.H>

void main()
{
	WR=0;
	P25=0;
	P26=0;
	P27=1;
	while(1)
	{
		P0=0x0F;
	}
}