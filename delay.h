#ifndef __DELAY_H
#define __DELAY_H
#include "stm32f10x.h"

void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);
void inc_loop_count(void);  /* 更新系统运行时间(在主循环调用) */
u32 millis(void);  /* 获取系统运行时间(ms) */

#endif
