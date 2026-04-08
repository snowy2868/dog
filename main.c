#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "oled.h"
#include "dht11.h"
#include "pwm.h"
#include "adc.h"
#include "servo.h"
#include "petaction.h"
#include "variable.h"
#include "workmode.h"
#include <string.h>
#include <stdlib.h>

/*==============================
  主程序全局变量
==============================*/
float temperature = 0;  /* 温度值(℃) */
float humidity = 0;     /* 湿度值(%) */
u16 light_ao = 0;      /* 光敏传感器AO模拟值(0-4095) */
u8 light_do = 0;       /* 光敏传感器DO数字值(0或1) */
u8 breath_brightness1 = 0;  /* 呼吸灯1亮度(0-100) */
u8 breath_brightness2 = 0;  /* 呼吸灯2亮度(0-100) */
u8 breath_dir1 = 1;         /* 呼吸灯1方向：1=变亮，0=变暗 */
u8 breath_dir2 = 1;         /* 呼吸灯2方向：1=变亮，0=变暗 */

/*
 * 功能：系统初始化函数
 * 说明：依次初始化所有硬件模块
 *   - 延时函数初始化
 *   - USART1初始化(语音模块，9600波特率，PA9/PA10)
 *   - USART3初始化(蓝牙模块，9600波特率，PB10/PB11)
 *   - OLED显示屏初始化
 *   - DHT11温湿度传感器初始化
 *   - 舵机PWM初始化
 *   - ADC1初始化(电池电压、光敏传感器)
 */
void System_Init(void)
{
    delay_init();          /* 延时初始化 */
    USART1_Init(9600);     /* 串口1初始化(语音模块) */
    USART3_Init(9600);     /* 串口3初始化(蓝牙模块) */
    OLED_Init();           /* OLED初始化 */
    DHT11_Init();          /* DHT11温湿度初始化 */
    Servo_Init();          /* 舵机初始化 */
    ADC1_Init();           /* ADC初始化 */
    WorkMode_Init();       /* 工作模式初始化 */
}

/*
 * 功能：OLED显示更新函数
 * 说明：
 *   - 支持三种显示模式：表情模式(0)、数据模式(1)、天气模式(Weather_Mode=1)
 *   - 只要显示模式、表情、电量或天气数据有变化就立即刷新屏幕
 *   - 表情模式：显示128x64表情图片 + 左上角电量
 *   - 数据模式：显示所有传感器数据(电量、温度、湿度、光照、电压)
 *   - 天气模式：显示天气信息(室外温湿度、是否下雨、带伞、防晒提示)
 *   - 表情对应表：
 *     0=睡觉(Face_sleep)
 *     1=瞪大眼(Face_stare)
 *     2=快乐(Face_happy)
 *     3=狂热(Face_mania)
 *     4=非常快乐(Face_very_happy)
 *     5=眼睛(Face_eyes)
 *     6=打招呼(Face_hello)
 */
