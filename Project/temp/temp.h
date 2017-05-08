/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TEMP_H
#define __TEMP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "OneWire.h"
static OWire *wire;
static u8 signbit, num_whole, num_decimal, present;

static u8 data[12];
static u8 addr[8];

void Temp_Init(void);
void Temp_Read(void);
void Temp_Convert(void);

#endif /*__TEMP_H*/