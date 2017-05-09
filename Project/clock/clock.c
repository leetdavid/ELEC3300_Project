/* Includes ------------------------------------------------------------------*/
#include "clock.h"
#include "ledm.h"
#include "waveplayer.h"
#include "main.h"

u8 h1, h2, m1, m2;
uint32_t time_raw;
//u8 i_index;
u8 halt_display_s;
uint32_t alarm_raw;
u8 alarm_on;
u8 show_time;

void Clock_Init(void) {
	//i_index = 0;
	halt_display_s = 0;
	alarm_raw = 0;
	alarm_on = 0;
	show_time = 0;

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

  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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

void halt_display(u8 seconds) {
	halt_display_s = seconds;
}

void Time_SetRaw(uint32_t time){
  RTC_WaitForLastTask();
  RTC_SetCounter(time);
  RTC_WaitForLastTask();
}

void Time_Set(u8 i_h1, u8 i_h2, u8 i_m1, u8 i_m2){
	uint32_t hh1 = i_h1, hh2 = i_h2, mm1 = i_m1, mm2 = i_m2;
  uint32_t THH = hh1 *10 + hh2;
  uint32_t TMM = mm1 *10 + mm2;
  Time_SetRaw(THH*3600 + TMM*60);
}

void Alarm_Set(u8 i_h1, u8 i_h2, u8 i_m1, u8 i_m2) {
	alarm_on = 1;
	uint32_t hh1 = i_h1, hh2 = i_h2, mm1 = i_m1, mm2 = i_m2;
	uint32_t THH = hh1 * 10 + hh2;
	uint32_t TMM = mm1 * 10 + mm2;
	alarm_raw = THH * 3600 + TMM * 60;
}

void Alarm_disable() {
	alarm_on = 0;
}

void Show_time_En() {
	show_time = 1;
}

void Show_time_Dis() {
	show_time = 0;
}

void RTC_IRQHandler(void) {
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
		RTC_WaitForLastTask();

		if (alarm_on == 1 && alarm_raw == getRawTime()) {
			//halt_display(10);
			play_next = 1;
			Show_time_En();
		}

		if (show_time == 1) {
			if (halt_display_s == 0) {
				Clock_UpdateValues();
				setDisplayTime(h1, h2, m1, m2);
			}
			else {
				halt_display_s--;
			}
			//setDisplayIcon(i_index);
			updateDisplay();
		}
		/*i_index++;
		if (i_index > 3) {
			i_index = 0;
		}*/

		RTC_ClearITPendingBit(RTC_IT_SEC);
	}
}