#ifndef __uart_H__
#define __uart_H__

void Uart1_Init(void);
void uart_send_byte(unsigned char dat);
void uart_send_string(unsigned char *dat);
#endif