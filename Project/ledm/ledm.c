/* Includes ------------------------------------------------------------------*/
#include "ledm.h"
#include <string.h>

const u8 row_array[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
static u8 row_no;
static u8 column_bit_no;
static u8 next_data_rising;
static u8 next_latch_rising;

void setDataPin(u8 value) {
  if(value > 0) {
    GPIOB->BSRR = GPIOB->IDR | GPIO_Pin_11;
  } else {
    GPIOB->BSRR = GPIOB->IDR & (~GPIO_Pin_11);
  }
}

void setDataClkPin(u8 value) {
  if(value > 0) {
    GPIOG->BSRR = GPIOG->IDR | GPIO_Pin_15;
  } else {
    GPIOG->BSRR = GPIOG->IDR & (~GPIO_Pin_15);
  }
}

void setLatchClkPin(u8 value) {
  if(value > 0) {
    GPIOC->BSRR = GPIOC->IDR | GPIO_Pin_5;
  } else {
    GPIOC->BSRR = GPIOC->IDR & (~GPIO_Pin_5);
  }
}

void LEDM_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  setDataPin(0);
  setDataClkPin(0);
  setLatchClkPin(0);
  
  memset(led_buffer, 0, sizeof(led_buffer));
  row_no = 0;
  column_bit_no = 0;
  next_data_rising = 0;
  next_latch_rising = 0;
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  TIM_TimeBaseStructure.TIM_Period = 1079; // TIMx_ARR register
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  
  TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void) {
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
    if(next_data_rising) {
      u32 output_value = 0;
      if(column_bit_no < DATA_BIT_LENGTH) {
        output_value = led_buffer[row_no] & (0x80000000 >> column_bit_no );
      } else {
        output_value = row_array[row_no] & (0x80 >> (column_bit_no - DATA_BIT_LENGTH));
      }
      setDataPin(output_value);
      column_bit_no++;
      if(column_bit_no>=COLUMN_BIT_LENGTH) {
        next_latch_rising = 1;
        column_bit_no = 0;
        row_no++;
        if(row_no >= ROW_COUNT) {
          row_no = 0;
        }
      }
      setDataClkPin(1);
      next_data_rising = 0;
    } else {
      setDataClkPin(0);
      if(next_latch_rising) {
        setLatchClkPin(1);
        next_latch_rising = 0;
      } else {
        setLatchClkPin(0);
        next_data_rising = 1;
      }
    }
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}