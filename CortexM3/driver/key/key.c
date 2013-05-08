/**
* @file		key.c
* @brief		Key  ����̹�  
* @author		���̼�/�ַ�� ���� ���� �п� 	
* @remarks
* @par �����̷� 
*	@li	2013.01.01 : �ʱ�  �Լ� �ۼ�  
*/

#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "key.h"

#include "serial.h"

/* Ű ADC �� ���� */
#define KEY_ADC_RIGHT		0	
#define KEY_ADC_UP			794		
#define KEY_ADC_CENTER		1638
#define KEY_ADC_LEFT		2445
#define KEY_ADC_DOWN		3251

#define KEY_ADC_VARIATION	200
#define KEY_NUMBER			5		// Ű�� ���� 

typedef struct{
	KEY_CODE Key;
	uint16_t KeyADC;
}ST_KEY_ADC;

const ST_KEY_ADC KeyADCTable[] = {{KEY_RIGHT, KEY_ADC_RIGHT}, {KEY_UP, KEY_ADC_UP},{KEY_CENTER, KEY_ADC_CENTER}, {KEY_LEFT, KEY_ADC_LEFT}, {KEY_DOWN, KEY_ADC_DOWN}}; 
KEY_HANDLE KeyHandle = (void *)0;

static KEY_CODE Key_FindKeyCode(uint16_t Data);


/**
* @fn		void Key_Init(void)
* @brief
*	- Key �ʱ�ȭ �Լ� 
* @remarks
* @param	void
* @return	void			
*/
void Key_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;  


	/*****  Ű  ���ͷ�Ʈ ����(PA1)  *****/
	/* �ܺ� ���ͷ�Ʈ ���� ���� */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	/* �ܺ� ���ͷ�Ʈ ���� */
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	

    /* EXTI line 1  Ȱ��ȭ */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	


	/*  PF6 (ADC Channel3_IN4) �� �Ƴ��α� �Է����� ���� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	// �Ƴ��α� ��� 
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* ADC3 Ŭ�� Ȱ��ȭ */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);


	/*  ADC3 ����*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 3;		// Regular ä���� ���� 
	ADC_Init(ADC3, &ADC_InitStructure);

	/* ADC3 Injected ä�� ���� */ 
	ADC_InjectedChannelConfig(ADC3, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
	ADC_InjectedSequencerLengthConfig(ADC3,1);		// Injected ä���� ���� ����

	/* ADC3 Ȱ��ȭ*/
	ADC_Cmd(ADC3, ENABLE);

	/* ADC Interrupt Configuation */
	ADC_ITConfig(ADC3, ADC_IT_JEOC, ENABLE);

	/* ADC3 IRQ �ʱ�ȭ  */
	NVIC_InitStructure.NVIC_IRQChannel = ADC3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/**
* @fn		void Key_Open(KEY_HANDLE Handle)
* @brief
*	- Key�� ����. 
* @remarks
* @param	KEY_HANDLE Handle	: �ڵ�  
* @return	void			
*/
void Key_Open(KEY_HANDLE Handle)
{
	KeyHandle = Handle;
}

/**
* @fn		void Key_Close(void)
* @brief
*	- Key�� �ݴ´�. 
* @remarks
* @param	void
* @return	void			
*/
void Key_Close(void)
{
	KeyHandle = (void *)0;
}


/**
* @fn		static KEY_CODE Key_FindKeyCode(uint16_t Data)
* @brief
*	- Key ���� ã�´�.
* @remarks
* @param	uint16_t Data	:  ADC ����� 
* @return	
*	- KEY_CODE	: Ű �� 		
*/
static KEY_CODE Key_FindKeyCode(uint16_t Data)
{
	uint8_t i;

	/*  Key ����  ã�Ƽ� �����Ѵ�.  */
	for(i = 0; i < KEY_NUMBER; i++)
	{
		if(((int16_t)Data > ((int16_t)KeyADCTable[i].KeyADC - KEY_ADC_VARIATION)) && (Data < (KeyADCTable[i].KeyADC + KEY_ADC_VARIATION)))
		{
			return KeyADCTable[i].Key;
		}
	}
	/* Ű�� �߰����� ���� */
	return KEY_NOTHING;
}



void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{

 		/* Start ADC1 Software Conversion */ 
		//ADC_SoftwareStartConvCmd(ADC3, ENABLE);
		ADC_SoftwareStartInjectedConvCmd(ADC3, ENABLE);
	  	/* Clear the EXTI Line 1 */
	  	EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

void ADC3_IRQHandler(void)
{
	uint32_t ADCValue;
	KEY_CODE Key;

	ADC_ClearFlag(ADC3, ADC_FLAG_JSTRT | ADC_FLAG_JEOC);	// Flag�� �ʱ�ȭ �Ѵ�.
	ADCValue = ADC3->JDR1;								 	// ADC ���� �д´�. 
	Key = Key_FindKeyCode((uint16_t)ADCValue);				// Ű ���� ã�´� 
	/* �ݹ� �Լ��� ȣ���Ѵ� */
	if(KeyHandle != (void *)0)
	{
		KeyHandle(Key);
	}
}




