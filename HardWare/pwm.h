#ifndef __PWM_H__
#define __PWM_H__
#include "stm32f10x.h"

void Pwm_Init(void);
void Pwm_SetDuty(uint8_t duty);

#endif
