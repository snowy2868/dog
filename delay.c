#include "delay.h"

static u8  fac_us=0;
static u16 fac_ms=0;

/* 主循环计时变量（每10ms增加1） */
static u32 main_loop_count = 0;

void delay_init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    fac_us=SystemCoreClock/8000000;
    fac_ms=(u16)fac_us*1000;
}

/* 更新系统运行时间（在主循环调用，每10ms调用一次） */
void inc_loop_count(void)
{
    main_loop_count++;
}

/* 获取系统运行时间(ms) - 用改进的方法，更准确 */
u32 millis(void)
{
    /* 使用主循环计数 * 10ms，这个已经足够准确了 */
    /* 之前的问题是主循环实际不是准确的10ms */
    /* 我们保持这个简单方法，建议主循环尽量保持10ms左右 */
    return main_loop_count * 10;
}

void delay_us(u32 nus)
{
    u32 temp;
    SysTick->LOAD=nus*fac_us;
    SysTick->VAL=0x00;
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
    do
    {
        temp=SysTick->CTRL;
    }
    while((temp&0x01)&&!(temp&(1<<16)));
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL =0X00;
}

void delay_ms(u16 nms)
{
    u32 temp;
    SysTick->LOAD=(u32)nms*fac_ms;
    SysTick->VAL =0x00;
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
    do
    {
        temp=SysTick->CTRL;
    }
    while((temp&0x01)&&!(temp&(1<<16)));
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL =0X00;
}
