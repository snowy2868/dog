#include "usart.h"
#include "variable.h"
#include "workmode.h"
#include "delay.h"

/*==============================
  串口接收缓冲相关变量
==============================*/
u8 rx_buffer[64];  /* 串口接收缓冲区，最多64字节 */
u8 rx_cnt = 0;     /* 接收缓冲区计数 */
u8 rx_flag = 0;    /* 接收完成标志 */

/*
 * 功能：USART1初始化函数（语音模块）
 * 参数：bound - 波特率(本代码固定为9600)
 * 说明：
 *   - 语音模块连接：PA9=TX, PA10=RX
 *   - 开启GPIOA和USART1时钟
 *   - 配置PA9为复用推挽输出(TX)
 *   - 配置PA10为上拉输入(RX)
 *   - 配置USART1参数：9600波特率，8数据位，1停止位，无校验
 *   - 配置NVIC中断优先级，使能接收中断
 *   - 波特率固定为9600(语音模块默认波特率)
 */
void USART1_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 开启GPIOA和USART1的时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    /* 配置PA9为复用推挽输出 - USART1_TX */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 配置PA10为上拉输入 - USART1_RX */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* USART1配置 */
    USART_InitStructure.USART_BaudRate = 9600;              /* 波特率固定为9600 */
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  /* 8位数据位 */
    USART_InitStructure.USART_StopBits = USART_StopBits_1;       /* 1位停止位 */
    USART_InitStructure.USART_Parity = USART_Parity_No;          /* 无校验位 */
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; /* 无硬件流控 */
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; /* 收发模式 */
    USART_Init(USART1, &USART_InitStructure);
    
    /* NVIC中断配置 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  /* 中断优先级分组2 */
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;              /* USART1中断通道 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;       /* 抢占优先级1（比蓝牙高） */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;              /* 子优先级1 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 /* 使能中断 */
    NVIC_Init(&NVIC_InitStructure);
    
    /* 使能USART1接收中断 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* 使能USART1 */
    USART_Cmd(USART1, ENABLE);
}

/*
 * 功能：USART3初始化函数（蓝牙模块）
 * 参数：bound - 波特率(本代码固定为9600)
 * 说明：
 *   - 开启GPIOB和USART3时钟
 *   - 配置PB10为复用推挽输出(TX)
 *   - 配置PB11为上拉输入(RX)
 *   - 配置USART3参数：9600波特率，8数据位，1停止位，无校验
 *   - 配置NVIC中断优先级，使能接收中断
 *   - 波特率固定为9600(蓝牙模块默认波特率)
 */
void USART3_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 开启GPIOB和USART3的时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    
    /* 配置PB10为复用推挽输出 - USART3_TX */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* 配置PB11为上拉输入 - USART3_RX */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* USART3配置 */
    USART_InitStructure.USART_BaudRate = 9600;              /* 波特率固定为9600 */
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  /* 8位数据位 */
    USART_InitStructure.USART_StopBits = USART_StopBits_1;       /* 1位停止位 */
    USART_InitStructure.USART_Parity = USART_Parity_No;          /* 无校验位 */
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; /* 无硬件流控 */
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; /* 收发模式 */
    USART_Init(USART3, &USART_InitStructure);
    
    /* NVIC中断配置 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  /* 中断优先级分组2 */
    
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;              /* USART3中断通道 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;       /* 抢占优先级2 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;              /* 子优先级1 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 /* 使能中断 */
    NVIC_Init(&NVIC_InitStructure);
    
    /* 使能USART3接收中断 */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    /* 使能USART3 */
    USART_Cmd(USART3, ENABLE);
}

/*
 * 功能：USART3发送一个字节
 * 参数：ch - 要发送的字节数据
 * 说明：等待发送完成标志TC置位后才返回
 */
void USART3_SendByte(u8 ch)
{
    USART_SendData(USART3, ch);                 /* 发送数据 */
    while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);  /* 等待发送完成 */
}

