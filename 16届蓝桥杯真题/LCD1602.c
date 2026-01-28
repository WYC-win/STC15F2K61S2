#include <STC15F2K60S2.H>
#include <intrins.h>

/* ========== CT107D 引脚映射（按原理图） ========== */
sbit LCD_RS  = P2^0;
sbit LCD_RW  = P2^1;
sbit LCD_EN  = P1^2;

sbit CT_A0   = P2^5;
sbit CT_A1   = P2^6;
sbit CT_A2   = P2^7;

sbit CT_WR   = P4^2;   // MCU_WR（/WR）

/* ========== 内部：往U7(段码锁存=Y7C)写 8 位数据，输出到 DD0~DD7 ========== */
static void CT107D_WriteU7(unsigned char dat)
{
    P0 = dat;                 // 数据先上 P0（送到 U7 的 D0~D7）

    /* 选通 Y7C：A2A1A0 = 111 */
    CT_A2 = 1; CT_A1 = 1; CT_A0 = 1;

    /* 产生 /WR 脉冲，让“Y7C门控”真正把数据锁存进去 */
    CT_WR = 0;
    _nop_(); _nop_(); _nop_();
    CT_WR = 1;

    /* 关闭选通，避免影响其它锁存 */
    CT_A2 = 0; CT_A1 = 0; CT_A0 = 0;
}

/* ========== A2风格的1ms级延时（12MHz下大致可用） ========== */
void LCD_Delay()
{
    unsigned char i, j;
    i = 2;
    j = 239;
    do { while (--j); } while (--i);
}

/* ========== 写命令：RS=0 ========== */
void LCD_WriteCommand(unsigned char Command)
{
    LCD_RS = 0;
    LCD_RW = 0;

    CT107D_WriteU7(Command);   // ★与A2唯一关键差别：数据不是直接P0，而是锁存到DD总线

    LCD_EN = 1;
    LCD_Delay();
    LCD_EN = 0;
    LCD_Delay();
}

/* ========== 写数据：RS=1 ========== */
void LCD_WriteData(unsigned char Data)
{
    LCD_RS = 1;
    LCD_RW = 0;

    CT107D_WriteU7(Data);

    LCD_EN = 1;
    LCD_Delay();
    LCD_EN = 0;
    LCD_Delay();
}

void LCD_SetCursor(unsigned char Line,unsigned char Column)
{
    if(Line==1)
    {
        LCD_WriteCommand(0x80|(Column-1));
    }
    else if(Line==2)
    {
        LCD_WriteCommand(0x80|(Column-1+0x40));
    }
}

/* ========== 初始化（A2同款） ========== */
void LCD_Init()
{
    /* 建议上电先保证 /WR 为高（空闲态） */
    CT_WR = 1;

    /* 1602 上电需要一点等待 */
    LCD_Delay(); LCD_Delay(); LCD_Delay(); LCD_Delay();

    LCD_WriteCommand(0x38); // 8位总线，两行，5*7
    LCD_WriteCommand(0x0c); // 显示开，光标关，闪烁关
    LCD_WriteCommand(0x06); // 光标右移，画面不动
    LCD_WriteCommand(0x01); // 清屏
}

/* ========== 显示函数（完全照A2写法） ========== */
void LCD_ShowChar(unsigned char Line,unsigned char Column,char Char)
{
    LCD_SetCursor(Line,Column);
    LCD_WriteData(Char);
}

void LCD_ShowString(unsigned char Line,unsigned char Column,char *String)
{
    unsigned char i;
    LCD_SetCursor(Line,Column);
    for(i=0;String[i]!='\0';i++)
    {
        LCD_WriteData(String[i]);
    }
}

int LCD_Pow(int X,int Y)
{
    unsigned char i;
    int Result=1;
    for(i=0;i<Y;i++)
    {
        Result*=X;
    }
    return Result;
}

void LCD_ShowNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length)
{
    unsigned char i;
    LCD_SetCursor(Line,Column);
    for(i=Length;i>0;i--)
    {
        LCD_WriteData(Number/LCD_Pow(10,i-1)%10+'0');
    }
}

void LCD_ShowSignedNum(unsigned char Line,unsigned char Column,int Number,unsigned char Length)
{
    unsigned char i;
    unsigned int Number1;
    LCD_SetCursor(Line,Column);
    if(Number>=0)
    {
        LCD_WriteData('+');
        Number1=Number;
    }
    else
    {
        LCD_WriteData('-');
        Number1=-Number;
    }
    for(i=Length;i>0;i--)
    {
        LCD_WriteData(Number1/LCD_Pow(10,i-1)%10+'0');
    }
}

void LCD_ShowHexNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length)
{
    unsigned char i,SingleNumber;
    LCD_SetCursor(Line,Column);
    for(i=Length;i>0;i--)
    {
        SingleNumber=Number/LCD_Pow(16,i-1)%16;
        if(SingleNumber<10)
        {
            LCD_WriteData(SingleNumber+'0');
        }
        else
        {
            LCD_WriteData(SingleNumber-10+'A');
        }
    }
}

void LCD_ShowBinNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length)
{
    unsigned char i;
    LCD_SetCursor(Line,Column);
    for(i=Length;i>0;i--)
    {
        LCD_WriteData(Number/LCD_Pow(2,i-1)%2+'0');
    }
}
