/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LEDM_H
#define __LEDM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define COLUMN_BIT_LENGTH   32
#define DATA_BIT_LENGTH     24
#define ROW_COUNT           8

static u32 led_buffer [8]; 

void setLatchClkPin(u8 value);
void LEDM_Init(void);
void update_Buffer(void);

#endif /*__LEDM_H*/