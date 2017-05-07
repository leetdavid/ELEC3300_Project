/* Includes ------------------------------------------------------------------*/
#include "button.h"
#include "lcd.h"

static u8 b1_push_count;
static u8 b2_push_count;

void Button_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
  EXTI_InitStructure.EXTI_Line = EXTI_Line14;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);
  EXTI_InitStructure.EXTI_Line = EXTI_Line15;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  b1_push_count = 0;
  b2_push_count = 0;
}

void EXTI15_10_IRQHandler(void) {
  if(EXTI_GetITStatus(EXTI_Line14) != RESET)
  {
    b1_push_count += 1;
    LCD_Clear();
    u8 testStr[] = "Test Button";
    LCD_DrawString(4, 0, testStr, sizeof testStr);
    LCD_DrawChar(6, 0, HexValueOffset[b1_push_count]);
    
    /* Clear the Key Button EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line14);
  }
  if(EXTI_GetITStatus(EXTI_Line15) != RESET)
  {    
    b2_push_count += 1;
    LCD_Clear();
    u8 testStr[] = "Test Button";
    LCD_DrawString(4, 0, testStr, sizeof testStr);
    LCD_DrawChar(6, 8, HexValueOffset[b2_push_count]);
    /* Clear the Key Button EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line15);
  }
}