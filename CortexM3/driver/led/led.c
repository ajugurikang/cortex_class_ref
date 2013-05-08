/**
* @file		stm32f10x_led.c
* @brief		LED 드라이버 
* @author		강이석	
* @remarks
* @par 수정이력 
*	@li	2012.12.10 : 초기  함수 작성  
*/

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

//#define LED_METHOD_GPIO
#define LED_METHOD_EXTIF



/** 
  * @brief  LED에 할당된 포트 정의 
  */

#define LED_0_PIN		GPIO_Pin_14
#define LED_1_PIN		GPIO_Pin_15
#define LED_2_PIN		GPIO_Pin_0
#define LED_3_PIN		GPIO_Pin_1
#define LED_4_PIN		GPIO_Pin_7
#define LED_5_PIN		GPIO_Pin_8
#define LED_6_PIN		GPIO_Pin_9
#define LED_7_PIN		GPIO_Pin_10
#define LED_A1_PIN		GPIO_Pin_1
#define LED_A2_PIN		GPIO_Pin_2
#define LED_NE4_PIN		GPIO_Pin_12
#define LED_NWE_PIN		GPIO_Pin_5

#define LED_0_PORT		GPIOD
#define LED_1_PORT		GPIOD
#define LED_2_PORT		GPIOD
#define LED_3_PORT		GPIOD
#define LED_4_PORT		GPIOE
#define LED_5_PORT		GPIOE
#define LED_6_PORT		GPIOE
#define LED_7_PORT		GPIOE
#define LED_A1_PORT		GPIOF
#define LED_A2_PORT		GPIOF
#define LED_NE4_PORT	GPIOG
#define LED_NWE_PORT	GPIOD


/** 
  * @brief  LED 포트 구조체 정의  
  */
typedef struct{
	GPIO_TypeDef * Port;
	uint16_t Pin;
}LED_PortDef;

typedef struct{
	volatile uint16_t Addr;
}LED_Typedef;

const LED_PortDef LED_PortTable[] = {{LED_0_PORT, LED_0_PIN}, {LED_1_PORT, LED_1_PIN}, {LED_2_PORT, LED_2_PIN}, {LED_3_PORT, LED_3_PIN},  
								{LED_4_PORT, LED_4_PIN}, {LED_5_PORT, LED_5_PIN}, {LED_6_PORT, LED_6_PIN}, {LED_7_PORT, LED_7_PIN}};


#define LED_BASE        ((uint32_t)(0x6C000004))
#define LED             (*(volatile uint16_t *)0x6C000004)



/**
* @fn		void LED_Init(void)
* @brief
*	@li		LED 초기화  
* @remarks
* @param	void
* @return	void			
*/
void LED_Init(void)
{
	#ifdef LED_METHOD_GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIO 초기화 */ 
	/* 1. GPIO 클럭 활성화 */ 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);
	
	/* 2. 포트 초기화 */ 
	 /* 2-1 포트 D 설정 (D0 ~ D3, FSMC_NWE) */
	 GPIO_InitStructure.GPIO_Pin = LED_0_PIN | LED_1_PIN | LED_2_PIN | LED_3_PIN | LED_NWE_PIN;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	 GPIO_Init(GPIOD, &GPIO_InitStructure);
	 /* 2-2 포트 E 설정 (D4 ~ D7) */
	 GPIO_InitStructure.GPIO_Pin = LED_4_PIN | LED_5_PIN | LED_6_PIN | LED_7_PIN;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	 GPIO_Init(GPIOE, &GPIO_InitStructure);
	 /* 2-3 포트 F 설정 (A1, A2) */
	 GPIO_InitStructure.GPIO_Pin = LED_A1_PIN | LED_A2_PIN;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	 GPIO_Init(GPIOF, &GPIO_InitStructure);
	 /* 2-4 포트 G 설정 (FSMC_NE4) */
	 GPIO_InitStructure.GPIO_Pin = LED_NE4_PIN;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	 GPIO_Init(GPIOG, &GPIO_InitStructure);

	#else
	#ifdef LED_METHOD_EXTIF

	


	#endif
	#endif

}

/**
* @fn		void LED_On(uint8 Data)
* @brief
*	@li		LED 를 점등한다.   
* @remarks
* @param	uint8 Data			:  LED를 킬 데이터 
* @return	void			
*/
void LED_On(uint8_t Data)
{
	#ifdef LED_METHOD_GPIO

	uint8_t i;


	// 1. LED 데이터를 출력한다. (D0 ~ D7)
	for(i = 0; i < 8; i++)
	{
		if(Data & (1 << i))		// 비트가 1인 경우 
		{
			GPIO_WriteBit(LED_PortTable[i].Port, LED_PortTable[i].Pin, Bit_SET);
		}
		else					// 비트가 0인 경우 
		{
			GPIO_WriteBit(LED_PortTable[i].Port, LED_PortTable[i].Pin, Bit_RESET);
		}
	}

	// 2. 어드레스 디코딩 데이터를 출력(A1 =1, A2 = 0, NE4 = 0)
	GPIO_WriteBit(LED_A1_PORT, LED_A1_PIN, Bit_SET);
	GPIO_WriteBit(LED_A2_PORT, LED_A2_PIN, Bit_RESET);
	GPIO_WriteBit(LED_NE4_PORT, LED_NE4_PIN, Bit_RESET);

	// 3.  LE 신호 출력 (NWE = 1 -> 0 -> 1)	
	GPIO_WriteBit(LED_NWE_PORT, LED_NWE_PIN, Bit_SET);
	GPIO_WriteBit(LED_NWE_PORT, LED_NWE_PIN, Bit_RESET);
	GPIO_WriteBit(LED_NWE_PORT, LED_NWE_PIN, Bit_SET);
	#else
//	#ifdef LED_METHOD_EXTIF

	LED = (uint16_t)Data;
//	LED->Addr = (uint16_t)Data;

//	#endif
	#endif	
}






