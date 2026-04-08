#ifndef _at24c02_H_
#define _at24c02_H_
void at24c02_write(unsigned char *dat,unsigned char addr,unsigned char num);
void at24c02_read(unsigned char *dat,unsigned char addr,unsigned char num);
#endif