#ifndef _ds1302_H_
#define _ds1302_H_
void Write_Ds1302(unsigned  char temp) ;
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )    ;
unsigned char Read_Ds1302_Byte ( unsigned char address );
void time_init(unsigned char num,unsigned char *dat);
extern unsigned char time_init_fist[3];
#endif