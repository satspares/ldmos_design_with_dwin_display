#ifndef __SYS_H__
#define __SYS_H__

#include "Dwin_T5L1H.h"
#include "Uart.h"

#ifndef NULL
 #define NULL ((void *)0)
#endif
 
extern u16    xdata        SysTick_RTC;
extern bit                 RTC_Flog;
//extern u16    xdata        Count_num1;
extern u16    xdata        TimVal;


#define  u8   unsigned char  
#define  s8     signed char
#define  u16  unsigned int 
#define  s16    signed int
#define  u32  unsigned long 
#define  s32    signed long


void Sys_Cpu_Init();


#endif