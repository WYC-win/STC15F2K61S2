#include <STC15F2K60S2.H>
#include "one_wire.h"
#include "delay.h"
#include "digital_tube.h"
#include "init.h"
unsigned char num;
unsigned long tem;
void main()
{
	init();
	while(1)
	{
		digital_tube(1,num);
//		digital_tube(2,2);
//		digital_tube(3,3);
//		digital_tube(4,4);
//		digital_tube(5,5);
//		digital_tube(6,6);
//		digital_tube(7,7);
//		digital_tube(8,8);
		tem++;
		if(tem>1000)
		{
			num++;
			tem=0;
		}
		if(num>15)
		{num=0;}
	}
}