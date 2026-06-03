#ifndef __APP_H__
#define __APP_H__

#include "stm32f10x.h"                  // Device header
#include "key.h"

typedef enum
{
	APP_MODE_MANUAL = 0,
	APP_MODE_AUTO
} AppMode_t;

void App_Init(void);

void App_Task10ms(void);
void App_Task100ms(void);
void App_Task500ms(void);

void App_HandleKeyEvent(KeyEvent_t event);

void App_SetMode(AppMode_t mode);
AppMode_t App_GetMode(void);

void App_SetManualDuty(uint8_t duty);
uint8_t App_GetCurrentDuty(void);

#endif
