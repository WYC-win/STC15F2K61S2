#ifndef _sys_H_
#define _sys_H_
void latch(unsigned char HC38);
void delay(unsigned int xms);
void init();
void digitaltube_fix(unsigned char tube_position,unsigned char tube_number,bit dp);
void digitaltube(unsigned char tube_position,unsigned char tube_number,bit dp);
unsigned char key_back();
float get_distance();
#endif