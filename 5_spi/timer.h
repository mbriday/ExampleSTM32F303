#ifndef __TIMER_H__
#define __TIMER_H__

// configure TIM2 to generate an interrupt
// each 'ms' milliseconds.
// as TIM2 is a 32-bits timer, ms is 32-bits wide.
void setupTIM2Interrupt(unsigned int ms);

//user function that is called in the interrupt handler.
void TIM2Callback(void);


#endif
