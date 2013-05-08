/**
* @file		rtc.c
* @brief		RTC 드라이버 
* @author		강이석	
* @remarks
* @par 수정이력 
*	@li	2012.12.29 : 초기  함수 작성  
*/

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_rtc.h"
#include "misc.h"
#include "rtc.h"

uint32_t RealTimeCount = 0;			// RTC 의 초단위 시간 

void RTC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* PWR and BKP clocks selection --------------------------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
	
	/* Reset Backup Domain */
//	BKP_DeInit();
	
	/* Enable the LSE OSC */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}
	
	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Enable the RTC Second */  
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/***** Enable the RTC Interrupt *****/
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

}

void RTC_SetTime(TIME Time)
{
	RealTimeCount = Time.Days * 86400 + Time.Hour * 3600 + Time.Minute * 60 + Time.Second;
	RTC_SetCounter(RealTimeCount);
}

TIME RTC_GetTime(void)
{
	TIME Time;
	
	Time.Second = RealTimeCount % 60;
	Time.Minute = (RealTimeCount / 60) % 60;
	Time.Hour = (RealTimeCount / 3600) % 24;
	Time.Days = RealTimeCount / 86400;

	return Time;
}


void RTC_IRQHandler(void)
{
	
	RealTimeCount = RTC_GetCounter();
	/* Clear the RTC Second Interrupt pending bit */  
	RTC_ClearITPendingBit(RTC_IT_SEC);	
}

