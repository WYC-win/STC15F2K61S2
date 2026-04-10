#include <STC15F2K60S2.H>
#include "onewire.h"

void  Convert_T()
{
	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x44);
}

 float get_temperature()
{
	 int temp;
	unsigned char MSB,LSB;
	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0xBE);
	LSB=Read_DS18B20();
	MSB=Read_DS18B20();
	temp=(((int)MSB<<8)|LSB);
	if((temp&0x8000)==0x0000)return temp*0.0625;
	else return (temp-65536)*0.0625;
}
