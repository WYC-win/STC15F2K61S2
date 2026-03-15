#include <STC15F2K60S2.H>
#include "delay.h"
#include "digital_tube.h"
#include "init.h"
#include "DS18B20.h"
#include "AD_DA.h"
#include "superwave.h"
#include "kbd_nb.h"


/* ====================== 锁存选择 ====================== */
#define LATCH_Y4C 0x80   /* LED */
#define LATCH_Y5C 0xA0   /* 继电器/蜂鸣器/电机/步进/喇叭 */

/* ====================== Y4C: LED 位定义 ====================== */
/* 原理图 U6: D0~D7 -> Q0~Q7 -> L1~L8，低电平点亮 */
#define LED1 0x01
#define LED2 0x02
#define LED3 0x04
#define LED4 0x08
#define LED5 0x10
#define LED6 0x20
#define LED7 0x40
#define LED8 0x80

/* ====================== Y5C: 执行器位定义 ====================== */
/* 原理图 U9 -> ULN2003A:
   D0->N_SPK, D1->STEPA, D2->STEPB, D3->STEPC,
   D4->STEPD, D5->N_RELAY, D6->N_MOTOR, D7->N_BUZZ */
#define SPK_BIT    0x01
#define STEPA_BIT  0x02
#define STEPB_BIT  0x04
#define STEPC_BIT  0x08
#define STEPD_BIT  0x10
#define RELAY_BIT  0x20
#define MOTOR_BIT  0x40
#define BUZZ_BIT   0x80

/* ====================== 按键值定义 ====================== */
/* 继承你当前矩阵键盘映射 */
#define KEY_S4 4
#define KEY_S5 5
#define KEY_S8 8
#define KEY_S9 9

/* ====================== 页面定义 ====================== */
#define PAGE_ENV   0
#define PAGE_MOVE  1
#define PAGE_PARA  2
#define PAGE_STAT  3

#define PARA_PC    0
#define PARA_PL    1

#define MOVE_STOP  1
#define MOVE_WALK  2
#define MOVE_RUN   3

/* ====================== 全局变量 ====================== */
unsigned char page = PAGE_ENV;
unsigned char para_page = PARA_PC;

unsigned int  tem = 30;             /* 当前温度 */
unsigned int  distance = 0;         /* 当前距离 */
unsigned int  move_ref_distance = 0;/* 运动状态比较基准距离(秒级) */

unsigned char light_adc = 0;        /* 光敏ADC原始值 */
unsigned char light_level = 1;      /* 光强等级 1~4 */

unsigned char tem_para = 30;        /* 温度参数 PC */
unsigned char dis_para = 30;        /* 距离参数 PL */

unsigned char move_state = MOVE_STOP;
unsigned int  relay_count = 0;      /* 继电器吸合次数 */

bit near_flag = 0;                  /* 接近 */
bit hot_flag  = 0;                  /* 高温 */

bit output_lock = 0;                /* 参数界面锁输出 */
unsigned char led_keep = 0x00;
unsigned char y5_keep  = 0x00;
unsigned char lock_led_base = 0x00;
unsigned char lock_move_state = MOVE_STOP;

bit relay_last = 0;

volatile unsigned int ms = 0;
unsigned int move_lock_ms = 0;
unsigned int blink_ms = 0;
bit blink_flag = 0;

unsigned int combo_hold_ms = 0;

/* ====================== 锁存输出函数 ====================== */
void latch_write(unsigned char dat, unsigned char sel)
{
    P0 = dat;
    P2 = (P2 & 0x1F) | sel;
    P2 &= 0x1F;       //取消选通
}

/* LED 低电平点亮 */
void led_write(unsigned char led_on)
{
    latch_write(~led_on, LATCH_Y4C);
}

/* Y5C 高电平有效 */
void y5_write(unsigned char y5_on)
{
    latch_write(y5_on, LATCH_Y5C);
}

/* ====================== 定时器0 1ms ====================== */
void Timer0_Init(void)      /* 1ms @ 12.000MHz, 12T */
{
    AUXR &= 0x7F;
    TMOD &= 0xF0;
    TMOD |= 0x01;
    TL0 = 0x18;
    TH0 = 0xFC;
    TF0 = 0;
    ET0 = 1;
    EA  = 1;
    TR0 = 1;
}

void Timer0_Isr(void) interrupt 1
{
    TL0 = 0x18;
    TH0 = 0xFC;
    ms++;

    if(move_lock_ms > 0) move_lock_ms--;
    if(blink_ms > 0) blink_ms--;
}

/* ====================== 界面显示 ====================== */
void environment_display(void)
{
    digital_tubefixed(1, 12);              /* C */
    digital_tubefixed(2, tem % 100 / 10);
    digital_tubefixed(3, tem % 10);
    digital_tube(7, 0xC8);                 /* n */
    digital_tubefixed(8, light_level);
}

void move_display(void)
{
    digital_tube(1, 0xC7);                 /* L */
    digital_tubefixed(2, move_state);
    digital_tubefixed(6, distance / 100);
    digital_tubefixed(7, distance % 100 / 10);
    digital_tubefixed(8, distance % 10);
}

