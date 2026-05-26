#include "key.h"
#include "stm32f10x.h" // Device header

#define KEY1_PORT GPIOA
#define KEY1_PIN GPIO_Pin_0

#define KEY2_PORT GPIOC
#define KEY2_PIN GPIO_Pin_13

typedef enum
{
    KEY_IDLE = 0,
    KEY_PRESS_DEBOUNCE,
    KEY_PRESS,
    KEY_RELEASE_DEBOUNCE,
    KEY_LONG_REPORTED
} key_state_t;

static key_event_t s_key_event = KEY_NONE;
static key_state_t s_key_state = KEY_IDLE;

static uint8_t s_press_tick = 0;
static uint8_t s_debounce_tick = 0;
static uint8_t s_long_reported = 0;

static uint8_t Key_Read(KEY key)
{
    if (key == KEY1)
    {
        if (GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN))
        {
            return 1; // 按键按下
        }
        else
        {
            return 0;
        }
    }

    if (key == KEY2)
    {
        if (GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN))
        {
            return 1; // 按键按下
        }
        else
        {
            return 0;
        }
    }
	
	return 0;
}

void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = KEY1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = KEY2_PIN;
    GPIO_Init(KEY2_PORT, &GPIO_InitStructure);
}

key_event_t Get_Keyevent(void)
{
    key_event_t event;
    event = s_key_event;
    s_key_event = KEY_NONE;
    return event;
}

void Key_Scan(void)
{
    uint8_t raw = Key_Read(KEY1);
    switch (s_key_state)
    {
    case KEY_IDLE:
    {
        s_debounce_tick = 0;
        s_press_tick = 0;
        s_long_reported = 0;

        if (raw)
        {
            s_key_state = KEY_PRESS_DEBOUNCE;
        }
        break;
    }

    case KEY_PRESS_DEBOUNCE:
    {
        if (raw)
        {
            s_debounce_tick++;
            if (s_debounce_tick >= 2)
            {
                s_debounce_tick = 0;
                s_key_state = KEY_PRESS;
            }
        }
        else
        {
            s_debounce_tick = 0;
            s_key_state = KEY_IDLE;
        }
        break;
    }

    case KEY_PRESS:
    {
        if (raw)
        {
            s_press_tick++;
            if (s_press_tick >= 50)
            {
                s_press_tick = 0;
                s_long_reported = 1;
                s_key_event = KEY_LONG;
                s_key_state = KEY_LONG_REPORTED;
            }
        }
        else
        {
            s_debounce_tick = 0;
            s_key_state = KEY_RELEASE_DEBOUNCE;
        }
        break;
    }

    case KEY_RELEASE_DEBOUNCE:
    {
        if (!raw)
        {
            s_debounce_tick++;
            if (s_debounce_tick >= 2)
            {
                if (!s_long_reported)
                {
                    if (s_press_tick < 50)
                    {
                        s_key_event = KEY_SHORT;
                    }
                }
                s_debounce_tick = 0;
                s_long_reported = 0;
                s_press_tick = 0;
                s_key_state = KEY_IDLE;
            }
        }
        else
        {
            s_debounce_tick = 0;
            s_key_state = KEY_PRESS;
        }
        break;
    }

    case KEY_LONG_REPORTED:
    {
        if (!raw)
        {
            s_debounce_tick = 0;
            s_key_state = KEY_RELEASE_DEBOUNCE;
        }
        break;
    }
    }
}
