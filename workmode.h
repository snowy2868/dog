#ifndef __WORKMODE_H_
#define __WORKMODE_H_

#include "stm32f10x.h"

/*==============================
  工作模式管理函数声明
==============================*/

/* 初始化工作模式 */
void WorkMode_Init(void);

/* 切换工作模式 */
void WorkMode_Switch(uint8_t new_mode);

/* 工作模式自动检查（光线、电量） */
void WorkMode_AutoCheck(void);

/* 活跃模式：5分钟无指令自动动作 */
void WorkMode_ActiveUpdate(void);

/* 省电模式唤醒 */
void WorkMode_WakeUp(void);

/* 记录收到指令时间 */
void WorkMode_RecordCommand(void);

/* 获取当前工作模式字符串 */
char* WorkMode_GetName(uint8_t mode);

#endif

