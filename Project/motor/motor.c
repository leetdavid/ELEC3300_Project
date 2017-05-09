/* Includes ------------------------------------------------------------------*/
#include "motor.h"

void Motor_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(GPIOB, &GPIO_InitStructure);

	//TIM_TimeBaseStructure.TIM_Period = 7199; // TIMx_ARR register
	//TIM_TimeBaseStructure.TIM_Prescaler = 0;
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	//TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	//TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM_OCInitStructure.TIM_Pulse = 2159;  // TIMx_CCRx register
	//TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//TIM_OC1Init(TIM3, &TIM_OCInitStructure);

	//TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	//TIM_ARRPreloadConfig(TIM3, ENABLE);

	//TIM_Cmd(TIM3, ENABLE);

	setMotorSwitchPin(0);
}

void setMotorSwitchPin(u8 value) {
	if (value > 0) {
		//GPIOC->BSRR = GPIOC->IDR | GPIO_Pin_5;
		GPIOB->BSRR = GPIO_Pin_0;
	}
	else {
		//GPIOC->BSRR = GPIOC->IDR & (~GPIO_Pin_5);
		GPIOB->BRR = GPIO_Pin_0;
	}
}