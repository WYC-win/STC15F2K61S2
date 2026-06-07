/*
 *  实验三：IAP15F2K61S2单片机综合实验
 *  显示：LCD1602（替换数码管）
 *  日期：2026.6.6
 */

#include <STC15F2K60S2.H>
#include "LCD1602.h"
#include "latch.h"
#include "kbd.h"
#include "iic.h"
#include "AD_DA.h"

//==================== 状态定义 ====================
#define state_main      0
#define state_led       1
#define state_uart_menu 2
#define state_uart_send 3
#define state_uart_recv 4
#define state_da_menu   5
#define state_da_saw    6
#define state_da_tri    7
#define state_da_sin    8

//==================== 按键映射 ====================
// 矩阵位置→返回值:
//       col1 col2 col3 col4
// row1   7   11   15   19
// row2   6   10   14   18
// row3   5    9   13   17
// row4   4    8   12   16
//
// 功能分配:
// S4(4)=INT0  S5(5)=BACK  S6(6)=0/+
// S8(8)=1     S9(9)=4     S11(11)=7
// S12(12)=2   S13(13)=5   S15(15)=8
// S16(16)=3   S17(17)=6   S19(19)=9
// S7(7)未用  S10(10)未用  S14(14)未用  S18(18)未用
#define KEY_INT0   4       // S4: LED间隔递减
#define KEY_BACK   5       // S5: 返回主菜单
#define KEY_0      6       // S6: 数字0(UART) / 频率+(DA)
#define KEY_DOWN   7       // S7: 频率-
#define KEY_1      8       // S8: 菜单1 / 数字1
#define KEY_4      9       // S9: 数字4
#define KEY_7     11       // S11: 数字7
#define KEY_2     12       // S12: 菜单2 / 数字2
#define KEY_5     13       // S13: 数字5
#define KEY_8     15       // S15: 数字8
#define KEY_3     16       // S16: 菜单3 / 数字3
#define KEY_6     17       // S17: 数字6
#define KEY_9     19       // S19: 数字9
#define KEY_UP     KEY_0    // S6: 频率+（同KEY_0）

//==================== 正弦波64点查找表 ====================
unsigned char code sin_tab[64] = {
    128,140,153,165,177,188,199,209,
    218,226,234,240,245,250,253,254,
    255,254,253,250,245,240,234,226,
    218,209,199,188,177,165,153,140,
    128,116,103, 91, 79, 68, 57, 47,
     38, 30, 22, 16, 11,  6,  3,  2,
      1,  2,  3,  6, 11, 16, 22, 30,
     38, 47, 57, 68, 79, 91,103,116
};

//==================== 全局变量 ====================
unsigned char page_count;            // 系统状态
unsigned int  led_interval;          // LED点亮间隔(ms)
unsigned int  led_tick;              // LED时基计数(ms)
unsigned char led_pattern;           // LED当前亮灯位
unsigned int  da_freq;               // DA输出频率(Hz)
unsigned char da_index;              // DA查表索引
unsigned char da_val;                // DA当前输出值
unsigned char rx_buf[28];            // 串口接收缓冲(27字符+结束符)
unsigned char rx_index;              // 接收缓冲索引
unsigned char refresh_count;         // LCD刷新计数
unsigned int  rx_idle_cnt;           // 串口空闲计数(新数据清屏用)
bit da_flag;                         // DA更新标志（ISR→主循环）

//==================== 函数声明 ====================
void init(void);
void Uart1_Init(void);
void uart_send_byte(unsigned char dat);
void uart_send_string(char *str);
void uart_send_num(unsigned int num);
void Timer0_LED_Init(void);
void Timer0_DA_Init(unsigned int us);
void show_main_menu(void);
void led_mode(void);
void uart_mode(void);
void da_mode(void);

//==================== 系统初始化 ====================
void init(void)
{
    P0 = 0xFF;
    latch(4);       // 熄灭LED
    P0 = 0x00;
    latch(5);       // 关闭蜂鸣器/继电器
}

