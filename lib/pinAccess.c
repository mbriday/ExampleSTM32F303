#include "pinAccess.h"

void clockForGpio(GPIO_TypeDef *port)
{
	if(port == GPIOA) RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	else if(port == GPIOB) RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	else if(port == GPIOC) RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	else if(port == GPIOD) RCC->AHBENR |= RCC_AHBENR_GPIODEN;
	//no GPIOE on stm303
	//else if(port == GPIOE) RCC->AHBENR |= RCC_AHBENR_GPIOEEN; 
	else if(port == GPIOF) RCC->AHBENR |= RCC_AHBENR_GPIOFEN;
	asm("nop");	//make sure clock is ok.
}

int pinMode(GPIO_TypeDef *port, 
            unsigned char numBit,
            unsigned char mode)
{
	int mask2Bits; //mask for 2bit fields
	//check arguments
	if(!IS_GPIO_ALL_INSTANCE(port)) return -1;
	if(numBit > 15) return -1;
	//
	clockForGpio(port);
	mask2Bits = (3 << (numBit*2)); 
	switch(mode)
	{
		case DISABLE: //MODER = 0, PUPDR = 0
			port->MODER &= ~mask2Bits;
			port->PUPDR &= ~mask2Bits;
			break;
		case OUTPUT: //MODER = 1, PUPDR = 0
			port->MODER &= ~mask2Bits;
			port->MODER |= (1<<(numBit*2));
			port->PUPDR &= ~mask2Bits;
			break;
		case INPUT: //MODER = 0, PUPDR = 0
			port->MODER &= ~mask2Bits;
			port->PUPDR &= ~mask2Bits;
			break;
		case INPUT_PULLUP: //MODER = 0, PUPDR = 1
			port->MODER &= ~mask2Bits;
			port->PUPDR &= ~mask2Bits;
			port->PUPDR |= (1<<(numBit*2));
			break;
		case INPUT_PULLDOWN: //MODER = 0, PUPDR = 3
			port->MODER &= ~mask2Bits;
			port->PUPDR &= ~mask2Bits;
			port->PUPDR |= (1<<((numBit*2)+1));
			break;
		default:
			break;
	}
	return 0;
}

void digitalWrite(GPIO_TypeDef *port, 
                  unsigned char numBit,
                  unsigned int value)
{
	if(!IS_GPIO_ALL_INSTANCE(port)) return;
	if(numBit > 15) return;
	
	if(value) port->BSRR = 1 << numBit;
	else      port->BSRR = 1 << (numBit+16);
}

void digitalToggle(GPIO_TypeDef *port, 
                  unsigned char numBit)
{
	if(!IS_GPIO_ALL_INSTANCE(port)) return;
	if(numBit > 15) return;

	if(port->IDR & (1<<numBit)) //if is set
		port->BSRR = 1 << (numBit+16);	//reset
	else
		port->BSRR = 1 << numBit;		//set
}


unsigned char digitalRead(GPIO_TypeDef *port,
                          unsigned char numBit)
{
	if(!IS_GPIO_ALL_INSTANCE(port)) return 0xFF;
	if(numBit > 15) return 0xFF;
	if(port->IDR & (1<<numBit)) //if is set
		return 1;
	return 0;
}

