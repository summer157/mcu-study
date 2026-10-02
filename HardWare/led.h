#ifndef __LED_H__
#define __LED_H__
#include "stm32f10x.h"

typedef enum
{
    LED_RED = 0,
    LED_GREEN,
    LED_BLUE
} LED;

void Led_Init(void);
void Led_On(LED led);
void Led_Off(LED led);
void Led_Toggle(LED led);

void Led_Task10ms(void);
void Led_BlinkStart(LED led,uint16_t interval_ms,uint16_t times);
void Led_BlinkStop(void);

#endif 
