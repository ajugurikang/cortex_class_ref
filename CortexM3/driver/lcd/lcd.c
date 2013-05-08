/**
* @file		stm32f10x_lcd.c
* @brief		LCD ����̹� 
* @author		���̼�	
* @remarks
* @par �����̷� 
*	@li	2012.12.26 : �ʱ�  �Լ� �ۼ�  
*/

//#define LCD_DUBUG			// LCD ����� �޽��� ��� 

#ifdef	LCD_DUBUG
#include "serial.h"	
#endif

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "msk2844.h"
#include "extif.h"
#include "lcd.h"
#include "stm32f10x_fsmc.h"

#include "engfont.h"
#include "korfont.h"



#define LCD_WIDTH	240
#define LCD_HEIGHT	400

#define LCD_FONT_ENG_COLUMN                8
#define LCD_FONT_ENG_ROW                   16
#define LCD_FONT_ENG_SIZE                  16      // ���� ��Ʈ�� �����ϴ� �޸� ũ��(����Ʈ)
#define LCD_FONT_KOR_COLUMN                16
#define LCD_FONT_KOR_ROW                   16
#define LCD_FONT_KOR_SIZE                  32      // �ѱ� ��Ʈ�� �����ϴ� �޸� ũ��(����Ʈ)




static void LCD_Reset(void);
static void LCD_Delay(uint32_t Delay);
static void LCD_WriteDataRepeat(uint16_t Data, uint32_t Repeat);
static uint16_t LCD_GetKFontIndex(uint16_t Code);




typedef struct
{
  __IO uint16_t Instruction;
  __IO uint16_t Data;
} LCD_TypeDef;


/* Note:  Bank 4 of NOR/SRAM Bank 4 */
#define LCD_BASE        ((uint32_t)(0x60000000 | 0x0C000000))
#define LCD             ((LCD_TypeDef *) LCD_BASE)



