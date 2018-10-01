#include "stm32f3xx.h"

/* system clock 
 * - use internal HSI RC oscillator => 8MHz
 * - PLL source is HSI/2 => 4MHz
 * - configure PLLx16    => HCLK = 64MHz
 * - flash needs 2 wait states (F>48MHz)
 * - AHB prescaler not set (1)
 * - APB1 and APB2 clocks sets to 64MHz.
 *
 * */
void SystemInit()
{
	// Must set flash latency (2 wait states): F>48MHz
	FLASH->ACR |= 2 << FLASH_ACR_LATENCY_Pos ; // add two wait states
	
	//make sure that HSI is the system clock
	RCC->CFGR &= ~RCC_CFGR_SW_Msk;		// reset SW configuration (HSI).
	
	RCC->CR &= ~(RCC_CR_PLLON);			// turn off PLL
	while(RCC->CR & RCC_CR_PLLRDY);		// wait until pll is stopped

	RCC->CFGR |= (RCC_CFGR_PLLMUL16);	// PLL multiply
	RCC->CR |= RCC_CR_PLLON;			// turn on PLL
	while(!(RCC->CR & RCC_CR_PLLRDY));

	RCC->CFGR |= RCC_CFGR_SW_PLL;		// PLL is system clock
}
