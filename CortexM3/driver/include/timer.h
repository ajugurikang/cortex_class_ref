#ifndef _TIMER_H_
#define _TIMER_H_

#include "stm32f10x.h"

typedef enum{
	ALARM0,
	ALARM1,
	ALARM2,
	ALARM3,
	ALARM4,
	ALARM_MAX	
}ALARM;

typedef void (*ALARM_CALLBACK)(void);

void Timer_Init(void);
void Alarm_Open(ALARM Alarm, uint16_t msPeriod, ALARM_CALLBACK Callback); 
void Alarm_Close(ALARM Alarm); 


#endif



