#include "led.h"

#define LED_R_PORT GPIOB
#define LED_G_PORT GPIOB
#define LED_B_PORT GPIOB

#define LED_R_PIN GPIO_Pin_5
#define LED_G_PIN GPIO_Pin_0
#define LED_B_PIN GPIO_Pin_1

typedef enum
{
	LED_BLINK_IDLE = 0,
	LED_BLINK_ON,
	LED_BLINK_OFF
} LedBlinkState_t;

static LedBlinkState_t s_blink_state = LED_BLINK_IDLE;
static LED s_blink_led;
static uint16_t s_blink_interval_ms = 0;
static uint16_t s_blink_tick_ms = 0;
static uint16_t s_blink_times = 0;
static uint8_t s_blink_infinite = 0;


void Led_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

    GPIO_InitStructure.GPIO_Pin = LED_R_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_R_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LED_G_PIN;
    GPIO_Init(LED_G_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LED_B_PIN;
    GPIO_Init(LED_B_PORT, &GPIO_InitStructure);

    GPIO_SetBits(LED_R_PORT,LED_R_PIN);
    GPIO_SetBits(LED_G_PORT,LED_G_PIN);
    GPIO_SetBits(LED_B_PORT,LED_B_PIN);
}

void Led_On(LED led)
{
    if(led == LED_RED)
    {
        GPIO_ResetBits(LED_R_PORT,LED_R_PIN);
    }

    if(led == LED_GREEN)
    {
        GPIO_ResetBits(LED_G_PORT,LED_G_PIN);
    }

    if(led == LED_BLUE)
    {
        GPIO_ResetBits(LED_B_PORT,LED_B_PIN);
    }
}

void Led_Off(LED led)
{
    if(led == LED_RED)
    {
        GPIO_SetBits(LED_R_PORT,LED_R_PIN);
    }

    if(led == LED_GREEN)
    {
        GPIO_SetBits(LED_G_PORT,LED_G_PIN);
    }

    if(led == LED_BLUE)
    {
        GPIO_SetBits(LED_B_PORT,LED_B_PIN);
    }
}

void Led_Toggle(LED led)
{
    if(led == LED_RED)
    {
        if(GPIO_ReadOutputDataBit(LED_R_PORT,LED_R_PIN))
        {
            GPIO_ResetBits(LED_R_PORT,LED_R_PIN);
        }
        else
        {
            GPIO_SetBits(LED_R_PORT,LED_R_PIN);
        }
    }

    if(led == LED_GREEN)
    {
        if(GPIO_ReadOutputDataBit(LED_G_PORT,LED_G_PIN))
        {
            GPIO_ResetBits(LED_G_PORT,LED_G_PIN);
        }
        else
        {
            GPIO_SetBits(LED_G_PORT,LED_G_PIN);
        }
    }

    if(led == LED_BLUE)
    {
        if(GPIO_ReadOutputDataBit(LED_B_PORT,LED_B_PIN))
        {
            GPIO_ResetBits(LED_B_PORT,LED_B_PIN);
        }
        else
        {
            GPIO_SetBits(LED_B_PORT,LED_B_PIN);
        }
    }    
}

void Led_BlinkStop(void)
{
	if(s_blink_state == LED_BLINK_IDLE)
	{
		return;
	}

	Led_Off(s_blink_led);
	s_blink_interval_ms = 0;
	s_blink_tick_ms = 0;
	s_blink_times = 0;
	s_blink_infinite = 0;
	s_blink_state = LED_BLINK_IDLE;
}

void Led_BlinkStart(LED led,uint16_t interval_ms,uint16_t times)
{
	if(interval_ms == 0)
	{
		Led_BlinkStop();
		return;
	}

	s_blink_led = led;
	s_blink_interval_ms = interval_ms;
	s_blink_tick_ms = 0;

	if(times == 0)
	{
		s_blink_infinite = 1;
		s_blink_times = 0;
	}
	else
	{
		s_blink_infinite = 0;
		s_blink_times = times;
	}

	Led_On(s_blink_led);
	s_blink_state = LED_BLINK_ON;
}

void Led_Task10ms(void)
{
	if(s_blink_state == LED_BLINK_IDLE)
	{
		return;
	}
	
	s_blink_tick_ms += 10;
	
	if(s_blink_tick_ms < s_blink_interval_ms)
	{
		return;
	}
	
	s_blink_tick_ms = 0;
	
	switch(s_blink_state)
	{
		case LED_BLINK_ON:
		{
			Led_Off(s_blink_led);
			s_blink_state = LED_BLINK_OFF;
			break;
		}
		
		case LED_BLINK_OFF:
		{
			if(s_blink_infinite)
			{
				Led_On(s_blink_led);
				s_blink_state = LED_BLINK_ON;
				break;
			}

			if(s_blink_times > 0)
			{
				s_blink_times--;
			}

			if(s_blink_times == 0)
			{
				Led_Off(s_blink_led);
				s_blink_state = LED_BLINK_IDLE;
				break;
			}

			Led_On(s_blink_led);
			s_blink_state = LED_BLINK_ON;
			break;
		}
		
		default:
		{
			Led_Off(s_blink_led);
			s_blink_state = LED_BLINK_IDLE;
			break;
		}
	}
}
