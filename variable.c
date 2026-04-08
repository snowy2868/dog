#include "stm32f10x.h"
#include "variable.h"

/*==============================
  动作相关变量初始化
==============================*/
uint16_t PAnumbers = Chongfunumber;  /* 动作剩余次数初始化为充能次数 */
uint16_t TiaoTurn = 0;               /* 跳上动作状态初始化为0 */
uint16_t TiaoTurn2 = 0;              /* 跳下动作状态初始化为0 */
uint16_t Action_Mode = 2;            /* 初始动作模式：2=站立 */
uint16_t SpeedDelay = 200;           /* 动作速度延迟：200ms */
uint16_t SwingDelay = 6;             /* 摇尾巴延迟：6ms */
uint16_t Face_Mode = 5;              /* 初始表情模式：5=眼睛 */
uint8_t WeiBa_Bit = 0;               /* 尾巴初始状态：0=停止 */
uint8_t WeiBa_Value = 90;            /* 尾巴初始角度：90度(中位) */
int8_t WeiBa_Dir = 1;                /* 尾巴初始方向：1=正向 */
uint16_t Sustainedmove = 0;          /* 持续运动标志初始化为0 */

/*==============================
  呼吸灯相关变量初始化
==============================*/
uint16_t Time;               /* 时间变量 */
uint16_t HuXi;               /* 呼吸变量 */
uint16_t PanDuan = 1;        /* 判断变量初始化为1 */
uint16_t Wait = 0;           /* 等待变量初始化为0 */
uint16_t AllLed = 1;         /* 所有LED使能 */
uint16_t BreatheLed = 0;     /* 呼吸LED初始关闭 */

/*==============================
  电量检测相关变量初始化
==============================*/
float Battery_Value = 0;     /* 电池电压初始化为0 */
uint16_t temp;               /* 临时变量1 */
uint16_t temp1 = 0;          /* 临时变量2初始化为0 */
uint16_t CurBattery;         /* 当前电量百分比 */
uint8_t Battery_num = 0;     /* 电量数值初始化为0 */
uint8_t Battery_Bit = 1;     /* 电量显示使能 */

/*==============================
  蓝牙通信相关变量初始化
==============================*/
uint8_t Bluetooth_Received_Data = 0;  /* 蓝牙接收数据初始化为0 */

/*==============================
  OLED显示模式变量初始化
==============================*/
/* OLED显示模式初始化为0：表情模式 */
uint8_t OLED_Display_Mode = 0;

/*==============================
  天气数据相关变量初始化
==============================*/
uint8_t Weather_Mode = 0;       /* 天气显示模式：0=不显示 */
int8_t Weather_Temp = 0;         /* 室外温度初始化为0℃ */
uint8_t Weather_Humi = 0;        /* 室外湿度初始化为0% */
uint8_t Weather_Rain = 0;        /* 初始：不下雨 */
uint8_t Weather_Umbrella = 0;    /* 初始：不需要伞 */
uint8_t Weather_Sunscreen = 0;   /* 初始：不需要防晒 */
uint8_t Weather_Icon = 0;        /* 天气图标初始化为0 */
uint8_t Weather_Rx_State = 0;    /* 天气数据接收状态：0=等待开始 */
uint8_t Weather_Rx_Buf[16] = {0};/* 天气数据接收缓冲区 */
uint8_t Weather_Rx_Cnt = 0;      /* 天气数据接收计数 */
uint32_t Weather_Start_Time = 0; /* 天气模式开始时间初始化为0 */

/*==============================
  调试转发相关变量初始化
==============================*/
uint8_t Debug_Rx_State = 0;      /* 调试转发接收状态：0=等待开始 */
uint8_t Debug_Rx_Buf[16] = {0}; /* 调试转发接收缓冲区 */
uint8_t Debug_Rx_Cnt = 0;        /* 调试转发接收计数 */

/*==============================
  工作模式相关变量初始化
==============================*/
uint8_t Work_Mode = MODE_ACTIVE;  /* 初始工作模式：活跃模式 */
uint32_t Last_Command_Time = 0;    /* 最后指令时间初始化为0 */
uint8_t Auto_Face_Action = 1;      /* 自动表情/动作使能：开启 */
uint32_t Auto_Action_Timer = 0;    /* 自动动作计时器初始化为0 */

/*==============================
  摇尾巴计时器初始化
==============================*/
uint32_t WeiBa_Start_Time = 0;     /* 摇尾巴开始时间初始化为0 */
uint16_t WeiBa_Count = 0;           /* 摇尾巴计数器初始化为0 */
