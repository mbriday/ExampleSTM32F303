#include "stm32f3xx.h"
#include "timer.h"
#include "spi.h"

//called by TIM2 interrupt handler.

void TIM2Callback(void)
{
	static int i=0;
	//basic SPI example with an I/O extender (MCP23S17)
	if(i&1) {	//odd
		beginTransaction();
		write8(0x40);	//write
		write8(0x12);	//GPIOA
		write8(0x55);	//set 8 outputs
		endTransaction();
	} else {			//even
		beginTransaction();
		write8(0x40);	//write
		write8(0x12);	//GPIOA
		write8(0xaa);	//set 8 outputs
		endTransaction();
	}
	i++;
}

void setup()
{
	setupSPI();
	
	beginTransaction();
	write8(0x40);	//write
	write8(0x00);	//IODIRA
	write8(0x0);	//config 8 outputs for GPIOA
	endTransaction();

	//callback each 200ms.
	setupTIM2Interrupt(200);
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