void parameter_display(void)
{
    digital_tube(1, 0x8C);                 /* P */

    if(para_page == PARA_PC)
    {
        digital_tubefixed(2, 12);          /* C */
        digital_tubefixed(7, tem_para / 10);
        digital_tubefixed(8, tem_para % 10);
    }
    else
    {
        digital_tube(2, 0xC7);             /* L */
        digital_tubefixed(7, dis_para / 10);
        digital_tubefixed(8, dis_para % 10);
    }
}

void stat_display(void)
{
    unsigned int n;

    digital_tube(1, 0xC8);                 /* n */
    digital_tubefixed(2, 12);              /* C */

    n = relay_count;
    if(n >= 1000) digital_tubefixed(5, n / 1000 % 10);
    else digital_tube(5, 0xFF);

    if(n >= 100) digital_tubefixed(6, n / 100 % 10);
    else digital_tube(6, 0xFF);

    if(n >= 10) digital_tubefixed(7, n / 10 % 10);
    else digital_tube(7, 0xFF);

    digital_tubefixed(8, n % 10);
}

/* ====================== 数据采集 ====================== */
void read_temp_data(void)
{
    tem = (unsigned int)(DS18B20_readT() + 0.5);/* 先读上一次转换结果 */
    DS18B20_convertT();                         /* 再启动下一次转换 */
}

void read_light_data(void)
{
    /* AIN1 */
    light_adc = AD_read(0x41);

    if(light_adc >= 153) light_level = 1;
    else if(light_adc >= 102) light_level = 2;
    else if(light_adc >= 25) light_level = 3;
    else light_level = 4;
}

void read_distance_data(void)
{
    distance = superwave_getdistance();
}

/* ====================== 状态判断 ====================== */
void judge_state(void)
{
    if((distance > 0) && (distance < dis_para)) near_flag = 1;
    else near_flag = 0;

    if(tem > tem_para) hot_flag = 1;
    else hot_flag = 0;
}

void update_move_state(void)
{
    unsigned int delta;
    unsigned char new_state;

    if(distance >= move_ref_distance) delta = distance - move_ref_distance;
    else delta = move_ref_distance - distance;

    if(move_lock_ms != 0)
    {
        move_ref_distance = distance;
        return;
    }

    if(delta < 5) new_state = MOVE_STOP;
    else if(delta < 10) new_state = MOVE_WALK;
    else new_state = MOVE_RUN;

    if(new_state != move_state)
    {
        move_state = new_state;
        move_lock_ms = 4000;    /* 官方逻辑是状态变化后约4秒锁定 */
    }

    move_ref_distance = distance;
}

/* ====================== 输出控制 ====================== */
void output_update(void)
{
    unsigned char led_on = 0x00;
    unsigned char y5_on  = 0x00;
    bit relay_now;

    if(output_lock)
    {
        led_on = lock_led_base;

        if(lock_move_state == MOVE_WALK)
        {
            led_on |= LED8;
        }
        else if(lock_move_state == MOVE_RUN)
        {
            if(blink_ms == 0)
            {
                blink_ms = 100;
                blink_flag = !blink_flag;
            }
            if(blink_flag) led_on |= LED8;
        }

        led_write(led_on);
        y5_write(y5_keep);
        return;
    }

    /* 接近时，L1~L4 显示光强等级 */
    if(near_flag)
    {
        if(light_level == 1) led_on |= LED1;
        else if(light_level == 2) led_on |= (LED1 | LED2);
        else if(light_level == 3) led_on |= (LED1 | LED2 | LED3);
        else led_on |= (LED1 | LED2 | LED3 | LED4);
    }

    /* L8 显示运动状态 */
    if(move_state == MOVE_WALK)
    {
        led_on |= LED8;
    }
    else if(move_state == MOVE_RUN)
    {
        if(blink_ms == 0)
        {
            blink_ms = 100;       /* 100ms 翻转一次 */
            blink_flag = !blink_flag;
        }
        if(blink_flag) led_on |= LED8;
    }

    /* 接近 + 高温 -> 吸合继电器 */
    if(near_flag && hot_flag)
    {
        y5_on |= RELAY_BIT;
    }

    /* 继电器吸合次数统计：上升沿计数 */
    relay_now = (y5_on & RELAY_BIT) ? 1 : 0;  //(y5_on & RELAY_BIT)为真则为1，否则为0
    if(relay_now && !relay_last)
    {
        relay_count++;
    }
    relay_last = relay_now;

    led_write(led_on);
    y5_write(y5_on);

    led_keep = led_on;
    y5_keep  = y5_on;
}

