#include "adc.h"
#include "delay.h"

/*
 * 功能：ADC1初始化函数
 * 说明：
 *   - 开启GPIOA和ADC1的时钟
 *   - 配置PA5为模拟输入(光敏传感器AO)
 *   - 配置PA4为模拟输入(电池电压检测)
 *   - 配置PA7为上拉输入(光敏传感器DO)
 *   - 配置ADC1为独立模式、单通道、软件触发
 *   - 执行ADC校准
 */
void ADC1_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 开启GPIOA和ADC1时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    /* ADC时钟配置为PCLK2/6 = 72MHz/6 = 12MHz */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    
    /* 配置PA5为模拟输入模式 - 光敏传感器AO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 配置PA4为模拟输入模式 - 电池电压检测 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 配置PA7为上拉输入模式 - 光敏传感器DO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 复位ADC1 */
    ADC_DeInit(ADC1);
    
    /* ADC1配置 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;              /* 独立模式 */
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                   /* 单通道模式 */
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;             /* 单次转换模式 */
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; /* 软件触发 */
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;          /* 数据右对齐 */
    ADC_InitStructure.ADC_NbrOfChannel = 1;                         /* 转换通道数量 */
    ADC_Init(ADC1, &ADC_InitStructure);
    
    /* 使能ADC1 */
    ADC_Cmd(ADC1, ENABLE);
    
    /* ADC复位校准 */
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));  /* 等待复位校准完成 */
    /* ADC开始校准 */
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));       /* 等待校准完成 */
}

/*
 * 功能：获取指定ADC通道的采样值
 * 参数：ch - ADC通道号，如ADC_Channel_4表示通道4
 * 返回：12位ADC转换结果(0-4095)
 * 说明：
 *   - 配置指定通道为规则通道
 *   - 软件启动转换
 *   - 等待转换完成(EOC标志)
 *   - 返回转换结果
 */
u16 Get_ADC_Value(u8 ch)
{
    /* 配置ADC规则通道，采样时间为239.5个周期 */
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);
    /* 软件启动ADC转换 */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    /* 等待转换完成标志EOC置1 */
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    /* 返回ADC转换结果 */
    return ADC_GetConversionValue(ADC1);
}

/*
 * 功能：获取电池电压值
 * 返回：电池实际电压(单位：伏特)
 * 说明：
 *   - 使用PA4(ADC通道4)进行采样
 *   - 连续采样10次，每次间隔1ms，取平均值滤波
 *   - 分压电路：上拉3个2k，下拉1个2k，分压比为1/4，所以计算时乘以4
 *   - 参考电压为3.3V，12位ADC分辨率为4096
 *   - 计算公式：电压 = ADC值 × 3.3V × 4 / 4095
 */
float Get_Battery_Voltage(void)
{
    u16 adc_val = 0;  /* ADC采样累加值 */
    
    /* 连续采样10次，提高精度 */
    for(u8 i=0; i<10; i++)
    {
        adc_val += Get_ADC_Value(ADC_Channel_4);  /* 采样通道4(PA4) */
        delay_ms(1);  /* 每次采样间隔1ms */
    }
    adc_val /= 10;  /* 取平均值 */
    
    /* 计算实际电压：ADC值 × 3.3V × 4(分压比) / 4095(12位分辨率) */
    return (float)adc_val * 3.3 * 4 / 4095;
}

/*
 * 功能：获取电池电量百分比
 * 返回：电量百分比(0-100)
 * 说明：
 *   - 锂电池电压范围：3.0V ~ 4.2V
 *   - 3.0V对应0%电量
 *   - 4.2V对应100%电量
 *   - 中间采用线性映射
 *   - 超过4.2V显示100%，低于3.0V显示0%
 */
u8 Get_Battery_Percent(void)
{
    float voltage = Get_Battery_Voltage();  /* 获取当前电池电压 */
    
    if(voltage >= 4.2) return 100;  /* 电压≥4.2V，显示100% */
    if(voltage <= 3.0) return 0;    /* 电压≤3.0V，显示0% */
    
    /* 线性计算百分比：(当前电压-最低电压) × 100 / (最高电压-最低电压) */
    return (u8)((voltage - 3.0) * 100 / (4.2 - 3.0));
}
