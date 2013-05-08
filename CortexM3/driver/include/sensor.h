#ifndef _SENSOR_H_
#define _SENSOR_H_

typedef enum{
	SENSOR_VOLUME,
	SENSOR_BRIGHT,		
	SENSOR_TEMP,
	SENSOR_MAX
}SENSOR_TYPE;


void Sensor_Init(void);
void Sensor_ScanStart(void);
uint16_t Sensor_GetData(SENSOR_TYPE Sensor);


#endif

