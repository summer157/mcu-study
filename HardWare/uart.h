#ifndef __UART_H__
#define __UART_H__

#include "stm32f10x.h"

void Uart1_Init(void);
void Uart1_SendByte(uint8_t data);
void Uart1_SendString(const char *str);
void Uart1_CmdTask(void);
void Uart1_SendNumber(uint16_t num);
void Cmd_ProcessLine(char *cmd);

uint8_t Str_ToUint16(char *str,uint16_t *value);
uint8_t Uart1_Available(void); 
uint8_t Uart1_ReadByte(void);

#endif