//==================== UART初始化 ====================
//==================== 串口中断（含接收处理） ====================
void Uart1_Isr(void) interrupt 4
{
    unsigned char ch;

    if(RI)
    {
         RI = 0;
        ch = SBUF;

        if(page_count == state_uart_recv)
        {
            if(ch >= 0x20 && ch < 0x7F)
            {
                // 空闲超时 → 新消息，清旧数据
                if(rx_idle_cnt >= 60000)
                {
                    rx_index = 0;
                    rx_buf[0] = '\0';
                }
                rx_idle_cnt = 0;

                if(rx_index < 27)
                {
                    rx_buf[rx_index++] = ch;
                    rx_buf[rx_index] = '\0';
                }
                else
                {
                    unsigned char j;
                    for(j = 0; j < 26; j++)
                        rx_buf[j] = rx_buf[j + 1];
                    rx_buf[26] = ch;
                    rx_buf[27] = '\0';
                }
            }
        }
    }
    if(TI)
        TI = 0;
}

void Uart1_Init(void)	//9600bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xBF;		//定时器时钟12T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xE6;			//设置定时初始值
	TH1 = 0xFF;			//设置定时初始值
	ET1 = 0;			//禁止定时器中断
	TR1 = 1;			//定时器1开始计时
	ES = 1;				//使能串口1中断
}

//==================== 串口发送 ====================
void uart_send_byte(unsigned char dat)
{
    SBUF = dat;
    while(TI == 0);
    TI = 0;
}

void uart_send_string(char *str)
{
    while(*str != '\0')
        uart_send_byte(*str++);
}

void uart_send_num(unsigned int num)
{
    uart_send_byte(num/1000%10 + '0');
    uart_send_byte(num/100%10 + '0');
    uart_send_byte(num/10%10 + '0');
    uart_send_byte(num%10 + '0');
}

//==================== Timer0初始化(LED模式, 1ms时基) ====================
void Timer0_LED_Init(void)      //1毫秒@12.000MHz
{
    AUXR &= 0x7F;               //定时器时钟12T模式
    TMOD &= 0xF0;               //设置定时器模式
    TMOD |= 0x01;               //定时器0模式1(16位)
    TL0 = 0x18;                 //设置定时初始值 65536-1000=64536
    TH0 = 0xFC;
    TF0 = 0;                    //清除TF0标志
    TR0 = 1;                    //定时器0开始计时
    ET0 = 1;                    //使能定时器0中断
}

//==================== Timer0初始化(DA模式, us级间隔) ====================
void Timer0_DA_Init(unsigned int us)
{
    unsigned int ticks;
    ticks = 65536 - us;

    AUXR &= 0x7F;               //定时器时钟12T模式
    TMOD &= 0xF0;
    TMOD |= 0x01;               //定时器0模式1
    TL0 = ticks & 0xFF;
    TH0 = ticks >> 8;
    TF0 = 0;
    TR0 = 1;
    ET0 = 1;
}

//==================== 显示主菜单 ====================
void show_main_menu(void)
{
    LCD_Init();
    LCD_ShowString(1, 1, "1.LED  2.UART");
    LCD_ShowString(2, 1, "3.DA   Name  ID");
}

//==================== LED流水灯模式 ====================
void led_mode(void)
{
    unsigned char key;

    LCD_Init();
    LCD_ShowString(1, 1, "LED Run");
    LCD_ShowString(2, 1, "T=    ms");

    led_interval = 500;
    led_pattern  = 0xFE;
    led_tick     = 0;
    refresh_count = 0;

    P0 = led_pattern;           // 低电平亮：0xFE→LED1亮，其余灭
    latch(4);
    LCD_ShowNum(2, 3, led_interval, 3);

    Timer0_LED_Init();
    IT0 = 1;                    // INT0下降沿触发
    EX0 = 1;                    // 使能INT0
    EA  = 1;

    // 串口上报初始值
    uart_send_string("LED T=");
    uart_send_num(led_interval);
    uart_send_string("ms\r\n");

    while(1)
    {
        key = key_back();
        if(key == KEY_BACK)
            break;

        // S4按下：LED间隔递减（INT0功能）
        if(key == KEY_INT0)
        {
            if(led_interval > 10)
                led_interval -= 10;
            else
                led_interval = 500;

            uart_send_string("T=");
            uart_send_num(led_interval);
            uart_send_string("ms\r\n");

            LCD_ShowNum(2, 3, led_interval, 3);
            led_tick = 0;
        }

        // 每500ms刷新LCD（refresh_count在ISR中1ms递增）
        if(refresh_count >= 500)
        {
            refresh_count = 0;
            LCD_ShowNum(2, 3, led_interval, 3);
        }
    }

    // 退出清理
    ET0 = 0;
    TR0 = 0;
    EX0 = 0;
    init();
}