void Update_OLED(void)
{
    float bat_volt;              /* 电池电压变量 */
    u8 bat_percent;              /* 电池电量百分比 */
    static u8 last_oled_mode = 0xFF;   /* 上次OLED显示模式，用于判断是否变化 */
    static u16 last_face_mode = 0xFFFF; /* 上次表情模式，用于判断是否变化 */
    static u8 last_bat_percent = 0xFF;  /* 上次电量百分比，用于判断是否变化 */
    static u8 last_weather_mode = 0xFF; /* 上次天气显示模式 */
    static int8_t last_weather_temp = 127; /* 上次天气温度 */
    static u8 last_weather_humi = 0xFF;   /* 上次天气湿度 */
    static u8 last_weather_rain = 0xFF;   /* 上次下雨状态 */
    static u8 last_weather_umbrella = 0xFF; /* 上次带伞状态 */
    static u8 last_weather_sunscreen = 0xFF; /* 上次防晒状态 */
    
    /* 获取电池电压和电量 */
    bat_volt = Get_Battery_Voltage();
    bat_percent = Get_Battery_Percent();
    
    /* ========== 天气模式10秒自动切换检测 ========== */
    if(Weather_Mode == 1)
    {
        u32 current_time = millis();
        /* 如果超过10秒（10000毫秒），自动退出天气模式 */
        if(current_time - Weather_Start_Time > 10000)
        {
            Weather_Mode = 0;
        }
    }
    
    /* 判断是否需要刷新屏幕 */
    u8 need_refresh = 0;
    if(OLED_Display_Mode != last_oled_mode || 
       Face_Mode != last_face_mode || 
       bat_percent != last_bat_percent ||
       Weather_Mode != last_weather_mode ||
       Weather_Temp != last_weather_temp ||
       Weather_Humi != last_weather_humi ||
       Weather_Rain != last_weather_rain ||
       Weather_Umbrella != last_weather_umbrella ||
       Weather_Sunscreen != last_weather_sunscreen)
    {
        need_refresh = 1;
    }
    
    if(need_refresh)
    {
        OLED_Clear();  /* 清屏 */
        /* 更新记录的状态 */
        last_oled_mode = OLED_Display_Mode;
        last_face_mode = Face_Mode;
        last_bat_percent = bat_percent;
        last_weather_mode = Weather_Mode;
        last_weather_temp = Weather_Temp;
        last_weather_humi = Weather_Humi;
        last_weather_rain = Weather_Rain;
        last_weather_umbrella = Weather_Umbrella;
        last_weather_sunscreen = Weather_Sunscreen;
        
        /* ========== 天气模式优先显示 ========== */
        if(Weather_Mode == 1)
        {
            /* 第0行：标题和电量 */
            OLED_ShowString(0, 0, "Weather:", OLED_8X16);
            OLED_ShowString(80, 0, "Bat:", OLED_6X8);
            OLED_ShowNum(104, 0, bat_percent, 3, OLED_6X8);
            OLED_ShowString(120, 0, "%", OLED_6X8);
            
            /* 第1行：室外温度和湿度 */
            OLED_ShowString(0, 16, "Out:", OLED_8X16);
            if(Weather_Temp >= 0)
            {
                OLED_ShowNum(32, 16, (u8)Weather_Temp, 2, OLED_8X16);
            }
            else
            {
                OLED_ShowString(32, 16, "-", OLED_8X16);
                OLED_ShowNum(40, 16, (u8)(-Weather_Temp), 2, OLED_8X16);
            }
            OLED_ShowString(56, 16, "C", OLED_8X16);
            
            OLED_ShowString(72, 16, "H:", OLED_8X16);
            OLED_ShowNum(88, 16, Weather_Humi, 2, OLED_8X16);
            OLED_ShowString(104, 16, "%", OLED_8X16);
            
            /* 第2行：是否下雨 */
            OLED_ShowString(0, 32, "Rain:", OLED_8X16);
            if(Weather_Rain == 1)
                OLED_ShowString(48, 32, "YES!", OLED_8X16);
            else
                OLED_ShowString(48, 32, "NO", OLED_8X16);
            
            /* 第3行：提示信息 */
            if(Weather_Umbrella == 1 && Weather_Sunscreen == 1)
            {
                OLED_ShowString(0, 48, "Umbrella+Sunscreen", OLED_6X8);
            }
            else if(Weather_Umbrella == 1)
            {
                OLED_ShowString(0, 48, "Take Umbrella!", OLED_8X16);
            }
            else if(Weather_Sunscreen == 1)
            {
                OLED_ShowString(0, 48, "Use Sunscreen!", OLED_8X16);
            }
            else
            {
                OLED_ShowString(0, 48, "Good Weather!", OLED_8X16);
            }
        }
        /* ========== 表情模式 ========== */
        else if(OLED_Display_Mode == 0)
        {
            /* 根据Face_Mode显示对应的表情图片 */
            switch(Face_Mode)
            {
                case 0:
                    OLED_ShowImage(0, 0, 128, 64, Face_sleep);      /* 睡觉表情 */
                    break;
                case 1:
                    OLED_ShowImage(0, 0, 128, 64, Face_stare);      /* 瞪大眼表情 */
                    break;
                case 2:
                    OLED_ShowImage(0, 0, 128, 64, Face_happy);      /* 快乐表情 */
                    break;
                case 3:
                    OLED_ShowImage(0, 0, 128, 64, Face_mania);      /* 狂热表情 */
                    break;
                case 4:
                    OLED_ShowImage(0, 0, 128, 64, Face_very_happy); /* 非常快乐表情 */
                    break;
                case 5:
                    OLED_ShowImage(0, 0, 128, 64, Face_eyes);       /* 眼睛表情 */
                    break;
                case 6:
                    OLED_ShowImage(0, 0, 128, 64, Face_hello);      /* 打招呼表情 */
                    break;
                default:
                    OLED_ShowImage(0, 0, 128, 64, Face_happy);      /* 默认快乐表情 */
                    break;
            }
            
            /* 在左上角显示电量(6x8字体) */
            OLED_ShowString(0, 0, "Bat:", OLED_6X8);
            OLED_ShowNum(24, 0, bat_percent, 3, OLED_6X8);
            OLED_ShowString(48, 0, "%", OLED_6X8);
        }
        /* ========== 数据模式 ========== */
        else
        {
            /* 第0行：显示电量百分比 */
            OLED_ShowString(0, 0, "Battery:", OLED_8X16);
            OLED_ShowNum(56, 0, bat_percent, 3, OLED_8X16);
            OLED_ShowString(80, 0, "%", OLED_8X16);
            
            /* 第1行：显示温度和湿度 */
            OLED_ShowString(0, 16, "Temp:", OLED_8X16);
            OLED_ShowNum(40, 16, (u16)temperature, 2, OLED_8X16);
            OLED_ShowString(56, 16, "C", OLED_8X16);
            
            OLED_ShowString(64, 16, "Humi:", OLED_8X16);
            OLED_ShowNum(104, 16, (u16)humidity, 2, OLED_8X16);
            OLED_ShowString(120, 16, "%", OLED_8X16);
            
            /* 第2行：显示光照AO值和DO值 */
            OLED_ShowString(0, 32, "Light:", OLED_8X16);
            OLED_ShowNum(48, 32, light_ao, 4, OLED_8X16);
            
            OLED_ShowString(88, 32, "DO:", OLED_8X16);
            OLED_ShowNum(112, 32, light_do, 1, OLED_8X16);
            
            /* 第3行：显示电池电压 */
            OLED_ShowString(0, 48, "Volt:", OLED_8X16);
            OLED_ShowNum(40, 48, (u16)(bat_volt*10), 2, OLED_8X16);      /* 整数部分 */
            OLED_ShowString(56, 48, ".", OLED_8X16);
            OLED_ShowNum(64, 48, (u16)(bat_volt*100)%10, 1, OLED_8X16); /* 小数部分 */
            OLED_ShowString(72, 48, "V", OLED_8X16);
        }
        
        /* 更新OLED显示(将显存内容刷新到屏幕) */
        OLED_Update();
    }
}

