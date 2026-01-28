#include <STC15F2K60S2.H>
#include "delay.h"


unsigned char digitaltube_number[16]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E};

void digital_tube(unsigned char digitaltube_position, unsigned char number)
{
    // -------- 位选：写入到 Y6C（数码管位选锁存）--------
    switch(digitaltube_position)
    {
        case 1:P0=0x01;break;
        case 2:P0=0x02;break;
        case 3:P0=0x04;break;
        case 4:P0=0x08;break;
        case 5:P0=0x10;break;
        case 6:P0=0x20;break;
        case 7:P0=0x40;break;
        case 8:P0=0x80;break;
		case 9:P0=0x90;break;
		case 10:P0=0x88;break;
		case 11:P0=0x83;break;
		case 12:P0=0xC6;break;
		case 13:P0=0xA1;break;
		case 14:P0=0x86;break;
		case 15:P0=0x8E;break;
        default:P0=0xFF;break;
    }
    P2 = (P2 & 0x1F) | 0xC0;   // 选通 Y6C
    P2 &= 0x1F;                // 关闭选通

    // -------- 段码：写入到 Y7C（数码管段选锁存）--------
    P0 = digitaltube_number[number & 0x0F];
    P2 = (P2 & 0x1F) | 0xE0;   // 选通 Y7C
    P2 &= 0x1F;                // 关闭选通

    delay(2);

/* -------- 消影（补的只有这里）-------- */
	P0 = 0xFF;                     // 段码全灭
	P2 = (P2 & 0x1F) | 0xE0;       // Y7C 段选锁存
	P2 &= 0x1F;                    // 关闭选通

}
