
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "i2c_sw.h"
#include "touch.h"
#include "serial.h"


#define TSC2003_I2C_ADDR	0x90

#define TOUCH_CMD_MEASURE_TEMP0		0x00
#define TOUCH_CMD_MEASURE_VBAT1		0x10
#define TOUCH_CMD_MEASURE_IN1		0x20
#define TOUCH_CMD_RESERVE1			0x30
#define TOUCH_CMD_MEASURE_TEMP1		0x40
#define TOUCH_CMD_MEASURE_VBAT2		0x50
#define TOUCH_CMD_MEASURE_IN2		0x60
#define TOUCH_CMD_RESERVE2			0x70
#define TOUCH_CMD_ACTIVE_XM			0x80
#define TOUCH_CMD_ACTIVE_YM			0x90
#define TOUCH_CMD_ACTIVE_YP_XM		0xA0
#define TOUCH_CMD_RESERVE3			0xB0
#define TOUCH_CMD_MEASURE_X			0xC0
#define TOUCH_CMD_MEASURE_Y			0xD0
#define TOUCH_CMD_MEASURE_Z1		0xE0
#define TOUCH_CMD_MEASURE_Z2		0xF0

#define TOUCH_PD_PD_CONVERSION		0x00
#define TOUCH_PD_INT_OFF_ADC_ON		0x04
#define TOUCH_PD_INT_ON_ADC_OFF		0x08
#define TOUCH_PD_INT_ON_ADC_ON		0x0C



uint8_t TouchBuffer[2];

TOUCH_HANDLE TouchHandle = (void *)0;


void Touch_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 초기 레지스터 값 설정 */
	/* X Position 측정	 */
	TouchBuffer[0] = TOUCH_CMD_MEASURE_X | TOUCH_PD_INT_ON_ADC_ON;
	I2C_SW_Write(TSC2003_I2C_ADDR, TouchBuffer, 1);
	I2C_SW_Read(TSC2003_I2C_ADDR, TouchBuffer, 2);
	/* Y Position 측정	 */
	TouchBuffer[0] = TOUCH_CMD_MEASURE_Y | TOUCH_PD_PD_CONVERSION;
	I2C_SW_Write(TSC2003_I2C_ADDR, TouchBuffer, 1);
	I2C_SW_Read(TSC2003_I2C_ADDR, TouchBuffer, 2);

	/***** 외부 인터럽트 설정 *****/
	// 외부 인터럽트 라인 설정 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource15);
	/* EXTI_InitStructure  초기화 */
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Line = EXTI_Line15;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	

    /* EXTI line 15  인터럽트 설정 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void Touch_Open(TOUCH_HANDLE Handle)
{
	TouchHandle = Handle;
}


void Touch_Close(void)
{
	TouchHandle = (void *)0;
}







/**
* @fn		void EXTI15_10_IRQHandler(void)
* @brief
*	- 외부 인터럽트 15 ~ 10 ISR(Interrupt Service Routine)  
* @remarks
* @param	void
* @return	void			
*/
void EXTI15_10_IRQHandler(void)
{

	uint16_t X, Y;

	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		/* X Position 측정   */
		TouchBuffer[0] = TOUCH_CMD_MEASURE_X | TOUCH_PD_INT_ON_ADC_ON;
		I2C_SW_Write(TSC2003_I2C_ADDR, TouchBuffer, 1);
		I2C_SW_Read(TSC2003_I2C_ADDR, TouchBuffer, 2);
		X = ((uint16_t)TouchBuffer[0] << 4) |(TouchBuffer[1] >> 4);
		/* Y Position 측정   */
		TouchBuffer[0] = TOUCH_CMD_MEASURE_Y | TOUCH_PD_PD_CONVERSION;
		I2C_SW_Write(TSC2003_I2C_ADDR, TouchBuffer, 1);
		I2C_SW_Read(TSC2003_I2C_ADDR, TouchBuffer, 2);
		Y = ((uint16_t)TouchBuffer[0] << 4) |(TouchBuffer[1] >> 4);

		if(TouchHandle != (void *)0)
		{
			TouchHandle(X,Y);
		}

	  	/* EXTI Line 15 Pending 비트 초기화 */
	  	EXTI_ClearITPendingBit(EXTI_Line15);		
	}

}

