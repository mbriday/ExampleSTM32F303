#include "stm32f3xx.h"

void EXTI1_IRQHandler()
{
	GPIOB->ODR ^= 1 ; //toggle led
	EXTI->PR |= EXTI_PR_PR1;	//it acknowledge
}

/* One led on pin PB0 (Nucleo D3) 
 * one push button on pin PB1 (Nucleo D6)
 */  
void setup()
{
	//config PB0 as output (Led)
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN_Msk; //clock for GPIOB
	asm("nop");					//wait until GPIOB clock is Ok.
	GPIOB->MODER |= 1 << GPIO_MODER_MODER0_Pos;	//PB0 output


	//config PB1 as input pull-up (push button)
	GPIOB->MODER &= ~GPIO_MODER_MODER1_Msk;	//input
	GPIOB->PUPDR |= 1 << GPIO_PUPDR_PUPDR1_Pos; //pull-up

	//config external interrupt on PB1
	//PBx associated to EXTIx => EXTI1 here
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	asm("nop");
	EXTI->IMR  |= EXTI_IMR_MR1;  //Mask register 1
	EXTI->FTSR |= EXTI_FTSR_TR1; //falling on exti1
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PB; //select port B for exti1
	NVIC_SetPriority(EXTI1_IRQn, 3);	//NVIC config
	NVIC_EnableIRQ(EXTI1_IRQn);
}


/* main function */
int main(void)
{
	setup();
	/* Infinite loop */
	while (1);
}

