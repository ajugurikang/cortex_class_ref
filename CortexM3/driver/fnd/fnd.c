/**
* @file		stm32f10x_fnd.c
* @brief		FND ����̹� 
* @author		���̼�	
* @remarks
* @par �����̷� 
*	@li	2012.12.10 : �ʱ�  �Լ� �ۼ�  
*/

#include "stm32f10x.h"
#include "timer.h"


#define FND *((uint16_t *)0x6C000008)

#define FND_NUMBER		4

static void FND_DisplayLoop(void);



                	       // 0,        1,        2,       3,         4,        5,        6,        7,       8,        9,        X
const uint8_t FND_Font[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xFF};
uint8_t FND_Data[FND_NUMBER]= {0xFF, 0xFF, 0xFF, 0xFF};



/**
* @fn		void FND_Init(void)
* @brief
*	@li		FND �ʱ�ȭ  
* @remarks
* @param	void
* @return	void			
*/
void FND_Init(void)
{

	/* Timer Callback �Լ� ��� : 5ms ���� ȣ�� �ǵ��� �Ѵ�. */
	Alarm_Open(ALARM0, 5, FND_DisplayLoop);


}

void FND_On(uint8_t Data, uint8_t Location, uint8_t Dot)
{
	uint16_t Output;

	/********************** Segment **********************/
	// ���� 8��Ʈ : Segment �� 
	 Output = FND_Font[Data];
	// Dot �� ����Ѵ�. 
	if(Dot != 0)
	{
		Output &= 0x7F;
	}
	/********************** Common **********************/
	// ���� 8��Ʈ��������4��Ʈ : Common
	Output =(Output | 0xFF00) & (~(1 << (Location + 8)));

	/* ������ ��� */
	FND = Output;
}



void FND_Write(uint8_t Data, uint8_t Location, uint8_t Dot)
{
	/* Segment �� ��� */
	 Data = FND_Font[Data];		// ���� 8��Ʈ : Segment �� 
	if(Dot != 0)				// Dot �� ����Ѵ�. 
	{
		Data &= 0x7F;
	}
	/* ���ۿ� ������ ���� */
	FND_Data[Location] = Data;
}

static void FND_DisplayLoop(void)
{
	static uint8_t LoopNum = 0;
	uint16_t Output;

	LoopNum++;
	LoopNum = LoopNum % 4;
 
	/* Segment �� ��� */
	Output = FND_Data[LoopNum];
	/* ���� 8��Ʈ��������4��Ʈ : Common �� ��� */
	Output =(Output | 0xFF00) & (~(1 << (LoopNum + 8)));

	FND = Output;
}

