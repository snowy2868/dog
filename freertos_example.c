
/*
 * FreeRTOS 多任务示例
 * 完美解决摇尾巴停顿 + 新功能易扩展的问题
 */

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
// ... 其他头文件

/* ==============================
   FreeRTOS 任务优先级（数字越大优先级越高）
   ============================== */
#define PRIORITY_WEIBA         5   /* 摇尾巴：最高优先级 */
#define PRIORITY_WORKMODE      4   /* 工作模式检查：高优先级 */
#define PRIORITY_OLED          3   /* OLED更新：中优先级 */
#define PRIORITY_SENSOR        2   /* 传感器读取：低优先级 */
#define PRIORITY_OTHER         1   /* 其他功能：最低优先级 */

/* ==============================
   FreeRTOS 任务栈大小
   ============================== */
#define STACK_SIZE_WEIBA       128
#define STACK_SIZE_WORKMODE    128
#define STACK_SIZE_OLED        256
#define STACK_SIZE_SENSOR      128
#define STACK_SIZE_OTHER       256

/* ==============================
   任务1：摇尾巴（最高优先级！）
   ============================== */
void Task_WeiBa(void *pvParameters)
{
    while(1)
    {
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
        
        /* 执行摇尾巴 */
        Action_SwingTail_NonBlocking();
        
        /* 延时很短，保证流畅 */
        vTaskDelay(2 / portTICK_PERIOD_MS);  /* 2ms */
    }
}

/* ==============================
   任务2：工作模式检查
   ============================== */
void Task_WorkMode(void *pvParameters)
{
    while(1)
    {
        /* 更新循环计数 */
        inc_loop_count();
        
        /* 工作模式自动检查 */
        WorkMode_AutoCheck();
        
        /* 活跃模式更新 */
        if (Work_Mode == MODE_ACTIVE)
        {
            WorkMode_ActiveUpdate();
        }
        
        /* 延时10ms */
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/* ==============================
   任务3：OLED更新
   ============================== */
void Task_OLED(void *pvParameters)
{
    while(1)
    {
        /* 更新OLED显示 */
        Update_OLED();
        
        /* 延时50ms（OLED不需要太快） */
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

/* ==============================
   任务4：传感器读取
   ============================== */
void Task_Sensor(void *pvParameters)
{
    static uint8_t dht11_count = 0;
    
    while(1)
    {
        /* 读取光敏传感器（每次都读） */
        if (Work_Mode != MODE_POWER_SAVE)
        {
            light_ao = Get_ADC_Value(ADC_Channel_5);
            light_do = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
        }
        
        /* 读取DHT11（每10次读一次，因为DHT11慢） */
        dht11_count++;
        if (dht11_count &gt;= 10)
        {
            dht11_count = 0;
            DHT11_Read_Data(&amp;temperature, &amp;humidity);
        }
        
        /* 延时20ms */
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

/* ==============================
   任务5：其他功能（呼吸灯、动作等）
   ============================== */
void Task_Other(void *pvParameters)
{
    while(1)
    {
        /* 呼吸灯更新（只有活跃模式） */
        if (Work_Mode == MODE_ACTIVE)
        {
            Breath_Led_Update();
        }
        
        /* 执行其他动作 */
        PetAction_Perform();
        
        /* 省电模式下延时更长 */
        if (Work_Mode == MODE_POWER_SAVE)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        else
        {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}

/* ==============================
   FreeRTOS 初始化和任务创建
   ============================== */
void FreeRTOS_Init(void)
{
    /* 创建摇尾巴任务 - 最高优先级 */
    xTaskCreate(
        Task_WeiBa,          /* 任务函数 */
        "WeiBa",             /* 任务名称 */
        STACK_SIZE_WEIBA,    /* 栈大小 */
        NULL,                /* 参数 */
        PRIORITY_WEIBA,      /* 优先级 */
        NULL                 /* 任务句柄 */
    );
    
    /* 创建工作模式任务 */
    xTaskCreate(
        Task_WorkMode,
        "WorkMode",
        STACK_SIZE_WORKMODE,
        NULL,
        PRIORITY_WORKMODE,
        NULL
    );
    
    /* 创建OLED任务 */
    xTaskCreate(
        Task_OLED,
        "OLED",
        STACK_SIZE_OLED,
        NULL,
        PRIORITY_OLED,
        NULL
    );
    
    /* 创建传感器任务 */
    xTaskCreate(
        Task_Sensor,
        "Sensor",
        STACK_SIZE_SENSOR,
        NULL,
        PRIORITY_SENSOR,
        NULL
    );
    
    /* 创建其他功能任务 */
    xTaskCreate(
        Task_Other,
        "Other",
        STACK_SIZE_OTHER,
        NULL,
        PRIORITY_OTHER,
        NULL
    );
    
    /* 启动调度器 */
    vTaskStartScheduler();
}

/* ==============================
   main函数（FreeRTOS版本）
   ============================== */
int main(void)
{
    /* 硬件初始化 */
    System_Init();  /* 你原来的初始化函数 */
    
    /* FreeRTOS初始化和启动 */
    FreeRTOS_Init();
    
    /* 永远不会到这里 */
    while(1)
    {
    }
}

/*
 * FreeRTOS优势总结：
 * ==============================
 * 1. 摇尾巴任务优先级最高，永远不会卡顿
 * 2. 每个任务独立运行，互不干扰
 * 3. 加新功能超级简单：只需要新建一个任务，xTaskCreate就行
 * 4. 不用担心新加功能影响摇尾巴
 * 5. 代码结构清晰，易维护
 * 
 * 怎么把现有代码改成FreeRTOS：
 * ==============================
 * 1. 把你原来的main.c里的初始化保留
 * 2. 把主循环里的功能拆分成多个任务
 * 3. 每个任务里用 vTaskDelay() 代替 delay_ms()
 * 4. 最后调用 vTaskStartScheduler() 启动调度器
 */
