/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
uint32_t EXTI_Line;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


int main(void)
{  
  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
     initialize the PLL and update the SystemFrequency variable. */
  SystemInit();
  
  /* Configure all unused GPIO port pins in Analog Input mode (floating input
     trigger OFF), this will reduce the power consumption and increase the device
     immunity against EMI/EMC *************************************************/

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE |RCC_APB2Periph_GPIOF | 
                         RCC_APB2Periph_GPIOG, ENABLE);  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_Init(GPIOG, &GPIO_InitStructure);  

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | 
                         RCC_APB2Periph_GPIOG, DISABLE);  
  
  /* Global initialization ends above.*/
  /* -------------------------------------------------------------------------- */

  /* Please add your project initialization code below */
 
  //Test Button
  Button_Init();
  //Test REncoder
  REncoder_Init();
  //Test LEDM
  LEDM_Init();
  //Test Clock
  Clock_Init();
  //Test Temp (Temperature sensor)
  Temp_Init();
  //Test Wireless
  Wireless_Init();
  
  //Test LCD
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
                         RCC_APB2Periph_AFIO, ENABLE);
  STM3210E_LCD_Init();
  LCD_Clear();
  u8 testStr[] = "Test LCD";
  LCD_DrawString(0, 0, testStr, sizeof testStr);
  
  //Test SD card
  // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  // NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  // NVIC_Init(&NVIC_InitStructure);
  
  //-------------------------- SD Init -----------------------------
  /*SD_Error Status = SD_Init();
  SD_CardInfo SDCardInfo;

  if (Status == SD_OK)
  {
    //----------------- Read CSD/CID MSD registers ------------------
    Status = SD_GetCardInfo(&SDCardInfo);
  }
  
  if (Status == SD_OK)
  {
    //----------------- Select Card --------------------------------
    Status = SD_SelectDeselect((u32) (SDCardInfo.RCA << 16));
  }
  
  if (Status == SD_OK)
  {
    //Status = SD_EnableWideBusOperation(SDIO_BusWide_4b);
  }
  
  // Set Device Transfer Mode to DMA
  if (Status == SD_OK)
  {  
    //Status = SD_SetDeviceMode(SD_DMA_MODE);//oet
    //Status = SD_SetDeviceMode(SD_POLLING_MODE);
    Status = SD_SetDeviceMode(SD_INTERRUPT_MODE);
	//printf("\r\nTEST OK!\r\n");
  }
  
  LCD_Clear();
  u8 updateStr[] = "SD Init";
  LCD_DrawString(0, 0, updateStr, sizeof updateStr);*/
  
  //Test FatFs
  FATFS fs;
  BYTE FStatus = f_mount(&fs, "0:", 1);
  if (FStatus) {
	LCD_Clear();
	//LCD_DrawChar(0, 0, FStatus);
    u8 updateStr2[] = "SD Mount Error";
	LCD_DrawString(0, 0, updateStr2, sizeof updateStr2);
  } else {
	LCD_Clear();
	u8 updateStr2[] = "SD Mounted";
	LCD_DrawString(0, 0, updateStr2, sizeof updateStr2);
  }
  
  /*FILINFO fno;
  FStatus = f_stat("test1.txt", &fno);
  if (FStatus) {
	LCD_Clear();
	//LCD_DrawChar(0, 0, FStatus);
    u8 updateStr2[] = "Read txt Error";
	LCD_DrawString(0, 0, updateStr2, sizeof updateStr2);
  } else {
	LCD_Clear();
	u8 updateStr2[] = "test1.txt Rd";
	LCD_DrawString(0, 0, updateStr2, sizeof updateStr2);
  }
  
  FStatus = f_stat("test123.txt", &fno);
  if (FStatus) {
	LCD_Clear();
	//LCD_DrawChar(0, 0, FStatus);
    u8 updateStr2[] = "Read txt Error";
	LCD_DrawString(0, 0, updateStr2, sizeof updateStr2);
  } else {
	LCD_Clear();
	u8 updateStr2[] = "test123.txt Rd";
	LCD_DrawString(0, 0, updateStr2, sizeof updateStr2);
  }*/
  
  //DIR dj;         /* Directory search object */
  //FILINFO fno2;
  //FStatus = f_findfirst(&dj, &fno2, "", "*.txt");
  /*LCD_Clear();
  DIR dir;
  FStatus = f_opendir(&dir, "/");                       // Open the directory
  if (FStatus == FR_OK) {
    u8 i = 0;
    do {
      FStatus = f_readdir(&dir, &fno2);                   // Read a directory item
      if (FStatus == FR_OK && fno2.fname[0] != 0) {
        if (fno2.fattrib & AM_DIR) {                    // It is a directory
          //char* dir_name;
          //sprintf(&dir_name, "/%s", fno2.fname);
        } else {                                       // It is a file.
          //printf("%s/%s\n", path, fno2.fname);
		  u8 displayStr[] = "             ";
		  for(int j = 0; j < 13 && fno2.fname[j]!= '\0'; j++) {
			displayStr[j] = (u8)fno2.fname[j];
		  }
		  //u8* displayStr = (u8*) fno2.fname;
		  if(i > 3) {
			LCD_Clear();
			i = 0;
		  }
		  LCD_DrawChar(i * 2, 0, HexValueOffset[i+1]);
		  LCD_DrawString(i * 2, 16, displayStr, sizeof displayStr);
		  i++;
		}
      }
    } while(FStatus == FR_OK && fno2.fname[0] != 0);
    f_closedir(&dir);
  }*/
  
  //Test waveplayer
  //WavePlayer_Start();
  uint32_t FileLen;
  WavePlayerMenu_Start("", "IBIZA.WAV", &FileLen);
  
  LCD_Clear();
  u8 lastStr[] = "Last Display";
  LCD_DrawString(0, 0, lastStr, sizeof lastStr);

  while (1)
  {
  /* Please add your project implementation code below */



  }
}


