/* Includes ------------------------------------------------------------------*/
#include "clock.h"

void Clock_Init(void) {
  Clock_RTC_Configuration();
}

void Clock_RTC_Configuration(void){

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

uint32_t getRawTime(void){
  time_raw = RTC_GetCounter();
  return time_raw;
}

void Clock_UpdateValues(void){
  time_raw = getRawTime();
  uint32_t THH = time_raw/3600;
  uint32_t TMM = (time_raw%3600)/60;
  //uint32_t TSS = (timevar%3600)%60;

  h1 = THH/10;
  h2 = THH%10;

  m1 = TMM/10;
  m2 = TMM%10;
}

void Time_SetRaw(uint32_t time){
  RTC_WaitForLastTask();
  RTC_SetCounter(time);
  RTC_WaitForLastTask();
}

void Time_Set(u8 h1, u8 h2, u8 m1, u8 m2){
  uint32_t hh1 = h1, hh2 = h2, mm1 = m1, mm2 = m2;
  uint32_t THH = hh1 << 4 + hh2;
  uint32_t TMM = mm1 << 4 + mm2;
  Time_SetRaw(THH*3600 + TMM*60);
}