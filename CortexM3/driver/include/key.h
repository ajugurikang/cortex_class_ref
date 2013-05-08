#ifndef _KEY_H_
#define _KEY_H_

typedef enum{
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_CENTER,
	KEY_NOTHING,
}KEY_CODE;

typedef void (*KEY_HANDLE)(KEY_CODE Key);

void Key_Init(void);
void Key_Open(KEY_HANDLE Handle);
void Key_Close(void);

#endif

