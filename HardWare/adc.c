#include "adc.h"

#define ADC_VREF_MV 3300u
#define ADC_MAX_VALUE 4095u

void Adc_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1,ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_11,1,ADC_SampleTime_55Cycles5);
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);
	
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) == SET);
	
}

uint16_t Adc_ReadRaw(void)
{
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
	
	return ADC_GetConversionValue(ADC1);
}

uint16_t Adc_ReadAverage(uint8_t times)
{
	uint8_t i = 0;
	uint32_t sum = 0;
	
	if(times == 0)
	{
		return 0;
	}
	
	for(i=0;i<times;i++)
	{
		sum = sum + Adc_ReadRaw();
	}
	
	return (uint16_t)(sum / times);
}

uint16_t Adc_RawToMv(uint16_t raw)
{
	if(raw > ADC_MAX_VALUE)
	{
		raw = ADC_MAX_VALUE;
	}
	
	return (uint16_t)((uint32_t)raw * ADC_VREF_MV / ADC_MAX_VALUE);
}

