/* Includes ------------------------------------------------------------------*/
#include "bluetooth.h"


//UART connection
static __IO uint32_t TimingDelay;
void UARTSend(const unsigned char * pucBuffer, unsigned long ulCount);
volatile char received_string[MAX_STRLEN+1];

void Bluetooth_Init(void) {
  BLUETOOTH_Init_USART1(BT_BAUD);
}

void BLUETOOTH_Init_USART1(uint32_t baudrate){
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // Pins 6 (TX) and 7 (RX) are used
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;           // the pins are configured as alternate function so the USART peripheral has access to them
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;      // this defines the IO speed and has nothing to do with the baudrate!
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;	      // this defines the output type as push pull mode (as opposed to open drain)
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;	      // this activates the pullup resistors on the IO pins
  
  GPIO_Init(GPIOB, &GPIO_InitStruct);                 // now all the values are passed to the GPIO_Init() function which sets the GPIO registers
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); //
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
  
  USART_InitStruct.USART_BaudRate = baudrate;				// the baudrate is set to the value we passed into this init function
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;// we want the data frame size to be 8 bits (standard)
  USART_InitStruct.USART_StopBits = USART_StopBits_1;		// we want 1 stop bit (standard)
  USART_InitStruct.USART_Parity = USART_Parity_No;		// we don't want a parity bit (standard)
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
  USART_Init(USART1, &USART_InitStruct);					// again all the properties are passed to the USART_Init function which takes care of all the bit setting
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt
  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		 // we want to configure the USART1 interrupts
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 // this sets the subpriority inside the group
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 // the USART1 interrupts are globally enabled
  NVIC_Init(&NVIC_InitStructure);							 // the properties are passed to the NVIC_Init function which takes care of the low level stuff
  
  // finally this enables the complete USART1 peripheral
  USART_Cmd(USART1, ENABLE);
}

void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount){
  while(ulCount--){
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, (uint8_t)*pucBuffer++);
  }
}

/*- Interruption handler -----------------------------------------------------*/

// this is the interrupt request handler (IRQ) for ALL USART1 interrupts
void USART1_IRQHandler(void){
  
  // check if the USART1 receive interrupt flag was set
  if( USART_GetITStatus(USART1, USART_IT_RXNE) ){
    
    static uint8_t cnt = 0; // this counter is used to determine the string length
    char t = USART1->DR; // the character from the USART1 data register is saved in t
    
    /* check if the received character is not the LF character (used to determine end of string)
    * or the if the maximum string length has been been reached
    */
    if( (t != 'n') && (cnt < MAX_STRLEN) ){
      received_string[cnt] = t;
      cnt++;
    }
    else{ // otherwise reset the character counter
      cnt = 0;
    }
  }
}