#include "stm32f3xx.h"
#include "adc.h"

/* basic example with ADC, polling mode with only 1 channel
 * channel is on pin PA3 <=> ADC1_IN4 <=> arduino name A2.
 * a potentiometer may be plugged to that pin so that voltage can
 * be set between 0 and 3.3V
 */  
void setup()
{
	ADCInit();

}


/* main function */
int main(void)
{
	//for gdb easy access.
	volatile ADC_TypeDef * __attribute((unused)) adc1=ADC1;
	volatile GPIO_TypeDef * __attribute((unused)) gpioa=GPIOA;

	volatile uint16_t __attribute((unused)) res;
	setup();
	/* Infinite loop */
	while (1)
	{
		res = ADCRead(4);
		for(volatile int i=0;i<100;i++);
	}
}

