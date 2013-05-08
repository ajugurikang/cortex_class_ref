#ifndef _STM32F10X_LCD_H_
#define _STM32F10X_LCD_H_

typedef enum{
	BLACK 	= 0x0000,
	WHITE 	= 0xFFFF,
	YELLOW 	= 0xFFE0,
	RED 	= 0xF800,
	GREEN 	= 0x07E0,
	BLUE 	= 0x001F,
	CYAN 	= 0x07FF,
	MAGENTA = 0xF81F,
}COLOR;

void LCD_Init(void);
void LCD_ClearScreen(COLOR Color);
void LCD_FillColor(uint16_t SX, uint16_t SY, uint16_t DX, uint16_t DY, COLOR Color);
void LCD_WriteEnglish(uint16_t SX, uint16_t SY, uint8_t Data, COLOR Color, COLOR BG_Color);
void LCD_WriteKorean(uint16_t SX, uint16_t SY, uint16_t Data, COLOR Color, COLOR BG_Color);
void LCD_WriteString(uint16_t SX, uint16_t SY,const char *String, COLOR Color, COLOR BG_Color);
void LCD_DrawPixel(uint16_t SX, uint16_t SY, COLOR Color);



#endif 



