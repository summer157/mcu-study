#include "timer.h"

static volatile uint32_t s_ms_tick = 0;
static volatile uint8_t timer_10ms_flag = 0;
static volatile uint8_t timer_100ms_flag = 0;
static volatile uint8_t timer_500ms_flag = 0;

void Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 71; // 内部时钟72M，72分频后，定时器时钟频率为1M
    TIM_TimeBaseStructure.TIM_Period = 999; // 定时器周期，设定自动重载寄存器的值，计数1000次溢出
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel=TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
    NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;
    NVIC_Init(&NVIC_InitStructure);   

	TIM_Cmd(TIM6, ENABLE);
}

void TIM6_IRQHandler(void)
{
    static uint16_t tick_10ms = 0;
	static uint16_t tick_100ms = 0;
	static uint16_t tick_500ms = 0;
	
    if(TIM_GetITStatus(TIM6, TIM_IT_Update)!= RESET)
    {
		s_ms_tick++;
        tick_10ms++;
		tick_100ms++;
		tick_500ms++;
		
        if(tick_10ms >= 10)
        {
            tick_10ms = 0;
            timer_10ms_flag = 1;
        }
		
        if(tick_100ms >= 100)
        {
            tick_100ms = 0;
            timer_100ms_flag = 1;
        }
		
        if(tick_500ms >= 500)
        {
            tick_500ms = 0;
            timer_500ms_flag = 1;
        }
		
        TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
    }
}

uint8_t Timer_GetAndClear10msFlag(void)
{
	if(timer_10ms_flag == 1)
	{
		timer_10ms_flag = 0;
		return 1;
	}
	return 0;
}


uint8_t Timer_GetAndClear100msFlag(void)
{
	if(timer_100ms_flag == 1)
	{
		timer_100ms_flag = 0;
		return 1;
	}
	return 0;
}

uint8_t Timer_GetAndClear500msFlag(void)
{
	if(timer_500ms_flag == 1)
	{
		timer_500ms_flag = 0;
		return 1;
	}
	return 0;
}

uint32_t Timer_GetMs(void)
{
	return s_ms_tick;
}