/**
* @fn		void LCD_Init(void)
* @brief
*	@li		LED �ʱ�ȭ  
* @remarks
* @param	void
* @return	void			
*/
void LCD_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/***** Reset ��Ʈ �ʱ�ȭ *****/
	// 1. GPIO Ŭ�� Ȱ��ȭ 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	// 2-1 ��Ʈ D ���� (D0 ~ D3, FSMC_NWE)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	LCD_Reset();

	// Power Control System 	
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;			LCD->Data = 0x1000;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0100;
	LCD_Delay(10000);	//LCD_msDelay(1);
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;  		LCD->Data = 0x000C;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0101;
	LCD_Delay(10000);	//LCD_msDelay(1);
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;  		LCD->Data = 0x1000;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0101;
	LCD_Delay(10000);	//LCD_msDelay(1);
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;  		LCD->Data = 0x1202;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0102;
	LCD_Delay(10000);	//LCD_msDelay(1);

	LCD->Instruction = MSK_IX_POWER_CONTROL1;  			LCD->Data = 0x4010;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;  		LCD->Data = 0x00BC;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0100;	
	LCD_Delay(10000);	//LCD_msDelay(1);
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;  		LCD->Data = 0x1200;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0100;
	LCD_Delay(10000);	//LCD_msDelay(1);
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;  		LCD->Data = 0x001C;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0101;
	LCD_Delay(10000);	//LCD_msDelay(35);

	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;  		LCD->Data = 0x1A00;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0100;
	LCD_Delay(10000);	//LCD_msDelay(1);
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;  		LCD->Data = 0x0200;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0103;	
	LCD_Delay(10000);	//LCD_msDelay(1);
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;  		LCD->Data = 0x1506;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0103;
	LCD_Delay(10000);	//LCD_msDelay(1);
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;  		LCD->Data = 0x0C41;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0104;
	LCD_Delay(750000);	//LCD_msDelay(75);

	// Display Control System
	LCD->Instruction = MSK_IX_DEVICE_OUTPUT_CONTROL;	LCD->Data = 0x0100;
	LCD->Instruction = MSK_IX_LCD_ALT_CONTROL;  		LCD->Data = 0x0700;
	LCD->Instruction = MSK_IX_ENTRY_MODE;  				LCD->Data = 0x0030;	
	LCD->Instruction = MSK_IX_DISPLAY_CONTROL2;			LCD->Data = 0x0506;
	LCD->Instruction = MSK_IX_DISPLAY_CONTROL3;  		LCD->Data = 0x0001;
	LCD->Instruction = MSK_IX_DISPLAY_CONTROL4;  		LCD->Data = 0x0000;
	LCD->Instruction = MSK_IX_FRAME_CYCLE_ADJ_CONTROL;  LCD->Data = 0x0010;
	LCD->Instruction = MSK_IX_GATE_LTPS_IF_CONTROL3;  	LCD->Data = 0x0000;
	LCD->Instruction = MSK_IX_GATE_LTPS_IF_CONTROL4;  	LCD->Data = 0x0002;
	LCD->Instruction = MSK_IX_GATE_LTPS_IF_CONTROL5;  	LCD->Data = 0x0000;
	LCD->Instruction = MSK_IX_GATE_LTPS_IF_CONTROL10;  	LCD->Data = 0x0010;
	LCD->Instruction = 0x01C;  							LCD->Data = 0x0000;
	LCD_Delay(10000);

	LCD->Instruction = 0x205;  							LCD->Data = 0x0000;
	LCD->Instruction = 0x70A;  							LCD->Data = 0x7800;
	LCD->Instruction = 0x70B;  							LCD->Data = 0x0000;
	LCD->Instruction = 0x70C;  							LCD->Data = 0x0030;
	LCD->Instruction = 0x70D;  							LCD->Data = 0x0000;
	LCD->Instruction = 0x70E;  							LCD->Data = 0x0000;
	LCD->Instruction = 0x70F;  							LCD->Data = 0x2100;
	LCD->Instruction = 0x710;  							LCD->Data = 0x0000;
	LCD_Delay(10000);

	// Gamma Control
	LCD->Instruction = MSK_IX_GAMMA_CONTROL1;  			LCD->Data = 0x0503;
	LCD->Instruction = MSK_IX_GAMMA_CONTROL2;  			LCD->Data = 0x0403;
	LCD->Instruction = MSK_IX_GAMMA_CONTROL3;  			LCD->Data = 0x0404;
	LCD->Instruction = MSK_IX_GAMMA_CONTROL4;  			LCD->Data = 0x0303;
	LCD->Instruction = MSK_IX_GAMMA_CONTROL5;  			LCD->Data = 0x0302;
	LCD->Instruction = MSK_IX_GAMMA_CONTROL6;  			LCD->Data = 0x0203;
	LCD->Instruction = MSK_IX_GAMMA_CONTROL7;  			LCD->Data = 0x0E14;
	LCD->Instruction = MSK_IX_GAMMA_CONTROL8;  			LCD->Data = 0x0503;
	LCD->Instruction = MSK_IX_GAMMA_CONTROL9;  			LCD->Data = 0x0403;
	LCD->Instruction = MSK_IX_GAMMA_CONTROL10;  		LCD->Data = 0x0404;
	LCD->Instruction = 0x030A;  						LCD->Data = 0x0303;
	LCD->Instruction = 0x030B; 		 					LCD->Data = 0x0302;
	LCD->Instruction = 0x030C;  						LCD->Data = 0x0203;
	LCD->Instruction = 0x030D;  						LCD->Data = 0x1F10;
	LCD_Delay(10000);

	//Coordinate Control System
	LCD->Instruction = MSK_IX_LINE_NUMBER_CONTROL;  	LCD->Data = 0x0031;
	LCD->Instruction = MSK_IX_SCREEN_CONTROL;  			LCD->Data = 0x0000;
	LCD->Instruction = MSK_IX_BASE_PIC_RAM_AREA_START;  LCD->Data = 0x0000;
	LCD->Instruction = MSK_IX_BASE_PIC_RAM_AREA_END;  	LCD->Data = 0x018F;
	LCD->Instruction = MSK_IX_VER_SCROLL_CONTROL;  		LCD->Data = 0x0000;
	LCD_Delay(10000);


	// Window Address Control System
	LCD->Instruction = MSK_IX_HOR_RAM_ADDR_POSITION1;  	LCD->Data = 0x0000; 	// Horizontal Start Address, Y = 0
	LCD->Instruction = MSK_IX_HOR_RAM_ADDR_POSITION2;  	LCD->Data = 0x00EF; 	// Horizontal End Address, Y = 239
	LCD->Instruction = MSK_IX_VER_RAM_ADDR_POSITION1;  	LCD->Data = 0x0000; 	// Vertical Start Address, X = 0
	LCD->Instruction = MSK_IX_VER_RAM_ADDR_POSITION2;  	LCD->Data = 0x018F;		// Vertical End Address, X = 399
	// RAM Access System
	LCD->Instruction = MSK_IX_RAM_ADDR_SET1;  			LCD->Data = 0x0000; 	// Horizontal Address, Y = 0
	LCD->Instruction = MSK_IX_RAM_ADDR_SET2;  			LCD->Data = 0x0000;		// Vertical Address, X = 0
	LCD_Delay(10000);

	// Power Control System
	LCD->Instruction = MSK_IX_POWER_CONTROL1;  			LCD->Data = 0x4110;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;  		LCD->Data = 0x061C;
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0101;
	LCD->Instruction = 0x0007;  						LCD->Data = 0x0001;
	LCD_Delay(450000);
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL2;  		LCD->Data = 0x161B;	
	LCD->Instruction = MSK_IX_GATE_IF_CONTROL1;  		LCD->Data = 0x0101;
	LCD->Instruction = 0x0007;  						LCD->Data = 0x0007;
	LCD_Delay(10000);
	LCD->Instruction = MSK_IX_DISPLAY_CONTROL1;  		LCD->Data = 0x0113;


	/* ȭ�� �ʱ�ȭ*/
	LCD_ClearScreen(BLACK);
	LCD_WriteString(56, 8, "< �ַ�� ��ũ >", YELLOW, BLACK);
	LCD_WriteString(8, 24, " - MCU : STM32F103ZET", YELLOW, BLACK);
	LCD_WriteString(8, 40, " - Title : LCD Test Program", YELLOW, BLACK);	
	LCD_FillColor(0, 300, 30, 100, BLACK);
	LCD_FillColor(30, 300, 30, 100, WHITE);	
	LCD_FillColor(60, 300, 30, 100, YELLOW);
	LCD_FillColor(90, 300, 30, 100, RED);
	LCD_FillColor(120, 300, 30, 100, GREEN);
	LCD_FillColor(150, 300, 30, 100, BLUE);
	LCD_FillColor(180, 300, 30, 100, CYAN);
	LCD_FillColor(210, 300, 30, 100, MAGENTA);
	
}


