#include "tim.h"
#include "seg.h"
#include <stdio.h>
#include "key.h"
#include "ds18b20.h"
#include "i2c.h"

unsigned int  uiTms;              		// 毫秒值
unsigned int  uiSec;              		// 秒值
unsigned char ucState;            		// 系统状态
// 0x00：环境界面，0x10：运动界面，0x20-0x21：参数界面，0x30：统计界面
unsigned char ucDis;                	// 距离值
unsigned char ucTem;                	// 温度值
unsigned char ucAdc;                	// ADC值
unsigned char ucN;                  	// 光强等级
unsigned char ucDS;                 	// 运动状态
unsigned char ucPC=30;               	// 温度参数
unsigned char ucPL=30;               	// 距离参数
unsigned char ucNum;                	// 继电器吸合次数

void Led_Proc(void);
void Seg_Proc(void);
void Key_Proc(void);
void Data_Proc(void);
// 主函数
void main(void)
{
  Close_Peripheral();
  T1_Init();
	
  while (1)
  {
    Seg_Proc();
    Key_Proc();
    Led_Proc();
    Data_Proc();
	}
}

unsigned char pucSeg_Char[12];       	// 显示字符
unsigned char pucSeg_Code[8];       	// 显示代码
unsigned char ucSeg_Pos;            	// 显示位置
unsigned int  uiSeg_Dly;            	// 显示刷新延时
unsigned char ucSeg_Dly;            	// 显示移位延时
void Seg_Proc(void)
{
  if (uiSeg_Dly >= 100)           		// 100ms刷新1次
  {
    uiSeg_Dly = 0;

    switch (ucState)
    {
      case 0:                       	// 环境状态界面
        sprintf(pucSeg_Char, "C%2u   N%1u", (int)ucTem, (int)ucN);
        break;
      case 0x10:                     	// 运动检测界面
        sprintf(pucSeg_Char, "L%1u   %03u", (int)ucDS, (int)ucDis);
        break;
      case 0x20:                      // 温度参数界面
        sprintf(pucSeg_Char, "PC    %2u", (int)ucPC);
        break;
      case 0x21:                    	// 距离参数界面
        sprintf(pucSeg_Char, "PL    %2u", (int)ucPL);
        break;
      case 0x30:                     	// 统计数据界面
        sprintf(pucSeg_Char, "NC  %4u", (int)ucNum);
    }
    Seg_Tran(pucSeg_Char, pucSeg_Code);
  }
}

unsigned char ucKey_Old;            	// 旧键值
unsigned char ucKey_Dly;            	// 按键刷新延时
unsigned int  uiKey_Dly=500;        	// 双击或长按延时
void Key_Proc(void)
{
  unsigned char ucKey_Dn;

  if(ucKey_Dly < 10)              		// 延时10ms消抖
    return;
  ucKey_Dly = 0;

  ucKey_Dn = Key_Read();          		// 键值读取
  if (ucKey_Dn != ucKey_Old) 	      	// 键值变化
  {
    ucKey_Old = ucKey_Dn;
    uiKey_Dly = 0;
	} else {
    ucKey_Dn = 0;
  }

  switch (ucKey_Dn)
  {
    case 4:                         	// S4按键按下
      ucState &= 0xF0;
      ucState += 0x10;              	// 切换主界面
      if (ucState >= 0x40)
        ucState = 0;
      break;
    case 5:                         	// S5按键按下
      if ((ucState & 0xf0) == 0x20)
        ucState ^= 1;               	// 切换参数子界面
	      break;
    case 8:                         	// S8按键按下
      if (ucState == 0x20)
	        if (ucPC < 80)
	          ucPC++;                   // 温度参数增加1℃
      if (ucState == 0x21)
        if (ucPL < 80)
          ucPL += 5;                 	// 距离参数增加5cm
      break;
    case 9:                         	// S9按键按下
      if (ucState == 0x20)
        if (ucPC > 20)
          ucPC--;                    	// 温度参数减小1℃
      if (ucState == 0x21)
	        if (ucPL > 20)
	          ucPL -= 5;                // 距离参数减小5cm
  }
  if ((ucKey_Old == 20) && (uiKey_Dly > 2000))
    ucNum = 0;                        // 清零继电器吸合次数
}

