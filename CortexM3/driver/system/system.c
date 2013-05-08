/**
* @file			system.c
* @brief		system ����̹� 
* @author		���̼�	
* @remarks
* @par �����̷� 
*	@li	2012.12.07 : �ʱ�  �Լ� �ۼ�  
*/


/** @addtogroup CortexM3_Driver
* @{
*/


/** @addtogroup System
*@{
*/



#include "stm32f10x.h"
#include "system_stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"
#include "misc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "system.h"

/** @defgroup System_private_Funtions 
*@{
*/

/**
*@}
*/

/**@defgroup System_private_Variables
*@{
*/

SYSTICK_CALLBACK SysTick_Callback = (void *)0;	/*!< SysTick �ݹ� �Լ����� ����  */


/**
*@}
*/



/**
*@addtogroup System_Exported_Functions
*@{
*/


/**
* @fn		void System_ClockConfig(void)
* @brief
*	@li		�ý��� Ŭ���� �����Ѵ�. 
* @remarks
* @param	void 
* @return	void			
*/
void System_ClockConfig(void)
{

	__IO ErrorStatus HSEStartUpStatus = SUCCESS;

	/* RCC system reset(for debug purpose) */
	RCC_DeInit();
	
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	
	if(HSEStartUpStatus == SUCCESS)
	{
	  /* Enable Prefetch Buffer */
	  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	
	  /* Flash 2 wait state */
	  FLASH_SetLatency(FLASH_Latency_2);
	  
	  /* AHB Ŭ������ :  HCLK = SYSCLK  = 72MHz */
	  RCC_HCLKConfig(RCC_SYSCLK_Div1); 
	
	  /* APB2( high-speed ) Ŭ�� ���� PCLK2 = HCLK = 72MHz */
	  RCC_PCLK2Config(RCC_HCLK_Div1); 
	
	  /* APB1 (low-speed) Ŭ�� ����  PCLK1 = HCLK/2 = 36MHz */
	  RCC_PCLK1Config(RCC_HCLK_Div2);
	
	  /* PLL ���� : PLLCLK = 8MHz * 9 = 72 MHz */
	  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
	
	  /* Enable PLL */ 
	  RCC_PLLCmd(ENABLE);
	
	  /* Wait till PLL is ready */
	  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	  {
	  }
	
	  /* Select PLL as system clock source */
	  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	
	  /* Wait till PLL is used as system clock source */
	  while(RCC_GetSYSCLKSource() != 0x08)
	  {
	  }
	}

}


/**
* @fn		void System_GPIOConfig(void)
* @brief
*	@li		GPIO  �ʱ�ȭ �� ���� 
* @remarks
* @param	void 
* @return	void			
*/

void System_GPIOConfig(void)
{
	/* GPIO �ʱ�ȭ : Ŭ�� Ȱ��ȭ  */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC 
			| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG 
			| RCC_APB2Periph_AFIO, ENABLE);
}





/**
* @fn		void System_SysTickConfig(uint32_t Period, SYSTICK_CALLBACK Callback)
* @brief
*	@li		system tick �� �����Ѵ�. 
* @remarks
* @param	uint32_t Period				: System Tick�� �ֱ� (������ ms) : �ִ� 233 ���� 
* @param	SYSTICK_CALLBACK Callback	: �ݹ��Լ� 
* @return	void			
*/
void System_SysTickConfig(uint32_t Period, SYSTICK_CALLBACK Callback)
{
	/* ms ������ System tick�� �ǵ��� ��ȯ�Ѵ�. (SystemCoreClock �϶� 1�ʰ� �ȴ�.) 	*/
	/* 1ms ���� ƽ�� �߻� �ҷ���  systemCoreClock / 1000 ���� ƽ�� �ʿ��ϴ� �׷��Ƿ� �츮�� ���ϴ� �ֱ� ���� ƽ�� �߻��ҷ��� 
	       Tick =  systemCoreClock  / 1000 * Period �� �ȴ�. */
	Period = SystemCoreClock /1000 * Period;
	if (SysTick_Config(Period))
	{ 
		/* Capture error */ 
		while (1);
	}
	 
	/* Configure the SysTick handler priority */
	NVIC_SetPriority(SysTick_IRQn, 0x0);

	/* �ݹ��Լ��� ����Ѵ�.  */
	SysTick_Callback = Callback;
	
}


/**
* @fn		void System_InterruptConfig(void)
* @brief
*	@li		���ͷ�Ʈ �����Ѵ�. 
* @remarks
* @param	void 
* @return	void			
*/
void System_InterruptConfig(void)
{

	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);
	 
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}



/**
* @fn		void System_I2CConfig(void)
* @brief
*	@li		I2C �����Ѵ�. 
* @remarks
* @param	void 
* @return	void			
*/
void System_I2CConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStructure;

	/* Ŭ�� Ȱ��ȭ */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	/* I2C1 ��Ʈ �ʱ�ȭ */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;						// SDA/SCL : Alternate function Open drain
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	/* I2C ��Ʈ �ʱ�ȭ */	


	/* I2C_InitStructure �ʱ�ȭ */
	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_Init(I2C1, &I2C_InitStructure);

	I2C_Cmd(I2C1,ENABLE);


//	I2C_SendData(I2C1, 0x34);



}





/** 
*@}group System_Exported_Functions
*/


/** @addtogroup System_private_Funtions 
*@{
*/


/**
* @fn		void SysTick_Handler(void)
* @brief
*	@li		SysTick ISR(Interrupt Service Routine)
* @remarks
* @param	void
* @return	void			
*/
void SysTick_Handler(void)
{
	if(SysTick_Callback != (void *)0)
	{
		SysTick_Callback();
	}
}


void HardFault_Handler(void)
{
	static uint16_t ErrorCount = 0;

	ErrorCount++;
}




/**
*@}
*/


/** 
*@}group System
*/

/** 
*@}group CortexM3_Driver
*/

