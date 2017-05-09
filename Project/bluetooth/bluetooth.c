/* Includes ------------------------------------------------------------------*/
#include "bluetooth.h"
#include "lcd.h"
#include "ledm.h"
#include "motor.h"
#include "waveplayer.h"
#include "clock.h"

u8 remain_value;
u8 remain_mode;
u8 rx_time[4];

void Bluetooth_Init(void){

	remain_value = 0;
	remain_mode = 0;
	for (int i = 0; i < 4; i++) {
		rx_time[i] = 0;
	}

	//Enable USART1 and GPIOA clock

	Bluetooth_GPIO_Configuration();
	Bluetooth_USART_Configuration();
  Bluetooth_NVIC_Configuration();

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
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
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
  NVIC_InitStructure.NVIC_IRQChannel                    = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
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



/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
* @brief  This function handles USARTx global interrupt request
* @param  None
* @retval None
*/
void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		USART_SendData(USART1, 'T');
		if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET) {
			char bt_input = USART_ReceiveData(USART1);
			if (bt_input == '\n' || bt_input == 0x0D) {
				// do nothing
			} else {
				if (remain_value == 0) {
					if (bt_input == 0x11) {
						setDisplayMode(0);
					}
					if (bt_input == 't' || bt_input == 'T') {
						remain_value = 4;
						remain_mode = 1;
					}
					else if (bt_input == 'y' || bt_input == 'Y') {
						setMotorSwitchPin(1);
					}
					else if (bt_input == 'z' || bt_input == 'Z') {
						setMotorSwitchPin(0);
					}
					else if (bt_input == 'p' || bt_input == 'P') {
						WavePlayer_Pause();
						//WavePlayer_Stop();
					}
					else if (bt_input == 'q' || bt_input == 'Q') {
						WavePlayer_RePlay();
					}
					else if (bt_input == 'a' || bt_input == 'A') {
						//u8 its0x20[] = "It's 0x20!";
						//LCD_DrawString(0, 0, its0x20, sizeof its0x20);
						//setDisplayIcon(0);
						//UARTSend("Icon Mode\r\n", sizeof("Icon Mode\r\n"));    // Send message to UART1
						remain_mode = 2;
						remain_value = 4;
					}
					else if (bt_input == 'd' || bt_input == 'D') {
						Alarm_disable();
					}
					else if (bt_input == 0x21) {
						setDisplayIcon(1);
					}
					else if (bt_input == 'K' || bt_input == 'k'/*0x22*/) {
						setDisplayIcon(2);
						updateDisplay();
						halt_display(3);
					}
					else if (bt_input == 0x23) {
						setDisplayIcon(3);
					}
					else {
						u8 datarcvd[] = "BT Data Received";
						LCD_DrawString(0, 0, datarcvd, sizeof datarcvd);
					}
				}
				else {
					rx_time[4 - remain_value] = bt_input - 0x30;
					remain_value--;
					if (remain_value == 0) {
						if (remain_mode == 1) {
							Time_Set(rx_time[0], rx_time[1], rx_time[2], rx_time[3]);
							//setDisplayTime(rx_time[0],rx_time[1],rx_time[2],rx_time[3]);
							/*uint32_t rTime = getRawTime();
							uint32_t THH = rTime / 3600;
							uint32_t TMM = (rTime % 3600) / 60;
							setDisplayTime(THH / 10, THH % 10, TMM / 10, TMM % 10);*/
							Clock_UpdateValues();
							setDisplayTime(h1, h2, m1, m2);
							updateDisplay();
							Show_time_En();
						}
						else if (remain_mode == 2) {
							Alarm_Set(rx_time[0], rx_time[1], rx_time[2], rx_time[3]);
						}
					}
				}
			}
		}
	}
}