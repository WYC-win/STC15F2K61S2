#include <STC15F2K60S2.H>
#include "onewire.h"
#include "iic.h"
#include "ds1302.h"
#include "74hc38.h"
#include "init.h"

void Delay(unsigned int n)	//@12.000MHz
{
	unsigned int i;
	for(i=0;i<n;i++)
	{
		unsigned char data i, j;

		i = 12;
		j = 169;
		do
		{
		while (--j);
		} while (--i);
	}
	
}

void main()
{
	init();
	while(1)
	{
	}
}
