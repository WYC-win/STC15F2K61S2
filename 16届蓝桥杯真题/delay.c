#include <STC15F2K60S2.H>
void delay(unsigned int xms)	//@12.000MHz
{
	unsigned char data i, j;
	while(xms--)
	{
		i = 12;
		j = 169;
		do
		{
			while (--j);
		} 
		while (--i);
	}
	
}