/**
* @fn		void LCD_ClearScreen(COLOR Color)
* @brief
*	- LCD ȭ���� �����.   
* @remarks
* @param	COLOR Color	: ����   
* @return	void			
*/
void LCD_ClearScreen(COLOR Color)
{
	// ��ǥ �̵�  
	LCD->Instruction = MSK_IX_RAM_ADDR_SET1;  		LCD->Data = 0x0000;		// X�� �ʱ�ȭ  
	LCD->Instruction = MSK_IX_RAM_ADDR_SET2;  		LCD->Data = 0x0000;		// Y�� �ʱ�ȭ  
	// ������ ���� 
	LCD->Instruction = MSK_IX_RAM_DATA_RW;
	LCD_WriteDataRepeat(Color, (uint32_t)LCD_WIDTH * LCD_HEIGHT);	
}


/**
* @fn		void LCD_FillColor(uint16_t SX, uint16_t SY, uint16_t Width, uint16_t Height, COLOR Color)
* @brief
*	- �簢������ ���� ä���. 
* @remarks
* @param	uint16_t SX		: X ���� ��ġ   
* @param	uint16_t SY		: Y ���� ��ġ
* @param	uint16_t Width	: �� 
* @param	uint16_t Height	: ���� 
* @param	COLOR Color		: ����   
* @return	void			
*/
void LCD_FillColor(uint16_t SX, uint16_t SY, uint16_t Width, uint16_t Height, COLOR Color)
{
	unsigned short i;

	for(i = 0; i < Height; i++)
	{
		// ��ǥ �̵� 
		LCD->Instruction = MSK_IX_RAM_ADDR_SET1;  		LCD->Data = SX;			// X�� �ʱ�ȭ  
		LCD->Instruction = MSK_IX_RAM_ADDR_SET2;  		LCD->Data = SY + i;		// Y�� �̵� 
		// ������ ���� 
		LCD->Instruction = MSK_IX_RAM_DATA_RW;
		LCD_WriteDataRepeat(Color, Width);					// X ������ 1������ ä���. 
	}
}

