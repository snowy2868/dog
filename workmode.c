#include "stm32f10x.h"
#include "workmode.h"
#include "variable.h"
#include "delay.h"
#include "oled.h"
#include "pwm.h"
#include "adc.h"
#include "petaction.h"
#include "servo.h"
#include <stdlib.h>

/*==============================
  本地辅助函数声明
==============================*/
static void Enter_PowerSaveMode(void);
static void Exit_PowerSaveMode(void);
static void Auto_Change_Face(void);
static void Auto_Do_Action(void);

/*==============================
  初始化工作模式
==============================*/
void WorkMode_Init(void)
{
    Work_Mode = MODE_ACTIVE;
    Last_Command_Time = millis();
    Auto_Action_Timer = 0;
    Auto_Face_Action = 1;
}

/*==============================
  获取工作模式名称
==============================*/
char* WorkMode_GetName(uint8_t mode)
{
    switch(mode)
    {
        case MODE_ACTIVE:    return "Active";
        case MODE_QUIET:     return "Quiet";
        case MODE_POWER_SAVE:return "PowerSave";
        default:             return "Unknown";
    }
}

/*==============================
  切换工作模式
==============================*/
void WorkMode_Switch(uint8_t new_mode)
{
    if (new_mode == Work_Mode) return;
    
    uint8_t old_mode = Work_Mode;
    
    /* 退出旧模式 */
    if (old_mode == MODE_POWER_SAVE)
    {
        Exit_PowerSaveMode();
    }
    
    /* 进入新模式 */
    Work_Mode = new_mode;
    
    switch(new_mode)
    {
        case MODE_ACTIVE:
            OLED_Clear();
            OLED_ShowString(0, 0, "Active Mode", OLED_8X16);
            OLED_Update();
            delay_ms(1000);
            AllLed = 1;
            BreatheLed = 1;
            Last_Command_Time = millis();
            break;
            
        case MODE_QUIET:
            OLED_Clear();
            OLED_ShowString(0, 0, "Quiet Mode", OLED_8X16);
            OLED_Update();
            delay_ms(1000);
            AllLed = 1;
            BreatheLed = 0;
            break;
            
        case MODE_POWER_SAVE:
            OLED_Clear();
            OLED_ShowString(0, 0, "Power Save", OLED_8X16);
            OLED_Update();
            delay_ms(1000);
            Enter_PowerSaveMode();
            break;
    }
}

/*==============================
  进入省电模式
==============================*/
static void Enter_PowerSaveMode(void)
{
    /* 关闭所有LED */
    AllLed = 0;
    BreatheLed = 0;
    Set_Breath_LED1_Brightness(0);
    Set_Breath_LED2_Brightness(0);
    
    /* 清空OLED */
    OLED_Clear();
    OLED_Update();
    
    /* 停止所有动作 */
    Action_Mode = 2;
    WeiBa_Bit = 0;
    Servo_Angle1(90);
    Servo_Angle2(90);
    Servo_Angle3(90);
    Servo_Angle4(90);
    WServo_Angle(90);
    
    /* 注意：不使用真正的STOP模式，因为串口中断无法唤醒
       改用软件模拟省电模式，保持CPU运行但降低功耗
    */
}

/*==============================
  退出省电模式
==============================*/
static void Exit_PowerSaveMode(void)
{
    /* 重新初始化各模块 */
    OLED_Init();
    ADC1_Init();
    
    /* 恢复LED */
    AllLed = 1;
    BreatheLed = 1;
    
    /* 记录唤醒时间 */
    Last_Command_Time = millis();
}

