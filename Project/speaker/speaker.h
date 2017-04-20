/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPEAKER_H
#define __SPEAKER_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_lib.h"
#include "stm32f10x.h"

/** 
  * @brief AUDIO DMA definitions  
  */ 
 
#define DAC_DHR8RD1_Address             0x40007410
#define DAC_DLR8RD2_Address             0x4000741C 
#define DAC_DHR8RD_Address              0x40007428

void Audio_Play(void);
void Audio_Pause(void);
void Audio_RePlay(void);
void Audio_Stop(void);

#endif /*__SPEAKER_H*/