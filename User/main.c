#include "stm32f10x.h"                  // Device header
#include "led.h"
#include "key.h"
#include "timer.h"

int main(void)
{	
	key_event_t key_event = KEY_NONE;
	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);//设置中断组为0
	
	Led_Init();
	Timer_Init();
	Key_Init();
	
	while(1)
	{
		if(Timer_Get10msFlag())
		{
			Timer_Clear10msFlag();
			Key_Scan();
		}
		
		key_event = Get_Keyevent();
		
		if(key_event == KEY_SHORT)
		{
			Led_Toggle(LED_BLUE);
		}
		
		else if(key_event == KEY_LONG)
		{
			Led_Off(LED_BLUE);
	    }
		
	}
}
