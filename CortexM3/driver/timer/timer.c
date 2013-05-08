/**
* @file		stm32f10x_timer.c
* @brief		Timer ����̹� 
* @author		���̼�	
* @remarks
* @par �����̷� 
*	@li	2012.12.29 : �ʱ�  �Լ� �ۼ�  
*/

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

#include "timer.h"


static ALARM_CALLBACK AlarmCallbackTable[ALARM_MAX] = {(void *)0, };
static uint16_t AlarmPeriodTable[ALARM_MAX] = {0, };


void Timer_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
	
	/** Timer6�� �̿��Ͽ� 1ms Ÿ�̸Ӹ� �����.
	 Timer6�� APB1�� ����Ǿ� �ְ� APB1�� Ŭ���� 36MHz �̴�.
	 ���� APB1�� Ŭ���� Prescaler == 1 �̸� Internal clock(CK_INT)�� APB1 Ŭ�� 36MHz�� �ǰ� 
	 �׷��� ������ Internal clock(CK_INT)�� APB1 Ŭ��* 2 =  36 * 2 = 72MHz
	 1ms = 1KHz = 36MHz * 2 / (720 * 100) 
	 Prescaler : 720 -1, Period (Auto Reload Resigster) : 100 -1
	*/
	/* Timer6 Ŭ���� Ȱ��ȭ ��Ų��. */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	

	/* Time6 �⺻ �������� ���� */
	TIM_TimeBaseStructure.TIM_Prescaler = 719;		// 720 -1 
	TIM_TimeBaseStructure.TIM_Period = 99;			// 100 -1 
	
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    /* TIM6 Update Interrupt  Ȱ��ȭ */
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

    /* TIM6 counter  Ȱ��ȭ */
    TIM_Cmd(TIM6, ENABLE);

}


void Alarm_Open(ALARM Alarm, uint16_t msPeriod, ALARM_CALLBACK Callback)
{
	AlarmPeriodTable[Alarm] = msPeriod;
	AlarmCallbackTable[Alarm] = Callback;
}

void Alarm_Close(ALARM Alarm)
{
	AlarmPeriodTable[Alarm] = 0;
	AlarmCallbackTable[Alarm] = (void *)0;
}



void TIM6_IRQHandler(void)
{
	static uint32_t Time = 0;
	uint8_t i;
	Time++;

	/* Clear the TIM1 Update pending bit */
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

	for(i = 0;i < ALARM_MAX;i++)
	{
		if(AlarmPeriodTable[i] != 0)
		{
			if((Time % AlarmPeriodTable[i]) == 0)
			{
				if(AlarmCallbackTable[i] != (void *)0)
				{
					AlarmCallbackTable[i]();
				}
			}
		}
	}
}



