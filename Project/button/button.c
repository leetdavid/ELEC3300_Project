/* Includes ------------------------------------------------------------------*/
#include "button.h"
#include "lcd.h"

static u8 b1_push_count;
static u8 b2_push_count;

void Button_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  b1_push_count = 0;
  b2_push_count = 0;
}

/**
* @brief  This function handles External line 0 interrupt request.
* @param  None
* @retval None
*/


/* Task 5: Modify the Interrupt Handler for EXTI_Line_0  */
void EXTI4_IRQHandler(void)
{
   if(EXTI_GetITStatus(EXTI_Line4) != RESET)
  {
/* Task 5: Add code here to toggle LED5 (i.e. PF.8) when */
/*          KEY1 (i.e. PA.0) is pressed.                  */

	   b1_push_count += 1;
	   LCD_Clear();
	   u8 testStr[] = "Test Button";
	   LCD_DrawString(4, 0, testStr, sizeof testStr);
	   LCD_DrawChar(6, 0, HexValueOffset[b1_push_count]);



/* Clear the Key Button EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line4);
  }  
}

/**
* @brief  This function handles External lines 9 to 5 interrupt request.
* @param  None
* @retval None
*/
void EXTI9_5_IRQHandler(void)
{
   if(EXTI_GetITStatus(EXTI_Line5) != RESET)
  {

	   b2_push_count += 1;
	   LCD_Clear();
	   u8 testStr[] = "Test Button";
	   LCD_DrawString(4, 0, testStr, sizeof testStr);
	   LCD_DrawChar(6, 8, HexValueOffset[b2_push_count]);
	   EXTI_ClearITPendingBit(EXTI_Line5);
  }
}

//void EXTI15_10_IRQHandler(void) {
//  if(EXTI_GetITStatus(EXTI_Line12) != RESET)
//  {
//    b1_push_count += 1;
//    LCD_Clear();
//    u8 testStr[] = "Test Button";
//    LCD_DrawString(4, 0, testStr, sizeof testStr);
//    LCD_DrawChar(6, 0, HexValueOffset[b1_push_count]);
//    
//    /* Clear the Key Button EXTI line pending bit */
//    EXTI_ClearITPendingBit(EXTI_Line12);
//  }
//  if(EXTI_GetITStatus(EXTI_Line14) != RESET)
//  {    
//    b2_push_count += 1;
//    LCD_Clear();
//    u8 testStr[] = "Test Button";
//    LCD_DrawString(4, 0, testStr, sizeof testStr);
//    LCD_DrawChar(6, 8, HexValueOffset[b2_push_count]);
//    /* Clear the Key Button EXTI line pending bit */
//    EXTI_ClearITPendingBit(EXTI_Line14);
//  }
//}