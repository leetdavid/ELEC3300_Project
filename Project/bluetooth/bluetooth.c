/* Includes ------------------------------------------------------------------*/
#include "bluetooth.h"

void Bluetooth_Init(void){

	//Enable USART1 and GPIOA clock

	//Bluetooth_NVIC_Configuration();
	Bluetooth_GPIO_Configuration();
	Bluetooth_USART_Configuration();

	const unsigned char welcome_str[] = " Welcome to Bluetooth!\r\n";
	UARTSend(welcome_str, sizeof(welcome_str));
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports
*******************************************************************************/
void Bluetooth_GPIO_Configuration(void){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART1
*******************************************************************************/
void Bluetooth_USART_Configuration(void){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  USART_InitTypeDef USART_InitStructure;

  USART_Cmd(USART1, ENABLE);
  
/* USART1 configuration ------------------------------------------------------*/
  USART_InitStructure.USART_BaudRate   			= BT_BAUD;        // Baud Rate
  USART_InitStructure.USART_WordLength 			= USART_WordLength_8b;
  USART_InitStructure.USART_StopBits   			= USART_StopBits_1;
  USART_InitStructure.USART_Parity     			= USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode       			= USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(USART1, &USART_InitStructure);

  //Enable the USART1 Receive Interrupt: this interrupt is generated
	// when USART1's data-receive register is not empty
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  
  NVIC_EnableIRQ(USART1_IRQn);
  /* Enable USART1 */
  //USART_Cmd(USART1, ENABLE);
}
  
/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void Bluetooth_NVIC_Configuration(void){
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel 										= USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
* Function Name  : UARTSend
* Description    : Send a string to the UART.
* Input          : - pucBuffer: buffers to be printed.
*                : - ulCount  : buffer's length
*******************************************************************************/
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount){
    //
    // Loop while there are more characters to send.
    //
  while(ulCount--){
    
    /* Loop until the end of transmission */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){
    	//looooooperino
    }
    USART_SendData(USART1, (uint8_t) *pucBuffer++);
  }
}