#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f10x.h" // Device header

void Timer_Init(void);
uint8_t Timer_Get10msFlag(void);
void Timer_Clear10msFlag(void);

#endif
