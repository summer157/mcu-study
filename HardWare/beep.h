#ifndef __BEEP_H__
#define __BEEP_H__
#include "stm32f10x.h"                  // Device header

void Beep_Init(void);
void Beep_On(void);
void Beep_Off(void);
void Beep_Task10ms(void);
void Beep_Trigger(uint16_t time);
#endif
