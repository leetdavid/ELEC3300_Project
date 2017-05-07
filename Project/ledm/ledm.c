/* Includes ------------------------------------------------------------------*/
#include "ledm.h"
#include <string.h>

const u8 row_array[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
static u8 row_no;
static u8 column_block_no;

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
  
  RCC_PCLK2Config(RCC_HCLK_Div2); 
  
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
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_Init(SPI1, &SPI_InitStructure); /* Configure SPI1 */
  SPI_Cmd(SPI1, ENABLE); /* Enable SPI1 */
  
  memset(led_buffer, 0, sizeof(led_buffer));
  row_no = 0;
  column_block_no = 0;
}

void update_Buffer(void) {
  for(int i = 0; i < ROW_COUNT; i++) {
    setLatchClkPin(0);
    u16 send_data = (led_buffer[i] & 0xFFFF0000) >> 16;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, send_data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    send_data = (led_buffer[i] & 0x0000FFFF);
    SPI_I2S_SendData(SPI1, send_data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    setLatchClkPin(1);
  }
}