/*==============================
  工作模式自动检查（光线、电量）
==============================*/
void WorkMode_AutoCheck(void)
{
    static uint8_t last_light_check = 0;
    static uint8_t last_battery_check = 0;
    
    /* 获取当前状态 */
    uint16_t light_value = Get_ADC_Value(ADC_Channel_5);  /* 光敏传感器值 */
    uint8_t bat_percent = Get_Battery_Percent();
    
    /* 如果当前已经在省电模式，就不再做任何自动切换 */
    if (Work_Mode == MODE_POWER_SAVE)
    {
        return;  /* 省电模式下，不做任何自动切换 */
    }
    
    /* 电池电量检查 - 优先级最高（只有在非省电模式下才检查） */
    if (bat_percent < BATTERY_THRESHOLD_POWER_SAVE)
    {
        if (!last_battery_check)
        {
            last_battery_check = 1;
            WorkMode_Switch(MODE_POWER_SAVE);
        }
        return;
    }
    else
    {
        last_battery_check = 0;
    }
    
    /* 光敏传感器检查 - 只在活跃/安静模式下检查 */
    if (light_value < LIGHT_THRESHOLD_QUIET)
    {
        if (Work_Mode == MODE_ACTIVE)
        {
            if (!last_light_check)
            {
                last_light_check = 1;
                WorkMode_Switch(MODE_QUIET);
            }
        }
    }
    else
    {
        if (Work_Mode == MODE_QUIET)
        {
            if (last_light_check)
            {
                last_light_check = 0;
                WorkMode_Switch(MODE_ACTIVE);
            }
        }
        last_light_check = 0;
    }
}

/*==============================
  活跃模式：5分钟无指令自动动作
==============================*/
void WorkMode_ActiveUpdate(void)
{
    if (Work_Mode != MODE_ACTIVE) return;
    if (!Auto_Face_Action) return;
    
    uint32_t current_time = millis();
    uint32_t elapsed = current_time - Last_Command_Time;
    
    /* 调整后的时间：因为主循环实际约20ms，所以设为30000（约1分钟真实时间）
       如果你想更快，改小一点，比如15000（约30秒）
       或者5000（约10秒）
    */
    #define AUTO_ACTION_INTERVAL  30000
    
    if (elapsed >= AUTO_ACTION_INTERVAL)
    {
        /* 随机选择表情或动作 */
        if (rand() % 2 == 0)
        {
            Auto_Change_Face();
        }
        else
        {
            Auto_Do_Action();
        }
        
        /* 重置计时器 */
        Last_Command_Time = current_time;
    }
}

/*==============================
  自动变换表情
==============================*/
static void Auto_Change_Face(void)
{
    uint8_t old_face = Face_Mode;
    uint8_t new_face;
    
    /* 随机选择一个不同的表情 */
    do
    {
        new_face = rand() % 7;  /* 0-6共7种表情 */
    } while (new_face == old_face);
    
    Face_Mode = new_face;
}

/*==============================
  自动做动作
==============================*/
static void Auto_Do_Action(void)
{
    uint8_t action = rand() % 5;  /* 0-4共5种动作 */
    
    switch(action)
    {
        case 0:
            /* 摇尾巴 */
            WeiBa_Bit = 1;
            WeiBa_Start_Time = millis();  /* 记录摇尾巴开始时间（保留） */
            WeiBa_Count = 0;  /* 重置计数器（新增） */
            break;
        case 1:
            /* 摇摆动作 */
            Action_Mode = 8;
            PAnumbers = SwingRepeatnumber;
            break;
        case 2:
            /* 打招呼 */
            Action_Mode = 13;
            PAnumbers = HelloRepeatnumber;
            break;
        case 3:
            /* 伸懒腰 */
            Action_Mode = 14;
            PAnumbers = 1;
            break;
        case 4:
            /* 拉伸腿 */
            Action_Mode = 15;
            PAnumbers = 1;
            break;
    }
}

/*==============================
  记录收到指令时间
==============================*/
void WorkMode_RecordCommand(void)
{
    Last_Command_Time = millis();
    
    /* 注意：省电模式下不要自动唤醒！
       让 usart.c 中的模式切换指令（0x60/0x61）来决定是否唤醒
    */
}

/*==============================
  省电模式唤醒（保留函数供将来使用）
==============================*/
void WorkMode_WakeUp(void)
{
    if (Work_Mode != MODE_POWER_SAVE) return;
    
    /* 切换到活跃模式 */
    WorkMode_Switch(MODE_ACTIVE);
}
