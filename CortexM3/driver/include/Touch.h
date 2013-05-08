#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "stm32f10x.h"

typedef void (*TOUCH_HANDLE)(uint16_t X_Position, uint16_t Y_Position);

void Touch_Init(void);
void Touch_Open(TOUCH_HANDLE Handle);
void Touch_Close(void);

#endif
