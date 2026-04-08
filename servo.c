#include "stm32f10x.h"
#include "pwm.h"

void Servo_Init()
{
    PWM_TIM2_Init();
    PWM_TIM3_Init();
}

void Servo_Angle1(float Angle)
{
    Set_SG90_Angle_LeftFront((u8)Angle);
}

void Servo_Angle2(float Angle)
{
    Set_SG90_Angle_RightFront((u8)Angle);
}

void Servo_Angle3(float Angle)
{
    Set_SG90_Angle_LeftBack((u8)Angle);
}

void Servo_Angle4(float Angle)
{
    Set_SG90_Angle_RightBack((u8)Angle);
}

void WServo_Angle(float Angle)
{
    Set_SG90_Angle_Tail((u8)Angle);
}
