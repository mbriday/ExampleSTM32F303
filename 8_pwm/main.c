#include "stm32f3xx.h"

volatile int duty = 0;

void TIM6_DAC1_IRQHandler()
{
	static int sens = 1;
	if(sens) {
		duty++;
		if(duty == 99) sens = 0;
	} else {
		duty--;
		if(duty == 0) sens = 1;
	}
	TIM2->CCR2 = duty;
	TIM6->SR &= ~TIM_SR_UIF;	//acknowledge
}

//D3  => PB0 => TIM3_CH3
//D13 => PB3 => TIM2_CH2

void setup()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; //clock for GPIOB
	asm("nop");					//wait until GPIOB clock is Ok.
	//select alternate function
	GPIOB->MODER &= ~GPIO_MODER_MODER3_Msk;
	GPIOB->MODER |= 2 << GPIO_MODER_MODER3_Pos;
	//alternate function is 1
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFRL3_Msk;
	GPIOB->AFR[0] |= 1 << GPIO_AFRL_AFRL3_Pos;

	//use TIM2@1KHz
	//input clock = 64MHz.
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	//reset peripheral (mandatory!)
	RCC->APB1RSTR |=  RCC_APB1RSTR_TIM2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;

	//config timer@1KHz, with 100 ticks (duty cycle at 1%)
	TIM2->PSC = 640-1;					//prescaler  : tick@10us
	TIM2->ARR = 100-1;					//auto-reload: counts 100 ticks => 1ms

	TIM2->CCMR1 &= ~TIM_CCMR1_CC2S_Msk;	//channel 2 as output
	TIM2->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;
	TIM2->CCMR1 |= 6 << TIM_CCMR1_OC2M_Pos;	//output PWM mode 1
	TIM2->CCMR1 |= TIM_CCMR1_OC2PE;			//pre-load register TIM2_CCR2

	TIM2->CR1 &= ~TIM_CR1_CMS_Msk;  //mode 1 // edge aligned mode
	TIM2->CR1  |= TIM_CR1_CEN;		//config reg : enable	
	TIM2->CCER |= TIM_CCER_CC2E;	//enable

	//use a simple timer to change width value.
	//use TIM6@100Hz
	//input clock = 64MHz.
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	//enable interrupt
	NVIC_SetPriority(TIM6_DAC1_IRQn, 3);
	NVIC_EnableIRQ(TIM6_DAC1_IRQn);
	//reset peripheral (mandatory!)
	RCC->APB1RSTR |=  RCC_APB1RSTR_TIM6RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST;

	asm("nop");
	TIM6->PSC = 64000-1;			//prescaler  : tick@1ms
	TIM6->ARR = 10-1;				//auto-reload: counts 10 ticks
	TIM6->DIER |= TIM_DIER_UIE;		//interrupt enable
	TIM6->CR1  |= TIM_CR1_CEN;		//config reg : enable	
}


/* main function */
int main(void)
{
	setup();
	/* Infinite loop */
	while (1)
	{
		//nothing
	}
}

