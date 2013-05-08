/**
* @file		system.h
* @brief		system ��� ����  
* @author		���̼�/�ַ�� ���� ���� �п� 	
* @remarks
* @par �����̷� 
*	@li	2013.01.25 : �ʱ�  �Լ� �ۼ�  
*/



#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "stm32f10x.h"


/** @defgroup CortexM3_Driver
  * @brief CortexM3 ����̹�  
  * @{
  */

/** @defgroup System
  * @brief System ����̹� 
  * @{
  */ 

/** @defgroup System_Exported_Types
  * @{
  */

/** 
  * @brief system tick callback function definition  
  */
typedef void (*SYSTICK_CALLBACK)(void);

/**
  * @}
  */


/** @defgroup System_Exported_Functions
  * @{
  */

void System_ClockConfig(void);	/*!< �ý��� Ŭ�� ����  */
void System_GPIOConfig(void);	/*!< GPIO �ʱ�ȭ �� ���� */
void System_SysTickConfig(uint32_t Period, SYSTICK_CALLBACK Callback);	/*!< �ý��� ƽ�� ���� */
void System_InterruptConfig(void);		/*!< ���ͷ�Ʈ ���� �� �켱���� ����  */
void System_I2CConfig(void);		/*!< I2C ����  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */



#endif


