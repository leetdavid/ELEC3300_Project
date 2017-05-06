/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RENCODER_H
#define __RENCODER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

bool ifPinsEqual(void);
void setStableState(void);
void REncoder_Init(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);

#endif /*__RENCODER_H*/