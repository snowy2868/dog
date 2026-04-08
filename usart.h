#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "workmode.h"
#include <stdio.h>

extern u8 rx_buffer[64];
extern u8 rx_cnt;
extern u8 rx_flag;
extern u8 Bluetooth_Received_Data;

/* USART3 - 蓝牙模块 */
void USART3_Init(u32 bound);
void USART3_SendByte(u8 ch);
void USART3_SendString(u8 *str);

/* USART1 - 语音模块 */
void USART1_Init(u32 bound);
void USART1_SendByte(u8 ch);
void USART1_SendString(u8 *str);

#endif
