#include "stm32f3xx.h"
#include "pinAccess.h"

void attente()
{
	volatile int i=0;
	for(i=0;i<200000;i++);
}

void setup()
{
	pinMode(GPIOB,3,OUTPUT);
}


/* main function */
int main(void)
{
	setup();
	/* Infinite loop */
	while (1)
	{
		/* Add application code here */
		digitalToggle(GPIOB,3);
		attente();
	}
}