/* ====================== 组合键原始电平读取 ====================== */
/* 用于统计界面 S8+S9 长按2秒清零 */
bit key_down_raw(unsigned char key_id)
{
    P44=1;P42=1;P35=1;P34=1;
    P30=1;P31=1;P32=1;P33=1;
    P44=0;
    if((P30==0) && (key_id==7)) return 1;
    if((P31==0) && (key_id==6)) return 1;
    if((P32==0) && (key_id==5)) return 1;
    if((P33==0) && (key_id==4)) return 1;

    P44=1;P42=1;P35=1;P34=1;
    P30=1;P31=1;P32=1;P33=1;
    P42=0;
    if((P30==0) && (key_id==11)) return 1;
    if((P31==0) && (key_id==10)) return 1;
    if((P32==0) && (key_id==9)) return 1;
    if((P33==0) && (key_id==8)) return 1;

    P44=1;P42=1;P35=1;P34=1;
    P30=1;P31=1;P32=1;P33=1;
    P35=0;
    if((P30==0) && (key_id==15)) return 1;
    if((P31==0) && (key_id==14)) return 1;
    if((P32==0) && (key_id==13)) return 1;
    if((P33==0) && (key_id==12)) return 1;

    P44=1;P42=1;P35=1;P34=1;
    P30=1;P31=1;P32=1;P33=1;
    P34=0;
    if((P30==0) && (key_id==19)) return 1;
    if((P31==0) && (key_id==18)) return 1;
    if((P32==0) && (key_id==17)) return 1;
    if((P33==0) && (key_id==16)) return 1;

    return 0;
}

/* ====================== 按键处理 ====================== */
void key_proc(void)
{
    unsigned char key;

    key = keynumber_nb();
    if(key == 0) return;

    /* S4 切界面 */
    if(key == KEY_S4)
    {
        page++;
        if(page >= 4) page = 0;

        if(page == PAGE_PARA)
        {
            para_page = PARA_PC;
            output_lock = 1;
            lock_led_base = led_keep & (unsigned char)(~LED8);
            lock_move_state = move_state;
        }
        else
        {
            output_lock = 0;
        }
    }

    /* 参数界面按键 */
    if(page == PAGE_PARA)
    {
        if(key == KEY_S5)
        {
            para_page++;
            if(para_page >= 2) para_page = 0;
        }

        if(key == KEY_S8)
        {
            if(para_page == PARA_PC)
            {
                if(tem_para < 80) tem_para++;
            }
            else
            {
                if(dis_para <= 75) dis_para += 5;
            }
        }

        if(key == KEY_S9)
        {
            if(para_page == PARA_PC)
            {
                if(tem_para > 20) tem_para--;
            }
            else
            {
                if(dis_para >= 25) dis_para -= 5;
            }
        }
    }
}

/* ====================== 主函数 ====================== */
void main(void)
{
    unsigned int last_key_ms  = 0;//按键
    unsigned int last_temp_ms = 0;//温度
    unsigned int last_lux_ms  = 0;//光敏
    unsigned int last_dis_ms  = 0;//距离
    unsigned int last_mov_ms  = 0;//运动状态

    init();
    Timer0_Init();

    /* 初始状态 */
    page = PAGE_ENV;
    para_page = PARA_PC;
    tem_para = 30;
    dis_para = 30;
    relay_count = 0;
    move_state = MOVE_STOP;
    output_lock = 0;
    led_write(0x00);
    y5_write(0x00);

	DS18B20_convertT();
    delay(750);
	
    read_temp_data();
    read_light_data();
    read_distance_data();
    move_ref_distance = distance;

    while(1)
    {
        /* 持续显示 */
        if(page == PAGE_ENV) environment_display();
        else if(page == PAGE_MOVE) move_display();
        else if(page == PAGE_PARA) parameter_display();
        else stat_display();

        /* 10ms 扫键 */
        if(ms - last_key_ms >= 10)
        {
            last_key_ms = ms;
            key_proc();
        }

        /* 温度每1000ms采样一次 */
        if(ms - last_temp_ms >= 1000)
        {
            last_temp_ms = ms;
            read_temp_data();
        }

        /* 光敏更快采样，避免场景切换时显示滞后 */
        if(ms - last_lux_ms >= 100)
        {
            last_lux_ms = ms;
            read_light_data();
        }

        /* 距离更快采样，避免继电器判定滞后一拍 */
        if(ms - last_dis_ms >= 200)
        {
            last_dis_ms = ms;
            read_distance_data();
        }

        /* 运动状态每1000ms判断一次 */
        if(ms - last_mov_ms >= 1000)
        {
            last_mov_ms = ms;
            update_move_state();
        }

        judge_state();
        output_update();

        /* 统计界面：S8 + S9 长按2秒清零 */
        if(page == PAGE_STAT)
        {
            if(key_down_raw(KEY_S8) && key_down_raw(KEY_S9))
            {
                if(combo_hold_ms == 0)
                {
                    combo_hold_ms = ms;   /* 记录组合键开始按下时刻 */
                }
                else if((unsigned int)(ms - combo_hold_ms) >= 2000)
                {
                    relay_count = 0;
                    combo_hold_ms = 0;
                }
            }
            else
            {
                combo_hold_ms = 0;
            }
        }
        else
        {
            combo_hold_ms = 0;
        }
    }
}




