#ifndef __VARIABLE_H_
#define __VARIABLE_H_

/*==============================
  动作相关宏定义和变量
==============================*/
#define Chongfunumber 2       /* 充能次数，前进后退动作重复次数 */
#define SwingRepeatnumber 3   /* 摇摆动作重复次数 */
#define HelloRepeatnumber 4   /* 打招呼动作重复次数 */

extern uint16_t PAnumbers;    /* 当前动作剩余执行次数 */
extern uint16_t TiaoTurn;     /* 跳上动作的状态切换变量 */
extern uint16_t TiaoTurn2;    /* 跳下动作的状态切换变量 */
extern uint16_t Action_Mode;   /* 当前动作模式：0=放松趴下,1=坐下,2=站立,3=趴下,4=前进,5=后退,6=左转,7=右转,8=摇摆,9=摇尾巴,10=跳上,11=跳下,12=站立2,13=打招呼,14=伸懒腰,15=拉伸腿 */
extern uint16_t SpeedDelay;    /* 动作速度延迟(ms)，值越大动作越慢 */
extern uint16_t SwingDelay;    /* 摇尾巴动作延迟(ms) */
extern uint16_t Face_Mode;     /* OLED表情模式：0=睡觉,1=瞪大眼,2=快乐,3=狂热,4=非常快乐,5=眼睛,6=打招呼 */
extern uint8_t WeiBa_Bit;      /* 尾巴使能位：0=停止,1=摇尾巴 */
extern uint8_t WeiBa_Value;    /* 尾巴当前角度值(30-150) */
extern int8_t WeiBa_Dir;       /* 尾巴摆动方向：1=正向,0=反向 */
extern uint16_t Sustainedmove; /* 持续运动标志：1=持续运动(按键按下),0=停止运动(按键松开) */

/*==============================
  呼吸灯相关变量
==============================*/
extern uint16_t Time;      /* 时间变量 */
extern uint16_t HuXi;      /* 呼吸变量 */
extern uint16_t PanDuan;   /* 判断变量 */
extern uint16_t Wait;      /* 等待变量 */
extern uint16_t AllLed;    /* 所有LED控制 */
extern uint16_t BreatheLed;/* 呼吸LED控制 */

/*==============================
  电量检测相关变量
==============================*/
extern float Battery_Value;   /* 电池电压值 */
extern uint16_t temp;         /* 临时变量1 */
extern uint16_t temp1;        /* 临时变量2 */
extern uint16_t CurBattery;   /* 当前电量百分比 */
extern uint8_t Battery_num;   /* 电量数值 */
extern uint8_t Battery_Bit;   /* 电量显示使能位 */

/*==============================
  蓝牙通信相关变量
==============================*/
extern uint8_t Bluetooth_Received_Data;  /* 蓝牙接收到的数据字节 */

/*==============================
  OLED显示模式变量
==============================*/
/* OLED显示模式：0=表情模式(显示128x64表情+电量)，1=数据模式(显示所有传感器数据) */
extern uint8_t OLED_Display_Mode;

/*==============================
  天气数据相关变量
==============================*/
/* 天气数据说明：
   - Weather_Mode: 0=不显示天气, 1=显示天气
   - Weather_Temp: 室外温度(℃)
   - Weather_Humi: 室外湿度(%)
   - Weather_Rain: 0=不下雨, 1=会下雨
   - Weather_Umbrella: 0=不需要伞, 1=需要带伞
   - Weather_Sunscreen: 0=不需要防晒, 1=需要涂防晒
   - Weather_Icon: 天气图标(1=晴,2=云,3=雨)
   - Weather_Start_Time: 天气模式开始时间，用于10秒自动切换
*/
extern uint8_t Weather_Mode;       /* 天气显示模式：0=不显示, 1=显示 */
extern int8_t Weather_Temp;        /* 室外温度(℃) */
extern uint8_t Weather_Humi;       /* 室外湿度(%) */
extern uint8_t Weather_Rain;       /* 是否下雨：0=不下雨, 1=会下雨 */
extern uint8_t Weather_Umbrella;   /* 是否带伞：0=不需要, 1=需要 */
extern uint8_t Weather_Sunscreen;  /* 是否防晒：0=不需要, 1=需要 */
extern uint8_t Weather_Icon;       /* 天气图标：1=晴,2=云,3=雨 */
extern uint8_t Weather_Rx_State;   /* 天气数据接收状态：0=等待开始, 1=接收中 */
extern uint8_t Weather_Rx_Buf[16]; /* 天气数据接收缓冲区 */
extern uint8_t Weather_Rx_Cnt;     /* 天气数据接收计数 */
extern uint32_t Weather_Start_Time;/* 天气模式开始时间(ms)，用于10秒自动切换 */

/*==============================
  调试转发相关变量
==============================*/
/* 调试转发数据格式：AA 55 01 温度 湿度 isRain isSun icon 00 00 55 AA (12字节)
   - Debug_Rx_State: 0=等待开始, 1=接收中
   - Debug_Rx_Buf: 调试数据接收缓冲区
   - Debug_Rx_Cnt: 调试数据接收计数
*/
extern uint8_t Debug_Rx_State;     /* 调试转发接收状态：0=等待开始, 1=接收中 */
extern uint8_t Debug_Rx_Buf[16];   /* 调试转发接收缓冲区 */
extern uint8_t Debug_Rx_Cnt;       /* 调试转发接收计数 */

/*==============================
  工作模式相关变量
==============================*/
/* 工作模式说明：
   - 0=活跃模式：正常运行，5分钟无指令自动变换表情/动作
   - 1=安静模式：正常运行，无额外功能
   - 2=省电模式：关闭灯光、停止传感器、进入STOP模式
*/
#define MODE_ACTIVE        0
#define MODE_QUIET         1
#define MODE_POWER_SAVE    2

extern uint8_t Work_Mode;           /* 当前工作模式：0=活跃,1=安静,2=省电 */
extern uint32_t Last_Command_Time;  /* 最后一次收到指令的时间(ms) */
extern uint8_t Auto_Face_Action;    /* 活跃模式自动表情/动作使能 */
extern uint32_t Auto_Action_Timer;  /* 活跃模式自动动作计时器 */

/* 光敏传感器阈值：低于此值自动切换安静模式 */
#define LIGHT_THRESHOLD_QUIET   1000  /* 光敏传感器值低于1000时自动安静模式 */

/* 电池电量阈值：低于此值自动切换省电模式 */
#define BATTERY_THRESHOLD_POWER_SAVE  20  /* 电量低于20%自动省电模式 */

/* 摇尾巴自动停止计数器（主循环约20ms一次，5秒=250次） */
#define WEIBA_STOP_COUNT  250  /* 摇尾巴5秒后自动停止（20ms*250=5秒） */

extern uint32_t WeiBa_Start_Time;    /* 摇尾巴开始时间（保留） */
extern uint16_t WeiBa_Count;           /* 摇尾巴计数器（新增，更可靠） */

#endif
