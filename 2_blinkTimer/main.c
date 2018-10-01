#include "stm32f3xx.h"

void TIM2_IRQHandler()
{
	GPIOB->ODR ^= 1<<(3);		//bit toggle
	TIM2->SR &= ~TIM_SR_UIF;	//acknowledge
}

void setup()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; //clock for GPIOB
	asm("nop");					//wait until GPIOB clock is Ok.
	GPIOB->MODER = 1 << (3*2);	//PB3 output

	//use TIM2@10Hz
	//input clock = 64MHz.
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	//enable interrupt
	NVIC_SetPriority(TIM2_IRQn, 3);
	NVIC_EnableIRQ(TIM2_IRQn);
	//reset peripheral (mandatory!)
	RCC->APB1RSTR |=  RCC_APB1RSTR_TIM2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;

	asm("nop");
	TIM2->PSC = 64000-1;			//prescaler  : tick@1ms
	TIM2->ARR = 100-1;				//auto-reload: counts 100 ticks
	//TIM2->EGR  |= TIM_EGR_UG;		//update event
	TIM2->DIER |= TIM_DIER_UIE;		//interrupt enable
	TIM2->CR1  |= TIM_CR1_CEN;		//config reg : enable	
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

