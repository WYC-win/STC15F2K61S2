#include <STC15F2K60S2.H>
#include "onewire.h"
#define skip_rom 0xCC
#define convert_T 0x44
#define read_T 0xBE
void ds18b20_convertT()
{
	init_ds18b20();
	Write_DS18B20(skip_rom);
	Write_DS18B20(convert_T);
	
}
 int ds18b20_readT()
{
	unsigned char HTSB,LTSB;
	int ds18b20_tem,Temp;
	init_ds18b20();
	Write_DS18B20(skip_rom);
	Write_DS18B20(read_T);
	LTSB=Read_DS18B20();
	HTSB=Read_DS18B20();
	ds18b20_tem=(HTSB<<8)|LTSB;
	Temp=ds18b20_tem/16;
	return Temp;
}
