#include "stm32f3xx.h"

void attente()
{
	volatile int i=0;
	for(i=0;i<200000;i++);
}

void setup()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN_Msk; //clock for GPIOB
	asm("nop");					//wait until GPIOB clock is Ok.
	GPIOB->MODER = 1 << (3*2);	//PB3 output
}


/* main function */
int main(void)
{
	setup();
	/* Infinite loop */
	while (1)
	{
		/* Add application code here */
		GPIOB->BSRR = 1<<3; 		//bit set
		attente();
		GPIOB->BSRR = 1<<(3+16); 	//bit reset
		attente();
	}
}

