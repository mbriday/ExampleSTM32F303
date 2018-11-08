#include "adc.h"
#include "stm32f3xx.h"


void ADCInit()
{
	volatile ADC_TypeDef * __attribute((unused)) adc1=ADC1;
	volatile GPIO_TypeDef * __attribute((unused)) gpioa=GPIOA;
	volatile RCC_TypeDef * __attribute((unused)) rcc=RCC;
	//specific for channel ADC1_IN4 <-> PA3
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //clock for GPIOA
	asm("nop");					//wait until GPIOx clock is Ok.
	GPIOA->MODER |= 3 << GPIO_MODER_MODER3_Pos; //analog config
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR3_Msk;

	//1 - input clock = 64MHz.
	//RCC->CFGR2 &= ~RCC_CFGR2_ADCPRE12_Msk;
	RCC->CFGR2 |=RCC_CFGR2_ADCPRE12_DIV1;
	RCC->AHBENR |= RCC_AHBENR_ADC12EN;
	asm("nop");
	//reset peripheral (mandatory!)
	RCC->AHBRSTR |=  RCC_AHBRSTR_ADC12RST;
	RCC->AHBRSTR &= ~RCC_AHBRSTR_ADC12RST;

	ADC12_COMMON->CCR &= ~ADC12_CCR_CKMODE;
	//ADC12_COMMON->CCR |= 1 << ADC12_CCR_CKMODE_Pos;
	//timings: SMP=100 => 19.5 ADC clock cycles.
	//total: sampling time + 12.5
	//set same config for all channels.
	ADC1->SMPR1 = 6 << ADC_SMPR1_SMP1_Pos |
	              6 << ADC_SMPR1_SMP2_Pos |
	              6 << ADC_SMPR1_SMP3_Pos |
	              6 << ADC_SMPR1_SMP4_Pos |
	              6 << ADC_SMPR1_SMP5_Pos |
	              6 << ADC_SMPR1_SMP6_Pos |
	              6 << ADC_SMPR1_SMP7_Pos |
	              6 << ADC_SMPR1_SMP8_Pos |
	              6 << ADC_SMPR1_SMP9_Pos ;

	ADC1->SMPR2 = 6 << ADC_SMPR2_SMP10_Pos |
	              6 << ADC_SMPR2_SMP11_Pos |
	              6 << ADC_SMPR2_SMP12_Pos |
	              6 << ADC_SMPR2_SMP13_Pos |
	              6 << ADC_SMPR2_SMP14_Pos |
	              6 << ADC_SMPR2_SMP15_Pos |
	              6 << ADC_SMPR2_SMP16_Pos |
	              6 << ADC_SMPR2_SMP17_Pos |
	              6 << ADC_SMPR2_SMP18_Pos ;

	//calibration procedure (p.318)
	//1- ADVREGEN should be set to 01
	ADC1->CR &= ~ADC_CR_ADVREGEN_Msk;
	ADC1->CR |= 1 << ADC_CR_ADVREGEN_Pos;
	//1b- wait for ADC stabilization time. 10us in worst case
	//a 1 000 000 loop requires 11 000 000 ticks with a 64MHz timer, with optim -O3
	//=> We need 640 ticks => ~ 60 loops. Let's say 100!
	for(volatile int i=0; i< 100;i++);
	
	//2- ADEN should be set to 0
	ADC1->CR &= ~ADC_CR_ADEN;
	//3- ADCALDIF=0 (no differential input)
	ADC1->CR &= ~ADC_CR_ADCALDIF;
	//4- ADCAL=1
	ADC1->CR |= ADC_CR_ADCAL;
	//5- wait until ADCAL = 0
	while(ADC1->CR & ADC_CR_ADCAL);
	//6- calibration is in ADC1_CALFACT register.
	
	//start ADC (p.320)
	//make sure ADRDY is 0 (writing a 1)
	ADC1->ISR |= ADC_ISR_ADRDY;
	//1- set ADEN
	ADC1->CR |= ADC_CR_ADEN;
	//2- wait until ADRDY = 1
	while(!(ADC1->ISR & ADC_ISR_ADRDY));

}

uint16_t ADCRead(int channel)
{
	volatile ADC_TypeDef * __attribute((unused)) adc1=ADC1;
	volatile GPIO_TypeDef * __attribute((unused)) gpioa=GPIOA;
	uint16_t result = 0;
	if(channel > 18 || channel == 0) return 0;
	//set channel (only one - no mask).
	ADC1->SQR1 = channel << ADC_SQR1_SQ1_Pos;
	//reset End Of Convertion flag (writing a 1)
	ADC1->ISR |= ADC_ISR_EOC;
	//start conversion
	ADC1->CR |= ADC_CR_ADSTART;
	//EOC flag: End Of Conversion
	while(!(ADC1->ISR & ADC_ISR_EOC));
	//result in ADC1->DR
	result = ADC1->DR;
	return result;
}