/**
* @fn		void LCD_WriteEnglish(uint16_t SX, uint16_t SY, uint8_t Data, COLOR Color, COLOR BG_Color)
* @brief
*	- ��� LCD�� ǥ���Ѵ�  
* @remarks
* @param	uint16_t SX		: X ��ġ   
* @param	uint16_t SY		: Y ��ġ
* @param	uint8_t Data		: ����
* @param	COLOR Color		: ���� ����   
* @param	COLOR BG_Color	: ��� ����   
* @return	void			
*/
void LCD_WriteEnglish(uint16_t SX, uint16_t SY, uint8_t Data, COLOR Color, COLOR BG_Color)
{
	uint8_t i,j, Font;
	uint16_t Index;

    // ������ �ε��� ���� ã�´�.
	Index = EngFont_IndexTable[Data];
    if(Index == 0xFF)		// ��Ʈ��  Index�� ���� ��� 
    {
	#ifdef	LCD_DUBUG
        Serial_WriteString(SERIAL_PORT1, "\r\n<Error> Can't find eng font index : ");
        Serial_WriteHexa(SERIAL_PORT1, Font);
	#endif
        return;
    }

	Index = Index * LCD_FONT_ENG_SIZE;

	for(j = 0; j < 16; j++)
	{
		Font = EngFont[j + Index];
		// ��ǥ �̵� 
		LCD->Instruction = MSK_IX_RAM_ADDR_SET1;  		LCD->Data = SX;			// X�� �ʱ�ȭ  
		LCD->Instruction = MSK_IX_RAM_ADDR_SET2;  		LCD->Data = SY + j;		// Y�� �̵� 		
		// ������ ���� 
		LCD->Instruction = MSK_IX_RAM_DATA_RW;

		for(i = 0; i < 8; i++)
		{
			(Font & (0x80 >> i))? (LCD->Data = Color): (LCD->Data = BG_Color);
		}
	}
}

/**
* @fn		void LCD_WriteKorean(uint16_t SX, uint16_t SY, uint16_t Data, COLOR Color, COLOR BG_Color)
* @brief
*	- �ѱ۸� LCD�� ǥ���Ѵ�  
* @remarks
* @param	uint16_t SX		: X ��ġ   
* @param	uint16_t SY		: Y ��ġ
* @param	uint16_t Data		: ����
* @param	COLOR Color		: ���� ����   
* @param	COLOR BG_Color	: ��� ����   
* @return	void			
*/
void LCD_WriteKorean(uint16_t SX, uint16_t SY, uint16_t Data, COLOR Color, COLOR BG_Color)
{
	uint8_t i,j;
	uint16_t Font, Index;

    // ������ �ε��� ���� ã�´�.
    Index = LCD_GetKFontIndex(Data);
	
    if(Index == 0xFFFF)		// ��Ʈ��  Index�� ���� ��� 
    {
#ifdef	LCD_DUBUG
        Serial_WriteString(SERIAL_PORT1, "\r\n<Error> Can't find kor font index : ");
        Serial_WriteString(SERIAL_PORT1, Font);
#endif
        return;
    }

	Index = Index * LCD_FONT_KOR_SIZE;	

	for(j = 0; j < 16; j++)
	{
		// Font �����͸� ������ �´�.( �ѱ��� �� �������� 16���� �Ǿ� �����Ƿ� 2����Ʈ�� ������ �´�) 
		Font = KorFont[j *2 + Index];
		Font = (Font << 8) + KorFont[j *2 + 1 + Index];
		// ��ǥ �̵� 
		LCD->Instruction = MSK_IX_RAM_ADDR_SET1;  		LCD->Data = SX;			// X�� �ʱ�ȭ  
		LCD->Instruction = MSK_IX_RAM_ADDR_SET2;  		LCD->Data = SY + j;		// Y�� �̵� 		
		// ������ ���� 
		LCD->Instruction = MSK_IX_RAM_DATA_RW;

		for(i = 0; i < 16; i++)
		{
			(Font & (0x8000 >> i))? (LCD->Data = Color) : (LCD->Data = BG_Color);	
		}		
	}
}

