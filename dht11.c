#include "dht11.h"
#include "delay.h"

void DHT11_Rst(void)	   
{                 
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
    
    GPIO_ResetBits(DHT11_PORT, DHT11_PIN);
    delay_ms(20);
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);
    delay_us(30);
}

u8 DHT11_Check(void) 	   
{   
    u8 retry=0;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
    
    while (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && retry < 100)
    {
        retry++;
        delay_us(1);
    };
    if(retry>=100)return 1;
    else retry=0;
    while (!GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) && retry < 100)
    {
        retry++;
        delay_us(1);
    };
    if(retry>=100)return 1;	    
    return 0;
}

u8 DHT11_Read_Bit(void) 			 
{
    u8 retry=0;
    while(GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)&&retry<100)
    {
        retry++;
        delay_us(1);
    }
    retry=0;
    while(!GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)&&retry<100)
    {
        retry++;
        delay_us(1);
    }
    delay_us(40);
    if(GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN))return 1;
    else return 0;		   
}

u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
    for (i=0;i<8;i++) 
    {
        dat<<=1; 
        dat|=DHT11_Read_Bit();
    }						    
    return dat;
}

u8 DHT11_Read_Data(float *temperature, float *humidity)    
{        
    u8 buf[5];
    u8 i;
    DHT11_Rst();
    if(DHT11_Check()==0)
    {
        for(i=0;i<5;i++)
        {
            buf[i]=DHT11_Read_Byte();
        }
        if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
        {
            *humidity=buf[0];
            *temperature=buf[2];
            if(buf[3]>0)
            {
                *temperature += 0.1;
            }
        }
    }else return 1;
    return 0;	    
}

void DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
    
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);
    DHT11_Rst();
    DHT11_Check();
}
