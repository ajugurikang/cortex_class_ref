/**
* @file		stm32f10x_timer.c
* @brief		Timer 드라이버 
* @author		강이석	
* @remarks
* @par 수정이력 
*	@li	2012.12.29 : 초기  함수 작성  
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
	
	/** Timer6를 이용하여 1ms 타이머를 만든다.
	 Timer6은 APB1에 연결되어 있고 APB1의 클럭은 36MHz 이다.
	 만약 APB1이 클럭의 Prescaler == 1 이면 Internal clock(CK_INT)는 APB1 클럭 36MHz가 되고 
	 그렇지 않으면 Internal clock(CK_INT)는 APB1 클럭* 2 =  36 * 2 = 72MHz
	 1ms = 1KHz = 36MHz * 2 / (720 * 100) 
	 Prescaler : 720 -1, Period (Auto Reload Resigster) : 100 -1
	*/
	/* Timer6 클럭을 활성화 시킨다. */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	

	/* Time6 기본 레지스터 설정 */
	TIM_TimeBaseStructure.TIM_Prescaler = 719;		// 720 -1 
	TIM_TimeBaseStructure.TIM_Period = 99;			// 100 -1 
	
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    /* TIM6 Update Interrupt  활성화 */
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

    /* TIM6 counter  활성화 */
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