/*
 * 功能：呼吸灯更新函数
 * 说明：
 *   - 控制两个呼吸灯(PB0和PB1)实现呼吸效果
 *   - 使用PWM控制亮度，范围0-100
 *   - 每2个循环周期(20ms)更新一次亮度
 *   - 亮度变化步长为2，变化更平滑
 *   - 亮度到100后开始变暗，到0后开始变亮，循环往复
 */
void Breath_Led_Update(void)
{
    static u8 breath_tick = 0;  /* 呼吸灯计时变量 */
    
    breath_tick++;
    if(breath_tick < 2)  /* 每2个周期(20ms)更新一次 */
    {
        return;
    }
    breath_tick = 0;  /* 重置计时器 */
    
    /* ========== 呼吸灯1(PB0) ========== */
    if(breath_dir1)  /* 方向：变亮 */
    {
        breath_brightness1 += 2;  /* 亮度加2 */
        if(breath_brightness1 >= 100) breath_dir1 = 0;  /* 到100后切换为变暗 */
    }
    else  /* 方向：变暗 */
    {
        breath_brightness1 -= 2;  /* 亮度减2 */
        if(breath_brightness1 == 0) breath_dir1 = 1;  /* 到0后切换为变亮 */
    }
    Set_Breath_LED1_Brightness(breath_brightness1);  /* 设置呼吸灯1PWM */
    
    /* ========== 呼吸灯2(PB1) ========== */
    if(breath_dir2)  /* 方向：变亮 */
    {
        breath_brightness2 += 2;  /* 亮度加2 */
        if(breath_brightness2 >= 100) breath_dir2 = 0;  /* 到100后切换为变暗 */
    }
    else  /* 方向：变暗 */
    {
        breath_brightness2 -= 2;  /* 亮度减2 */
        if(breath_brightness2 == 0) breath_dir2 = 1;  /* 到0后切换为变亮 */
    }
    Set_Breath_LED2_Brightness(breath_brightness2);  /* 设置呼吸灯2PWM */
}

