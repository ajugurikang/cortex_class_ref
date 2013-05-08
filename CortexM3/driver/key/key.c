/**
* @file		key.c
* @brief		Key  드라이버  
* @author		강이석/솔루션 공과 전자 학원 	
* @remarks
* @par 수정이력 
*	@li	2013.01.01 : 초기  함수 작성  
*/

#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "key.h"

#include "serial.h"

/* 키 ADC 값 정의 */
#define KEY_ADC_RIGHT		0	
#define KEY_ADC_UP			794		
#define KEY_ADC_CENTER		1638
#define KEY_ADC_LEFT		2445
#define KEY_ADC_DOWN		3251

#define KEY_ADC_VARIATION	200
#define KEY_NUMBER			5		// 키의 개수 

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
*	- Key 초기화 함수 
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


	/*****  키  인터럽트 설정(PA1)  *****/
	/* 외부 인터럽트 라인 설정 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	/* 외부 인터럽트 설정 */
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	

    /* EXTI line 1  활성화 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	


	/*  PF6 (ADC Channel3_IN4) 를 아날로그 입력으로 설정 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	// 아날로그 모드 
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* ADC3 클럭 활성화 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);


	/*  ADC3 설정*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 3;		// Regular 채널의 개수 
	ADC_Init(ADC3, &ADC_InitStructure);

	/* ADC3 Injected 채널 설정 */ 
	ADC_InjectedChannelConfig(ADC3, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
	ADC_InjectedSequencerLengthConfig(ADC3,1);		// Injected 채널의 개수 설정

	/* ADC3 활성화*/
	ADC_Cmd(ADC3, ENABLE);

	/* ADC Interrupt Configuation */
	ADC_ITConfig(ADC3, ADC_IT_JEOC, ENABLE);

	/* ADC3 IRQ 초기화  */
	NVIC_InitStructure.NVIC_IRQChannel = ADC3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/**
* @fn		void Key_Open(KEY_HANDLE Handle)
* @brief
*	- Key를 연다. 
* @remarks
* @param	KEY_HANDLE Handle	: 핸들  
* @return	void			
*/
void Key_Open(KEY_HANDLE Handle)
{
	KeyHandle = Handle;
}

/**
* @fn		void Key_Close(void)
* @brief
*	- Key를 닫는다. 
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
*	- Key 값을 찾는다.
* @remarks
* @param	uint16_t Data	:  ADC 결과값 
* @return	
*	- KEY_CODE	: 키 값 		
*/
static KEY_CODE Key_FindKeyCode(uint16_t Data)
{
	uint8_t i;

	/*  Key 값을  찾아서 리턴한다.  */
	for(i = 0; i < KEY_NUMBER; i++)
	{
		if(((int16_t)Data > ((int16_t)KeyADCTable[i].KeyADC - KEY_ADC_VARIATION)) && (Data < (KeyADCTable[i].KeyADC + KEY_ADC_VARIATION)))
		{
			return KeyADCTable[i].Key;
		}
	}
	/* 키를 발견하지 못함 */
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

	ADC_ClearFlag(ADC3, ADC_FLAG_JSTRT | ADC_FLAG_JEOC);	// Flag를 초기화 한다.
	ADCValue = ADC3->JDR1;								 	// ADC 값을 읽는다. 
	Key = Key_FindKeyCode((uint16_t)ADCValue);				// 키 값을 찾는다 
	/* 콜백 함수를 호출한다 */
	if(KeyHandle != (void *)0)
	{
		KeyHandle(Key);
	}
}




