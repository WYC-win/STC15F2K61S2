#ifndef __one_wire_H__
#define __one_wire_H__

unsigned char onewire_init(void);
void onewire_sendbit(unsigned char Bit);
unsigned char onewire_receivebit(void);
void onewire_sendbyte(unsigned char byte);
unsigned char onewire_receivebyte(void);
#endif