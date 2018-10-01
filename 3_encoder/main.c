#include "stm32f3xx.h"

/* first simulate an encoder input
 * - use TIM3 to generate an interrupt @10Hz
 * - use 2 GPIO to generate the encoder outputs (A and B).
 *   GPIOS are D7/D8 (pin 10,11) associated to PF0 and PF1.
 * then, use encoder capability of TIM2 to decode
 * - TIM2_CH1 and TIM2_CH2 are connected to A0/A1 (PA0, PA1)
 * - first, we configure these alternate inputs (GPIOA0/1)
 * - change T1 polarity (to count up)
 * 
 * ** add wires:
 *    -> D8 - A0
 *    -> D7 - A1
 * the TIM2->CNT value should increase at the same rate that TIM3.
 */
void TIM3_IRQHandler()
{
	const int tab[] = {0,1,3,2};
	static int i=0;
	i = (i+1) & 0x3;
	GPIOF->ODR = (GPIOF->ODR & ~0x3) | tab[i]; //simulate encoder
	GPIOB->ODR ^= 1<<(3);		//bit toggle
	TIM3->SR &= ~TIM_SR_UIF;	//acknowledge
}

void setup()
{
	//init led
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; //clock for GPIOB
	asm("nop");					//wait until GPIOB clock is Ok.
	GPIOB->MODER = 1 << GPIO_MODER_MODER3_Pos;	//PB3 output

	//init GPIO to simulate encoder (2 outputs PF0,PF1).
	RCC->AHBENR |= RCC_AHBENR_GPIOFEN; //clock for GPIOF
	asm("nop");					//wait until GPIOF clock is Ok.
	GPIOF->MODER =	1<<GPIO_MODER_MODER0_Pos|	//PF0 output
	                1<<GPIO_MODER_MODER1_Pos;	//PF1 output

	//use TIM3@10Hz with interrupt => encoder simulation
	//input clock = 64MHz.
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	//enable interrupt
	NVIC_SetPriority(TIM3_IRQn, 3);
	NVIC_EnableIRQ(TIM3_IRQn);
	//reset peripheral (mandatory!)
	RCC->APB1RSTR |=  RCC_APB1RSTR_TIM3RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;

	asm("nop");
	TIM3->PSC = 64000-1;			//prescaler  : tick@1ms
	TIM3->ARR = 100-1;				//auto-reload: counts 100 ticks
	TIM3->EGR  |= TIM_EGR_UG;		//update event
	TIM3->DIER |= TIM_DIER_UIE;		//interrupt enable
	TIM3->CR1  |= TIM_CR1_CEN;		//config reg : enable	

	//use TIM2 encoder function
	// TIM2_CH1 is PA0 (pin A0)
	// TIM2_CH2 is PA1 (pin A1)
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //clock for GPIOA
	asm("nop");					//wait until GPIOA clock is Ok.
	//TIM2_CH1 and TIM2_CH2 are alternate function AF1
	//p.40 of datasheet (physical part)
	GPIOA->OTYPER |= GPIO_OTYPER_OT_0|
	                 GPIO_OTYPER_OT_1;			//open drain
	GPIOA->AFR[0] |= 1 << GPIO_AFRL_AFRL0_Pos|
	                 1 << GPIO_AFRL_AFRL1_Pos;	//alternate func is AF1
	GPIOA->MODER |= 2 << GPIO_MODER_MODER0_Pos|
	                2 << GPIO_MODER_MODER1_Pos; //alternate function
	//input clock = 64MHz. Required?
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	//reset peripheral (mandatory!)
	RCC->APB1RSTR |=  RCC_APB1RSTR_TIM2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
	asm("nop");
	TIM2->CCMR1 |= 1 << TIM_CCMR1_CC1S_Pos; //Input Compare 1 on TI1
	TIM2->CCMR1 |= 1 << TIM_CCMR1_CC2S_Pos; //IC2 on TI2
	TIM2->CCER  |= TIM_CCER_CC1P;			//invert TIM2_CH1 polarity (sense)
	TIM2->SMCR  |= 3 << TIM_SMCR_SMS_Pos;	//encoder mode: SMS=011 (mode x4)
	TIM2->CR1   |= TIM_CR1_CEN;				//config reg : enable	
	
}


/* main function */
int main(void)
{
	//2 unused declarations that allows to retrieve easily info in gdb
	volatile TIM_TypeDef *  __attribute__((unused)) tim=TIM2;
	volatile GPIO_TypeDef * __attribute__((unused)) gpioa=GPIOA;
	setup();
	/* Infinite loop */
	while (1)
	{
		//nothing
		asm("nop");
	}
}

