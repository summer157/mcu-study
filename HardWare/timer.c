#include "timer.h"

static volatile uint8_t timer_10ms_flag = 0;

void Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);

    TIM_TimeBaseStructure.TIM_Prescaler = 71; // 内部时钟72M，72分频后，定时器时钟频率为1M
    TIM_TimeBaseStructure.TIM_Period = 1000; // 定时器周期，设定自动重载寄存器的值，计数1000次溢出
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM6, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    //设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel=TIM6_IRQn;
    //设置主优先级为0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;
    //设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
    NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM6_IRQHandler(void)
{
    static uint16_t tick_1ms = 0;

    if(TIM_GetITStatus(TIM6, TIM_IT_Update)!= RESET)
    {
        tick_1ms++;
        if(tick_1ms >= 10)
        {
            tick_1ms = 0;
            timer_10ms_flag = 1;
        }
        TIM_ClearITPendingBit(TIM6,TIM_FLAG_Update);
    }
}

uint8_t Timer_Get10msFlag(void)
{
    return timer_10ms_flag;
}

void Timer_Clear10msFlag(void)
{
    timer_10ms_flag = 0;
}
