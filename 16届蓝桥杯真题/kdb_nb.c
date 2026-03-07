// kbd_nb.c / 或直接放进 kbd.c
#include <STC15F2K60S2.H>

// 返回：0=无新按键事件；其它=键值（沿用你原先的7/6/5/4...19/18/17/16映射）
unsigned char keynumber_nb(void)
{
    static unsigned char last_key = 0;     // 上一次“稳定键值”
    static unsigned char stable_cnt = 0;   // 稳定计数
    unsigned char key = 0;

    // 下面这段“读当前键值”基本照抄你原 kbd.c，但【不要 delay】【不要 while 等松手】
    P44=1;P42=1;P35=1;P34=1;
    P30=1;P31=1;P32=1;P33=1;

    P44=0;
    if(P30==0) key=7;
    else if(P31==0) key=6;
    else if(P32==0) key=5;
    else if(P33==0) key=4;

    P44=1;P42=1;P35=1;P34=1;
    P30=1;P31=1;P32=1;P33=1;
    P42=0;
    if(P30==0) key=11;
    else if(P31==0) key=10;
    else if(P32==0) key=9;
    else if(P33==0) key=8;

    P44=1;P42=1;P35=1;P34=1;
    P30=1;P31=1;P32=1;P33=1;
    P35=0;
    if(P30==0) key=15;
    else if(P31==0) key=14;
    else if(P32==0) key=13;
    else if(P33==0) key=12;

    P44=1;P42=1;P35=1;P34=1;
    P30=1;P31=1;P32=1;P33=1;
    P34=0;
    if(P30==0) key=19;
    else if(P31==0) key=18;
    else if(P32==0) key=17;
    else if(P33==0) key=16;

    // ====== 消抖 + “按下沿”触发（不阻塞） ======
    if(key == last_key)
    {
        if(stable_cnt < 3) stable_cnt++; // 连续3次一致认为稳定（比如每10ms一次 → 30ms消抖）
    }
    else
    {
        stable_cnt = 0;
        last_key = key;
        return 0;
    }

    if(stable_cnt == 3)
    {
        // 稳定后只在“从0到非0”的瞬间返回一次
        static unsigned char reported = 0;
        if(last_key != 0 && reported == 0)
        {
            reported = 1;
            return last_key;   // 产生一次“按下事件”
        }
        if(last_key == 0)
        {
            reported = 0;      // 松手后允许下一次按下事件
        }
    }

    return 0;
}