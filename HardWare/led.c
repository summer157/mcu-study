#include "led.h"
#include "stm32f10x.h"                  // Device header

#define LED_R_PORT GPIOB
#define LED_G_PORT GPIOB
#define LED_B_PORT GPIOB

#define LED_R_PIN GPIO_Pin_5
#define LED_G_PIN GPIO_Pin_0
#define LED_B_PIN GPIO_Pin_1

void Led_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

    GPIO_InitStructure.GPIO_Pin = LED_R_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_R_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LED_G_PIN;
    GPIO_Init(LED_G_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LED_B_PIN;
    GPIO_Init(LED_B_PORT, &GPIO_InitStructure);

    GPIO_SetBits(LED_R_PORT,LED_R_PIN);
    GPIO_SetBits(LED_G_PORT,LED_G_PIN);
    GPIO_SetBits(LED_B_PORT,LED_B_PIN);
}

void Led_On(LED led)
{
    if(led == LED_RED)
    {
        GPIO_ResetBits(LED_R_PORT,LED_R_PIN);
    }

    if(led == LED_GREEN)
    {
        GPIO_ResetBits(LED_G_PORT,LED_G_PIN);
    }

    if(led == LED_BLUE)
    {
        GPIO_ResetBits(LED_B_PORT,LED_B_PIN);
    }
}

void Led_Off(LED led)
{
    if(led == LED_RED)
    {
        GPIO_SetBits(LED_R_PORT,LED_R_PIN);
    }

    if(led == LED_GREEN)
    {
        GPIO_SetBits(LED_G_PORT,LED_G_PIN);
    }

    if(led == LED_BLUE)
    {
        GPIO_SetBits(LED_B_PORT,LED_B_PIN);
    }
}

void Led_Toggle(LED led)
{
    if(led == LED_RED)
    {
        if(GPIO_ReadOutputDataBit(LED_R_PORT,LED_R_PIN))
        {
            GPIO_ResetBits(LED_R_PORT,LED_R_PIN);
        }
        else
        {
            GPIO_SetBits(LED_R_PORT,LED_R_PIN);
        }
    }

    if(led == LED_GREEN)
    {
        if(GPIO_ReadOutputDataBit(LED_G_PORT,LED_G_PIN))
        {
            GPIO_ResetBits(LED_G_PORT,LED_G_PIN);
        }
        else
        {
            GPIO_SetBits(LED_G_PORT,LED_G_PIN);
        }
    }

    if(led == LED_BLUE)
    {
        if(GPIO_ReadOutputDataBit(LED_B_PORT,LED_B_PIN))
        {
            GPIO_ResetBits(LED_B_PORT,LED_B_PIN);
        }
        else
        {
            GPIO_SetBits(LED_B_PORT,LED_B_PIN);
        }
    }    
}
