
#ifndef _at24c02_H_
#define _at24c02_H_
void at24c02_read(unsigned char addr,unsigned char num,unsigned char *dat);
void at24c02_write(unsigned char addr,unsigned char num,unsigned char *dat);
#endif