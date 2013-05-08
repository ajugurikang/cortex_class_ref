/**
* @file		stm32f10x_uart.c
* @brief		UART 드라이버 
* @author		강이석	
* @remarks
* @par 수정이력 
*	@li	2012.12.10 : 초기  함수 작성  
*	@li	2013.01.05 :   Serial_WriteHexa()를 8비트에서 32비트로 수정 

*/


#include "stm32f10x.h"
#include "serial.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

/* DMA 사용시 */
#include "stm32f10x_dma.h"					
static void Serial_DMA_Configuration(void); 	
uint8_t SerialBuffer[16];					


SERIAL_CALLBACK SerialCallbackTable[SERIAL_PORT_MAX] = {(void *)0, };
const uint8_t *HexaString = "0123456789ABCDEF";


/**
* @fn		void Serial_Open(SERIAL_PORT Port, SERIAL_CALLBACK Callback)
* @brief
*	@li		시리얼 포트를 연다.
* @remarks
* @param	SERIAL_PORT Port				: 시리얼포트
* @param	uint32_t Baudrate				: Baudrate
* @param	SERIAL_CALLBACK Callback		: 시리얼로 데이터를 받을 때 콜백함수 
* @return	void			
*/
void Serial_Open(SERIAL_PORT Port, uint32_t Baudrate, SERIAL_CALLBACK Callback)
{
	uint32_t TempReg, APBClock, USARTDiv;
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_TypeDef *USART_Port;
	RCC_ClocksTypeDef RCC_ClocksStatus;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	switch(Port)
	{
		case SERIAL_PORT1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);		// USART1 클럭 활성화 
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;						// USART1_TX(PA9) : 출력핀 
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(GPIOA, &GPIO_InitStruct);
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;						// USART1_RX(PA10) : 입력핀(Pull up / Floating)		
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(GPIOA, &GPIO_InitStruct);
			USART_Port = USART1;
			Serial_DMA_Configuration();		// DMA 테스트 용 
			break;
		case SERIAL_PORT2 :			
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);		// USART2 클럭 활성화 
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;						// USART2_TX(PA2) : 출력핀 
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(GPIOA, &GPIO_InitStruct);	
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;						// USART2_RX(PA3) : 입력핀(Pull up / Floating)		
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(GPIOA, &GPIO_InitStruct);	
			USART_Port = USART2;
			break;
		default :
			break;
	}

	/***** CR1 설정(Control Register 1)
	UE : USART enable  
	M: Word length (0 = 1 Start bit, 8 Data bits, nStop bit,   1 = Start bit, 9 Data bits, n Stop bit)  
	WAKE: Wakeup method 
	PCE: Parity control enable (0 = Parity control disable, 1 = Parity control enable)
	PS: Parity selection ( 0 = Even parity, 1 = Odd parity)
	PEIE: PE(Parity Error) interrupt enable 
	TXEIE: TXE interrupt enable
	TCIE: Transmission complete interrupt enable 
	RXNEIE: RXNE interrupt enable
	IDLEIE: IDLE interrupt enable
	TE: Transmitter enable
	RE: Receiver enable
	RWU: Receiver wakeup
	SBK: Send break
	**/
	// 
	TempReg = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;
	USART_Port->CR1 = TempReg;

	USART_Port->CR3 |= USART_CR3_DMAR;		// Receive DMA 활성화 
	
	/*****  CR1 설정(Control Register 1)
	LINEN: LIN mode enable (0 = LIN mode disabled,  1 = LIN mode enabled)
	STOP: STOP bits (00 = 1 Stop bit, 01 = 0.5 Stop bit, 10 = 2 Stop bits, 11 = 1.5 Stop bit)
	CLKEN: Clock enable (0 = CK pin disabled, 1 = CK pin enabled)
	CPOL: Clock polarity (0 = Steady low value on CK pin outside transmission window,  1 = Steady high value on CK pin outside transmission window)
	CPHA: Clock phase ( 0 = The first clock transition is the first data capture edge,  1 = The second clock transition is the first data capture edge)
	LBCL: Last bit clock pulse
	LBDIE: LIN break detection interrupt enable (0: Interrupt is inhibited)
	LBDL: lin break detection length (0 = 10 bit break detection,  1 = 11 bit break detection)
	ADD[3:0]: Address of the USART node
	**/
	TempReg = 0;
	USART_Port->CR2 = TempReg;	

	/*****  Baudrate 계산 (Baudrate = fck/(16*USARTDIV) *****/
	// 1.  버스 클럭을 가지고 온다. 
	RCC_GetClocksFreq(&RCC_ClocksStatus);	
	if (Port == SERIAL_PORT1)		// USART1는 APB2 를 사용한다. 
	{
	  APBClock = RCC_ClocksStatus.PCLK2_Frequency;
	}
	else							// USART1이 아닌  것은  APB1 를 사용한다. 
	{
	  APBClock = RCC_ClocksStatus.PCLK1_Frequency;
	}

	/* USARTDIV = fck/(16*Baudrate)   
	BRR 레지스터는  USARTDIV의 정수 부분(matissa : 12비트)과 소수 부분(Fraction : 4비트)로 나누어 진다. 	
	소수 부분(4비트)을 정수 부분으로 계산하기 위해 전체적으로 x16를 한다. 그런데 반올림 계산을 하기 위해
	x32를 곱한 다음 최하위 비트가 1 이면 반올림 해야 하므로 오른쪽으로 1 쉬프트(자리 복귀) 한다음 +1(반올림) 한다.  
	*/
	USARTDiv = APBClock * 2 / Baudrate;		// fck / (16 * Baudrate) * 32
	/* 반올림 해야 한는지 검사  */
	if(USARTDiv	& 0x00000001)	// 비트0이 1 이면 반올림 해야 됨. 
	{
		USARTDiv >>= 1;
		USARTDiv += 1;
	}
	else		// 반올림이 필요없음 
	{
		USARTDiv >>= 1;		
	}	
	USART_Port->BRR = USARTDiv;


	/***** Enable the UART Interrupt *****/
	switch(Port)			// 인터럽트 소스 결정 
	{
		case SERIAL_PORT1 :
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
			break;
		case SERIAL_PORT2 :
			NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
			break;
		default :
			break;
	}
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/***** 콜백 함수를 등록한다. *****/
	SerialCallbackTable[Port] = Callback;

}


