/* Includes ------------------------------------------------------------------*/
#include "ledm.h"
#include <string.h>
#include "clock.h"
//#include <stdio.h>
//#include "lcd.h"

const u8 row_array[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
static u8 row_no;
static u8 column_block_no;
static u8 update_count;
static uint32_t delay_counter;

static uint32_t time_mode_revert;

void computeDisplay(void){

  /*--- Time ---*/
  if(mode == 0){ 
    //TODO: get time
    setDisplayTime(1, 2, 3, 4);

    /*--- Visualization ---*/
  } else if(mode == 1){


    /*--- Custom/Icons ---*/
  } else if(mode == 2){
    mode_prev = mode;
    setDisplayMode(3);
    //how to access clock.h time_raw?
    time_mode_revert = getRawTime() + 3; //will revert mode in 3 seconds
    setDisplayIcon(icon);

    /*--- Showing Icons Service ---*/
  } else if(mode == 3){
    //if enought time passes, revert mode back to what it previously was
      if(getRawTime() >= time_mode_revert){
        mode = mode_prev;
        clearDisplay();
      }
  }

  updateDisplay();
}

void setDisplayMode(u8 m){
  mode = m;
}

void setDisplayIcon(u8 icon){
  setDisplay(icons[icon][0],icons[icon][1],icons[icon][2]);
}

void setDisplay(u8 *r, u8 *g, u8 *b){
  for(u8 i = 0; i < 8; i++){
    disp_r[i] = r[i];
    disp_g[i] = g[i];
    disp_b[i] = b[i];
  }
}

void updateDisplay(void){
  for(u8 i = 0; i < 8; i++){
    led_buffer[i*4 + 0] = ~(disp_b[i]);
    led_buffer[i*4 + 1] = ~(disp_r[i]);
    led_buffer[i*4 + 2] = ~(disp_g[i]);
    led_buffer[i*4 + 3] = 1 << i;
  }
}

void copyNum(u8 *arr, u8 num, u8 x, u8 y){
  for(u8 i = 0; i < 5; i++)
    arr[i+y] |= nums[num][i] >> x;
}

void clearDisplay(void){
  setDisplay(disp_blank, disp_blank, disp_blank);
}

void refreshMatrixTest(void) {
	if (DMA_GetFlagStatus(DMA1_FLAG_TC5) == SET && SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == SET && delay_counter == 0) {
		row_no++;
		if (row_no >= ROW_COUNT) {
			row_no = 0;
		}
		DMA_ClearFlag(DMA1_FLAG_TC5);
		//DMA_Cmd(DMA1_Channel3, DISABLE);
		DMA1_Channel5->CCR = 0x0000;
		setLatchClkPin(0);
		delay_counter++;
	}
	if (delay_counter > 0) {
		delay_counter++;
		if (delay_counter >= 500) {
			delay_counter = 0;
			setLatchClkPin(1);
			DMA2_Channel5->CPAR = 0x4001300C;
			DMA2_Channel5->CNDTR = 4;
			DMA2_Channel5->CMAR = (uint32_t)&led_buffer[row_no * 4];
			//DMA_Cmd(DMA1_Channel3, ENABLE);
			DMA2_Channel5->CCR = 0x2093;
			//SPI_I2S_SendData(SPI2, 0xFF);
		}
	}
}

void refreshMatrixLoopTest(void) {
	while (1) {
		refreshMatrixTest();
	}
}

void refreshMatrixTest2(void) {
	if (delay_counter > 0) {
		delay_counter++;
		if (delay_counter >= 1) {
			delay_counter = 0;
			setLatchClkPin(1);
		}
	}
	else if (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == SET) {
		if (column_block_no < 4) {
			SPI_I2S_SendData(SPI2, led_buffer[row_no * 4 + column_block_no]);
			column_block_no++;
		}
		else {
			column_block_no = 0;
			row_no++;
			if (row_no >= ROW_COUNT) {
				row_no = 0;
			}
			setLatchClkPin(0);
			delay_counter++;
		}
	}
}

void refreshMatrixLoopTest2(void) {
	while (1) {
		refreshMatrixTest2();
	}
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
  //RCC_PCLK2Config(RCC_HCLK_Div2); 
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; /* Master Mode */
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
  SPI_Init(SPI2, &SPI_InitStructure); /* Configure SPI2 */
  
  //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  DMA_DeInit(DMA1_Channel3); 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)0x4000380C;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&led_buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 4;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  //DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  
  /* Enable SPI_MASTER DMA Tx request */
  //SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
  
  SPI_Cmd(SPI2, ENABLE); /* Enable SPI2 */
  
  //DMA_Cmd(DMA1_Channel5, ENABLE);
  
  //DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
 
  //Enable DMA1 channel IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //NVIC_Init(&NVIC_InitStructure);
  
  memset(led_buffer, 0, sizeof(led_buffer));
  setLatchClkPin(0);
  row_no = 0;
  column_block_no = 0;
  update_count = 0;
  delay_counter = 0;
  
  // test codes  
  /*for(int i = 0; i < 8; i++) {
    disp_r[i] = icons[0][0][i];
    disp_g[i] = icons[0][1][i];
    disp_b[i] = icons[0][2][i];
  }*/
  setDisplayIcon(0);

  //memcpy(disp_r, icons[0][0], sizeof(disp_r) * 8);
  //memcpy(disp_g, icons[0][1], sizeof(disp_g) * 8);
  //memcpy(disp_b, icons[0][2], sizeof(disp_b) * 8);

  updateDisplay();
  
  update_Buffer();
}

void DMA1_Channel5_IRQHandler(void)
{
  //Test on DMA1 Channel1 Transfer Complete interrupt
  if(DMA_GetITStatus(DMA1_IT_TC5))
  {
    //DMA1_Channel3->CNDTR = 32;
    update_count++;
    //if(update_count >= 4) {
    //  update_count = 0;
    //}
	//LCD_Clear();
	//LCD_DrawChar(2, 0, HexValueOffset[update_count]);
    //Clear DMA1 Channel5 Complete Transfer, Transfer Complete and Global interrupt pending bits
    DMA_ClearITPendingBit(DMA1_IT_GL5);
  }
}

void update_Buffer(void) {
  //u8 send_data;
  for(int i = 0; i < ROW_COUNT; i++) {
    setLatchClkPin(0);
    for(int j = 0; j < 4; j++) {
      SPI_I2S_SendData(SPI2, led_buffer[i*4 + j]);
      while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    }
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