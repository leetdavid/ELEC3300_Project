/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include <stdio.h>

#define BT_BAUD 9600
#define MAX_STRLEN 20

/*********************************
 * BLUETOOTH CONNECTION GUIDE
 * PA9 and PA10
 * BT(RX) to PA9(USART1_TX)
 * BT(TX) to PA10(USART1_RX)
 */

void USART1_IRQHandler(void);

/* Utility Functions */
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);

/* Initialization Functions */
void Bluetooth_Init(void);

/* Sub-Initialization Functions */
void Bluetooth_NVIC_Configuration(void);
void Bluetooth_GPIO_Configuration(void);
void Bluetooth_USART_Configuration(void);

#endif /*__WIRELESS_H*/