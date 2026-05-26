#ifndef __KEY_H__
#define __KEY_H__

typedef enum
{
    KEY_NONE = 0,
    KEY_SHORT,
    KEY_LONG
} key_event_t;

typedef enum
{
    KEY1 = 0,
    KEY2
} KEY;

void Key_Init(void);
void Key_Scan(void); // 10ms定时任务里扫描
key_event_t Get_Keyevent(void);

#endif