unsigned char ucLed;                	// LED值
unsigned char ucLed_Dly;            	// LED刷新延时
unsigned char ucUln_Flg;            	// 继电器吸合标志
unsigned char ucLed8_Flg;             //LED8闪烁标志
void Led_Proc(void)
{

  if (ucLed_Dly < 100)              	// 100ms刷新1次
    return;
  ucLed_Dly = 0;
  
  if ((ucState & 0xf0) == 0x20)      	// 参数界面
  {
    if(ucLed8_Flg == 1) ucLed ^= 0x80;
    Led_Disp(ucLed);
    return;
  }

  if (ucDis < ucPL)                   // 接近
    switch (ucN)                      // 光照等级
    {
      case 1:
        ucLed |= 1;			           		// L1点亮
        ucLed &= 0xF1;	         			// L2-L4熄灭
        break;
      case 2:
        ucLed |= 3;		                // L1-L2点亮
        ucLed &= 0xF3;		        		// L3-L4熄灭
        break;
      case 3:
        ucLed |= 7;				          	// L1-L3点亮
        ucLed &= 0xF7;		          	// L4熄灭
        break;
      case 4:
        ucLed |= 15;			          	// L1-L4点亮
        break;
    }
  else
    ucLed &= 0xF0;			            	// L1-L4熄灭

  switch (ucDS)                     	// 运动状态
  {
    case 1:
        ucLed8_Flg = 0;
	      ucLed &= 0x7F;		  	      	// L8熄灭
	      break;
    case 2:
        ucLed8_Flg = 0;
	      ucLed |= 0x80;	            	// L8点亮
	      break;
    case 3:
        ucLed8_Flg = 1;
	      ucLed ^= 0x80;	          		// L8闪烁
        break;
  }
  Led_Disp(ucLed);
                                     	// 接近和高温判定
  if ((ucDis < ucPL) && (ucTem > ucPC))
  {
    Uln_Ctrl(0x10);	  	            	// 继电器吸合
    if (ucUln_Flg == 0)
	    {
	      ucUln_Flg = 1;
	      ucNum++;  	                 	// 继电器吸合次数+1
	    }
	  }
  else
  {
    Uln_Ctrl(0);  	                	// 继电器断开
	    ucUln_Flg = 0;
  }
}

unsigned int  uiSec1, uiSec2;       	// 秒计时
unsigned char ucDis1;               	// 距离值
unsigned char ucDS1;                	// 运动状态
void Data_Proc(void)
{
  unsigned char ucDL;
	
  if (uiSec1 == uiSec)
    return;
  uiSec1 = uiSec;
	
  ucAdc = PCF8591_Adc(1)/5.1;        	// 光强检测
  if (ucAdc >= 30)
    ucN = 1;
  else if (ucAdc >= 20)
    ucN = 2;
  else if (ucAdc >= 5)
    ucN = 3;
  else
    ucN = 4;
		
  ucDis = Dist_Meas();              	// 移动检测
  if ((uiSec-uiSec2) >= 4)          	// 未锁定
  {
    if (ucDis > ucDis1)
      ucDL = ucDis - ucDis1;
    else
      ucDL = ucDis1 - ucDis;

    if (ucDL < 5)                   	// 状态判定
	      ucDS = 1;
    else if (ucDL < 10)
      ucDS = 2;
    else
      ucDS = 3;
  }
  ucDis1 = ucDis;	
	
  if (ucDS1 != ucDS)                	// 状态变化
  {
    ucDS1 = ucDS;
    uiSec2 = uiSec;
  }
	
  ucTem = Temp_Read()>>4;
}
