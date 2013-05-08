#ifndef _I2C_SW_H_
#define _I2C_SW_H_

#include "stm32f10x.h"

void I2C_SW_Init(void);
uint8_t I2C_SW_Write(uint8_t Addr, uint8_t *Data, uint8_t Length);
uint8_t I2C_SW_Read(uint8_t Addr, uint8_t *Data, uint8_t Length);

#endif
