#include <STC15F2K60S2.H>
#include "onewire.h"

void  Convert_T()
{
	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x44);
}

 int get_temperature()
{
	unsigned int temp;
	unsigned char MSB,LSB;
	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0xBE);
	LSB=Read_DS18B20();
	MSB=Read_DS18B20();
	temp=(((int)MSB<<8)|LSB)/16;
	return temp;
}
