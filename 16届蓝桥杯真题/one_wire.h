#ifndef __one_wire_H__
#define __one_wire_H__

unsigned char onewire_init(void);
void Delay70us(void);
void Delay500us(void);
void Delay5us(void);
void Delay10us(void);
void Delay50us(void);
void onewire_sendbit(unsigned char Bit);
unsigned char onewire_receivebit(void);
void onewire_sendbyte(unsigned char byte);
unsigned char onewire_receivebyte(void);
#endif