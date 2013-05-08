#ifndef _RTC_H_
#define _RTC_H_

#include "stm32f10x.h"

typedef struct{
	uint8_t	Second;
	uint8_t Minute;
	uint8_t Hour;
	uint8_t Days;
}TIME;

void RTC_Init(void);
TIME RTC_GetTime(void);
void RTC_SetTime(TIME Time);



#endif


