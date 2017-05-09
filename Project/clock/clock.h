/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CLOCK_H
#define __CLOCK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/*--- The Time is in BCD format ---*/
extern u8 h1, h2, m1, m2;
extern uint32_t time_raw;
extern u8 halt_display_s;

void Clock_Init(void);
void Clock_RTC_Configuration(void);
void Clock_UpdateValues(void);
uint32_t getRawTime(void);
void halt_display(u8 seconds);

void Time_SetRaw(uint32_t time);
void Time_Set(u8 i_h1, u8 i_h2, u8 i_m1, u8 i_m2);
void Alarm_Set(u8 i_h1, u8 i_h2, u8 i_m1, u8 i_m2);
void Alarm_disable(void);
void Show_time_En(void);
void Show_time_Dis(void);

#endif /*__CLOCK_H*/