/**
* @fn		void LCD_WriteString(uint16_t SX, uint16_t SY,const char *String, COLOR Color, COLOR BG_Color)
* @brief
*	- ���ڿ��� LCD�� ǥ���Ѵ�  
* @remarks
* @param	uint16_t SX		: X ��ġ   
* @param	uint16_t SY		: Y ��ġ
* @param	const char *String	: ���ڿ�
* @param	COLOR Color		: ���� ����   
* @param	COLOR BG_Color	: ��� ����   
* @return	void			
*/
void LCD_WriteString(uint16_t SX, uint16_t SY,const char *String, COLOR Color, COLOR BG_Color)
{
    uint16_t Data;

    while(*(String) != 0)
    {
        // �������� �ѱ����� ����( �ѱ��� 0xB000 �̻� ��ġ�Ѵ�.)
        if(*String < 0x80)      // ����
        {
            Data = *String;
            LCD_WriteEnglish(SX, SY, (uint8_t)Data, Color, BG_Color);
            String++;
            SX += 8;
        }
        else                    // �ѱ�
        {
            Data = (*String << 8) + *(String + 1);
            LCD_WriteKorean(SX, SY, Data, Color, BG_Color);
            String += 2;
            SX += 16;
        }
    }
}

/**
* @fn		void LCD_DrawPixel(UWORD SX, UWORD SY, COLOR Color)
* @brief
*	- ���� LCD�� ǥ���Ѵ�  
* @remarks
* @param	UWORD SX		: X ��ġ   
* @param	UWORD SY		: Y ��ġ
* @param	COLOR Color		: ����   
* @return	void			
*/
void LCD_DrawPixel(uint16_t SX, uint16_t SY, COLOR Color)
{
	/* ��ǥ �̵� */ 
	LCD->Instruction = MSK_IX_RAM_ADDR_SET1; LCD->Data = SX; 	// X�� �̵�  
	LCD->Instruction = MSK_IX_RAM_ADDR_SET2; LCD->Data =SY; 	// Y�� �̵� 
	/* ������ ���� */ 
	LCD->Instruction = MSK_IX_RAM_DATA_RW;
	LCD->Data = Color;											// ���� ��´�.  
}



/**
* @fn		static void LCD_Reset(void)
* @brief
*	- LCD�� ���½�Ų��.
* @remarks
* @param	void
* @return	void			
*/
static void LCD_Reset(void)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
	LCD_Delay(1000);	
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
	LCD_Delay(100000);
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
}

/**
* @fn		static void LCD_Delay(uint32_t Delay)
* @brief
*	- ���� �Լ� 
*	- 0.5��: 2000000
* @remarks
* @param	void
* @return	void			
*/
static void LCD_Delay(uint32_t Delay)
{
	volatile uint32_t i;

	for(i = 0; i < Delay;i++)
	{
		i = i;
	}
}



/**
* @fn		static void LCD_WriteDataRepeat(uint16_t Data, uint32_t Repeat)
* @brief
*	- LCD�� �����͸� �ݺ��ؼ� ����. 
* @remarks
* @param	uint16_t			: LCD ������   
* @param	uint32_t Repeat	: �ݺ�Ƚ��   
* @return	void			
*/
static void LCD_WriteDataRepeat(uint16_t Data, uint32_t Repeat)
{
	uint32_t i;

	for(i = 0; i < Repeat; i++)
	{
		LCD->Data = Data;
	}	
}


/**
* @fn		static uint16_t LCD_GetKFontIndex(uint16_t Code)
* @brief
*	- �ѱ� ��Ʈ�� Index�� ���Ѵ�.
* @remarks
* @param	unsigned short Code	: �ѱ� �ڵ�     
* @return	void			
*/
static uint16_t LCD_GetKFontIndex(uint16_t Code)
{
	uint16_t left = 0;
	uint16_t right = sizeof(KorFont_IndexTable)/sizeof(uint16_t);
	uint16_t mid;
	uint16_t tmp;

	while (left <= right) 
	{
		mid = (right + left) / 2;
		tmp = KorFont_IndexTable[mid];
		if (tmp == Code)
			return mid;

		if (Code < tmp)
			right = mid - 1;
		else
			left = mid + 1;
	}

	return 0xFFFF;	// not found, return the first character
}



