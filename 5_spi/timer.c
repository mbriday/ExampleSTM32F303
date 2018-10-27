#include "timer.h"
#include "stm32f3xx.h"

// configure TIM2 to generate an interrupt
// each 'ms' milliseconds.
// as TIM2 is a 32-bits timer, ms is 32-bits wide.
void setupTIM2Interrupt(unsigned int ms)
{
	if(ms == 0) return;
	//use TIM2 under interrupt
	//input clock = 64MHz.
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	asm("nop");
	//reset peripheral (mandatory!)
	RCC->APB1RSTR |=  RCC_APB1RSTR_TIM2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;

	asm("nop");
	TIM2->PSC = 64000-1;			//prescaler  : tick@1ms
	TIM2->ARR = ms-1;				//auto-reload: counts ms ticks
	TIM2->DIER |= TIM_DIER_UIE;		//interrupt enable
	TIM2->CR1  |= TIM_CR1_CEN;		//config reg : enable	

	//enable interrupt
	NVIC_SetPriority(TIM2_IRQn, 3);
	NVIC_EnableIRQ(TIM2_IRQn);
}


//basic SPI example, based on a I/O extender MCP 23S17
//A led on extended PORTA.0 is toggled under interrupt.
void TIM2_IRQHandler()
{
	TIM2Callback();
	TIM2->SR &= ~TIM_SR_UIF;	//acknowledge

}
