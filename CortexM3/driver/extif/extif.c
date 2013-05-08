#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_fsmc.h"
#include "extif.h"


static void ExtIF_GPIOConfig(void);
static void ExtIF_FSMCConfig(void);



void ExtIF_Init(void)
{
	ExtIF_GPIOConfig();
	ExtIF_FSMCConfig();
}

static void ExtIF_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) as alternate 
     function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Set PF.00(A0),PF.01(A1),PF.02(A2),PF.03(A3),PF.04(A4),PF.05(A5) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* Set PG.12(NE4 (LCD/CS)) as alternate function push pull - CE3(LCD /CS) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
}




static void ExtIF_FSMCConfig(void)
{
	  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	  FSMC_NORSRAMTimingInitTypeDef  p;

	/* FSMC 클럭 활성화 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

	
	/*-- FSMC Configuration ------------------------------------------------------*/
	/*----------------------- SRAM Bank 4 ----------------------------------------*/
	  /* FSMC_Bank1_NORSRAM4 configuration */
	  p.FSMC_AddressSetupTime = 1;
	  p.FSMC_AddressHoldTime = 0;
	  p.FSMC_DataSetupTime = 2;
	  p.FSMC_BusTurnAroundDuration = 0;
	  p.FSMC_CLKDivision = 0;
	  p.FSMC_DataLatency = 0;
	  p.FSMC_AccessMode = FSMC_AccessMode_A;
	
	  /* Color LCD configuration ------------------------------------
		 LCD configured as follow:
			- Data/Address MUX = Disable
			- Memory Type = SRAM
			- Data Width = 16bit
			- Write Operation = Enable
			- Extended Mode = Enable
			- Asynchronous Wait = Disable */
	  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
	
	  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  
	
	  /* BANK 4 (of NOR/SRAM Bank 1~4) is enabled */
	  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);

}

