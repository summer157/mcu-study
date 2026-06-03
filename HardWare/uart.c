#include "uart.h"
#include "app.h"
#include "led.h"
#include "beep.h"
#include "string.h"

#define MAX_ARGS 3
#define UART1_RX_BUF_SIZE 64
#define UART1_CMD_BUF_SIZE 24
#define CMD_TABLE_SIZE ((sizeof(s_cmd_table)) / sizeof(s_cmd_table[0]))

typedef struct
{
	const char *name;
	void (*handler)(uint8_t argc, char *argv[]);
} CmdItem_t;

static uint8_t s_cmd_len = 0;
static volatile uint8_t s_rx_read = 0;
static volatile uint8_t s_rx_write = 0;
static volatile uint8_t s_rx_overflow = 0;

static char s_cmd_buf[UART1_CMD_BUF_SIZE];
static volatile uint8_t s_rx_buf[UART1_RX_BUF_SIZE];

static void Cmd_Help(uint8_t argc, char *argv[]);
static void Cmd_Led(uint8_t argc, char *argv[]);
static void Cmd_Beep(uint8_t argc, char *argv[]);
static void Cmd_Pwm(uint8_t argc, char *argv[]);
static void Cmd_Mode(uint8_t argc, char *argv[]);

static uint8_t Uart1_RxNextIndex(uint8_t index);
static uint8_t Cmd_Split(char *cmd, char *argv[], uint8_t max_args);

static const CmdItem_t s_cmd_table[] =
{
	{"HELP", Cmd_Help},
	{"LED", Cmd_Led},
	{"BEEP", Cmd_Beep},
	{"PWM", Cmd_Pwm},
	{"MODE", Cmd_Mode}
};

static uint8_t Uart1_RxNextIndex(uint8_t index)
{
	index++;
	if (index >= UART1_RX_BUF_SIZE)
	{
		index = 0;
	}
	return index;
}

static void Cmd_Help(uint8_t argc, char *argv[])
{
	if (argc != 1)
	{
		Uart1_SendString("Usage: HELP\r\n");
		return;
	}

	Uart1_SendString("Command: \r\n");
	Uart1_SendString("HELP\r\n");
	Uart1_SendString("LED ON RED\r\n");
	Uart1_SendString("LED OFF RED\r\n");
	Uart1_SendString("LED ON GREEN\r\n");
	Uart1_SendString("LED OFF GREEN\r\n");
	Uart1_SendString("LED ON BLUE\r\n");
	Uart1_SendString("LED OFF BLUE\r\n");
	Uart1_SendString("LED TOGGLE BLUE\r\n");
	Uart1_SendString("LED TOGGLE RED\r\n");
	Uart1_SendString("LED TOGGLE GREEN\r\n");
	Uart1_SendString("BEEP 200\r\n");
	Uart1_SendString("PWM 50\r\n");
	Uart1_SendString("MODE AUTO\r\n");
	Uart1_SendString("MODE MANUAL\r\n");
}

static void Cmd_Pwm(uint8_t argc, char *argv[])
{
	uint16_t pwm = 0;

	if (argc != 2)
	{
		Uart1_SendString("Usage: PWM 0-100\r\n");
		return;
	}

	if (Str_ToUint16(argv[1], &pwm) == 0)
	{
		Uart1_SendString("Invalid number\r\n");
		return;
	}

	if (pwm > 100)
	{
		Uart1_SendString("Usage: PWM 0-100\r\n");
		return;
	}
	
	App_SetManualDuty(pwm);
}

static void Cmd_Led(uint8_t argc, char *argv[])
{
	LED color;

	if (argc != 3)
	{
		Uart1_SendString("Usage: LED ON/OFF RED/GREEN/BLUE\r\n");
		return;
	}

	if ((strcmp(argv[1], "ON")) && (strcmp(argv[1], "OFF")) && (strcmp(argv[1], "TOGGLE")))
	{
		Uart1_SendString("Usage: ON or OFF or TOGGLE\r\n");
		return;
	}

	if (strcmp(argv[2], "BLUE") && strcmp(argv[2], "RED") && strcmp(argv[2], "GREEN"))
	{
		Uart1_SendString("Usage: RED or GREEN or BLUE\r\n");
		return;
	}

	if (!strcmp(argv[2], "RED"))
	{
		color = LED_RED;
	}

	else if (!strcmp(argv[2], "GREEN"))
	{
		color = LED_GREEN;
	}

	else if (!strcmp(argv[2], "BLUE"))
	{
		color = LED_BLUE;
	}

	else
	{
		Uart1_SendString("Usage: RED or GREEN or BLUE\r\n");
		return;
	}

	if (!strcmp(argv[1], "ON"))
	{
		Led_On(color);
		Uart1_SendString("OK: LED ON\r\n");
	}

	else if (!strcmp(argv[1], "OFF"))
	{
		Led_Off(color);
		Uart1_SendString("OK: LED OFF\r\n");
	}

	else if (!strcmp(argv[1], "TOGGLE"))
	{
		Led_Toggle(color);
		Uart1_SendString("OK: LED TOGGLE\r\n");
	}

	else
	{
		Uart1_SendString("Usage: ON or OFF or TOGGLE\r\n");
		return;
	}
}

static void Cmd_Beep(uint8_t argc, char *argv[])
{
	uint16_t beep_time = 0;

	if (argc != 2)
	{
		Uart1_SendString("Usage: Beep 10-1000\r\n");
		return;
	}

	if (Str_ToUint16(argv[1], &beep_time))
	{
		if (beep_time >= 10 && beep_time <= 1000)
		{
			Beep_Trigger(beep_time);
			Uart1_SendString("OK: BEEP ON\r\n");
		}
		else
		{
			Uart1_SendString("Usage: 10-1000\r\n");
		}
	}

	else
	{
		Uart1_SendString("Invalid number\r\n");
	}
}

