#include "app.h"
#include "pwm.h"
#include "adc.h"
#include "led.h"
#include "uart.h"
#include "beep.h"

#define ADC_MAX_VALUE 4095u
#define DUTY_TABLE_SIZE  (sizeof(s_duty_table) / sizeof(s_duty_table[0]))

static AppMode_t s_app_mode = APP_MODE_MANUAL;

static const uint8_t s_duty_table[] = {0, 30, 60, 100};
static uint8_t s_duty_index = 0;

static uint8_t s_current_duty = 0;
static uint16_t s_adc_raw = 0;
static uint16_t s_adc_mv = 0;

void App_Init(void)
{
	s_app_mode = APP_MODE_MANUAL;
	s_duty_index = 0;
	s_current_duty = 0;
	s_adc_raw = 0;
	s_adc_mv = 0;
	
	Pwm_SetDuty(s_current_duty);
}

void App_SetMode(AppMode_t mode)
{
	if(mode == APP_MODE_AUTO)
	{
		s_app_mode = APP_MODE_AUTO;
		Uart1_SendString("OK: MODE AUTO\r\n");
	}
	else if(mode == APP_MODE_MANUAL)
	{
		s_app_mode = APP_MODE_MANUAL;
		Uart1_SendString("OK: MODE MANUAL\r\n");
	}
	else
	{
		Uart1_SendString("ERR: INVALID MODE\r\n");
	}
}

void App_HandleKeyEvent(KeyEvent_t event)
{
	if(event == KEY_EVENT_NONE)
	{
		return;
	}
	
	if(event == KEY_EVENT_SHORT)
	{
		if(s_app_mode == APP_MODE_MANUAL)
		{
			s_duty_index++;
			if(s_duty_index >= DUTY_TABLE_SIZE)
			{
				s_duty_index = 0;
			}
			s_current_duty = s_duty_table[s_duty_index];
			Pwm_SetDuty(s_current_duty);
			
			Beep_Trigger(50);
		}
		
		if(s_app_mode == APP_MODE_AUTO)
		{
			Uart1_SendString("AUTO mode: key short ignored\r\n");
			Beep_Trigger(50);
		}
	}
	
	else if(event == KEY_EVENT_LONG)
	{
		if(s_app_mode == APP_MODE_AUTO)
		{
			App_SetMode(APP_MODE_MANUAL);
		}
		else
		{
			App_SetMode(APP_MODE_AUTO);
		}
		
		Beep_Trigger(100);
	}
}

void App_Task10ms(void)
{
	
}

void App_Task100ms(void)
{
	uint8_t duty = 0;
	
	if(s_app_mode != APP_MODE_AUTO)
	{
		return;
	}
	
	s_adc_raw = Adc_ReadAverage(8);
	s_adc_mv = Adc_RawToMv(s_adc_raw);
	
	duty = (uint8_t)((uint32_t)s_adc_raw * 100 / ADC_MAX_VALUE);
	
	s_current_duty = duty;
	Pwm_SetDuty(s_current_duty);
}

void App_Task500ms(void)
{
	Uart1_SendString("MODE=");
	
	if(s_app_mode == APP_MODE_AUTO)
	{
		Uart1_SendString("AUTO");
	}
	else
	{
		Uart1_SendString("MANUAL");
	}
	
	Uart1_SendString("   ADC=");
	Uart1_SendNumber(s_adc_raw);
	
	Uart1_SendString("   ADC_MV=");
	Uart1_SendNumber(s_adc_mv);
	
	Uart1_SendString("   DUTY=");
	Uart1_SendNumber(s_current_duty);
	
	Uart1_SendString("\r\n");
}

void App_SetManualDuty(uint8_t duty)
{
	if(duty > 100)
	{
		duty = 100;
	}
	
	if(s_app_mode != APP_MODE_MANUAL)
	{
		Uart1_SendString("ERR: AUTO mode,switch to MANUAL first\r\n");
		return;
	}
	
	s_current_duty = duty;
	Pwm_SetDuty(s_current_duty);
	
	Uart1_SendString("OK: PWM SET\r\n");
}

AppMode_t App_GetMode(void)
{
	return s_app_mode;
}

uint8_t App_GetCurrentDuty(void)
{
	return s_current_duty;
}