/*
 * 功能：USART3发送字符串
 * 参数：str - 要发送的字符串指针
 * 说明：循环调用USART3_SendByte发送每个字符，直到遇到字符串结束符'\0'
 */
void USART3_SendString(u8 *str)
{
    while(*str)  /* 当字符串未结束时 */
    {
        USART3_SendByte(*str++);  /* 发送当前字符，指针后移 */
    }
}

/*
 * 功能：USART1发送一个字节（语音模块）
 * 参数：ch - 要发送的字节数据
 * 说明：等待发送完成标志TC置位后才返回
 */
void USART1_SendByte(u8 ch)
{
    USART_SendData(USART1, ch);                 /* 发送数据 */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  /* 等待发送完成 */
}

/*
 * 功能：USART1发送字符串（语音模块）
 * 参数：str - 要发送的字符串指针
 * 说明：循环调用USART1_SendByte发送每个字符，直到遇到字符串结束符'\0'
 */
void USART1_SendString(u8 *str)
{
    while(*str)  /* 当字符串未结束时 */
    {
        USART1_SendByte(*str++);  /* 发送当前字符，指针后移 */
    }
}

/*
 * 功能：USART1中断服务函数（语音指令接收）
 * 说明：
 *   - 语音模块通过PA9(TX)/PA10(RX)连接
 *   - 接收语音识别模块发送的指令
 *   - 指令协议与蓝牙模块相同
 *   - 语音模式下Sustainedmove=0（不持续运动）
 *   - 立即切换表情并执行对应动作
 */
