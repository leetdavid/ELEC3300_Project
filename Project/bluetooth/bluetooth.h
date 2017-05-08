/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include <stdio.h>

#define BT_BAUD 28800
#define MAX_STRLEN 20

void BLUETOOTH_Init(void);
void BLUETOOTH_Init_USART1(uint32_t baudrate);

#endif /*__WIRELESS_H*/