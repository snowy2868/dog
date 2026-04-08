
/*
 * 主循环优化示例
 * 核心思路：分时复用，让摇尾巴每次都能执行，其他功能轮询
 */

#include "stm32f10x.h"
#include "delay.h"
#include "workmode.h"
#include "variable.h"
// ... 其他头文件

/* 主循环状态机 */
typedef enum
{
    STATE_CHECK_WORKMODE = 0,  // 检查工作模式
    STATE_UPDATE_OLED,          // 更新OLED
    STATE_READ_DHT11,           // 读取DHT11
    STATE_READ_SENSOR,          // 读取传感器
    STATE_BREATH_LED,           // 呼吸灯
    STATE_OTHER,                // 其他功能
    STATE_MAX                   // 状态数
} MainState_t;

void Main_Loop_Optimized(void)
{
    static MainState_t state = STATE_CHECK_WORKMODE;
    static uint8_t dht11_read_count = 0;  // DHT11不用每次都读
    
    while(1)
    {
        /* ========== 1. 摇尾巴：每次循环都执行（最重要！） ========== */
        inc_loop_count();  // 更新循环计数
        
        /* 摇尾巴自动停止检查 */
        if (WeiBa_Bit)
        {
            WeiBa_Count++;
            if (WeiBa_Count &gt;= WEIBA_STOP_COUNT)
            {
                WeiBa_Bit = 0;
                WeiBa_Count = 0;
            }
        }
        
        /* 执行摇尾巴（每次都执行，保证流畅） */
        Action_SwingTail_NonBlocking();
        
        /* ========== 2. 其他功能：轮询执行（分时复用） ========== */
        switch(state)
        {
            case STATE_CHECK_WORKMODE:
                /* 检查工作模式 */
                WorkMode_AutoCheck();
                
                /* 活跃模式更新 */
                if (Work_Mode == MODE_ACTIVE)
                {
                    WorkMode_ActiveUpdate();
                }
                break;
                
            case STATE_UPDATE_OLED:
                /* 更新OLED显示（不是每次都更新） */
                Update_OLED();
                break;
                
            case STATE_READ_DHT11:
                /* DHT11不用每次都读，每10次读一次 */
                dht11_read_count++;
                if (dht11_read_count &gt;= 10)
                {
                    dht11_read_count = 0;
                    DHT11_Read_Data(&amp;temperature, &amp;humidity);
                }
                break;
                
            case STATE_READ_SENSOR:
                /* 读取光敏等传感器 */
                if (Work_Mode != MODE_POWER_SAVE)
                {
                    light_ao = Get_ADC_Value(ADC_Channel_5);
                    light_do = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
                }
                break;
                
            case STATE_BREATH_LED:
                /* 呼吸灯（只有活跃模式） */
                if (Work_Mode == MODE_ACTIVE)
                {
                    Breath_Led_Update();
                }
                break;
                
            case STATE_OTHER:
                /* 执行其他动作 */
                PetAction_Perform();
                break;
                
            default:
                break;
        }
        
        /* ========== 3. 状态切换（轮询下一个功能） ========== */
        state++;
        if (state &gt;= STATE_MAX)
        {
            state = STATE_CHECK_WORKMODE;
        }
        
        /* ========== 4. 根据工作模式调整延时 ========== */
        if (Work_Mode == MODE_POWER_SAVE)
        {
            delay_ms(50);  /* 省电模式用更长的延时 */
        }
        else
        {
            delay_ms(2);   /* 活跃模式用更短的延时，保证摇尾巴流畅 */
        }
    }
}

/*
 * 另一种更简单的方案：
 * 把耗时的DHT11等操作改成"每隔N次才执行一次"
 */
void Main_Loop_Simple(void)
{
    static uint16_t loop_count = 0;
    
    while(1)
    {
        loop_count++;
        
        /* ========== 关键功能：每次都执行 ========== */
        inc_loop_count();
        
        /* 摇尾巴 */
        if (WeiBa_Bit)
        {
            WeiBa_Count++;
            if (WeiBa_Count &gt;= WEIBA_STOP_COUNT)
            {
                WeiBa_Bit = 0;
                WeiBa_Count = 0;
            }
        }
        Action_SwingTail_NonBlocking();
        
        /* 工作模式检查 */
        WorkMode_AutoCheck();
        
        /* ========== 耗时功能：每隔几次才执行 ========== */
        
        /* 每2次循环更新一次OLED */
        if (loop_count % 2 == 0)
        {
            Update_OLED();
        }
        
        /* 每10次循环才读一次DHT11 */
        if (loop_count % 10 == 0)
        {
            DHT11_Read_Data(&amp;temperature, &amp;humidity);
        }
        
        /* 每5次循环才读一次传感器 */
        if (loop_count % 5 == 0 &amp;&amp; Work_Mode != MODE_POWER_SAVE)
        {
            light_ao = Get_ADC_Value(ADC_Channel_5);
            light_do = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
        }
        
        /* 每3次循环才更新呼吸灯 */
        if (loop_count % 3 == 0 &amp;&amp; Work_Mode == MODE_ACTIVE)
        {
            Breath_Led_Update();
        }
        
        /* 活跃模式更新 */
        if (Work_Mode == MODE_ACTIVE)
        {
            WorkMode_ActiveUpdate();
        }
        
        /* 其他动作 */
        PetAction_Perform();
        
        /* 更短的延时 */
        delay_ms(2);
    }
}