static void Cmd_Mode(uint8_t argc,char *argv[])
{
	if(argc != 2)
	{
		Uart1_SendString("Usage: MODE AUTO/MANUAL\r\n");
		return;
	}
	
	if(strcmp(argv[1],"AUTO") == 0)
	{
		App_SetMode(APP_MODE_AUTO);
	}
	else if(strcmp(argv[1],"MANUAL") == 0)
	{
		App_SetMode(APP_MODE_MANUAL);
	}
	else
	{
		Uart1_SendString("Usage: AUTO/MANUAL\r\n");
		return;
	}
}

static uint8_t Cmd_Split(char *cmd, char *argv[], uint8_t max_args)
{
	uint8_t args = 0;
	while (*cmd != '\0')
	{
		// 跳过空格
		while (*cmd == ' ')
		{
			cmd++;
		}
		// 如果跳过空格后遇到结束符，退出循环
		if (*cmd == '\0')
		{
			break;
		}

		// 如果超出最大参数数量，退出循环
		if (args >= max_args)
		{
			break;
		}
		// 将参数首字符地址写入数组
		argv[args] = cmd;
		args++;
		// 寻找当前参数的结束位置
		while (*cmd != ' ' && *cmd != '\0')
		{
			cmd++;
		}
		// 遇到空格则改为结束符，切断字符串
		if (*cmd == ' ')
		{
			*cmd = '\0';
			cmd++;
		}
	}
	return args;
}

void Uart1_CmdTask(void)
{
	uint8_t data;

	if (s_rx_overflow)
	{
		s_rx_overflow = 0;
		Uart1_SendString("ERR: RX BUFFER OVERFLOW\r\n");
	}

	while (Uart1_Available())
	{
		data = Uart1_ReadByte();
		if (data == '\r' || data == '\n')
		{
			if (s_cmd_len > 0)
			{
				s_cmd_buf[s_cmd_len] = '\0';
				Cmd_ProcessLine(s_cmd_buf);
				s_cmd_len = 0;
			}
		}

		else
		{
			if (s_cmd_len < UART1_CMD_BUF_SIZE - 1)
			{
				s_cmd_buf[s_cmd_len] = (char)data;
				s_cmd_len++;
			}
			else
			{
				s_cmd_len = 0;
				Uart1_SendString("ERR: CMD TOO LONG\r\n");
			}
		}
	}
}

void Uart1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 1. 开启 GPIOA 和 USART1 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

	/* 2. PA9 配置为 USART1_TX，复用推挽输出 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 3. PA10 配置为 USART1_RX，上拉输入 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 4. 配置 USART1：115200, 8N1 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/* 5. 使能接收中断，配置NVIC */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	/* 6. 使能 USART1 */
	USART_Cmd(USART1, ENABLE);
}

void Uart1_SendByte(uint8_t data)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
	{
	}
	USART_SendData(USART1, data);
}

void Uart1_SendString(const char *str)
{
	while (*str != '\0')
	{
		Uart1_SendByte((uint8_t)*str);
		str++;
	}
}

uint8_t Str_ToUint16(char *str, uint16_t *value)
{
	uint32_t result = 0;

	if (str == 0 || value == 0)
	{
		return 0;
	}

	if (*str == '\0')
	{
		return 0;
	}

	while (*str != '\0')
	{
		if (*str >= '0' && *str <= '9')
		{
			result = result * 10 + (uint8_t)(*str - '0');
			str++;
		}

		else
		{
			return 0;
		}
	}

	if (result > 65535)
	{
		return 0;
	}

	*value = result;
	return 1;
}

void Uart1_SendNumber(uint16_t num)
{
	char buf[6];
	uint8_t i = 0;

	if (num == 0)
	{
		Uart1_SendByte('0');
		return;
	}

	while (num > 0)
	{
		buf[i] = (char)(num % 10 + '0');
		num = num / 10;
		i++;
	}

	while (i > 0)
	{
		i--;
		Uart1_SendByte(buf[i]);
	}
}

uint8_t Uart1_Available(void)
{
	if (s_rx_read != s_rx_write)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t Uart1_ReadByte(void)
{
	uint8_t data;

	if (s_rx_read == s_rx_write)
	{
		return 0;
	}

	data = s_rx_buf[s_rx_read];
	s_rx_read = Uart1_RxNextIndex(s_rx_read);

	return data;
}

void Cmd_ProcessLine(char *cmd)
{
	uint8_t argc = 0;
	uint8_t i = 0;
	char *argv[MAX_ARGS];

	if (cmd == 0)
	{
		return;
	}

	argc = Cmd_Split(cmd, argv, MAX_ARGS);

	if (argc == 0)
	{
		return;
	}

	for (i = 0; i < CMD_TABLE_SIZE; i++)
	{
		if (strcmp(s_cmd_table[i].name, argv[0]) == 0)
		{
			s_cmd_table[i].handler(argc, argv);
			return;
		}
	}

	Uart1_SendString("Unknown command\r\n");
}

void USART1_IRQHandler(void)
{
	uint8_t data;
	uint8_t next_write;

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		data = (uint8_t)USART_ReceiveData(USART1);
		next_write = Uart1_RxNextIndex(s_rx_write);
		if (next_write != s_rx_read)
		{
			s_rx_buf[s_rx_write] = data;
			s_rx_write = next_write;
		}
		else
		{
			s_rx_overflow = 1;
		}
	}
}
