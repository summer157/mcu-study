#include "beep.h"

#define BEEP_PORT GPIOA
#define BEEP_PIN GPIO_Pin_8

static uint16_t s_beep_time_ms;
	
void Beep_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BEEP_PORT,&GPIO_InitStructure);
	
	GPIO_ResetBits(BEEP_PORT,BEEP_PIN);
}

void Beep_On(void)
{
	GPIO_SetBits(BEEP_PORT,BEEP_PIN);
}

void Beep_Off(void)
{
	GPIO_ResetBits(BEEP_PORT,BEEP_PIN);
}

void Beep_Task10ms(void)
{
	if(s_beep_time_ms > 0)
	{
		if(s_beep_time_ms >=10)
		{
			s_beep_time_ms = s_beep_time_ms - 10;
		}
		else
		{
			s_beep_time_ms = 0;
		}
	}
	
	if(s_beep_time_ms == 0)
	{
		Beep_Off();
	}
}

void Beep_Trigger(uint16_t time)
{
	s_beep_time_ms = time;
	Beep_On();
}
