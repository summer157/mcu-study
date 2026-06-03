#ifndef __KEY_H__
#define __KEY_H__

typedef enum
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SHORT,
    KEY_EVENT_LONG
} KeyEvent_t;

typedef enum
{
    KEY1 = 0,
    KEY2
} KEY;

void Key_Init(void);
void Key_Scan10ms(void); // 10ms定时任务里扫描
KeyEvent_t Key_GetEvent(void);

#endif
