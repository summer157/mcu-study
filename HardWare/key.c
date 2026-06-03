#include "stm32f10x.h"
#include "key.h"

/*
 * 假设：
 * 1. Key_Scan10ms() 每 10ms 调用一次
 * 2. 按键高电平有效
 * 3. 当前模板只处理一个按键
 */

#define KEY_SCAN_PERIOD_MS       10u
#define KEY_DEBOUNCE_TIME_MS     20u
#define KEY_LONG_TIME_MS         1000u

#define KEY_DEBOUNCE_TICKS       (KEY_DEBOUNCE_TIME_MS / KEY_SCAN_PERIOD_MS)
#define KEY_LONG_TICKS           (KEY_LONG_TIME_MS / KEY_SCAN_PERIOD_MS)

typedef enum
{
    KEY_STATE_IDLE = 0, // 空闲，等待按下
    KEY_STATE_PRESS_DEBOUNCE, // 按下消抖
    KEY_STATE_PRESSED, // 已确认按下，等待短按/长按结果
    KEY_STATE_LONG_REPORTED, // 长按已上报，只等稳定释放
    KEY_STATE_RELEASE_DEBOUNCE // 短按路径下的释放消抖
} KeyState_t;

static KeyState_t s_key_state = KEY_STATE_IDLE;

static uint16_t s_debounce_tick = 0;
static uint16_t s_press_tick = 0;

static volatile KeyEvent_t s_key_event = KEY_EVENT_NONE;

/*
 * 读取原始按键电平
 * 返回 1：按下
 * 返回 0：松开
 */
static uint8_t Key_ReadRaw(void)
{
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET)
    {
        return 1;
    }

    return 0;
}


/*
 * 按键初始化
 */
void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    s_key_state = KEY_STATE_IDLE;
    s_debounce_tick = 0;
    s_press_tick = 0;
    s_key_event = KEY_EVENT_NONE;
}


/*
 * 每 10ms 调用一次
 */
void Key_Scan10ms(void)
{
    uint8_t raw;

    raw = Key_ReadRaw();

    switch(s_key_state)
    {
        case KEY_STATE_IDLE:
        {
            if(raw)
            {
                s_debounce_tick = 0;
                s_press_tick = 0;
                s_key_state = KEY_STATE_PRESS_DEBOUNCE;
            }
            break;
        }

        case KEY_STATE_PRESS_DEBOUNCE:
        {
            if(raw)
            {
                s_debounce_tick++;

                if(s_debounce_tick >= KEY_DEBOUNCE_TICKS)
                {
                    s_debounce_tick = 0;
                    s_press_tick = 0;
                    s_key_state = KEY_STATE_PRESSED;
                }
            }
            else
            {
                s_debounce_tick = 0;
                s_press_tick = 0;
                s_key_state = KEY_STATE_IDLE;
            }
            break;
        }

        case KEY_STATE_PRESSED:
        {
            if(raw)
            {
                if(s_press_tick < 0xFFFF)
                {
                    s_press_tick++;
                }

                if(s_press_tick >= KEY_LONG_TICKS)
                {
                    s_key_event = KEY_EVENT_LONG;

                    s_debounce_tick = 0;
                    s_key_state = KEY_STATE_LONG_REPORTED;
                }
            }
            else
            {
                s_debounce_tick = 0;
                s_key_state = KEY_STATE_RELEASE_DEBOUNCE;
            }
            break;
        }

        case KEY_STATE_RELEASE_DEBOUNCE:
        {
            if(!raw)
            {
                s_debounce_tick++;

                if(s_debounce_tick >= KEY_DEBOUNCE_TICKS)
                {
                    s_key_event = KEY_EVENT_SHORT;

                    s_debounce_tick = 0;
                    s_press_tick = 0;
                    s_key_state = KEY_STATE_IDLE;
                }
            }
            else
            {
                /*
                 * 释放消抖失败，说明刚才可能只是抖动，
                 * 按键仍然处于按下状态，回到 PRESSED 继续计时。
                 */
                s_debounce_tick = 0;
                s_key_state = KEY_STATE_PRESSED;
            }
            break;
        }

        case KEY_STATE_LONG_REPORTED:
        {
            if(!raw)
            {
                s_debounce_tick++;

                if(s_debounce_tick >= KEY_DEBOUNCE_TICKS)
                {
                    /*
                     * 长按已经上报过了。
                     * 这里稳定释放后只回到 IDLE，
                     * 不再上报短按。
                     */
                    s_debounce_tick = 0;
                    s_press_tick = 0;
                    s_key_state = KEY_STATE_IDLE;
                }
            }
            else
            {
                /*
                 * 释放过程中又检测到按下，认为是释放抖动或仍未松手。
                 * 继续停留在 LONG_REPORTED。
                 * 重点：不要回到 PRESSED。
                 */
                s_debounce_tick = 0;
            }
            break;
        }

        default:
        {
            s_key_state = KEY_STATE_IDLE;
            s_debounce_tick = 0;
            s_press_tick = 0;
            break;
        }
    }
}


/*
 * 获取按键事件
 * 事件被取走后清空
 */
KeyEvent_t Key_GetEvent(void)
{
    KeyEvent_t event;

    event = s_key_event;
    s_key_event = KEY_EVENT_NONE;

    return event;
}

