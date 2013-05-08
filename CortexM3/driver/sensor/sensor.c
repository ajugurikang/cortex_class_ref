#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#include "sensor.h"


static void Sensor_DMA_Configuration(void);

uint16_t Sensor_Buffer[SENSOR_MAX] = {0, };

void Sensor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	ADC_InitTypeDef ADC_InitStructure;


	/* GPIO 포트 설정
	VOLUME_ADC : PF7, BRIGHT_ADC : PF8 , TEMP_ADC :PF9
	를 아날로그 입력으로 설정 
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	// 아날로그 모드 
	GPIO_Init(GPIOF, &GPIO_InitStructure);	

	/* ADC3  클럭 활성화  */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	/* ADC3 설정 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//  소프트웨어 트리거 사용 
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 3;
	ADC_Init(ADC3, &ADC_InitStructure);

	/* ADC3 regular channel 설정  */ 
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_6, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_7, 3, ADC_SampleTime_55Cycles5);
	/* DMA 설정 */
	Sensor_DMA_Configuration();
	/* ADC3 활성화 */
	ADC_Cmd(ADC3, ENABLE);

	/* ADC3  Calibration 한다. */
	/* ADC3 reset calibaration register 초기화 */	
	ADC_ResetCalibration(ADC3);
	/* ADC3 reset calibration register의 초기화가 되었는지 검사  */
	while(ADC_GetResetCalibrationStatus(ADC3));
	/* ADC3 calibaration 시작  */
	ADC_StartCalibration(ADC3);
	/* ADC3 calibration 이 끝났는지 검사  */
	while(ADC_GetCalibrationStatus(ADC3));

}

void Sensor_ScanStart(void)
{
	ADC_SoftwareStartConvCmd(ADC3,ENABLE);
}


uint16_t Sensor_GetData(SENSOR_TYPE Sensor)
{
	uint16_t RetValue;
	
	switch(Sensor)
	{
		case SENSOR_VOLUME :
			RetValue = Sensor_Buffer[SENSOR_VOLUME];
			break;
		case SENSOR_BRIGHT:
			RetValue = Sensor_Buffer[SENSOR_BRIGHT];
			break;			
		case SENSOR_TEMP:
			RetValue = 22 * (uint32_t)Sensor_Buffer[SENSOR_TEMP] / 273 - 50;
			break;
	}
	return RetValue;
}


static void Sensor_DMA_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;

	/* DMA2 클럭 활성화 : ADC3는 DMA2 Channel 5 사용  */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    /* DMA2  설정 */
    DMA_DeInit(DMA2_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC3->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Sensor_Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 3;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel5, &DMA_InitStructure);

    /* DMA2 Channel5 활성화 */
    DMA_Cmd(DMA2_Channel5, ENABLE);
	/* ADC3 DMA 활성화 */
	ADC_DMACmd(ADC3,ENABLE);
}


