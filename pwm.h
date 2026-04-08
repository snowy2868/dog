#ifndef __PWM_H
#define __PWM_H
#include "stm32f10x.h"

void PWM_TIM2_Init(void);
void PWM_TIM3_Init(void);
void Set_SG90_Angle_LeftFront(u8 angle);
void Set_SG90_Angle_RightFront(u8 angle);
void Set_SG90_Angle_LeftBack(u8 angle);
void Set_SG90_Angle_RightBack(u8 angle);
void Set_SG90_Angle_Tail(u8 angle);
void Set_Breath_LED1_Brightness(u8 brightness);
void Set_Breath_LED2_Brightness(u8 brightness);

#endif
