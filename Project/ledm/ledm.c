/* Includes ------------------------------------------------------------------*/
#include "ledm.h"
//#include "stm32_eval.h"
#include <string.h>
//#include <stdio.h>

const u8 row_array[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
static u8 row_no;
static u8 column_block_no;
static u8 update_count;

void computeDisplay(void){
  switch(mode){
    case 0: /*--- Time ---*/
      //TODO: get time
      setDisplayTime(1, 2, 3, 4);
      break;
    case 1: /*--- Visualization ---*/

      break;
    case 2: /*--- Custom/Icons ---*/
      u8 icon = 2; //make this value modifiable

      break;
  }

  updateDisplay();
}

void setMode(u8 m){
  mode = m;
}

void setIcon(u8 icon){
  u8 i;
  for(i = 0; i < 8; i++){
    disp_r[i] = icons[icon][0][i];
    disp_g[i] = icons[icon][1][i];
    disp_b[i] = icons[icon][2][i];
  }
}

void updateDisplay(void){
  for(u8 i = 0; i < 8; i++){
    led_buffer[i*4 + 0] = ~(disp_b[i]);
    led_buffer[i*4 + 1] = ~(disp_r[i]);
    led_buffer[i*4 + 2] = ~(disp_g[i]);
    led_buffer[i*4 + 3] = 1 << i;
    //led_buffer[i] = 
    //    (~(disp_b[i]) & 0x000000FF) << 24 | 
    //    (~(disp_r[i]) & 0x000000FF) << 16 | 
    //    (~(disp_g[i]) & 0x000000FF) << 8 |
    //    1 << i;
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

void setDisplayTime(u8 h1, u8 h2, u8 m1, u8 m2){
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
  DMA_InitTypeDef DMA_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
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
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
  SPI_Init(SPI1, &SPI_InitStructure); /* Configure SPI1 */
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  DMA_DeInit(DMA1_Channel3); 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)0x4001300C;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&led_buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 32;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  
  /* Enable SPI_MASTER DMA Tx request */
  SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
  
  SPI_Cmd(SPI1, ENABLE); /* Enable SPI1 */
  
  DMA_Cmd(DMA1_Channel3, ENABLE);
  
  DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
 
  //Enable DMA1 channel IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //while(!DMA_GetFlagStatus(DMA1_FLAG_TC3));
  
  memset(led_buffer, 0, sizeof(led_buffer));
  row_no = 0;
  column_block_no = 0;
  update_count = 0;
  
  // test codes  
  for(int i = 0; i < 8; i++) {
    disp_r[i] = icons[0][0][i];
    disp_g[i] = icons[0][1][i];
    disp_b[i] = icons[0][2][i];
  }
  //memcpy(disp_r, icons[0][0], sizeof(disp_r) * 8);
  //memcpy(disp_g, icons[0][1], sizeof(disp_g) * 8);
  //memcpy(disp_b, icons[0][2], sizeof(disp_b) * 8);
  updateDisplay();
  
  update_Buffer();
  
  /* Initalize RTC */
  //LEDM_RTC_Configuration();
}

void DMA1_Channel3_IRQHandler(void)
{
  //Test on DMA1 Channel1 Transfer Complete interrupt
  if(DMA_GetITStatus(DMA1_IT_TC3))
  {
    //DMA1_Channel3->CNDTR = 32;
    update_count++;
    if(update_count >= 4) {
      update_count = 0;
    }
    //Clear DMA1 Channel1 Half Transfer, Transfer Complete and Global interrupt pending bits
    DMA_ClearITPendingBit(DMA1_IT_GL3);
  }
}

void update_Buffer(void) {
  //u8 send_data;
  for(int i = 0; i < ROW_COUNT; i++) {
    setLatchClkPin(0);
    for(int j = 0; j < 4; j++) {
      SPI_I2S_SendData(SPI1, led_buffer[i*4 + j]);
      while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    }
    //send_data = (led_buffer[i] & 0xFF000000) >> 24;
    //SPI_I2S_SendData(SPI1, send_data);
    //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    //send_data = (led_buffer[i] & 0x00FF0000) >> 16;
    //SPI_I2S_SendData(SPI1, send_data);
    //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    //send_data = (led_buffer[i] & 0x0000FF00) >> 8;
    //SPI_I2S_SendData(SPI1, send_data);
    //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    //send_data = (led_buffer[i] & 0x000000FF);
    //SPI_I2S_SendData(SPI1, row_array[i]);
    //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
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