/**
* @file		stm32f10x_uart.c
* @brief		UART ����̹� 
* @author		���̼�	
* @remarks
* @par �����̷� 
*	@li	2012.12.10 : �ʱ�  �Լ� �ۼ�  
*	@li	2013.01.05 :   Serial_WriteHexa()�� 8��Ʈ���� 32��Ʈ�� ���� 

*/


#include "stm32f10x.h"
#include "serial.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

/* DMA ���� */
#include "stm32f10x_dma.h"					
static void Serial_DMA_Configuration(void); 	
uint8_t SerialBuffer[16];					


SERIAL_CALLBACK SerialCallbackTable[SERIAL_PORT_MAX] = {(void *)0, };
const uint8_t *HexaString = "0123456789ABCDEF";


/**
* @fn		void Serial_Open(SERIAL_PORT Port, SERIAL_CALLBACK Callback)
* @brief
*	@li		�ø��� ��Ʈ�� ����.
* @remarks
* @param	SERIAL_PORT Port				: �ø�����Ʈ
* @param	uint32_t Baudrate				: Baudrate
* @param	SERIAL_CALLBACK Callback		: �ø���� �����͸� ���� �� �ݹ��Լ� 
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
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);		// USART1 Ŭ�� Ȱ��ȭ 
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;						// USART1_TX(PA9) : ����� 
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(GPIOA, &GPIO_InitStruct);
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;						// USART1_RX(PA10) : �Է���(Pull up / Floating)		
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(GPIOA, &GPIO_InitStruct);
			USART_Port = USART1;
			Serial_DMA_Configuration();		// DMA �׽�Ʈ �� 
			break;
		case SERIAL_PORT2 :			
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);		// USART2 Ŭ�� Ȱ��ȭ 
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;						// USART2_TX(PA2) : ����� 
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(GPIOA, &GPIO_InitStruct);	
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;						// USART2_RX(PA3) : �Է���(Pull up / Floating)		
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(GPIOA, &GPIO_InitStruct);	
			USART_Port = USART2;
			break;
		default :
			break;
	}

	/***** CR1 ����(Control Register 1)
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

	USART_Port->CR3 |= USART_CR3_DMAR;		// Receive DMA Ȱ��ȭ 
	
	/*****  CR1 ����(Control Register 1)
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

	/*****  Baudrate ��� (Baudrate = fck/(16*USARTDIV) *****/
	// 1.  ���� Ŭ���� ������ �´�. 
	RCC_GetClocksFreq(&RCC_ClocksStatus);	
	if (Port == SERIAL_PORT1)		// USART1�� APB2 �� ����Ѵ�. 
	{
	  APBClock = RCC_ClocksStatus.PCLK2_Frequency;
	}
	else							// USART1�� �ƴ�  ����  APB1 �� ����Ѵ�. 
	{
	  APBClock = RCC_ClocksStatus.PCLK1_Frequency;
	}

	/* USARTDIV = fck/(16*Baudrate)   
	BRR �������ʹ�  USARTDIV�� ���� �κ�(matissa : 12��Ʈ)�� �Ҽ� �κ�(Fraction : 4��Ʈ)�� ������ ����. 	
	�Ҽ� �κ�(4��Ʈ)�� ���� �κ����� ����ϱ� ���� ��ü������ x16�� �Ѵ�. �׷��� �ݿø� ����� �ϱ� ����
	x32�� ���� ���� ������ ��Ʈ�� 1 �̸� �ݿø� �ؾ� �ϹǷ� ���������� 1 ����Ʈ(�ڸ� ����) �Ѵ��� +1(�ݿø�) �Ѵ�.  
	*/
	USARTDiv = APBClock * 2 / Baudrate;		// fck / (16 * Baudrate) * 32
	/* �ݿø� �ؾ� �Ѵ��� �˻�  */
	if(USARTDiv	& 0x00000001)	// ��Ʈ0�� 1 �̸� �ݿø� �ؾ� ��. 
	{
		USARTDiv >>= 1;
		USARTDiv += 1;
	}
	else		// �ݿø��� �ʿ���� 
	{
		USARTDiv >>= 1;		
	}	
	USART_Port->BRR = USARTDiv;


	/***** Enable the UART Interrupt *****/
	switch(Port)			// ���ͷ�Ʈ �ҽ� ���� 
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

	/***** �ݹ� �Լ��� ����Ѵ�. *****/
	SerialCallbackTable[Port] = Callback;

}


/**
* @fn		void Serial_Open(SERIAL_PORT Port, SERIAL_CALLBACK Callback)
* @brief
*	@li		�ø��� ��Ʈ�� �ݴ´�.
* @remarks
* @param	SERIAL_PORT Port				: �ø�����Ʈ
* @return	void			
*/
void Serial_Close(SERIAL_PORT Port)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/***** Disable the UART Interrupt *****/
	switch(Port)			// ���ͷ�Ʈ �ҽ� ���� 
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
	
	/***** �ݹ� �Լ��� ��� ����Ѵ�. *****/	
	SerialCallbackTable[Port] = (void *)0;
}




/**
* @fn		void Serial_WriteByte(SERIAL_PORT Port, uint8_t Data)
* @brief
*	@li		UART �� �� ����Ʈ�� ������.
* @remarks
* @param	USART_TypeDef* USARTx		: �ø�����Ʈ
* @param	uint8_t Data					: ������ 
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
	while((USART_Port->SR & USART_SR_TXE) == 0);	// Data register�� �� ������ ��ٸ� 
	
	USART_Port->DR = (uint16_t)Data;
	
}

/**
* @fn		void UART_WriteHexa(USART_TypeDef* USARTx, uint32_t Data)
* @brief
*	@li		UART �� 4 ����Ʈ�� ���� ��� �ƽ�Ű�� �ڵ�� ������
* @remarks
* @param	USART_TypeDef* USARTx		: �ø�����Ʈ, USARTx���� x�� 1 ~ 5 �� �� �� �ִ�
* @param	uint8_t Data					: ������ 
* @return	void			
*/
void Serial_WriteHexa(SERIAL_PORT Port, uint32_t Data)
{
	uint8_t Temp , i;

	Serial_WriteByte(Port, '0');
	Serial_WriteByte(Port, 'x');

	for(i = 0;i < 8; i++)
	{
		// 1. 4��Ʈ�� ������ �´�. 
		Temp = (Data >> (28 - 4*i))& 0x0F;
		// 2. �ƽ�Ű �ڵ�� ��ȯ�Ѵ�. 
		Temp = HexaString[Temp];
		// 3. UART�� ����Ѵ�. 
		Serial_WriteByte(Port, Temp);
	}	
}

/**
* @fn		void Serial_WriteString(SERIAL_PORT Port, uint8_t* String)
* @brief
*	@li		UART �� ���ڿ��� ������.
* @remarks
* @param	USART_TypeDef* USARTx		: �ø�����Ʈ, USARTx���� x�� 1 ~ 5 �� �� �� �ִ�
* @param	uint8_t* String				: ���ڿ� 
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

	/* DMA1 Ŭ�� Ȱ��ȭ */
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
	/*RXNE �÷��׸� Ŭ���� �Ѵ�.  */
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
	/*RXNE �÷��׸� Ŭ���� �Ѵ�.  */
	USART2->SR &= (~USART_SR_RXNE);
	
	if(SerialCallbackTable[SERIAL_PORT2] != (void *)0)
	{
		SerialCallbackTable[SERIAL_PORT2](USART2->DR);
	}
}






