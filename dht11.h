#ifndef __DHT11_H
#define __DHT11_H
#include "stm32f10x.h"

#define DHT11_PIN GPIO_Pin_7
#define DHT11_PORT GPIOB

void DHT11_Init(void);
u8 DHT11_Read_Data(float *temperature, float *humidity);

#endif
