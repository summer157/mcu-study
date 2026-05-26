#ifndef __LED_H__
#define __LED_H__

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

#endif 
