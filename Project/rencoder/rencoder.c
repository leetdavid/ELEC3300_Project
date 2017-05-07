/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_it.h"
#include "rencoder.h"
#include "lcd.h"

#define H_Stable_State  0
#define L_Stable_State  1
#define Turning_CW      2
#define Turning_CCW     3

static u8 rencoder_state;
static uint32_t turn_count;

u8 ifPinsEqual(void) {
  bool pinC0 = GPIOC->IDR & GPIO_Pin_0 == GPIO_Pin_0 ? TRUE : FALSE;
  bool pinC1 = GPIOC->IDR & GPIO_Pin_1 == GPIO_Pin_1 ? TRUE : FALSE;
  return pinC0 == pinC1;
}

void setStableState(void) {
  if(ifPinsEqual()) {
    if(GPIOC->IDR & GPIO_Pin_0 == GPIO_Pin_0) {
      rencoder_state = H_Stable_State;
    } else {
      rencoder_state = L_Stable_State;
    }
  }
}

void REncoder_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  setStableState();
  turn_count = 0;
}

void EXTI0_IRQHandler(void) {
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    if(ifPinsEqual()) {
      turn_count = turn_count - 1;
      LCD_Clear();
      u8 testStr[] = "Turned CCW";
      LCD_DrawString(0, 0, testStr, sizeof testStr);
      LCD_DrawChar(2, 0, HexValueOffset[turn_count]);
      setStableState();
    } else {
      if(rencoder_state == H_Stable_State || rencoder_state == L_Stable_State) {
        rencoder_state = Turning_CW;
      }
    }
    
    /* Clear the Key Button EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
  }  
}

void EXTI1_IRQHandler(void) {
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
    if(ifPinsEqual()) {
      turn_count = turn_count + 1;
      LCD_Clear();
      u8 testStr[] = "Turned CW";
      LCD_DrawString(0, 0, testStr, sizeof testStr);
      LCD_DrawChar(2, 0, HexValueOffset[turn_count]);
      setStableState();
    } else {
      if(rencoder_state == H_Stable_State || rencoder_state == L_Stable_State) {
        rencoder_state = Turning_CCW;
      }
    }
    
    /* Clear the Key Button EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line1);
  }  
}