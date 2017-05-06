/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTON_H
#define __BUTTON_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

void Button_Init(void);
void EXTI15_10_IRQHandler(void);

#endif /*__BUTTON_H*/