//==================== 串口功能模式 ====================
void uart_mode(void)
{
    unsigned char key;
    unsigned char i;

    // 子菜单
    LCD_Init();
    LCD_ShowString(1, 1, "1.Send 2.Recv");
    LCD_ShowString(2, 1, "UART Test");

    while(1)
    {
        key = key_back();
        if(key == KEY_BACK)
        {
            EA = 0;
            ES = 0;
            return;
        }
        if(key == KEY_1)
        {
            page_count = state_uart_send;
            break;
        }
        if(key == KEY_2)
        {
            page_count = state_uart_recv;
            break;
        }
    }

    //===== 发送模式 =====
    if(page_count == state_uart_send)
    {
        LCD_Init();
        LCD_ShowString(1, 1, "Send: press 0-9");
        LCD_ShowString(2, 1, "Key:     ");

        REN = 0;            // 关闭接收，释放P3.1(RXD)避免干扰ROW2按键扫描
        EA = 1;
        ES = 1;

        while(1)
        {
            key = key_back();
            if(key == KEY_BACK)
                break;

            // 数字按键：8→1,12→2,16→3,9→4,13→5,17→6,11→7,15→8,19→9,6→0
            if(key == KEY_1)        uart_send_byte('1');
            else if(key == KEY_2)   uart_send_byte('2');
            else if(key == KEY_3)   uart_send_byte('3');
            else if(key == KEY_4)   uart_send_byte('4');
            else if(key == KEY_5)   uart_send_byte('5');
            else if(key == KEY_6)   uart_send_byte('6');
            else if(key == KEY_7)   uart_send_byte('7');
            else if(key == KEY_8)   uart_send_byte('8');
            else if(key == KEY_9)   uart_send_byte('9');
            else if(key == KEY_0)   uart_send_byte('0');
            else continue;      // 非数字键，不刷新LCD

            LCD_ShowChar(2, 5, '>');
            LCD_ShowNum(2, 7, key, 2);
        }
        REN = 1;            // 恢复接收使能
    }
    //===== 接收模式 =====
    else if(page_count == state_uart_recv)
    {
        unsigned char rx_last = 0;

        rx_idle_cnt = 0;
        rx_index = 0;
        for(i = 0; i < 27; i++)
            rx_buf[i] = '\0';

        LCD_Init();
        LCD_ShowString(1, 1, "Recv:           ");
        LCD_ShowString(2, 1, "                ");

        REN = 1;
        EA = 1;
        ES = 1;

        while(1)
        {
            // 只查S5，不碰P3.1/RXD
            P44 = 0;
            if(P32 == 0)
            {
                P44 = 1;
                break;
            }
            P44 = 1;

            // 空闲计数（ISR收到数据时清零）
            if(rx_idle_cnt < 60000)
                rx_idle_cnt++;

            // 空闲超时 且 有新数据 → 一次性刷新LCD
            if(rx_idle_cnt >= 60000 && rx_index != rx_last)
            {
                rx_last = rx_index;
                ES = 0;
                LCD_ShowString(1, 6, "           ");
                LCD_ShowString(2, 1, "                ");
                for(i = 0; i < rx_index && i < 11; i++)
                    LCD_ShowChar(1, 6 + i, rx_buf[i]);
                if(rx_index > 11)
                {
                    for(i = 11; i < rx_index && i < 27; i++)
                        LCD_ShowChar(2, i - 10, rx_buf[i]);
                }
                ES = 1;
            }
        }
        ES = 0;
    }
}

