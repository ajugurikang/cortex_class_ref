#ifndef _FND_H_
#define _FND_H_

#include "stm32f10x.h"

void FND_Init(void);
void FND_On(uint8_t Data, uint8_t Location, uint8_t Dot);
void FND_Write(uint8_t Data, uint8_t Location, uint8_t Dot);


#endif

