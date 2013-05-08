/**
* @file			system.c
* @brief		system 드라이버 
* @author		강이석	
* @remarks
* @par 수정이력 
*	@li	2012.12.07 : 초기  함수 작성  
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

SYSTICK_CALLBACK SysTick_Callback = (void *)0;	/*!< SysTick 콜백 함수저장 변수  */


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
*	@li		시스템 클럭을 설정한다. 
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
	  
	  /* AHB 클럭설정 :  HCLK = SYSCLK  = 72MHz */
	  RCC_HCLKConfig(RCC_SYSCLK_Div1); 
	
	  /* APB2( high-speed ) 클럭 설정 PCLK2 = HCLK = 72MHz */
	  RCC_PCLK2Config(RCC_HCLK_Div1); 
	
	  /* APB1 (low-speed) 클럭 설정  PCLK1 = HCLK/2 = 36MHz */
	  RCC_PCLK1Config(RCC_HCLK_Div2);
	
	  /* PLL 설정 : PLLCLK = 8MHz * 9 = 72 MHz */
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
*	@li		GPIO  초기화 및 설정 
* @remarks
* @param	void 
* @return	void			
*/

void System_GPIOConfig(void)
{
	/* GPIO 초기화 : 클럭 활성화  */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC 
			| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG 
			| RCC_APB2Periph_AFIO, ENABLE);
}





/**
* @fn		void System_SysTickConfig(uint32_t Period, SYSTICK_CALLBACK Callback)
* @brief
*	@li		system tick 를 설정한다. 
* @remarks
* @param	uint32_t Period				: System Tick의 주기 (단위는 ms) : 최대 233 까지 
* @param	SYSTICK_CALLBACK Callback	: 콜백함수 
* @return	void			
*/
void System_SysTickConfig(uint32_t Period, SYSTICK_CALLBACK Callback)
{
	/* ms 단위로 System tick이 되도록 변환한다. (SystemCoreClock 일때 1초가 된다.) 	*/
	/* 1ms 마다 틱이 발생 할려며  systemCoreClock / 1000 개의 틱이 필요하다 그러므로 우리가 원하는 주기 마다 틱이 발생할려면 
	       Tick =  systemCoreClock  / 1000 * Period 가 된다. */
	Period = SystemCoreClock /1000 * Period;
	if (SysTick_Config(Period))
	{ 
		/* Capture error */ 
		while (1);
	}
	 
	/* Configure the SysTick handler priority */
	NVIC_SetPriority(SysTick_IRQn, 0x0);

	/* 콜백함수를 등록한다.  */
	SysTick_Callback = Callback;
	
}


/**
* @fn		void System_InterruptConfig(void)
* @brief
*	@li		인터럽트 설정한다. 
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
*	@li		I2C 설정한다. 
* @remarks
* @param	void 
* @return	void			
*/
void System_I2CConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStructure;

	/* 클럭 활성화 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	/* I2C1 포트 초기화 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;						// SDA/SCL : Alternate function Open drain
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	/* I2C 포트 초기화 */	


	/* I2C_InitStructure 초기화 */
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

