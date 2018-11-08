#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

//use only ADC1
//potentiometer associated to PA3 <=> ADC1_IN4
//temperature sensor in ADC1_IN16
//VBat/2 in ADC1_IN17
//
//basic configuration: no differential input, single conversion mode.
void ADCInit();

uint16_t ADCRead(int channel);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
