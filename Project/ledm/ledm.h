/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LEDM_H
#define __LEDM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define COLUMN_BIT_LENGTH   32
#define DATA_BIT_LENGTH     24
#define ROW_COUNT           8

static u32 led_buffer [8]; 

void setDataPin(u8 value);
void setDataClkPin(u8 value);
void setLatchClkPin(u8 value);
void LEDM_Init(void);
void TIM2_IRQHandler(void);

/*---- High-Layer Functions ----*/
void showDisplay(u8 row, u8 rval, u8 gval, u8 bval);
void copyNum(u8 arr[8], u8 num, u8 x, u8 y);
void setTime(u8 h1, u8 h2, u8 m1, u8 m2);

/* Minor Methods */
void delay(void);

#endif /*__LEDM_H*/