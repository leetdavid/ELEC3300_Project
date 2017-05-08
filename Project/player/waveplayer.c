/**
  ******************************************************************************
  * @file    waveplayer.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    08-April-2011
  * @brief   This file includes the wave player driver for the STM32100E-EVAL demo.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32100E_EVAL_Demo
  * @{
  */

/** @defgroup WAVEPLAYER
  * @brief    This file includes the wave player driver for the STM32100E-EVAL 
  *           demo.
  * @{
  */

/** @defgroup WAVEPLAYER_Private_Types
  * @{
  */
typedef enum
{
  LittleEndian,
  BigEndian
}Endianness;
/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_Defines
  * @{
  */
#define  CHUNK_ID                            0x52494646  /* correspond to the letters 'RIFF' */
#define  FILE_FORMAT                         0x57415645  /* correspond to the letters 'WAVE' */
#define  FORMAT_ID                           0x666D7420  /* correspond to the letters 'fmt ' */
#define  DATA_ID                             0x64617461  /* correspond to the letters 'data' */
#define  FACT_ID                             0x66616374  /* correspond to the letters 'fact' */
#define  WAVE_FORMAT_PCM                     0x01
#define  FORMAT_CHNUK_SIZE                   0x10

#define	SECTOR_SIZE				512

/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_Variables
  * @{
  */
static WAVE_FormatTypeDef WAVE_Format;
static __IO ErrorCode WaveFileStatus = Unvalid_RIFF_ID;
__IO uint16_t TIM6ARRValue = 1088;
__IO uint32_t WaveDataLength = 0, VolCount = 64;
__IO uint8_t NumChannels = 0;
static __IO uint32_t SpeechDataOffset = 0x00;
static uint32_t wavelen = 0;
extern uint8_t sector[SECTOR_SIZE];
//extern VOLINFO  vi;
extern DIR  di;
extern uint8_t Wavebuffer[512];
uint8_t Wavebuffer2[512];
extern __IO uint32_t wavecounter;
FILINFO fiInfo;
FIL fiwave;
//static uint8_t previoustmp = 64;
extern uint8_t OutPut_Device;
/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_FunctionPrototypes
  * @{
  */
static uint32_t ReadUnit(uint8_t *buffer, uint8_t idx, uint8_t NbrOfBytes, Endianness BytesFormat);
static ErrorCode WavePlayer_WaveParsing(uint8_t *DirName, /*uint8_t*/char *FileName, uint32_t *FileLen);
/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_Functions
  * @{
  */

/**
  * @brief  Wave player Initialization
  * @param  None
  * @retval None
  */
void WavePlayer_Init(void)
{  
  //Audio_Init(OUTPUT_DEVICE_SPEAKER, DEFAULT_VOLUME);
}

/**
  * @brief  Returns the Wave file status.
  * @param  None
  * @retval Wave file status.
  */
ErrorCode Get_WaveFileStatus(void)
{
  return (WaveFileStatus);
}

/**
  * @brief  Start wave playing
  * @param  None
  * @retval None
  */
uint8_t WavePlayer_Start(void)
{
  /* Initialization */
  WavePlayer_Init();

  /* Read the Speech wave file status */
  WaveFileStatus = WavePlayer_WaveParsing("", "IBIZA.WAV", &wavelen);

  if (WaveFileStatus == Valid_WAVE_File) /* the .WAV file is valid */
  {
    /* Set WaveDataLenght to the Speech wave length */
    WaveDataLength = WAVE_Format.DataSize;
    
    TIM_SetAutoreload(TIM6, TIM6ARRValue);

    /* Start TIM6 */
    TIM_Cmd(TIM6, ENABLE);
  }
  else
  {

	  LCD_Clear();
	  u8 lastStr[] = "Invalid WAV";
	  LCD_DrawString(0, 0, lastStr, sizeof lastStr);
    /*LCD_Clear(LCD_COLOR_WHITE);
    LCD_SetBackColor(LCD_COLOR_GREEN);
    LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_DisplayStringLine(LCD_LINE_5, " ERROR: No Wave File");
    LCD_DisplayStringLine(LCD_LINE_6, " Press Joystick to  ");
    LCD_DisplayStringLine(LCD_LINE_7, " exit...            ");
    while (Menu_ReadKey() == NOKEY)
    {}
    LCD_Clear(LCD_COLOR_WHITE);
    Demo_IntExtOnOffConfig(ENABLE);*/
    return 1;
  }
 return 0;
}

/**
  * @brief  Stop wave playing
  * @param  None
  * @retval None
  */
void WavePlayer_Stop(void)
{
  Audio_Stop();
}

/**
  * @brief  Pause wave playing
  * @param  None
  * @retval None
  */
void WavePlayer_RePlay(void)
{
  Audio_RePlay();
}

/**
  * @brief  Pause wave playing
  * @param  None
  * @retval None
  */
void WavePlayer_Pause(void)
{
  Audio_Pause();
}

/**
  * @brief  Decrements the played wave data length.
  * @param  Current value of  WaveDataLength variable
  * @retval None.
  */
void WavePointerUpdate(uint32_t value)
{
  //DFS_Seek(&fiwave, value, Wavebuffer2);
	f_lseek(&fiwave, value);
}

/**
  * @brief  Decrements the played wave data length.
  * @param  None
  * @retval Current value of  WaveDataLength variable.
  */
uint32_t Decrement_WaveDataLength(void)
{
  if (WaveDataLength != 0x00)
  {
    WaveDataLength--;
  }
  return (WaveDataLength);
}


/**
  * @brief  Decrements the played wave data length.
  * @param  None
  * @retval Current value of  WaveDataLength variable.
  */
void Set_WaveDataLength(uint32_t value)
{
  WaveDataLength = value;
}

/**
  * @brief  Checks the format of the .WAV file and gets information about
  *   the audio format. This is done by reading the value of a
  *   number of parameters stored in the file header and comparing
  *   these to the values expected authenticates the format of a
  *   standard .WAV  file (44 bytes will be read). If  it is a valid
  *   .WAV file format, it continues reading the header to determine
  *   the  audio format such as the sample rate and the sampled data
  *   size. If the audio format is supported by this application,
  *   it retrieves the audio format in WAVE_Format structure and
  *   returns a zero value. Otherwise the function fails and the
  *   return value is nonzero.In this case, the return value specifies
  *   the cause of  the function fails. The error codes that can be
  *   returned by this function are declared in the header file.
  * @param  None
  * @retval Zero value if the function succeed, otherwise it return
  *         a nonzero value which specifies the error code.
  */
static ErrorCode WavePlayer_WaveParsing(uint8_t *DirName, /*uint8_t*/char *FileName, uint32_t *FileLen)
{
  uint32_t Temp = 0x00;
  uint32_t ExtraFormatBytes = 0;
  __IO uint32_t err = 0;
  uint32_t number_of_clusters;
  uint32_t i;
  BYTE FStatus;

  LCD_Clear();
  u8 updateStr[] = "WParsing()";
  LCD_DrawString(0, 0, updateStr, sizeof updateStr);
  
  if (f_stat(FileName, &fiInfo) == 0) {
	  *FileLen = fiInfo.fsize;
	  number_of_clusters = fiInfo.fsize / 512;
	  if ((fiInfo.fsize % SECTOR_SIZE) > 0) {
	  	number_of_clusters++;
	  }
  }
	
  FStatus = f_open(&fiwave, FileName, FA_READ|FA_OPEN_EXISTING);
  if(FStatus == FR_OK) {
	  UINT bufferSize = sizeof (Wavebuffer);
	  FStatus = f_read(&fiwave, Wavebuffer, bufferSize/*SECTOR_SIZE*/, &i);
	  //f_close(&fiwave);
  }

  
  /* Read chunkID, must be 'RIFF'  ----------------------------------------------*/
  Temp = ReadUnit(Wavebuffer, 0, 4, BigEndian);
  if (Temp != CHUNK_ID)
  {
    return(Unvalid_RIFF_ID);
  }

  /* Read the file length ----------------------------------------------------*/
  WAVE_Format.RIFFchunksize = ReadUnit(Wavebuffer, 4, 4, LittleEndian);

  /* Read the file format, must be 'WAVE' ------------------------------------*/
  Temp = ReadUnit(Wavebuffer, 8, 4, BigEndian);
  if (Temp != FILE_FORMAT)
  {
    return(Unvalid_WAVE_Format);
  }

  /* Read the format chunk, must be'fmt ' --------------------------------------*/
  Temp = ReadUnit(Wavebuffer, 12, 4, BigEndian);
  if (Temp != FORMAT_ID)
  {
    return(Unvalid_FormatChunk_ID);
  }
  /* Read the length of the 'fmt' data, must be 0x10 -------------------------*/
  Temp = ReadUnit(Wavebuffer, 16, 4, LittleEndian);
  if (Temp != 0x10)
  {
    ExtraFormatBytes = 1;
  }
  /* Read the audio format, must be 0x01 (PCM) -------------------------------*/
  WAVE_Format.FormatTag = ReadUnit(Wavebuffer, 20, 2, LittleEndian);
  if (WAVE_Format.FormatTag != WAVE_FORMAT_PCM)
  {
    return(Unsupporetd_FormatTag);
  }

  /* Read the number of channels, must be 0x01 (Mono) ------------------------*/
  WAVE_Format.NumChannels = ReadUnit(Wavebuffer, 22, 2, LittleEndian);

  if (WAVE_Format.NumChannels == CHANNEL_STEREO)
  {
    NumChannels = 2;
  }
  else if (WAVE_Format.NumChannels == CHANNEL_MONO)
  {
    NumChannels = 1;
  }
  else
  {
    return(Unsupporetd_Number_Of_Channel);
  }
  /* Read the Sample Rate ----------------------------------------------------*/
  WAVE_Format.SampleRate = ReadUnit(Wavebuffer, 24, 4, LittleEndian);
 
  switch (WAVE_Format.SampleRate)
  {
  case SAMPLE_RATE_8000 :
    TIM6ARRValue = 3000;
    break; /* 8KHz = 24MHz / 3000 */
  case SAMPLE_RATE_11025:
    TIM6ARRValue = 2176;
    break; /* 11.025KHz = 24MHz / 2176 */
  case SAMPLE_RATE_22050:
    TIM6ARRValue = 1088;
    break; /* 22.05KHz = 24MHz / 1088 */
  case SAMPLE_RATE_44100:
    //TIM6ARRValue = 544;
    TIM6ARRValue = 1632; /* 44.1kHz = 72MHz / 1632 */
    break; /* 44.1KHz = 24MHz / 544 */
  default:
    return(Unsupporetd_Sample_Rate);
  }
  uint8_t tempDiv = NumChannels;
  TIM6ARRValue = TIM6ARRValue / tempDiv;
  
  /* Read the Byte Rate ------------------------------------------------------*/
  WAVE_Format.ByteRate = ReadUnit(Wavebuffer, 28, 4, LittleEndian);

  /* Read the block alignment ------------------------------------------------*/
  WAVE_Format.BlockAlign = ReadUnit(Wavebuffer, 32, 2, LittleEndian);

  /* Read the number of bits per sample --------------------------------------*/
  WAVE_Format.BitsPerSample = ReadUnit(Wavebuffer, 34, 2, LittleEndian);
  if (WAVE_Format.BitsPerSample != BITS_PER_SAMPLE_8)
  {
    return(Unsupporetd_Bits_Per_Sample);
  }
  SpeechDataOffset = 36;
  /* If there is Extra format bytes, these bytes will be defined in "Fact Chunk" */
  if (ExtraFormatBytes == 1)
  {
    /* Read th Extra format bytes, must be 0x00 ------------------------------*/
    Temp = ReadUnit(Wavebuffer, 36, 2, LittleEndian);
    if (Temp != 0x00)
    {
      return(Unsupporetd_ExtraFormatBytes);
    }
    /* Read the Fact chunk, must be 'fact' -----------------------------------*/
    Temp = ReadUnit(Wavebuffer, 38, 4, BigEndian);
    if (Temp != FACT_ID)
    {
      return(Unvalid_FactChunk_ID);
    }
    /* Read Fact chunk data Size ---------------------------------------------*/
    Temp = ReadUnit(Wavebuffer, 42, 4, LittleEndian);

    SpeechDataOffset += 10 + Temp;
  }
  /* Read the Data chunk, must be 'data' ---------------------------------------*/
  Temp = ReadUnit(Wavebuffer, SpeechDataOffset, 4, BigEndian);
  SpeechDataOffset += 4;
  if (Temp != DATA_ID)
  //if (Temp != DATA_ID && Temp != LIST_ID)
  {
    return(Unvalid_DataChunk_ID);
  }

  /* Read the number of sample data ------------------------------------------*/
  WAVE_Format.DataSize = ReadUnit(Wavebuffer, SpeechDataOffset, 4, LittleEndian);
  SpeechDataOffset += 4;
  wavecounter =  SpeechDataOffset;
  return(Valid_WAVE_File);
}

/**
  * @brief  Start wave playing
  * @param  None
  * @retval None
  */
uint8_t WavePlayerMenu_Start(uint8_t *DirName, /*uint8_t*/char *FileName, uint32_t *FileLen)
{
  uint32_t var;
  //uint8_t tmp;// , KeyState = NOKEY;
 
  LCD_Clear();
  u8 updateStr[] = "WMStart()";
  LCD_DrawString(0, 0, updateStr, sizeof updateStr);

  /* Read the Speech wave file status */
  WaveFileStatus = WavePlayer_WaveParsing(DirName, FileName, &wavelen);
  //Audio_Play(NumChannels);
  Audio_Play();
  
  /* TIM6 TRGO selection */
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
  TIM_SetAutoreload(TIM6, TIM6ARRValue);
 
  if (WaveFileStatus == Valid_WAVE_File)  /* the .WAV file is valid */
  {
    /* Set WaveDataLenght to the Speech wave length */
    WaveDataLength = WAVE_Format.DataSize;
  }
  else
  {
	  return 0; //NOKEY;
  }
  
  /* Start TIM6 */
  TIM_Cmd(TIM6, ENABLE);
    
  /*LCD_Clear();
  u8 updateStr2[] = "WMStart() Loop";
  LCD_DrawString(0, 0, updateStr2, sizeof updateStr2);*/

  while (WaveDataLength)
  {   
    //DFS_ReadFile(&fiwave, sector, Wavebuffer, &var, SECTOR_SIZE);
    BYTE FStatus = f_read(&fiwave, Wavebuffer, SECTOR_SIZE, &var);
   
    if (WaveDataLength) WaveDataLength -= 512;
    if (WaveDataLength < 512) WaveDataLength = 0;
  
    while (DMA_GetFlagStatus(DMA2_FLAG_TC3) == RESET)
    {     
    //  tmp = (uint8_t) ((uint32_t)((WAVE_Format.DataSize - WaveDataLength) * 100) / WAVE_Format.DataSize);
    //  /*LCD_SetTextColor(LCD_COLOR_MAGENTA);
    //  LCD_DrawLine(LCD_LINE_7, 310 - ((tmp) * 3), 16, LCD_DIR_VERTICAL); */     
    }
    
    DMA2->IFCR = DMA2_FLAG_TC3;
    DMA2_Channel3->CCR = 0x0;
      
    /*KeyState = LCD_Update();
    
    if (KeyState == DOWN)
    {
      return DOWN;
    }*/
    
    if (NumChannels == 2)
    {    
      DMA2_Channel3->CPAR = DAC_DHR8RD_Address;
    }
    else if (NumChannels == 1)
    {
      DMA2_Channel3->CPAR = DAC_DHR8RD1_Address;
    }
    
    DMA2_Channel3->CNDTR = 0x200;
    DMA2_Channel3->CMAR = (uint32_t) & Wavebuffer;
    DMA2_Channel3->CCR = 0x2091;
    
    //DFS_ReadFile(&fiwave, sector, Wavebuffer2, &var,SECTOR_SIZE);
    FStatus = f_read(&fiwave, Wavebuffer2, SECTOR_SIZE, &var);
    
    if (WaveDataLength) WaveDataLength -= 512;
    if (WaveDataLength < 512) WaveDataLength = 0;

    while (DMA_GetFlagStatus(DMA2_FLAG_TC3) == RESET)
    {
    //  tmp = (uint8_t) ((uint32_t)((WAVE_Format.DataSize - WaveDataLength) * 100) / WAVE_Format.DataSize);
    //  /*LCD_SetTextColor(LCD_COLOR_MAGENTA);
    //  LCD_DrawLine(LCD_LINE_7, 310 - ((tmp) * 3), 16, LCD_DIR_VERTICAL); */ 
    }
        
    DMA2->IFCR = DMA2_FLAG_TC3;
    DMA2_Channel3->CCR = 0x0;

    /*KeyState = LCD_Update();
    
    if (KeyState == DOWN)
    {
      return DOWN;
    }*/
    
    if (NumChannels == 2)
    {
      DMA2_Channel3->CPAR = DAC_DHR8RD_Address;
    }
    else if (NumChannels == 1)
    {
      DMA2_Channel3->CPAR = DAC_DHR8RD1_Address;    
    }
    
    DMA2_Channel3->CNDTR = 0x200;
    DMA2_Channel3->CMAR = (uint32_t) & Wavebuffer2;
    DMA2_Channel3->CCR = 0x2091;
  }

  /*LCD_Clear();
  u8 updateStr3[] = "WMStart() Exit";
  LCD_DrawString(0, 0, updateStr3, sizeof updateStr3);*/
   
  f_close(&fiwave);

  DMA2_Channel3->CCR = 0x0;
  
  /* Disable TIM6 */
  TIM_Cmd(TIM6, DISABLE);
  WaveDataLength = 0;

  return 0;// NOKEY;
}

/**
  * @brief  Reads a number of bytes from the SPI Flash and reorder them in Big 
  *         or little endian.
  * @param  NbrOfBytes: number of bytes to read.
  *         This parameter must be a number between 1 and 4.
  * @param  ReadAddr: external memory address to read from.
  * @param  Endians: specifies the bytes endianness.
  *         This parameter can be one of the following values:
  *             - LittleEndian
  *             - BigEndian
  * @retval Bytes read from the SPI Flash.
  */
static uint32_t ReadUnit(uint8_t *buffer, uint8_t idx, uint8_t NbrOfBytes, Endianness BytesFormat)
{
  uint32_t index = 0;
  uint32_t Temp = 0;
  
  for (index = 0; index < NbrOfBytes; index++)
  {
    Temp |= buffer[idx + index] << (index * 8);
  }
  
  if (BytesFormat == BigEndian)
  {
    Temp = __REV(Temp);
  }
  return Temp;
}
/**
  * @brief  Controls the wave player application LCD display messages
  * @param  None.
  * @retval None.
  */
uint8_t LCD_Update(void)
{ 
  //uint8_t KeyState = NOKEY;
  //uint32_t tmp = 0, index = 0;

  //KeyState = Menu_ReadKey();
  //if (KeyState == SEL)
  //{
  //  DMA2_Channel3->CCR = 0x0;

  //  /* Disable TIM6 */
  //  TIM_Cmd(TIM6, DISABLE);
  //  
  //  /* Set the text and the background color */
  //  /*LCD_SetBackColor(LCD_COLOR_BLUE);
  //  LCD_SetTextColor(LCD_COLOR_WHITE);
  //  LCD_DisplayStringLine(LCD_LINE_3, "SEL  -> Play        ");
  //  LCD_DisplayStringLine(LCD_LINE_4, "DOWN -> Exit        ");
  //  LCD_SetBackColor(LCD_COLOR_WHITE); 
  //  LCD_ClearLine(LCD_LINE_2);
  //  LCD_ClearLine(LCD_LINE_5);
  //  LCD_SetTextColor(LCD_COLOR_RED);
  //  LCD_SetBackColor(LCD_COLOR_BLUE);
  //  LCD_DisplayStringLine(LCD_LINE_6, "       Paused       ");*/

  //  /*KeyState = Menu_ReadKey();

  //  while ((KeyState != SEL) && (KeyState != DOWN))
  //  {
  //    KeyState = Menu_ReadKey();
  //  }*/
  //  
  //  if (KeyState == SEL)
  //  {
  //    DMA2_Channel3->CCR = 0x0;
  //    
  //    /* Enable TIM6 */
  //    TIM_Cmd(TIM6, ENABLE);
  //    return KeyState;
  //  }
  //}

  //if (KeyState == DOWN)
  //{ 
  //  return KeyState;
  //}
  //if (KeyState == LEFT)
  //{
  //  tmp = fiwave.pointer; 
  //  if(tmp > (fiwave.filelen/20))
  //  {
  //    tmp -= fiwave.filelen/20;
  //  }
  //  else
  //  {
  //    tmp = 0;
  //  }
  //  DFS_Seek(&fiwave, tmp, sector);
  //  
  //  WaveDataLength += (WAVE_Format.DataSize/20);
  //  if(WaveDataLength > WAVE_Format.DataSize)
  //  {
  //    WaveDataLength = WAVE_Format.DataSize;
  //  }
  //  
  //  /*LCD_ClearLine(LCD_LINE_7);    
  //  LCD_SetTextColor(LCD_COLOR_MAGENTA);*/
  //  /* Set the Back Color */
  //  /*LCD_SetBackColor(LCD_COLOR_BLUE);
  //  LCD_DrawRect(LCD_LINE_7, 310, 16, 300);   */
  //  
  //  tmp = (uint8_t) ((uint32_t)((WAVE_Format.DataSize - WaveDataLength) * 100) / WAVE_Format.DataSize);
  //  /*LCD_SetTextColor(LCD_COLOR_MAGENTA);
  //  for(index = 0; index < tmp; index++)
  //  {  
  //    LCD_DrawLine(LCD_LINE_7, 310 - ((index) * 3), 16, LCD_DIR_VERTICAL);  
  //  }*/
  //} 
  //if (KeyState == RIGHT)
  //{
  //  tmp = fiwave.pointer; 
  //  tmp += fiwave.filelen/20;
  //  if(tmp >  fiwave.filelen)
  //  {
  //    tmp = fiwave.filelen;
  //  }
  //  
  //  DFS_Seek(&fiwave, tmp, sector);
  //  
  //  if(WaveDataLength < (WAVE_Format.DataSize/20))
  //  {
  //    WaveDataLength = 0;
  //  }
  //  else
  //  {
  //    WaveDataLength -= (WAVE_Format.DataSize/20);
  //  }
  //  
  //  /*LCD_ClearLine(LCD_LINE_7);
  //  LCD_SetTextColor(LCD_COLOR_MAGENTA);*/
  //  
  //  /* Set the Back Color */
  //  L/*CD_SetBackColor(LCD_COLOR_BLUE);
  //  
  //  LCD_DrawRect(LCD_LINE_7, 310, 16, 300); */
  //  
  //  tmp = (uint8_t) ((uint32_t)((WAVE_Format.DataSize - WaveDataLength) * 100) / WAVE_Format.DataSize);
  //  /*LCD_SetTextColor(LCD_COLOR_MAGENTA);
  //  for(index = 0; index < tmp; index++)
  //  {  
  //    LCD_DrawLine(LCD_LINE_7, 310 - ((index) * 3), 16, LCD_DIR_VERTICAL);  
  //  }*/
  //} 
  //
  //if (KeyState == KEY)
  //{
  //  VolCount += 5;
  //  
  //  if(VolCount > 95)
  //  {
  //    VolCount = 95;
  //  }
  //  
  //  /* Save the current position */
  //  previoustmp = VolCount;      
  //}
  ///* Change volume level */
  //if(KeyState == TAMPER)
  //{      
  //  if (VolCount < 10)
  //  {
  //    VolCount = 10;
  //  }
  //  VolCount -= 5;
  //  
  //  /* Save the current position */
  //  previoustmp = VolCount;    
  //}  
  //return NOKEY;
return 0;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
