/**
* @file		stm32f10x_fnd.c
* @brief		FND 드라이버 
* @author		강이석	
* @remarks
* @par 수정이력 
*	@li	2012.12.10 : 초기  함수 작성  
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
*	@li		FND 초기화  
* @remarks
* @param	void
* @return	void			
*/
void FND_Init(void)
{

	/* Timer Callback 함수 등록 : 5ms 마다 호출 되도록 한다. */
	Alarm_Open(ALARM0, 5, FND_DisplayLoop);


}

void FND_On(uint8_t Data, uint8_t Location, uint8_t Dot)
{
	uint16_t Output;

	/********************** Segment **********************/
	// 하위 8비트 : Segment 값 
	 Output = FND_Font[Data];
	// Dot 를 계산한다. 
	if(Dot != 0)
	{
		Output &= 0x7F;
	}
	/********************** Common **********************/
	// 상위 8비트에서하위4비트 : Common
	Output =(Output | 0xFF00) & (~(1 << (Location + 8)));

	/* 데이터 출력 */
	FND = Output;
}



void FND_Write(uint8_t Data, uint8_t Location, uint8_t Dot)
{
	/* Segment 값 계산 */
	 Data = FND_Font[Data];		// 하위 8비트 : Segment 값 
	if(Dot != 0)				// Dot 를 계산한다. 
	{
		Data &= 0x7F;
	}
	/* 버퍼에 데이터 저장 */
	FND_Data[Location] = Data;
}

static void FND_DisplayLoop(void)
{
	static uint8_t LoopNum = 0;
	uint16_t Output;

	LoopNum++;
	LoopNum = LoopNum % 4;
 
	/* Segment 값 계산 */
	Output = FND_Data[LoopNum];
	/* 상위 8비트에서하위4비트 : Common 값 계산 */
	Output =(Output | 0xFF00) & (~(1 << (LoopNum + 8)));

	FND = Output;
}

