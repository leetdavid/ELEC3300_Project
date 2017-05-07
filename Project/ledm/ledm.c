/* Includes ------------------------------------------------------------------*/
#include "ledm.h"
//#include "stm32_eval.h"
#include <string.h>
//#include <stdio.h>

const u8 row_array[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
static u8 row_no;
static u8 column_block_no;

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
        (~(disp_b[i]) & 0x000000FF) << 24 | 
        (~(disp_r[i]) & 0x000000FF) << 16 | 
        (~(disp_g[i]) & 0x000000FF) << 8 |
        1 << i;
  }
}

void copyNum(u8 *arr, u8 num, u8 x, u8 y){
  for(u8 i = 0; i < 5; i++)
    arr[i+y] |= nums[num][i] >> x;
}

void clearDisplay(void){
  memcpy(disp_r, disp_blank, sizeof(disp_r));
  memcpy(disp_g, disp_blank, sizeof(disp_r));
  memcpy(disp_b, disp_blank, sizeof(disp_r));
}

void setTime(u8 h1, u8 h2, u8 m1, u8 m2){
  clearDisplay();
  copyNum(disp_r, h1, 0, 0);
  copyNum(disp_r, h2, 4, 0);
  copyNum(disp_b, m1, 1, 3);
  copyNum(disp_b, m2, 5, 3);
}


void setLatchClkPin(u8 value) {
  if(value > 0) {
    //GPIOC->BSRR = GPIOC->IDR | GPIO_Pin_5;
    GPIOC->BSRR = GPIO_Pin_5;
  } else {
    //GPIOC->BSRR = GPIOC->IDR & (~GPIO_Pin_5);
    GPIOC->BRR = GPIO_Pin_5;
  }
}

void LEDM_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
  //RCC_PCLK2Config(RCC_HCLK_Div2); 
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; /* Master Mode */
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
  SPI_Init(SPI1, &SPI_InitStructure); /* Configure SPI1 */
  
  SPI_Cmd(SPI1, ENABLE); /* Enable SPI1 */
  
  memset(led_buffer, 0, sizeof(led_buffer));
  row_no = 0;
  column_block_no = 0;
  
  // test codes  
  //memcpy(disp_r, icons[0][0], sizeof(disp_r) * 8);
  //memcpy(disp_g, icons[0][1], sizeof(disp_g) * 8);
  //memcpy(disp_b, icons[0][2], sizeof(disp_b) * 8);
  updateDisplay();
  
  update_Buffer();
  
  /* Initalize RTC */
  //LEDM_RTC_Configuration();
}

void update_Buffer(void) {
  u8 send_data;
  for(int i = 0; i < ROW_COUNT; i++) {
    setLatchClkPin(0);
    send_data = (led_buffer[i] & 0xFF000000) >> 24;
    SPI_I2S_SendData(SPI1, send_data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    send_data = (led_buffer[i] & 0x00FF0000) >> 16;
    SPI_I2S_SendData(SPI1, send_data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    send_data = (led_buffer[i] & 0x0000FF00) >> 8;
    SPI_I2S_SendData(SPI1, send_data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    send_data = (led_buffer[i] & 0x000000FF);
    SPI_I2S_SendData(SPI1, send_data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    for (int i=0; i<500; i++);
    setLatchClkPin(1);
  }
}

//#ifdef __CC_ARM
//__asm void wait(){
//  nop
//  BX lr
//}
//#endif

//void LEDMdelay(void);
void LEDMdelay(void){
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