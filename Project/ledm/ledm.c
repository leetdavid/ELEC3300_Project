/* Includes ------------------------------------------------------------------*/
#include "ledm.h"
#include "stm32_eval.h"
#include <string.h>
#include <stdio.h>

const u8 row_array[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
static u8 row_no;
static u8 column_bit_no;
static u8 next_data_rising;
static u8 next_latch_rising;

void getCurrentTime(void){
  uint32_t timevar = RTC_GetCounter();
  uint32_t THH = timevar/3600;
  uint32_t TMM = (timevar%3600)/60;
  //uint32_t TSS = (timevar%3600)%60;

  h1 = THH/10;
  h2 = THH%10;

  m1 = TMM/10;
  m2 = TMM%10;
}

void updateDisplay(void){
  for(u8 i = 0; i < 8; i++){

    led_buffer[i] = 
        0x00000000 |
        ~(disp_b[i]) << 24 | 
        ~(disp_r[i]) << 16 | 
        ~(disp_g[i]) << 8 |
        1 << i;
  }
}

void copyNum(u8 *arr, u8 num, u8 x, u8 y){
  for(u8 i = 0; i < 5; i++)
    arr[i+y] |= nums[num][i] >> x;
}

void clearDisplay(void){
  for(u8 i = 0; i < 8; i++){
    disp_r[i] = 0;
    disp_g[i] = 0;
    disp_b[i] = 0;
  }
}

void setTime(u8 h1, u8 h2, u8 m1, u8 m2){
  clearDisplay();
  copyNum(disp_r, h1, 0, 0);
  copyNum(disp_r, h2, 4, 0);
  copyNum(disp_b, m1, 1, 3);
  copyNum(disp_b, m2, 5, 3);
}

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

  /* Initalize RTC */
  LEDM_RTC_Configuration();
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

#ifdef __CC_ARM
__asm void wait(){
  nop
  BX lr
}
#endif

void delay(void);
void delay(){
  vu8 i=0x8;
  while(i--)
    #ifdef __CC_ARM
    wait(); 
    #else
       asm("nop");
    #endif
}

void setMode(u8 m){
  mode = m;
}

void LEDM_RTC_Configuration(void){

  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  BKP_DeInit();

  RCC_LSEConfig(RCC_LSE_ON);
  /* wait until LSE is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  RCC_RTCCLKCmd(ENABLE);

  RTC_WaitForSynchro();
  RTC_WaitForLastTask();
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  RTC_WaitForLastTask();
  
  /* Set RTC Prescaler: set RTC period to 1 sec */
  /* RTC period = RTCCLK/RTC_PR = 32.768kHz/(32767+1) */
  RTC_SetPrescaler(32767); 

  RTC_WaitForLastTask();

}