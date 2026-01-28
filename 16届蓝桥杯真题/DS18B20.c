#include <STC15F2K60S2.H>
#include "one_wire.h"

#define DS18B20_skiprom 0xCC
#define DS18B20_convert_T 0x44
#define DS18B20_read_scratchpad 0xBE
void DS18B20_convertT(void)
{
	onewire_init();
	onewire_sendbyte(DS18B20_skiprom);
	onewire_sendbyte(DS18B20_convert_T);
}

float DS18B20_readT(void)
{
	unsigned char TLSB,TMSB;
	int temp;
	float T;
	onewire_init();
	onewire_sendbyte(DS18B20_skiprom);
	onewire_sendbyte(DS18B20_read_scratchpad);
	TLSB=onewire_receivebyte();
	TMSB=onewire_receivebyte();
	temp=(TMSB<<8)|TLSB;
	T=temp/16.0;
	return T;
}