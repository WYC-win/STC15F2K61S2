
// Header:STC15初始化驱动程序
// File Name: 
// Author:WYC
// Date:2026 1.27

#include <STC15F2K60S2.H>
void init(void)
{
// 关外设（按书里的 Cls_Peripheral 思路写）
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0x80;   // Y4C（LED锁存）
    P2 &= 0x1F;

    P0 = 0x00;
    P2 = (P2 & 0x1F) | 0xA0;   // Y5C（驱动器锁存）
    P2 &= 0x1F;
}