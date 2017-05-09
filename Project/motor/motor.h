/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTOR_H
#define __MOTOR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

void Motor_Init(void);
void setMotorSwitchPin(u8 value);

#endif /*__MOTOR_H*/