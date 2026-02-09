
// Header:DS18B20温度传感器驱动
// File Name: 
// Author:WYC
// Date:2026 1.28

#include <STC15F2K60S2.H>
#include "onewire.h"

#define DS18B20_skiprom 0xCC
#define DS18B20_convert_T 0x44
#define DS18B20_read_scratchpad 0xBE
void DS18B20_convertT(void)
{
	init_ds18b20();
	Write_DS18B20(DS18B20_skiprom);
	Write_DS18B20(DS18B20_convert_T);
}

float DS18B20_readT(void)
{
	unsigned char TLSB,TMSB;
	int temp;
	float T;
	init_ds18b20();
	Write_DS18B20(DS18B20_skiprom);
	Write_DS18B20(DS18B20_read_scratchpad);
	TLSB=Read_DS18B20();
	TMSB=Read_DS18B20();
	temp=(TMSB<<8)|TLSB;
	T=temp/16.0;
	return T;
}