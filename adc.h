#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x.h"

/*
 * 功能：ADC1初始化
 * 说明：配置PA4(电池电压检测)、PA5(光敏传感器AO)为模拟输入模式
 *       PA7(光敏传感器DO)为上拉输入模式
 */
void ADC1_Init(void);

/*
 * 功能：获取指定ADC通道的采样值
 * 参数：ch - ADC通道号(ADC_Channel_x)
 * 返回：12位ADC采样值(0-4095)
 */
u16 Get_ADC_Value(u8 ch);

/*
 * 功能：获取电池电压值
 * 返回：电池电压(单位：V)，已考虑1/4分压电路
 * 说明：连续采样10次取平均值，提高精度
 */
float Get_Battery_Voltage(void);

/*
 * 功能：获取电池电量百分比
 * 返回：电量百分比(0-100)
 * 说明：3.0V对应0%，4.2V对应100%，线性映射
 */
u8 Get_Battery_Percent(void);

#endif
