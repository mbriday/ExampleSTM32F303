#include "stm32f3xx.h"
#include "can.h"

//basic interrupt that sends a CAN message each 500ms.
void TIM2_IRQHandler()
{
	static uint8_t i = 0;
	CANMsg msg;
	//unused declarations that allows to retrieve easily info in gdb
	volatile CAN_TypeDef * __attribute__((unused)) can=CAN; 
	msg.data[0] = i;
	msg.data[1] = ~i;
	msg.data[2] = 0x55;
	msg.data[3] = 0xaa;
	msg.data[4] = 0x0;
	msg.data[5] = 0xff;
	msg.data[6] = 0xbe;
	msg.data[7] = 0xef;
	msg.len = 8;
	msg.idf = 0x12345;
	CANSendMessage(&msg);

	i++;
	GPIOB->ODR ^= 1<<(3);		//bit toggle (led)
	TIM2->SR &= ~TIM_SR_UIF;	//acknowledge

}

void setup()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; //clock for GPIOB
	asm("nop");					//wait until GPIOB clock is Ok.
	GPIOB->MODER = 1 << (3*2);	//PB3 output

	CANSetup();

	//use TIM2 under interrupt (500ms)
	//input clock = 64MHz.
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	//reset peripheral (mandatory!)
	RCC->APB1RSTR |=  RCC_APB1RSTR_TIM2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;

	asm("nop");
	TIM2->PSC = 64000-1;			//prescaler  : tick@1ms
	TIM2->ARR = 500-1;				//auto-reload: counts 500 ticks
	//TIM2->EGR  |= TIM_EGR_UG;		//update event
	TIM2->DIER |= TIM_DIER_UIE;		//interrupt enable
	TIM2->CR1  |= TIM_CR1_CEN;		//config reg : enable	

	//enable interrupt
	NVIC_SetPriority(TIM2_IRQn, 3);
	NVIC_EnableIRQ(TIM2_IRQn);
}


/* main function */
int main(void)
{
	setup();
	/* Infinite loop */
	while (1)
	{
		//nothing
		asm("nop");
	}
}