/*
 * 功能：主函数
 * 说明：
 *   - 系统初始化
 *   - 显示"System Init..."提示
 *   - 所有舵机复位到90度(中位)
 *   - 主循环：
 *     1. 读取DHT11温湿度数据
 *     2. 读取光敏传感器AO和DO值
 *     3. 更新OLED显示
 *     4. 更新呼吸灯
 *     5. 执行机械狗动作
 *     6. 执行摇尾巴动作
 *     7. 延时10ms
 */
int main(void)
{
    /* ========== 系统初始化 ========== */
    System_Init();
    
    /* 显示初始化提示 */
    OLED_Clear();
    OLED_ShowString(0, 0, "System Init...", OLED_8X16);
    OLED_Update();
    delay_ms(1000);  /* 延时1秒 */
    
    /* ========== 舵机复位 ========== */
    /* 所有舵机回到90度中位 */
    Servo_Angle1(90);  /* 左前舵机 */
    Servo_Angle2(90);  /* 右前舵机 */
    Servo_Angle3(90);  /* 左后舵机 */
    Servo_Angle4(90);  /* 右后舵机 */
    WServo_Angle(90);  /* 尾巴舵机 */
    
    /* ========== 主循环（分时复用优化版） ========== */
    static uint16_t loop_count = 0;  /* 主循环计数器 */
    
    while(1)
    {
        loop_count++;
        
        /* ========== 1. 关键功能：每次循环都执行（保证流畅） ========== */
        inc_loop_count();
        
        /* 摇尾巴自动停止检查 */
        if (WeiBa_Bit)
        {
            WeiBa_Count++;
            if (WeiBa_Count >= WEIBA_STOP_COUNT)
            {
                WeiBa_Bit = 0;
                WeiBa_Count = 0;
            }
        }
        
        /* 执行摇尾巴（每次都执行，保证流畅！） */
        Action_SwingTail_NonBlocking();
        
        /* 工作模式检查（每次都执行） */
        WorkMode_AutoCheck();
        
        /* ========== 2. 根据工作模式执行 ========== */
        if (Work_Mode == MODE_POWER_SAVE)
        {
            /* ========== 省电模式 ========== */
            static uint32_t last_check_time = 0;
            uint32_t current_time = millis();
            
            /* 每500ms检查一次是否需要显示 */
            if (current_time - last_check_time > 500)
            {
                last_check_time = current_time;
                OLED_Clear();
                OLED_ShowString(0, 0, "Power Save", OLED_8X16);
                OLED_ShowString(0, 20, "Send cmd", OLED_8X16);
                OLED_ShowString(0, 40, "to wake up", OLED_8X16);
                OLED_Update();
            }
            
            PetAction_Perform();
            delay_ms(50);  /* 省电模式用稍长的延时 */
        }
        else
        {
            /* ========== 活跃模式 或 安静模式 ========== */
            
            /* ========== 3. 耗时功能：分批次执行 ========== */
            
            /* 每2次循环更新一次OLED */
            if (loop_count % 2 == 0)
            {
                Update_OLED();
            }
            
            /* 每10次循环才读一次DHT11（DHT11很慢） */
            if (loop_count % 10 == 0)
            {
                DHT11_Read_Data(&temperature, &humidity);
            }
            
            /* 每4次循环读一次传感器 */
            if (loop_count % 4 == 0)
            {
                light_ao = Get_ADC_Value(ADC_Channel_5);
                light_do = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
            }
            
            /* 每3次循环才更新呼吸灯 */
            if (loop_count % 3 == 0 && Work_Mode == MODE_ACTIVE)
            {
                Breath_Led_Update();
            }
            
            /* 活跃模式更新（每次都执行） */
            WorkMode_ActiveUpdate();
            
            /* 其他动作（每次都执行） */
            PetAction_Perform();
            
            /* 更短的延时，保证摇尾巴流畅！ */
            delay_ms(2);
        }
    }
}
