/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CLOCK_H
#define __CLOCK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/*--- The Time is in BCD format ---*/
static u8 h1, h2, m1, m2;
static uint32_t time_raw;

void Clock_Init(void);
void Clock_RTC_Configuration(void);
void Clock_UpdateValues(void);
uint32_t getRawTime(void);

void Time_SetRaw(uint32_t time);
void Time_Set(u8 h1, u8 h2, u8 m1, u8 m2);

#endif /*__CLOCK_H*/