//==================== DA转换模式 ====================
void da_mode(void)
{
    unsigned char key;

    // 子菜单
    LCD_Init();
    LCD_ShowString(1, 1, "1.Saw 2.Tri 3.Sin");
    LCD_ShowString(2, 1, "Select Wave");

    while(1)
    {
        key = key_back();
        if(key == KEY_BACK)
        {
            ET0 = 0;
            TR0 = 0;
            DA_out(0);
            init();
            return;
        }
        if(key == KEY_1)
        {
            page_count = state_da_saw;
            break;
        }
        if(key == KEY_2)
        {
            page_count = state_da_tri;
            break;
        }
        if(key == KEY_3)
        {
            page_count = state_da_sin;
            break;
        }
    }

    // 初始化DA
    da_freq = 100;
    da_index = 0;
    refresh_count = 0;

    LCD_Init();
    LCD_ShowString(2, 1, "F=    Hz UP/DN");

    if(page_count == state_da_saw)
        LCD_ShowString(1, 1, "Saw Wave");
    else if(page_count == state_da_tri)
        LCD_ShowString(1, 1, "Tri Wave");
    else
        LCD_ShowString(1, 1, "Sin Wave");

    LCD_ShowNum(2, 3, da_freq, 4);

    // 启动DA输出 (f=100Hz, 64点/周期, T_sample=1000000/6400=156us)
    Timer0_DA_Init(156);
    EA = 1;

    while(1)
    {
        // DA更新：ISR设标志，主循环执行I2C输出
        if(da_flag)
        {
            da_flag = 0;
            DA_out(da_val);
        }

        key = key_back();
        if(key == KEY_BACK)
            break;

        // 频率调整
        if(key == KEY_UP)
        {
            ET0 = 0;
            if(da_freq < 1000) da_freq += 10;
            Timer0_DA_Init(1000000UL / (64 * da_freq));
            LCD_ShowNum(2, 3, da_freq, 4);
            ET0 = 1;
        }
        else if(key == KEY_DOWN)
        {
            ET0 = 0;
            if(da_freq > 10) da_freq -= 10;
            Timer0_DA_Init(1000000UL / (64 * da_freq));
            LCD_ShowNum(2, 3, da_freq, 4);
            ET0 = 1;
        }
    }

    // 退出清理
    ET0 = 0;
    TR0 = 0;
    DA_out(0);
    init();
}

//==================== Timer0中断服务 ====================
void Timer0_Isr(void) interrupt 1
{
    unsigned int reload;
    unsigned char val;

    if(page_count == state_led)
    {
        // LED模式：1ms时基
        TL0 = 0x18;
        TH0 = 0xFC;

        led_tick++;
        refresh_count++;            // 1ms计数，用于LCD刷新
        if(led_tick >= led_interval)
        {
            led_tick = 0;
            // 跑马灯移位
            if(led_pattern == 0x7F)
                led_pattern = 0xFE;
            else
                led_pattern = (led_pattern << 1) | 0x01;
            P0 = led_pattern;
            latch(4);
        }
    }
    else if(page_count >= state_da_saw && page_count <= state_da_sin)
    {
        // DA模式：波形输出
        reload = 65536 - (1000000UL / (64 * da_freq));
        TL0 = reload & 0xFF;
        TH0 = reload >> 8;

        da_index++;
        if(da_index >= 64)
            da_index = 0;

        if(page_count == state_da_saw)
            val = da_index * 4;                     // 锯齿波
        else if(page_count == state_da_tri)
        {
            if(da_index < 32)
                val = da_index * 8;                 // 三角波上升
            else
                val = (63 - da_index) * 8;          // 三角波下降
        }
        else
            val = sin_tab[da_index];                // 正弦波查表

        da_val = val;       // 传给主循环输出
        da_flag = 1;
    }
}

//==================== INT0中断(LED间隔递减) ====================
void INT0_Isr(void) interrupt 0
{
    if(page_count == state_led)
    {
        if(led_interval > 10)
            led_interval -= 10;
        else
            led_interval = 500;

        // 串口上报
        uart_send_string("T=");
        uart_send_num(led_interval);
        uart_send_string("ms\r\n");

        // 同步LCD和LED节拍
        LCD_ShowNum(2, 3, led_interval, 3);
        led_tick = 0;
    }
}

//==================== 主函数 ====================
void main()
{
    unsigned char key;

    init();
    Uart1_Init();
    EA = 1;

    page_count = state_main;
    show_main_menu();

    while(1)
    {
        key = key_back();

        if(key == KEY_1)
        {
            page_count = state_led;
            led_mode();
            page_count = state_main;
            show_main_menu();
        }
        else if(key == KEY_2)
        {
            page_count = state_uart_menu;
            uart_mode();
            page_count = state_main;
            show_main_menu();
        }
        else if(key == KEY_3)
        {
            page_count = state_da_menu;
            da_mode();
            page_count = state_main;
            show_main_menu();
        }
    }
}
