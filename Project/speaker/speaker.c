#include "speaker.h"

/* Init Structure definition */
DAC_InitTypeDef            DAC_InitStructure;
DMA_InitTypeDef            DMA_InitStructure;
TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
//extern uint8_t Wavebuffer[512];
uint8_t OutputMode = 0;
extern __IO uint16_t TIM6ARRValue;

__IO uint32_t wavecounter = 0;
uint8_t Wavebuffer[512];

/* Peripherals configuration functions */
static void Audio_Mono_DMA_Config(void);

/**
  * @brief  Configures the DMA/DAC according to the number of channels
  * @param  nb_channel:'1' for mono, '2' for stereo
  * @retval None
  */
void Audio_Play(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
    
  /* Enable peripheral clocks ------------------------------------------------*/
  /* GPIOA Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  
  /* TIM6 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically 
  connected to the DAC converter. In order to avoid parasitic consumption, 
  the GPIO pin should be configured in analog */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* TIM6 Configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = TIM6ARRValue;          
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
     
  /* TIM6 TRGO selection */
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
 
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  
  /* DAC Channel1: 8bit right---------------------------------------------------*/
  /* DAC Channel1 Init */
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
  automatically connected to the DAC converter. */
  DAC_Cmd(DAC_Channel_1, ENABLE);
  /* Enable DMA for DAC Channel1 */
  DAC_DMACmd(DAC_Channel_1, ENABLE);
    
  TIM_Cmd(TIM6, ENABLE); 
  /* Initialize and prepare the DMA Stream in mono case */
  Audio_Mono_DMA_Config();
}

/**
  * @brief  Pauses the audio file stream
  * @param  None
  * @retval None
  */
void Audio_Pause(void)
{  
  TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
  /* Disable TIM6 update interrupt */
  TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
}

/**
  * @brief  Replays the audio file stream
  * @param  None
  * @retval None
  */
void Audio_RePlay(void)
{  
  TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
  /* Enable TIM6 update interrupt */
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
}

/**
  * @brief  Stops the audio file stream
  * @param  None
  * @retval None
  */
void Audio_Stop(void)
{
  /* Disable TIM6 update interrupt */
  TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
  /* Disable TIM6 */
  TIM_Cmd(TIM6, DISABLE);
}

/**
  * @brief   Initialize and prepare the DMA Stream in mono case .
  * @param  None
  * @retval None
  */
static void Audio_Mono_DMA_Config(void)  
{   
  /* Enable the DMA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE); 

  /* Configure the DMA Stream */
  DMA_DeInit(DMA2_Channel3); 
 
  /* Set the parameters to be configured */
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8RD1_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) & Wavebuffer; /* This address will be configured in play function */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 0x200; /* This address will be configured in play function */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_Init(DMA2_Channel3, &DMA_InitStructure);  
  /* Enable DMA2 Channel3 */
  DMA_Cmd(DMA2_Channel3, ENABLE);  
}
