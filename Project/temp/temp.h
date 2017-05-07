/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TEMP_H
#define __TEMP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

void Temp_Init(void);
void Temp_Read(void);
void Temp_WaitForConversion(void);
u8 Temp_GetPrecision(void);
void Temp_Convert(void);

#endif /*__TEMP_H*/