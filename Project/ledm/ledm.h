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

#endif /*__LEDM_H*/