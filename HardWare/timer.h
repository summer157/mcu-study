#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f10x.h" // Device header

void Timer_Init(void);
uint8_t Timer_GetAndClear10msFlag(void);
uint8_t Timer_GetAndClear100msFlag(void);
uint8_t Timer_GetAndClear500msFlag(void);
uint32_t Timer_GetMs(void);

#endif
