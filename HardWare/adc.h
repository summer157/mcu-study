#ifndef __ADC_H__
#define __ADC_H__
#include "stm32f10x.h"                  // Device header

void Adc_Init(void);
uint16_t Adc_ReadRaw(void);
uint16_t Adc_ReadAverage(uint8_t times);
uint16_t Adc_RawToMv(uint16_t raw);

#endif
