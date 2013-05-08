/**
* @file		system.h
* @brief		system 헤더 파일  
* @author		강이석/솔루션 공과 전자 학원 	
* @remarks
* @par 수정이력 
*	@li	2013.01.25 : 초기  함수 작성  
*/



#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "stm32f10x.h"


/** @defgroup CortexM3_Driver
  * @brief CortexM3 드라이버  
  * @{
  */

/** @defgroup System
  * @brief System 드라이버 
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

void System_ClockConfig(void);	/*!< 시스템 클럭 설정  */
void System_GPIOConfig(void);	/*!< GPIO 초기화 및 설정 */
void System_SysTickConfig(uint32_t Period, SYSTICK_CALLBACK Callback);	/*!< 시스템 틱을 설정 */
void System_InterruptConfig(void);		/*!< 인터럽트 벡터 및 우선순위 설정  */
void System_I2CConfig(void);		/*!< I2C 설정  */

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