void USART1_IRQHandler(void)
{
    /* 检查是否是接收中断 */
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        u8 ch = USART_ReceiveData(USART1);  /* 读取接收到的数据 */
        
        Sustainedmove = 0;  /* 语音模式：不持续运动 */
        
        /* ========== 天气数据接收处理 ========== */
        if(Weather_Rx_State == 0)  /* 等待天气数据开始字节 */
        {
            if(ch == 0xAA)  /* 天气数据开始标志：0xAA */
            {
                Weather_Rx_State = 1;  /* 切换到接收状态 */
                Weather_Rx_Cnt = 0;     /* 计数清零 */
            }
        }
        else if(Weather_Rx_State == 1)  /* 正在接收天气数据 */
        {
            Weather_Rx_Buf[Weather_Rx_Cnt++] = ch;  /* 存入缓冲区 */
            
            if(Weather_Rx_Cnt >= 7)  /* 接收完7个字节数据 */
            {
                /* 检查结束标志：0x55 */
                if(Weather_Rx_Buf[6] == 0x55)
                {
                    /* 解析天气数据 */
                    Weather_Temp = (int8_t)Weather_Rx_Buf[0];      /* 温度(℃) */
                    Weather_Humi = Weather_Rx_Buf[1];               /* 湿度(%) */
                    Weather_Rain = Weather_Rx_Buf[2];                /* 是否下雨 */
                    Weather_Umbrella = Weather_Rx_Buf[3];            /* 是否带伞 */
                    Weather_Sunscreen = Weather_Rx_Buf[4];           /* 是否防晒 */
                    Weather_Mode = 1;  /* 开启天气显示 */
                }
                Weather_Rx_State = 0;  /* 重置接收状态 */
            }
        }
        
        /* ========== 语音指令处理 ========== */
        /* 如果不是天气数据接收状态，处理普通语音指令 */
        if(Weather_Rx_State == 0)
        {
            /* 特殊处理0x71（切换天气模式），不退出天气模式 */
            if(ch == 0x71)
            {
                Weather_Mode ^= 1;  /* 切换天气显示模式：0→1或1→0 */
                if(Weather_Mode == 1)
                {
                    Weather_Start_Time = millis();  /* 如果开启天气模式，记录开始时间 */
                }
                /* 收到语音模块的0x71指令，转发给蓝牙模块 */
                USART3_SendByte(0x71);
            }
            else
            {
                /* 其他按键时，退出天气模式 */
                Weather_Mode = 0;
                
                /* 根据接收到的指令执行相应动作 */
                if(ch == 0x29)
                {
                    Face_Mode = 0;    /* 立即切换表情：睡觉 */
                    Action_Mode = 0;  /* 放松趴下 */
                }
                else if(ch == 0x30)
                {
                    Face_Mode = 1;    /* 立即切换表情：瞪大眼 */
                    Action_Mode = 1;  /* 坐下 */
                }
                else if(ch == 0x31)
                {
                    Face_Mode = 5;    /* 立即切换表情：眼睛 */
                    Action_Mode = 2;  /* 站立 */
                }
                else if(ch == 0x32)
                {
                    Face_Mode = 1;    /* 立即切换表情：瞪大眼 */
                    Action_Mode = 3;  /* 趴下 */
                }
                else if(ch == 0x33)
                {
                    Face_Mode = 2;    /* 立即切换表情：快乐 */
                    Action_Mode = 4;  /* 前进 */
                }
                else if(ch == 0x34)
                {
                    Face_Mode = 2;    /* 立即切换表情：快乐 */
                    Action_Mode = 5;  /* 后退 */
                }
                else if(ch == 0x35)
                {
                    Face_Mode = 2;    /* 立即切换表情：快乐 */
                    Action_Mode = 6;  /* 左转 */
                }
                else if(ch == 0x36)
                {
                    Face_Mode = 2;    /* 立即切换表情：快乐 */
                    Action_Mode = 7;  /* 右转 */
                }
                else if(ch == 0x37)
                {
                    Face_Mode = 4;    /* 立即切换表情：非常快乐 */
                    Action_Mode = 8;  /* 摇摆 */
                }
                else if(ch == 0x38)
                {
                    /* 调节动作速度：循环递减，到120后重置为200 */
                    if(SpeedDelay == 120) {Face_Mode = 3; SpeedDelay = 200;}
                    else if(SpeedDelay > 100) SpeedDelay -= 20;
                    else {Face_Mode = 2; SpeedDelay = 200;}
                }
                else if(ch == 0x39)
                {
                    /* 调节摇尾巴速度：循环递减，到4后重置为9 */
                    if(SwingDelay == 4) {Face_Mode = 3; SwingDelay = 9;}
                    else if(SwingDelay > 3) SwingDelay--;
                    else {Face_Mode = 4; SwingDelay = 9;}
                }
                else if(ch == 0x40)
                {
                    WeiBa_Bit ^= 1;  /* 切换尾巴使能位：0→1或1→0 */
                    Face_Mode = 1;    /* 切换表情：瞪大眼 */
                }
                else if(ch == 0x41)
                {
                    Face_Mode = 2;    /* 立即切换表情：快乐 */
                    Action_Mode = 10; /* 跳上 */
                }
                else if(ch == 0x42)
                {
                    Face_Mode = 2;    /* 立即切换表情：快乐 */
                    Action_Mode = 11; /* 跳下 */
                }
                else if(ch == 0x43)
                {
                    Face_Mode = 6;    /* 立即切换表情：打招呼 */
                    Action_Mode = 13; /* 打招呼 */
                }
                else if(ch == 0x44)
                {
                    AllLed = 1;  /* 开启所有灯光 */
                }
                else if(ch == 0x45)
                {
                    AllLed = 0;  /* 关闭所有灯光 */
                }
                else if(ch == 0x46)
                {
                    BreatheLed = 1;  /* 开启呼吸灯 */
                }
                else if(ch == 0x47)
                {
                    BreatheLed = 0;  /* 关闭呼吸灯 */
                }
                else if(ch == 0x48)
                {
                    Face_Mode = 6;    /* 立即切换表情：打招呼 */
                    Action_Mode = 14; /* 伸懒腰 */
                }
                else if(ch == 0x49)
                {
                    Face_Mode = 6;    /* 立即切换表情：打招呼 */
                    Action_Mode = 15; /* 拉伸腿 */
                }
                else if(ch == 0x50)
                {
                    Battery_Bit ^= 1;  /* 切换电量显示 */
                }
                else if(ch == 0x70)
                {
                    OLED_Display_Mode ^= 1;  /* 切换OLED显示模式：0→1或1→0 */
                }
                else if(ch == 0x60)
                {
                    WorkMode_Switch(MODE_ACTIVE);  /* 切换到活跃模式 */
                }
                else if(ch == 0x61)
                {
                    WorkMode_Switch(MODE_QUIET);   /* 切换到安静模式 */
                }
                else if(ch == 0x62)
                {
                    WorkMode_Switch(MODE_POWER_SAVE);  /* 切换到省电模式 */
                }
            }
        }
        
        /* ========== 记录指令时间 ========== */
        WorkMode_RecordCommand();
        
        /* 清除USART1接收中断标志 */
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/*
 * 功能：USART3中断服务函数
 * 说明：
 *   - 当接收到数据时触发此中断
 *   - 读取接收到的字节并存入Bluetooth_Received_Data
 *   - 优先处理天气数据接收（0xAA开头）
 *   - 只有在非天气接收状态时才处理动作指令
 *   - 指令说明：
 *     0x29 - 放松趴下
 *     0x30 - 坐下
 *     0x31 - 站立
 *     0x32 - 趴下
 *     0x33 - 前进
 *     0x34 - 后退
 *     0x35 - 左转
 *     0x36 - 右转
 *     0x37 - 摇摆
 *     0x38 - 调节动作速度
 *     0x39 - 调节摇尾巴速度
 *     0x40 - 切换尾巴开关
 *     0x41 - 跳上
 *     0x42 - 跳下
 *     0x43 - 打招呼
 *     0x48 - 伸懒腰
 *     0x49 - 拉伸腿
 *     0x70 - 切换OLED显示模式(表情/数据)
 *     0x71 - 切换天气显示模式
 */
void USART3_IRQHandler(void)
{
    /* 检查是否是接收中断 */
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        u8 ch = USART_ReceiveData(USART3);  /* 读取接收到的数据 */
        Bluetooth_Received_Data = ch;        /* 保存到全局变量 */
        
        /* ========== 调试转发数据接收处理（最高优先级）========== */
        if(Debug_Rx_State == 0)  /* 等待调试转发数据开始字节 */
        {
            if(ch == 0xAA)  /* 调试转发数据开始标志：0xAA */
            {
                Debug_Rx_State = 1;  /* 切换到接收状态 */
                Debug_Rx_Cnt = 0;     /* 计数清零 */
                Debug_Rx_Buf[Debug_Rx_Cnt++] = ch;  /* 存入第一个字节 */
            }
            else
            {
                /* ========== 天气数据接收处理（次优先级）========== */
                if(Weather_Rx_State == 0)  /* 等待天气数据开始字节 */
                {
                    if(ch == 0xAA)  /* 天气数据开始标志：0xAA */
                    {
                        Weather_Rx_State = 1;  /* 切换到接收状态 */
                        Weather_Rx_Cnt = 0;     /* 计数清零 */
                    }
                    else
                    {
                        /* ========== 不是天气数据，处理动作指令 ========== */
                        Sustainedmove = 1;  /* 设置持续运动标志(表示有按键按下) */
                        
                        /* 回显：把接收到的数据发回去 */
                        USART_SendData(USART3, ch);
                        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
                        
                        /* 特殊处理0x71（切换天气模式），不退出天气模式 */
                        if(ch == 0x71)
                        {
                            Weather_Mode ^= 1;  /* 切换天气显示模式：0→1或1→0 */
                            if(Weather_Mode == 1)
                            {
                                Weather_Start_Time = millis();  /* 如果开启天气模式，记录开始时间 */
                            }
                        }
                        else
                        {
                            /* 其他按键时，退出天气模式 */
                            Weather_Mode = 0;
                            
                            /* 根据接收到的指令执行相应动作 */
                            if(ch == 0x29)
                            {
                                Face_Mode = 0;    /* 立即切换表情：睡觉 */
                                Action_Mode = 0;  /* 放松趴下 */
                            }
                            else if(ch == 0x30)
                            {
                                Face_Mode = 5;    /* 立即切换表情：眼睛 */
                                Action_Mode = 1;  /* 坐下 */
                            }
                            else if(ch == 0x31)
                            {
                                Face_Mode = 2;    /* 立即切换表情：快乐 */
                                Action_Mode = 2;  /* 站立 */
                            }
                            else if(ch == 0x32)
                            {
                                Action_Mode = 3;  /* 趴下 */
                            }
                            else if(ch == 0x33)
                            {
                                Face_Mode = 4;    /* 立即切换表情：非常快乐 */
                                Action_Mode = 4;  /* 前进 */
                            }
                            else if(ch == 0x34)
                            {
                                Face_Mode = 1;    /* 立即切换表情：瞪大眼 */
                                Action_Mode = 5;  /* 后退 */
                            }
                            else if(ch == 0x35)
                            {
                                Action_Mode = 6;  /* 左转 */
                            }
                            else if(ch == 0x36)
                            {
                                Action_Mode = 7;  /* 右转 */
                            }
                            else if(ch == 0x37)
                            {
                                Action_Mode = 8;  /* 摇摆 */
                            }
                            else if(ch == 0x38)
                            {
                                /* 调节动作速度：循环递减，到120后重置为200 */
                                if(SpeedDelay == 120) SpeedDelay = 200;
                                else if(SpeedDelay > 100) SpeedDelay -= 20;
                                else SpeedDelay = 200;
                            }
                            else if(ch == 0x39)
                            {
                                /* 调节摇尾巴速度：循环递减，到4后重置为9 */
                                if(SwingDelay == 4) SwingDelay = 9;
                                else if(SwingDelay > 3) SwingDelay--;
                                else SwingDelay = 9;
                            }
                            else if(ch == 0x40)
                            {
                                Face_Mode = 4;
                                WeiBa_Bit ^= 1;  /* 切换尾巴使能位：0→1或1→0 */
                            }
                            else if(ch == 0x41)
                            {
                                Action_Mode = 10;  /* 跳上 */
                            }
                            else if(ch == 0x42)
                            {
                                Action_Mode = 11;  /* 跳下 */
                            }
                            else if(ch == 0x43)
                            {
                                Face_Mode = 6;    /* 立即切换表情：打招呼 */
                                Action_Mode = 13;  /* 打招呼 */
                            }
                            else if(ch == 0x48)
                            {
                                Face_Mode = 1;    /* 立即切换表情：狂热 */
                                Action_Mode = 14;  /* 伸懒腰 */
                            }
                            else if(ch == 0x49)
                            {
                                Action_Mode = 15;  /* 拉伸腿 */
                            }
                            else if(ch == 0x70)
                            {
                                OLED_Display_Mode ^= 1;  /* 切换OLED显示模式：0→1或1→0 */
                            }
                            else if(ch == 0x60)
                            {
                                WorkMode_Switch(MODE_ACTIVE);  /* 切换到活跃模式 */
                            }
                            else if(ch == 0x61)
                            {
                                WorkMode_Switch(MODE_QUIET);   /* 切换到安静模式 */
                            }
                            else if(ch == 0x62)
                            {
                                WorkMode_Switch(MODE_POWER_SAVE);  /* 切换到省电模式 */
                            }
                        }
                        
                        /* ========== 记录指令时间 ========== */
                        WorkMode_RecordCommand();
                        
                        /* 保存到接收缓冲区(如果缓冲区未满) */
                        if(rx_cnt < 63)
                        {
                            rx_buffer[rx_cnt++] = ch;
                            if(ch == '\n' || ch == '\r')
                            {
                                rx_flag = 1;
                            }
                        }
                    }
                }
                else if(Weather_Rx_State == 1)  /* 正在接收天气数据 */
                {
                    Weather_Rx_Buf[Weather_Rx_Cnt++] = ch;  /* 存入缓冲区 */
                    
                    if(Weather_Rx_Cnt >= 8)  /* 接收完8个字节数据（0xAA之后的8个字节） */
                    {
                        /* 检查结束标志：0x55（在第8个字节，缓冲区索引7） */
                        if(Weather_Rx_Buf[7] == 0x55)
                        {
                            /* 解析天气数据 */
                            Weather_Temp = (int8_t)Weather_Rx_Buf[0];      /* 温度(℃) - 缓冲区索引0 */
                            Weather_Humi = Weather_Rx_Buf[1];               /* 湿度(%) - 缓冲区索引1 */
                            Weather_Rain = Weather_Rx_Buf[2];                /* 是否下雨 - 缓冲区索引2 */
                            Weather_Umbrella = Weather_Rx_Buf[3];            /* 是否带伞 - 缓冲区索引3 */
                            Weather_Sunscreen = Weather_Rx_Buf[4];           /* 是否防晒 - 缓冲区索引4 */
                            Weather_Icon = Weather_Rx_Buf[5];                 /* 天气图标 - 缓冲区索引5 */
                            Weather_Mode = 1;  /* 开启天气显示 */
                            Weather_Start_Time = millis();  /* 记录天气模式开始时间 */
                            
                            /* 向语音模块发送天气数据 */
                            /* 格式：AA 55 01 温度 湿度 isRain isSun icon 00 00 55 AA */
                            USART1_SendByte(0xAA);    /* 帧头1 */
                            USART1_SendByte(0x55);    /* 帧头2 */
                            USART1_SendByte(0x01);    /* 固定0x01 */
                            USART1_SendByte((u8)Weather_Temp);  /* 温度 */
                            USART1_SendByte(Weather_Humi);      /* 湿度 */
                            USART1_SendByte(Weather_Rain);      /* 是否下雨 */
                            USART1_SendByte(Weather_Sunscreen); /* 是否防晒 */
                            USART1_SendByte(Weather_Icon);      /* 天气图标 */
                            USART1_SendByte(0x00);    /* 保留1 */
                            USART1_SendByte(0x00);    /* 保留2 */
                            USART1_SendByte(0x55);    /* 帧尾1 */
                            USART1_SendByte(0xAA);    /* 帧尾2 */
                        }
                        Weather_Rx_State = 0;  /* 重置接收状态 */
                    }
                }
            }
        }
        else if(Debug_Rx_State == 1)  /* 正在接收调试转发数据 */
        {
            Debug_Rx_Buf[Debug_Rx_Cnt++] = ch;  /* 存入缓冲区 */
            
            /* 检查第二个字节，判断是调试转发还是天气数据 */
            if(Debug_Rx_Cnt == 2)
            {
                if(Debug_Rx_Buf[1] == 0x55)
                {
                    /* 第二个字节是0x55，继续接收调试转发数据（12字节） */
                }
                else
                {
                    /* 第二个字节不是0x55，说明是天气数据，切换到天气接收状态 */
                    Weather_Rx_State = 1;
                    Weather_Rx_Cnt = 0;
                    /* 把第一个字节(0xAA)后面的字节复制到天气缓冲区 */
                    Weather_Rx_Buf[Weather_Rx_Cnt++] = Debug_Rx_Buf[1];
                    Debug_Rx_State = 0;  /* 退出调试接收状态 */
                }
            }
            else if(Debug_Rx_Cnt >= 12)  /* 接收完12字节调试数据 */
            {
                /* 检查完整帧：AA 55 01 ... 55 AA */
                if(Debug_Rx_Buf[0] == 0xAA && Debug_Rx_Buf[1] == 0x55 && Debug_Rx_Buf[2] == 0x01 
                   && Debug_Rx_Buf[10] == 0x55 && Debug_Rx_Buf[11] == 0xAA)
                {
                    /* 调试转发：直接转发给语音模块 */
                    for(uint8_t i = 0; i < 12; i++)
                    {
                        USART1_SendByte(Debug_Rx_Buf[i]);
                    }
                }
                Debug_Rx_State = 0;  /* 重置接收状态 */
            }
        }
        
        /* 清除USART3接收中断标志 */
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