/**
* @fn		void Serial_Open(SERIAL_PORT Port, SERIAL_CALLBACK Callback)
* @brief
*	@li		시리얼 포트를 닫는다.
* @remarks
* @param	SERIAL_PORT Port				: 시리얼포트
* @return	void			
*/
void Serial_Close(SERIAL_PORT Port)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/***** Disable the UART Interrupt *****/
	switch(Port)			// 인터럽트 소스 결정 
	{
		case SERIAL_PORT1 :
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
			break;
		case SERIAL_PORT2 :
			NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
			break;
		default :
			break;
	}
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	/***** 콜백 함수를 등록 취소한다. *****/	
	SerialCallbackTable[Port] = (void *)0;
}




/**
* @fn		void Serial_WriteByte(SERIAL_PORT Port, uint8_t Data)
* @brief
*	@li		UART 로 한 바이트를 보낸다.
* @remarks
* @param	USART_TypeDef* USARTx		: 시리얼포트
* @param	uint8_t Data					: 데이터 
* @return	void			
*/
void Serial_WriteByte(SERIAL_PORT Port, uint8_t Data)
{
	USART_TypeDef *USART_Port;

	switch(Port)
	{
		case SERIAL_PORT1:
			USART_Port = USART1;
			break;
		case SERIAL_PORT2 :
			USART_Port = USART2;
			break;
		default :
			break;
	}
	while((USART_Port->SR & USART_SR_TXE) == 0);	// Data register가 빌 때가지 기다림 
	
	USART_Port->DR = (uint16_t)Data;
	
}

/**
* @fn		void UART_WriteHexa(USART_TypeDef* USARTx, uint32_t Data)
* @brief
*	@li		UART 로 4 바이트를 값을 헥사 아스키로 코드로 보낸다
* @remarks
* @param	USART_TypeDef* USARTx		: 시리얼포트, USARTx에서 x는 1 ~ 5 가 될 수 있다
* @param	uint8_t Data					: 데이터 
* @return	void			
*/
void Serial_WriteHexa(SERIAL_PORT Port, uint32_t Data)
{
	uint8_t Temp , i;

	Serial_WriteByte(Port, '0');
	Serial_WriteByte(Port, 'x');

	for(i = 0;i < 8; i++)
	{
		// 1. 4비트를 가지고 온다. 
		Temp = (Data >> (28 - 4*i))& 0x0F;
		// 2. 아스키 코드로 변환한다. 
		Temp = HexaString[Temp];
		// 3. UART로 출력한다. 
		Serial_WriteByte(Port, Temp);
	}	
}

/**
* @fn		void Serial_WriteString(SERIAL_PORT Port, uint8_t* String)
* @brief
*	@li		UART 로 문자열을 보낸다.
* @remarks
* @param	USART_TypeDef* USARTx		: 시리얼포트, USARTx에서 x는 1 ~ 5 가 될 수 있다
* @param	uint8_t* String				: 문자열 
* @return	void			
*/
void Serial_WriteString(SERIAL_PORT Port, uint8_t* String)
{
	while(*String != 0)
	{
		Serial_WriteByte(Port, *String);
		String++;
	}
}

static void Serial_DMA_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;

	/* DMA1 클럭 활성화 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);


    /* DMA1 channel1 configuration */
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SerialBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 16;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    /* Enable DMA1 Channel1 */
    DMA_Cmd(DMA1_Channel5, ENABLE);
}





/**
* @fn		void USART1_IRQHandler(void)
* @brief
*	@li		UART1 ISR(Interrupt Service Routine)
* @remarks
* @param	void
* @return	void			
*/

void USART1_IRQHandler(void)
{
	/*RXNE 플래그를 클리어 한다.  */
	USART1->SR &= (~USART_SR_RXNE);
	
	if(SerialCallbackTable[SERIAL_PORT1] != (void *)0)
	{
		SerialCallbackTable[SERIAL_PORT1](USART1->DR);
	}
}


/**
* @fn		void USART2_IRQHandler(void)
* @brief
*	@li		UART2 ISR(Interrupt Service Routine)
* @remarks
* @param	void
* @return	void			
*/
void USART2_IRQHandler(void)
{
	/*RXNE 플래그를 클리어 한다.  */
	USART2->SR &= (~USART_SR_RXNE);
	
	if(SerialCallbackTable[SERIAL_PORT2] != (void *)0)
	{
		SerialCallbackTable[SERIAL_PORT2](USART2->DR);
	}
}






