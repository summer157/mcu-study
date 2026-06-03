#include "app.h"
#include "pwm.h"
#include "adc.h"
#include "led.h"
#include "uart.h"
#include "beep.h"
#include "timer.h"

int main(void)
{
    KeyEvent_t key_event = KEY_EVENT_NONE;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    Led_Init();
    Key_Init();
    Pwm_Init();
	Adc_Init();
    Beep_Init();
    Uart1_Init();
    Timer_Init();
	
	App_Init();

    Uart1_SendString("hello stm32\r\n");

    while (1)
    {
        if (Timer_GetAndClear10msFlag())
        {
            Beep_Task10ms();
            Led_Task10ms();
            Key_Scan10ms();
			App_Task10ms();
        }
		
		if(Timer_GetAndClear100msFlag())
		{
			App_Task100ms();
		}
		
		if(Timer_GetAndClear500msFlag())
		{
			App_Task500ms();
		}
		
        key_event = Key_GetEvent();
		App_HandleKeyEvent(key_event);

        Uart1_CmdTask();
    }
}
